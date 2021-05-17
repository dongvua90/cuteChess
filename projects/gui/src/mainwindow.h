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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <board/side.h>
#include "menuscreen.h"
#include "firstscreen.h"
#include "beginplayfriend.h"
#include "robot.h"
#include "threadir.h"
#include "board/genericmove.h"
#include "beginplaycomputer.h"
#include "lichessdata.h"

namespace Chess {
	class Board;
	class Move;
}
class QCloseEvent;
class QTabBar;
class GameViewer;
class MoveList;
class PgnGame;
class ChessGame;
class ChessPlayer;
class Tournament;
class GameTabBar;
/**
 * MainWindow
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(ChessGame* game);
		virtual ~MainWindow();
    Robot *robot;
    Robot* getRobot(); // trả lại đối tượng robot, vì chỉ được phép khởi tạo 1 đối tượng robot

	public slots:
		void addGame(ChessGame* game);
        void newLichessGameComputer(LichessData dat);
	private slots:
        void onTest2(uint8_t qFrom,uint8_t qTo,enum Robot::MOVETYPE move_type);
        void onTest3(QString move);
        void onChallengeAi();
        void onPieceError(uint8_t board[]);
        void onHumanTurn(int timeleft);
        void onEngineTurn(int timeleft);
		void newGame();
		void onGameManagerFinished();
		void onGameStartFailed(ChessGame* game);
		void resignGame();
        void drawGame();
        void onMenuNewgame();
        void onMenuNewgameOnline();
        void onMenuNewgameComputer();
        void newGameOnline(int timer,int timeInc,QString variant,bool color,bool billELO,QString nickname);

	private:
        void setCurrentGame();
        QString nameOnClock(const QString& name, Chess::Side side);
		GameViewer* m_gameViewer;
		MoveList* m_moveList;
		QPointer<ChessPlayer> m_players[2];

		QString m_currentFile;

        bool isNewGame=false;

        MenuScreen *menu;
        FirstScreen *fristscreen;
        BeginPlayFriend *beginPlayFriend;
        BeginPlayComputer *beginPlayComputer;

        ThreadIR *threadIr;
        ChessGame *main_game;

        QString lichessId;
};

#endif // MAINWINDOW_H
