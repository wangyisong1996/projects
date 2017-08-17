#include "clientmainwindow.h"
#include "ui_clientmainwindow.h"

#include "logindialog.h"

#include <QMessageBox>

#define HV_CENTER (Qt::AlignHCenter | Qt::AlignVCenter)

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

QString UserInfo::to_string() {
	if (username == "") return "加载中...";
	QString tmp;
	tmp.sprintf("%.2lf%%", 100.0 * win / (all <= 0 ? 1 : all));
	return "用户名: " + username + "\n" +
		"胜场数: " + NUM(win) + "\n" +
		"总场数: " + NUM(all) + "\n" +
		"胜率: " + tmp + "\n" +
		"积分: " + NUM(rating) + "\n" +
		"排名: " + NUM(rank);
}

ClientMainWindow::ClientMainWindow(QWidget *parent, QTcpSocket *socket, QByteArray uuid) :
	QMainWindow(parent),
	ui(new Ui::ClientMainWindow)
{
	ui->setupUi(this);
	
	this->socket = socket;
	this->user_uuid = uuid;
	
	font_db.addApplicationFont("MicrosoftYaHei.ttf");
	
	query_user_info(&my_user_info, user_uuid);
	
	in_transition = true;
	state_trans_time = CURTIME().addMSecs(-499);
	next_state = STATE_IN_LOBBY;
	
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
	timer->start(10);
}

ClientMainWindow::~ClientMainWindow()
{
	delete ui;
}

void ClientMainWindow::paintEvent(QPaintEvent *) {
	this->draw();
}

void ClientMainWindow::keyPressEvent(QKeyEvent *e) {
	if (e->key() == Qt::Key_R) {
		++R_press_cnt;
	} else if (e->key() == Qt::Key_Up) {
		++up_press_cnt;
	} else if (e->key() == Qt::Key_Down) {
		++down_press_cnt;
	} else if (e->key() == Qt::Key_Left) {
		++left_press_cnt;
	} else if (e->key() == Qt::Key_Right) {
		++right_press_cnt;
	} else if (e->key() == Qt::Key_Space) {
		++space_press_cnt;
	}
}

void ClientMainWindow::mousePressEvent(QMouseEvent *e) {
	
}

void ClientMainWindow::disconnect() {
	delete socket;
	timer->deleteLater();
	this->deleteLater();
	QMessageBox::warning(NULL, "QGomoku", "你已经掉线，请重新登录");
	LoginDialog *dialog = new LoginDialog();
	dialog->show();
}

void ClientMainWindow::send(QByteArray s) {
	socket->write(s);
}

void ClientMainWindow::try_to_read_data() {
	if (data_size < MAX_DATA_SIZE) {
		int ret = socket->read(data + data_size, MAX_DATA_SIZE - data_size);
		if (ret > 0) {
			data_size += ret;
		}
	}
}

int ClientMainWindow::has_data() {
	if (data_size <= 0) {
		return 0;
	}
	char d_code = data[0];
	switch (d_code) {
		case 0x0f:
			data_len = 1;
			break;
		case 0x11:
			if (data_size <= 1) return 0;
			if (data[1] == 0x00) {
				data_len = 2;
			} else if (data[1] == 0x01) {
				if (data_size <= 3) return 0;
				int l = data[2];
				if (l < 0 || l > 20) {
					return -1;
				}
				data_len = 1 + 1 + 1 + l + 16;
			} else {
				return -1;
			}
			break;
		case 0x20:
			data_len = 1;
			break;
		case 0x2f:
			data_len = 1;
			break;
		case 0x40:
			data_len = 1 + 1 + 16;
			break;
		case 0x41:
			data_len = 1;
			break;
		case 0x42:
			data_len = 1 + 4;
			break;
		case 0x43:
			data_len = 1 + 2;
			break;
		case 0x44:
			data_len = 1 + 2;
			break;
		case 0x45:
			data_len = 1 + 1 + 4;
			break;
		case 0x4f:
			data_len = 1 + 1 + 4 + 16 + 57;
			if (data_size >= data_len) {
				// QMessageBox::warning(NULL, "hello data[1]:", NUM(data[1]));
				if (data[1] != 0x00 && data[1] != 0x01) {
					return -1;
				}
				int cnt_b = 0, cnt_w = 0;
				for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
					// 额外的bug：char是有符号的
					// 额外的额外的bug：有符号转无符号不能直接+128
					// 【论 NOIP, 离散, 计原 没学好的坏处】
					int tmp = ((unsigned char*)data)[1 + 1 + 4 + 16 + i / 4];
					tmp >>= (i % 4) * 2;
					tmp &= 3;
					if (tmp == 2) {
						++cnt_b;
					} else if (tmp == 3) {
						++cnt_w;
					} else if (tmp != 0) {
						return -1;
					}
				}
				bool is_black_turn;
				if (cnt_b == cnt_w) {
					is_black_turn = true;
				} else if (cnt_b - cnt_w == 1) {
					is_black_turn = false;
				} else {
					return -1;
				}
				// QMessageBox::warning(NULL, "!!!", NUM((int)is_black_turn) + NUM(data[1]));
				bool is_my_turn = is_black_turn ^ (data[1] == 0x01) ^ 1;
				if (is_my_turn) {
					data_len += 4 + 4;
				}
			} else {
				return 0;
			}
			break;
		default:
			// QMessageBox::warning(NULL, "", NUM((int)d_code));
			return -1;
	}
	// if (data_size >= data_len) {
	// 	QMessageBox::warning(NULL, "get", NUM((int)d_code));
	// }
	return data_size >= data_len;
}

void ClientMainWindow::remove_data() {
	if (data_len < 0 || data_len > MAX_DATA_SIZE) return;
	if (data_len > data_size) return;
	for (int i = data_len; i < data_size; i++) {
		data[i - data_len] = data[i];
	}
	data_size -= data_len;
}

void ClientMainWindow::process_data() {
	char d_code = data[0];
	switch (d_code) {
		case 0x0f: {
			// heartbeat
			remove_data();
			send("\x0f");
			break;
		}
		case 0x11: {
			// user info
			
			if (user_info_queries.size() == 0) {
				disconnect();
				return;
			}
			
			UserInfo *u = user_info_queries.front();
			user_info_queries.pop_front();
			
			if (data[1] == 0x01) {
				int l = data[2];
				if (l < 0 || l > 20) {
					disconnect();
				}
				u->username = QString(QByteArray(data + 3, l));
				
				union {
					char c[16];
					int i[4];
				} tmp;
				for (int i = 0; i < 16; i++) {
					tmp.c[i] = data[3 + l + i];
				}
				u->win = tmp.i[0];
				u->all = tmp.i[1];
				u->rating = tmp.i[2];
				u->rank = tmp.i[3];
				
				if (u->win < 0 || u->all < 0 || u->win > u->all) {
					disconnect();
					return;
				}
			}
			
			remove_data();
			break;
		}
		case 0x20: {
			// random game request received
			remove_data();
			
			if (state == STATE_IN_LOBBY) {
				show_notification("正在寻找对手");
				is_finding = true;
			}
			
			break;
		}
		case 0x2f: {
			// random game request cancelled
			remove_data();
			
			if (state == STATE_IN_LOBBY) {
				show_notification("已取消寻找对手");
				is_finding = false;
			}
			
			break;
		}
		case 0x40: {
			// start game
			// [is_b][uuid]
			if (data[1] == 0x00) {
				my_player = 3;
			} else if (data[1] == 0x01) {
				my_player = 2;
			} else {
				disconnect();
				return;
			}
			
			QByteArray uuid(data + 2, 16);
			oppo_user_info.clear();
			
			query_user_info(&oppo_user_info, uuid);
			
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					board[i][j] = 0;
				}
			}
			
			my_x = my_y = oppo_x = oppo_y = 7;
			
			game_started = false;
			game_over = false;
			
			trans_state(STATE_IN_GAME);
			show_notification("对战开始", 1000);
			cd_player = 0;
			
			remove_data();
			
			break;
		}
		case 0x41: {
			// start game signal
			remove_data();
			
			is_finding = false;
			game_started = true;
			start_next_round();
			
			break;
		}
		case 0x42: {
			// [code]
			
			
			game_code = QByteArray(data + 1, 4);
			
			start_my_round();
			
			remove_data();
			break;
		}
		case 0x43: {
			// my operation
			// [x][y]
			
			int x = data[1], y = data[2];
			if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
				disconnect();
				return;
			}
			
			game_move(my_player, x, y);
			
			start_next_round();
			
			remove_data();
			break;
		}
		case 0x44: {
			// oppo's operation
			// [x][y]
			
			int x = data[1], y = data[2];
			if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
				disconnect();
				return;
			}
			
			game_move(my_player ^ 1, x, y);
			
			start_next_round();
			
			remove_data();
			break;
		}
		case 0x45: {
			// game ends
			if (!in_transition && state != STATE_IN_GAME) {
				disconnect();
				return;
			}
			bool win;
			if (data[1] == 0x00) {
				win = false;
			} else if (data[1] == 0x01) {
				win = true;
			} else {
				disconnect();
				return;
			}
			
			union {
				char c[4];
				int i;
			} d_rating;
			for (int i = 0; i < 4; i++) {
				d_rating.c[i] = data[2 + i];
			}
			
			QString s = "对战结束，";
			if (win) {
				s += "你赢了，积分增加" + NUM(d_rating.i);
			} else {
				s += "你输了，积分减少" + NUM(d_rating.i);
			}
			
			show_notification(s, 1000);
			trans_state(STATE_IN_LOBBY, 4000);
			
			query_user_info(&my_user_info, user_uuid);
			
			remove_data();
			break;
		}
		case 0x4f: {
			// restore game
			// ...
			
			is_finding = false;
			trans_state(STATE_IN_GAME);
			
			if (data[1] == 0x00) {
				my_player = 3;
			} else {
				my_player = 2;
			}
			
			oppo_user_info.clear();
			query_user_info(&oppo_user_info, QByteArray(data + 2, 16));
			
			my_x = data[18];
			my_y = data[19];
			oppo_x = data[20];
			oppo_y = data[21];
			
			if (my_x < 0 || my_x >= BOARD_SIZE || my_y < 0 || my_y >= BOARD_SIZE) {
				disconnect();
				return;
			}
			if (oppo_x < 0 || oppo_x >= BOARD_SIZE || oppo_y < 0 || oppo_y >= BOARD_SIZE) {
				disconnect();
				return;
			}
			
			game_started = true;
			game_over = false;
			
			int cnt_b = 0, cnt_w = 0;
			for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++) {
				int tmp = ((unsigned char*)data)[1 + 1 + 4 + 16 + i / 4];
				tmp >>= (i % 4) * 2;
				tmp &= 3;
				if (tmp == 2) {
					++cnt_b;
				} else if (tmp == 3) {
					++cnt_w;
				} else if (tmp != 0) {
					disconnect();
					return;
				}
				board[i / BOARD_SIZE][i % BOARD_SIZE] = tmp;
			}
			bool is_black_turn;
			if (cnt_b == cnt_w) {
				is_black_turn = true;
			} else if (cnt_b - cnt_w == 1) {
				is_black_turn = false;
			} else {
				disconnect();
				return;
			}
			// QMessageBox::warning(NULL, "!!!", NUM((int)is_black_turn) + NUM(data[1]));
			bool is_my_turn = is_black_turn ^ (data[1] == 0x01) ^ 1;
			
			start_next_round();
			cd_player = my_player ^ 1;
			cd_start_time = CURTIME();
			
			if (is_my_turn) {
				game_code = QByteArray(data + 1 + 1 + 4 + 16 + 57, 4);
				union {
					char c[4];
					int i;
				} cd_t;
				for (int i = 0; i < 4; i++) {
					cd_t.c[i] = data[1 + 1 + 4 + 16 + 57 + 4 + i];
				}
				start_my_round();
				cd_start_time = CURTIME().addMSecs(-20000 + cd_t.i);
			}
			
			remove_data();
			break;
		}
		default: {
			// 不清真
			disconnect();
		}
	}
}

void ClientMainWindow::process_input() {
	if (in_transition) {
		// disable & delete all inputs
		R_press_cnt = 0;
		
		up_press_cnt = 0;
		down_press_cnt = 0;
		left_press_cnt = 0;
		right_press_cnt = 0;
		space_press_cnt = 0;
		return;
	}
	
	if (R_press_cnt) {
		R_press_cnt = 0;
		
		if (state == STATE_IN_LOBBY) {
			if (!is_finding) {
				// random game req
				send("\x20");
			} else {
				// cancel random game
				send("\x2f");
			}
		}
	}
	while (up_press_cnt--) {
		if (state == STATE_IN_GAME && is_my_turn && !space_pressed) {
			my_x = qMax(my_x - 1, 0);
		}
	}
	up_press_cnt = 0;
	
	while (down_press_cnt--) {
		if (state == STATE_IN_GAME && is_my_turn && !space_pressed) {
			my_x = qMin(my_x + 1, BOARD_SIZE - 1);
		}
	}
	down_press_cnt = 0;
	
	while (left_press_cnt--) {
		if (state == STATE_IN_GAME && is_my_turn && !space_pressed) {
			my_y = qMax(my_y - 1, 0);
		}
	}
	left_press_cnt = 0;
	
	while (right_press_cnt--) {
		if (state == STATE_IN_GAME && is_my_turn && !space_pressed) {
			my_y = qMin(my_y + 1, BOARD_SIZE - 1);
		}
	}
	right_press_cnt = 0;
	
	if (space_press_cnt) {
		space_press_cnt = 0;
		if (state == STATE_IN_GAME && is_my_turn) {
			if (!space_pressed && board[my_x][my_y] == 0) {
				space_pressed = true;
				cd_player = 0;
				send(("\x42" + game_code + (char)my_x) + (char)my_y);
			}
		}
	}
	
	if (state == STATE_IN_GAME && is_my_turn && !space_pressed) {
		if (cd_start_time.msecsTo(CURTIME()) > 20000) {
			space_pressed = true;
			random_move();
		}
	}
}

void ClientMainWindow::query_user_info(UserInfo *u, QByteArray uuid) {
	user_info_queries.append(u);
	send("\x11" + uuid);
}

void ClientMainWindow::trans_state(int s, int delay) {
	in_transition = true;
	next_state = s;
	state_trans_time = CURTIME().addMSecs(delay);
}

void ClientMainWindow::start_next_round() {
	is_my_turn = false;
	
	int d = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == 2) {
				++d;
			} else if (board[i][j] == 3) {
				--d;
			}
		}
	}
	if (d + 2 != my_player) {
		cd_player = d + 2;
		cd_start_time = CURTIME();
	}
}

void ClientMainWindow::start_my_round() {
	is_my_turn = true;
	space_pressed = false;
	show_notification("轮到你下棋了");
	
	cd_player = my_player;
	cd_start_time = CURTIME();
}

void ClientMainWindow::game_move(int player, int x, int y) {
	if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
		disconnect();
		return;
	}
	if (player != 2 && player != 3) {
		disconnect();
		return;
	}
	if (board[x][y] != 0) {
		disconnect();
		return;
	}
	board[x][y] = player;
	if (player == my_player) {
		my_x = x, my_y = y;
	} else {
		oppo_x = x, oppo_y = y;
	}
	
	check_game_over();
}

void ClientMainWindow::random_move() {
	int cnt = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == 0) {
				++cnt;
			}
		}
	}
	if (cnt == 0) {
		disconnect();
		return;
	}
	int X = RAND_INT() % cnt;
	if (X < 0) X += cnt;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (X == 0) {
				my_x = i, my_y = j;
				cd_player = 0;
				send(("\x42" + game_code + (char)i) + (char)j);
				return;
			} else {
				--X;
			}
		}
	}
}

void ClientMainWindow::check_game_over() {
	if (game_over) return;
	
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
					game_over = true;
					over_x1 = i, over_y1 = j;
					over_x2 = i + dx[id] * 4;
					over_y2 = j + dy[id] * 4;
					break;
				}
			}
			if (game_over) break;
		}
		if (game_over) break;
	}
}


void ClientMainWindow::draw() {
	QPainter p(this);
	// QPainter只能创建一个！！！
	
	static QImage img_background(":/images/background.png");
	
	p.drawImage(0, 0, img_background);
	
	QDateTime cur = CURTIME();
	
	int state_to_draw = state;
	double opacity = 0.0;
	
	if (in_transition) {
		int t = state_trans_time.msecsTo(CURTIME());
		if (t > 500) {
			state_to_draw = next_state;
			opacity = t > 1000 ? 0.0 : (1000 - t) / 500.0;
		} else if (t > 0) {
			opacity = t / 500.0;
		}
	}
	
	this->draw_state(p, state_to_draw);
	
	if (in_transition) {
		p.setOpacity(opacity);
		p.setBrush(QColor("black"));
		p.drawRect(0, 0, 1000, 600);
		p.setOpacity(1.0);
	}
	
	if (has_notification) {
		this->draw_notification(p);
	}
}

static QImage get_black_stone() {
	static bool get = 0;
	static QImageReader reader(":/images/black-stone.png");
	static QImage ret;
	
	if (!get) {
		reader.setScaledSize(QSize(62.5, 62.5));
		ret = reader.read();
		get = true;
	}
	
	return ret;
}

static QImage get_white_stone() {
	static bool get = 0;
	static QImageReader reader(":/images/white-stone.png");
	static QImage ret;
	
	if (!get) {
		reader.setScaledSize(QSize(62.5, 62.5));
		ret = reader.read();
		get = true;
	}
	
	return ret;
}

static QImage get_bomb_image() {
	static bool get = 0;
	static QImageReader reader(":/images/bomb.png");
	static QImage ret;
	
	if (!get) {
		QSize reader_size = reader.size();
		double rate = 33 * 0.8 / qMax(reader_size.width(), reader_size.height());
		reader.setScaledSize(reader_size * rate);
		ret = reader.read();
		get = true;
	}
	
	return ret;
}

void ClientMainWindow::draw_state(QPainter &p, int state) {
	if (state == STATE_IN_LOBBY) {
		p.setBrush(QColor(200, 243, 255, 128));
		p.drawRect(350, 100, 300, 200);
		p.setFont(font_db.font("微软雅黑", "normal", 16));
		p.drawText(350, 100, 300, 200, HV_CENTER, my_user_info.to_string());
		
		
		QString text;
		if (!is_finding) {
			text = "随机对战 [R]";
		} else {
			text = "正在寻找对手";
			static int cnt = 0;
			static QDateTime last_t = CURTIME();
			if (last_t.msecsTo(CURTIME()) > 500) {
				cnt = (cnt + 1) % 3;
				last_t = CURTIME();
			}
			for (int i = 0; i <= cnt; i++) {
				text += ".";
			}
			text += "\n";
			text += "取消 [R]";
		}
		
		p.setBrush(QColor(255, 255, 255, 128));
		p.drawRect(400, 400, 200, 100);
		p.setFont(font_db.font("微软雅黑", "normal", 14));
		p.drawText(400, 400, 200, 100, HV_CENTER, text);
	} else if (state == STATE_IN_GAME) {
		static QImage img_board(":/images/board.png");  // 580^2
		
		p.drawImage(210, 10, img_board);
		
		draw_board(p, 241, 41, 518);  // 37 * 14
		
		
		p.setPen(Qt::black);
		
		p.setBrush(QColor(200, 243, 255, 128));
		p.drawRect(10, 410, 180, 180);
		p.setFont(font_db.font("微软雅黑", "normal", 13));
		p.drawText(10, 410, 180, 180, HV_CENTER, my_user_info.to_string());
		
		
		p.setBrush(QColor(200, 243, 255, 128));
		p.drawRect(810, 10, 180, 180);
		p.setFont(font_db.font("微软雅黑", "normal", 13));
		p.drawText(810, 10, 180, 180, HV_CENTER, oppo_user_info.to_string());
		
		
		if (cd_player == 2 || cd_player == 3) {
			draw_player_cd(p);
		}
	}
}


static void draw_frame_fixed(QPainter &p, double x, double y, double size, QColor color) {
	size *= 0.5;
	p.setPen(color);
	p.drawLine(x - size, y - size, x + size, y - size);
	p.drawLine(x + size, y - size, x + size, y + size);
	p.drawLine(x + size, y + size, x - size, y + size);
	p.drawLine(x - size, y + size, x - size, y - size);
}

static void draw_frame(QPainter &p, double x, double y, double size, QColor color) {
	size *= 0.25;
	p.setPen(color);
	
	p.drawLine(x + size * 2, y + size, x + size * 2, y + size * 2);
	p.drawLine(x + size * 2, y + size * 2, x + size, y + size * 2);

	p.drawLine(x - size * 2, y + size, x - size * 2, y + size * 2);
	p.drawLine(x - size * 2, y + size * 2, x - size, y + size * 2);

	p.drawLine(x + size * 2, y - size, x + size * 2, y - size * 2);
	p.drawLine(x + size * 2, y - size * 2, x + size, y - size * 2);

	p.drawLine(x - size * 2, y - size, x - size * 2, y - size * 2);
	p.drawLine(x - size * 2, y - size * 2, x - size, y - size * 2);
}

bool ClientMainWindow::check_dangerous_grid(int x, int y) {
	if (!game_started) return false;
	if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return false;
	if (board[x][y]) return false;
	
	static int board[BOARD_SIZE][BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = this->board[i][j];
		}
	}
	board[x][y] = my_player ^ 1;
	
	static const int dx[4] = {1, 1, 1, 0};
	static const int dy[4] = {-1, 0, 1, 1};
	
	int cnt_3 = 0, cnt_4 = 0;
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			for (int id = 0; id < 4; id++) {
				int tx = i + dx[id] * 4, ty = j + dy[id] * 4;
				if (tx < 0 || tx >= BOARD_SIZE || ty < 0 || ty >= BOARD_SIZE) {
					continue;
				}
				int cnt = 0;
				for (int k = 0; k < 5; k++) {
					int tmp = board[tx][ty];
					if (tmp == (my_player ^ 1)) {
						++cnt;
					}
					tx -= dx[id], ty -= dy[id];
				}
				tx = i + dx[id] * 4, ty = j + dy[id] * 4;
				if (cnt == 5) {
					return true;
				} else if (cnt == 3 && board[i][j] == 0 && board[tx][ty] == 0) {
					++cnt_3;
				} else if (cnt == 4 && (board[i][j] == 0 || board[tx][ty] == 0)) {
					++cnt_4;
				}
			}
		}
	}
	
	if (cnt_3 + cnt_4 > 1) {
		return true;
	}
	
	return false;
}

void ClientMainWindow::draw_board(QPainter &p, int x0, int y0, int size) {
	double len = size / (-1.0 + BOARD_SIZE);
	x0 -= size / (-1 + BOARD_SIZE);
	y0 -= size / (-1 + BOARD_SIZE);
	for (int i = 1; i <= BOARD_SIZE; i++) {
		int xpos = (int)(x0 + len * i + 0.5);
		int ypos = (int)(y0 + len * i + 0.5);
		p.drawLine(x0 + len, ypos, x0 + len + size, ypos);
		p.drawLine(xpos, y0 + len, xpos, y0 + len + size);
	}
	
	// QImageReader reader(":/images/bomb.png");
	// QSize reader_size = reader.size();
	// double rate = len * 0.8 / qMax(reader_size.width(), reader_size.height());
	// double image_width = reader_size.width() * rate;
	// double image_height = reader_size.height() * rate;
	// reader.setScaledSize(reader_size * rate);
	// QImage bomb_image = reader.read();
	
	// bool show_dangerous_grid = this->show_dangerous_grid;
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			double xpos = x0 + len * (j + 1);
			double ypos = y0 + len * (i + 1);
			
			int val = board[i][j];
			if (val != 2 && val != 3) {
				if (check_dangerous_grid(i, j)) {
					QImage img = get_bomb_image();
					p.drawImage(xpos - img.width() * 0.5, ypos - img.height() * 0.5, img);
				}
				continue;
			}
			
			if (val == 2) {
				p.setBrush(QColor("black"));
			} else {
				p.setBrush(QColor("white"));
			}
			p.setPen(QColor("black"));
			
			double r = 0.9 * len * 0.5;
			p.drawEllipse(QPointF(xpos, ypos), r, r);
		}
	}
	
	
	
	// if (state == STATE_BLACK_TURN || state == STATE_WHITE_TURN) {
	if (game_started) {
		
		int x1, y1, x2, y2;
		// x1: my, x2: oppo
		
		if (is_my_turn) {
			x1 = my_x, y1 = my_y;
			x2 = oppo_x, y2 = oppo_y;
		} else {
			x2 = my_x, y2 = my_y;
			x1 = oppo_x, y1 = oppo_y;
		}
		
		double dt = QTime(0, 0, 0).msecsTo(QTime::currentTime()) * 0.001;
		
		if (!is_my_turn || space_pressed) dt = 0;
		
		draw_frame(p, x0 + len * (y1 + 1), y0 + len * (x1 + 1),
			len * (0.9 + 0.1 * qSin(dt * 2 * PI / 0.5)), QColor("#191919"));
		
		draw_frame_fixed(p, x0 + len * (y2 + 1), y0 + len * (x2 + 1),
			len * 0.9, QColor("#555555"));
		
	}
	
	if (game_over) {
		double x1 = x0 + len * (over_y1 + 1);
		double y1 = y0 + len * (over_x1 + 1);
		double x2 = x0 + len * (over_y2 + 1);
		double y2 = y0 + len * (over_x2 + 1);
		
		p.setPen(QColor("green"));
		p.drawLine(x1, y1, x2, y2);
	}
	
	
}

void ClientMainWindow::draw_player_cd(QPainter &p) {
	if (game_over) return;
	
	int pos_x, pos_y;
	if (cd_player == my_player) {
		pos_x = 0, pos_y = 300;
	} else {
		pos_x = 800, pos_y = 200;
	}
	
	p.setPen(Qt::NoPen);
	p.setBrush(QColor(255, 255, 255, 63));
	p.drawRect(pos_x + 10, pos_y + 10, 180, 80);
	
	p.drawImage(pos_x + 19, pos_y + 19, cd_player == 2 ? get_black_stone() : get_white_stone());
	
	p.setFont(font_db.font("微软雅黑", "bold", 20));
	
	int t = 20000 - cd_start_time.msecsTo(CURTIME());
	if (t < 0) t = 0;
	t = (t + 500) / 1000;
	QString s;
	s.sprintf("%02d", t);
	// 忘记setpen ......
	p.setPen(Qt::black);
	p.drawText(pos_x + 100, pos_y + 10, 80, 80, HV_CENTER, s);
}

void ClientMainWindow::show_notification(QString s, int delay) {
	notification_time = CURTIME().addMSecs(delay);
	notification_str = s;
	has_notification = true;
}

void ClientMainWindow::draw_notification(QPainter &p) {
	if (!has_notification) return;
	
	int t = notification_time.msecsTo(CURTIME());
	
	if (t < 0) {
		return;
	}
	
	int pos = 260;
	double opa = 1.0;
	
	if (t < 100) {
		pos += (100 - t) * 0.5;
		opa = t / 100.0;
	} else if (t > 1100 && t <= 1400) {
		opa = 1.0 - (t - 1100) / 300.0;
	} else if (t > 1400) {
		has_notification = false;
		return;
	}
	
	p.setOpacity(opa);
	p.setBrush(QColor(0, 0, 0, 83));
	p.setPen(Qt::NoPen);
	p.drawRect(0, pos, 1000, 60);
	
	p.setBrush(QColor(255, 255, 150));
	p.setPen(Qt::black);
	
	QFont font = font_db.font("微软雅黑", "bold", 21);
	QPainterPath path;
	path.addText(0, 0, font, notification_str);
	QRectF rect = path.boundingRect();
	path.translate(QPointF(500, pos + 30) - rect.center());
	p.drawPath(path);
	// p.drawText(0, pos, 1000, 60, HV_CENTER, notification_str);
	
	
	p.setOpacity(1.0);
}

void ClientMainWindow::tick() {
	if (socket->state() != QTcpSocket::ConnectedState) {
		disconnect();
		return;
	}
	
	this->try_to_read_data();
	int has_d = this->has_data();
	
	if (has_d == -1) {
		disconnect();
		return;
	}
	
	if (has_d == 1) {
		this->process_data();
	}
	
	this->process_input();
	
	if (in_transition && state_trans_time.msecsTo(CURTIME()) > 1000) {
		in_transition = false;
		state = next_state;
	}
	
	this->repaint();
}


