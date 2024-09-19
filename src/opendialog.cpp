#include "opendialog.h"
#include "./ui_opendialog.h"
#include "settings.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <WDBReader/Detection.hpp>

OpenDialog::OpenDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OpenDialog)
{
    ui->setupUi(this);

    ui->lineEditDirectory->setText(Settings::get(config::app::last_save_location));
    ui->lineEditDefs->setText(Settings::get(config::app::definitions_location));

    ui->comboBoxFilesystem->addItem("CASC");
    ui->comboBoxFilesystem->addItem("MPQ");
    ui->comboBoxFilesystem->addItem("Native");

    {
        QRegularExpression re("^\\d+\\.\\d+\\.\\d+\\.\\d+$");
        QRegularExpressionValidator* validator = new QRegularExpressionValidator(re, this);
        ui->comboBoxVersion->lineEdit()->setValidator(validator);
        ui->comboBoxVersion->lineEdit()->setPlaceholderText("X.X.X.XXXXX");
    }

    {
        ui->comboBoxLocale->addItem("enUS");
        ui->comboBoxLocale->addItem("enGB");
        ui->comboBoxLocale->addItem("koKR");
        ui->comboBoxLocale->addItem("frFR");
        ui->comboBoxLocale->addItem("deDE");
        ui->comboBoxLocale->addItem("zhCN");
        ui->comboBoxLocale->addItem("zhTW");
        ui->comboBoxLocale->addItem("esES");
        ui->comboBoxLocale->addItem("esMX");
        ui->comboBoxLocale->addItem("ruRU");
        ui->comboBoxLocale->addItem("ptPT");
        ui->comboBoxLocale->addItem("ptBR");
        ui->comboBoxLocale->addItem("itIT");
    }

    connect(ui->lineEditDirectory, &QLineEdit::textChanged, this, &OpenDialog::validateOpen);
    connect(ui->comboBoxVersion, &QComboBox::currentTextChanged, this, &OpenDialog::validateOpen);
    connect(ui->comboBoxVersion, &QComboBox::currentIndexChanged, this, &OpenDialog::validateOpen);
    connect(ui->comboBoxLocale, &QComboBox::currentTextChanged, this, &OpenDialog::validateOpen);

    connect(ui->comboBoxVersion, &QComboBox::currentIndexChanged, [&](int index) {
        if (index >= 0) {
            auto locale_str = ui->comboBoxVersion->currentData().toStringList()[1];
            for (auto i = 0; i < ui->comboBoxLocale->count(); i++) {
                if (ui->comboBoxLocale->itemText(i) == locale_str) {
                    ui->comboBoxLocale->setCurrentIndex(i);
                }
            }
        }
    });


    connect(ui->pushButtonPickDir, &QPushButton::pressed, [&]() {
        if (handleDirPick(ui->lineEditDirectory)) {
            autoDetect();
        }
    });

    connect(ui->pushButtonPickDefs, &QPushButton::pressed, [&]() {
        if(handleDirPick(ui->lineEditDefs)) {
            validateOpen();
        }
    });

    
    connect(ui->pushButtonCancel, &QPushButton::pressed, [&]() {
        reject(); 
    });

    connect(ui->pushButtonOpen, &QPushButton::pressed, [&]() {
        const auto path = ui->lineEditDirectory->text().toStdString();
        if (!std::filesystem::is_directory(path)) {
            QMessageBox::warning(this, "Error", "Directory doesnt exist.");
            return;
        }

        std::optional<WDBReader::GameVersion> parsed_ver;
        std::string name = "";

        if (ui->comboBoxVersion->currentIndex() >= 0) {
            // detected
            parsed_ver = WDBReader::GameVersion::fromString(
                ui->comboBoxVersion->currentData().toStringList()[0].toStdString()
            );

            name = ui->comboBoxVersion->currentData().toStringList()[2].toStdString();
        }
        else {
            // user chosen
            parsed_ver = WDBReader::GameVersion::fromString(
                ui->comboBoxVersion->currentText().toStdString()
            );
        }

        if (!parsed_ver.has_value()) {
            QMessageBox::warning(this, "Error", "Unable to parse version string.");
            return;
        }


        OpenInfo info;
        info.path = path;
        info.locale = ui->comboBoxLocale->currentText().toStdString();
        info.filesystem = ui->comboBoxFilesystem->currentText().toStdString();
        info.name = name;
        info.version = *parsed_ver;
        info.defs_path = ui->lineEditDefs->text().toStdString();

        Settings::set(config::app::last_save_location, ui->lineEditDirectory->text());
        Settings::set(config::app::definitions_location, ui->lineEditDefs->text());

        emit chosen(std::move(info));

        accept();
    });

    validateOpen();

    autoDetect();
}

OpenDialog::~OpenDialog()
{
    delete ui;
}

void OpenDialog::autoDetect()
{
    QString dir = ui->lineEditDirectory->text();
    bool found = false;

    ui->comboBoxVersion->clear();

    if (!dir.isEmpty()) {
        auto detected = WDBReader::Detector::all().detect(dir.toStdString());

        found = detected.size() > 0;

        for (const auto& install : detected) {
            const auto build = install.version;
            QString build_str = QString("%1.%2.%3.%4")
                .arg(build.expansion)
                .arg(build.major)
                .arg(build.minor)
                .arg(build.build);

            QStringList data{
                build_str,
                install.locales.size() > 0 ? QString::fromStdString(install.locales[0]) : "",
                QString::fromStdString(install.name)
            };

            QString name = build_str;
            if (install.name.length() > 0) {
                name += " [" + QString::fromStdString(install.name) + "]";
            }

            ui->comboBoxVersion->addItem(name, data);
        }

        if (found) {
            ui->comboBoxVersion->setCurrentIndex(0);
        }
    }

   
    if (found) {
        const auto dir_path = std::filesystem::path(dir.toStdString()) / "Data";
        bool found_mpq = false;

        if (std::filesystem::is_directory(dir_path)) {
            for (auto& it : std::filesystem::directory_iterator(dir_path)) {
                if (it.is_regular_file() && (it.path().extension() == ".MPQ" || it.path().extension() == ".mpq")) {
                    found_mpq = true;
                    break;
                }
            }
        }

        if (found_mpq) {
            dir += QDir::separator();
            dir += "Data";
            ui->lineEditDirectory->setText(dir);
            ui->comboBoxFilesystem->setCurrentIndex(1);
        }
        else {
            ui->comboBoxFilesystem->setCurrentIndex(0);
        }
    }
    else {
        //cannot find wow info, assume using native FS
        ui->comboBoxFilesystem->setCurrentIndex(2);
    }


    if (!found) {
        ui->comboBoxVersion->setEditText("");
    }

    validateOpen();
}

void OpenDialog::validateOpen()
{
    bool valid = true;
    
    valid = valid && ui->lineEditDirectory->text().length() > 0;
    valid = valid && ui->comboBoxVersion->currentText().length() > 0;
    valid = valid && ui->comboBoxLocale->currentText().length() > 0;
    valid = valid && ui->lineEditDefs->text().length() > 0;

    ui->pushButtonOpen->setDisabled(!valid);
}

bool OpenDialog::handleDirPick(QLineEdit* editor)
{
    QString dir = QFileDialog::getExistingDirectory(this,
        "Open Directory",
        editor->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!dir.isEmpty()) {
        editor->setText(dir);
        return true;
    }

    return false;
}
