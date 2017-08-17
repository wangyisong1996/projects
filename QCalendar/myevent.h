#ifndef MYEVENT_H
#define MYEVENT_H

#include "utils.h"

#include <QString>

struct MyEvent {
	long long ID;
	long long datetime;
	QString name;
	QString location;
	
	int repeat_type;
	// 0 -- no, 1 -- day, 2 -- week, 3 -- month, 4 -- year
	int repeat_interval;
	
	static long long gen_ID();
};

inline bool operator < (const MyEvent &a, const MyEvent &b) {
	return a.ID < b.ID;
}

#endif
