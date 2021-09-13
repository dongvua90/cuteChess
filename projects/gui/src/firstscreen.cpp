#include "firstscreen.h"
#include "ui_firstscreen.h"
#include "robochessapp.h"

FirstScreen::FirstScreen(QWidget *parent) : QDialog(parent)
  ,ui(new Ui::Fristscreen)
{
    ui->setupUi(this);
    move(0,0);
    ui->header_layout->setAlignment(Qt::AlignRight);
    info = new TaskInfo();
    ui->header_layout->addWidget(info);

    dlg_newGameOffLine          = new NewGameOfflineDialog();
    dlg_newGameOnlineComputer   = new NewGameOnlineComputerDialog(this);
    dlg_newgameOnlineFriend     = new NewgameOnlineFriendDlg(this);
    dlg_settings                = new Settings(this);               // dialog settings

    connect(RobochessApplication::instance()->lichess,&Lichess::usernameChanged,this,&FirstScreen::changeUserName);
}

void FirstScreen::changeUserName(QString username)
{
    ui->lb_username->setText(username);
}

void FirstScreen::on_btn_friend_clicked()
{
    if(dlg_newgameOnlineFriend->exec() == QDialog::Accepted)
    {
         emit onGameonlineFriend();
    }
}

void FirstScreen::on_btn_computer_clicked()
{
    if(dlg_newGameOnlineComputer->exec() == QDialog::Accepted)
    {
         emit onGameonlineComputer();
    }
}

void FirstScreen::on_setting_clicked()
{
    dlg_settings->exec();
}

void FirstScreen::on_btn_playOffline_clicked()
{
   if(dlg_newGameOffLine->exec() == QDialog::Accepted)
   {
       emit onCpuGame();
       close();
   }
}
