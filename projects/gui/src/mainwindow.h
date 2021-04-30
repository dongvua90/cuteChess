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

	public slots:
		void addGame(ChessGame* game);

	protected:
		virtual void closeEvent(QCloseEvent* event);
		void closeCurrentGame();

	private slots:
		void newGame();
		bool save();
        bool saveAs();
		void onTabChanged(int index);
		void onTabCloseRequested(int index);
		void closeTab(int index);
		void destroyGame(ChessGame* game);
		void onGameManagerFinished();
		void onGameStartFailed(ChessGame* game);
		void onGameFinished(ChessGame* game);
		void closeAllGames();
		void resignGame();
        void myClose();
        void onMenuNewgame();
        void onMenuNewgameOnline();
        void newGameOnline(int timer,int timeInc,QString variant,bool color,bool billELO,QString nickname);

	private:
		struct TabData
		{
            explicit TabData(ChessGame* m_game,
                     Tournament* m_tournament = nullptr);

			ChessGame* m_id;
			QPointer<ChessGame> m_game;
			PgnGame* m_pgn;
            Tournament* m_tournament;
			bool m_finished;
		};
		void createToolBars();
		QString genericTitle(const TabData& gameData) const;
		QString nameOnClock(const QString& name, Chess::Side side) const;
		void lockCurrentGame();
		void unlockCurrentGame();
		bool saveGame(const QString& fileName);
		void setCurrentGame(const TabData& gameData);
		void removeGame(int index);
		int tabIndex(ChessGame* game) const;
        int tabIndex(Tournament* tournament, bool freeTab = false) const;

		QMenu* m_gameMenu;
		QMenu* m_windowMenu;

		GameTabBar* m_tabBar;

		GameViewer* m_gameViewer;
		MoveList* m_moveList;

		QPointer<ChessGame> m_game;
		QPointer<ChessPlayer> m_players[2];
		QList<TabData> m_tabs;

		QString m_currentFile;
		bool m_closing;
		bool m_readyToClose;

		bool m_firstTabAutoCloseEnabled;

        bool isNewGame=false;

        MenuScreen *menu;
        FirstScreen *fristscreen;
        BeginPlayFriend *beginPlayFriend;
};

#endif // MAINWINDOW_H
