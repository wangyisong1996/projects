#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>

#include "mycalendar.h"
#include "myeventframe.h"

#include <QResizeEvent>
#include <QMap>

#include "eventmanager.h"

#include <QTextStream>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
    int get_transparency_setting() {
    	return transparency;
    }
    void change_transparency_setting(int val);
    
    void set_lang(int l);
    
    bool save_to_file(QString filename);
    bool load_from_file(QString filename);

private:
    Ui::Widget *ui;
    void keyPressEvent(QKeyEvent *e);

    

    QGridLayout *main_layout;
    
	MyCalendar *calendar;
	MyEventFrame *event_frame;
	
	QWidget *w_calendar, *w_event_frame;
	
	bool selected = false;
	int sY, sM, sD;
	
	void resizeEvent(QResizeEvent *);
	
	void refresh_content();
	
	QMap<int, QColor> map_color;
	
	EventManager *eventmanager;
	
	void set_color(int yy, int mm, int dd, QColor c);
	
	int transparency = 0;  // 0 ~ 99
	
	void write_settings(QDataStream &f);
	void write_colors(QDataStream &f);
	void write_events(QDataStream &f);
	
	void read_settings(QDataStream &f);
	void read_colors(QDataStream &f);
	void read_events(QDataStream &f);

private slots:
    void on_timer_update();
    void on_calendar_click(int yy, int mm, int dd);
    void on_eventframe_color_change(QColor color);
    void on_settings_click();
    
    void on_shortcut_activated();
    
};

#endif // WIDGET_H
