#include "databasemodel.h"
#include "schemahelpers.h"
#include <ranges>
#include <QColor>
#include <set>

DatabaseModel::DatabaseModel(std::shared_ptr<Database> db, QObject *parent)
    : QAbstractTableModel(parent), _db(db)
{
    _pagination.pageSize = 1000;

    assert(_db);

    const auto& schema = _db->getSchema();

    for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {
        const QString data_type = schemaFieldTypeToString(field.type);
        QString data_size = "";

        switch (field.type) {
        case WDBReader::Database::Field::Type::FLOAT:
        case WDBReader::Database::Field::Type::INT:
            data_size = QString::number(field.bytes);
            break;
        }

        auto make_header_item = [&](int32_t index = -1) {
            auto label = QString::fromStdString(name);
            if (index >= 0) {
                label += QString("[%1]").arg(index);
            }

            Column header;
            header.type = field.type;
            header.label = label;
            header.tooltip = label + "\n" + data_type;

            if (data_size.size() > 0) {
                header.tooltip += " " + data_size;
            }

            header.format.showSigned = field.annotation.isSigned;

            _columns.push_back(std::move(header));
        };

        if (field.isArray()) {
            for (auto a = 0; a < field.size; a++) {
                make_header_item(a);
            }
        }
        else {
            make_header_item();
        }
    }

    _pagination.currentPageIndex = 0;
    _pagination.endPageIndex = (_db->getRowCount() / _pagination.pageSize);
    _pagination.totalRowCount = _db->getRowCount();
    _pagination.currentRowIndexes = { 0,0 };

    loadPage(0);
}

int DatabaseModel::rowCount(const QModelIndex& /*parent*/) const
{
    return _records.size();
}

int DatabaseModel::columnCount(const QModelIndex& /*parent*/) const
{
    if (_db) {
        return _db->getSchema().elementCount();
    }

    return 0;
}

QVariant DatabaseModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (_db) {
        const auto& rec = _records[row];

        if (rec.data.size() == 0) {
            return QVariant();
        }

        switch (role) {
        case Qt::DisplayRole:
        {
            QString display;

            if (rec.encryptionState == WDBReader::Database::RecordEncryption::ENCRYPTED) {
                display = "???";
            }
            else {
                std::visit([this, col, &display](const auto& val) {
                    using val_t = std::decay_t<decltype(val)>;

                    if constexpr (std::is_same_v<WDBReader::Database::string_data_t, val_t>)
                    {
                        std::string_view view(val.get());
                        display = QString::fromUtf8(view.data(), view.size());
                    }
                    else if constexpr (std::is_floating_point_v<val_t>) {
                        display = QString::number(val, 'g', 14);
                    }
                    else {
                        const auto& format = _columns[col].format;
                        if (format.showBinary) {
                            display = QString("%1").arg(
                                static_cast<uint64_t>(val), sizeof(val) * 8, 2, QChar('0')
                            );
                        }
                        else if (format.showHex) {
                            display = QString("0x%1").arg(
                                static_cast<uint64_t>(val), sizeof(val) * 2, 16, QChar('0')
                            );
                        }
                        else if (format.showSigned) {
                            display = QString::number(
                                static_cast<std::make_signed_t<val_t>>(val)
                            );
                        }
                        else {
                            display = QString::number(val);
                        }
                    }
                }, rec.data[col]);
            }

            return display;
        }
            break;
        case Qt::TextAlignmentRole:
        {
            int align = Qt::AlignLeft | Qt::AlignVCenter;

            std::visit([&align](const auto& val) {
                if constexpr (!std::is_same_v<WDBReader::Database::string_data_t, std::decay_t<decltype(val)>>)
                {
                    align = Qt::AlignRight | Qt::AlignVCenter;
                }
                }, rec.data[col]);

            return align;
        }
            break;
        case Qt::ForegroundRole:
        {
            if (rec.encryptionState == WDBReader::Database::RecordEncryption::ENCRYPTED) {
                return QColor(Qt::red);
            }

            break;
        }
        }

    }

    return QVariant();
}

QVariant DatabaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && _columns.size() > 0) {
        switch (role) {
        case Qt::DisplayRole:
            return _columns[section].label;
            break;
        case Qt::ToolTipRole:
            return _columns[section].tooltip;
            break;
        }
            
    }
    else if (orientation == Qt::Vertical && _records.size() > 0) {
        switch (role) {
        case Qt::DisplayRole:
            return QString::number(_records[section].recordIndex);
            break;
        }
    }

    return QVariant();
}


void DatabaseModel::setPage(size_t page)
{
    assert(_db);
    emit beginResetModel();
    loadPage(page);
    emit endResetModel();

    emit paginationChanged(true, _pagination);

}

void DatabaseModel::setColumnFormat(int index, const ColumnFormat& format)
{
    //TODO only update column
    emit beginResetModel();
    _columns[index].format = format;
    emit endResetModel();
}

uint64_t DatabaseModel::convertToDbIndex(int model_index)
{
    return (_pagination.currentRowIndexes.first - 1) + model_index;
}

void DatabaseModel::query(Query&& q)
{
    std::set<std::pair<Identifier, bool>> used_identifers;

    loop_conditions(q, false, [&used_identifers](const Condition& cond) -> bool {
        used_identifers.emplace(cond.lhs, false);

        auto check_val = [&used_identifers](const Value& val, bool is_arr) {
            if (std::holds_alternative<Identifier>(val.value)) {
                used_identifers.emplace(std::get<Identifier>(val.value), is_arr);
            }
        };

        bool val_expect_array = false;

        if (cond.op == CompareOperation::IN_ARR || cond.op == CompareOperation::NOT_IN_ARR) {
            val_expect_array = true;
        }
        else {
            if (!std::holds_alternative<Value>(cond.rhs)) {
                throw std::runtime_error("RHS must be an value.");
            }
        }

        std::visit([&used_identifers, &check_val, val_expect_array](const auto& rhs) {
            using rhs_t = std::decay_t<decltype(rhs)>;

            if constexpr (std::is_same_v<Value, rhs_t>) {
                check_val(rhs, val_expect_array);
            }
            else if constexpr (std::is_same_v<Array, rhs_t>) {
                for (const auto& v : rhs.value) {
                    check_val(v, false);
                }
            }

            }, cond.rhs);


        return true;
    });

    QueryContext context(std::move(q));

    const auto& names = _db->getSchema().names();
    size_t field_index = 0;
    size_t field_range_index = 0;
    for (const auto& field : _db->getSchema().fields()) {
        const auto& name = names[field_index++];

        if (used_identifers.size() == 0) {
            break;
        }

        bool check = true;

        while (check) {
            check = false;
            
            for (auto it = used_identifers.begin(); it != used_identifers.end(); ++it) {
                if (it->first.value == name) {
                    if (it->second) {
                        if (!field.isArray() || it->first.index.has_value()) {
                            throw std::runtime_error(std::string("Identifier must be array: ").append(it->first.value));
                        }
                    }
                    else {
                        if (field.isArray() && !it->first.index.has_value()) {
                            throw std::runtime_error(std::string("Identifier must be value: ").append(it->first.value));
                        }
                    }

                    if (it->first.index.has_value()) {
                        const auto index = it->first.index.value();
                        if (index >= field.size) {
                            throw std::runtime_error(std::string("Invalid index ").append(it->first.value));
                        }
                        context.id_lookup.emplace(it->first, std::make_pair( field_range_index + index, field_range_index + index));
                    }
                    else {
                        context.id_lookup.emplace(it->first, std::make_pair(field_range_index, field_range_index + (field.size - 1)));
                    }

                    used_identifers.erase(it);
                    check = true;
                    break;
                }
            }
        }

        field_range_index += field.size;
    }

    if (used_identifers.size() > 0) {
        throw std::runtime_error(std::string("Unknown identifier: ").append(used_identifers.begin()->first.value));
    }

    _query_context.emplace(std::move(context));

    setPage(0);
}

void DatabaseModel::clearQuery()
{
    _query_context = std::nullopt;
    setPage(0);
}

bool DatabaseModel::loop_conditions(const Query& q, bool short_circuit, std::function<bool(const Condition&)> callback) const
{

    std::function<bool(const Expression&)> loop_expr;
    std::function<bool(const ConditionGroup&)> loop_group;

    loop_expr = [&](const Expression& expr) -> bool {
        return std::visit([&](const auto& val) {
            using val_t = std::decay_t<decltype(val)>;

            if constexpr (std::is_same_v<Condition, val_t>) {
                return callback(val);
            }
            else {
                return loop_group(val);
            }

            }, expr.value);
    };

    loop_group = [&](const ConditionGroup& group) -> bool {

        assert(group.lhs);
        const bool lhs = loop_expr(*group.lhs);

        if (short_circuit) {
            if (!lhs && group.op == LogicOperation::AND) {
                return false;
            }
            else if (lhs && group.op == LogicOperation::OR) {
                return true;
            }
        }

        assert(group.rhs);
        const bool rhs = loop_expr(*group.rhs);

        if (group.op == LogicOperation::AND) {
            return lhs && rhs;
        }
        else if (group.op == LogicOperation::OR) {
            return lhs || rhs;
        }
        
        throw std::logic_error("Unknown group op.");
    };


    return loop_expr(q.value);
}

static bool compare(const std::string& lhs, CompareOperation op, const std::string& rhs) {

    switch (op) {
    case CompareOperation::EQUAL:
        return lhs == rhs;
    case CompareOperation::NOT_EQUAL:
        return lhs != rhs;
    }

    throw std::logic_error("Unknown string compare op.");
}

template<typename T>
static bool compare(const T& lhs, CompareOperation op, const T& rhs) {

    switch (op) {
    case CompareOperation::EQUAL:
        return lhs == rhs;
    case CompareOperation::NOT_EQUAL:
        return lhs != rhs;
    case CompareOperation::LESS_THAN:
        return lhs < rhs;
    case CompareOperation::LESS_THAN_EQUAL:
        return lhs <= rhs;
    case CompareOperation::GREATER_THAN:
        return lhs > rhs;
    case CompareOperation::GREATER_THAN_EQUAL:
        return lhs >= rhs;
    default:
        throw std::logic_error("Unknown compare op.");
    }

    return false;
}

bool DatabaseModel::evaluateRecord(const WDBReader::Database::RuntimeRecord& rec) const
{
    assert(_query_context.has_value());

    auto single_identifier_value = [&](const Identifier& id) {
        const auto& id_index = _query_context.value().id_lookup.at(id);
        assert(id_index.first == id_index.second);
        return reinterpret_cast<const WDBReader::Database::runtime_value_ref_t*>(&rec.data[id_index.first]);
    };

    auto evaluate = [&]<typename T>(const T& lhs, CompareOperation op, const Value& rhs) -> bool {

        if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<String>(rhs.value)) {
                const String& str = std::get<String>(rhs.value);
                return compare(lhs, op, str.value);
            }
            else if (std::holds_alternative<Identifier>(rhs.value)) {
                auto val = single_identifier_value(std::get<Identifier>(rhs.value));

                if (std::holds_alternative<WDBReader::Database::string_data_ref_t>(*val)) {
                    auto raw_str = std::get<WDBReader::Database::string_data_ref_t>(*val);
                    std::string str(raw_str);
                    return compare(lhs, op, str);
                }
            }
        }
        else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, float>) {
            if (std::holds_alternative<Number>(rhs.value)) {
                const Number& number = std::get<Number>(rhs.value);

                return std::visit([&](auto num_val) {
                    return compare(lhs, op, (T)num_val);
                }, number.value);
            }
            else if (std::holds_alternative<Identifier>(rhs.value)) {
                auto val = single_identifier_value(std::get<Identifier>(rhs.value));

                return std::visit([&](const auto& tmp) -> T {
                    using tmp_t = std::decay_t<decltype(tmp)>;

                    if constexpr (std::is_integral_v<tmp_t> || std::is_same_v<tmp_t, float>) {
                        return  compare(lhs, op, (T)tmp);
                    }

                    throw std::runtime_error("Invalid type for comparison.");
                }, *val);
            }
        }
        else {
            static_assert(false);
        } 

        throw std::logic_error("Unable to evaluate.");
    };

    auto evaluate_all = [&]<typename T>(const T & lhs, CompareOperation op, const Condition::rhs_t & rhs) -> bool {
        if (op == CompareOperation::IN_ARR || op == CompareOperation::NOT_IN_ARR) {
            if (std::holds_alternative<Array>(rhs)) {
                const Array& rhs_val = std::get<Array>(rhs);

                for (const auto& val : rhs_val.value) {
                    const auto arr_op = op == CompareOperation::IN_ARR ? CompareOperation::EQUAL : CompareOperation::NOT_EQUAL;
                    if (evaluate(lhs, arr_op, val)) {
                        return true;
                    }
                }

                return false;
            }
            else if (std::holds_alternative<Value>(rhs)) {
                const auto& val = std::get<Value>(rhs);

                if (std::holds_alternative<Identifier>(val.value)) {
                    //TODO
                    throw std::logic_error("Array operations not yet implemented for identifiers.");
                }
            }
        }
        else if (std::holds_alternative<Value>(rhs)) {
            return evaluate(lhs, op, std::get<Value>(rhs));
        }

        throw std::runtime_error("Invalid comparison.");
    };


    bool matched = loop_conditions(_query_context.value().query, true, [&](const Condition& cond) -> bool {
        auto& lval_index = _query_context.value().id_lookup.at(cond.lhs);
        assert(lval_index.first == lval_index.second);
        auto lhs_var = reinterpret_cast<const WDBReader::Database::runtime_value_ref_t*>(&rec.data[lval_index.first]);

        return std::visit([&](const auto& lval) {

            using lval_t = std::decay_t<decltype(lval)>;
            if constexpr (std::is_same_v<lval_t, float>) {
                return evaluate_all(lval, cond.op, cond.rhs);
            }
            else if constexpr (std::is_same_v<lval_t, WDBReader::Database::string_data_ref_t>) {
                std::string str(lval);
                return evaluate_all(str, cond.op, cond.rhs);
            }
            else if constexpr (std::is_integral_v<lval_t>) {
                //TODO this probably doesnt handle conversion properly.
                return evaluate_all(int64_t(lval), cond.op, cond.rhs);
            }

            return false;

        }, *lhs_var);


        
        return false;
    });

    return matched;
}

void DatabaseModel::loadPage(size_t page)
{
    _records.clear();

    const size_t start_index = [&] {
        
        if (_query_context.has_value() && page > 0) {
            const auto prev = page - 1;
            return _query_context->page_indexes.at(prev);
        }
        
        return page * _pagination.pageSize;
    }();

    size_t i = start_index;

    auto it = _db->getDataSource()->begin();
    std::advance(it, start_index);

    size_t current_count = 0;
    for (;it != _db->getDataSource()->end(); ++it) {

        if (current_count >= _pagination.pageSize) {
            break;
        }

        const bool use = _query_context.has_value() ? evaluateRecord(*it) : true;
        
        i++;
       
        if (use) {
            current_count++;
            _records.push_back(std::move(*it));
        }
    }

    if (_query_context.has_value()) {
        _query_context->page_indexes[page] = i;
    }

    _pagination.currentRowIndexes = { start_index + 1, i };
    _pagination.currentPageIndex = page;
}
