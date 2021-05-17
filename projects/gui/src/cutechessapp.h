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

#ifndef CUTE_CHESS_APPLICATION_H
#define CUTE_CHESS_APPLICATION_H

#include <QApplication>
#include <QPointer>
#include "robot.h"
#include "threadir.h"
#include "virtualkeyboard.h"
#include "lichess.h"

class EngineManager;
class GameManager;
class MainWindow;
class ChessGame;

class CuteChessApplication : public QApplication
{
	Q_OBJECT

	public:
		CuteChessApplication(int& argc, char* argv[]);
		virtual ~CuteChessApplication();
		GameManager* gameManager();

		static CuteChessApplication* instance();
		static QString userName();

        Robot *robot;
        Robot* getIntanceRobot();
        ThreadIR *mythread;
        ThreadIR* getIntanceThreadIr();
        VirtualKeyboard *vtKeyboard;
        Lichess *lichess;
	public slots:
		MainWindow* newGameWindow(ChessGame* game);
		void newDefaultGame();
		void onQuitAction();

	private:
		GameManager* m_gameManager;
		bool m_initialWindowCreated;


	private slots:
		void onLastWindowClosed();
};

#endif // CUTE_CHESS_APPLICATION_H
