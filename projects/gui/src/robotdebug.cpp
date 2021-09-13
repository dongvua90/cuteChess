#include "robotdebug.h"
#include <QDebug>

RobotDebug::RobotDebug(QObject *parent) : QObject(parent)
{

}

void RobotDebug::armRobotMove(QString mov)
{
    qDebug()<<"ARM-robot-movr:"<<mov;
}

void RobotDebug::requestMakeMove()
{
    qDebug()<<"BOARD-request-move->";
}

void RobotDebug::boardMoveError()
{
    qDebug()<<"BOARD-move-error";
}
