#include "robot_mainboard.h"
#include "robochessapp.h"
#include <QTimer>
robot_mainboard::robot_mainboard(QObject *parent) : QThread(parent)
{

}

void robot_mainboard::run()
{
    int fd =open("/dev/ttyS1",O_RDWR | O_NONBLOCK);
    if(fd < 0){
        printf("Faile open ttyS1\r\n");
    }else if(write(fd,"I\r\n",3)!= 3){
        printf("ttyS1-Faile write Move to Mainboard\r\n");
    }
    close(fd);

    QProcess *process = new QProcess();
    connect(process,&QProcess::readyRead,[process](){
        QByteArray data = process->readAll();
        qDebug() <<data;
        if(data.at(0)=='-'){
            if(data.at(1)=='B'){
                qDebug()<<"Batttery:";
                QByteArray dat = data.mid(2,4);
                int bat = dat.toInt();
                RobochessApplication::instance()->info_battery = bat;
                qDebug()<<"Bat:"<<bat;
            }else if(data.at(1)=='b'){
                QByteArray dat = data.mid(2,1);
                int isCharger = dat.toInt();
                RobochessApplication::instance()->info_battery_charger = isCharger;
                qDebug()<<"Charger:"<<isCharger;
            }else if(data.at(1)=='f'){
                RobochessApplication::instance()->info_arm_move_finish = 1;
                qDebug()<<"move finish";
            }
        }

    });
    process->start("cat /dev/ttyS1",QIODevice::ReadOnly);
    process->waitForFinished(-1);
    process->deleteLater();
}

void robot_mainboard::mloop()
{
    char buff[50];
        int fd =open("/dev/ttyS1",O_RDWR | O_NONBLOCK);
        if(fd < 0){
            printf("Faile open ttyS1\r\n");
        }else if(write(fd,"I\r\n",3)!= 3){
            printf("ttyS1-Faile write Move to Mainboard\r\n");
        }else if(read(fd,buff,10)!=10){
            printf("ttyS1 read faile");
        }
        printf("data:%s\r\n",buff);
        close(fd);
}
