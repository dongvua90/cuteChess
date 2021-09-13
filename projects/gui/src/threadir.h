#ifndef THREADIR_H
#define THREADIR_H
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>
#include <QThread>
#include <QObject>

class ThreadIR : public QThread
{
    Q_OBJECT
public:
    explicit ThreadIR(QObject *parent = 0);
    void run();
    void clearHumanEnter();
    bool getHumanEnter();
    virtual ~ThreadIR() {};

signals:
    void onHumanEnter();
    void onButtonTest();
private:
    struct input_event ev[4];
    int checkIr();
    bool isHumanEnter=false;
};

#endif // THREADIR_H
