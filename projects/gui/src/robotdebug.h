#ifndef ROBOTDEBUG_H
#define ROBOTDEBUG_H
#include <QObject>

class RobotDebug : public QObject
{
    Q_OBJECT
public:
    explicit RobotDebug(QObject *parent = nullptr);

public slots:
    void armRobotMove(QString mov);
    void requestMakeMove();
    void boardMoveError();

signals:
    void armRobotMovedFinish();
    void boardSensorMakeMove(QString mov);
};

#endif // ROBOTDEBUG_H
