#include "schemaexport.h"

#include "schemahelpers.h"
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <ranges>


QString SchemaExport::asCSV(const std::string& db_name, const WDBReader::Database::RuntimeSchema& schema)
{
    QString str;
    QTextStream stream(&str);
    constexpr QChar SEP = ';';

    stream << "Name" << SEP
        << "Data Type" << SEP
        << "Size" << SEP
        << "Count" << SEP
        << "Annotations" << SEP 
        << '\n';

    for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {
        stream << QString::fromStdString(name) << SEP;
        stream << schemaFieldTypeToString(field.type) << SEP;
        stream << field.bytes << SEP;
        stream << field.size << SEP;
        stream << schemaFieldAnnotationsToString<QStringList>(field.annotation).join(',') << SEP
            << '\n';
    }

    return str;
}

QString SchemaExport::asJSON(const std::string& db_name, const WDBReader::Database::RuntimeSchema& schema)
{
    QJsonArray json;

    for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {
        QJsonObject obj{
            {"name", QString::fromStdString(name)},
            {"data_type", schemaFieldTypeToString(field.type)},
            {"size", field.bytes},
            {"count", field.size},
            {"annotations", schemaFieldAnnotationsToString<QJsonArray>(field.annotation)}
        };

        json.append(std::move(obj));
    }

    QJsonDocument doc(json);
    return doc.toJson(QJsonDocument::Indented);
}

QString SchemaExport::asSQL(const std::string& db_name, const WDBReader::Database::RuntimeSchema& schema)
{
    QString str;
    QTextStream stream(&str);

    stream << "CREATE TABLE `" << QString::fromStdString(schemaPlainName(db_name)) << "` (" << '\n';

    int i = 0;
    const auto field_size = schema.fields().size();
    

    auto make_field = [&](const WDBReader::Database::Field& field, const std::string& name, int32_t index = -1) {

        auto fmt_name = QString::fromStdString(name);

        if (index >= 0) {
            fmt_name += QString("_%1").arg(index);
        }

        stream << '`' << fmt_name << '`';

        switch (field.type) {
        case WDBReader::Database::Field::Type::INT:
            switch (field.size) {
                case sizeof(uint8_t) :
                    stream << " TINYINT UNSIGNED NOT NULL";
                    break;
                case sizeof(uint16_t) :
                    stream << " SMALLINT UNSIGNED NOT NULL";
                    break;
                case sizeof(uint32_t) :
                    stream << " INT UNSIGNED NOT NULL";
                    break;
                case sizeof(uint64_t) :
                    stream << " BIGINT UNSIGNED NOT NULL";
                    break;
                default: 
                    throw std::logic_error("Unknown field size.");
                    break;
            }
            break;
        case WDBReader::Database::Field::Type::FLOAT:
            stream << " FLOAT NOT NULL";
            break;
        case WDBReader::Database::Field::Type::STRING:
        case WDBReader::Database::Field::Type::LANG_STRING:
            stream << " VARCHAR(255) NOT NULL";
            break;
        default:
            throw std::logic_error("Unknown field type.");
            break;
        }
    };
    
    std::string id_name;
    
    for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {
        auto finish_line = [&]() {
            if (i < field_size || !id_name.empty()) {
                stream << ',';
            }

            stream << '\n';
        };

        if (field.isArray()) {
            for (auto a = 0; a < field.size; a++) {
                stream << '\t';
                make_field(field, name, a);
                finish_line();
            }
        }
        else {
            if (field.annotation.isId && id_name.empty()) {
                id_name = name;
            }

            stream << '\t';
            make_field(field, name);
            finish_line();
        }
    }

    if (!id_name.empty()) {
        stream << "\tPRIMARY KEY (`" << QString::fromStdString(id_name) << "`)\n";
    }

    stream << ")";

    return str;
}


void CSVDataExport::exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes)
{
    auto& schema = _db->getSchema();

    for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {
        if (field.isArray()) {
            for (auto a = 0; a < field.size; a++) {
                _stream << '"' << QString::fromStdString(name) << '[' << a << ']' << '"' << ',';
            } 
        }
        else {
            _stream << '"' << QString::fromStdString(name) << '"' << ',';
        }
    }

    _stream << '\n';

    handleRows(indexes, [&](WDBReader::Database::RuntimeRecord&& rec) {
        for (const auto& field : schema(rec)) {
            for (const auto& val : field.value) {
                _stream << formatValue(val, '"') << ',';   
            }
        }
        _stream << '\n';
    });
}

void JSONDataExport::exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes)
{
    auto& schema = _db->getSchema();
    QJsonArray json;

    handleRows(indexes, [&](WDBReader::Database::RuntimeRecord&& rec) {
        QJsonObject obj;

        auto set_val = [](const auto& val) {
            QJsonValue jv;
            
            //TODO column  formatting
            std::visit([&](const auto& v) {
                using val_t = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<WDBReader::Database::string_data_ref_t, val_t>) {
                    std::string_view view(v);
                    jv =  QString::fromUtf8(view.data(), view.size());
                }
                else if constexpr (std::is_floating_point_v<val_t>) {
                    jv = v;
                }
                else {
                    jv = (int64_t)v;
                }
            }, val);

            return jv;
        };

        for (const auto& field : schema(rec)) {
         
            if (field.value.size() <= 1) {
                obj[QString::fromStdString(*field.name)] = set_val(field.value[0]);
            }
            else {
                QJsonArray arr;
                for (const auto& val : field.value) {
                    arr.append(set_val(val));
                }
                obj[QString::fromStdString(*field.name)] = std::move(arr);
            }
        }

        json.append(std::move(obj));
    });

    QJsonDocument doc(json);
    _stream << doc.toJson(QJsonDocument::Indented);
}

void SQLInsertDataExport::exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes)
{
    uint64_t row = 0;
    auto& schema = _db->getSchema();
    const QString name = QString::fromStdString(schemaPlainName(_db->getName()));
    constexpr uint64_t chunk = 1000;

    handleRows(indexes, [&](WDBReader::Database::RuntimeRecord&& rec) {
        if (row++ % chunk == 0) { 
            _stream << "INSERT INTO `" << name << "` (";
            bool first = true;
            for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {
                if (!first) {
                    _stream << ", ";
                }

                if (field.isArray()) {
                    for (auto a = 0; a < field.size; a++) {
                        _stream << '`' << QString::fromStdString(name) << '_' << a << '`';
                    }
                }
                else {
                    _stream << '`' << QString::fromStdString(name) << '`';
                }

                first = false;
            }

            _stream << ")\n";
            _stream << "VALUES \n";
        }

        _stream << "(";
        bool first = true;
        for (const auto& field : schema(rec)) {
            for (const auto& val : field.value) {

                if (!first) {
                    _stream << ", ";
                }

                _stream << formatValue(val, '"');
                first = false;
            }
        }

        _stream << ")";
        _stream << ((row % chunk == 0) ? ';' : ',');
        _stream << '\n';

    });
}

void SQLUpdateDataExport::exportTo(std::optional<std::pair<uint64_t, uint64_t>> indexes)
{
    auto& schema = _db->getSchema();
    const QString name = QString::fromStdString(schemaPlainName(_db->getName()));

    handleRows(indexes, [&](WDBReader::Database::RuntimeRecord&& rec) {

        QString id_name;
        QString id_val;

        _stream << "UPDATE `" << name << "` SET ";
        bool first = true;
        for (const auto& field : schema(rec)) {
            if (field.field->annotation.isId) {
                assert(id_name.isEmpty());
                assert(id_val.isEmpty());
                assert(field.value.size() == 1);

                id_name = QString::fromStdString(*field.name);
                id_val = formatValue(field.value[0], '"');
            }
            else {
                
                uint64_t idx = 0;
                for (const auto& val : field.value) {

                    if (!first) {
                        _stream << ", ";
                    }

                    if (field.field->isArray()) {
                        _stream << '`' << QString::fromStdString(*field.name) << '_' << idx << '`';
                    }
                    else {
                        _stream << '`' << QString::fromStdString(*field.name) << '`';
                    }

                    _stream << " = " << formatValue(val, '"');
                    first = false;
                    idx++;
                }
            }
        }

        assert(!id_name.isEmpty());
        assert(!id_val.isEmpty());

        _stream << " WHERE `" << id_name << "` = " << id_val << ";\n";

    });
}
