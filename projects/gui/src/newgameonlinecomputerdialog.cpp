#include "newgameonlinecomputerdialog.h"
#include "ui_beginGameComputer.h"
#include "robochessapp.h"
#include <board/boardfactory.h>
#include <QLineEdit>


NewGameOnlineComputerDialog::NewGameOnlineComputerDialog(QWidget *parent) : QDialog(parent)
   ,m_level(1)
   ,s_variant("standard")
   ,ui(new Ui::BeginGameonlineComputer)
{
    ui->setupUi(this);//comboBox.lineEdit()->setAlignment(Qt::AlignCenter);

    ui->comboBox_variant->addItem("standard");
    ui->comboBox_variant->addItem("crazyhouse");
    ui->comboBox_variant->addItem("chess960");
    ui->comboBox_variant->addItem("kingOfTheHill");
    ui->comboBox_variant->addItem("threeCheck");
    ui->comboBox_variant->addItem("antichess");
    ui->comboBox_variant->addItem("atomic");
    ui->comboBox_variant->addItem("horde");
    ui->comboBox_variant->addItem("racingKings");
    readSetting();
    ui->comboBox_variant->setCurrentIndex(m_variant);
    ui->slider_level->setValue(m_level);
    setTimeInc(m_timeInc);
    ui->slider_timeInc->setValue(m_timeInc);
    setTimer(m_timer);
    ui->slider_timer->setValue(m_timer);
    setLevel(m_level);
    on_btn_playerColor_toggled(m_color);
}

ChessGame* NewGameOnlineComputerDialog::createGame()
{
    auto board = Chess::BoardFactory::create(ui->comboBox_variant->currentText());
    auto pgn = new PgnGame();
    pgn->setSite(QSettings().value("pgn/site").toString());
    auto game = new ChessGame(board, pgn);

    QString stim=QString::number(convertTimer())+":00+"+QString::number(convertTimeInc());
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
//void ChallengeTheAI(int level,int time_limit,int time_inc,QString variant,bool color);
void NewGameOnlineComputerDialog::createLichessGame()
{
    RobochessApplication::instance()->lichess->ChallengeTheAI(m_level,convertTimer()*60,
       convertTimeInc(),s_variant,m_color);
}

bool NewGameOnlineComputerDialog::humanIsWhite()
{
    if(m_color)
        return false;
    return true;
}

void NewGameOnlineComputerDialog::readSetting()
{
    QSettings setting;
    m_timer = setting.value("COMPUTER-timer").toInt();
    m_timeInc = setting.value("COMPUTER-timerInc").toInt();
    m_level = setting.value("COMPUTER-level").toInt();
    m_color = setting.value("COMPUTER-color").toBool();
    m_variant = setting.value("COMPUTER-variant").toInt();
}

void NewGameOnlineComputerDialog::writeSetting()
{
    m_variant = ui->comboBox_variant->currentIndex();
    s_variant = ui->comboBox_variant->currentText();
    QSettings setting;
    setting.setValue("COMPUTER-timer",m_timer);
    setting.setValue("COMPUTER-timerInc",m_timeInc);
    setting.setValue("COMPUTER-level",m_level);
    setting.setValue("COMPUTER-color",m_color);
    setting.setValue("COMPUTER-variant",m_variant);
    RobochessApplication::instance()->lichess->global_time_limit = convertTimer();
    RobochessApplication::instance()->lichess->global_time_inc = convertTimeInc();
}

void NewGameOnlineComputerDialog::setTimer(int timer)
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

void NewGameOnlineComputerDialog::setTimeInc(int time)
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

void NewGameOnlineComputerDialog::setLevel(int value)
{
    m_level = value;
    ui->lb_level->setText(QString::number(m_level));
}

int NewGameOnlineComputerDialog::convertTimer()
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

int NewGameOnlineComputerDialog::convertTimeInc()
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

void NewGameOnlineComputerDialog::on_slider_level_valueChanged(int value)
{
   setLevel(value);
}

void NewGameOnlineComputerDialog::on_slider_timer_valueChanged(int value)
{
    m_timer = value;
    setTimer(m_timer);
}

void NewGameOnlineComputerDialog::on_slider_timeInc_valueChanged(int value)
{
    m_timeInc = value;
    setTimeInc(m_timeInc);
}

void NewGameOnlineComputerDialog::on_btn_playerColor_toggled(bool checked)
{
    m_color = checked;
    ui->btn_playerColor->setChecked(checked);
    if(checked){
        ui->btn_playerColor->setText("play as Black");
    }else{
        ui->btn_playerColor->setText("play as White");
    }
}

void NewGameOnlineComputerDialog::on_btn_ok_clicked()
{
    writeSetting();
    createLichessGame();
    accept();
}

void NewGameOnlineComputerDialog::on_btn_close_clicked()
{
    close();
}
