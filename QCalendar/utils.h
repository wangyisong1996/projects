#ifndef UTILS_H
#define UTILS_H

#include <QDate>
#include <QString>

inline int date_to_int(int yy, int mm, int dd) {
	return yy * 32 * 13 + mm * 32 + dd;
}

inline long long datetime_to_ll(int yy, int mm, int dd, int h, int m, int s = 0) {
	return date_to_int(yy, mm, dd) * 86400LL + (h * 3600 + m * 60 + s);
}

inline long long date_to_ll(int yy, int mm, int dd) {
	return date_to_int(yy, mm, dd) * 86400LL;
}

inline int date_to_dayofweek(int yy, int mm, int dd) {
	return QDate(yy, mm, dd).dayOfWeek() - 1;
}

inline void int_to_date(int x, int &yy, int &mm, int &dd) {
	dd = x % 32;
	x /= 32;
	mm = x % 13;
	yy = x / 13;
}

inline void int_to_time(int x, int &h, int &m, int &s) {
	s = x % 60;
	x /= 60;
	m = x % 60;
	h = x / 60;
}

inline void ll_to_datetime(long long x, int &yy, int &mm, int &dd, int &h, int &m, int &s) {
	int_to_date(x / 86400, yy, mm, dd);
	int_to_time(x % 86400, h, m, s);
}

inline QString h2(QString s) {
	return "<h2>" + s + "</h2>";
}

inline QString h3(QString s) {
	return "<h3>" + s + "</h3>";
}

#endif
