#include "newgameonlinefrienddlg.h"
#include "ui_beginGameOnine.h"
#include <QSettings>
#include <QDebug>
#include "robochessapp.h"
#include <board/boardfactory.h>

NewgameOnlineFriendDlg::NewgameOnlineFriendDlg(QWidget *parent) : QDialog(parent)
  ,ui(new Ui::BeginGameonline)
{

    ui->setupUi(this);
    move(0,0);
    ui->btn_inputuser->setVisible(false);
    ui->comboBox_variant->addItem("Standard");
    ui->comboBox_variant->addItem("CrazyHouse");
    ui->comboBox_variant->addItem("Chess960");
    ui->comboBox_variant->addItem("King of the Hill");
    ui->comboBox_variant->addItem("Three-Check");
    ui->comboBox_variant->addItem("AntiChess");
    ui->comboBox_variant->addItem("Atomic");
    ui->comboBox_variant->addItem("Horde");
    ui->comboBox_variant->addItem("RacingKings");
    ui->comboBox_variant->addItem("FromPosition");

    readSetting();
    ui->comboBox_variant->setCurrentIndex(m_variant);
    setTimer(m_timer);
    setTimeInc(m_timeInc);
    ui->slider_timeInc->setValue(m_timeInc);
    ui->slider_timer->setValue(m_timer);
    ui->btn_playerColor->setChecked(m_color);
    on_btn_playerColor_toggled(m_color);
    on_btn_billElo_toggled(m_billElo);
    ui->btn_inputuser->setText(m_nickname);
    vtKeyboard = RobochessApplication::instance()->vtKeyboard;
    connect(vtKeyboard,&VirtualKeyboard::keyboarEnter,this,&NewgameOnlineFriendDlg::onKeyboardEnter);
        connect(vtKeyboard,&VirtualKeyboard::keyboardCancel,this,&NewgameOnlineFriendDlg::onKeyboardCancel);


}

ChessGame *NewgameOnlineFriendDlg::createGame()
{
    auto board = Chess::BoardFactory::create(ui->comboBox_variant->currentText());
    auto pgn = new PgnGame();
    pgn->setSite(QSettings().value("pgn/site").toString());
    auto game = new ChessGame(board, pgn);

    QString stim=QString::number(s_timer)+":00+"+QString::number(s_timeInc);
    Chess::Side human_side,Ai_side;
    if(m_color){
        human_side = Chess::Side::Black;
        Ai_side = Chess::Side::White;
    }else{
       human_side = Chess::Side::White;
       Ai_side=Chess::Side::Black;
    }
    game->setTimeControl(TimeControl(stim),human_side);
    game->setTimeControl(TimeControl(stim),Ai_side);
    return game;
}

void NewgameOnlineFriendDlg::createLichessGame()
{
    if(ui->btn_friend->isChecked()){ // chọn đấu với bạn
        RobochessApplication::instance()->lichess->ChallengeTheFriend(m_nickname,
            m_billElo,convertTimer()*60,convertTimeInc(),s_variant,m_color);
    }else{                          // chọn đấu với thế giới
        RobochessApplication::instance()->lichess->ChallengeTheWord(m_billElo,
            convertTimer(),convertTimeInc(),s_variant,m_color);
    }

}

bool NewgameOnlineFriendDlg::humanIsWhite()
{
    if(m_color)
        return false;
    return true;
}


void NewgameOnlineFriendDlg::onKeyboardEnter(QString data)
{
    if(vtKeyboard->editing==vtKeyboard->INPUT_FRIEND){
        m_nickname = data;
        ui->btn_inputuser->setText(m_nickname);
        vtKeyboard->hide();
    }
}

void NewgameOnlineFriendDlg::onKeyboardCancel()
{
    vtKeyboard->hide();
}


void NewgameOnlineFriendDlg::on_slider_timer_valueChanged(int value)
{
    m_timer = value;
    setTimer(m_timer);
}

void NewgameOnlineFriendDlg::on_slider_timeInc_valueChanged(int value)
{
    m_timeInc = value;
    setTimeInc(m_timeInc);
}

void NewgameOnlineFriendDlg::on_btn_close_clicked()
{
    close();
}

void NewgameOnlineFriendDlg::on_btn_ok_clicked()
{
    writeSetting();
    createLichessGame();
    accept();
}

void NewgameOnlineFriendDlg::setTimer(int timer)
{
    switch (timer) {
        case 0: ui->lb_timer->setText("5 min"); break;
        case 1: ui->lb_timer->setText("10 min"); break;
        case 2: ui->lb_timer->setText("20 min"); break;
        case 3: ui->lb_timer->setText("30 min"); break;
        case 4: ui->lb_timer->setText("45 min"); break;
        case 5: ui->lb_timer->setText("60 min"); break;
        case 6: ui->lb_timer->setText("120 min"); break;
        case 7: ui->lb_timer->setText("180 min"); break;
        case 8: ui->lb_timer->setText("no limit"); break;
    }
}

int NewgameOnlineFriendDlg::convertTimer()
{
    int s_timer;
    switch (m_timer) {
        case 0: s_timer=5; break;
        case 1: s_timer=10; break;
        case 2: s_timer=20; break;
        case 3: s_timer=30; break;
        case 4: s_timer=45; break;
        case 5: s_timer=60; break;
        case 6: s_timer=120; break;
        case 7: s_timer=180; break;
        case 8: s_timer=0; break;
    }
    return s_timer;
}

int NewgameOnlineFriendDlg::convertTimeInc()
{
    int s_timeInc;
    switch (m_timeInc) {
        case 0: s_timeInc=0; break;
        case 1: s_timeInc=3; break;
        case 2: s_timeInc=5; break;
        case 3: s_timeInc=10; break;
        case 4: s_timeInc=15; break;
        case 5: s_timeInc=20; break;
        case 6: s_timeInc=30; break;
        case 7: s_timeInc=45; break;
        case 8: s_timeInc=60; break;
    }
    return s_timeInc;
}

void NewgameOnlineFriendDlg::setTimeInc(int time)
{
    switch (time) {
        case 0: ui->lb_timeInc->setText("0 s"); break;
        case 1: ui->lb_timeInc->setText("3 s"); break;
        case 2: ui->lb_timeInc->setText("5 s"); break;
        case 3: ui->lb_timeInc->setText("10 s"); break;
        case 4: ui->lb_timeInc->setText("15 s"); break;
        case 5: ui->lb_timeInc->setText("20 s"); break;
        case 6: ui->lb_timeInc->setText("30 s"); break;
        case 7: ui->lb_timeInc->setText("45 s"); break;
        case 8: ui->lb_timeInc->setText("60 s"); break;
    }
}

void NewgameOnlineFriendDlg::readSetting()
{
        QSettings setting;
        m_timer = setting.value("ONLINE-timer").toInt();
        m_timeInc = setting.value("ONLINE-timerInc").toInt();
        m_billElo = setting.value("ONLINE-billElo").toBool();
        m_color = setting.value("ONLINE-color").toBool();
        m_variant = setting.value("ONLINE-variant").toInt();
        m_nickname = setting.value("ONLINE-nickname").toString();
}

void NewgameOnlineFriendDlg::writeSetting()
{
    m_variant = ui->comboBox_variant->currentIndex();
    QSettings setting;
    setting.setValue("ONLINE-timer",m_timer);
    setting.setValue("ONLINE-timerInc",m_timeInc);
    setting.setValue("ONLINE-billElo",m_billElo);
    setting.setValue("ONLINE-color",m_color);
    setting.setValue("ONLINE-variant",m_variant);
    setting.setValue("ONLINE-nickname",m_nickname);
    RobochessApplication::instance()->lichess->global_time_limit = convertTimer();
    RobochessApplication::instance()->lichess->global_time_inc = convertTimeInc();
}


void NewgameOnlineFriendDlg::on_btn_playerColor_toggled(bool checked)
{
    m_color = checked;
    if(checked){
        ui->btn_playerColor->setText("play as Black");
    }else{
        ui->btn_playerColor->setText("play as White");
    }
}

void NewgameOnlineFriendDlg::on_btn_billElo_toggled(bool checked)
{
    m_billElo = checked;
    if(checked){
        ui->btn_billElo->setText("bill ELO");
    }else{
        ui->btn_billElo->setText("no Bill ELO");
    }
}

void NewgameOnlineFriendDlg::on_btn_inputuser_clicked()
{
    vtKeyboard->editing = vtKeyboard->INPUT_FRIEND;
    vtKeyboard->setData(m_nickname);
    vtKeyboard->exec();
}

void NewgameOnlineFriendDlg::on_btn_friend_toggled(bool checked)
{
    if(checked){
        ui->btn_friend->setText("Mời Bạn bè");
        ui->btn_inputuser->setVisible(true);
    }else{
        ui->btn_friend->setText("Mời Thế giới");
        ui->btn_inputuser->setVisible(false);
    }
}
