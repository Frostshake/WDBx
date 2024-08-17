#include "exportdialog.h"
#include "ui_exportdialog.h"
#include <QFileDialog>
#include <QMessageBox>

ExportDialog::ExportDialog(std::weak_ptr<DatabaseModel> db, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog),
    _db(db), _isExporting(false)
{
    ui->setupUi(this);

    ui->progressBar->hide();

    ui->comboBoxFormat->addItem("CSV");
    ui->comboBoxFormat->addItem("JSON");
    ui->comboBoxFormat->addItem("SQL Insert");
    ui->comboBoxFormat->addItem("SQL Update");

    connect(ui->pushButtonExport, &QPushButton::pressed, this, &ExportDialog::beginExport);
}

ExportDialog::~ExportDialog()
{
    if (_exporter != nullptr) {
        _exporter->cancel();
    }
    _isExporting.wait(true);

    delete ui;
}

void ExportDialog::beginExport()
{
    if (_isExporting) {
        return;
    }

    const auto export_str = ui->comboBoxFormat->currentText();
    QString selected_filter = export_str;
    auto selected_sep = selected_filter.indexOf(' ');
    if (selected_sep > 0) {
        selected_filter = selected_filter.mid(0, selected_sep);
    }

    auto save_location = QFileDialog::getSaveFileName(this,
        "Export to file", 
        "./export",
        "CSV (*.csv);;JSON (*.json);;SQL (*.sql)", 
        &selected_filter
    );

    if (!save_location.isEmpty())
    {
        ui->pushButtonExport->setDisabled(true);
        ui->comboBoxFormat->setDisabled(true);
        ui->progressBar->setValue(0);
        ui->progressBar->show();

        _asyncExport = std::async(std::launch::async, [=, this]() {
            try {
                doExport(save_location, export_str);
                accept();
            }
            catch (std::exception& e) {
                QMetaObject::invokeMethod(this, [this, e] {
                    QMessageBox::critical(this, "Export Error", e.what());
                });
            }

            QMetaObject::invokeMethod(this, [this] {
                ui->progressBar->hide();
                ui->pushButtonExport->setDisabled(false);
                ui->comboBoxFormat->setDisabled(false);
            });
        });
    }
}

void ExportDialog::doExport(QString path, QString type)
{
    _isExporting = true;
    _isExporting.notify_all();

    auto guard = qScopeGuard([this] { 
        _isExporting = false;   
        _isExporting.notify_all();
        _exporter = nullptr;
    });

    QFile file(path);
    QTextStream stream(&file);

    if (auto db = _db.lock()) {
        if (type == "CSV") {
            _exporter = std::make_unique<CSVDataExport>(db, stream);
        }
        else if (type == "JSON") {
            _exporter = std::make_unique<JSONDataExport>(db, stream);
        }
        else if (type == "SQL Insert") {
            _exporter = std::make_unique<SQLInsertDataExport>(db, stream);
        }
        else if (type == "SQL Update") {
            _exporter = std::make_unique<SQLUpdateDataExport>(db, stream);
        }
    }
    else {
        throw std::runtime_error("Unable to access database for export.");
    }

    if (_exporter == nullptr) {
        throw std::runtime_error("Unable to export type.");
    }

    _percent = 0.0f;

    _exporter->onProgressed([this](size_t start, size_t end) {

        float next_percent = float(start) / float(end);

        if (next_percent > (_percent + 0.01f)) {
            QMetaObject::invokeMethod(this, [ this] {
                ui->progressBar->setValue(_percent * 100);
            });
            _percent = next_percent;
        }
    });

    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        _exporter->exportTo();
    }
    else {
        throw std::runtime_error("Unable to write file.");
    }
}
