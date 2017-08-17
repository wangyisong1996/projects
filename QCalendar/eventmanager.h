#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QMap>
#include <QPair>
#include <QString>
#include <QVector>

#include "utils.h"
#include "myevent.h"

class EventManager {
	
public:
	EventManager();
	~EventManager();
	
	void add_event(long long datetime, QString name, QString location,
					int repeat_type, int repeat_interval, long long ID = MyEvent::gen_ID());
	QVector<long long> get_events_since(long long datetime, int count = 10);
	QVector<long long> get_events_before(long long datetime, int count = 10);  // R)
	QVector<long long> get_events_between(long long datetime1, long long datetime2);  // [L, R)
	MyEvent get_event_by_ID(long long ID);
	
	void del_event(long long ID);
	
	void set_add_event(long long datetime, QString name, QString location,
						int repeat_type, int repeat_interval);
	void set_del_event(long long ID);
	void set_edit_event(long long ID, long long datetime, QString name, QString location,
						int repeat_type, int repeat_interval);
	
	QMap<MyEvent, bool> get_all_events();
	
	void clear_all_events();
	
private:
	QMap<MyEvent, bool> set1;
	QMap<QPair<long long, long long>, bool> set2;  // <datetime, ID>
	QMap<QPair<long long, long long>, bool> set2_r;  // <datetime, ID>
	
	void check_op();
	void update_events();
	
	bool checking = false;
	
	// 1: add; 2: del; 3: edit
	int op = 0;
	long long _datetime;
	QString _name;
	QString _location;
	int _repeat_type;
	int _repeat_interval;
	long long _ID;
};

#endif
