#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "database.h"
#include "filesystem.h"
#include "opendialog.h"
#include "settings.h"
#include "table.h"
#include <memory>
#include <optional>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <WDBReader/Detection.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showOpenDialog();
    void chooseDatabaseDir(const OpenInfo& info);
    void openDatabase(const std::string& name);

    void onPaginationChanged(bool has_db, const Pagination& page);

    void movePage(int32_t dir);

private:

    Ui::MainWindow *ui;
    QStandardItemModel* databasesModel;
    QSortFilterProxyModel* databasesProxy;

    QLabel* labelStatusClientVer;
    QLabel* labelStatusPage;
    QPushButton* buttonStatusPrev;
    QPushButton* buttonStatusNext;

    std::unique_ptr<Filesystem> _fs;
    std::optional<WDBReader::ClientInfo> _clientInfo;
    std::filesystem::path _defsDir;
};
#endif // MAINWINDOW_H
