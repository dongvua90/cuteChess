#include "threadir.h"
#include <QDebug>
#define IRCODE_ENTER 32798

ThreadIR::ThreadIR(QObject *parent) :QThread(parent)
{

}

void ThreadIR::run()
{
    checkIr();
}

void ThreadIR::clearHumanEnter()
{
    isHumanEnter = false;
}

bool ThreadIR::getHumanEnter()
{
    return isHumanEnter;
}

int ThreadIR::checkIr()
{
    int fd_ir;
    fd_ir = open("/dev/input/event1",O_RDONLY);
    if(fd_ir <0){
       printf("error open device");
       return -1;
    }
    while (1) {
        int rd = read(fd_ir, ev, sizeof(ev));
        if (rd < (int) sizeof(struct input_event)) {
           perror("\nevtest: error reading");
           return -1;
        }

        for (int i = 0; i < rd / sizeof(struct input_event); i++) {
            if(ev[i].value >0){
                int code = ev[i].value;
                ioctl(fd_ir, EVIOCGRAB, (void*)0);
                printf("IRcode:%d\r\n",code);
                if(code == IRCODE_ENTER)
                    emit onHumanEnter();
            }
        }
    }

    close(fd_ir);
    return 0;
}