#ifndef VIEWEVENTDIALOG_H
#define VIEWEVENTDIALOG_H

#include <QDialog>
#include <QGridLayout>

#include "mylabel.h"

#include "eventmanager.h"

#include <QString>

namespace Ui {
class ViewEventDialog;
}

class ViewEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewEventDialog(QWidget *parent, EventManager *eventmanager, long long ID);
    ~ViewEventDialog();

private:
    Ui::ViewEventDialog *ui;
    
    EventManager *eventmanager;
    long long ID;
    
    QGridLayout *layout;
    
    MyLabel *lbl_event_info;
    MyLabel *lbl_edit, *lbl_delete;
    
    QString get_event_info_text(long long id);

public slots:
	void refresh_content();

private slots:
	void on_edit_clicked();
	void on_delete_clicked();
};

#endif // VIEWEVENTDIALOG_H
