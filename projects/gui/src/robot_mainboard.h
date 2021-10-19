#ifndef ROBOT_MAINBOARD_H
#define ROBOT_MAINBOARD_H

#include <QThread>
#include <QProcess>
#include <QtDebug>

class robot_mainboard : public QThread
{
public:
    explicit robot_mainboard(QObject *parent = nullptr);
    void run();
signals:
    void armMoved();
private:
    void mloop();
};

#endif // ROBOT_MAINBOARD_H
