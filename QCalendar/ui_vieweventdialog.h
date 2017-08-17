/********************************************************************************
** Form generated from reading UI file 'vieweventdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWEVENTDIALOG_H
#define UI_VIEWEVENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_ViewEventDialog
{
public:

    void setupUi(QDialog *ViewEventDialog)
    {
        if (ViewEventDialog->objectName().isEmpty())
            ViewEventDialog->setObjectName(QStringLiteral("ViewEventDialog"));
        ViewEventDialog->resize(400, 300);
        ViewEventDialog->setModal(true);

        retranslateUi(ViewEventDialog);

        QMetaObject::connectSlotsByName(ViewEventDialog);
    } // setupUi

    void retranslateUi(QDialog *ViewEventDialog)
    {
        ViewEventDialog->setWindowTitle(QApplication::translate("ViewEventDialog", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class ViewEventDialog: public Ui_ViewEventDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWEVENTDIALOG_H
