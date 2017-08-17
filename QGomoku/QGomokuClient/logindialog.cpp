#include "logindialog.h"
#include "ui_logindialog.h"

#include <QMessageBox>
#include <QApplication>

#include "clientmainwindow.h"



LoginDialog::LoginDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LoginDialog)
{
	ui->setupUi(this);
	
	connect(ui->button_login, SIGNAL(clicked()), this, SLOT(login()));
}

LoginDialog::~LoginDialog()
{
	delete ui;
}

void LoginDialog::login() {
	
	ui->button_login->setEnabled(false);
	
	QString username = ui->txt_username->text();
	QString password = ui->txt_password->text();
	
	int len_username = username.length();
	if (len_username < 3 || len_username > 20) {
		QMessageBox::warning(this, "QGomoku", "用户名长度必须在 3 ~ 20 之间，且只能由字母、数字、下划线构成");
		ui->button_login->setEnabled(true);
		return;
	}
	
	QByteArray i_login;
	
	i_login += '\x01';
	for (int i = 0; i < 4; i++) {
		i_login += PROTOCOL_VERSION[i];
	}
	
	i_login += '\x10';
	i_login += (char)len_username;
	
	for (int i = 0; i < len_username; i++) {
		if ((username[i] < 'a' || username[i] > 'z') &&
			(username[i] < 'A' || username[i] > 'Z') &&
			(username[i] < '0' || username[i] > '9') &&
			username[i] != '_') {
			QMessageBox::warning(this, "QGomoku", "用户名长度必须在 3 ~ 20 之间，且只能由字母、数字、下划线构成");
			ui->button_login->setEnabled(true);
			return;
		}
		i_login += username[i].toLatin1();
	}
	
	if (password.length() == 0) {
		QMessageBox::warning(this, "QGomoku", "密码不能为空");
		ui->button_login->setEnabled(true);
		return;
	}
	
	QByteArray pw = password.toUtf8();
	
	i_login += pw_to_hash1(pw);
	
	QTcpSocket *socket = new QTcpSocket(NULL);
	
	socket->connectToHost(SERVER_NAME, SERVER_PORT);
	
	if (!socket->waitForConnected(5000)) {
		delete socket;
		QMessageBox::warning(this, "QGomoku", "连接服务器失败");
		ui->button_login->setEnabled(true);
		return;
	}
	
	socket->write(i_login);
	
	char data[20];
	int data_size = 0;
	
	for (int i = 0; i < 100; i++) {
		socket->waitForReadyRead(100);
		if (socket->state() != QTcpSocket::ConnectedState) {
			delete socket;
			QMessageBox::warning(this, "QGomoku", "登录失败");
			ui->button_login->setEnabled(true);
			return;
		}
		data_size += socket->read(data + data_size, 20 - data_size);
		if (data_size == 20) {
			break;
		}
	}
	if (socket->state() != QTcpSocket::ConnectedState || data_size < 20) {
		delete socket;
		QMessageBox::warning(this, "QGomoku", "登录失败");
		ui->button_login->setEnabled(true);
		return;
	}
	
	// 0101 match
	// 1001[uuid] login ok
	
	if (data[0] != 0x01 || data[1] != 0x01 || data[2] != 0x10 || data[3] != 0x01) {
		delete socket;
		QMessageBox::warning(this, "QGomoku", "登录失败");
		ui->button_login->setEnabled(true);
		return;
	}
	
	QByteArray uuid = QByteArray(data + 4, 16);
	// QMessageBox::warning(this, "QGomoku", "登录成功！");
	
	ClientMainWindow *window = new ClientMainWindow(NULL, socket, uuid);
	window->show();
	
	this->deleteLater();
}
