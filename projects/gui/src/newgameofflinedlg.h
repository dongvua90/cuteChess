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

#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>
#include <board/side.h>
#include <engineconfiguration.h>

class ChessGame;
class PlayerBuilder;
//class EngineConfigurationModel;
//class EngineConfigurationProxyModel;
class EngineManager;
//myadd
class TimeControl;
class GameAdjudicator;

namespace Ui {
	class NewGameDialog;
}

/*!
 * \brief The NewGameDialog class provides a dialog for creating a new game.
 */
class NewGameOfflineDialog : public QDialog
{
	Q_OBJECT

	public:
		/*! Player's type. */
		enum PlayerType
		{
			/*! Human player. */
			Human,
			/*! Computer controlled player. */
			CPU
		};

		/*!
		 * Creates a "New Game" dialog with \a engineManager as the
		 * source of engine configurations.
		 */
        NewGameOfflineDialog(QWidget* parent = nullptr);
		/*! Destroys the dialog. */
        virtual ~NewGameOfflineDialog();

		/*! Creates and returns the ChessGame object. */
		ChessGame* createGame() const;
		/*! Creates and returns the PlayerBuilder for \a side. */
		PlayerBuilder* createPlayerBuilder(Chess::Side side) const;
        bool humanIsWhite();
	private slots:

        void on_btn_playerColor_toggled(bool checked);

        void on_slider_timer_valueChanged(int value);

        void on_slider_timeInc_valueChanged(int value);

        void on_slider_skillLevel_valueChanged(int value);

        void on_btn_limitStrength_toggled(bool checked);

        void on_btn_close_clicked();

        void on_btn_ok_clicked();

        void on_comboBox_variant_currentIndexChanged(int index);

private:
		PlayerType playerType(Chess::Side side) const;
		Ui::NewGameDialog* ui;
        int m_timer;
        int m_timeInc;
        int m_skillLevel,m_elo;
        bool m_limitStrenger;
        bool m_color;
        int m_variant;
        void setTimer(int timer);
        int  convertToTimer();
        int  convertToTimeInc();
        void setTimeInc(int time);
        void readSetting();
        void writeSetting();
};

#endif // NEWGAMEDIALOG_H
