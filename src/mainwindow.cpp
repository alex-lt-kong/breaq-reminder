#include "mainwindow.h"
#include "dialog-editnotes.h"
#include "ui_mainwindow.h"

#include <QAudioDevice>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QtCore>
#include <QtGui>
#include <spdlog/spdlog.h>

#define ORGANIZATION_NAME "ak-studio"
#define APPLICATION_NAME "breakq-reminder"

void MainWindow::initMediaPlayer()
{
    player = new QMediaPlayer(this);
    if (!player->isAvailable()) {
        SPDLOG_INFO("player->isAvailable() == false, returning");
        return;
    }

    connect(player,
            &QMediaPlayer::errorOccurred,
            this,
            [this](QMediaPlayer::Error error, const QString &errorString) -> void {
                SPDLOG_ERROR("QMediaPlayer::errorOccurred, errorString: {}",
                             errorString.toStdString());
            });
    auto audioOutput = new QAudioOutput();

    audioOutput->setDevice(QMediaDevices::defaultAudioOutput());
    SPDLOG_INFO("audioOutput->device().description(): {}",
                audioOutput->device().description().toStdString());

    player->setAudioOutput(audioOutput);
    /*
    player->setSource(QUrl::fromLocalFile(
        "/home/mamsds/Documents/data1/repos/breaq-reminder/resources/notification.mp3"));
    */
    player->setSource(QUrl("qrc:/notification.mp3"));
    audioOutput->setVolume(50);
    // SPDLOG_INFO("player->errorString(): {}", player->errorString().toStdString());
    // SPDLOG_INFO("player->hasAudio(): {}", player->hasAudio());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tmrBg = new QTimer(this);
    connect(tmrBg, SIGNAL(timeout()), this, SLOT(backgroundLoop()));
    tmrFg = new QTimer();
    connect(tmrFg, SIGNAL(timeout()), this, SLOT(foregroundLoop()));

    initTrayMenu();
    initBackgroundCycle();
    // initBackgroundCycle() calls loadSettings();

    ui->plainTextEdit->setTabStopDistance(20);

    ui->textEditMarkdownDisp->document()->setIndentWidth(15);
    ui->textEditMarkdownDisp->document()->setBaselineOffset(-10);
    /*
    qreal lineSpacing = 10;
    QTextCursor textCursor = ui->textEditMarkdownDisp->textCursor();
    QTextBlockFormat *newFormat = new QTextBlockFormat();
    textCursor.clearSelection();
    textCursor.select(QTextCursor::Document);
    newFormat->setLineHeight(lineSpacing, QTextBlockFormat::ProportionalHeight);
    textCursor.setBlockFormat(*newFormat);*/

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint
                   | Qt::Dialog | Qt::Tool);

    initMediaPlayer();

    //    QApplication::setFont(QFont("Noto Sans CJK SC Medium", 9));

    //InitWindowWidth = this->size().width();
    //InitWindowHeight = this->size().height();
}

void MainWindow::on_actionExitTriggered()
{
    tmrBg->stop();
    tmrFg->stop();
    QApplication::quit();
}

void MainWindow::on_BackgroundCycleDurationChanged()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    if (actionBgLen1->isChecked())
        settings.setValue("BackgroundCycleDurationMin", 1);
    else if (actionBgLen15->isChecked())
        settings.setValue("BackgroundCycleDurationMin", 15);
    else if (actionBgLen20->isChecked())
        settings.setValue("BackgroundCycleDurationMin", 20);
    else
        UpdatTrayMenuCheckStatus();
    loadSettings();
}

void MainWindow::on_ScreenIndexChanged()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    for (int i = 0; i < actionScreens.size(); ++i) {
        if (actionScreens[i]->isChecked())
            settings.setValue("ScreenIndex", i);
    }
    loadSettings();
}

void MainWindow::on_SkipBreaksChanged()
{
    breaks_to_skip = 0;
    for (int i = 0; i < action_break_counts.size(); ++i) {
        if (action_break_counts[i]->isChecked()) {
            breaks_to_skip = i;
            break;
        }
    }
}

void MainWindow::on_ForegroundCycleDurationChanged()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    if (actionFgLen20->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 20);
    else if (actionFgLen40->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 40);
    else if (actionFgLen60->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 60);
    else if (actionFgLen120->isChecked())
        settings.setValue("ForegroundCycleDurationSec", 120);
    loadSettings();
}

void MainWindow::initTrayMenu()
{
    setWindowIcon(QIcon(":/leaf.ico"));
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/leaf.ico"));
    trayIcon->setToolTip("Breaq Reminder");

    trayIcon->show();

    menuTray = new QMenu(this);

    actionEditNotes = menuTray->addAction("Edit Notes");
    connect(actionEditNotes, SIGNAL(triggered()), this, SLOT(on_actionEditNotesTriggered()));
    menuTray->addAction(actionEditNotes);

    actionRestoreWindow = menuTray->addAction("Resotre Window Position");
    actionRestoreWindow->setCheckable(true);
    connect(actionRestoreWindow, SIGNAL(triggered()), this, SLOT(on_actionRestoreWindowTriggered()));
    menuTray->addAction(actionRestoreWindow);

    //  actionSkipBreak = menuTray->addAction("Skip Breaks");
    ag_skip_breaks = new QActionGroup(this);
    menu_skip_breaks = menuTray->addMenu("Skip Breaks");
    constexpr size_t max_breaks = 6;
    for (int i = 0; i < max_breaks; ++i) {
        action_break_counts.emplace_back(menu_skip_breaks->addAction(QString::number(i)));
        action_break_counts[i]->setCheckable(true);
        ag_skip_breaks->addAction(action_break_counts[i]);
        connect(action_break_counts[i], SIGNAL(changed()), this, SLOT(on_SkipBreaksChanged()));
    }
    action_break_counts[0]->setChecked(true);

    ag_screen_selection = new QActionGroup(this);
    menuScreenSelection = menuTray->addMenu("Select screen");
    auto allScreens = QGuiApplication::screens();
    for (size_t i = 0; i < allScreens.size(); ++i) {
        auto screenStr = QString("[%1]: %2, %3")
                             .arg(QString::number(i),
                                  allScreens[i]->manufacturer().length() > 0
                                      ? allScreens[i]->manufacturer()
                                      : "Unknown manufacturer",
                                  allScreens[i]->name());
        actionScreens.emplace_back(menuScreenSelection->addAction(screenStr));
        actionScreens[i]->setCheckable(true);
        ag_screen_selection->addAction(actionScreens[i]);
        connect(actionScreens[i], SIGNAL(changed()), this, SLOT(on_ScreenIndexChanged()));
    }

    menuBreakInverval = menuTray->addMenu("Break Interval (Min)");
    actionBgLen1 = menuBreakInverval->addAction("1");
    actionBgLen1->setCheckable(true);
    actionBgLen15 = menuBreakInverval->addAction("15");
    actionBgLen15->setCheckable(true);
    actionBgLen20 = menuBreakInverval->addAction("20");
    actionBgLen20->setCheckable(true);
    ag_background_cycle_duration_min = new QActionGroup(this);
    ag_background_cycle_duration_min->addAction(actionBgLen1);
    ag_background_cycle_duration_min->addAction(actionBgLen15);
    ag_background_cycle_duration_min->addAction(actionBgLen20);
    connect(actionBgLen1, SIGNAL(changed()), this, SLOT(on_BackgroundCycleDurationChanged()));
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
    actiongroupForegroundCycleDurationSec = new QActionGroup(this);
    actiongroupForegroundCycleDurationSec->addAction(actionFgLen20);
    actiongroupForegroundCycleDurationSec->addAction(actionFgLen40);
    actiongroupForegroundCycleDurationSec->addAction(actionFgLen60);
    actiongroupForegroundCycleDurationSec->addAction(actionFgLen120);
    connect(actionFgLen20, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));
    connect(actionFgLen40, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));
    connect(actionFgLen20, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));
    connect(actionFgLen120, SIGNAL(changed()), this, SLOT(on_ForegroundCycleDurationChanged()));

    actionExit = menuTray->addAction("Exit");
    // connect(actionExit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));
    connect(actionExit, &QAction::triggered, this, &MainWindow::on_actionExitTriggered);
    menuTray->addAction(actionExit);

    UpdatTrayMenuCheckStatus();
    trayIcon->setContextMenu(menuTray);
}

void MainWindow::on_actionRestoreWindowTriggered()
{
    IsRestoreWindow = !IsRestoreWindow;
    actionRestoreWindow->setChecked(IsRestoreWindow);
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue("RestoreWindow", IsRestoreWindow);
}

void MainWindow::UpdatTrayMenuCheckStatus()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    auto t = settings.value("BackgroundCycleDurationMin").toInt();
    if (t == 1)
        actionBgLen1->setChecked(true);
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

    t = settings.value("ScreenIndex").toInt();
    if (t >= actionScreens.size())
        t = 0;
    actionScreens[t]->setChecked(true);
}

void MainWindow::setWindowSizeAndLocation()
{
    // It appears that this function only needs to be called once to fix the size of the window.
    // this->setFixedSize(InitWindowWidth, InitWindowHeight);
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    std::vector<int> WindowXOffSets;
    std::vector<int> WindowYOffSets;
    SPDLOG_INFO("Called, allScreens:");
    auto allScreens = QGuiApplication::screens();
    for (size_t i = 0; i < allScreens.size(); ++i) {
        WindowXOffSets.push_back(settings.value(QString("WindowXOffSet_%1").arg(i), 0).toInt());
        WindowYOffSets.push_back(settings.value(QString("WindowYOffSet_%1").arg(i), 0).toInt());
        SPDLOG_INFO("[{}] selected: {}, manufacturer(): {}, model: {}, name: {}, WindowXOffSets: "
                    "{}, WindowXOffSets: {}",
                    i,
                    screenIdx == i,
                    allScreens[i]->manufacturer().toStdString(),
                    allScreens[i]->model().toStdString(),
                    allScreens[i]->name().toStdString(),
                    WindowXOffSets[i],
                    WindowYOffSets[i]);
        settings.setValue(QString("WindowXOffSet_%1").arg(i), WindowXOffSets[i]);
        settings.setValue(QString("WindowYOffSet_%1").arg(i), WindowYOffSets[i]);
    }
    if (screenIdx >= allScreens.length()) {
        screenIdx = 0;
    }
    // Per Qt https://doc.qt.io/qt-6/qscreen.html#availableGeometry-prop
    // availableGeometry() will not be useful if X11 system has more than one monitor connected

    SPDLOG_INFO("availableGeometry(): {}x{}, availableSize(): {}x{}",
                allScreens[screenIdx]->availableGeometry().width(),
                allScreens[screenIdx]->availableGeometry().height(),
                allScreens[screenIdx]->availableSize().width(),
                allScreens[screenIdx]->availableSize().height());
    auto screenWidth = allScreens[screenIdx]->availableGeometry().width();
    auto screenHeight = allScreens[screenIdx]->availableGeometry().height();
    auto screenX = allScreens[screenIdx]->availableGeometry().x();
    auto screenY = allScreens[screenIdx]->availableGeometry().y();
    auto windowX = (screenWidth - this->size().width() + WindowXOffSets[screenIdx]);
    auto windowY = (screenHeight - this->size().height() + WindowYOffSets[screenIdx]);

    setGeometry(screenX + windowX, screenY + windowY, this->size().width(), this->size().height());
}

MainWindow::~MainWindow()
{
    for (int i = 0; i < actionScreens.size(); ++i) {
        delete actionScreens[i];
    }
    // delete actiongroupForegroundCycleDurationSec;
    delete menuTray;
    // delete trayIcon;
    delete ui;
}

void MainWindow::backgroundLoop()
{
    int t = (background_cycle_duration_min * 60 - background_cycle_sec_count + 59) / 60;
    trayIcon->setToolTip("Breaq Reminder\n" + QString::number(t) + " minute" + (t != 1 ? "s" : "")
                         + " before the next break");
    background_cycle_sec_count++;

    if (background_cycle_sec_count == background_cycle_duration_min * 60 - 10)
        foregroundLoopNotification();
    else if (background_cycle_sec_count >= background_cycle_duration_min * 60)
        initForegroundCycle();
}

void MainWindow::foregroundLoop()
{
    foreground_sec_count++;
    if (foreground_sec_count == 1) {
        ui->lblReminderText->setText(
            "Time to have a " + QString::number(foreground_cycle_duration_sec) + "-second break!");
        ui->tabWidget->setCurrentIndex(0);
    }

    if (breaks_to_skip > 0) {
        initBackgroundCycle();
    }

    if (foreground_sec_count <= foreground_cycle_duration_sec) {
        ui->progressBar->setValue(foreground_sec_count * 100.0 / foreground_cycle_duration_sec);
        ui->btnGo->setText(QString::number(foreground_sec_count) + "/"
                           + QString::number(foreground_cycle_duration_sec));
    }

    if (foreground_sec_count >= foreground_cycle_duration_sec) {
        tmrFg->stop();
        SPDLOG_INFO("tmrFg->stop()ed");
        // We need the !ui->btnGo->isEnabled() part as users can change\
        // foreground_cycle_duration_sec during foreground cycle

        // SPDLOG_INFO("player->hasAudio()ing");
        if (player->isAvailable()) {
            player->play();
        } else
            SPDLOG_WARN("player->isAvailable() is false, skipped play()ing");

        // ui->btnGo->setEnabled(true);
        ui->btnRestart->setEnabled(true);
        ui->btnGo->setText("Go!");
    }

    if (foreground_sec_count % 10 == 0 && IsRestoreWindow) {
        setWindowSizeAndLocation();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    background_cycle_duration_min = settings.value("BackgroundCycleDurationMin").toInt();
    foreground_cycle_duration_sec = settings.value("ForegroundCycleDurationSec").toInt();
    screenIdx = settings.value("ScreenIndex", 0).toInt();
    if (screenIdx >= QGuiApplication::screens().size()) {
        screenIdx = 0;
    }
    IsRestoreWindow = settings.value("RestoreWindow", true).toBool();
    actionRestoreWindow->setChecked(IsRestoreWindow);

    resize(settings.value("MainWindowWidth", 354).toInt(),
           settings.value("MainWindowHeight", 245).toInt());

    UpdatTrayMenuCheckStatus();
    ui->plainTextEdit->setPlainText(settings.value("Notes").toString());
}

void MainWindow::initBackgroundCycle()
{
    SPDLOG_INFO("Called");
    loadSettings();
    background_cycle_sec_count = 0;
    this->hide();
    // tmrFg will be stopped by itself, but we will also stop() it here
    tmrFg->stop();
    tmrBg->start(1000);
    SPDLOG_INFO("tmrBg->start()ed, background_cycle_duration_min: {}",
                background_cycle_duration_min);
    // ui->btnGo->setEnabled(false);
    ui->btnRestart->setEnabled(false);
}

void MainWindow::foregroundLoopNotification()
{
    SPDLOG_INFO("Called");
    trayIcon->showMessage("Breaq Reminder", "Time to have a break!", QSystemTrayIcon::NoIcon, 3500);
    // The original version is QSystemTrayIcon::Information, which will cause this bug:
    // https://stackoverflow.com/questions/45827951/missing-file-icon-is-shown-in-the-system-tray-when-running-showmessage-with-no
}

void MainWindow::initForegroundCycle()
{
    SPDLOG_INFO("Called");
    loadSettings();
    foreground_sec_count = 0;
    background_cycle_sec_count = 0;
    tmrBg->stop();

    if (breaks_to_skip > 0) {
        qDebug() << "breaks_to_skip: " << breaks_to_skip
                 << ", this foregound cycle will be skipped";
        --breaks_to_skip;
        for (int i = 0; i < action_break_counts.size(); ++i) {
            if (i == breaks_to_skip) {
                action_break_counts[i]->setChecked(true);
                break;
            }
        }
        initBackgroundCycle();
    } else {
        tmrFg->start(1000);
        if (actionRestoreWindow->isChecked()) {
            setWindowSizeAndLocation();
        }
        // ui->btnGo->setEnabled(false);
        ui->progressBar->setValue(0);
        ui->btnRestart->setEnabled(false);
        this->show();
    }
}

void MainWindow::on_actionEditNotesTriggered()
{
    dialogEditNotes myEN;
    myEN.exec();
}

void MainWindow::on_plainTextEdit_textChanged()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue("Notes", ui->plainTextEdit->toPlainText());
    ui->textEditMarkdownDisp->setMarkdown(ui->plainTextEdit->toPlainText());
}

void MainWindow::on_pushButtonClicked()
{
    dialogEditNotes myEN;
    myEN.exec();
}

void MainWindow::on_btnGo_clicked()
{
    {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        settings.setValue("MainWindowWidth", size().width());
        settings.setValue("MainWindowHeight", size().height());
    }
    initBackgroundCycle();
}

void MainWindow::on_btnRestart_clicked()
{
    {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        settings.setValue("MainWindowWidth", size().width());
        settings.setValue("MainWindowHeight", size().height());
    }
    initForegroundCycle();
}
