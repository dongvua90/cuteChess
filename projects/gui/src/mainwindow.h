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

namespace Chess {
	class Board;
	class Move;
}
class QMenu;
class QAction;
class QCloseEvent;
class QTabBar;
class GameViewer;
class MoveList;
class PlainTextLog;
class PgnGame;
class ChessGame;
class ChessPlayer;
class PgnTagsModel;
class Tournament;
class GameTabBar;
class EvalHistory;
class EvalWidget;

/**
 * MainWindow
*/
class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(ChessGame* game);
		virtual ~MainWindow();
		QString windowListTitle() const;

	public slots:
		void addGame(ChessGame* game);

	protected:
		virtual void closeEvent(QCloseEvent* event);
		void closeCurrentGame();

	private slots:
		void newGame();
		void showGameWindow();
		void updateWindowTitle();
		void updateMenus();
		bool save();
		bool saveAs();
		void onTabChanged(int index);
		void onTabCloseRequested(int index);
		void closeTab(int index);
		void destroyGame(ChessGame* game);
		void onGameManagerFinished();
		void onGameStartFailed(ChessGame* game);
		void onGameFinished(ChessGame* game);
		void editMoveComment(int ply, const QString& comment);
		void copyFen();
		void pasteFen();
		void copyPgn();
		void closeAllGames();
		void resignGame();

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

		void createActions();
		void createMenus();
		void createToolBars();
		void createDockWindows();
		void readSettings();
		void writeSettings();
		QString genericTitle(const TabData& gameData) const;
		QString nameOnClock(const QString& name, Chess::Side side) const;
		void lockCurrentGame();
		void unlockCurrentGame();
		bool saveGame(const QString& fileName);
		bool askToSave();
		void setCurrentGame(const TabData& gameData);
		void removeGame(int index);
		int tabIndex(ChessGame* game) const;
        int tabIndex(Tournament* tournament, bool freeTab = false) const;
//		void addDefaultWindowMenu();

		QMenu* m_gameMenu;
		QMenu* m_toolsMenu;
		QMenu* m_viewMenu;
		QMenu* m_windowMenu;

		GameTabBar* m_tabBar;

		GameViewer* m_gameViewer;
		MoveList* m_moveList;
		PgnTagsModel* m_tagsModel;

		QAction* m_quitGameAct;
		QAction* m_newGameAct;
		QAction* m_resignGameAct;
		QAction* m_closeGameAct;
		QAction* m_saveGameAct;
		QAction* m_saveGameAsAct;
		QAction* m_copyFenAct;
		QAction* m_pasteFenAct;
		QAction* m_copyPgnAct;
		QAction* m_flipBoardAct;

		QAction* m_minimizeAct;
//		QAction* m_showGameDatabaseWindowAct;
//		QAction* m_showGameWallAct;
//		QAction* m_showPreviousTabAct;
//		QAction* m_showNextTabAct;
        QAction* m_showSettingsAct;

		PlainTextLog* m_engineDebugLog;


		QPointer<ChessGame> m_game;
		QPointer<ChessPlayer> m_players[2];
		QList<TabData> m_tabs;

		QString m_currentFile;
		bool m_closing;
		bool m_readyToClose;

		bool m_firstTabAutoCloseEnabled;
};

#endif // MAINWINDOW_H
