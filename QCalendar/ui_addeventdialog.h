/********************************************************************************
** Form generated from reading UI file 'addeventdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDEVENTDIALOG_H
#define UI_ADDEVENTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_AddEventDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *lbl_event_name;
    QLineEdit *txt_event_name;
    QLabel *lbl_event_datetime;
    QLineEdit *txt_year;
    QLineEdit *txt_month;
    QLineEdit *txt_date;
    QLineEdit *txt_hour;
    QLineEdit *txt_minute;
    QLabel *lbl_event_location;
    QLineEdit *txt_event_location;
    QCheckBox *chk_repeat;
    QSpinBox *spin_repeat;
    QLabel *lbl_repeat_per;
    QComboBox *combo_repeat;

    void setupUi(QDialog *AddEventDialog)
    {
        if (AddEventDialog->objectName().isEmpty())
            AddEventDialog->setObjectName(QStringLiteral("AddEventDialog"));
        AddEventDialog->resize(400, 300);
        AddEventDialog->setSizeGripEnabled(false);
        AddEventDialog->setModal(true);
        buttonBox = new QDialogButtonBox(AddEventDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lbl_event_name = new QLabel(AddEventDialog);
        lbl_event_name->setObjectName(QStringLiteral("lbl_event_name"));
        lbl_event_name->setGeometry(QRect(30, 10, 261, 21));
        txt_event_name = new QLineEdit(AddEventDialog);
        txt_event_name->setObjectName(QStringLiteral("txt_event_name"));
        txt_event_name->setGeometry(QRect(30, 40, 281, 21));
        txt_event_name->setMaxLength(20);
        lbl_event_datetime = new QLabel(AddEventDialog);
        lbl_event_datetime->setObjectName(QStringLiteral("lbl_event_datetime"));
        lbl_event_datetime->setGeometry(QRect(30, 70, 341, 21));
        txt_year = new QLineEdit(AddEventDialog);
        txt_year->setObjectName(QStringLiteral("txt_year"));
        txt_year->setGeometry(QRect(30, 100, 41, 21));
        txt_year->setMaxLength(4);
        txt_month = new QLineEdit(AddEventDialog);
        txt_month->setObjectName(QStringLiteral("txt_month"));
        txt_month->setGeometry(QRect(80, 100, 21, 21));
        txt_month->setMaxLength(2);
        txt_date = new QLineEdit(AddEventDialog);
        txt_date->setObjectName(QStringLiteral("txt_date"));
        txt_date->setGeometry(QRect(110, 100, 21, 21));
        txt_date->setMaxLength(2);
        txt_hour = new QLineEdit(AddEventDialog);
        txt_hour->setObjectName(QStringLiteral("txt_hour"));
        txt_hour->setGeometry(QRect(150, 100, 21, 21));
        txt_hour->setMaxLength(2);
        txt_minute = new QLineEdit(AddEventDialog);
        txt_minute->setObjectName(QStringLiteral("txt_minute"));
        txt_minute->setGeometry(QRect(180, 100, 21, 21));
        txt_minute->setMaxLength(2);
        lbl_event_location = new QLabel(AddEventDialog);
        lbl_event_location->setObjectName(QStringLiteral("lbl_event_location"));
        lbl_event_location->setGeometry(QRect(30, 130, 261, 21));
        txt_event_location = new QLineEdit(AddEventDialog);
        txt_event_location->setObjectName(QStringLiteral("txt_event_location"));
        txt_event_location->setGeometry(QRect(30, 160, 281, 21));
        txt_event_location->setMaxLength(20);
        chk_repeat = new QCheckBox(AddEventDialog);
        chk_repeat->setObjectName(QStringLiteral("chk_repeat"));
        chk_repeat->setGeometry(QRect(30, 200, 81, 21));
        spin_repeat = new QSpinBox(AddEventDialog);
        spin_repeat->setObjectName(QStringLiteral("spin_repeat"));
        spin_repeat->setGeometry(QRect(190, 200, 42, 22));
        spin_repeat->setMinimum(1);
        spin_repeat->setMaximum(999);
        lbl_repeat_per = new QLabel(AddEventDialog);
        lbl_repeat_per->setObjectName(QStringLiteral("lbl_repeat_per"));
        lbl_repeat_per->setGeometry(QRect(120, 200, 61, 21));
        lbl_repeat_per->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        combo_repeat = new QComboBox(AddEventDialog);
        combo_repeat->setObjectName(QStringLiteral("combo_repeat"));
        combo_repeat->setGeometry(QRect(240, 200, 69, 22));

        retranslateUi(AddEventDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), AddEventDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(AddEventDialog);
    } // setupUi

    void retranslateUi(QDialog *AddEventDialog)
    {
        AddEventDialog->setWindowTitle(QString());
        lbl_event_name->setText(QApplication::translate("AddEventDialog", "TextLabel", 0));
        lbl_event_datetime->setText(QApplication::translate("AddEventDialog", "TextLabel", 0));
        txt_hour->setPlaceholderText(QApplication::translate("AddEventDialog", "16", 0));
        txt_minute->setPlaceholderText(QApplication::translate("AddEventDialog", "00", 0));
        lbl_event_location->setText(QApplication::translate("AddEventDialog", "TextLabel", 0));
        chk_repeat->setText(QApplication::translate("AddEventDialog", "CheckBox", 0));
        lbl_repeat_per->setText(QApplication::translate("AddEventDialog", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class AddEventDialog: public Ui_AddEventDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDEVENTDIALOG_H
