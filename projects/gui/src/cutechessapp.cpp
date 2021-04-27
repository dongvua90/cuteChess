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

#include "cutechessapp.h"

#include <QCoreApplication>
//#include <QDir>
#include <QTime>
//#include <QFileInfo>
#include <QSettings>

#include <mersenne.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <humanbuilder.h>
#include "mainwindow.h"
#ifndef Q_OS_WIN32
#	include <sys/types.h>
#	include <pwd.h>
#endif


CuteChessApplication::CuteChessApplication(int& argc, char* argv[])
	: QApplication(argc, argv),
	  m_gameManager(nullptr),
	  m_initialWindowCreated(false)
{
	Mersenne::initialize(QTime(0,0,0).msecsTo(QTime::currentTime()));

//	setQuitOnLastWindowClosed(false);
}

CuteChessApplication::~CuteChessApplication()
{

}

CuteChessApplication* CuteChessApplication::instance()
{
	return static_cast<CuteChessApplication*>(QApplication::instance());
}

QString CuteChessApplication::userName()
{
    return "pika";
}

GameManager* CuteChessApplication::gameManager()
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

MainWindow* CuteChessApplication::newGameWindow(ChessGame* game)
{
	MainWindow* mainWindow = new MainWindow(game);
	mainWindow->show();
    return mainWindow;
}

void CuteChessApplication::newDefaultGame()
{
	// default game is a human versus human game using standard variant and
	// infinite time control
	ChessGame* game = new ChessGame(Chess::BoardFactory::create("standard"),
		new PgnGame());

	game->setTimeControl(TimeControl("inf"));
	game->pause();

	connect(game, SIGNAL(started(ChessGame*)),
		this, SLOT(newGameWindow(ChessGame*)));

	gameManager()->newGame(game,
			       new HumanBuilder(userName()),
			       new HumanBuilder(userName()));
}

void CuteChessApplication::onQuitAction()
{
	closeAllWindows();
}

void CuteChessApplication::onLastWindowClosed()
{
	if (!m_initialWindowCreated)
		return;

	if (m_gameManager != nullptr)
	{
		connect(m_gameManager, SIGNAL(finished()), this, SLOT(quit()));
		m_gameManager->finish();
	}
	else
		quit();
}
