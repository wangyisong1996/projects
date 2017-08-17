#include "addeventdialog.h"
#include "ui_addeventdialog.h"

#include "language.h"

#define lang Language::get_lang()

#include "utils.h"

#include <QString>
#include <QMessageBox>

#define NUM(x) QString::number(x, 10)

static const QString str_add_event_caption[2] = {
	"Add event", "添加事件"
};

static const QString str_event_name[2] = {
	"Name", "名称"
};

static const QString str_event_datetime[2] = {
	"Date &amp; Time (year - month - date  hour : minute)",
	"日期 &amp; 时间 (年 - 月 - 日  小时 : 分钟)"
};

static const QString str_event_location[2] = {
	"Location", "地点"
};

static const QString str_repeat[2] = {
	"Repeat", "重复"
};

static const QString str_repeat_per[2] = {
	"per", "每"
};

static const QString str_repeat_options[2][4] = {
	{"day(s)", "week(s)", "month(s)", "year(s)"},
	{"天", "周", "月", "年"}
};

AddEventDialog::AddEventDialog(QWidget *parent, EventManager *eventmanager, int Y, int M, int D) :
    QDialog(parent),
    ui(new Ui::AddEventDialog),
    eventmanager(eventmanager), Y(Y), M(M), D(D)
{
    ui->setupUi(this);
    
    this->setWindowTitle(str_add_event_caption[lang]);
    
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(on_accept_clicked()));
    
    ui->lbl_event_name->setText("<h3>" + str_event_name[lang] + "</h3>");
    ui->lbl_event_datetime->setText("<h3>" + str_event_datetime[lang] + "</h3>");
    ui->lbl_event_location->setText("<h3>" + str_event_location[lang] + "</h3>");
    
    ui->txt_year->setText(QString::number(Y, 10));
    ui->txt_month->setText(QString::number(M, 10));
    ui->txt_date->setText(QString::number(D, 10));
    
    ui->chk_repeat->setText(str_repeat[lang]);
    ui->lbl_repeat_per->setText(str_repeat_per[lang]);
    for (int i = 0; i < 4; i++) {
    	ui->combo_repeat->insertItem(i, str_repeat_options[lang][i]);
    }
    this->on_chk_repeat_toggled(false);
}

static const QString str_edit_event_caption[2] = {
	"Edit event", "编辑事件"
};

AddEventDialog::AddEventDialog(QWidget *parent, EventManager *eventmanager, long long ID) :
    QDialog(parent),
    ui(new Ui::AddEventDialog),
    eventmanager(eventmanager)
 {
	ui->setupUi(this);
	
	this->ID = ID;
	
	this->setWindowTitle(str_edit_event_caption[lang]);
	
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(on_edit_clicked()));
	
	ui->lbl_event_name->setText("<h3>" + str_event_name[lang] + "</h3>");
    ui->lbl_event_datetime->setText("<h3>" + str_event_datetime[lang] + "</h3>");
    ui->lbl_event_location->setText("<h3>" + str_event_location[lang] + "</h3>");
    
    MyEvent e = eventmanager->get_event_by_ID(ID);
    ui->txt_event_name->setText(e.name);
    ui->txt_event_location->setText(e.location);
    int yy, mm, dd, h, m, s;
    ll_to_datetime(e.datetime, yy, mm, dd, h, m, s);
    ui->txt_year->setText(NUM(yy));
    ui->txt_month->setText(NUM(mm));
    ui->txt_date->setText(NUM(dd));
    ui->txt_hour->setText(NUM(h));
    ui->txt_minute->setText(NUM(m));
    
    ui->chk_repeat->setText(str_repeat[lang]);
    ui->lbl_repeat_per->setText(str_repeat_per[lang]);
    for (int i = 0; i < 4; i++) {
    	ui->combo_repeat->insertItem(i, str_repeat_options[lang][i]);
    }
    
    ui->chk_repeat->setChecked(e.repeat_type > 0);
    if (e.repeat_type > 0) {
    	ui->spin_repeat->setValue(e.repeat_interval);
    	ui->combo_repeat->setCurrentIndex(e.repeat_type - 1);
    }
    
    this->on_chk_repeat_toggled(ui->chk_repeat->isChecked());
}

AddEventDialog::~AddEventDialog()
{
    delete ui;
}

void AddEventDialog::on_AddEventDialog_accepted()
{
    // QMessageBox::warning(NULL, "warning", "heiheihei", QMessageBox::Yes);
}

static void gen_warning(QString text) {
	QMessageBox::warning(NULL, str_add_event_caption[lang], text, QMessageBox::Yes);
}

static const QString str_name_empty[2] = {
	"Please fill the event's name!", "请填写事件名称！"
};

static int conv_to_int(QString s, int l, int r) {
	bool ok = false;
	int ret = s.toInt(&ok, 10);
	if (!ok) ret = l - 1;
	return ret;
}

static inline bool is_leap_year(int y) {
	return y % 4 == 0 || (y % 100 != 0 && y % 400 == 0);
}

static const int days_in_months[2][13] = {
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static const QString str_invalid_datetime[2] = {
	"Please fill with a correct date & time!", "请填写一个正确的日期和时间！"
};


void AddEventDialog::on_accept_clicked() {
	QString name = ui->txt_event_name->text().trimmed();
	if (name == "") {
		return gen_warning(str_name_empty[lang]), void();
	}
	
	QString s_yy, s_mm, s_dd, s_h, s_m;
	s_yy = ui->txt_year->text();
	s_mm = ui->txt_month->text();
	s_dd = ui->txt_date->text();
	s_h = ui->txt_hour->text();
	s_m = ui->txt_minute->text();
	
	int yy = conv_to_int(s_yy, 1920, 9999);
	int mm = conv_to_int(s_mm, 1, 12);
	int dd = conv_to_int(s_dd, 1, 31);
	int h = conv_to_int(s_h, 0, 23);
	int m = conv_to_int(s_m, 0, 59);
	
	bool invalid_datetime = false;
	if (yy < 1920 || yy > 9999) invalid_datetime = true;
	if (mm < 1 || mm > 12) invalid_datetime = true;
	if (invalid_datetime) {
		return gen_warning(str_invalid_datetime[lang]), void();
	}
	bool is_l = is_leap_year(yy);
	int days = days_in_months[is_l][mm];
	if (dd < 1 || dd > days) invalid_datetime = true;
	if (h < 0 || h > 23) invalid_datetime = true;
	if (m < 0 || m > 59) invalid_datetime = true;
	if (invalid_datetime) {
		return gen_warning(str_invalid_datetime[lang]), void();
	}
	
	QString location = ui->txt_event_location->text().trimmed();
	
	long long datetime = datetime_to_ll(yy, mm, dd, h, m, 0);
	
	int repeat_type = 0, repeat_interval = 0;
	if (ui->chk_repeat->isChecked()) {
		repeat_type = ui->combo_repeat->currentIndex() + 1;
		repeat_interval = ui->spin_repeat->value();
		// QMessageBox::warning(NULL, QString::number(repeat_type), "heiheihei", QMessageBox::Yes);
	}
	
	eventmanager->set_add_event(datetime, name, location, repeat_type, repeat_interval);
	
	this->accept();
}


void AddEventDialog::on_chk_repeat_toggled(bool checked)
{
	if (checked) {
		ui->lbl_repeat_per->show();
		ui->spin_repeat->show();
		ui->combo_repeat->show();
	} else {
		ui->lbl_repeat_per->hide();
		ui->spin_repeat->hide();
		ui->combo_repeat->hide();
	}
}

void AddEventDialog::on_edit_clicked() {
	QString name = ui->txt_event_name->text().trimmed();
	if (name == "") {
		return gen_warning(str_name_empty[lang]), void();
	}
	
	QString s_yy, s_mm, s_dd, s_h, s_m;
	s_yy = ui->txt_year->text();
	s_mm = ui->txt_month->text();
	s_dd = ui->txt_date->text();
	s_h = ui->txt_hour->text();
	s_m = ui->txt_minute->text();
	
	int yy = conv_to_int(s_yy, 1920, 9999);
	int mm = conv_to_int(s_mm, 1, 12);
	int dd = conv_to_int(s_dd, 1, 31);
	int h = conv_to_int(s_h, 0, 23);
	int m = conv_to_int(s_m, 0, 59);
	
	bool invalid_datetime = false;
	if (yy < 1920 || yy > 9999) invalid_datetime = true;
	if (mm < 1 || mm > 12) invalid_datetime = true;
	if (invalid_datetime) {
		return gen_warning(str_invalid_datetime[lang]), void();
	}
	bool is_l = is_leap_year(yy);
	int days = days_in_months[is_l][mm];
	if (dd < 1 || dd > days) invalid_datetime = true;
	if (h < 0 || h > 23) invalid_datetime = true;
	if (m < 0 || m > 59) invalid_datetime = true;
	if (invalid_datetime) {
		return gen_warning(str_invalid_datetime[lang]), void();
	}
	
	QString location = ui->txt_event_location->text().trimmed();
	
	long long datetime = datetime_to_ll(yy, mm, dd, h, m, 0);
	
	int repeat_type = 0, repeat_interval = 0;
	if (ui->chk_repeat->isChecked()) {
		repeat_type = ui->combo_repeat->currentIndex() + 1;
		repeat_interval = ui->spin_repeat->value();
		// QMessageBox::warning(NULL, QString::number(repeat_type), "heiheihei", QMessageBox::Yes);
	}
	
	eventmanager->set_edit_event(ID, datetime, name, location, repeat_type, repeat_interval);
	
	this->accept();
}
