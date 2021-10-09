#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialogeditnotes.h"
#include <QtGui>
#include <QtCore>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tmrBg = new QTimer(this);
    connect(tmrBg, SIGNAL(timeout()), this, SLOT(backgroundLoop()));
    tmrFg = new QTimer();
    connect(tmrFg, SIGNAL(timeout()), this, SLOT(foregroundLoop()));

    startupTime = QDateTime::currentDateTime();
    initTrayMenu();
    initBackgroundLoop();

    QSettings settings("MamsdsStudio", "MamsdsQBreakReminder");
    ui->plainTextEdit->setPlainText(settings.value("Notes").toString());

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::Dialog | Qt::Tool);

    ui->btnIcon->setIcon(QIcon(":/Leaf.png"));
//    ui->btnIcon->setFixedSize(64, 64);
//    ui->btnIcon->setIconSize(ui->btnIcon->size());

    player = new QMediaPlayer(this);
    player->setVolume(50);
    player->setMedia(QUrl("qrc:/Notification.mp3"));

//    QApplication::setFont(QFont("Noto Sans CJK SC Medium", 9));

    InitWindowWidth = this->size().width();
    InitWindowHeight = this->size().height();
}

QString MainWindow::secondsToString(qint64 seconds)
{
    const qint64 DAY = 86400;
    qint64 days = seconds / DAY;
    QTime t = QTime(0,0).addSecs(seconds % DAY);
    return QString::number(days) + " day" + (days != 1 ? "s, " : ", ")
          + QString::number(t.hour()) + " hour" + (t.hour() != 1 ? "s, " : ", ")
          + QString::number(t.minute()) + " minute" + (t.minute() != 1 ? "s and " : " and ")
          + QString::number(t.second()) + " second" + (t.second() != 1 ? "s" : "");
//  return QString("%1 days, %2 hours, %3 minutes, %4 seconds").arg(days).arg(t.hour()).arg(t.minute()).arg(t.second());
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_BgLength_changed()
{
    QSettings settings("MamsdsStudio", "MamsdsQBreakReminder");
    if (actionBgLen1->isChecked())
        settings.setValue("BgLength", 1);
    else if (actionBgLen10->isChecked())
        settings.setValue("BgLength", 10);
    else if (actionBgLen15->isChecked())
        settings.setValue("BgLength", 15);
    else if (actionBgLen20->isChecked())
        settings.setValue("BgLength", 20);
    else
        DetermineMenuCheckStatus();
    qDebug() << "on_BgLength_changed: " << settings.value("BgLength").toInt();
    loadSettings();
}

void MainWindow::on_FgLength_changed()
{
    QSettings settings("MamsdsStudio", "MamsdsQBreakReminder");
    if (actionFgLen20->isChecked())
        settings.setValue("FgLength", 20);
    else if (actionFgLen40->isChecked())
        settings.setValue("FgLength", 40);
    else if (actionFgLen60->isChecked())
        settings.setValue("FgLength", 60);
    else if (actionFgLen120->isChecked())
        settings.setValue("FgLength", 120);
    else
        DetermineMenuCheckStatus();
//    qDebug() << "on_FgLength_changed: " << settings.value("FgLength").toInt();
    loadSettings();
}

void MainWindow::initTrayMenu()
{
    QPixmap map = QPixmap(":/Leaf.png");
    setWindowIcon(QIcon(":/Leaf.png"));
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/Leaf.png"));
    trayIcon->setToolTip("Mamsds QBreak Reminder");

    trayIcon->show();

    menuTray = new QMenu(this);

    actionEditNotes = menuTray->addAction("Edit Notes");
    connect(actionEditNotes, SIGNAL(triggered()), this, SLOT(on_actionEditNotes_triggered()));
    menuTray->addAction(actionEditNotes);

    actionSkipBreak = menuTray->addAction("Skip Next Break");
    connect(actionSkipBreak, SIGNAL(triggered()), this, SLOT(on_actionSkipBreak_triggered()));
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
    connect(actionBgLen1, SIGNAL(changed()), this, SLOT(on_BgLength_changed()));
    connect(actionBgLen10, SIGNAL(changed()), this, SLOT(on_BgLength_changed()));
    connect(actionBgLen15, SIGNAL(changed()), this, SLOT(on_BgLength_changed()));
    connect(actionBgLen20, SIGNAL(changed()), this, SLOT(on_BgLength_changed()));

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
    connect(actionFgLen20, SIGNAL(changed()), this, SLOT(on_FgLength_changed()));
    connect(actionFgLen40, SIGNAL(changed()), this, SLOT(on_FgLength_changed()));
    connect(actionFgLen20, SIGNAL(changed()), this, SLOT(on_FgLength_changed()));
    connect(actionFgLen120, SIGNAL(changed()), this, SLOT(on_FgLength_changed()));

    actionExit = menuTray->addAction("Exit");
    connect(actionExit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()));
    menuTray->addAction(actionExit);

    DetermineMenuCheckStatus();
    trayIcon->setContextMenu(menuTray);
}

void MainWindow::DetermineMenuCheckStatus()
{
    QSettings settings("MamsdsStudio", "MamsdsQBreakReminder");
    int t = settings.value("BgLength").toInt();
    if (t == 1)
        actionBgLen1->setChecked(true);
    else if (t == 10)
        actionBgLen10->setChecked(true);
    else if (t == 15)
        actionBgLen15->setChecked(true);
    else
        actionBgLen20->setChecked(true);

    t = settings.value("FgLength").toInt();
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

    this->setFixedSize(InitWindowWidth, InitWindowHeight);  // It appears that this function only needs to be called once to fix the size of the window.

    screenWidth = QGuiApplication::primaryScreen()->availableGeometry().width();
    screenHeight = QGuiApplication::primaryScreen()->availableGeometry().height();

    x = (screenWidth - this->size().width());
    y = (screenHeight - this->size().height());

    setGeometry(x, y, this->size().width(), this->size().height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::backgroundLoop()
{
    int t = (BgLength * 60 - BgCount + 59) / 60;
    trayIcon->setToolTip("Mamsds QBreak Reminder\n" + QString::number(t) + " minute" + (t != 1 ? "s" : "") + " before the next break");
    BgCount++;

    if (BgCount == BgLength * 60 - 10)
        foregroundLoopNotification();
    else if (BgCount >= BgLength * 60)
        initForegroundLoop();
}

void MainWindow::foregroundLoop()
{
    FgCount++;
    ui->progressBar->setValue(FgCount * 100.0 / FgLength );
    ui->btnOK->setText(QString::number(FgCount) + "/" + QString::number(FgLength));
    ui->lblText->setText("Hi Pal, time to have a " + QString::number(FgLength) + "-second break! You have been using the computer for at least: " + secondsToString(startupTime.secsTo(QDateTime::currentDateTime())));

    if (IsBreakSkipped)
        this->initBackgroundLoop();

/*    if (!IsMouseReleased)
    {
        QCursor c = QCursor();
        c.setPos(mapToGlobal(QPoint(ui->btnIcon->x() + ui->btnIcon->size().width() / 2, ui->btnIcon->y() + ui->btnIcon->size().width() / 2)));
        setCursor(c);
    }*/

    if (FgCount >= FgLength)
    {
        tmrFg->stop();
        player->play();

        ui->btnOK->setEnabled(true);
        ui->btnOK->setText("Start!");
    }

}

void MainWindow::loadSettings()
{
    QSettings settings("MamsdsStudio", "MamsdsQBreakReminder");
    BgLength = settings.value("BgLength").toInt();
    FgLength = settings.value("FgLength").toInt();
}

void MainWindow::initBackgroundLoop()
{
    loadSettings();
    IsBreakSkipped = false;
    BgCount = 0;
    tmrBg->start(1000);
    tmrFg->stop();
    this->hide();
    ui->btnOK->setEnabled(false);
}

void MainWindow::foregroundLoopNotification()
{
    trayIcon->showMessage("Mamsds QBreak Reminder", "Time to have a break!", QSystemTrayIcon::Information,3500);
}

void MainWindow::initForegroundLoop()
{
    loadSettings();
    FgCount = 0;
    BgCount = 0;
    IsMouseReleased = true;
    QSettings settings("MamsdsStudio", "MamsdsQBreakReminder");
    ui->plainTextEdit->setPlainText(settings.value("Notes").toString());
    tmrBg->stop();

    if (IsBreakSkipped)
        this->initBackgroundLoop();
    else
    {
        this->show();
        tmrFg->start(1000);
        setWindowSizeAndLocation();
    }
}

void MainWindow::on_btnOK_clicked()
{
    initBackgroundLoop();
}

void MainWindow::on_actionEditNotes_triggered()
{
    qDebug() << "on_actionEditNotes_triggered";
    dialogEditNotes myEN;
    myEN.exec();
}

void MainWindow::on_actionSkipBreak_triggered()
{
    IsBreakSkipped = true;
}

void MainWindow::on_plainTextEdit_textChanged()
{
    QSettings settings("MamsdsStudio", "MamsdsQBreakReminder");
    settings.setValue("Notes", ui->plainTextEdit->toPlainText());
}

void MainWindow::on_pushButton_clicked()
{
    qDebug() << "on_actionEditNotes_triggered";
    dialogEditNotes myEN;
    myEN.exec();
}

void MainWindow::on_btnIcon_clicked()
{
    IsMouseReleased = true;
}
