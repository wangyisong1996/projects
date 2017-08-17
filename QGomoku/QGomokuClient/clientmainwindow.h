#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QByteArray>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDateTime>
#include <QString>
#include <QPainter>
#include <QImage>
#include <QList>
#include <QFontDatabase>
#include <QUuid>
#include <QtMath>
#include <QImageReader>

#define NUM(x) QString::number(x, 10)
#define CURTIME() QDateTime::currentDateTime()

#define PI 3.141592653589793

const int MAX_DATA_SIZE = 10000;

const int BOARD_SIZE = 15;

struct UserInfo {
	QString username;
	int win;
	int all;
	int rating;
	int rank;
	
	void clear() {
		username = "";
	}
	
	QString to_string();
};

const int STATE_IN_LOBBY = 1;
const int STATE_IN_GAME = 2;

namespace Ui {
class ClientMainWindow;
}

class ClientMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ClientMainWindow(QWidget *parent, QTcpSocket *socket, QByteArray uuid);
	~ClientMainWindow();

private:
	Ui::ClientMainWindow *ui;
	
	void paintEvent(QPaintEvent *);
	void keyPressEvent(QKeyEvent *);
	void mousePressEvent(QMouseEvent *);
	
	QTimer *timer = NULL;
	QTcpSocket *socket = NULL;
	QByteArray user_uuid;
	
	void disconnect();
	void send(QByteArray s);
	
	
	// data (instruction)
	
	char data[MAX_DATA_SIZE];
	int data_size = 0;
	int data_len = 0;
	
	void try_to_read_data();
	int has_data();  // -1: meat
	void remove_data();
	void process_data();
	void process_input();
	
	
	// player data
	
	UserInfo my_user_info, oppo_user_info;
	QList<UserInfo *> user_info_queries;
	
	void query_user_info(UserInfo *u, QByteArray uuid);
	
	
	// game
	
	int board[BOARD_SIZE][BOARD_SIZE];
	bool game_started = false;
	int my_x = 7, my_y = 7;
	int oppo_x = 7, oppo_y = 7;
	int my_player;
	
	bool is_my_turn;
	bool game_over;
	int over_x1, over_y1, over_x2, over_y2;
	
	bool space_pressed = false;
	
	QByteArray game_code;
	
	QDateTime cd_start_time;
	int cd_player = 0;  // 非2,3表示没有
	
	// 如果游戏已经结束就先啥都不干
	void start_next_round();
	
	void start_my_round();
	void game_move(int player, int x, int y);
	void random_move();
	
	void check_game_over();
	
	
	// UI
	int state = STATE_IN_LOBBY;
	int next_state = 0;
	QDateTime state_trans_time;
	bool in_transition = false;
	
	bool is_finding = false;
	
	bool has_notification = false;
	QDateTime notification_time;
	QString notification_str;
	
	void trans_state(int s, int delay = 0);
	
	void draw();
	void draw_state(QPainter &p, int state);
	
	bool check_dangerous_grid(int x, int y);
	
	void draw_board(QPainter &p, int x0, int y0, int size);
	void draw_player_cd(QPainter &p);
	
	void show_notification(QString s, int delay = 0);
	void draw_notification(QPainter &p);
	
	QFontDatabase font_db;
	
	
	// input
	
	int R_press_cnt = 0;
	int up_press_cnt = 0;
	int down_press_cnt = 0;
	int left_press_cnt = 0;
	int right_press_cnt = 0;
	int space_press_cnt = 0;

private slots:
	void tick();


};

#endif // CLIENTMAINWINDOW_H
