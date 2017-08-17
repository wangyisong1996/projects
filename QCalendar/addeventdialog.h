#ifndef ADDEVENTDIALOG_H
#define ADDEVENTDIALOG_H

#include <QDialog>

#include "eventmanager.h"

namespace Ui {
class AddEventDialog;
}

class AddEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEventDialog(QWidget *parent, EventManager *eventmanager, int Y, int M, int D);
    explicit AddEventDialog(QWidget *parent, EventManager *eventmanager, long long ID);
    ~AddEventDialog();

private slots:
    void on_AddEventDialog_accepted();
    void on_accept_clicked();
    void on_edit_clicked();

    void on_chk_repeat_toggled(bool checked);

private:
    Ui::AddEventDialog *ui;
    
    EventManager *eventmanager;
    int Y, M, D;
    long long ID;
};

#endif // ADDEVENTDIALOG_H
