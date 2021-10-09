#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

//    QFontDatabase::addApplicationFont(":/NotoSansCJKsc-Medium.otf"); This line would lead to a compilation failure. But the program CAN work correctly without it.

    QFont font = w.font();
    if (QGuiApplication::platformName() == "windows") {
        font.setFamily("Noto Sans CJK SC Medium");
    } else {
        font.setFamily("Noto Sans CJK SC Medium");
    }
    w.setFont(font);

    return a.exec();
}
