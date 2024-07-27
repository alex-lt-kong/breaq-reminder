#include "global_variables.h"
#include "logger.h"
#include "mainwindow.h"

#include <QApplication>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    init_logger();
    SPDLOG_INFO("BreaqReminder started (git commit: {})", GIT_COMMIT_HASH);
    SPDLOG_INFO("Settings are stored at: {}", settings.fileName().toStdString());
    QApplication a(argc, argv);
    MainWindow w;
    return a.exec();
}
