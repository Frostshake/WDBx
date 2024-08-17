#include "table.h"
#include "ui_table.h"
#include "schemaexport.h"
#include "schemahelpers.h"
#include <QClipboard>
#include <QFileDialog>
#include <ranges>

namespace WDBR = WDBReader;
namespace WDBR_DB = WDBReader::Database;

Table::Table(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Table)
{
    ui->setupUi(this);

    setupDataTable();
    setupSchemaTable();
}

Table::~Table()
{
    delete ui;
}

void Table::openDatabase(std::unique_ptr<Database> db)
{
    _db = std::move(db);
    dataModel = std::make_shared<DatabaseModel>(_db, nullptr);
    ui->tableViewData->setModel(dataModel.get());

    connect(dataModel.get(), &DatabaseModel::paginationChanged, [&](auto ...args) {
        emit paginationChanged(args...);
    });

    const auto& schema = _db->getSchema();
    schemaModel->removeRows(0, schemaModel->rowCount());

    for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {

        const QString data_type = schemaFieldTypeToString(field.type);
        QString data_size = "";
        QString array_size = "";
        const auto annotations = schemaFieldAnnotationsToString<QStringList>(field.annotation);

        QBrush data_type_color = QApplication::palette().text().color();

        if (field.isArray()) {
            array_size = QString::number(field.size);
        }

        switch (field.type) {
        case WDBR_DB::Field::Type::FLOAT:
            data_size = QString::number(field.bytes);
            data_type_color = Qt::darkRed;
            break;
        case WDBR_DB::Field::Type::INT:
            data_size = QString::number(field.bytes);
            data_type_color = Qt::darkBlue;
            break;
        case WDBR_DB::Field::Type::LANG_STRING:
        case WDBR_DB::Field::Type::STRING:
            data_type_color = Qt::darkGreen;
            data_type_color = Qt::darkGreen;
            break;
        }

        auto data_type_item = new QStandardItem(data_type);
        data_type_item->setForeground(data_type_color);

        QList<QStandardItem*> columns = {
            new QStandardItem(QString::fromStdString(name)),
            data_type_item,
            new QStandardItem(data_size),
            new QStandardItem(array_size),
            new QStandardItem(annotations.join(','))
        };

        schemaModel->appendRow(columns);
    }

    ui->tableViewTable->resizeColumnToContents(0);

    ui->labelTableName->setText(QString::fromStdString(_db->getName()));
    ui->labelTableFileSize->setText(this->locale().formattedDataSize(_db->getFileSize()));
    ui->labelTableType->setText(QString::fromStdString(_db->getType()).toUpper());
    ui->labelTableRows->setText(QString::number(_db->getRowCount(), 10));

    dataHeaderMenu = new QMenu(ui->tableViewData->horizontalHeader());

    int element_index = 0;
    for (const auto& [name, field] : std::views::zip(schema.names(), schema.fields())) {
        bool show_index = field.size > 1;
        for (auto i = 0; i < field.size; i++) {

            auto fmt_name = QString::fromStdString(name);
            if (show_index) {
                fmt_name += "[" + QString::number(i) + "]";
            }
            auto* action = new QAction(fmt_name);
            action->setCheckable(true);
            action->setChecked(true);

            connect(action, &QAction::toggled, [element_index, this](bool checked) {
                ui->tableViewData->setColumnHidden(element_index, !checked);
            });
            dataHeaderMenu->addAction(action);
            element_index++;
        }
    }

    dataHeaderMenu->addSeparator();
    dataHeaderMenu->addMenu(dataHeaderFormatMenu);

    headerFormatSigned->setChecked(false);
    headerFormatHex->setChecked(false);
    headerFormatBinary->setChecked(false);

    setPage(0);

    for (auto i = 0; i < ui->tableViewData->colorCount(); i++) {
        ui->tableViewData->setColumnHidden(i, false);
    }

    ui->tableViewData->resizeColumnsToContents();
    
}

std::shared_ptr<DatabaseModel> Table::getDatabaseModel()
{
    return dataModel;
}

void Table::clearDatabase()
{
    ui->tableViewData->setModel(nullptr);
    dataModel = nullptr;
    schemaModel->removeRows(0, schemaModel->rowCount());

    _db = nullptr;
}

void Table::setPage(size_t page)
{
    dataModel->setPage(page);
}

void Table::setupDataTable()
{
    ui->tableViewData->setSelectionMode(QAbstractItemView::ContiguousSelection);
    ui->tableViewData->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    dataHeaderMenu = nullptr;
    dataHeaderFormatMenu = new QMenu("Format");

    auto format_handler = [&](bool checked) {
        QAction* sender = qobject_cast<QAction*>(QObject::sender());

        if (sender) {
            auto col_index = sender->data().toInt();
            if (col_index >= 0) {
                DatabaseModel::ColumnFormat fmt;
                fmt.showSigned = (sender == headerFormatSigned) && sender->isChecked();
                fmt.showHex = (sender == headerFormatHex) && sender->isChecked();
                fmt.showBinary = (sender == headerFormatBinary) && sender->isChecked();
                dataModel->setColumnFormat(col_index, fmt);
            }
        }
    };

    headerFormatSigned = new QAction("Signed");
    headerFormatSigned->setCheckable(true);
    connect(headerFormatSigned, &QAction::toggled, this, format_handler);
    dataHeaderFormatMenu->addAction(headerFormatSigned);

    headerFormatHex = new QAction("Hex");
    headerFormatHex->setCheckable(true);
    connect(headerFormatHex, &QAction::toggled, this, format_handler);
    dataHeaderFormatMenu->addAction(headerFormatHex);

    headerFormatBinary = new QAction("Binary");
    headerFormatBinary->setCheckable(true);
    connect(headerFormatBinary, &QAction::toggled, this, format_handler);
    dataHeaderFormatMenu->addAction(headerFormatBinary);

    ui->tableViewData->horizontalHeader()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tableViewData->horizontalHeader(), &QHeaderView::customContextMenuRequested, [&](const QPoint& point) {
        if (dataHeaderMenu != nullptr) {
            const auto index = ui->tableViewData->indexAt(point);
            const auto& col = dataModel->column(index.column());

            if (col.type == WDBR::Database::Field::Type::INT) {

                auto setup_format = [&index](QAction* action, bool checked) {
                    action->blockSignals(true);
                    action->setChecked(checked);
                    action->setData(index.column());
                    action->blockSignals(false);
                    };

                dataHeaderFormatMenu->setDisabled(false);

                setup_format(headerFormatSigned, col.format.showSigned);
                setup_format(headerFormatHex, col.format.showHex);
                setup_format(headerFormatBinary, col.format.showBinary);
            }
            else {
                dataHeaderFormatMenu->setDisabled(true);
            }

            dataHeaderMenu->popup(ui->tableViewData->horizontalHeader()->viewport()->mapToGlobal(point));
        }
    });

    auto data_export_builder = [&]<typename T>(const auto& name, auto filter) -> QAction* {
        auto temp = new QAction(name);

        connect(temp, &QAction::triggered, [&,filter]() {

            auto* selected = ui->tableViewData->selectionModel();
            if (selected->selectedIndexes().size() > 0) {

                int first_index = std::numeric_limits<int>::max();
                int last_index = 0;
                for (auto& index : selected->selectedIndexes()) {
                    first_index = std::min(first_index, index.row());
                    last_index = std::max(last_index, index.row());
                }

                QString str;
                QTextStream stream(&str);
                T exporter(dataModel, stream);
                exporter.exportTo(std::make_pair(
                    dataModel->convertToDbIndex(first_index), dataModel->convertToDbIndex(last_index)
                ));
                
                QGuiApplication::clipboard()->setText(str);
            }
        });

        return temp;
    };


    dataBodyMenu = new QMenu(ui->tableViewData);
    bodyCopy = new QAction("Copy");
    connect(bodyCopy, &QAction::triggered, [&]() {
        auto* selected = ui->tableViewData->selectionModel();
        if (selected->selectedIndexes().size() > 0) {
            const auto first_index = selected->selectedIndexes().first();
            QGuiApplication::clipboard()->setText(
                dataModel->data(first_index, Qt::DisplayRole).toString()
            );
        }      
    });

    dataBodyMenu->addAction(bodyCopy);
    dataBodyCopyAs = new QMenu("Copy As", dataBodyMenu);
    bodyCopyAsCSV = data_export_builder.operator()<CSVDataExport>("CSV", "CSV (*.csv)");
    dataBodyCopyAs->addAction(bodyCopyAsCSV);
    bodyCopyAsJson = data_export_builder.operator()<JSONDataExport>("JSON", "JSON (*.json)");
    dataBodyCopyAs->addAction(bodyCopyAsJson);
    bodyCopyAsSQLInsert = data_export_builder.operator()<SQLInsertDataExport>("SQL Insert", "SQL (*.sql)");
    dataBodyCopyAs->addAction(bodyCopyAsSQLInsert);
    bodyCopyAsSQLUpdate = data_export_builder.operator()<SQLUpdateDataExport>("SQL Update", "SQL (*.sql)");
    dataBodyCopyAs->addAction(bodyCopyAsSQLUpdate);

    dataBodyMenu->addMenu(dataBodyCopyAs);

    auto body_context_menu_handler = [&](const QPoint& point) {
        if (dataBodyMenu != nullptr) {
            dataBodyMenu->popup(ui->tableViewData->viewport()->mapToGlobal(point));
        }
    };

    ui->tableViewData->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tableViewData, &QTableView::customContextMenuRequested, this, body_context_menu_handler);

    ui->tableViewData->verticalHeader()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tableViewData->verticalHeader(), &QHeaderView::customContextMenuRequested, this, body_context_menu_handler);
}

void Table::setupSchemaTable()
{
    ui->tableViewTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableViewTable->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    schemaModel = new QStandardItemModel();
    schemaModel->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    schemaModel->setHorizontalHeaderItem(1, new QStandardItem("Data Type"));
    schemaModel->setHorizontalHeaderItem(2, new QStandardItem("Size"));
    schemaModel->setHorizontalHeaderItem(3, new QStandardItem("Count"));
    schemaModel->setHorizontalHeaderItem(4, new QStandardItem("Annotations"));
    ui->tableViewTable->setModel(schemaModel);


    schemaMenu = new QMenu(ui->tableViewTable);
    schemaCopy = new QAction("Copy");
    connect(schemaCopy, &QAction::triggered, [&]() {
        QGuiApplication::clipboard()->setText(schemaCopy->data().toString());
    });
    schemaMenu->addAction(schemaCopy);

    schemaCopyAs = new QMenu("Copy As", schemaMenu);
    
    auto schema_export_builder = [&](const auto& name, auto fn) -> QAction* {
        auto temp = new QAction(name);
        schemaCopyAs->addAction(temp);

        connect(temp, &QAction::triggered, [&, fn]() {
            const auto str = fn(_db->getName(), _db->getSchema());
            QGuiApplication::clipboard()->setText(str);
        });

        return temp;
    };

    schemaCopyCSV = schema_export_builder("CSV", SchemaExport::asCSV);
    schemaCopyJson = schema_export_builder("JSON", SchemaExport::asJSON);
    schemaCopySQLCreate = schema_export_builder("SQL Create", SchemaExport::asSQL);

    schemaMenu->addMenu(schemaCopyAs);


    auto context_menu_handler = [&](const QPoint& point) {
        if (schemaMenu != nullptr) {
            const auto index = ui->tableViewTable->indexAt(point);

            if (index.isValid()) {
                schemaCopy->setData(schemaModel->data(index, Qt::DisplayRole));
            }
            else {
                schemaCopy->setData("");
            }

            schemaMenu->popup(ui->tableViewTable->viewport()->mapToGlobal(point));
        }
    };

    ui->tableViewTable->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->tableViewTable, &QTableView::customContextMenuRequested, context_menu_handler);

    ui->tableViewTable->horizontalHeader()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    ui->tableViewTable->verticalHeader()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    connect(ui->tableViewTable->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, context_menu_handler);
    connect(ui->tableViewTable->verticalHeader(), &QHeaderView::customContextMenuRequested, this, context_menu_handler);
}
