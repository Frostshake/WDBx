#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "exportdialog.h"
#include "schemahelpers.h"
#include <QtWidgets>

namespace WDBR = WDBReader;
namespace WDBR_DB = WDBReader::Database;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Settings::init(this);

    ui->setupUi(this);

    // status bar //

    labelStatusClientVer = new QLabel("");
    labelStatusClientVer->setContentsMargins(QMargins(6, 0, 6, 0));
    labelStatusClientVer->setTextInteractionFlags(
        Qt::TextInteractionFlag::TextSelectableByMouse | Qt::TextInteractionFlag::TextSelectableByKeyboard
    );
    ui->statusbar->addWidget(labelStatusClientVer);

    labelStatusPage = new QLabel("");
    labelStatusPage->setContentsMargins(QMargins(6, 0, 6, 0));
    ui->statusbar->addPermanentWidget(labelStatusPage);

    buttonStatusPrev = new QPushButton();
    buttonStatusPrev->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
    buttonStatusPrev->setToolTip("Previous");
    connect(buttonStatusPrev, &QPushButton::pressed, [&]() {
        movePage(-1);
    });
    ui->statusbar->addPermanentWidget(buttonStatusPrev);

    buttonStatusNext = new QPushButton();
    buttonStatusNext->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    buttonStatusNext->setToolTip("Next");
    connect(buttonStatusNext, &QPushButton::pressed, [&]() {
        movePage(+1);
    });
    ui->statusbar->addPermanentWidget(buttonStatusNext);


    // menu

    connect(ui->actionOpen, &QAction::triggered, [&]() {
        showOpenDialog();
    });

    ui->actionExport->setDisabled(true);
    connect(ui->actionExport, &QAction::triggered, [&]() {
        ExportDialog* dialog = new ExportDialog(ui->widgetTable->getDatabaseModel(), this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);

        dialog->show();
    });

    connect(ui->actionExit, &QAction::triggered, [&]() {
        this->close();
     });

    connect(ui->actionAbout, &QAction::triggered, [&]() {
        QMessageBox::information(this, "About WDBx", "WDBx version " WDBX_VERSION);
    });

    connect(ui->actionHelp, &QAction::triggered, [&]() {
        QDesktopServices::openUrl(QUrl("https://github.com/Frostshake/WDBx"));
    });


    databasesModel = new QStandardItemModel();
    databasesProxy = new QSortFilterProxyModel();
    databasesProxy->setSourceModel(databasesModel);
    databasesProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->listViewDatabases->setModel(databasesProxy);

    connect(ui->widgetTable, &Table::paginationChanged, this, &MainWindow::onPaginationChanged);
    buttonStatusPrev->setDisabled(true);
    buttonStatusNext->setDisabled(true);
    labelStatusPage->setText("");

    connect(ui->listViewDatabases->selectionModel(), &QItemSelectionModel::currentChanged, 
        [&](const QModelIndex& current, const QModelIndex& previous) {   
            QStandardItem* item = databasesModel->itemFromIndex(databasesProxy->mapToSource(current));
            if (item != nullptr) {
                std::string db_name = item->text().toStdString();
                openDatabase(db_name);
            }
        });

    connect(ui->lineEditDatabases, &QLineEdit::textChanged, [&](const QString& text) {
        databasesProxy->setFilterWildcard(text);
    });


    showOpenDialog();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showOpenDialog()
{
    OpenDialog* open_dialog = new OpenDialog(this);
    open_dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(open_dialog, &OpenDialog::chosen, [&](OpenInfo info) {
        chooseDatabaseDir(info);
    });

    open_dialog->show();
}

void MainWindow::chooseDatabaseDir(const OpenInfo& info)
{
    databasesModel->clear();

    _clientInfo = [&info] {
        WDBReader::ClientInfo ci;
        ci.name = info.name;
        ci.locales = { info.locale };
        ci.version = info.version;
        return ci;
    }();

    _defsDir = info.defs_path;

    {
        auto label_str = _clientInfo->version.toString() + " (" + _clientInfo->locales[0] + ")";
        if (_clientInfo->name.length() > 0) {
            label_str += " [" + _clientInfo->name + "]";
        }
        labelStatusClientVer->setText(QString::fromStdString(label_str));
    }

    if (info.filesystem == "CASC") {
        _fs = std::make_unique<CASCFilesystem>(info.path, _clientInfo->locales[0], _clientInfo->name);
    }
    else if (info.filesystem == "MPQ") {
        _fs = std::make_unique<MPQFilesystem>(info.path);
    } 
    else if (info.filesystem == "Native") {
        _fs = std::make_unique<NativeFilesystem>(info.path);
    }
    else {
        _fs = nullptr;
        throw std::logic_error("Unknown filesystem type.");
    }

    for (const auto& name : _fs->discovered()) {
        QStandardItem* item = new QStandardItem(
            QApplication::style()->standardIcon(QStyle::SP_FileIcon), QString::fromStdString(name)
        );
        item->setToolTip(QString::fromStdString(schemaPlainName(name)));

        databasesModel->appendRow(item);
    }
}

void MainWindow::openDatabase(const std::string& name)
{
    assert(_clientInfo.has_value());
    assert(_defsDir.string().length() > 0);

    std::unique_ptr<Database> db;

    try {
        db = std::make_unique<Database>(name);
        db->load(*_clientInfo, _defsDir, _fs->open(name));
    }
    catch (std::exception& e) {
        ui->widgetTable->clearDatabase();

        QMessageBox::critical(this, "Error", QString("Unable to open database. \n%1").arg(e.what()));
        return;
    }

    ui->widgetTable->openDatabase(std::move(db));
    ui->actionExport->setDisabled(false);
}

void MainWindow::onPaginationChanged(bool has_db, const Pagination& page)
{
    buttonStatusPrev->setDisabled(!has_db);
    buttonStatusNext->setDisabled(!has_db);

    if (!has_db) {
        labelStatusPage->setText("");
    }
    else {
        labelStatusPage->setText(QString("%1 - %2 of %3")
            .arg(page.currentRowIndexes.first)
            .arg(page.currentRowIndexes.second)
            .arg(page.totalRowCount)
        );
    }
}

void MainWindow::movePage(int32_t dir)
{
    const auto& page = ui->widgetTable->getPagination();
    auto new_page = page.currentPageIndex + dir;
    new_page = std::max(0, new_page);
    new_page = std::min(page.endPageIndex, new_page);

    if (new_page != page.currentPageIndex) {
        ui->widgetTable->setPage(new_page);
    }
}



