#include "eventmanager.h"

#include <QMessageBox>

EventManager::EventManager() : set1(), set2() {
	// set2.insert(qMakePair(0x3f3f3f3f3f3f3f3fLL, -1), 0);
}

EventManager::~EventManager() {}

void EventManager::add_event(long long datetime, QString name, QString location,
							int repeat_type, int repeat_interval, long long ID) {
	if (repeat_type < 0 || repeat_type > 4) return;
	if (repeat_type != 0 && repeat_interval <= 0) return;
	
	MyEvent e;
	e.ID = ID;
	e.datetime = datetime;
	e.name = name;
	e.location = location;
	e.repeat_type = repeat_type;
	e.repeat_interval = repeat_interval;
	
	set1.insert(e, 0);
	
	set2.insert(qMakePair(e.datetime, e.ID), 0);
	
	if (repeat_type != 0) {
		set2_r.insert(qMakePair(e.datetime, e.ID), 0);
	}
}

QVector<long long> EventManager::get_events_since(long long datetime, int count) {
	check_op();
	
	QVector<long long> ret;
	QMap<QPair<long long, long long>, bool>::iterator it = set2.lowerBound(qMakePair(datetime, 0LL));
	if (count < 0) count = 0;
	while (it != set2.end() && count--) {
		ret.push_back(it.key().second);
		++it;
	}
	return ret;
}

QVector<long long> EventManager::get_events_before(long long datetime, int count) {
	check_op();
	
	QVector<long long> ret;
	QMap<QPair<long long, long long>, bool>::iterator it = set2.lowerBound(qMakePair(datetime, 0LL));
	if (count < 0) count = 0;
	while (it != set2.begin() && count--) {
		--it;
		ret.push_back(it.key().second);
	}
	return ret;
}

QVector<long long> EventManager::get_events_between(long long datetime1, long long datetime2) {
	check_op();
	
	QVector<long long> ret;
	// QMessageBox::warning(NULL, "warning", "heiheihei", QMessageBox::Yes);
	// QMap<QPair<long long, long long> > set2;
	// std::map<QPair<long long, long long>, bool> m = set2.toStdMap();
	// QMessageBox::warning(NULL, "warning", "heiheihei", QMessageBox::Yes);
	// std::map<QPair<long long, long long>, bool>::iterator it = m.lower_bound(qMakePair(datetime1, 0LL));
	
	// QMessageBox::warning(NULL, "in query", "this = " + QString::number((int)this, 10), QMessageBox::Yes);
	
	// QMap<QPair<long long, long long>, bool> set2 = this->set2;
	
	QMap<QPair<long long, long long>, bool>::iterator it = set2.lowerBound(qMakePair(datetime1, 0LL));
	
	// QMessageBox::warning(NULL, "warning", it == m.end() ? "233" : "hahaha", QMessageBox::Yes);
	while (it != set2.end() && it.key().first < datetime2) {
		ret.push_back(it.key().second);
		++it;
	}
	// QMessageBox::warning(NULL, "warning", it == set2.end() ? "233" : "hahaha", QMessageBox::Yes);
	return ret;
}

MyEvent EventManager::get_event_by_ID(long long ID) {
	check_op();
	
	MyEvent ret;
	ret.ID = ID;
	QMap<MyEvent, bool>::iterator it = set1.find(ret);
	if (it == set1.end()) {
		ret.ID = -1;
		return ret;
	} else {
		return it.key();
	}
}

void EventManager::check_op() {
	while (checking);
	checking = true;
	
	update_events();
	
	if (op == 0) {
		checking = false;
		return;
	}
	
	if (op == 1) {  // add event
		op = -1;
		add_event(_datetime, _name, _location, _repeat_type, _repeat_interval);
		op = 0;
	} else if (op == 2) {
		op = -1;
		del_event(_ID);
		op = 0;
	} else if (op == 3) {
		op = -1;
		del_event(_ID);
		add_event(_datetime, _name, _location, _repeat_type, _repeat_interval, _ID);
		op = 0;
	} else {
		op = 0;
	}
	
	checking = false;
}

void EventManager::update_events() {
	QDateTime cur_time = QDateTime::currentDateTime();
	int yy, mm, dd, h, m, s;
	yy = cur_time.date().year();
	mm = cur_time.date().month();
	dd = cur_time.date().day();
	h = cur_time.time().hour();
	m = cur_time.time().minute();
	s = cur_time.time().second();
	
	while (1) {
		long long ll_cur_time = datetime_to_ll(yy, mm, dd, h, m, s);
		auto it = set2_r.lowerBound(qMakePair(ll_cur_time + 1, 0));
		QVector<long long> res;
		while (it != set2_r.begin()) {
			--it;
			res.push_back(it.key().second);
		}
		
		if (res.empty()) break;
		// if (res.size()) QMessageBox::warning(NULL, "warning", QString::number(res.size(), 10), QMessageBox::Yes);
		
		for (auto id : res) {
			MyEvent tmp;
			tmp.ID = id;
			MyEvent e = set1.find(tmp).key();
			int yy, mm, dd;
			int_to_date(e.datetime / 86400, yy, mm, dd);
			QDate d(yy, mm, dd);
			Q_ASSERT(e.repeat_interval > 0);
			switch (e.repeat_type) {
				case 1:
					d = d.addDays(e.repeat_interval);
					break;
				case 2:
					d = d.addDays(e.repeat_interval * 7);
					break;
				case 3:
					d = d.addMonths(e.repeat_interval);
					break;
				case 4:
					d = d.addYears(e.repeat_interval);
					break;
				default:
					break;
			}
			// QMessageBox::warning(NULL, "type" + QString::number(e.repeat_type), d.toString("yyyy MM dd"), QMessageBox::Yes);
			long long ll_dt = e.datetime % 86400 + date_to_int(d.year(), d.month(), d.day()) * 86400LL;
			set2.remove(qMakePair(e.datetime, id));
			set2_r.remove(qMakePair(e.datetime, id));
			set1.remove(e);
			e.datetime = ll_dt;
			set1.insert(e, 0);
			set2.insert(qMakePair(e.datetime, id), 0);
			set2_r.insert(qMakePair(e.datetime, id), 0);
		}
	}
}

void EventManager::set_add_event(long long datetime, QString name, QString location, int repeat_type, int repeat_interval) {
	check_op();
	
	_datetime = datetime;
	_name = name;
	_location = location;
	_repeat_type = repeat_type;
	_repeat_interval = repeat_interval;
	op = 1;
}

void EventManager::del_event(long long ID) {
	MyEvent tmp;
	tmp.ID = ID;
	QMap<MyEvent, bool>::iterator it = set1.find(tmp);
	if (it == set1.end()) {
		return;
	}
	MyEvent e = it.key();
	set2.remove(qMakePair(e.datetime, ID));
	set2_r.remove(qMakePair(e.datetime, ID));
	set1.remove(e);
}

void EventManager::set_del_event(long long ID) {
	check_op();
	
	_ID = ID;
	op = 2;
}

void EventManager::set_edit_event(long long ID, long long datetime, QString name, QString location,
									int repeat_type, int repeat_interval) {
	check_op();
	
	_ID = ID;
	_datetime = datetime;
	_name = name;
	_location = location;
	_repeat_type = repeat_type;
	_repeat_interval = repeat_interval;
	op = 3;
}

QMap<MyEvent, bool> EventManager::get_all_events() {
	check_op();
	
	return set1;
}

void EventManager::clear_all_events() {
	check_op();
	
	set1.clear();
	set2.clear();
	set2_r.clear();
}
