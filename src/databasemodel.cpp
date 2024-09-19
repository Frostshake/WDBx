#include "databasemodel.h"
#include "schemahelpers.h"
#include <ranges>
#include <QColor>

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
    if (orientation == Qt::Horizontal) {
        switch (role) {
        case Qt::DisplayRole:
            return _columns[section].label;
            break;
        case Qt::ToolTipRole:
            return _columns[section].tooltip;
            break;
        }
            
    }
    else if (orientation == Qt::Vertical) {
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

void DatabaseModel::loadPage(size_t page)
{
    _records.clear();

    const size_t start_index = page * _pagination.pageSize;
    size_t i = start_index;

    auto it = _db->getDataSource()->begin();
    std::advance(it, start_index);

    for (;it != _db->getDataSource()->end(); ++it) {

        if (i >= _pagination.pageSize + start_index) {
            break;
        }
        i++;

        _records.push_back(std::move(*it));
    }

    _pagination.currentRowIndexes = { start_index + 1, i };
    _pagination.currentPageIndex = page;
}
