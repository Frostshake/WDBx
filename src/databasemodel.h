#pragma once

#include "database.h"
#include <QAbstractTableModel>
#include <memory>
#include <vector>
#include "query.h"

struct Pagination {
public:
    int32_t currentPageIndex{ 0 };
    size_t pageSize{ 0 };
    int32_t endPageIndex{ 0 };
    size_t totalRowCount{ 0 };
    std::pair<size_t, size_t> currentRowIndexes{ 0,0 };
};

class DatabaseModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    struct ColumnFormat {
    public:
        bool showSigned{ false };
        bool showHex{ false };
        bool showBinary{ false };
    };

    struct Column {
    public:
        QString label;
        QString tooltip;
        ColumnFormat format;
        WDBReader::Database::Field::Type type;
    };

    struct QueryContext {
        QueryContext(Query&& q) : query(std::move(q)) {}
        Query query;
        std::map<Identifier, std::pair<size_t, size_t>> id_lookup;
        std::map<size_t, size_t> page_indexes;
    };

    explicit DatabaseModel(std::shared_ptr<Database> db, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setPage(size_t page);
    void setColumnFormat(int index, const ColumnFormat& format);

    const Pagination& pagination() const {
        return _pagination;
    }

    const Column& column(int index) const {
        return _columns[index];
    }

    uint64_t convertToDbIndex(int model_index);

    void query(Query&& q);
    void clearQuery();

signals:
    void paginationChanged(bool has_db, const Pagination& page);

protected:

    bool loop_conditions(const Query& q, bool short_circuit, std::function<bool(const Condition&)> callback) const;
    bool evaluateRecord(const WDBReader::Database::RuntimeRecord& rec) const;

    void loadPage(size_t page);

    std::shared_ptr<Database> _db;
    Pagination _pagination;

    std::vector<Column> _columns;
    std::vector<WDBReader::Database::RuntimeRecord> _records;

    std::optional<QueryContext> _query_context;

    friend class DataExport;
};