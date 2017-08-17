/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QLabel *lbl_transparency;
    QSlider *slider_transparency;
    QLabel *lbl_lang;
    QWidget *widget_lang;
    QRadioButton *radio_lang_0;
    QRadioButton *radio_lang_1;
    QPushButton *button_save;
    QPushButton *button_load;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QStringLiteral("SettingsDialog"));
        SettingsDialog->resize(400, 211);
        SettingsDialog->setModal(true);
        lbl_transparency = new QLabel(SettingsDialog);
        lbl_transparency->setObjectName(QStringLiteral("lbl_transparency"));
        lbl_transparency->setGeometry(QRect(30, 10, 121, 21));
        slider_transparency = new QSlider(SettingsDialog);
        slider_transparency->setObjectName(QStringLiteral("slider_transparency"));
        slider_transparency->setGeometry(QRect(30, 40, 331, 22));
        slider_transparency->setOrientation(Qt::Horizontal);
        slider_transparency->setTickPosition(QSlider::TicksBothSides);
        slider_transparency->setTickInterval(10);
        lbl_lang = new QLabel(SettingsDialog);
        lbl_lang->setObjectName(QStringLiteral("lbl_lang"));
        lbl_lang->setGeometry(QRect(30, 70, 121, 31));
        widget_lang = new QWidget(SettingsDialog);
        widget_lang->setObjectName(QStringLiteral("widget_lang"));
        widget_lang->setGeometry(QRect(130, 70, 231, 31));
        radio_lang_0 = new QRadioButton(widget_lang);
        radio_lang_0->setObjectName(QStringLiteral("radio_lang_0"));
        radio_lang_0->setGeometry(QRect(20, 0, 101, 31));
        radio_lang_1 = new QRadioButton(widget_lang);
        radio_lang_1->setObjectName(QStringLiteral("radio_lang_1"));
        radio_lang_1->setGeometry(QRect(140, 0, 101, 31));
        button_save = new QPushButton(SettingsDialog);
        button_save->setObjectName(QStringLiteral("button_save"));
        button_save->setGeometry(QRect(30, 120, 331, 31));
        button_load = new QPushButton(SettingsDialog);
        button_load->setObjectName(QStringLiteral("button_load"));
        button_load->setGeometry(QRect(30, 160, 331, 31));

        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "Dialog", 0));
        lbl_transparency->setText(QApplication::translate("SettingsDialog", "TextLabel", 0));
        lbl_lang->setText(QApplication::translate("SettingsDialog", "TextLabel", 0));
        radio_lang_0->setText(QApplication::translate("SettingsDialog", "RadioButton", 0));
        radio_lang_1->setText(QApplication::translate("SettingsDialog", "RadioButton", 0));
        button_save->setText(QApplication::translate("SettingsDialog", "PushButton", 0));
        button_load->setText(QApplication::translate("SettingsDialog", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
