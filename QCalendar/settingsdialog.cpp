#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "language.h"
#define lang Language::get_lang()

#include <QString>
#include <QFileDialog>

static const QString str_transparency[2] = {
	"Transparency", "透明度"
};

static const QString str_settings[2] = {
	"Settings", "设置"
};


SettingsDialog::SettingsDialog(QWidget *parent, Widget *main_window) :
    QDialog(parent),
    ui(new Ui::SettingsDialog), main_window(main_window)
{
    ui->setupUi(this);
    
    this->load_ui_texts();
    
    ui->slider_transparency->setValue(main_window->get_transparency_setting());
    connect(ui->slider_transparency, SIGNAL(valueChanged(int)), this, SLOT(on_slider_transparency_value_changed(int)));
    
    connect(ui->radio_lang_0, SIGNAL(clicked()), this, SLOT(on_lang_0_set()));
    connect(ui->radio_lang_1, SIGNAL(clicked()), this, SLOT(on_lang_1_set()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

static const QString str_save_to_file[2] = {
	"Save configurations to file", "保存配置到文件"
};

static const QString str_load_from_file[2] = {
	"Load configurations from file", "从文件载入配置"
};

void SettingsDialog::load_ui_texts() {
	this->setWindowTitle(str_settings[lang]);
	ui->lbl_transparency->setText(h3(str_transparency[lang]));
	ui->lbl_lang->setText(h3("Language/语言"));
	ui->radio_lang_0->setText("English/英文");
	ui->radio_lang_1->setText("Chinese/中文");
	(lang == 0 ? ui->radio_lang_0 : ui->radio_lang_1)->setChecked(true);
	ui->button_save->setText(str_save_to_file[lang]);
	ui->button_load->setText(str_load_from_file[lang]);
}

void SettingsDialog::on_slider_transparency_value_changed(int val) {
	main_window->change_transparency_setting(val);
}

void SettingsDialog::on_lang_0_set() {
	this->set_lang(0);
}

void SettingsDialog::on_lang_1_set() {
	this->set_lang(1);
}

void SettingsDialog::set_lang(int l) {
	main_window->set_lang(l);
	this->load_ui_texts();
}

void SettingsDialog::on_button_save_clicked() {
	QString filename = QFileDialog::getSaveFileName(this);
	if (filename == "") return;
	main_window->save_to_file(filename);
}

void SettingsDialog::on_button_load_clicked() {
	QString filename = QFileDialog::getOpenFileName(this);
	if (filename == "") return;
	main_window->load_from_file(filename);
	this->close();
}

