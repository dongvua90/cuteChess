#include "neroapp.h"

#include "mainwindow.h"
#include <mersenne.h>
#include <enginemanager.h>
#include <gamemanager.h>
#include <board/boardfactory.h>
#include <chessgame.h>
#include <timecontrol.h>
#include <humanbuilder.h>

#include "settingsdlg.h"
#include "tournamentresultsdlg.h"
#include "gamedatabasedlg.h"
#include "gamedatabasemanager.h"
#include "importprogressdlg.h"
#include "pgnimporter.h"
#include "gamewall.h"
#include <QSettings>
#include "neromainwindown.h"
#include <chessplayer.h>

NeroApp::NeroApp(int &argc, char *argv[]) : QApplication(argc,argv)
{
    ChessGame* game = new ChessGame(Chess::BoardFactory::create("standard"),
        new PgnGame());

    game->setTimeControl(TimeControl("inf"));
    game->pause();


    connect(game, SIGNAL(started(ChessGame*)),
        this, SLOT(started(ChessGame*)));
    connect(game,SIGNAL(humanEnabled()),this,SLOT(humanEnabled()));
    connect(game,SIGNAL(fenChanged(QString*)),this,SLOT(fenChanged(QString*)));
    connect(game,&ChessGame::moveMade,this,&NeroApp::moveMade);
    connect(game,&ChessGame::moveChanged,this,&NeroApp::moveChanged);
    connect(game,&ChessGame::scoreChanged,this,&NeroApp::scoreChanged);
    connect(game,&ChessGame::initialized,this,&NeroApp::initialized);
    connect(game,&ChessGame::finished,this,&NeroApp::finished);
    connect(game,&ChessGame::startFailed,this,&NeroApp::startFailed);
    connect(game,&ChessGame::playersReady,this,&NeroApp::playersReady);

    gameManager()->newGame(game,
                   new HumanBuilder("phuc"),
                       new HumanBuilder("stock"));


    NeroMainWindown *windown = new NeroMainWindown(game);
    windown->show();
}


GameManager *NeroApp::gameManager()
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

NeroApp *NeroApp::instance()
{
    return static_cast<NeroApp*>(QApplication::instance());
}

void NeroApp::humanEnabled(bool enab)
{
    qDebug()<<"humanEnable:"<<enab;
}

void NeroApp::fenChanged(const QString &fenString)
{
    qDebug()<<"frnChanged:"<<fenString;
}

void NeroApp::moveMade(const Chess::GenericMove &move, const QString &sanString, const QString &comment)
{
    qDebug()<<"makeMove:"<<move.sourceSquare().file()<<"-"<<move.sourceSquare().rank()
            <<"->"<<move.targetSquare().file()<<"-"<<move.targetSquare().rank();
    qDebug()<<"makeMove-scanString:"<<sanString;
    qDebug()<<"makeMove-Comment:"<<comment;
}

void NeroApp::moveChanged(int ply, const Chess::GenericMove &move, const QString &sanString, const QString &comment)
{
    qDebug()<<"moveChanged:";
}

void NeroApp::scoreChanged(int ply, int score)
{
    qDebug()<<"scoreChanged-ply:"<<ply<<"score:"<<score;
}

void NeroApp::initialized(ChessGame *game)
{
    qDebug()<<"initialized-chessgame";
}

void NeroApp::started(ChessGame *game)
{
    qDebug()<<"started-chessgame:"<<game->board()->fenString();
    qDebug()<<"-chessgame-WhiteName:"<<game->player(Chess::Side::White)->name();
    qDebug()<<"-chessgame-BlackName:"<<game->player(Chess::Side::Black)->name();

}

void NeroApp::finished(ChessGame *game, Chess::Result result)
{
    qDebug()<<"finished:";
}

void NeroApp::startFailed(ChessGame *game)
{
    qDebug()<<"startFailed";
}

void NeroApp::playersReady()
{
    qDebug()<<"playerReady";
}

