#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QScreen>
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
    void on_actionEditNotesTriggered();
    void on_actionRestoreWindowTriggered();
    void on_actionExitTriggered();
    void on_ForegroundCycleDurationChanged();
    void on_BackgroundCycleDurationChanged();
    void on_plainTextEdit_textChanged();
    void on_pushButtonClicked();
    void on_btnGo_clicked();
    void on_btnRestart_clicked();
    void on_ScreenIndexChanged();
    void on_SkipBreaksChanged();

private:
    Ui::MainWindow *ui;
    QTimer *tmrBg;
    QTimer *tmrFg;

    void initForegroundCycle();
    void initBackgroundCycle();
    void foregroundLoopNotification();

    void initTrayMenu();
    void setWindowSizeAndLocation();
    void UpdatTrayMenuCheckStatus();
    void loadSettings();
    QString secondsToString(qint64 seconds);

    QSystemTrayIcon *trayIcon;

    QMediaPlayer *player;

    int background_cycle_sec_count, foreground_sec_count;
    int background_cycle_duration_min = 20;
    int foreground_cycle_duration_sec = 5;
    bool IsRestoreWindow;

    int screenIdx;
    int breaks_to_skip = 0;

    QMenu *menuTray;
    QMenu *menu_skip_breaks, *menuFgLengthSettings, *menuBreakInverval, *menuScreenSelection;

    QAction *actionEditNotes, *actionSkipBreak, *actionRestoreWindow, *actionExit;

    QActionGroup *ag_skip_breaks;
    std::vector<QAction *> action_break_counts;

    QActionGroup *ag_background_cycle_duration_min;
    QAction *actionBgLen1, *actionBgLen15, *actionBgLen20;

    QActionGroup *actiongroupForegroundCycleDurationSec;
    QAction *actionFgLen20, *actionFgLen40, *actionFgLen60, *actionFgLen120;

    QActionGroup *ag_screen_selection;
    std::vector<QAction *> actionScreens;
};

#endif // MAINWINDOW_H
