#include "challengedialog.h"
#include "ui_challenge.h"
#include "robochessapp.h"

ChallengeDialog::ChallengeDialog(QWidget* parent) :QDialog(parent)
  ,ui(new Ui::Challenge)
{
    ui->setupUi(this);
}

void ChallengeDialog::setChallenger(QString rival_name, QString info)
{
    ui->lb_rivalname->setText(rival_name);
    ui->lb_info->setText(info);
}

void ChallengeDialog::on_btn_ok_clicked()
{
    RobochessApplication::instance()->lichess->acceptChallenge();
    close();
}

void ChallengeDialog::on_btn_decline_clicked()
{
    RobochessApplication::instance()->lichess->declineChallenge();
    close();
}
