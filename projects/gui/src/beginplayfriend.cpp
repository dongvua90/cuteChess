#include "beginplayfriend.h"
#include "ui_beginGameOnine.h"
#include <QSettings>
#include <QDebug>
#include "cutechessapp.h"

BeginPlayFriend::BeginPlayFriend(QWidget *parent) : QDialog(parent)
  ,ui(new Ui::BeginGameonline)
{
    ui->setupUi(this);
    ui->comboBox_variant->addItem("Standard");
    ui->comboBox_variant->addItem("CrazyHouse");
    ui->comboBox_variant->addItem("Chess960");
    ui->comboBox_variant->addItem("King of the Hill");
    ui->comboBox_variant->addItem("Three-Check");
    ui->comboBox_variant->addItem("AntiChess");
    ui->comboBox_variant->addItem("Atomic");
    ui->comboBox_variant->addItem("Horde");
    ui->comboBox_variant->addItem("RacingKings");
    ui->comboBox_variant->addItem("FromPosition");

    readSetting();
    ui->comboBox_variant->setCurrentIndex(m_variant);
    setTimer(m_timer);
    setTimeInc(m_timeInc);
    ui->slider_timeInc->setValue(m_timeInc);
    ui->slider_timer->setValue(m_timer);
    ui->btn_playerColor->setChecked(m_color);
    on_btn_playerColor_toggled(m_color);
    on_btn_billElo_toggled(m_billElo);
    ui->btn_inputuser->setText(m_nickname);
    vtKeyboard = CuteChessApplication::instance()->vtKeyboard;
    connect(vtKeyboard,&VirtualKeyboard::keyboarEnter,this,&BeginPlayFriend::onKeyboardEnter);
    connect(vtKeyboard,&VirtualKeyboard::keyboardCancel,this,&BeginPlayFriend::onKeyboardCancel);
}

void BeginPlayFriend::onKeyboardEnter(QString data)
{
    m_nickname = data;
    ui->btn_inputuser->setText(m_nickname);
    vtKeyboard->hide();
}

void BeginPlayFriend::onKeyboardCancel()
{
    vtKeyboard->hide();
}


void BeginPlayFriend::on_slider_timer_valueChanged(int value)
{
    m_timer = value;
    setTimer(m_timer);
}

void BeginPlayFriend::on_slider_timeInc_valueChanged(int value)
{
    m_timeInc = value;
    setTimeInc(m_timeInc);
}

void BeginPlayFriend::on_btn_close_clicked()
{
    close();
}

void BeginPlayFriend::on_btn_ok_clicked()
{
    writeSetting();
    emit onPlayOnline(m_timer,m_timeInc,ui->comboBox_variant->currentText(),m_color,m_billElo,m_nickname);
    close();
}

void BeginPlayFriend::setTimer(int timer)
{
    switch (timer) {
        case 0: ui->lb_timer->setText("5 min"); break;
        case 1: ui->lb_timer->setText("10 min"); break;
        case 2: ui->lb_timer->setText("20 min"); break;
        case 3: ui->lb_timer->setText("30 min"); break;
        case 4: ui->lb_timer->setText("45 min"); break;
        case 5: ui->lb_timer->setText("60 min"); break;
        case 6: ui->lb_timer->setText("120 min"); break;
        case 7: ui->lb_timer->setText("180 min"); break;
        case 8: ui->lb_timer->setText("no limit"); break;
    }
}

int BeginPlayFriend::convertToTimer()
{
    int tim;
    switch (m_timer) {
        case 0: tim=5; break;
        case 1: tim=10; break;
        case 2: tim=20; break;
        case 3: tim=30; break;
        case 4: tim=45; break;
        case 5: tim=60; break;
        case 6: tim=120; break;
        case 7: tim=180; break;
        case 8: tim=1000; break;
    }
    return tim;
}

int BeginPlayFriend::convertToTimeInc()
{
    int tim;
    switch (m_timer) {
        case 0: tim=0; break;
        case 1: tim=3; break;
        case 2: tim=5; break;
        case 3: tim=10; break;
        case 4: tim=15; break;
        case 5: tim=20; break;
        case 6: tim=30; break;
        case 7: tim=45; break;
        case 8: tim=60; break;
    }
    return tim;
}

void BeginPlayFriend::setTimeInc(int time)
{
    switch (time) {
        case 0: ui->lb_timeInc->setText("0 s"); break;
        case 1: ui->lb_timeInc->setText("3 s"); break;
        case 2: ui->lb_timeInc->setText("5 s"); break;
        case 3: ui->lb_timeInc->setText("10 s"); break;
        case 4: ui->lb_timeInc->setText("15 s"); break;
        case 5: ui->lb_timeInc->setText("20 s"); break;
        case 6: ui->lb_timeInc->setText("30 s"); break;
        case 7: ui->lb_timeInc->setText("45 s"); break;
        case 8: ui->lb_timeInc->setText("60 s"); break;
    }
}

void BeginPlayFriend::readSetting()
{
        QSettings setting;
        m_timer = setting.value("ONLINE-timer").toInt();
        m_timeInc = setting.value("ONLINE-timerInc").toInt();
        m_billElo = setting.value("ONLINE-billElo").toBool();
        m_color = setting.value("ONLINE-color").toBool();
        m_variant = setting.value("ONLINE-variant").toInt();
        m_nickname = setting.value("ONLINE-nickname").toString();
}

void BeginPlayFriend::writeSetting()
{
    m_variant = ui->comboBox_variant->currentIndex();
    QSettings setting;
    setting.setValue("ONLINE-timer",m_timer);
    setting.setValue("ONLINE-timerInc",m_timeInc);
    setting.setValue("ONLINE-billElo",m_billElo);
    setting.setValue("ONLINE-color",m_color);
    setting.setValue("ONLINE-variant",m_variant);
    setting.setValue("ONLINE-nickname",m_nickname);
}


void BeginPlayFriend::on_btn_playerColor_toggled(bool checked)
{
    m_color = checked;
    if(checked){
        ui->btn_playerColor->setText("play as Black");
    }else{
        ui->btn_playerColor->setText("play as White");
    }
}

void BeginPlayFriend::on_btn_billElo_toggled(bool checked)
{
    m_billElo = checked;
    if(checked){
        ui->btn_billElo->setText("bill ELO");
    }else{
        ui->btn_billElo->setText("no Bill ELO");
    }
}

void BeginPlayFriend::on_btn_inputuser_clicked()
{
    vtKeyboard->setData(m_nickname);
    vtKeyboard->exec();
}
