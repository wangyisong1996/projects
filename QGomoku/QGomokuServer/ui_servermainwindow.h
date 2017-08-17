/********************************************************************************
** Form generated from reading UI file 'servermainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERMAINWINDOW_H
#define UI_SERVERMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerMainWindow
{
public:
    QWidget *centralWidget;
    QLabel *label1;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ServerMainWindow)
    {
        if (ServerMainWindow->objectName().isEmpty())
            ServerMainWindow->setObjectName(QStringLiteral("ServerMainWindow"));
        ServerMainWindow->resize(402, 347);
        centralWidget = new QWidget(ServerMainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        label1 = new QLabel(centralWidget);
        label1->setObjectName(QStringLiteral("label1"));
        label1->setGeometry(QRect(40, 10, 341, 271));
        ServerMainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ServerMainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 402, 21));
        ServerMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ServerMainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ServerMainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ServerMainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ServerMainWindow->setStatusBar(statusBar);

        retranslateUi(ServerMainWindow);

        QMetaObject::connectSlotsByName(ServerMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ServerMainWindow)
    {
        ServerMainWindow->setWindowTitle(QApplication::translate("ServerMainWindow", "ServerMainWindow", 0));
        label1->setText(QApplication::translate("ServerMainWindow", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class ServerMainWindow: public Ui_ServerMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERMAINWINDOW_H
