#include "servermainwindow.h"
#include "ui_servermainwindow.h"

#include <QTimer>

#define NUM(x) QString::number(x, 10)

ServerMainWindow::ServerMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ServerMainWindow)
{
	ui->setupUi(this);
	
	server = new GomokuServer();
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
	timer->start(100);
}

ServerMainWindow::~ServerMainWindow()
{
	delete ui;
	delete server;
}

void ServerMainWindow::on_timer() {
	int n_users = server->n_users();
	int n_conns = server->n_connections();
	int n_try_logins = server->n_try_logins();
	int n_onlines = server->n_onlines();
	
	int tick_rate = server->tick_rate();
	
	long long n_total_connections = server->n_total_connections();
	long long n_login_oks = server->n_login_oks();
	int n_max_connection = server->n_max_connection();
	int n_max_online = server->n_max_online();
	
	int n_total_games = server->n_total_games();
	int n_running_games = server->n_running_games();
	int n_max_running_games = server->n_max_running_games();
	int n_in_game_users = server->n_in_game_users();
	int n_max_in_game_users = server->n_max_in_game_users();
	
	int n_running_secs = server->n_running_secs();
	
	QString s = "";
	
	s += "Users: " + NUM(n_users) + "\n";
	s += "Connections: " + NUM(n_conns) + "\n";
	s += "Try logins: " + NUM(n_try_logins) + "\n";
	s += "Onlines: " + NUM(n_onlines) + "\n";
	s += "Tick rate: " + NUM(tick_rate) + "\n";
	s += "Total connections: " + NUM(n_total_connections) + "\n";
	s += "Login OK: " + NUM(n_login_oks) + "\n";
	s += "Max connections: " + NUM(n_max_connection) + "\n";
	s += "Max online: " + NUM(n_max_online) + "\n";
	s += "Total games: " + NUM(n_total_games) + "\n";
	s += "Running games: " + NUM(n_running_games) + "\n";
	s += "Max running games: " + NUM(n_max_running_games) + "\n";
	s += "In game users: " + NUM(n_in_game_users) + "\n";
	s += "Max in game users: " + NUM(n_max_in_game_users) + "\n";
	
	s += "\n";
	s += "Uptime: " + NUM(n_running_secs / 86400) + " days ";
	s += QTime(0, 0, 0).addSecs(n_running_secs % 86400).toString("hh : mm : ss") + "\n";
	
	ui->label1->setText(s);
}
