#ifndef BEGINPLAYFRIEND_H
#define BEGINPLAYFRIEND_H
#include <QDialog>

namespace Ui {
class BeginGameonline;
}

class BeginPlayFriend : public QDialog
{
    Q_OBJECT
public:
    explicit BeginPlayFriend();

signals:
    void onPlayOnline(int timer,int timeInc,QString variant,bool color,bool billELO,QString nickname);
private slots:

    void on_slider_timer_valueChanged(int value);

    void on_slider_timeInc_valueChanged(int value);

    void on_btn_close_clicked();

    void on_btn_ok_clicked();

    void on_btn_playerColor_toggled(bool checked);

    void on_btn_billElo_toggled(bool checked);

    void on_lineEdit_lichessUser_editingFinished();

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

};

#endif // BEGINPLAYFRIEND_H
