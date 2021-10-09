#ifndef DIALOGEDITNOTES_H
#define DIALOGEDITNOTES_H

#include <QDialog>

namespace Ui {
class dialogEditNotes;
}

class dialogEditNotes : public QDialog
{
    Q_OBJECT

public:
    explicit dialogEditNotes(QWidget *parent = 0);
    ~dialogEditNotes();

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::dialogEditNotes *ui;
};

#endif // DIALOGEDITNOTES_H
