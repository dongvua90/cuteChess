#include "firstscreen.h"
#include "ui_firstscreen.h"

FirstScreen::FirstScreen(QWidget *parent) : QDialog(parent)
  ,ui(new Ui::Fristscreen)
{
    ui->setupUi(this);

    ui->header_layout->setAlignment(Qt::AlignRight);
    info = new TaskInfo();
    ui->header_layout->addWidget(info);
}

void FirstScreen::on_btn_friend_clicked()
{

//    close();
    setVisible(false);
    emit onOnlineGame();
}

void FirstScreen::on_btn_computer_clicked()
{

//    close();
    setVisible(false);
    emit onCpuGame();
}

void FirstScreen::on_setting_clicked()
{

}
