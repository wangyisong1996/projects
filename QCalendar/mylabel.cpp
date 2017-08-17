#include "mylabel.h"

#include <QFrame>

MyLabel::MyLabel(QString str, QWidget *parent) : QLabel(str, parent) {
	
}

MyLabel::~MyLabel() {}

void MyLabel::mousePressEvent(QMouseEvent *e) {
	if (e->button() != Qt::LeftButton) {
		return;
	}
	if (presscolor != "") {
		this->setStyleSheet("background-color: " + presscolor);
	}
	emit on_mousedown();
}

void MyLabel::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() != Qt::LeftButton) {
		return;
	}
	if (hovercolor != "") {
		this->setStyleSheet("background-color: " + hovercolor);
	}
	emit on_mouseup();
}

void MyLabel::enterEvent(QEvent *e) {
	if (hovercolor != "") this->setStyleSheet("background-color: " + hovercolor);
	emit on_mouseenter();
}

void MyLabel::leaveEvent(QEvent *e) {
	this->setStyleSheet(bgcolor == "" ? "" : "background-color: " + bgcolor);
	emit on_mouseleave();
}


void MyLabel::set_bgcolor(QString s) {
	bgcolor = s;
	this->setStyleSheet(bgcolor == "" ? "" : "background-color: " + bgcolor);
}

void MyLabel::setText(QString s) {
	if (s != "" && s[0] != '<') {
		s = s.toHtmlEscaped();
	}
	s = "<div style=\"font-family: 微软雅黑; font-weight: normal;\">" + s + "</div>";
	QLabel::setText(s);
}