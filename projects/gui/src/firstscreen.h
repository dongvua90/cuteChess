#ifndef FIRSTSCREEN_H
#define FIRSTSCREEN_H
#include <QDialog>
#include "taskinfo.h"
#include "robot.h"
#include "newgameonlinecomputerdialog.h"
#include "newgameofflinedlg.h"
#include "newgameonlinefrienddlg.h"
#include "settings.h"

namespace Ui {
class Fristscreen;
}

class FirstScreen : public QDialog
{
    Q_OBJECT
public:
    explicit FirstScreen(QWidget *parent);
    NewGameOfflineDialog        *dlg_newGameOffLine;
    NewGameOnlineComputerDialog *dlg_newGameOnlineComputer;
    NewgameOnlineFriendDlg      *dlg_newgameOnlineFriend;
    Settings                    *dlg_settings;
public slots:
    void changeUserName(QString username);

signals:
    void onCpuGame();
    void onGameonlineFriend();
    void onGameonlineComputer();

private slots:
    void on_btn_friend_clicked();

    void on_btn_computer_clicked();

    void on_setting_clicked();

    void on_btn_playOffline_clicked();

private:
    Ui::Fristscreen *ui;
    TaskInfo *info;
    Robot *robot;

};

#endif // FIRSTSCREEN_H
