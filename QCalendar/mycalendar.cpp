#include "mycalendar.h"
#include "language.h"

#include "utils.h"

#define lang Language::get_lang()

#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QDateTime>

static MyLabel * new_label(QString text = "233") {
    MyLabel *l = new MyLabel(text);
    l->setAlignment(Qt::AlignCenter);
    l->setWordWrap(true);
    // l->setFrameStyle(QFrame::Panel);  // QFrame::Sunken
    // l->bgcolor = "white";
    l->leaveEvent(NULL);
    return l;
}

MyCalendar::MyCalendar(QWidget *parent) {
	layout = new QGridLayout();
	layout->setSpacing(1);
	layout->setMargin(1);
	
	layout->addWidget(lbl_title = new_label(), 0, 1, 1, 5);
	layout->addWidget(lbl_prev = new_label(), 0, 6);
	layout->addWidget(lbl_next = new_label(), 0, 7);
	
	layout->addWidget(lbl_titleweek = new_label(), 1, 0);
	lbl_titleweek->set_bgcolor("#dddddd");
	
	for (int i = 0; i < 7; i++) {
		layout->addWidget(lbl_daynames[i] = new_label(), 1, i + 1);
		lbl_daynames[i]->set_bgcolor("#dddddd");
	}
	
	for (int i = 0; i < 6; i++) {
		layout->addWidget(lbl_weekids[i] = new_label(), i + 2, 0);
		lbl_weekids[i]->set_bgcolor("#dfdfdf");
	}
	
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			layout->addWidget(lbl_days[i][j] = new_label(), i + 2, j + 1);
		}
	}
	
	connect(lbl_next, SIGNAL(on_mouseup()), this, SLOT(next_month()));
	connect(lbl_prev, SIGNAL(on_mouseup()), this, SLOT(prev_month()));
	
	lbl_next->hovercolor = "#bbbbff";
	lbl_prev->hovercolor = "#bbbbff";
	
	lbl_next->presscolor = "#8888ff";
	lbl_prev->presscolor = "#8888ff";
	
	mapper = new QSignalMapper(this);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			int id = i * 7 + j;
			connect(lbl_days[i][j], SIGNAL(on_mousedown()), mapper, SLOT(map()));
			mapper->setMapping(lbl_days[i][j], id);
		}
	}
	
	connect(mapper, SIGNAL(mapped(int)), this, SLOT(on_lbl_days_click(int)));
	
	QDateTime datetime = QDateTime::currentDateTime();
	int tmp_yy, tmp_mm;
	bool ok;
	tmp_yy = datetime.toString("yyyy").toInt(&ok, 10);
	tmp_mm = datetime.toString("MM").toInt(&ok, 10);
	
	this->set_month(tmp_yy, tmp_mm);
}

MyCalendar::~MyCalendar() {}

static inline bool is_leap_year(int y) {
	return y % 4 == 0 || (y % 100 != 0 && y % 400 == 0);
}

static const int days_in_months[2][13] = {
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static const QString month_names[2][13] = {
	{"", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
	{"", "一月", "二月", "三月", "四月", "五月", "六月", "七月", "八月", "九月", "十月", "十一月", "十二月"}
};

static const QString day_names[2][7] = {
	{"Mon", "Tue", "Wed", "Thur", "Fri", "Sat", "Sun"},
	{"星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"}
};

static const QString prev_name[2] = {
	"&lt;", "&lt;"
};

static const QString next_name[2] = {
	"&gt;", "&gt;"
};

static const QString title_week_name[2] = {
	"Week", "周"
};

void MyCalendar::set_month(int yy, int mm) {
	if (yy < 1920) {
		return;
	}
	Y = yy, M = mm;
	
	int now_y = 1916;
	int now_m = 1;
	int now_d = 1;
	int now_day = 5;  // Sat
	
	while (now_y < yy) {
		bool is_l = is_leap_year(now_y);
		now_day = (now_day + 1 + is_l) % 7;
		++now_y;
	}
	
	bool is_l = is_leap_year(yy);
	
	int days = 0;
	for (int i = 1; i < mm; i++) {
		days += days_in_months[is_l][i];
	}
	
	int week_id = 1;
	
	for (int i = 1; i <= days; i++) {
		now_day = (now_day + 1) % 7;
		if (now_day == 0) {
			++week_id;
		}
	}
	
	now_m = mm;
	now_d = 1;
	
	int x = 0, y = now_day;
	
	int last_yy, last_mm;
	int last_month_days;
	if (mm == 1) {
		last_month_days = 31;
		last_yy = yy - 1;
		last_mm = 12;
	} else {
		last_month_days = days_in_months[is_l][mm - 1];
		last_yy = yy;
		last_mm = mm - 1;
	}
	
	for (int i = now_day - 1; i >= 0; i--) {
		dates[0][i] = (Date){last_yy, last_mm, last_month_days - (now_day - 1 - i)};
	}
	
	lbl_weekids[0]->setText(QString::number(week_id, 10));
	
	while (x < 6) {
		if (y == 0) {
			lbl_weekids[x]->setText(now_y == yy ? QString::number(week_id, 10) : "");
		}
		dates[x][y] = (Date){now_y, now_m, now_d};
		y = (y + 1) % 7;
		x += y == 0;
		week_id += y == 0;
		if (++now_d > days_in_months[is_l][now_m]) {
			now_d = 1;
			if (++now_m > 12) {
				now_m = 1;
				++now_y;
			}
		}
	}
	
	first_day_id = date_to_int(dates[0][0].yy, dates[0][0].mm, dates[0][0].dd);
	last_day_id = date_to_int(dates[5][6].yy, dates[5][6].mm, dates[5][6].dd);
	
	this->refresh_content();
}

void MyCalendar::refresh_content() {
	lbl_title->setText("<h2>" + QString::number(Y, 10) + " " + month_names[lang][M] + "</h2>");
	lbl_prev->setText("<h2>" + prev_name[lang] + "</h2>");
	lbl_next->setText("<h2>" + next_name[lang] + "</h2>");
	
	lbl_titleweek->setText(title_week_name[lang]);
	
	for (int i = 0; i < 7; i++) {
		lbl_daynames[i]->setText("<h3>" + day_names[lang][i] + "</h3>");
	}
	
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			lbl_days[i][j]->setText(QString::number(dates[i][j].dd, 10));
			
			if (selected) {
				int yy = dates[i][j].yy, mm = dates[i][j].mm, dd = dates[i][j].dd;
				if (yy == sY && mm == sM && dd == sD) {
					// lbl_days[i][j]->set_bgcolor("#ffaaaa");
					// lbl_days[i][j]->setText("&lt;" + lbl_days[i][j]->text() + "&gt;");
					lbl_days[i][j]->setText("[" + QString::number(dates[i][j].dd, 10) + "]");
				}
			}
			
			if (dates[i][j].mm == M) {
				lbl_days[i][j]->setText("<h2>" + lbl_days[i][j]->text() + "</h2>");
			} else {
				lbl_days[i][j]->setText("<font color=#333333>" + lbl_days[i][j]->text() + "</font>");
				lbl_days[i][j]->setText("<h3>" + lbl_days[i][j]->text() + "</h3>");
			}
			
			
			int day_id = date_to_int(dates[i][j].yy, dates[i][j].mm, dates[i][j].dd);
			if (map_color.count(day_id) > 0) {
				lbl_days[i][j]->set_bgcolor(map_color[day_id].name());
			} else {
				lbl_days[i][j]->set_bgcolor("white");
			}
			
			
		}
	}
}

void MyCalendar::next_month() {
	this->set_month(Y + (M == 12), M == 12 ? 1 : M + 1);
}

void MyCalendar::prev_month() {
	this->set_month(Y - (M == 1), M == 1 ? 12 : M - 1);
}

void MyCalendar::on_lbl_days_click(int id) {
	int x = id / 7;
	int y = id % 7;
	int yy = dates[x][y].yy, mm = dates[x][y].mm, dd = dates[x][y].dd;
	emit on_click(yy, mm, dd);
}

void MyCalendar::unselect() {
	if (selected) {
		selected = false;
		this->refresh_content();
	}
}

void MyCalendar::select(int yy, int mm, int dd) {
	selected = true;
	sY = yy, sM = mm, sD = dd;
	this->refresh_content();
}

void MyCalendar::set_color(int yy, int mm, int dd, QColor color) {
	int id = date_to_int(yy, mm, dd);
	map_color[id] = color;
	if (id >= first_day_id && id <= last_day_id) {
		this->refresh_content();
	}
}

void MyCalendar::refresh_all() {
	this->refresh_content();
}

void MyCalendar::clear_all_colors() {
	map_color.clear();
	this->refresh_content();
}
