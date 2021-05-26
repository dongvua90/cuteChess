#ifndef NEWGAMEONLINEFRIENDDLG_H
#define NEWGAMEONLINEFRIENDDLG_H
#include <QDialog>
#include "virtualkeyboard.h"
#include "chessgame.h"

namespace Ui {
class BeginGameonline;
}

class NewgameOnlineFriendDlg : public QDialog
{
    Q_OBJECT
public:
    explicit NewgameOnlineFriendDlg(QWidget *parent);
    ChessGame* createGame();
    void createLichessGame();
    bool humanIsWhite();
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

    void on_btn_friend_toggled(bool checked);

private:
    Ui::BeginGameonline *ui;
    int m_timer;
    int m_timeInc;
    bool m_color;
    bool m_billElo;
    int m_variant;
    QString m_nickname;
    int s_timer;
    int s_timeInc;
    QString s_variant;
    void setTimer(int timer);
    int  convertTimer();
    int  convertTimeInc();
    void setTimeInc(int time);
    void readSetting();
    void writeSetting();
    VirtualKeyboard *vtKeyboard;

};

#endif // NEWGAMEONLINEFRIENDDLG_H
