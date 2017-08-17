#include "myeventframe.h"

#include <QMessageBox>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QDateTime>
#include <QColor>

#include "mylabel.h"
#include "utils.h"
#include "language.h"

#include "eventmanager.h"

#define lang Language::get_lang()

#define NUM(x) QString::number(x, 10)

#include "addeventdialog.h"

#include "vieweventdialog.h"

static MyLabel * new_label(QString text = "") {
	MyLabel *l = new MyLabel(text);
	l->setAlignment(Qt::AlignCenter);
	l->setWordWrap(true);
	// l->setFrameStyle(QFrame::Panel);  // QFrame::Sunken
	// l->bgcolor = "white";
	l->leaveEvent(NULL);
	return l;
}

static QWidget * get_widget_from_layout(QLayout *l) {
	QWidget *ret = new QWidget();
	ret->setLayout(l);
	return ret;
}


static QColor lighter(QColor c) {
	int r = c.red(), g = c.green(), b = c.blue();
	return QColor((r + 255 * 2) / 3, (g + 255 * 2) / 3, (b + 255 * 2) / 3);
}

static const QColor day_colors[8] = {
	// "white", "red", "green", "blue", "cyan", "magenta", "yellow", "grey"
	"white",
	lighter("red"),
	lighter("green"),
	lighter("blue"),
	lighter("cyan"),
	lighter("magenta"),
	lighter("yellow"),
	lighter("grey")
};

static const QString day_names[2][7] = {
	{"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"},
	{"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"}
};


MyEventFrame::MyEventFrame(EventManager *e) : QWidget() {
	// lbl = new_label("2333");
	
	layout_info = new QGridLayout();
	layout_day = new QGridLayout();
	
	widget_info = get_widget_from_layout(layout_info);
	widget_day = get_widget_from_layout(layout_day);
	
	// layout_info->addWidget(lbl, 0, 0);
	
	this->build_layout_day();
	this->build_layout_info();
	
	widget = widget_info;
	
	this->setEventManager(e);
	
	this->refresh_content();
}

MyEventFrame::~MyEventFrame() {}



static QString gen_event_text(const MyEvent &e) {
	QString ret, tmp;
	ret = "<h3>";
	
	int h, m, s;
	int_to_time(e.datetime % 86400, h, m, s);
	tmp.sprintf("%02d:%02d", h, m);
	ret += tmp;
	
	ret += "&nbsp;";
	ret += e.name.toHtmlEscaped();
	
	
	
	
	if (e.location != "") {
		ret += "&nbsp;";
		ret += "@" + e.location.toHtmlEscaped();
	}
	
	ret += "</h3>";
	return ret;
}


static const QString str_no_event[2] = {
	"No event.", "没有事件."
};

static const QString str_info_event_texts[2][2][2] = {
	{{"To&nbsp;", "&nbsp;"}, {"Since&nbsp;", "&nbsp;"}},
	{{"离&nbsp;", "&nbsp;还有"}, {"&nbsp;", "&nbsp;已经"}}
};

static QString gen_info_event_text(const MyEvent &e, const QDateTime &t) {
	int yy, mm, dd, h, m, s;
	ll_to_datetime(e.datetime, yy, mm, dd, h, m, s);
	QDateTime t0(QDate(yy, mm, dd), QTime(h, m, s));
	bool is_since = t > t0;
	QString ret;
	ret = str_info_event_texts[lang][is_since][0] + e.name.toHtmlEscaped()
			+ str_info_event_texts[lang][is_since][1];
	return "<h3 style=\"font-family: 微软雅黑; font-weight: normal;\">" + ret + "</h3>";
}

static const QString str_day[2] = {
	"day", "天"
};

static const QString str_hour[2] = {
	"hour", "小时"
};

static const QString str_minute[2] = {
	"min", "分钟"
};

static const QString str_second[2] = {
	"sec", "秒"
};

static QString gen_info_event_cd_text(const MyEvent &e, const QDateTime &t) {
	int yy, mm, dd, h, m, s;
	ll_to_datetime(e.datetime, yy, mm, dd, h, m, s);
	QDateTime t0(QDate(yy, mm, dd), QTime(h, m, s));
	bool is_since = t > t0;
	long long tmp = is_since ? t0.secsTo(t) : t.secsTo(t0);
	
	// tmp = QDateTime(QDate(2016, 8, 25), QTime(13, 30, 0, 100)).secsTo(QDateTime(QDate(2016, 8, 25), QTime(13, 30, 1)));
	
	QString ret = "<h2 style=\"font-family: 微软雅黑; font-weight: normal;\">";
	
	if (tmp >= 86400) {
		tmp /= 86400;
		ret += NUM(tmp) + "&nbsp;" + str_day[lang];
	} else if (tmp >= 3600) {
		tmp /= 3600;
		ret += NUM(tmp) + "&nbsp;" + str_hour[lang];
	} else if (tmp >= 60) {
		tmp /= 60;
		ret += NUM(tmp) + "&nbsp;" + str_minute[lang];
	} else {
		ret += NUM(tmp) + "&nbsp;" + str_second[lang];
	}
	if (tmp > 1 && lang == 0) {
		ret += "s";
	}
	
	ret += "</h2>";
	
	return ret;
}

static const QString str_day_of_week[2][7] = {
	{"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"},
	{"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"}
};

void MyEventFrame::refresh_content() {
	
	if (selected) {
		// lbl->setText("Selected " + NUM(Y) + "-" + NUM(M) + "-" + NUM(D));
		
		long long ll_date = date_to_ll(Y, M, D);
		QVector<long long> IDs = eventmanager->get_events_between(ll_date, ll_date + 86400);
		
		
		day_event_count = qMin((int)IDs.size(), 5);
		for (int i = 0; i < 5; i++) {
			lbl_day_events[i]->hovercolor = "";
		}
		if (day_event_count > 0) {
			for (int i = 0; i < day_event_count; i++) {
				long long id = IDs[i];
				day_event_IDs[i] = id;
				MyEvent e = eventmanager->get_event_by_ID(id);
				lbl_day_events[i]->setText(gen_event_text(e));
				lbl_day_events[i]->setFrameStyle(QFrame::Panel);
				lbl_day_events[i]->hovercolor = "#bbbbff";
			}
			for (int i = day_event_count; i < 5; i++) {
				lbl_day_events[i]->setText("");
				lbl_day_events[i]->setFrameStyle(0);
			}
		} else {
			lbl_day_events[0]->setText(str_no_event[lang]);
			lbl_day_events[0]->setFrameStyle(0);
			for (int i = 1; i < 5; i++) {
				lbl_day_events[i]->setText("");
				lbl_day_events[i]->setFrameStyle(0);
			}
		}
	} else {
		// QDateTime datetime = QDateTime::currentDateTime();
		// lbl->setText("Now: " + datetime.toString("yyyy-MM-dd hh:mm:ss"));
		
		QDateTime now_datetime = QDateTime::currentDateTime();
		int yy, mm, dd, h, m, s;
		yy = now_datetime.date().year();
		mm = now_datetime.date().month();
		dd = now_datetime.date().day();
		h = now_datetime.time().hour();
		m = now_datetime.time().minute();
		s = now_datetime.time().second();
		
		QString s_title = "<h2 style=\"font-family: 微软雅黑; font-weight: normal;\">";
		s_title += now_datetime.date().toString("yyyy - MM - dd");
		s_title += ", " + str_day_of_week[lang][now_datetime.date().dayOfWeek() - 1];
		s_title += ", " + now_datetime.time().toString("hh : mm : ss");
		s_title += "</h2>";
		lbl_info_title->setText(s_title);
		
		long long ll_datetime = datetime_to_ll(yy, mm, dd, h, m, s);
		QVector<long long> IDs = eventmanager->get_events_since(ll_datetime, 6);
		int count1 = (int)IDs.size();
		if (count1 < 6) {
			QVector<long long> IDs1 = eventmanager->get_events_before(ll_datetime, 6 - count1);
			for (int i = 0; i < (int)IDs1.size(); i++) {
				IDs.push_back(IDs1[i]);
			}
		}
		info_event_count = qMin((int)IDs.size(), 6);
		for (int i = 0; i < 6; i++) {
			lbl_info_events[i]->hovercolor = "";
		}
		if (info_event_count > 0) {
			lbl_info_event_cds[0]->show();
			for (int i = 0; i < info_event_count; i++) {
				long long id = IDs[i];
				info_event_IDs[i] = id;
				MyEvent e = eventmanager->get_event_by_ID(id);
				lbl_info_events[i]->setText(gen_info_event_text(e, now_datetime));
				lbl_info_events[i]->setFrameStyle(QFrame::Panel);
				lbl_info_events[i]->hovercolor = "#bbbbff";
				
				lbl_info_event_cds[i]->setText(gen_info_event_cd_text(e, now_datetime));
				lbl_info_event_cds[i]->setFrameStyle(QFrame::Panel);
			}
			for (int i = info_event_count; i < 6; i++) {
				lbl_info_events[i]->setText("");
				lbl_info_events[i]->setFrameStyle(0);
				lbl_info_event_cds[i]->setText("");
				lbl_info_event_cds[i]->setFrameStyle(0);
			}
		} else {
			lbl_info_events[0]->setText(str_no_event[lang]);
			lbl_info_events[0]->setFrameStyle(0);
			lbl_info_event_cds[0]->hide();
			for (int i = 1; i < 6; i++) {
				lbl_info_events[i]->setText("");
				lbl_info_events[i]->setFrameStyle(0);
				lbl_info_event_cds[i]->setText("");
				lbl_info_event_cds[i]->setFrameStyle(0);
			}
		}
	}
	
	emit on_content_refresh();
}

void MyEventFrame::unselect() {
	if (selected) {
		selected = false;
		
		widget = widget_info;
		this->refresh_content();
	}
}

void MyEventFrame::select(int yy, int mm, int dd) {
	Y = yy, M = mm, D = dd;
	selected = true;
	// TODO
	
	lbl_day_title->setText("<h2>" + NUM(Y) + "-" + NUM(M) + "-" + NUM(D) + " " +
							day_names[lang][date_to_dayofweek(Y, M, D)] + "</h2>");
	
	widget = widget_day;
	
	this->refresh_content();
}

static const QString str_add_event[2] = {
	"Add event", "添加事件"
};

void MyEventFrame::build_layout_day() {
	layout_day->setSpacing(8);
	layout_day->setMargin(8);
	// layout_day->setStyleSheet("background-color: white");
	
	layout_day->addWidget(lbl_day_title = new_label(), 0, 0, 1, 8);
	
	map_day_color = new QSignalMapper(this);
	
	
	
	for (int i = 0; i < 8; i++) {
		layout_day->addWidget(lbl_day_colors[i] = new_label(""), 1, i, 1, 1);
		lbl_day_colors[i]->setMaximumSize(QSize(36, 36));
		lbl_day_colors[i]->set_bgcolor(day_colors[i].name());
		connect(lbl_day_colors[i], SIGNAL(on_mousedown()), map_day_color, SLOT(map()));
		map_day_color->setMapping(lbl_day_colors[i], i);
	}
	connect(map_day_color, SIGNAL(mapped(int)), this, SLOT(on_day_color_click(int)));
	
	
	map_day_events = new QSignalMapper(this);
	for (int i = 0; i < 5; i++) {
		layout_day->addWidget(lbl_day_events[i] = new_label(), i + 2, 1, 1, 6);
		lbl_day_events[i]->setFrameStyle(QFrame::Panel);
		connect(lbl_day_events[i], SIGNAL(on_mouseup()), map_day_events, SLOT(map()));
		map_day_events->setMapping(lbl_day_events[i], i);
	}
	connect(map_day_events, SIGNAL(mapped(int)), this, SLOT(on_day_events_click(int)));
	
	layout_day->addWidget(lbl_day_add_event = new_label(), 7, 0, 1, 8);
	lbl_day_add_event->setText("<h3>" + str_add_event[lang] + "</h3>");
	lbl_day_add_event->hovercolor = "#bbbbff";
	lbl_day_add_event->presscolor = "#8888ff";
	connect(lbl_day_add_event, SIGNAL(on_mouseup()), this, SLOT(on_add_event_click()));
}

void MyEventFrame::change_color(QColor color) {
	for (int i = 0; i < 8; i++) {
		if (color == day_colors[i]) {
			lbl_day_colors[i]->setText("<h2>√</h2>");
		} else {
			lbl_day_colors[i]->setText("");
		}
	}
}

void MyEventFrame::on_day_color_click(int id) {
	emit on_color_change(day_colors[id]);
	change_color(day_colors[id]);
}

void MyEventFrame::on_add_event_click() {
	int yy, mm, dd;
	if (selected) {
		yy = Y, mm = M, dd = D;
	} else {
		QDate d = QDate::currentDate();
		yy = d.year(), mm = d.month(), dd = d.day();
	}
	
	static AddEventDialog *dialog = NULL;
	if (dialog) delete dialog;
	dialog = new AddEventDialog(this, eventmanager, yy, mm, dd);
	dialog->show();
}

static const QString str_settings[2] = {
	"Settings", "设置"
};

void MyEventFrame::build_layout_info() {
	layout_info->setSpacing(8);
	layout_info->setMargin(8);
	
	layout_info->addWidget(lbl_info_title = new_label(), 0, 0, 1, 8);
	
	map_info_events = new QSignalMapper(this);
	for (int i = 0; i < 6; i++) {
		layout_info->addWidget(lbl_info_events[i] = new_label(), i + 1, 0, 1, 6);
		lbl_info_events[i]->setFrameStyle(QFrame::Panel);
		connect(lbl_info_events[i], SIGNAL(on_mouseup()), map_info_events, SLOT(map()));
		map_info_events->setMapping(lbl_info_events[i], i);
		
		layout_info->addWidget(lbl_info_event_cds[i] = new_label(), i + 1, 6, 1, 2);
		lbl_info_event_cds[i]->setFrameStyle(QFrame::Panel);
	}
	connect(map_info_events, SIGNAL(mapped(int)), this, SLOT(on_info_events_click(int)));
	
	layout_info->addWidget(lbl_info_add_event = new_label(), 7, 0, 1, 4);
	lbl_info_add_event->setText("<h3>" + str_add_event[lang] + "</h3>");
	lbl_info_add_event->hovercolor = "#bbbbff";
	lbl_info_add_event->presscolor = "#8888ff";
	connect(lbl_info_add_event, SIGNAL(on_mouseup()), this, SLOT(on_add_event_click()));
	
	layout_info->addWidget(lbl_info_settings = new_label(), 7, 4, 1, 4);
	lbl_info_settings->setText("<h3>" + str_settings[lang] + "</h3>");
	lbl_info_settings->hovercolor = "#bbbbff";
	lbl_info_settings->presscolor = "#8888ff";
	connect(lbl_info_settings, SIGNAL(on_mouseup()), this, SIGNAL(on_settings_click()));
}

void MyEventFrame::show_view_event_dialog(long long ID) {
	static ViewEventDialog *dialog = NULL;
	if (dialog) delete dialog;
	dialog = new ViewEventDialog(this, eventmanager, ID);
	connect(this, SIGNAL(on_content_refresh()), dialog, SLOT(refresh_content()));
	dialog->show();
}

void MyEventFrame::on_day_events_click(int id) {
	if (id >= day_event_count) return;
	this->show_view_event_dialog(day_event_IDs[id]);
}

void MyEventFrame::on_info_events_click(int id) {
	if (id >= info_event_count) return;
	this->show_view_event_dialog(info_event_IDs[id]);
}

void MyEventFrame::refresh_all() {
	lbl_day_add_event->setText("<h3>" + str_add_event[lang] + "</h3>");
	lbl_info_add_event->setText("<h3>" + str_add_event[lang] + "</h3>");
	lbl_info_settings->setText("<h3>" + str_settings[lang] + "</h3>");
	this->refresh_content();
	
}
