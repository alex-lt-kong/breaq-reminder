#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QFont font = w.font();
    if (QGuiApplication::platformName() == "windows") {
        font.setFamily("Microsoft Yahei");
    } else {
        font.setFamily("Noto Sans CJK SC Medium");
    }
    QApplication::setFont(font);

    return a.exec();
}
