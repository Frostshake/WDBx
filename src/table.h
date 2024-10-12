#ifndef TABLE_H
#define TABLE_H

#include "database.h"
#include "databasemodel.h"
#include <memory>
#include <QWidget>
#include <QMenu>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "query.h"

namespace Ui {
class Table;
}

class Table : public QWidget
{
    Q_OBJECT

public:
    explicit Table(QWidget *parent = nullptr);
    ~Table();

    void openDatabase(std::unique_ptr<Database> db);
    std::shared_ptr<DatabaseModel> getDatabaseModel();
    void clearDatabase();

    const Pagination& getPagination() const {
        return dataModel->pagination();
    }

    void setPage(size_t page);

    void editorVisible(bool visible);
    bool editorToggleVisible();

    void query(Query&& q) {
        if (_db) {
            dataModel->query(std::move(q));
        }
    }

    void clearQuery() {
        if (_db) {
            dataModel->clearQuery();
        }
    }

signals:
    void paginationChanged(bool has_db, const Pagination& page);

private:

    void setupDataTable();
    void setupSchemaTable();

    Ui::Table *ui;

    std::shared_ptr<Database> _db;
    std::shared_ptr<DatabaseModel> dataModel;

    QMenu* dataHeaderMenu;
    QMenu* dataHeaderFormatMenu;
    QAction* headerFormatSigned;
    QAction* headerFormatHex;
    QAction* headerFormatBinary;

    QMenu* dataBodyMenu;
    QAction* bodyCopy;
    QMenu* dataBodyCopyAs;
    QAction* bodyCopyAsCSV;
    QAction* bodyCopyAsJson;
    QAction* bodyCopyAsSQLInsert;
    QAction* bodyCopyAsSQLUpdate;

    QStandardItemModel* schemaModel;
    QMenu* schemaMenu;
    QAction* schemaCopy;
    QMenu* schemaCopyAs;
    QAction* schemaCopyCSV;
    QAction* schemaCopyJson;
    QAction* schemaCopySQLCreate;

};

#endif // TABLE_H
