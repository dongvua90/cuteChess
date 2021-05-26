#include "settings.h"
#include "ui_settings.h"
#include "robochessapp.h"
Settings::Settings(QWidget *parent) :QDialog(parent),
    ui(new Ui::DlgSettings)
{
    ui->setupUi(this);
    move(0,0);
    setting = new QSettings();
    speed_mode = setting->value("SETTING_SPPEDMODE").toBool();
    RobochessApplication::instance()->info_sound  = setting->value("SETTING_SOUND").toBool();
    token       = setting->value("SETTING_TOKEN").toString();
    ui->btn_speedMode->setChecked(speed_mode);
    ui->btn_sound->setChecked(RobochessApplication::instance()->info_sound);
    ui->btn_inputtoken->setText(token);

    vtKeyboard = RobochessApplication::instance()->vtKeyboard;
    connect(vtKeyboard,&VirtualKeyboard::keyboarEnter,this,&Settings::onKeyboardEnter);
    connect(vtKeyboard,&VirtualKeyboard::keyboardCancel,this,&Settings::onKeyboardCancel);
    connect(RobochessApplication::instance()->lichess,&Lichess::usernameChanged,this,&Settings::onUsernameChanged);
}

void Settings::onKeyboardEnter(QString data)
{
    if(vtKeyboard->editing == vtKeyboard->INPUT_TOKEN){
        token = data;
        setting->setValue("SETTING_TOKEN",token);
        ui->btn_inputtoken->setText(token);
        RobochessApplication::instance()->lichess->m_token = token;
        RobochessApplication::instance()->lichess->getAccount();
        vtKeyboard->hide();
    }
}

void Settings::onKeyboardCancel()
{
    vtKeyboard->hide();
}

void Settings::onUsernameChanged(QString username)
{
    if(username.length()<1)
        ui->lb_username->setText("token error!");
    else
        ui->lb_username->setText(username);
}

void Settings::on_btn_close_clicked()
{
    close();
}

void Settings::on_btn_speedMode_toggled(bool checked)
{
    speed_mode = checked;
    setting->setValue("SETTING_SPPEDMODE",speed_mode);
}

void Settings::on_btn_sound_toggled(bool checked)
{
    RobochessApplication::instance()->info_sound = checked;
    setting->setValue("SETTING_SOUND",RobochessApplication::instance()->info_sound);
}

void Settings::on_btn_wifi_scan_clicked()
{

}

void Settings::on_btn_help_clicked()
{

}

void Settings::on_btn_inputtoken_clicked()
{
    vtKeyboard->editing = vtKeyboard->INPUT_TOKEN;
    vtKeyboard->setData(token);
    vtKeyboard->exec();
}
