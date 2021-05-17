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
#include "board/result.h"

#ifdef QT_DEBUGD
#include <modeltest.h>
#endif

MainWindow::MainWindow(ChessGame* game)
{
    setFixedSize(580,272);
    move(0,0);
    main_game = game;
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
    connect(menu,SIGNAL(onDraw()),this,SLOT(drawGame()));
    connect(menu,SIGNAL(onFlip()),m_gameViewer->boardScene(),SLOT(flip()));




//endmyedit

	QWidget* mainWidget = new QWidget(this);
	mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    mainWidget->setContentsMargins(0,0,0,0);

    connect(CuteChessApplication::instance()->gameManager(),
        SIGNAL(finished()), this, SLOT(onGameManagerFinished()),
        Qt::QueuedConnection);

    //edit
    fristscreen = new FirstScreen(this);
    beginPlayFriend = new BeginPlayFriend(this);
    beginPlayComputer = new BeginPlayComputer(this);
    connect(fristscreen,SIGNAL(onCpuGame()),this,SLOT(newLichessGameComputer()));
    connect(fristscreen,SIGNAL(onOnlineGame()),this,SLOT(onMenuNewgameOnline()));
    connect(fristscreen,SIGNAL(onGameonlineComputer()),this,SLOT(onMenuNewgameComputer()));
    connect(beginPlayFriend,&BeginPlayFriend::onPlayOnline,this,&MainWindow::newGameOnline);
    connect(beginPlayComputer,&BeginPlayComputer::challengeAI,
            CuteChessApplication::instance()->lichess,&Lichess::ChallengeTheAI);
    connect(CuteChessApplication::instance()->lichess,&Lichess::stateGameNewGame,this,&MainWindow::newLichessGameComputer);

    fristscreen->exec();

    robot = CuteChessApplication::instance()->getIntanceRobot();

    threadIr = CuteChessApplication::instance()->getIntanceThreadIr();
    connect(threadIr,&ThreadIR::onHumanEnter,robot,&Robot::humanEnter);

    connect(m_gameViewer->boardScene(),&BoardScene::humanMoveError,robot,&Robot::onCheckMoveIsError);
    connect(m_gameViewer->boardScene(),&BoardScene::humanMove,robot,&Robot::onCheckMoveIsOk);

    //debug
    connect(m_gameViewer,&GameViewer::debugMakeMove,this,&MainWindow::onTest3);
}

MainWindow::~MainWindow()
{
}

void MainWindow::addGame(ChessGame* game)
{
    m_gameViewer->mvlist->setGame(game);
    m_gameViewer->setGame(game);

    connect(game, SIGNAL(finished(ChessGame*)),
            this, SLOT(onGameFinished(ChessGame*)));
    //lichess
    connect(CuteChessApplication::instance()->lichess,&Lichess::stateGameMoves,m_gameViewer,&GameViewer::makemove2);

    connect(m_gameViewer->boardScene(),&BoardScene::humanMakeMove,
            CuteChessApplication::instance()->lichess,&Lichess::makeMove);
    //edit
//    connect(game,&ChessGame::fenChanged,robot,&Robot::setFenOrigin);
//    connect(robot,&Robot::onPieceError,this,&MainWindow::onPieceError);
//    robot->setFenOrigin(game->board()->fenString());
//    connect(robot,&Robot::onHumanMakeMove,m_gameViewer,&GameViewer::makemove);
//    if(game->player(Chess::Side::White)->isHuman())
//        robot->requestHumanMove();
//    connect(game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
//        robot, SLOT(robotMakeMove(Chess::GenericMove)));
//    connect(game,SIGNAL(humanEnabled(bool)),robot,SLOT(humanEnabled(bool)));
//    Chess::Result ree(Chess::Result::Draw);
//    game->onResignation(ree);

    setCurrentGame();

}

void MainWindow::newLichessGameComputer(LichessData dat)
{
    if(lichessId.compare(dat.id) == 0)
        return;
    else
        lichessId = dat.id;
    qDebug()<<"newLichessGameCompter:"<<lichessId;
    auto board = Chess::BoardFactory::create(dat.variant);
    auto pgn = new PgnGame();
    pgn->setSite(QSettings().value("pgn/site").toString());
    main_game = new ChessGame(board, pgn);
    m_gameViewer->disconnectGame();
//    QString time_black= QString::number(dat.black_timeReamin);
    main_game->setTimeControl(TimeControl("10:10"),Chess::Side::White);
    main_game->setTimeControl(TimeControl("5:10"),Chess::Side::Black);

    PlayerBuilder* builders[2];
    builders[Chess::Side::White] = new HumanBuilder("Pika");
    builders[Chess::Side::Black] = new HumanBuilder("Lichess");


    if (builders[main_game->board()->sideToMove()]->isHuman())
        main_game->pause();

    // Start the game in a new tab
    connect(main_game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGame(ChessGame*)));
    connect(main_game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));
    CuteChessApplication::instance()->gameManager()->newGame(main_game,
        builders[Chess::Side::White], builders[Chess::Side::Black]);
}

void MainWindow::setCurrentGame()
{
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
    for (int i = 0; i < 2; i++)
    {
        Chess::Side side = Chess::Side::Type(i);
        ChessPlayer* player(main_game->player(side));
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

    if (main_game->boardShouldBeFlipped())
        m_gameViewer->boardScene()->flip();
}

QString MainWindow::nameOnClock(const QString &name, Chess::Side side)
{
    QString text = name;
    bool displaySide = QSettings().value("ui/display_players_sides_on_clocks", false)
                      .toBool();
    if (displaySide)
        text.append(QString(" (%1)").arg(side.toString()));
    return text;
}


void MainWindow::onPieceError(uint8_t board[])
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

void MainWindow::onTest3(QString move)
{
    qDebug()<<"onTeast3:"<<move;
    CuteChessApplication::instance()->lichess->makeMove(move);
}

void MainWindow::onChallengeAi()
{
    qDebug()<<"onChallengeAI";
//    CuteChessApplication::instance()->lichess->ChallengeTheAI();
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
    main_game->stop();
    main_game->kill();
    main_game = dlg.createGame();
//    auto game = dlg.createGame();
    if (!main_game)
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

    if (builders[main_game->board()->sideToMove()]->isHuman())
        main_game->pause();

    // Start the game in a new tab
    connect(main_game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGame(ChessGame*)));
    connect(main_game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));
    CuteChessApplication::instance()->gameManager()->newGame(main_game,
        builders[Chess::Side::White], builders[Chess::Side::Black]);
}

void MainWindow::onGameStartFailed(ChessGame* game)
{
	QMessageBox::critical(this, tr("Game Error"), game->errorString());
}

void MainWindow::onGameManagerFinished()
{
    qDebug("onGameManagerFinish");
    main_game->kill();
}

void MainWindow::resignGame()
{
    Chess::Result resuft(Chess::Result::Resignation);
    main_game->onResignation(resuft);
}

void MainWindow::drawGame()
{
    Chess::Result resuft(Chess::Result::Draw);
    main_game->onResignation(resuft);
}

void MainWindow::onMenuNewgame()
{
    fristscreen->setVisible(true);
}

void MainWindow::onMenuNewgameOnline()
{
    beginPlayFriend->exec();
}

void MainWindow::onMenuNewgameComputer()
{
    beginPlayComputer->exec();
}

void MainWindow::newGameOnline(int timer,int timeInc,QString variant,bool color,bool billELO,QString nickname)
{
    auto board = Chess::BoardFactory::create(variant);
    auto pgn = new PgnGame();
    pgn->setSite(QSettings().value("pgn/site").toString());
    main_game = new ChessGame(board, pgn);

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
        main_game->setTimeControl(TimeControl(stim));


    PlayerBuilder* builders[2];
    builders[0] = new HumanBuilder(CuteChessApplication::userName(), false);
    builders[1] = new HumanBuilder(nickname, false);

    if (builders[main_game->board()->sideToMove()]->isHuman())
        main_game->pause();

    // Start the game in a new tab
    connect(main_game, SIGNAL(initialized(ChessGame*)),
        this, SLOT(addGame(ChessGame*)));
    connect(main_game, SIGNAL(startFailed(ChessGame*)),
        this, SLOT(onGameStartFailed(ChessGame*)));
    CuteChessApplication::instance()->gameManager()->newGame(main_game,
        builders[Chess::Side::White], builders[Chess::Side::Black]);
}
