#include "myevent.h"

#include <QMap>
#include <QtGlobal>
#include <QDateTime>

static int _ = (qsrand(QDateTime(QDate(2000, 0, 0), QTime(0, 0, 0)).secsTo(QDateTime::currentDateTime())), 0);

static QMap<long long, bool> set_IDs;

static inline int _rand() {
	return qrand() % 32768;
}

static inline int _rand2() {
	return _rand() * 32768 + _rand();
}

static inline long long _rand4() {
	return _rand2() * (1LL << 30) + _rand2();
}

long long MyEvent::gen_ID() {
	while (1) {
		long long t = _rand4();
		if (set_IDs.count(t) == 0) {
			set_IDs.insert(t, 0);
			return t;
		}
	}
}
