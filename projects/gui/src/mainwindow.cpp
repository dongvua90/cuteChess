/*
    This file is part of Cute Chess.

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

#include "mainwindow.h"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QTreeView>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>
#include <QWindow>
#include <QSettings>
#include "boardview/boardscene.h"
#include <chessgame.h>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QDesktopWidget>
#endif
#include <QSysInfo>

#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <playerbuilder.h>
#include <chessplayer.h>
#include <humanbuilder.h>
#include <tournament.h>

#include "cutechessapp.h"
#include "gameviewer.h"
#include "movelist.h"
#include "newgamedlg.h"
#include "chessclock.h"
#include "gametabbar.h"
#include "boardview/boardscene.h"
#include <QPushButton>
#include <enginebuilder.h>

#ifdef QT_DEBUGD
#include <modeltest.h>
#endif

MainWindow::TabData::TabData(ChessGame* game, Tournament* tournament)
    : m_id(game),
      m_game(game),
      m_finished(false)
{
}

MainWindow::MainWindow(ChessGame* game)
    : m_game(nullptr),
	  m_closing(false),
	  m_readyToClose(false),
      m_firstTabAutoCloseEnabled(false)
{
    setFixedSize(480,272);
    move(0,0);
	m_gameViewer = new GameViewer(Qt::Horizontal, nullptr, true);
	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);
		m_gameViewer->chessClock(side)->setPlayerName(side.toString());
	}
    m_gameViewer->setContentsMargins(0,0,0,0);

	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(m_gameViewer);

	// The content margins look stupid when used with dock widgets
	mainLayout->setContentsMargins(0, 0, 0, 0);
//myedit
    //slot menu
    menu = m_gameViewer->getMenu();
    connect(menu,SIGNAL(onNewgame()),this,SLOT(onMenuNewgame()));
    connect(menu,SIGNAL(onResign()),this,SLOT(resignGame()));
    connect(menu,SIGNAL(onFlip()),m_gameViewer->boardScene(),SLOT(flip()));
    connect(menu,SIGNAL(onSave()),this,SLOT(save()));



//endmyedit

	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    mainWidget->setContentsMargins(0,0,0,0);

    createToolBars();

    connect(CuteChessApplication::instance()->gameManager(),
        SIGNAL(finished()), this, SLOT(onGameManagerFinished()),
        Qt::QueuedConnection);

    //edit
//    addGame(game);
    fristscreen = new FirstScreen(this);
    beginPlayFriend = new BeginPlayFriend(this);
    connect(fristscreen,SIGNAL(onCpuGame()),this,SLOT(newGame()));
    connect(fristscreen,SIGNAL(onOnlineGame()),this,SLOT(onMenuNewgameOnline()));
    connect(beginPlayFriend,&BeginPlayFriend::onPlayOnline,this,&MainWindow::newGameOnline);

    fristscreen->exec();

    robot = CuteChessApplication::instance()->getIntanceRobot();

    threadIr = CuteChessApplication::instance()->getIntanceThreadIr();
    connect(threadIr,&ThreadIR::onHumanEnter,robot,&Robot::humanEnter);

    connect(m_gameViewer->boardScene(),&BoardScene::humanMoveError,robot,&Robot::onCheckMoveIsError);
    connect(m_gameViewer->boardScene(),&BoardScene::humanMove,robot,&Robot::onCheckMoveIsOk);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createToolBars()
{
	m_tabBar = new GameTabBar();
	m_tabBar->setDocumentMode(true);
	m_tabBar->setTabsClosable(true);
	m_tabBar->setMovable(false);
	m_tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	connect(m_tabBar, SIGNAL(currentChanged(int)),
		this, SLOT(onTabChanged(int)));
	connect(m_tabBar, SIGNAL(tabCloseRequested(int)),
		this, SLOT(onTabCloseRequested(int)));


	QToolBar* toolBar = new QToolBar(tr("Game Tabs"));
	toolBar->setObjectName("GameTabs");
	toolBar->setVisible(false);
	toolBar->setFloatable(false);
	toolBar->setMovable(false);
	toolBar->setAllowedAreas(Qt::TopToolBarArea);
	toolBar->addWidget(m_tabBar);
    addToolBar(toolBar);
}

void MainWindow::addGame(ChessGame* game)
{
    m_gameViewer->mvlist->setGame(game);
    if(m_tabBar->count()>1){    // only show 2 tab .
         closeCurrentGame();
    }
    TabData tab(game);

    connect(game, SIGNAL(finished(ChessGame*)),
            this, SLOT(onGameFinished(ChessGame*)));

    //edit
    connect(game,&ChessGame::fenChanged,robot,&Robot::setFenOrigin);
    connect(robot,&Robot::onPieceError,this,&MainWindow::onTest);
    robot->setFenOrigin(game->board()->fenString());
    connect(robot,&Robot::onHumanMakeMove,m_gameViewer,&GameViewer::makemove);
    if(game->player(Chess::Side::White)->isHuman())
        robot->requestHumanMove();
    connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
        robot, SLOT(robotMakeMove(Chess::GenericMove)));
    connect(game,SIGNAL(humanEnabled(bool)),robot,SLOT(humanEnabled(bool)));


    m_tabs.append(tab);
    m_tabBar->setCurrentIndex(m_tabBar->addTab(genericTitle(tab)));

    // close initial tab if unused and if enabled by settings
    if (m_tabs.size() >= 2 &&  m_firstTabAutoCloseEnabled)
    {
        qDebug()<<"AutoClose-true";
        if (QSettings().value("ui/close_unused_initial_tab", true).toBool() &&
        !m_tabs[0].m_game.isNull()
        &&  m_tabs[0].m_game.data()->moves().isEmpty())
        {
            qDebug("Closed tab init");
            closeTab(0);
        }
        m_firstTabAutoCloseEnabled = false;
    }

    if (m_tabs.size() >= 2) //show tabview
    {
//         m_tabBar->parentWidget()->show();
    }

}

void MainWindow::removeGame(int index)
{
	Q_ASSERT(index != -1);

	m_tabs.removeAt(index);
	m_tabBar->removeTab(index);

	if (m_tabs.size() == 1)
		m_tabBar->parentWidget()->hide();
}

void MainWindow::destroyGame(ChessGame* game)
{
    qDebug()<<"destroyGame";
	Q_ASSERT(game != nullptr);

	int index = tabIndex(game);
	Q_ASSERT(index != -1);
	TabData tab = m_tabs.at(index);

	removeGame(index);

    if (tab.m_tournament == nullptr)
        game->deleteLater();
    delete tab.m_pgn;

	if (m_tabs.isEmpty())
    {
		close();
    }
}

void MainWindow::setCurrentGame(const TabData& gameData)
{
	if (gameData.m_game == m_game && m_game != nullptr)
		return;

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_players[i]);
		if (player != nullptr)
		{
			disconnect(player, nullptr,
			           m_gameViewer->chessClock(Chess::Side::White), nullptr);
			disconnect(player, nullptr,
			           m_gameViewer->chessClock(Chess::Side::Black), nullptr);
		}
	}

	if (m_game != nullptr)
	{
		m_game->pgn()->setTagReceiver(nullptr);
		m_gameViewer->disconnectGame();
//		disconnect(m_game, nullptr, m_moveList, nullptr);

		ChessGame* tmp = m_game;
		m_game = nullptr;

		// QObject::disconnect() is not atomic, so we need to flush
		// all pending events from the previous game before switching
		// to the next one.
		tmp->lockThread();
		CuteChessApplication::processEvents();
		tmp->unlockThread();

		// If the call to CuteChessApplication::processEvents() caused
		// a new game to be selected as the current game, then our
		// work here is done.
		if (m_game != nullptr)
			return;
	}

	m_game = gameData.m_game;

	lockCurrentGame();


//	m_moveList->setGame(m_game, gameData.m_pgn);

	if (m_game == nullptr)
	{
		m_gameViewer->setGame(gameData.m_pgn);

		for (int i = 0; i < 2; i++)
		{
			Chess::Side side = Chess::Side::Type(i);
			auto clock = m_gameViewer->chessClock(side);
			clock->stop();
			clock->setInfiniteTime(true);
			QString name = nameOnClock(gameData.m_pgn->playerName(side),
						   side);
			clock->setPlayerName(name);
		}

		return;
	}
	else
		m_gameViewer->setGame(m_game);

//	m_tagsModel->setTags(gameData.m_pgn->tags());
//	gameData.m_pgn->setTagReceiver(m_tagsModel);

	for (int i = 0; i < 2; i++)
	{
		Chess::Side side = Chess::Side::Type(i);
		ChessPlayer* player(m_game->player(side));
		m_players[i] = player;

		auto clock = m_gameViewer->chessClock(side);

		clock->stop();
		QString name = nameOnClock(player->name(), side);
		clock->setPlayerName(name);
		connect(player, SIGNAL(nameChanged(QString)),
			clock, SLOT(setPlayerName(QString)));

		clock->setInfiniteTime(player->timeControl()->isInfinite());

		if (player->state() == ChessPlayer::Thinking)
			clock->start(player->timeControl()->activeTimeLeft());
		else
			clock->setTime(player->timeControl()->timeLeft());

		connect(player, SIGNAL(startedThinking(int)),
			clock, SLOT(start(int)));
		connect(player, SIGNAL(stoppedThinking()),
			clock, SLOT(stop()));
        //edit

        if(player->isHuman()){
            connect(player,&ChessPlayer::startedThinking,this,&MainWindow::onHumanTurn);
        }else{
            connect(player,&ChessPlayer::startedThinking,this,&MainWindow::onEngineTurn);
        }
	}

	if (m_game->boardShouldBeFlipped())
		m_gameViewer->boardScene()->flip();

	unlockCurrentGame();
}

int MainWindow::tabIndex(ChessGame* game) const
{
	Q_ASSERT(game != nullptr);

	for (int i = 0; i < m_tabs.size(); i++)
	{
		if (m_tabs.at(i).m_id == game)
			return i;
	}

	return -1;
}

int MainWindow::tabIndex(Tournament* tournament, bool freeTab) const
{
    Q_ASSERT(tournament != nullptr);

    for (int i = 0; i < m_tabs.size(); i++)
    {
        const TabData& tab = m_tabs.at(i);

        if (tab.m_tournament == tournament
        &&  (!freeTab || (tab.m_game == nullptr || tab.m_finished)))
            return i;
    }

    return -1;
}

void MainWindow::onTabChanged(int index)
{
    qDebug()<<"onTabChanged:"<<index;

	if (index == -1 || m_closing)
		m_game = nullptr;
    else{
        qDebug()<<"setCurentGame"<<index;
        if(index==0){
             setCurrentGame(m_tabs.at(index));
        }else{
          setCurrentGame(m_tabs.at(index));
        }
    }
}

void MainWindow::onTabCloseRequested(int index)
{
    const TabData& tab = m_tabs.at(index);

    if (tab.m_tournament && tab.m_game)
    {
        auto btn = QMessageBox::question(this, tr("End tournament game"),
               tr("Do you really want to end the active tournament game?"));
        if (btn != QMessageBox::Yes)
            return;
    }

    closeTab(index);
}

void MainWindow::closeTab(int index)
{
	const TabData& tab = m_tabs.at(index);

	if (tab.m_game == nullptr)
    {
		delete tab.m_pgn;
		removeGame(index);

		if (m_tabs.isEmpty())
        {
			close();
        }
		return;
	}

	if (tab.m_finished)
		destroyGame(tab.m_game);
	else
	{
		connect(tab.m_game, SIGNAL(finished(ChessGame*)),
			this, SLOT(destroyGame(ChessGame*)));
		QMetaObject::invokeMethod(tab.m_game, "stop", Qt::QueuedConnection);
	}
}

void MainWindow::closeCurrentGame()
{
    closeTab(m_tabBar->currentIndex());
}

void MainWindow::onTest(uint8_t board[])
{
    m_gameViewer->boardScene()->setPieceError(board);
}

void MainWindow::onTest2(uint8_t qFrom,uint8_t qTo,enum Robot::MOVETYPE move_type)
{
    qDebug()<<"onTest2:"<<qFrom<<" to "<<qTo<<" type:"<<move_type;
    int m_file,m_rank;
    m_file = qFrom%8;
    m_rank = (63-qFrom)/8;
    //    GraphicsPiece *piece = pieceAt(m_gameViewer->boardScene()->squarePos(Chess::Square(m_file,m_rank)));
}

void MainWindow::onTest3()
{
    qDebug()<<"onTeast3";
}

void MainWindow::onHumanTurn(int timeleft)
{
    qDebug()<<"onHumanTurn:"<<timeleft;
    robot->setFenOrigin(m_gameViewer->board()->fenString());
    robot->requestHumanMove();

}

void MainWindow::onEngineTurn(int timeleft)
{
    qDebug()<<"onEngineTurn:"<<timeleft;
}

void MainWindow::newGame()
{
//    EngineManager* engineManager = CuteChessApplication::instance()->engineManager();
    NewGameDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted){
         fristscreen->setVisible(true);
        return;
    }
    fristscreen->setVisible(false);
    auto game = dlg.createGame();
    if (!game)
    {
        QMessageBox::critical(this, tr("Could not initialize game"),
                      tr("The game could not be initialized "
                     "due to an invalid opening."));
        return;
    }

    PlayerBuilder* builders[2] = {
        dlg.createPlayerBuilder(Chess::Side::White),
        dlg.createPlayerBuilder(Chess::Side::Black)
    };

    if (builders[game->board()->sideToMove()]->isHuman())
        game->pause();

    // Start the game in a new tab
    connect(game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGame(ChessGame*)));
    connect(game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));
    CuteChessApplication::instance()->gameManager()->newGame(game,
        builders[Chess::Side::White], builders[Chess::Side::Black]);
}

void MainWindow::onGameStartFailed(ChessGame* game)
{
	QMessageBox::critical(this, tr("Game Error"), game->errorString());
}

void MainWindow::onGameFinished(ChessGame* game)
{
	int tIndex = tabIndex(game);
	if (tIndex == -1)
		return;

	auto& tab = m_tabs[tIndex];
	tab.m_finished = true;
	QString title = genericTitle(tab);
	m_tabBar->setTabText(tIndex, title);
	if (game == m_game)
	{
		// Finished tournament games are destroyed immediately
		// so we can't touch the game object any more.
        if (tab.m_tournament)
            m_game = nullptr;
	}

	// save game notation of non-tournament games to default PGN file
    if (!tab.m_tournament
    &&  !game->pgn()->isNull()
    &&  	(  !game->pgn()->moves().isEmpty()   // ignore empty games
        || !game->pgn()->result().isNone())) // without adjudication
    {
        QString fileName = QSettings().value("games/default_pgn_output_file", QString())
                          .toString();

        if (!fileName.isEmpty())
            game->pgn()->write(fileName);
            //TODO: reaction on error
    }
}

QString MainWindow::genericTitle(const TabData& gameData) const
{
	QString white;
	QString black;
	Chess::Result result;
	if (gameData.m_game)
	{
		white = gameData.m_game->player(Chess::Side::White)->name();
		black = gameData.m_game->player(Chess::Side::Black)->name();
		result = gameData.m_game->result();
	}
	else
	{
		white = gameData.m_pgn->playerName(Chess::Side::White);
		black = gameData.m_pgn->playerName(Chess::Side::Black);
		result = gameData.m_pgn->result();
	}

	if (result.isNone())
		return tr("%1 vs %2").arg(white, black);
	else
		return tr("%1 vs %2 (%3)")
		       .arg(white, black, result.toShortString());
}

QString MainWindow::nameOnClock(const QString& name, Chess::Side side) const
{
	QString text = name;
	bool displaySide = QSettings().value("ui/display_players_sides_on_clocks", false)
				      .toBool();
	if (displaySide)
		text.append(QString(" (%1)").arg(side.toString()));
	return text;
}

void MainWindow::lockCurrentGame()
{
	if (m_game != nullptr)
		m_game->lockThread();
}

void MainWindow::unlockCurrentGame()
{
	if (m_game != nullptr)
		m_game->unlockThread();
}

bool MainWindow::save()
{
	if (m_currentFile.isEmpty())
		return saveAs();

	return saveGame(m_currentFile);
}

bool MainWindow::saveAs()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Game"),
        QString(),
        tr("Portable Game Notation (*.pgn);;All Files (*.*)"),
        nullptr,
        QFileDialog::DontConfirmOverwrite);
    if (fileName.isEmpty())
        return false;

    return saveGame(fileName);
}

bool MainWindow::saveGame(const QString& fileName)
{
	lockCurrentGame();
	bool ok = m_tabs.at(m_tabBar->currentIndex()).m_pgn->write(fileName);
	unlockCurrentGame();

	if (!ok)
		return false;

	m_currentFile = fileName;
	setWindowModified(false);

	return true;
}

void MainWindow::onGameManagerFinished()
{
	m_readyToClose = true;
	close();
}

void MainWindow::closeAllGames()
{
	auto app = CuteChessApplication::instance();

	for (int i = m_tabs.size() - 1; i >= 0; i--)
		closeTab(i);

	if (m_tabs.isEmpty())
		app->gameManager()->finish();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (m_readyToClose)
	{
		QMainWindow::closeEvent(event);
		return;
	}

//	if (askToSave())
//	{
		m_closing = true;

//        if (m_stopTournamentAct->isEnabled())
//            m_stopTournamentAct->trigger();
//        else
            closeAllGames();
//	}

	event->ignore();
}


void MainWindow::resignGame()
{
	if (m_game.isNull() || m_game->isFinished())
		return;

	ChessPlayer * player = m_game->playerToMove();
	if (!player->isHuman())
	{
		player = m_game->playerToWait();
		if (!player->isHuman())
			return;
	}
	Chess::Side side = player->side();
	auto result = Chess::Result(Chess::Result::Resignation,
				    side.opposite());
	QMetaObject::invokeMethod(m_game, "onResignation",
				  Qt::QueuedConnection,
                              Q_ARG(Chess::Result, result));
}

void MainWindow::myClose()
{
    auto focusWindow = CuteChessApplication::activeWindow();
    if (!focusWindow)
        return;

    auto focusMainWindow = qobject_cast<MainWindow*>(focusWindow);
    if (focusMainWindow)
    {
        focusMainWindow->closeCurrentGame();
        return;
    }
    focusWindow->close();
}

void MainWindow::onMenuNewgame()
{
    fristscreen->setVisible(true);
}

void MainWindow::onMenuNewgameOnline()
{
    beginPlayFriend->show();
}

void MainWindow::newGameOnline(int timer,int timeInc,QString variant,bool color,bool billELO,QString nickname)
{
    auto board = Chess::BoardFactory::create(variant);
    auto pgn = new PgnGame();
    pgn->setSite(QSettings().value("pgn/site").toString());
    auto game = new ChessGame(board, pgn);

    int timsum;
    switch (timer) {
        case 0: timsum=5; break;
        case 1: timsum=10; break;
        case 2: timsum=20; break;
        case 3: timsum=30; break;
        case 4: timsum=45; break;
        case 5: timsum=60; break;
        case 6: timsum=120; break;
        case 7: timsum=180; break;
        case 8: timsum=1000; break;
    }

    int tim;
    switch (timeInc) {
        case 0: tim=0; break;
        case 1: tim=3; break;
        case 2: tim=5; break;
        case 3: tim=10; break;
        case 4: tim=15; break;
        case 5: tim=20; break;
        case 6: tim=30; break;
        case 7: tim=45; break;
        case 8: tim=60; break;
    }
    QString stim=QString::number(timsum)+":00+"+QString::number(tim);
        game->setTimeControl(TimeControl(stim));


    PlayerBuilder* builders[2];
    builders[0] = new HumanBuilder(CuteChessApplication::userName(), false);
    builders[1] = new HumanBuilder(nickname, false);

    if (builders[game->board()->sideToMove()]->isHuman())
        game->pause();

    // Start the game in a new tab
    connect(game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGame(ChessGame*)));
    connect(game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));
    CuteChessApplication::instance()->gameManager()->newGame(game,
        builders[Chess::Side::White], builders[Chess::Side::Black]);
}
