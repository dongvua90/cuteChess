#ifndef TASKINFO_H
#define TASKINFO_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

class TaskInfo : public QWidget
{
    Q_OBJECT
public:
    TaskInfo(QWidget *parent = nullptr);
    void setSound(bool enable);
    void setBattery(int bat);
    void setCharger(bool ischarging);
public slots:
    void setWifiSignal(uint8_t signal);
    void onInfo();
    void onValue(int);

private:
    QLabel *wifi,*sound,*battery,*battery_charging;
    QTimer *timer;
private slots:
    void update();
};

#endif // TASKINFO_H
