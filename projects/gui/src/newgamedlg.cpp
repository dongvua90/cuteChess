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
//#include "engineconfigurationmodel.h"
//#include "engineconfigproxymodel.h"
//#include "engineconfigurationdlg.h"
#include "timecontroldlg.h"
#include "stringvalidator.h"
#include <timecontrol.h>


NewGameDialog::NewGameDialog(QWidget* parent)
	: QDialog(parent),
      ui(new Ui::NewGameDialog)
    ,m_timer(5)
    ,m_timeInc(0)
{
	ui->setupUi(this);
    ui->comboBox_variant->addItem("standard");
    ui->comboBox_variant->addItem("atomic");
    ui->comboBox_variant->addItem("kingofthehill");
    ui->comboBox_variant->addItem("crazyhouse");
    ui->slider_timer->setValue(m_timer);
    ui->lb_timer->setText(QString::number(m_timer));
    ui->slider_timeInc->setValue(m_timeInc);
    ui->lb_timeInc->setText(QString::number(m_timeInc));
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
    QString stim=QString::number(m_timer)+":00+"+QString::number(m_timeInc);
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
            config.setOption("Limit Strenger","false");
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


void NewGameDialog::on_btn_playerColor_toggled(bool checked)
{
    if(checked){
        ui->btn_playerColor->setText("play as Black");
    }else{
        ui->btn_playerColor->setText("play as White");
    }
}

void NewGameDialog::on_slider_timer_valueChanged(int value)
{
    m_timer = value;
    ui->lb_timer->setText(QString::number(m_timer)+" min");
}

void NewGameDialog::on_slider_timeInc_valueChanged(int value)
{
    m_timeInc = value;
    ui->lb_timeInc->setText(QString::number(m_timeInc)+" s");
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

void NewGameDialog::on_checkBox_limitStreng_toggled(bool checked)
{
    m_limitStrenger = checked;
}
