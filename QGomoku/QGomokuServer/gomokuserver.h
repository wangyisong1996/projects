#ifndef GOMOKUSERVER_H
#define GOMOKUSERVER_H

// <del> Try not to use Q OBJECT </del>

#include <QTcpServer>
#include <QTcpSocket>
#include <QUuid>
#include <QCryptographicHash>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QByteArray>
#include <QMessageBox>

const int SERVER_PORT = 53000;

const int MAX_CONNECTIONS_PER_TICK = 1000;

const int MAX_INSTRUCTION_SIZE = 100;

const int MIN_USERNAME_LEN = 3;
const int MAX_USERNAME_LEN = 20;

const char PROTOCOL_VERSION[4] = {16, 9, 4, 8};

inline QByteArray MD5(const QByteArray &s) {
	return QCryptographicHash::hash(s, QCryptographicHash::Md5);
}

// !!!!!!!!!!

inline QByteArray pw_to_hash1(const QByteArray &pw) {
	// take care of it
	return MD5(MD5(pw) + QByteArray("I love Qt, "));
}

inline QByteArray hash1_to_hash2(const QByteArray &hash1) {
	// take care of it
	return MD5(hash1 + QByteArray("Qt makes me happy !!!"));
}

// ----------

struct UserInfo {
	int uid;
	QUuid uuid;
	QString username;
	QByteArray pw_hash_2;
	int win;
	int all;
	double rating;
	int rank;  // cached rank
};

struct Instruction {
	char data[MAX_INSTRUCTION_SIZE];
	int len;
	
	Instruction() {
		len = 0;
	}
	
	void read(QTcpSocket *socket) {
		if (len < MAX_INSTRUCTION_SIZE) {
			int ret = socket->read(data + len, MAX_INSTRUCTION_SIZE - len);
			if (ret > 0) {
				len += ret;
			}
		}
	}
	
	int has_instruction();  // -1: 不清真
	
	int instruction_len;
	
	void remove_instruction();
};


const int USER_STATE_IDLE = 1;
const int USER_STATE_FINDING = 2;
const int USER_STATE_INGAME = 3;



const int BOARD_SIZE = 15;

struct GameInfo {
	int board[BOARD_SIZE][BOARD_SIZE];
	// 0 -- none, 2 -- black, 3 -- white
	
	int uid_b, uid_w;
	
	int b_x, b_y, w_x, w_y;
	
	int code;
	
	bool is_black_turn;
	bool is_started;
	
	QDateTime last_op_time;
	
	QDateTime start_time;
	
	GameInfo(int uid_b, int uid_w);
	GameInfo() {}
	
	int next_move_uid();
	
	bool check_game_over(int &winner);
};

class GomokuServer : public QObject {
	Q_OBJECT
	
public:
	GomokuServer();
	~GomokuServer();
	
	int n_users() {
		return (int)userinfo.size();
	}
	
	int n_connections() {
		return (int)socket_to_uid.size();
	}
	
	int n_try_logins() {
		return cnt_try_login;
	}
	
	int n_onlines() {
		return (int)uid_to_socket.size();
	}
	
	int tick_rate() {
		return (int)ticks.size();
	}
	
	long long n_total_connections() {
		return total_connections;
	}
	
	long long n_login_oks() {
		return login_oks;
	}
	
	int n_max_connection() {
		return max_connection;
	}
	
	int n_max_online() {
		return max_online;
	}
	
	int n_total_games() {
		return cnt_games;
	}
	
	int n_running_games() {
		return (int)games.size();
	}
	
	int n_max_running_games() {
		return max_running_games;
	}
	
	int n_in_game_users() {
		return cnt_in_game_users;
	}
	
	int n_max_in_game_users() {
		return max_in_game_users;
	}
	
	int n_running_secs() {
		return running_secs;
	}

private:
	// tick related
	QMap<QDateTime, bool> ticks;
	
	
	// server status related
	long long total_connections = 0;
	long long login_oks = 0;
	int max_connection = 0;
	int max_online = 0;
	
	// server related
	
	QTcpServer *tcpserver = NULL;
	
	void init_server();
	
	QMap<int, QTcpSocket*> uid_to_socket;
	QMap<QTcpSocket*, int> socket_to_uid;
	
	QMap<QTcpSocket*, QDateTime> socket_to_last_HBT;
	QMap<QTcpSocket*, bool> socket_to_has_HB;
	QMap<QTcpSocket*, bool> socket_to_has_sent_HB;
	
	QMap<QTcpSocket*, Instruction> socket_to_i;
	
	int cnt_try_login = 0;
	
	int running_secs = 0;
	QDateTime server_start_time = QDateTime::currentDateTime();
	
	void get_new_connections();
	void connect_client(QTcpSocket *socket);
	void disconnect_client(QTcpSocket *socket);
	
	void process_existing_connections();
	// returns: has been disconnected
	bool process_socket(QTcpSocket *socket);
	void send_to_socket(QTcpSocket *socket, const QByteArray &data);
	
	void send_to_user(int uid, const QByteArray &data);
	
	// database related
	
	QVector<UserInfo> userinfo;
	bool add_user(const UserInfo &u);
	
	QMap<QUuid, int> user_uuid_to_uid;
	QMap<QString, int> user_name_to_uid;
	
	
	// user info related
	
	QMap<int, int> uid_to_status;  // 只有在线的用户才有
	
	void process_user_login(int uid);
	
	
	// game related
	
	// 不需要保证这个用户在线
	QMap<int, QUuid> uid_to_gameid;
	QMap<QUuid, GameInfo> games;
	
	int cnt_games = 0;
	int max_running_games = 0;
	int cnt_in_game_users = 0;
	int max_in_game_users = 0;
	
	QDateTime last_pair_time = QDateTime::currentDateTime();
	
	// returns: bool success
	bool process_move_instruction(int uid, int code, char _x, char _y);
	
	void process_all_games();
	
	void restore_game(int uid);
	
	void game_random_move(QUuid id);
	void game_move(QUuid id, int x, int y);
	
	void pair_users();
	void start_game(int uid1, int uid2);

private slots:
	void tick();

};

#endif
