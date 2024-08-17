#include "mainwindow.h"

#include <QApplication>
#include <QStyleHints>
#include <QStyle>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (a.styleHints()->colorScheme() == Qt::ColorScheme::Dark) {
        // the default theme isnt great in dark mode, fusion looks slightly better.
        a.setStyle("fusion");
    }

    MainWindow w;
    w.show();
    return a.exec();
}
