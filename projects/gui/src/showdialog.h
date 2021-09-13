#ifndef SHOWDIALOG_H
#define SHOWDIALOG_H

#include <QDialog>
#include <QTimer>
namespace Ui {
class Mdialog;
}

class ShowDialog : public QDialog
{
    Q_OBJECT
public:
    ShowDialog(QWidget *parent);
public slots:
    void setTitle(QString title);
    void setInfo(QString info);
    void showDialog(QString title,QString info,int time_show=2000,bool is_show_btn_cancel=false,bool is_show_btn_ok=false);
private:
    Ui::Mdialog *ui;
    QTimer *timer;
private slots:
    void hideDialog();
    void on_btn_ok_clicked();
    void on_btn_cancel_clicked();
};

#endif // SHOWDIALOG_H
