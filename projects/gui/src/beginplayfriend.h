#ifndef BEGINPLAYFRIEND_H
#define BEGINPLAYFRIEND_H
#include <QDialog>
#include "virtualkeyboard.h"

namespace Ui {
class BeginGameonline;
}

class BeginPlayFriend : public QDialog
{
    Q_OBJECT
public:
    explicit BeginPlayFriend(QWidget *parent);

signals:
    void onPlayOnline(int timer,int timeInc,QString variant,bool color,bool billELO,QString nickname);
private slots:
    void onKeyboardEnter(QString data);
    void onKeyboardCancel();
    void on_slider_timer_valueChanged(int value);

    void on_slider_timeInc_valueChanged(int value);

    void on_btn_close_clicked();

    void on_btn_ok_clicked();

    void on_btn_playerColor_toggled(bool checked);

    void on_btn_billElo_toggled(bool checked);

    void on_btn_inputuser_clicked();

private:
    Ui::BeginGameonline *ui;
    int m_timer;
    int m_timeInc;
    bool m_color;
    bool m_billElo;
    int m_variant;
    QString m_nickname;
    void setTimer(int timer);
    int  convertToTimer();
    int  convertToTimeInc();
    void setTimeInc(int time);
    void readSetting();
    void writeSetting();
    VirtualKeyboard *vtKeyboard;

};

#endif // BEGINPLAYFRIEND_H
