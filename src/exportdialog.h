#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include "databasemodel.h"
#include "schemaexport.h"
#include <QDialog>
#include <future>  


namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(std::weak_ptr<DatabaseModel> db, QWidget *parent = nullptr);
    ~ExportDialog();

protected:

    void beginExport();
    void doExport(QString path, QString type);

private:
    Ui::ExportDialog *ui;  
 
    std::weak_ptr<DatabaseModel> _db;

    std::atomic<bool> _isExporting;
    std::future<void> _asyncExport;
    std::unique_ptr<DataExport> _exporter;
    float _percent;
};

#endif // EXPORTDIALOG_H
