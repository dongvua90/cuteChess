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

#include "robochessapp.h"

#include <QCoreApplication>
#include <QTime>
#include <QSettings>

#include <mersenne.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <humanbuilder.h>
#include "mainwindow.h"


RobochessApplication::RobochessApplication(int& argc, char* argv[])
	: QApplication(argc, argv),
      m_gameManager(nullptr)
{
	Mersenne::initialize(QTime(0,0,0).msecsTo(QTime::currentTime()));
    robot = new Robot();
    mythread = new ThreadIR();
    mythread->start(QThread::LowPriority);
    vtKeyboard = new VirtualKeyboard();
    lichess = new Lichess();
    mainWindow = new MainWindow();




    mainWindow->show();

}

RobochessApplication::~RobochessApplication()
{

}

RobochessApplication* RobochessApplication::instance()
{
    return static_cast<RobochessApplication*>(QApplication::instance());
}

GameManager* RobochessApplication::gameManager()
{
	if (m_gameManager == nullptr)
	{
		m_gameManager = new GameManager(this);
		int concurrency = QSettings()
			.value("tournament/concurrency", 1).toInt();
		m_gameManager->setConcurrency(concurrency);
	}

	return m_gameManager;
}
