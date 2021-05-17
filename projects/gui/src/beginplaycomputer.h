#ifndef BEGINPLAYCOMPUTER_H
#define BEGINPLAYCOMPUTER_H

#include <QDialog>
#include <QWidget>
#include <QSettings>
namespace Ui {
class BeginGameonlineComputer;
}
class BeginPlayComputer : public QDialog
{
    Q_OBJECT
public:
    explicit BeginPlayComputer(QWidget *parent);
signals:
    void challengeAI(int level,int time_limit,int time_inc,QString variant,bool color);
private slots:
    void on_slider_level_valueChanged(int value);

    void on_slider_timer_valueChanged(int value);

    void on_slider_timeInc_valueChanged(int value);

    void on_btn_playerColor_toggled(bool checked);

    void on_btn_ok_clicked();

    void on_btn_close_clicked();

private:
    Ui::BeginGameonlineComputer *ui;
    int m_timer;
    int m_timeInc;
    bool m_color;
    int m_level;
    int m_variant;

    void readSetting();
    void writeSetting();
    void setTimer(int timer);
    int  convertToTimer();
    int  convertToTimeInc();
    void setTimeInc(int time);
    void setLevel(int value);
};

#endif // BEGINPLAYCOMPUTER_H
