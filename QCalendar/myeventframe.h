#ifndef MYEVENTFRAME_H
#define MYEVENTFRAME_H

#include <QWidget>
#include <QGridLayout>
#include <QString>
#include <QLabel>
#include <QSignalMapper>
#include <QMessageBox>

#include "mylabel.h"
#include "eventmanager.h"

class MyEventFrame : public QWidget
{
	Q_OBJECT

public:
	explicit MyEventFrame(EventManager *e);
	~MyEventFrame();
	
	
	// QGridLayout * getLayout() {
	// 	return layout;
	// }
	QWidget * getWidget() {
		return widget;
	}
	
	void unselect();
	void select(int yy, int mm, int dd);
	
	void setEventManager(EventManager *e) {
		this->eventmanager = e;
		
		// QMessageBox::warning(NULL, "set e", QString::number((int)e, 10), QMessageBox::Yes);
	}
	
	void refresh_all();

private:

	// QGridLayout *layout;
	QWidget *widget;
	
	bool selected = false;
	int Y, M, D;
	
	MyLabel *lbl;
	
	QGridLayout *layout_info, *layout_day;
	QWidget *widget_info, *widget_day;
	
	EventManager *eventmanager = NULL;
	
	// layout day implementation
	MyLabel *lbl_day_title;
	MyLabel *lbl_day_colors[8];
	MyLabel *lbl_day_events[5];
	MyLabel *lbl_day_add_event;
	
	long long day_event_IDs[5];
	int day_event_count = 0;
	
	QSignalMapper *map_day_color;
	QSignalMapper *map_day_events;
	
	// layout info implementation
	MyLabel *lbl_info_title;
	MyLabel *lbl_info_events[6];
	MyLabel *lbl_info_event_cds[6];  // cd = countdown
	MyLabel *lbl_info_add_event;
	MyLabel *lbl_info_settings;
	
	long long info_event_IDs[6];
	int info_event_count = 0;
	
	QSignalMapper *map_info_events;
	
	
	void build_layout_day();
	void build_layout_info();
	
	void show_view_event_dialog(long long ID);
	

signals:
	void on_color_change(QColor color);
	void on_settings_click();
	void on_content_refresh();

public slots:
	void refresh_content();
	void change_color(QColor color);

private slots:
	void on_day_color_click(int id);
	void on_add_event_click();
	
	void on_day_events_click(int id);
	void on_info_events_click(int id);

};


#endif
