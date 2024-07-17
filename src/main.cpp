#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    /*
    QFont font = w.font();
    if (QGuiApplication::platformName() == "windows") {
        font.setFamily("Microsoft Yahei");
    }
    QApplication::setFont(font);
    */
    return a.exec();
}
