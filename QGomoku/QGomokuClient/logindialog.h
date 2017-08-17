#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QString>
#include <QCryptographicHash>

const QString SERVER_NAME = "pi.wys-home.com";

const int SERVER_PORT = 53000;

const char PROTOCOL_VERSION[4] = {16, 9, 4, 8};

inline QByteArray MD5(const QByteArray &s) {
	return QCryptographicHash::hash(s, QCryptographicHash::Md5);
}

// !!!!!!!!!!

inline QByteArray pw_to_hash1(const QByteArray &pw) {
	// take care of it
	return MD5(MD5(pw) + QByteArray("I love Qt, "));
}


namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
	Q_OBJECT

public:
	explicit LoginDialog(QWidget *parent = 0);
	~LoginDialog();

private:
	Ui::LoginDialog *ui;

private slots:
	void login();

};

#endif // LOGINDIALOG_H
