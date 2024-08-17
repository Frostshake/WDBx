#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <filesystem>
#include <string>
#include <WDBReader/Utility.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class OpenDialog; }
QT_END_NAMESPACE

struct OpenInfo {
public:
    std::filesystem::path path;
    std::string locale;
    std::string filesystem;
    std::string name;
    WDBReader::GameVersion version;
    std::filesystem::path defs_path;
};

class OpenDialog : public QDialog
{
    Q_OBJECT

public:
    OpenDialog(QWidget *parent = nullptr);
    ~OpenDialog();

signals:
    void chosen(OpenInfo info);

protected:
    void autoDetect();
    void validateOpen();
    bool handleDirPick(QLineEdit* editor);

private:
    Ui::OpenDialog *ui;
};
#endif // OPENDIALOG_H
