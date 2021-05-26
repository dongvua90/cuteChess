#include "taskinfo.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPixmap>
#include <QTimerEvent>
#include <QDebug>
#include <QString>
#include "robochessapp.h"

TaskInfo::TaskInfo(QWidget *parent) : QWidget(parent)
{
    timer = new QTimer();
    wifi = new QLabel();
    sound = new QLabel();
    battery = new QLabel();
    battery_charging = new QLabel();
    QHBoxLayout *layout = new QHBoxLayout();


    wifi->setPixmap(QPixmap(":/ui/wifi_signal3.png"));
    sound->setPixmap(QPixmap(":/ui/sound_on.png"));
    battery->setPixmap(QPixmap(":/ui/battery_level4.png"));
    battery_charging->setPixmap(QPixmap(":ui/charger.png"));
    wifi->setFixedWidth(50);
    sound->setFixedWidth(40);
    battery->setFixedWidth(30);

    battery_charging->setParent(battery);
    battery_charging->move(6,6);
    battery_charging->setVisible(false);

    layout->setAlignment(Qt::AlignRight);
    layout->setMargin(0);
    layout->addWidget(wifi);
    layout->addWidget(sound);
    layout->addWidget(battery);
    setFixedHeight(43);
    setFixedWidth(130);
    setLayout(layout);

    connect(timer,&QTimer::timeout,this,&TaskInfo::update);
    timer->start(1000);
}

void TaskInfo::setWifiSignal(uint8_t signal)
{
    switch(signal)
    {
    case 0: wifi->setPixmap(QPixmap(":/ui/mwifi_error.png"));
        break;
    case 1: wifi->setPixmap(QPixmap(":/ui/mwifi_signal1.png"));
        break;
    case 2: wifi->setPixmap(QPixmap(":/ui/mwifi_signal2.png"));
        break;
    case 3: wifi->setPixmap(QPixmap(":/ui/mwifi_signal3.png"));
        break;
    case 4: wifi->setPixmap(QPixmap(":/ui/mwifi_signal4.png"));
        break;
    }
}

void TaskInfo::onInfo()
{
    qDebug("onTaskinfo");
}

void TaskInfo::onValue(int value)
{
    qDebug()<<"onValue:"<<value;
}

void TaskInfo::update()
{
    setWifiSignal(RobochessApplication::instance()->info_wifi);
    setBattery(RobochessApplication::instance()->info_sound);
    setCharger(RobochessApplication::instance()->info_battery_charger);
    if(RobochessApplication::instance()->info_battery_charger && RobochessApplication::instance()->info_battery==0)
    {
        RobochessApplication::instance()->info_battery = 1;
        setBattery(1);
    }else{
        setBattery(RobochessApplication::instance()->info_battery);
    }
    setSound(RobochessApplication::instance()->info_sound);
}

void TaskInfo::setSound(bool enable)
{
    if(enable)
    {
        sound->setPixmap(QPixmap(":/ui/sound_on.png"));
    }else
    {
        sound->setPixmap(QPixmap(":/ui/sound_off.png"));
    }
}

void TaskInfo::setBattery(uint8_t bat)
{
    switch(bat)
    {
    case 0: battery->setPixmap(QPixmap(":/ui/battery_error.png"));
        break;
    case 1: battery->setPixmap(QPixmap(":/ui/battery_level1.png"));
        break;
    case 2: battery->setPixmap(QPixmap(":/ui/battery_level2.png"));
        break;
    case 3: battery->setPixmap(QPixmap(":/ui/battery_level3.png"));
        break;
    case 4: battery->setPixmap(QPixmap(":/ui/battery_level4.png"));
        break;
    case 5: battery->setPixmap(QPixmap(":/ui/battery_level5.png"));
        break;
    }
}

void TaskInfo::setCharger(bool ischarging)
{
    battery_charging->setVisible(ischarging);
}
