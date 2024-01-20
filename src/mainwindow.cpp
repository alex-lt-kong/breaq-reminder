#include "mainwindow.h"
#include <QtCore>
#include <QtGui>
#include "dialog-editnotes.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tmrBg = new QTimer(this);
    connect(tmrBg, SIGNAL(timeout()), this, SLOT(backgroundLoop()));
    tmrFg = new QTimer();
    connect(tmrFg, SIGNAL(timeout()), this, SLOT(foregroundLoop()));

    initTrayMenu();
    initBackgroundCycle();

    QSettings settings("ak-studio", "q-break-reminder");
    ui->plainTextEdit->setPlainText(settings.value("Notes").toString());
    ui->textEditMarkdownDisp->document()->setIndentWidth(10);

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint
                   | Qt::Dialog | Qt::Tool);

    player = new QMediaPlayer(this);
    //player->setVolume(50);
    player->setSource(QUrl("qrc:/Notification.mp3"));

//    QApplication::setFont(QFont("Noto Sans CJK SC Medium", 9));

    InitWindowWidth = this->size().width();
    InitWindowHeight = this->size().height();
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_BackgroundCycleDurationChanged()
{
    QSettings settings("ak-studio", "q-break-reminder");
    if (actionBgLen1->isChecked())
        settings.setValue("BackgroundCycleDurationMin", 1);
    else if (actionBgLen10->isChecked())
        settings.setValue("BackgroundCycleDurationMin", 10);
    else if (actionBgLen15->isChecked())
        settings.setValue("BackgroundCycleDurationMin", 15);
    else if (actionBgLen20->isChecked())
        settings.setValue("BackgroundCycleDurationMin", 20);
    else
        UpdateDurationCheckStatus();
    qDebug() << "BackgroundCycleDurationMin changed to:"
             << settings.value("BackgroundCycleDurationMin").toInt();
    loadSettings();
}

void MainWindow::on_ForegroundCycleDurationChanged()
{
    QSettings settings("ak-studio", "q-break-reminder");
    if (actionFgLen20->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 20);
    else if (actionFgLen40->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 40);
    else if (actionFgLen60->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 60);
    else if (actionFgLen120->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 120);
    else
        UpdateDurationCheckStatus();
    //    qDebug() << "on_ForegroundCycleDurationChanged: " << settings.value("ForegroundCycleDurationSec").toInt();
    loadSettings();
}

void MainWindow::initTrayMenu()
{
    setWindowIcon(QIcon(":/leaf.png"));
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/leaf.png"));
    trayIcon->setToolTip("QBreak Reminder");

    trayIcon->show();

    menuTray = new QMenu(this);

    actionEditNotes = menuTray->addAction("Edit Notes");
    connect(actionEditNotes, SIGNAL(triggered()), this, SLOT(on_actionEditNotesTriggered()));
    menuTray->addAction(actionEditNotes);

    actionSkipBreak = menuTray->addAction("Skip Next Break");
    actionSkipBreak->setCheckable(true);
    connect(actionSkipBreak, SIGNAL(triggered()), this, SLOT(on_actionSkipBreakTriggered()));
    menuTray->addAction(actionSkipBreak);

    menuBreakInverval = menuTray->addMenu("Break Interval (Min)");
    actionBgLen1 = menuBreakInverval->addAction("1");
    actionBgLen1->setCheckable(true);
    actionBgLen10 = menuBreakInverval->addAction("10");
    actionBgLen10->setCheckable(true);
    actionBgLen15 = menuBreakInverval->addAction("15");
    actionBgLen15->setCheckable(true);
    actionBgLen20 = menuBreakInverval->addAction("20");
    actionBgLen20->setCheckable(true);
    actiongroupBgLength = new QActionGroup(this);
    actiongroupBgLength->addAction(actionBgLen1);
    actiongroupBgLength->addAction(actionBgLen10);
    actiongroupBgLength->addAction(actionBgLen15);
    actiongroupBgLength->addAction(actionBgLen20);
    connect(actionBgLen1, SIGNAL(changed()), this, SLOT(on_BackgroundCycleDurationChanged()));
    connect(actionBgLen10, SIGNAL(changed()), this, SLOT(on_BackgroundCycleDurationChanged()));
    connect(actionBgLen15, SIGNAL(changed()), this, SLOT(on_BackgroundCycleDurationChanged()));
    connect(actionBgLen20, SIGNAL(changed()), this, SLOT(on_BackgroundCycleDurationChanged()));

    menuFgLengthSettings = menuTray->addMenu("Break Length (Sec)");
    actionFgLen20 = menuFgLengthSettings->addAction("20");
    actionFgLen20->setCheckable(true);
    actionFgLen40 = menuFgLengthSettings->addAction("40");
    actionFgLen40->setCheckable(true);
    actionFgLen60 = menuFgLengthSettings->addAction("60");
    actionFgLen60->setCheckable(true);
    actionFgLen120 = menuFgLengthSettings->addAction("120");
    actionFgLen120->setCheckable(true);
    actiongroupFgLength = new QActionGroup(this);
    actiongroupFgLength->addAction(actionFgLen20);
    actiongroupFgLength->addAction(actionFgLen40);
    actiongroupFgLength->addAction(actionFgLen60);
    actiongroupFgLength->addAction(actionFgLen120);
    connect(actionFgLen20, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));
    connect(actionFgLen40, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));
    connect(actionFgLen20, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));
    connect(actionFgLen120, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));

    actionExit = menuTray->addAction("Exit");
    connect(actionExit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));
    menuTray->addAction(actionExit);

    UpdateDurationCheckStatus();
    trayIcon->setContextMenu(menuTray);
}

void MainWindow::UpdateDurationCheckStatus()
{
    QSettings settings("ak-studio", "q-break-reminder");
    auto t = settings.value("BackgroundCycleDurationMin").toInt();
    if (t == 1)
        actionBgLen1->setChecked(true);
    else if (t == 10)
        actionBgLen10->setChecked(true);
    else if (t == 15)
        actionBgLen15->setChecked(true);
    else
        actionBgLen20->setChecked(true);

    t = settings.value("ForegroundCycleDurationSec").toInt();
    if (t == 20)
        actionFgLen20->setChecked(true);
    else if (t == 40)
        actionFgLen40->setChecked(true);
    else if (t == 60)
        actionFgLen60->setChecked(true);
    else
        actionFgLen120->setChecked(true);
}

void MainWindow::setWindowSizeAndLocation()
{
    int x, y;
    int screenWidth;
    int screenHeight;

    // It appears that this function only needs to be called once to fix the size of the window.
    this->setFixedSize(InitWindowWidth, InitWindowHeight);

    screenWidth = QGuiApplication::primaryScreen()->availableGeometry().width();
    screenHeight = QGuiApplication::primaryScreen()->availableGeometry().height();

    x = (screenWidth - this->size().width());
    y = (screenHeight - this->size().height());

    // https://stackoverflow.com/questions/3203095/display-window-full-screen-on-secondary-monitor-using-qt
    this->windowHandle()->setScreen(QGuiApplication::primaryScreen());
    setGeometry(x, y, this->size().width(), this->size().height());
}

MainWindow::~MainWindow()
{
    delete actiongroupFgLength;
    delete menuTray;
    delete trayIcon;
    delete ui;
}

void MainWindow::backgroundLoop()
{
    int t = (background_cycle_duration_min * 60 - BgCount + 59) / 60;
    trayIcon->setToolTip("QBreak Reminder\n" + QString::number(t) + " minute" + (t != 1 ? "s" : "") + " before the next break");
    BgCount++;

    if (BgCount == background_cycle_duration_min * 60 - 10)
        foregroundLoopNotification();
    else if (BgCount >= background_cycle_duration_min * 60)
        initForegroundCycle();
}

void MainWindow::foregroundLoop()
{
    foreground_sec_count++;
    ui->progressBar->setValue(foreground_sec_count * 100.0 / foreground_cycle_duration_sec);
    ui->btnGo->setText(QString::number(foreground_sec_count) + "/"
                       + QString::number(foreground_cycle_duration_sec));
    ui->lblText->setText("Time to have a " + QString::number(foreground_cycle_duration_sec)
                         + "-second break!");

    if (IsBreakSkipped)
        this->initBackgroundCycle();

    if (foreground_sec_count >= foreground_cycle_duration_sec) {
        tmrFg->stop();
        player->play();

        ui->btnGo->setEnabled(true);
        ui->btnRestart->setEnabled(true);
        ui->btnGo->setText("Go!");
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("ak-studio", "q-break-reminder");
    background_cycle_duration_min = settings.value("BackgroundCycleDurationMin").toInt();
    foreground_cycle_duration_sec = settings.value("ForegroundCycleDurationSec").toInt();
}

void MainWindow::initBackgroundCycle()
{
    loadSettings();
    IsBreakSkipped = false;
    actionSkipBreak->setChecked(IsBreakSkipped);
    BgCount = 0;
    tmrBg->start(1000);
    tmrFg->stop();
    this->hide();
    ui->btnGo->setEnabled(false);
    ui->btnRestart->setEnabled(false);
}

void MainWindow::foregroundLoopNotification()
{
    trayIcon->showMessage("QBreak Reminder", "Time to have a break!", QSystemTrayIcon::NoIcon, 3500);
    // The original version is QSystemTrayIcon::Information, which will cause this bug:
    // https://stackoverflow.com/questions/45827951/missing-file-icon-is-shown-in-the-system-tray-when-running-showmessage-with-no
}

void MainWindow::initForegroundCycle()
{
    loadSettings();
    foreground_sec_count = 0;
    BgCount = 0;
    IsMouseReleased = true;
    QSettings settings("ak-studio", "q-break-reminder");
    ui->plainTextEdit->setPlainText(settings.value("Notes").toString());
    tmrBg->stop();

    if (IsBreakSkipped) {
        qDebug() << "this break should be skipped, starting backgound cycle again.";
        this->initBackgroundCycle();
    } else {
        this->show();
        tmrFg->start(1000);
        setWindowSizeAndLocation();
        ui->btnGo->setEnabled(false);
        ui->btnRestart->setEnabled(false);
    }
}

void MainWindow::on_actionEditNotesTriggered()
{
    dialogEditNotes myEN;
    myEN.exec();
}

void MainWindow::on_actionSkipBreakTriggered()
{
    IsBreakSkipped = true;
    actionSkipBreak->setChecked(IsBreakSkipped);
}

void MainWindow::on_plainTextEdit_textChanged()
{
    QSettings settings("ak-studio", "q-break-reminder");
    settings.setValue("Notes", ui->plainTextEdit->toPlainText());
    ui->textEditMarkdownDisp->setMarkdown(ui->plainTextEdit->toPlainText());
}

void MainWindow::on_pushButtonClicked()
{
    dialogEditNotes myEN;
    myEN.exec();
}

void MainWindow::on_btnIconClicked()
{
    IsMouseReleased = true;
}

void MainWindow::on_btnGo_clicked()
{
    initBackgroundCycle();
}


void MainWindow::on_btnRestart_clicked()
{
    initForegroundCycle();
}

