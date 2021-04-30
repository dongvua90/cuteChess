#ifndef TASKINFO_H
#define TASKINFO_H

#include <QWidget>
#include <QLabel>
class TaskInfo : public QWidget
{
    Q_OBJECT
public:
    TaskInfo();
    void setSound(bool enable);
    void setBattery(uint8_t bat);
public slots:
    void setWifiSignal(uint8_t signal);
    void onInfo();
    void onValue(int);

private:
    QLabel *wifi,*sound,*battery;
};

#endif // TASKINFO_H
