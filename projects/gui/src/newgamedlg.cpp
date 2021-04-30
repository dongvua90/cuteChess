/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "newgamedlg.h"
#include "ui_newgamedlg.h"

#include <QAbstractItemView>
#include <QSettings>

#include <board/boardfactory.h>
#include <chessgame.h>
#include <enginebuilder.h>
#include <humanbuilder.h>
#include <enginemanager.h>
#include <openingsuite.h>

#include "cutechessapp.h"
#include "timecontroldlg.h"
#include "stringvalidator.h"
#include <timecontrol.h>
#include <QSettings>


NewGameDialog::NewGameDialog(QWidget* parent)
	: QDialog(parent),
      ui(new Ui::NewGameDialog)
{
	ui->setupUi(this);


    ui->comboBox_variant->addItem("standard");
    ui->comboBox_variant->addItem("atomic");
    ui->comboBox_variant->addItem("kingofthehill");
    ui->comboBox_variant->addItem("crazyhouse");

    readSetting();
    ui->comboBox_variant->setCurrentIndex(m_variant);
    setTimer(m_timer);
    setTimeInc(m_timeInc);
    ui->slider_timeInc->setValue(m_timeInc);
    ui->slider_timer->setValue(m_timer);
    ui->slider_skillLevel->setValue(m_skillLevel);
    ui->slider_uciElo->setValue(m_elo);
    ui->lb_skillLevel->setText(QString::number(m_skillLevel));
    ui->lb_ucielo->setText(QString::number(m_elo));
    ui->btn_playerColor->setChecked(m_color);
    ui->btn_limitStrength->setChecked(m_limitStrenger);
    on_btn_playerColor_toggled(m_color);

    //setvisibale Skill level & UCI
    if(ui->btn_limitStrength->isChecked())
    {
        ui->labelUCI->setVisible(false);
        ui->slider_uciElo->setVisible(false);
        ui->lb_ucielo->setVisible(false);
        ui->labelSkillLevel->setVisible(true);
        ui->slider_skillLevel->setVisible(true);
        ui->lb_skillLevel->setVisible(true);
    }else{
        ui->labelUCI->setVisible(true);
        ui->slider_uciElo->setVisible(true);
        ui->lb_ucielo->setVisible(true);
        ui->labelSkillLevel->setVisible(false);
        ui->slider_skillLevel->setVisible(false);
        ui->lb_skillLevel->setVisible(false);
    }
}

NewGameDialog::~NewGameDialog()
{
	delete ui;
}

ChessGame* NewGameDialog::createGame() const
{
    auto board = Chess::BoardFactory::create(ui->comboBox_variant->currentText());
	auto pgn = new PgnGame();
	pgn->setSite(QSettings().value("pgn/site").toString());
	auto game = new ChessGame(board, pgn);

    int timsum;
    switch (m_timer) {
        case 0: timsum=5; break;
        case 1: timsum=10; break;
        case 2: timsum=20; break;
        case 3: timsum=30; break;
        case 4: timsum=45; break;
        case 5: timsum=60; break;
        case 6: timsum=120; break;
        case 7: timsum=180; break;
        case 8: timsum=1000; break;
    }

    int tim;
    switch (m_timeInc) {
        case 0: tim=0; break;
        case 1: tim=3; break;
        case 2: tim=5; break;
        case 3: tim=10; break;
        case 4: tim=15; break;
        case 5: tim=20; break;
        case 6: tim=30; break;
        case 7: tim=45; break;
        case 8: tim=60; break;
    }
    QString stim=QString::number(timsum)+":00+"+QString::number(tim);
        game->setTimeControl(TimeControl(stim));



	return game;
}

PlayerBuilder* NewGameDialog::createPlayerBuilder(Chess::Side side) const
{
	if (playerType(side) == CPU)
	{
        EngineConfiguration config;
        config.setWorkingDirectory("/home/pika/Documents/LicheePiZero/QtProject/Stockfish/src");
        config.setCommand("./stockfish");
        config.setProtocol("uci");
        config.setOption("nodestime","1");
        config.setOption("Hash","1");
        config.setOption("Skill Level",QString::number(m_skillLevel));
        if(m_limitStrenger){
            config.setOption("UCI_LimitStrength","true");
            config.setOption("UCI_Elo",QString::number(m_elo));
        }else{
            config.setOption("UCI_LimitStrength","false");
        }

		return new EngineBuilder(config);
	}
	bool ignoreFlag = QSettings().value("games/human_can_play_after_timeout",
					   true).toBool();
	return new HumanBuilder(CuteChessApplication::userName(), ignoreFlag);
}


NewGameDialog::PlayerType NewGameDialog::playerType(Chess::Side side) const
{
	Q_ASSERT(!side.isNull());

	if (side == Chess::Side::White)
    {
        if(ui->btn_playerColor->isChecked())
            return CPU;
        else
            return Human;

    }else{
        if(ui->btn_playerColor->isChecked())
            return Human;
        else
            return CPU;
    }
}

void NewGameDialog::setTimer(int timer)
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

int NewGameDialog::convertToTimer()
{
    int tim;
    switch (m_timer) {
        case 0: tim=5; break;
        case 1: tim=10; break;
        case 2: tim=20; break;
        case 3: tim=30; break;
        case 4: tim=45; break;
        case 5: tim=60; break;
        case 6: tim=120; break;
        case 7: tim=180; break;
        case 8: tim=1000; break;
    }
    return tim;
}

int NewGameDialog::convertToTimeInc()
{
    int tim;
    switch (m_timer) {
        case 0: tim=0; break;
        case 1: tim=3; break;
        case 2: tim=5; break;
        case 3: tim=10; break;
        case 4: tim=15; break;
        case 5: tim=20; break;
        case 6: tim=30; break;
        case 7: tim=45; break;
        case 8: tim=60; break;
    }
    return tim;
}

void NewGameDialog::setTimeInc(int time)
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

void NewGameDialog::readSetting()
{
    QSettings setting;
    m_timer = setting.value("CPU-timer").toInt();
    m_timeInc = setting.value("CPU-timerInc").toInt();
    m_skillLevel = setting.value("CPU-skillLevel").toInt();
    m_elo = setting.value("CPU-elo").toInt();
    m_limitStrenger = setting.value("CPU-limitStrength").toBool();
    m_color = setting.value("CPU-color").toBool();
    m_variant = setting.value("CPU-variant").toInt();
}

void NewGameDialog::writeSetting()
{
    QSettings setting;
    setting.setValue("CPU-timer",m_timer);
    setting.setValue("CPU-timerInc",m_timeInc);
    setting.setValue("CPU-skillLevel",m_skillLevel);
    setting.setValue("CPU-elo",m_elo);
    setting.setValue("CPU-limitStrength",m_limitStrenger);
    setting.setValue("CPU-color",m_color);
    setting.setValue("CPU-variant",m_variant);
}


void NewGameDialog::on_btn_playerColor_toggled(bool checked)
{
    m_color = checked;
    if(checked){       
        ui->btn_playerColor->setText("play as Black");
    }else{
        ui->btn_playerColor->setText("play as White");
    }
}

void NewGameDialog::on_slider_timer_valueChanged(int value)
{
    m_timer = value;
    setTimer(m_timer);
}

void NewGameDialog::on_slider_timeInc_valueChanged(int value)
{
    m_timeInc = value;
    setTimeInc(m_timeInc);
}

void NewGameDialog::on_slider_skillLevel_valueChanged(int value)
{
    m_skillLevel = value;
    ui->lb_skillLevel->setText(QString::number(m_skillLevel));
}

void NewGameDialog::on_slider_uciElo_valueChanged(int value)
{
    m_elo = value;
    ui->lb_ucielo->setText(QString::number(m_elo));
}

void NewGameDialog::on_btn_limitStrength_toggled(bool checked)
{
    m_limitStrenger = checked;
    if(checked)
    {
        ui->btn_limitStrength->setText("Full Strength");
        ui->labelUCI->setVisible(false);
        ui->slider_uciElo->setVisible(false);
        ui->lb_ucielo->setVisible(false);
        ui->labelSkillLevel->setVisible(true);
        ui->slider_skillLevel->setVisible(true);
        ui->lb_skillLevel->setVisible(true);
    }else{
        ui->btn_limitStrength->setText("Limit Strength");
        ui->labelUCI->setVisible(true);
        ui->slider_uciElo->setVisible(true);
        ui->lb_ucielo->setVisible(true);
        ui->labelSkillLevel->setVisible(false);
        ui->slider_skillLevel->setVisible(false);
        ui->lb_skillLevel->setVisible(false);
    }
}

void NewGameDialog::on_btn_close_clicked()
{
    close();
}

void NewGameDialog::on_btn_ok_clicked()
{
    writeSetting();
//    finished(QDialog::Accepted);
    accept();
}

void NewGameDialog::on_comboBox_variant_currentIndexChanged(int index)
{
    m_variant = index;
    qDebug()<<"indexVariant:"<<index;
}
