#include "vieweventdialog.h"
#include "ui_vieweventdialog.h"

#include "language.h"

#define lang Language::get_lang()

#include "utils.h"

#include <QMessageBox>

#include "addeventdialog.h"

static MyLabel * new_label(QString text = "") {
	MyLabel *l = new MyLabel(text);
	l->setAlignment(Qt::AlignCenter);
	l->setWordWrap(true);
	// l->setFrameStyle(QFrame::Panel);  // QFrame::Sunken
	// l->bgcolor = "white";
	l->leaveEvent(NULL);
	return l;
}

static const QString str_window_title[2] = {
	"View Event", "查看事件"
};

static const QString str_edit[2] = {
	"Edit", "编辑"
};

static const QString str_delete[2] = {
	"Delete", "删除"
};

ViewEventDialog::ViewEventDialog(QWidget *parent, EventManager *eventmanager, long long ID) :
    QDialog(parent),
    ui(new Ui::ViewEventDialog), eventmanager(eventmanager), ID(ID)
{
    ui->setupUi(this);
    
    this->setWindowTitle(str_window_title[lang]);
    
    layout = new QGridLayout();
    
    this->setLayout(layout);
    
    
    layout->addWidget(lbl_event_info = new_label(), 0, 0, 4, 1);
    layout->addWidget(lbl_edit = new_label(), 4, 0, 1, 1);
    layout->addWidget(lbl_delete = new_label(), 5, 0, 1, 1);
    
    lbl_event_info->setText(get_event_info_text(ID));
    lbl_event_info->setFrameStyle(QFrame::Panel);
    // lbl_event_info->setAlignment(Qt::AlignLeft);
    
    lbl_edit->setText(str_edit[lang]);
    lbl_delete->setText(str_delete[lang]);
    
    lbl_edit->hovercolor = "#bbbbff";
    lbl_edit->presscolor = "#8888ff";
    
    lbl_delete->hovercolor = "#ffbbbb";
    lbl_delete->presscolor = "#ff8888";
    
    connect(lbl_edit, SIGNAL(on_mouseup()), this, SLOT(on_edit_clicked()));
    connect(lbl_delete, SIGNAL(on_mouseup()), this, SLOT(on_delete_clicked()));
}

ViewEventDialog::~ViewEventDialog()
{
    delete ui;
    delete layout;
}

static const QString str_event_name[2] = {
	"Event's name", "事件名称"
};

static const QString str_event_time[2] = {
	"Time", "时间"
};

static const QString str_location[2] = {
	"Location", "地点"
};

static const QString str_not_set[2] = {
	"(not set)", "(未设置)"
};

QString ViewEventDialog::get_event_info_text(long long id) {
	MyEvent e = eventmanager->get_event_by_ID(id);
	QString ret;
	
	ret += h2(str_event_name[lang]) + "";
	ret += e.name.toHtmlEscaped() + "";
	
	ret += h2(str_event_time[lang]) + "";
	int yy, mm, dd, h, m, s;
	ll_to_datetime(e.datetime, yy, mm, dd, h, m, s);
	ret += QDateTime(QDate(yy, mm, dd), QTime(h, m, s))
			.toString("yyyy - MM - dd  hh : mm").toHtmlEscaped() + "";
	
	ret += h2(str_location[lang]) + "";
	ret += (e.location == "" ? str_not_set[lang] : e.location).toHtmlEscaped() + "";
	
	ret += "<br />";
	
	// ret = "<div style=\"font-family: 微软雅黑\">" + ret + "</div>";
	
	return ret;
}

void ViewEventDialog::refresh_content() {
	lbl_event_info->setText(get_event_info_text(ID));
}

void ViewEventDialog::on_edit_clicked() {
	AddEventDialog *dialog = NULL;
	if (dialog) delete dialog;
	dialog = new AddEventDialog(this, eventmanager, ID);
	dialog->show();
}

static const QString str_areyousuretodel[2] = {
	"Are you sure to delete this event ?", "你真的要删除这个事件吗？"
};

void ViewEventDialog::on_delete_clicked() {
	if (QMessageBox::warning(NULL, "QCalendar", str_areyousuretodel[lang],
		QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
		eventmanager->set_del_event(ID);
		this->close();
	}
}

