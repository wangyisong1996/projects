#include "gomokuserver.h"

#define NUM(x) QString::number(x, 10)
#define CURTIME() QDateTime::currentDateTime()

#include <cstdlib>
#include <algorithm>
#include <fstream>

#include <QtMath>

static inline int RAND_INT() {
	QByteArray tmp = QUuid::createUuid().toRfc4122();
	union {
		char c[4];
		int i;
	} tmp2;
	for (int i = 0; i < 4; i++) {
		tmp2.c[i] = tmp[i];
	}
	return tmp2.i;
}

static inline QByteArray INT_TO_BA(int x) {
	union {
		char c[4];
		int i;
	} tmp;
	tmp.i = x;
	return QByteArray(tmp.c, 4);
}

GomokuServer::GomokuServer() {
	init_server();
	
	tcpserver = new QTcpServer(this);
	
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
	
	bool ret = tcpserver->listen(QHostAddress::Any, SERVER_PORT);
	if (!ret) {
		QMessageBox::warning(NULL, "Error", "Can't listen on port " + NUM(SERVER_PORT));
		exit(EXIT_FAILURE);
	}
	
	timer->start(40);
	
}

GomokuServer::~GomokuServer() {
	
}

static inline UserInfo gen_test_user(int uid, QString name, QString pw) {
	UserInfo ret;
	ret.uid = uid;
	ret.uuid = QUuid::createUuid();
	ret.username = name;
	ret.pw_hash_2 = hash1_to_hash2(pw_to_hash1(pw.toUtf8()));
	ret.win = ret.all = 0;
	ret.rating = 1500.0;
	ret.rank = uid + 1;
	return ret;
}

void GomokuServer::init_server() {
	// TODO: really do nothing ?
	
	// add_user(gen_test_user(0, "test01", "test233"));
	// add_user(gen_test_user(1, "test02", "iloveqt"));
	
	std::ifstream ifs("user_list.txt", std::ifstream::in);
	int n;
	ifs >> n;
	for (int i = 0; i < n; i++) {
		std::string u, p;
		ifs >> u >> p;
		add_user(gen_test_user(i, u.data(), p.data()));
	}
	ifs.close();
}

bool GomokuServer::add_user(const UserInfo &u) {
	if ((int)userinfo.size() != u.uid) return false;
	userinfo.push_back(u);
	int uid = u.uid;
	user_uuid_to_uid[u.uuid] = uid;
	user_name_to_uid[u.username] = uid;
	return true;
}

void GomokuServer::get_new_connections() {
	int tmp = MAX_CONNECTIONS_PER_TICK;
	QTcpSocket *socket = NULL;
	while (tmp--) {
		socket = tcpserver->nextPendingConnection();
		if (socket == NULL) break;
		connect_client(socket);
	}
}

void GomokuServer::connect_client(QTcpSocket *socket) {
	socket->setReadBufferSize(MAX_INSTRUCTION_SIZE);
	socket_to_uid[socket] = -1;
	socket_to_last_HBT[socket] = CURTIME();
	socket_to_has_HB[socket] = true;
	socket_to_has_sent_HB[socket] = true;
	socket_to_i[socket] = Instruction();
	
	++total_connections;
	max_connection = qMax(max_connection, (int)socket_to_uid.size());
}

void GomokuServer::disconnect_client(QTcpSocket *socket) {
	if (socket == NULL) return;
	
	int uid = socket_to_uid[socket];
	if (uid >= 0 && uid < (int)userinfo.size()) {
		// TODO: delete user info ?
		uid_to_socket.remove(uid);
		uid_to_status.remove(uid);
		
		// if in game?
		if (uid_to_gameid.count(uid) > 0) {
			--cnt_in_game_users;
		}
	}
	if (socket_to_uid[socket] == -2) {
		--cnt_try_login;
	}
	socket_to_uid.remove(socket);
	socket_to_last_HBT.remove(socket);
	socket_to_has_HB.remove(socket);
	socket_to_has_sent_HB.remove(socket);
	socket_to_i.remove(socket);
	socket->close();
	delete socket;
}

// 0: No; 1: Yes; -1: 不清真
int Instruction::has_instruction() {
	if (len <= 0) {
		return 0;
	}
	char i_code = data[0];
	switch (i_code) {
		case 0x01:
			instruction_len = 1 + 4;
			break;
		case 0x0f:
			instruction_len = 1;
			break;
		case 0x10:
			if (len <= 1) return 0;
			if (data[1] < MIN_USERNAME_LEN || data[1] > MAX_USERNAME_LEN) {
				return -1;
			} else {
				instruction_len = 1 + 1 + data[1] + 16;
			}
			break;
		case 0x11:
			instruction_len = 1 + 16;
			break;
		case 0x20:
			instruction_len = 1;
			break;
		case 0x2f:
			instruction_len = 1;
			break;
		case 0x42:
			instruction_len = 1 + 4 + 1 + 1;
			break;
		default:
			return -1;
	}
	return len >= instruction_len;
}

// No validation
void Instruction::remove_instruction() {
	if (instruction_len < 0 || instruction_len > MAX_INSTRUCTION_SIZE) return;
	if (instruction_len > len) return;
	for (int i = instruction_len; i < len; i++) {
		data[i - instruction_len] = data[i];
	}
	len -= instruction_len;
}

void GomokuServer::send_to_socket(QTcpSocket *socket, const QByteArray &data) {
	socket->write(data);
}

void GomokuServer::send_to_user(int uid, const QByteArray &data) {
	if (uid_to_socket.count(uid) == 0) {
		return;
	}
	this->send_to_socket(uid_to_socket[uid], data);
}

#define DISCONNECT() (disconnect_client(socket), 1)
#define SEND(s) send_to_socket(socket, s)

bool GomokuServer::process_socket(QTcpSocket *socket) {
	int uid = socket_to_uid[socket];
	Instruction &I = socket_to_i[socket];
	char i_code = I.data[0];
	if (uid < 0) {
		if (uid == -1) {
			// just connected
			// 必须为[01][ver]，否则不清真
			if (i_code == 0x01) {
				for (int i = 0; i < 4; i++) {
					if (I.data[i + 1] != PROTOCOL_VERSION[i]) {
						return DISCONNECT();
					}
				}
				// version matched
				I.remove_instruction();
				SEND("\x01\x01");
				socket_to_uid[socket] = -2;
				++cnt_try_login;
				socket_to_last_HBT[socket] = CURTIME();
			} else {
				return DISCONNECT();
			}
		} else if (uid == -2) {
			// 已确定版本正确，等待登录
			// 必须为login指令
			if (i_code == 0x10) {
				// 检查用户名是否清真
				int len_username = I.data[1];
				QString username;
				for (int i = 0; i < len_username; i++) {
					char tmp = I.data[2 + i];
					if ((tmp < 'a' || tmp > 'z') && (tmp < 'A' || tmp > 'Z') && (tmp < '0' || tmp > '9') && tmp != '_') {
						return DISCONNECT();
					}
					username.append(tmp);
				}
				auto it = user_name_to_uid.find(username);
				if (it == user_name_to_uid.end()) {
					// No such user
					return DISCONNECT();
				}
				int _uid = it.value();
				if (uid_to_status.find(_uid) != uid_to_status.end()) {
					// Already logged in
					return DISCONNECT();
				}
				QByteArray pw_hash1;
				for (int i = 0; i < 16; i++) {
					pw_hash1.append(I.data[2 + len_username + i]);
				}
				QByteArray pw_hash2 = hash1_to_hash2(pw_hash1);
				if (pw_hash2 != userinfo[_uid].pw_hash_2) {
					// Wrong password
					return DISCONNECT();
				}
				I.remove_instruction();
				
				--cnt_try_login;
				
				socket_to_uid[socket] = _uid;
				uid_to_socket[_uid] = socket;
				
				
				QUuid uuid = userinfo[_uid].uuid;
				SEND(QByteArray("\x10\x01") + uuid.toRfc4122());
				socket_to_last_HBT[socket] = CURTIME();
				socket_to_has_HB[socket] = true;
				socket_to_has_sent_HB[socket] = false;
				
				process_user_login(_uid);
				
			} else {
				return DISCONNECT();
			}
		} else {
			// 什么鬼情况？
			return DISCONNECT();
		}
	} else {
		// uid >= 0
		if (uid >= (int)userinfo.size()) {
			// 什么鬼情况？
			return DISCONNECT();
		}
		
		// special case: HB (heartbeat)
		if (i_code == 0x0f) {
			if (socket_to_has_HB[socket]) {
				// client之前已经发过HB，不清真
				return DISCONNECT();
			}
			socket_to_has_HB[socket] = true;
			socket_to_has_sent_HB[socket] = false;
			socket_to_last_HBT[socket] = CURTIME();
			I.remove_instruction();
			return false;
		}
		
		switch (i_code) {
			case 0x11: {
				char tmp[16];
				for (int i = 0; i < 16; i++) {
					tmp[i] = I.data[1 + i];
				}
				QUuid uuid = QUuid::fromRfc4122(QByteArray(tmp, 16));
				I.remove_instruction();
				auto it = user_uuid_to_uid.find(uuid);
				if (it == user_uuid_to_uid.end()) {
					SEND(QByteArray("\x11\x00", 2));  // failed
				} else {
					int uid = it.value();
					int win = userinfo[uid].win;
					int total = userinfo[uid].all;
					int rating = (int)userinfo[uid].rating;
					int rank = userinfo[uid].rank;
					
					QByteArray username = userinfo[uid].username.toUtf8();
					int len = username.length();
					if (len <= 0 || len > 20) {
						SEND(QByteArray("\x11\x00", 2));
						break;
					}
					
					QByteArray data = "\x11\x01";
					data += (char)len;
					data += username;
					
					union {
						char c[16];
						int i[4];
					} tmp;
					tmp.i[0] = win;
					tmp.i[1] = total;
					tmp.i[2] = rating;
					tmp.i[3] = rank;
					
					SEND(data + QByteArray(tmp.c, 16));
				}
				break;
			}
			case 0x20: {
				// random game request
				I.remove_instruction();
				if (uid_to_status[uid] != USER_STATE_IDLE) {
					// 猪肉，不过可能是误点
					break;
				}
				uid_to_status[uid] = USER_STATE_FINDING;
				SEND("\x20");
				break;
			}
			case 0x2f: {
				// cancel random game
				I.remove_instruction();
				if (uid_to_status[uid] != USER_STATE_FINDING) {
					// 同上
					break;
				}
				uid_to_status[uid] = USER_STATE_IDLE;
				SEND("\x2f");
				break;
			}
			case 0x42: {
				// move
				union {
					int i;
					char c[4];
				} code;
				for (int i = 0; i < 4; i++) {
					code.c[i] = I.data[1 + i];
				}
				char x = I.data[5], y = I.data[6];
				I.remove_instruction();
				process_move_instruction(uid, code.i, x, y);
				break;
			}
			default: {
				// 不清真
				return DISCONNECT();
			}
		}
	}
	
	return false;
}

#undef DISCONNECT
#undef SEND

void GomokuServer::process_existing_connections() {
	QVector<QTcpSocket*> sockets;
	for (QMap<QTcpSocket*, int>::iterator it = socket_to_uid.begin(); it != socket_to_uid.end(); ++it) {
		sockets.push_back(it.key());
	}
	for (QTcpSocket *socket : sockets) {
		if (socket->state() != QTcpSocket::ConnectedState) {
			disconnect_client(socket);
			continue;
		}
		
		socket_to_i[socket].read(socket);
		
		int has_i = socket_to_i[socket].has_instruction();
		if (has_i == -1) {
			disconnect_client(socket);
			continue;
		}
		if (has_i == 1) {
			if (process_socket(socket)) continue;
		}
		if (socket_to_last_HBT[socket].msecsTo(CURTIME()) > 5000) {
			if (!socket_to_has_sent_HB[socket]) {
				send_to_socket(socket, "\x0f");
				socket_to_has_sent_HB[socket] = true;
				socket_to_has_HB[socket] = false;
				socket_to_last_HBT[socket] = CURTIME();
			} else {
				disconnect_client(socket);
			}
		}
	}
}

// This is called after sending the [login ok] instruction
void GomokuServer::process_user_login(int uid) {
	uid_to_status[uid] = USER_STATE_IDLE;
	
	++login_oks;
	max_online = qMax(max_online, (int)uid_to_socket.size());
	
	// TODO: game ?
	if (uid_to_gameid.count(uid) > 0) {
		++cnt_in_game_users;
		max_in_game_users = qMax(max_in_game_users, cnt_in_game_users);
		// restore the game
		restore_game(uid);
	}
}

GameInfo::GameInfo(int uid_b, int uid_w) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = 0;
		}
	}
	this->uid_b = uid_b;
	this->uid_w = uid_w;
	this->last_op_time = CURTIME();
	is_black_turn = false;
	is_started = false;
	start_time = CURTIME().addMSecs(3000);
	b_x = b_y = w_x = w_y = 7;
}

int GameInfo::next_move_uid() {
	if (!is_started) {
		return -1;
	}
	return is_black_turn ? uid_b : uid_w;
}

bool GameInfo::check_game_over(int &winner) {
	static const int dx[4] = {1, 1, 1, 0};
	static const int dy[4] = {-1, 0, 1, 1};
	
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			for (int id = 0; id < 4; id++) {
				int tx = i + dx[id] * 4, ty = j + dy[id] * 4;
				if (tx < 0 || tx >= BOARD_SIZE || ty < 0 || ty >= BOARD_SIZE) {
					continue;
				}
				int cnt_b = 0, cnt_w = 0;
				for (int k = 0; k < 5; k++) {
					int tmp = board[tx][ty];
					if (tmp == 2) {
						++cnt_b;
					} else if (tmp == 3) {
						++cnt_w;
					}
					tx -= dx[id], ty -= dy[id];
				}
				if (cnt_b == 5 || cnt_w == 5) {
					winner = cnt_b == 5 ? 2 : 3;
					return true;
				}
			}
		}
	}
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			// maya我怎么写成!=0了......
			if (board[i][j] != 2 && board[i][j] != 3) {
				return false;
			}
		}
	}
	winner = 3;  // draw ?
	return true;
}

bool GomokuServer::process_move_instruction(int uid, int code, char _x, char _y) {
	if (uid_to_gameid.count(uid) == 0) {
		return false;
	}
	QUuid id = uid_to_gameid[uid];
	GameInfo &g = games[id];
	if (g.next_move_uid() != uid) {
		return false;
	}
	if (g.code != code) {
		return false;
	}
	if (_x < 0 || _x >= BOARD_SIZE || _y < 0 || _y >= BOARD_SIZE) {
		return false;
	}
	if (g.board[(int)_x][(int)_y] != 0) {
		return false;
	}
	this->game_move(id, _x, _y);
	return true;
}

void GomokuServer::process_all_games() {
	QVector<QUuid> game_ids;
	for (auto it = games.begin(); it != games.end(); it++) {
		game_ids.push_back(it.key());
	}
	for (auto id : game_ids) {
		GameInfo &g = games[id];
		if (!g.is_started && CURTIME() > g.start_time) {
			g.is_started = true;
			g.last_op_time = CURTIME();
			g.is_black_turn = true;
			g.code = RAND_INT();
			send_to_user(g.uid_b, "\x41");
			send_to_user(g.uid_w, "\x41");
			send_to_user(g.uid_b, "\x42" + INT_TO_BA(g.code));
		}
		if (g.last_op_time.msecsTo(CURTIME()) > 25000) {
			game_random_move(id);
		}
	}
}

void GomokuServer::restore_game(int uid) {
	uid_to_status[uid] = USER_STATE_INGAME;
	QUuid id = uid_to_gameid[uid];
	GameInfo &g = games[id];
	
	
	
	if (!g.is_started) {
		QByteArray data = "\x41";
		if (g.uid_b == uid) {
			data += "\x01" + userinfo[g.uid_w].uuid.toRfc4122();
		} else {
			data += QByteArray("\x00", 1) + userinfo[g.uid_b].uuid.toRfc4122();
		}
		send_to_user(uid, data);
	} else {
		QByteArray data = "\x4f";
		if (g.uid_b == uid) {
			data += "\x01" + userinfo[g.uid_w].uuid.toRfc4122();
			data += (char)g.b_x;
			data += (char)g.b_y;
			data += (char)g.w_x;
			data += (char)g.w_y;
		} else {
			data += QByteArray("\x00", 1) + userinfo[g.uid_b].uuid.toRfc4122();
			data += (char)g.w_x;
			data += (char)g.w_y;
			data += (char)g.b_x;
			data += (char)g.b_y;
		}
		unsigned char A[57];
		for (int i = 0; i < 57; i++) {
			A[i] = 0;
		}
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				int id = i * BOARD_SIZE + j;
				A[id / 4] += (int)g.board[i][j] << ((id % 4) * 2);
			}
		}
		data += QByteArray((char *)A, 57);
		if (g.next_move_uid() == uid) {
			data += INT_TO_BA(g.code);
			int t = 25000 - g.last_op_time.msecsTo(CURTIME());
			if (t > 20000) t = 20000;
			if (t <= 0) {
				t = 1;
			}
			data += INT_TO_BA(t);
		}
		// QMessageBox::warning(NULL, "send to u:", NUM(data[0]) + " " + NUM(data[1]));
		send_to_user(uid, data);
	}
}

void GomokuServer::game_random_move(QUuid id) {
	if (games.count(id) == 0) {
		return;
	}
	GameInfo &g = games[id];
	if (g.is_started == false) {
		return;
	}
	int cnt = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			cnt += g.board[i][j] == 0;
		}
	}
	
	if (cnt == 0) {
		return;
	}
	
	
	int tmp3 = RAND_INT() % cnt;
	if (tmp3 < 0) tmp3 += cnt;
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (g.board[i][j] == 0) {
				if (tmp3 == 0) {
					return game_move(id, i, j), void();
				} else {
					--tmp3;
				}
			}
		}
	}
}

void GomokuServer::game_move(QUuid id, int x, int y) {
	if (games.count(id) == 0) {
		return;
	}
	GameInfo &g = games[id];
	if (g.is_started == false) {
		return;
	}
	if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
		return;
	}
	if (g.board[x][y] != 0) {
		return;
	}
	
	// 需要发送的东西：
	// 操作方的[43]（已收到操作） 不管他是否进行了操作
	// 另一方的[44]（对方操作）
	// 如果对战结束，则还需要做结算处理
	
	int uid_A = g.is_black_turn ? g.uid_b : g.uid_w;
	int uid_B = g.is_black_turn ? g.uid_w : g.uid_b;
	
	if (g.is_black_turn) {
		g.b_x = x;
		g.b_y = y;
	} else {
		g.w_x = x;
		g.w_y = y;
	}
	
	g.board[x][y] = g.is_black_turn ? 2 : 3;
	g.is_black_turn ^= 1;
	g.last_op_time = CURTIME();
	
	send_to_user(uid_A, (QByteArray("\x43") + (char)x) + (char)y);
	send_to_user(uid_B, (QByteArray("\x44") + (char)x) + (char)y);
	
	int winner = 0;
	if (g.check_game_over(winner)) {
		int uid_winner = winner == 2 ? g.uid_b : g.uid_w;
		int uid_loser = winner == 2 ? g.uid_w : g.uid_b;
		
		if (uid_to_socket.count(uid_winner) == 0 &&
			uid_to_socket.count(uid_loser) == 0) {
			// 这是个feature，不是bug
			return;
		}
		
		// 结算rating？
		double &R_1 = userinfo[uid_winner].rating;
		double &R_2 = userinfo[uid_loser].rating;
		
		++userinfo[uid_winner].win;
		++userinfo[uid_winner].all;
		++userinfo[uid_loser].all;
		
		double E_1 = 1.0 / (1.0 + qExp(qLn(10.0) / 400.0 * (R_2 - R_1)));
		double E_2 = 1.0 / (1.0 + qExp(qLn(10.0) / 400.0 * (R_1 - R_2)));
		
		double K = (R_1 <= 2400 || R_2 <= 2400) ? 32.0 : 16.0;
		
		double delta_1 = K * (1.0 - E_1);
		double delta_2 = K * E_2;  // negative
		
		union {
			int i;
			char c[4];
		} tmp;
		
		tmp.i = (int)delta_1;
		
		send_to_user(uid_winner, QByteArray("\x45\x01") + QByteArray(tmp.c, 4));
		
		tmp.i = (int)delta_2;
		
		send_to_user(uid_loser, QByteArray("\x45\x00", 2) + QByteArray(tmp.c, 4));
		
		// apply
		R_1 += delta_1;
		R_2 -= delta_2;
		
		// 记得删除游戏，并处理用户状态
		games.remove(id);
		uid_to_gameid.remove(uid_winner);
		uid_to_gameid.remove(uid_loser);
		
		if (uid_to_status.count(uid_winner) > 0) {
			uid_to_status[uid_winner] = USER_STATE_IDLE;
			--cnt_in_game_users;
		}
		
		if (uid_to_status.count(uid_loser) > 0) {
			uid_to_status[uid_loser] = USER_STATE_IDLE;
			--cnt_in_game_users;
		}
		
		{
			int tmp = 1;
			for (auto u : userinfo) {
				if (u.rating > R_1) ++tmp;
			}
			userinfo[uid_winner].rank = tmp;
			
			tmp = 1;
			for (auto u : userinfo) {
				if (u.rating > R_2) ++tmp;
			}
			userinfo[uid_loser].rank = tmp;
		}
		
		
		
		
		// all done
	} else {
		// 发送下一回合的信息
		g.code = RAND_INT();
		send_to_user(uid_B, QByteArray("\x42") + INT_TO_BA(g.code));
	}
}

void GomokuServer::pair_users() {
	QVector<int> uids;
	for (auto it = uid_to_status.begin(); it != uid_to_status.end(); it++) {
		if (it.value() == USER_STATE_FINDING) {
			uids.push_back(it.key());
		}
	}
	if ((int)uids.size() <= 1) {
		return;
	}
	std::random_shuffle(uids.begin(), uids.end());
	int size = (int)uids.size();
	for (int i = 0; i + 1 < size; i += 2) {
		start_game(uids[i], uids[i + 1]);
	}
}

void GomokuServer::start_game(int uid1, int uid2) {
	QUuid id = QUuid::createUuid();
	uid_to_gameid[uid1] = id;
	uid_to_gameid[uid2] = id;
	
	uid_to_status[uid1] = USER_STATE_INGAME;
	uid_to_status[uid2] = USER_STATE_INGAME;
	
	cnt_in_game_users += 2;
	max_in_game_users = qMax(max_in_game_users, cnt_in_game_users);
	
	++cnt_games;
	
	games[id] = GameInfo(uid1, uid2);
	max_running_games = qMax(max_running_games, (int)games.size());
	
	send_to_user(uid1, QByteArray("\x40\x01") + userinfo[uid2].uuid.toRfc4122());
	send_to_user(uid2, QByteArray("\x40\x00", 2) + userinfo[uid1].uuid.toRfc4122());
}

void GomokuServer::tick() {
	this->process_existing_connections();
	this->process_all_games();
	this->get_new_connections();
	
	auto t = CURTIME();
	while (ticks.size() && ticks.begin().key().msecsTo(t) > 1000) {
		ticks.erase(ticks.begin());
	}
	ticks[t] = 1;
	
	if (last_pair_time.msecsTo(CURTIME()) > 5000) {
		last_pair_time = CURTIME();
		this->pair_users();
	}
	
	running_secs = server_start_time.secsTo(CURTIME());
}
