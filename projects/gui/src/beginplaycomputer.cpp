#include "beginplaycomputer.h"
#include "ui_beginGameComputer.h"
#include "cutechessapp.h"

BeginPlayComputer::BeginPlayComputer(QWidget *parent) : QDialog(parent)
  ,ui(new Ui::BeginGameonlineComputer)
  ,m_level(1)
{
    ui->setupUi(this);
    ui->comboBox_variant->addItem("standard");
    ui->comboBox_variant->addItem("crazyhouse");
    ui->comboBox_variant->addItem("chess960");
    ui->comboBox_variant->addItem("kingOfTheHill");
    ui->comboBox_variant->addItem("threeCheck");
    ui->comboBox_variant->addItem("antichess");
    ui->comboBox_variant->addItem("atomic");
    ui->comboBox_variant->addItem("horde");
    ui->comboBox_variant->addItem("racingKings");
//    ui->comboBox_variant->addItem("FromPosition");
    readSetting();
    ui->comboBox_variant->setCurrentIndex(m_variant);
    ui->slider_level->setValue(m_level);
    setTimeInc(m_timeInc);
    ui->slider_timeInc->setValue(m_timeInc);
    setTimer(m_timer);
    ui->slider_timer->setValue(m_timer);
    setLevel(m_level);
    on_btn_playerColor_toggled(m_color);
}

void BeginPlayComputer::readSetting()
{
    QSettings setting;
    m_timer = setting.value("COMPUTER-timer").toInt();
    m_timeInc = setting.value("COMPUTER-timerInc").toInt();
    m_level = setting.value("COMPUTER-level").toInt();
    m_color = setting.value("COMPUTER-color").toBool();
    m_variant = setting.value("COMPUTER-variant").toInt();
}

void BeginPlayComputer::writeSetting()
{
    m_variant = ui->comboBox_variant->currentIndex();
    QSettings setting;
    setting.setValue("COMPUTER-timer",m_timer);
    setting.setValue("COMPUTER-timerInc",m_timeInc);
    setting.setValue("COMPUTER-level",m_level);
    setting.setValue("COMPUTER-color",m_color);
    setting.setValue("COMPUTER-variant",m_variant);
}

void BeginPlayComputer::setTimer(int timer)
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

int BeginPlayComputer::convertToTimer()
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
        case 8: tim=0; break;
    }
    return tim;
}

int BeginPlayComputer::convertToTimeInc()
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

void BeginPlayComputer::setTimeInc(int time)
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

void BeginPlayComputer::setLevel(int value)
{
    m_level = value;
    ui->lb_level->setText(QString::number(m_level));
}

void BeginPlayComputer::on_slider_level_valueChanged(int value)
{
   setLevel(value);
}

void BeginPlayComputer::on_slider_timer_valueChanged(int value)
{
    m_timer = value;
    setTimer(m_timer);
}

void BeginPlayComputer::on_slider_timeInc_valueChanged(int value)
{
    m_timeInc = value;
    setTimeInc(m_timeInc);
}

void BeginPlayComputer::on_btn_playerColor_toggled(bool checked)
{
    m_color = checked;
    ui->btn_playerColor->setChecked(checked);
    if(checked){
        ui->btn_playerColor->setText("play as Black");
    }else{
        ui->btn_playerColor->setText("play as White");
    }
}

void BeginPlayComputer::on_btn_ok_clicked()
{
    writeSetting();
    emit challengeAI(m_level,convertToTimer()*60,convertToTimeInc(),ui->comboBox_variant->currentText(),m_color);
    close();
}

void BeginPlayComputer::on_btn_close_clicked()
{
    close();
}
