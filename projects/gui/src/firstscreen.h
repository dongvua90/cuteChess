#ifndef FIRSTSCREEN_H
#define FIRSTSCREEN_H
#include <QDialog>
#include "taskinfo.h"

namespace Ui {
class Fristscreen;
}

class FirstScreen : public QDialog
{
    Q_OBJECT
public:
    explicit FirstScreen(QWidget *parent = nullptr);

signals:
    void onCpuGame();
    void onOnlineGame();

private slots:
    void on_btn_friend_clicked();

    void on_btn_computer_clicked();

    void on_setting_clicked();

private:
    Ui::Fristscreen *ui;
    TaskInfo *info;

};

#endif // FIRSTSCREEN_H
