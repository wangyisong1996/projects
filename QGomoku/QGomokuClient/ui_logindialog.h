/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QLabel *lbl_username;
    QLabel *lbl_password;
    QLineEdit *txt_username;
    QLineEdit *txt_password;
    QPushButton *button_login;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QStringLiteral("LoginDialog"));
        LoginDialog->resize(409, 134);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(LoginDialog->sizePolicy().hasHeightForWidth());
        LoginDialog->setSizePolicy(sizePolicy);
        lbl_username = new QLabel(LoginDialog);
        lbl_username->setObjectName(QStringLiteral("lbl_username"));
        lbl_username->setGeometry(QRect(20, 20, 71, 31));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(12);
        lbl_username->setFont(font);
        lbl_password = new QLabel(LoginDialog);
        lbl_password->setObjectName(QStringLiteral("lbl_password"));
        lbl_password->setGeometry(QRect(20, 70, 71, 31));
        lbl_password->setFont(font);
        txt_username = new QLineEdit(LoginDialog);
        txt_username->setObjectName(QStringLiteral("txt_username"));
        txt_username->setGeometry(QRect(90, 28, 171, 20));
        txt_username->setMaxLength(20);
        txt_password = new QLineEdit(LoginDialog);
        txt_password->setObjectName(QStringLiteral("txt_password"));
        txt_password->setGeometry(QRect(90, 78, 171, 20));
        txt_password->setMaxLength(32767);
        txt_password->setEchoMode(QLineEdit::Password);
        button_login = new QPushButton(LoginDialog);
        button_login->setObjectName(QStringLiteral("button_login"));
        button_login->setGeometry(QRect(280, 28, 101, 71));
        button_login->setFont(font);

        retranslateUi(LoginDialog);

        button_login->setDefault(true);


        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "QGomoku - Login", 0));
        lbl_username->setText(QApplication::translate("LoginDialog", "\347\224\250\346\210\267\345\220\215:", 0));
        lbl_password->setText(QApplication::translate("LoginDialog", "\345\257\206\347\240\201:", 0));
        txt_username->setText(QString());
        txt_password->setInputMask(QString());
        txt_password->setText(QString());
        txt_password->setPlaceholderText(QString());
        button_login->setText(QApplication::translate("LoginDialog", "\347\231\273\345\275\225", 0));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
