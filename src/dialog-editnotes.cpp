#include "dialog-editnotes.h"
#include "ui_dialog-editnotes.h"

#include <QtCore>
#include <QtGui>

dialogEditNotes::dialogEditNotes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogEditNotes)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::WindowTitleHint
                   | Qt::Dialog | Qt::Tool);
    QSettings settings("ak-studio", "q-break-reminder");
    ui->txtNewNotes->setPlainText(settings.value("Notes").toString());

    QIcon icon = QIcon(":/leaf.png");
    setWindowIcon(icon);

    QApplication::setFont(QFont("Noto Sans CJK SC Medium", 9));
}

dialogEditNotes::~dialogEditNotes()
{
    delete ui;
}

void dialogEditNotes::on_btnOK_clicked()
{
    QSettings settings("ak-studio", "q-break-reminder");
    settings.setValue("Notes", ui->txtNewNotes->toPlainText());
    on_btnCancel_clicked();
}

void dialogEditNotes::on_btnCancel_clicked()
{
    this->close();
}

void dialogEditNotes::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier
        && (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return))
        on_btnOK_clicked();
    else if (event->key() == Qt::Key_Escape)
        on_btnCancel_clicked();
}
