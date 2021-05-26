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
#include "newgameonlinefrienddlg.h"
#include "robot.h"
#include "threadir.h"
#include "board/genericmove.h"
#include "newgameonlinecomputerdialog.h"
#include "lichessdata.h"
#include "newgameofflinedlg.h"
#include "showdialog.h"
#include "challengedialog.h"
#include "settings.h"
#include "robotdebug.h"

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
        explicit MainWindow();
		virtual ~MainWindow();
    Robot *robot;
    Robot* getRobot(); // trả lại đối tượng robot, vì chỉ được phép khởi tạo 1 đối tượng robot
    enum TypeGame{TYPEGAME_OFFLINE=0,TYPEGAME_COMPUTER,TYPEGAME_FRIEND,TYPEGAME_ONLINE};
    enum TypeGame typegame;
public: signals:
    void armMove(QString move);
    void requestMove();
    void humanMoveError();
private: signals:
    void humanTurn();
private slots:
    //test
        void onTestHumanTurn();

        void onMakeMove(QString move);
        void onHumanTurn(int timeleft);
        // New Game Offline
        void newGameOffline();             // tạo 1 game mới đấu với engine nội
        void addGameOffline(ChessGame* game);
        void movesOfflineMake(Chess::GenericMove mov);
        void onGameOfflineFinished(ChessGame* game,
                                   Chess::Result result);
        //New Game Online
        void newGameOnlineAi();
        void newGameOnlineFriend();
        void addGameOnline(QString Rival_name,bool revalIsComputer,QString Player_name,bool flip,QString moves);
        void addGameToViewer(ChessGame* game);
        void movesOnlineMake(QString mov);
        void onGameOnlineFinish(QString idGame,bool is_white_winner,bool isDraw,QString status);
        void oldGameReturn();       // hàm gọi lại của timer để khôi phục game cũ
        void abortGameOnline(bool success);
        void resignGameOnline(bool success);

        // Report Error
        void onHumanMoveError(QString err);
		void onGameStartFailed(ChessGame* game);
        void onChallengeCancel();
        void onChallenger(QString rival_name,QString info);

        void onSettingDlg();

        // Slot of Menu
        void onMenuResignGame();
        void onMenuDrawGame();
        void onMenuAbortGame();
        void onMenuNewgame();

	private:
        void setCurrentGame();
        QString nameOnClock(const QString& name, Chess::Side side);
        QString convertMoveToString(Chess::GenericMove mov);
		GameViewer* m_gameViewer;
		MoveList* m_moveList;
		QPointer<ChessPlayer> m_players[2];

        MenuScreen *menu;
        FirstScreen *fristscreen;



        ThreadIR *threadIr;
        ChessGame *main_game;
        //test
        RobotDebug *robotdebug;

        QString lichessId;

        bool humanIsWhite;
        int moves_length=0;
        // dialog
        NewGameOfflineDialog        *dlg_newGameOffLine;
        NewGameOnlineComputerDialog *dlg_newGameOnlineComputer;
        NewgameOnlineFriendDlg      *dlg_newgameOnlineFriend;
        QDialog                     *dlg_challenge_cancel;
        ChallengeDialog             *dlg_challenge;
        Settings                    *dlg_settings;
        ShowDialog *sdialog;

        // các biến dành cho việc khôi phục game cũ
        QTimer *timer_oldGameReturn;
        QStringList moves_returnGame;
        int num_returnGame=0;
};

#endif // MAINWINDOW_H
