/********************************************************************************
** Form generated from reading UI file 'clientmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTMAINWINDOW_H
#define UI_CLIENTMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientMainWindow
{
public:
    QWidget *centralWidget;

    void setupUi(QMainWindow *ClientMainWindow)
    {
        if (ClientMainWindow->objectName().isEmpty())
            ClientMainWindow->setObjectName(QStringLiteral("ClientMainWindow"));
        ClientMainWindow->resize(1000, 600);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ClientMainWindow->sizePolicy().hasHeightForWidth());
        ClientMainWindow->setSizePolicy(sizePolicy);
        ClientMainWindow->setMinimumSize(QSize(1000, 600));
        ClientMainWindow->setMaximumSize(QSize(1000, 600));
        centralWidget = new QWidget(ClientMainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ClientMainWindow->setCentralWidget(centralWidget);

        retranslateUi(ClientMainWindow);

        QMetaObject::connectSlotsByName(ClientMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ClientMainWindow)
    {
        ClientMainWindow->setWindowTitle(QApplication::translate("ClientMainWindow", "ClientMainWindow", 0));
    } // retranslateUi

};

namespace Ui {
    class ClientMainWindow: public Ui_ClientMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTMAINWINDOW_H
