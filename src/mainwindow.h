#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QDesktopWidget>
#include <QDateTime>
#include <QMediaPlayer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();    


private slots:
    void backgroundLoop();
    void foregroundLoop();
    void on_btnOK_clicked();
    void on_actionEditNotes_triggered();
    void on_actionSkipBreak_triggered();
    void on_actionExit_triggered();
    void on_BgLength_changed();
    void on_FgLength_changed();
    void on_plainTextEdit_textChanged();
    void on_pushButton_clicked();

    void on_btnIcon_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *tmrBg;
    QTimer *tmrFg;

    void initBackgroundLoop();
    void foregroundLoopNotification();
    void initForegroundLoop();
    void initTrayMenu();
    void setWindowSizeAndLocation();
    void DetermineMenuCheckStatus();
    void loadSettings();
    QString secondsToString(qint64 seconds);

    QSystemTrayIcon *trayIcon;

    QMediaPlayer *player;

    int BgCount, FgCount;
    int BgLength = 20, FgLength = 5;
    bool IsBreakSkipped;
    bool IsMouseReleased;
    int InitWindowWidth, InitWindowHeight;

    QDateTime startupTime;

    QMenu *menuTray;
    QMenu *menuFgLengthSettings, *menuBreakInverval;
    QAction *actionEditNotes, *actionSkipBreak, *actionExit;
    QActionGroup *actiongroupBgLength;
    QAction *actionBgLen1, *actionBgLen10, *actionBgLen15, *actionBgLen20;
    QActionGroup *actiongroupFgLength;
    QAction *actionFgLen20, *actionFgLen40, *actionFgLen60, *actionFgLen120;


};

#endif // MAINWINDOW_H
