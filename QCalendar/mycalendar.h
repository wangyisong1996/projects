#ifndef MYCALENDAR_H
#define MYCALENDAR_H

#include <QWidget>
#include <QGridLayout>
#include <QString>
#include <QLabel>
#include <QSignalMapper>
#include <QMap>
#include <QColor>

#include "mylabel.h"


class MyCalendar : public QWidget
{
	Q_OBJECT

public:
	explicit MyCalendar(QWidget *parent = 0);
	~MyCalendar();

	void set_month(int yy, int mm);
	// void set_content(int yy, int dd, int mm, QString text);
	
	QGridLayout * getLayout() {
		return layout;
	}
	
	void unselect();
	void select(int yy, int mm, int dd);
	
	void set_color(int yy, int mm, int dd, QColor color);
	
	void refresh_all();
	
	void clear_all_colors();

private:

	QGridLayout *layout;
	
	void refresh_content();
	
	MyLabel *lbl_weekids[6];
	MyLabel *lbl_daynames[7];
	MyLabel *lbl_days[6][7];
	MyLabel *lbl_title, *lbl_prev, *lbl_next;
	MyLabel *lbl_titleweek;
	
	struct Date {
		int yy, mm, dd;
	};
	
	Date dates[6][7];
	
	int Y, M;
	int first_day_id, last_day_id;
	
	bool selected = false;
	int sY, sM, sD;
	
	QSignalMapper *mapper;
	
	QMap<int, QColor> map_color;

signals:
	void on_click(int yy, int mm, int dd);

public slots:
	void next_month();
	void prev_month();

private slots:
	void on_lbl_days_click(int id);

};


#endif // MYCALENDAR_H
