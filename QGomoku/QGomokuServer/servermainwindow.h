#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>

#include "gomokuserver.h"

namespace Ui {
class ServerMainWindow;
}

class ServerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ServerMainWindow(QWidget *parent = 0);
	~ServerMainWindow();
	

private:
	Ui::ServerMainWindow *ui;
	
	GomokuServer *server = NULL;

private slots:
	void on_timer();
	
};

#endif // SERVERMAINWINDOW_H
