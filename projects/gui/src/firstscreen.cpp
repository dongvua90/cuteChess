#include "firstscreen.h"
#include "ui_firstscreen.h"
#include "cutechessapp.h"

FirstScreen::FirstScreen(QWidget *parent) : QDialog(parent)
  ,ui(new Ui::Fristscreen)
{
    ui->setupUi(this);
    move(0,0);
    ui->header_layout->setAlignment(Qt::AlignRight);
    info = new TaskInfo();
    ui->header_layout->addWidget(info);

  robot = CuteChessApplication::instance()->getIntanceRobot();
  connect(robot,&Robot::onBatteryChanged,this,&FirstScreen::on_battery_changed);
}

void FirstScreen::on_battery_changed(int bat, bool isChanger)
{
    if(bat >= 1600){
        info->setBattery(5);
    }else if(bat <1600 && bat >=1550){
        info->setBattery(4);
    }else if(bat <1550 && bat >=1470){
        info->setBattery(3);
    }else if(bat <1470 && bat >=1400){
        info->setBattery(2);
    }else if(bat <1400 && bat >=1330){
        info->setBattery(1);
    }else if(bat <1330){
        info->setBattery(0);
    }
    if(isChanger){
        info->setBattery(6);
    }
}

void FirstScreen::on_btn_friend_clicked()
{

//    close();
    setVisible(false);
    emit onOnlineGame();
}

void FirstScreen::on_btn_computer_clicked()
{
    setVisible(false);
    emit onGameonlineComputer();
}

void FirstScreen::on_setting_clicked()
{

}

void FirstScreen::on_btn_playOffline_clicked()
{
    setVisible(false);
    emit onCpuGame();
}
