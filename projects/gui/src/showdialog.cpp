#include "showdialog.h"
#include "ui_dialog.h"
ShowDialog::ShowDialog(QWidget *parent):QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::Mdialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    move(140,120);
    ui->btn_cancel->setVisible(false);
    ui->btn_ok->setVisible(false);

    timer = new QTimer();
    connect(timer,&QTimer::timeout,this,&ShowDialog::hideDialog);
}

void ShowDialog::setTitle(QString title)
{
    ui->lb_title->setText(title);
}

void ShowDialog::setInfo(QString info)
{
    ui->lb_info->setText(info);
}

void ShowDialog::showDialog(QString title, QString info, int time_show, bool is_show_btn_cancel, bool is_show_btn_ok)
{
    if(is_show_btn_cancel){
        ui->btn_cancel->setVisible(true);
    }
    if(is_show_btn_ok){
        ui->btn_ok->setVisible(true);
    }
     ui->lb_title->setText(title);
     ui->lb_info->setText(info);
     timer->start(time_show);
     this->show();
}

void ShowDialog::hideDialog()
{
    close();
}

void ShowDialog::on_btn_ok_clicked()
{

}

void ShowDialog::on_btn_cancel_clicked()
{

}
