#include "widget.h"
#include "ui_widget.h"
#include <QWidget>
#include <QKeyEvent>
#include <QFlags>
#include <QLabel>
#include <QFont>
#include <QGridLayout>
#include <QTimer>
#include <QTabWidget>
#include <QString>
#include <QMessageBox>
#include <QTextStream>
#include <QTextDocument>
#include <QShortcut>

#include "language.h"
#include "utils.h"

#include "addeventdialog.h"

#define lang Language::get_lang()

#include "settingsdialog.h"

static QLabel * new_label(QString text) {
	QLabel *l = new QLabel();
	l->setText(text);
	l->setAlignment(Qt::AlignCenter);
	l->setWordWrap(true);
	l->setFrameStyle(QFrame::Panel);  // QFrame::Sunken
	return l;
}

static QWidget * get_widget_from_layout(QLayout *l) {
	QWidget *ret = new QWidget();
	ret->setLayout(l);
	return ret;
}

static const QString str_calendar[2] = {
	"Calendar", "日历"
};


static QColor lighter(QColor c) {
	int r = c.red(), g = c.green(), b = c.blue();
	return QColor((r + 255 * 2) / 3, (g + 255 * 2) / 3, (b + 255 * 2) / 3);
}


Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);
	this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnBottomHint);
	
	eventmanager = new EventManager();
	
	calendar = new MyCalendar();
	event_frame = new MyEventFrame(eventmanager);
	
	// QMessageBox::warning(NULL, "warning", "heiheiheiheihei\n" + QString::number((int)eventmanager, 10), QMessageBox::Yes);

	// layout_labels = new QGridLayout();

	// layout_labels->setSpacing(2);
	// layout_labels->setMargin(2);



	// layout_labels->addWidget(new_label("hello"), 0, 0, 1, 1);
	// layout_labels->addWidget(new_label("world"), 0, 1, 1, 1);

	// this->setLayout(layout_labels);
	
	// eventmanager->add_event(datetime_to_ll(2016, 8, 25, 9, 0, 0), "辣鸡小学期", "东主楼9-224", 1, 1);
	// eventmanager->add_event(datetime_to_ll(2016, 8, 25, 14, 0, 0), "辣鸡小学期", "六教6A203", 1, 1);
	// eventmanager->add_event(datetime_to_ll(2016, 8, 25, 19, 0, 0), "辣鸡小学期", "东主楼9-224", 1, 1);
	
	
	connect(calendar, SIGNAL(on_click(int, int, int)), this, SLOT(on_calendar_click(int, int, int)));
	
	connect(event_frame, SIGNAL(on_color_change(QColor)), this, SLOT(on_eventframe_color_change(QColor)));
	connect(event_frame, SIGNAL(on_settings_click()), this, SLOT(on_settings_click()));
	
	
	
	
	
	
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(on_timer_update()));
	timer->start(1000);
	
	
	w_calendar = get_widget_from_layout(calendar->getLayout());
	w_event_frame = event_frame->getWidget();
	
	
	
	
	// this->set_color(2016, 8, 25, lighter("cyan"));
	// this->set_color(2016, 9, 3, lighter("red"));
	// this->set_color(2016, 12, 25, lighter("green"));
	// this->set_color(2016, 8, 17, lighter("grey"));
	
	if (selected) {
		this->on_calendar_click(sY, sM, sD);
		this->on_calendar_click(sY, sM, sD);
	}
	
	
	main_layout = new QGridLayout();
	this->setLayout(main_layout);
	
	
	// QMessageBox::warning(NULL, "main th", "this = " + QString::number((int)eventmanager, 10), QMessageBox::Yes);
	// QVector<long long> IDs = eventmanager->get_events_between(23333, 23333 + 86400);
	
	
	QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+T"), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(on_shortcut_activated()));
}

Widget::~Widget()
{
	delete ui;
}

void Widget::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_Space) {
		// if (this->is_opaque) {
		// 	this->setWindowOpacity(1.0);
		// 	this->setWindowFlags(Qt::Widget|Qt::WindowStaysOnBottomHint);
		// } else {
		// 	this->setWindowOpacity(0.7);
		// 	this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnBottomHint);
		// }
		// this->show();
		// this->is_opaque ^= 1;
		
		static AddEventDialog *dialog = NULL;
		if (dialog) delete dialog;
		dialog = new AddEventDialog(this, eventmanager, 2016, 8, 25);
		dialog->show();
		
		// QMessageBox::warning(NULL, "warning", "heiheihei", QMessageBox::Yes);
	}
	e->accept();
}


void Widget::on_timer_update() {
	// QMessageBox::warning(NULL, "warning", "heiheihei", QMessageBox::Yes);
	event_frame->refresh_content();
}


void Widget::resizeEvent(QResizeEvent *e) {
	this->refresh_content();
}

void Widget::on_calendar_click(int yy, int mm, int dd) {
	if (!selected || yy != sY || mm != sM || dd != sD) {
		selected = true;
		sY = yy, sM = mm, sD = dd;
		calendar->select(yy, mm, dd);
		event_frame->select(yy, mm, dd);
		if (map_color.count(date_to_int(sY, sM, sD)) > 0) {
			event_frame->change_color(map_color[date_to_int(sY, sM, sD)]);
		} else {
			event_frame->change_color("white");
		}
	} else {
		selected = false;
		calendar->unselect();
		event_frame->unselect();
	}
	this->refresh_content();
}

void Widget::refresh_content() {
	int width = this->geometry().width();
	int height = this->geometry().height();
	
	// main_layout->removeItem(main_layout->itemAt(0));
	// main_layout->removeItem(main_layout->itemAt(0));
	w_calendar->hide();
	w_event_frame->hide();
	// main_layout->removeWidget(w_calendar);
	// main_layout->removeWidget(w_event_frame);
	
	w_event_frame = event_frame->getWidget();
	// QMessageBox::warning(NULL, "warning", QString::number((int)event_frame->getLayout()), QMessageBox::Yes);
	
	
	
	if (width > height) {
		main_layout->addWidget(w_calendar, 0, 0, 2, 1);
		main_layout->addWidget(w_event_frame, 0, 1, 2, 1);
	} else {
		main_layout->addWidget(w_calendar, 0, 0, 1, 2);
		main_layout->addWidget(w_event_frame, 1, 0, 1, 2);
	}
	
	w_calendar->show();
	w_event_frame->show();
}

void Widget::set_color(int yy, int mm, int dd, QColor c) {
	calendar->set_color(yy, mm, dd, c);
	map_color[date_to_int(yy, mm, dd)] = c;
}

void Widget::on_eventframe_color_change(QColor color) {
	if (selected) {
		this->set_color(sY, sM, sD, color);
	}
}

void Widget::change_transparency_setting(int val) {
	if (val < 0 || val > 99) return;
	transparency = val;
	this->setWindowOpacity(1.0 - 0.01 * val);
}

void Widget::on_settings_click() {
	static SettingsDialog *dialog = NULL;
	if (dialog) delete dialog;
	dialog = new SettingsDialog(NULL, this);
	dialog->show();
}

void Widget::set_lang(int l) {
	if (l < 0 || l > 1 || l == lang) {
		return;
	}
	Language::set_lang(l);
	calendar->refresh_all();
	event_frame->refresh_all();
}

bool Widget::save_to_file(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}
	QDataStream f(&file);
	this->write_settings(f);
	this->write_colors(f);
	this->write_events(f);
	// f.flush();
	return true;
}

bool Widget::load_from_file(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}
	QDataStream f(&file);
	this->read_settings(f);
	this->read_colors(f);
	this->read_events(f);
	
	event_frame->refresh_all();
	return true;
}

void Widget::write_settings(QDataStream &f) {
	f << (int)lang;
	f << this->transparency;
}

void Widget::write_colors(QDataStream &f) {
	QMap<int, QColor> map = this->map_color;
	int cnt = 0;
	for (auto it = map.begin(); it != map.end(); ++it) {
		if (it.value() != QColor("#ffffff")) {
			++cnt;
		}
	}
	f << cnt;
	for (auto it = map.begin(); it != map.end(); ++it) {
		if (it.value() != QColor("#ffffff")) {
			f << it.key() << it.value().name();
		}
	}
}

// static QDataStream & operator << (QDataStream &f, const QString &s) {
// 	int len = s.length();
// 	f << len;
// 	for (int i = 0; i < len; i++) {
// 		f << s[i];
// 	}
// 	return f;
// }

// static QDataString & operator >> (QDataStream &f, QString &s) {
// 	int len;
// 	f >> len;
// 	s.clear();
// 	for (int i = 0; i < len; i++) {
// 		QChar c;
// 		f >> c;
// 		s.append(c);
// 	}
// 	return f;
// }

void Widget::write_events(QDataStream &f) {
	QMap<MyEvent, bool> map = eventmanager->get_all_events();
	int cnt = (int)map.size();
	f << cnt;
	for (auto it = map.begin(); it != map.end(); ++it) {
		MyEvent e = it.key();
		f << e.ID << e.datetime;
		f << e.name << e.location;
		f << e.repeat_type << e.repeat_interval;
	}
}

void Widget::read_settings(QDataStream &f) {
	int l, t;
	f >> l >> t;
	this->set_lang(l);
	this->change_transparency_setting(t);
}

void Widget::read_colors(QDataStream &f) {
	int cnt;
	f >> cnt;
	calendar->clear_all_colors();
	map_color.clear();
	for (int i = 0; i < cnt; i++) {
		int i_date;
		QString s;
		f >> i_date >> s;
		int yy, mm, dd;
		int_to_date(i_date, yy, mm, dd);
		this->set_color(yy, mm, dd, QColor(s));
	}
	if (selected) {
		if (map_color.count(date_to_int(sY, sM, sD)) > 0) {
			event_frame->change_color(map_color[date_to_int(sY, sM, sD)]);
		} else {
			event_frame->change_color("white");
		}
	}
}

static inline QString unescape_html(QString s) {
	QTextDocument text;
	text.setHtml(s);
	return text.toPlainText();
}

void Widget::read_events(QDataStream &f) {
	int cnt;
	f >> cnt;
	eventmanager->clear_all_events();
	for (int i = 0; i < cnt; i++) {
		long long ID, datetime;
		f >> ID >> datetime;
		QString name, location;
		f >> name >> location;
		int repeat_type, repeat_interval;
		f >> repeat_type >> repeat_interval;
		eventmanager->add_event(datetime, name, location, repeat_type, repeat_interval, ID);
	}
}

void Widget::on_shortcut_activated() {
	static bool f = false;
	
	f ^= 1;
	
	if (f) {
		this->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget | Qt::WindowStaysOnTopHint |
			Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTransparentForInput);
	} else {
		this->setWindowFlags(Qt::Widget);
	}
	this->show();
}
