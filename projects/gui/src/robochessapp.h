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

class RobochessApplication : public QApplication
{
	Q_OBJECT

	public:
        RobochessApplication(int& argc, char* argv[]);
        virtual ~RobochessApplication();

		GameManager* gameManager();
        static RobochessApplication* instance(); // dùng để truy cập chính class này
        Robot *robot;               // class để thao tác với phần cứng robot
        ThreadIR *mythread;         // class tạo 1 thread mới để lấy tín hiệu IR_remote
        VirtualKeyboard *vtKeyboard;// class tạo bàn phím ảo
        Lichess *lichess;           // class các chức năng của lichess
        MainWindow* mainWindow;     // class giao diện chính




        int info_wifi,info_battery=2;
        bool info_battery_charger=false;
        bool info_arm_move_finish=false;
        bool info_sound = false;
	private:
        GameManager* m_gameManager; // class quản lý game
};

#endif // CUTE_CHESS_APPLICATION_H
