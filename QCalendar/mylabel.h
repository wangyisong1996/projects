#ifndef MYLABEL_H
#define MYLABEL_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QString>

class MyLabel : public QLabel
{
	Q_OBJECT

public:
	explicit MyLabel(QString str, QWidget *parent = 0);
	~MyLabel();
	
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void enterEvent(QEvent *e);
	void leaveEvent(QEvent *e);
	
	QString bgcolor;
	QString hovercolor, presscolor;
	
	void set_bgcolor(QString s);
	
	void setText(QString s);  // override
	
signals:
	void on_mouseenter();
	void on_mouseleave();
	
	void on_mousedown();
	void on_mouseup();

	
};

#endif
