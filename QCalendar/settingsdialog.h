#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "widget.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent, Widget *main_window);
    ~SettingsDialog();

private slots:
    void on_slider_transparency_value_changed(int val);
    
    void on_lang_0_set();
    void on_lang_1_set();
    
    void on_button_save_clicked();
    void on_button_load_clicked();

private:
    Ui::SettingsDialog *ui;
    
    Widget *main_window;
    
    void load_ui_texts();
    
    void set_lang(int l);
};

#endif // SETTINGSDIALOG_H
