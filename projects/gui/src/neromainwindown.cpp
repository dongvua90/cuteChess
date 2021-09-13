#include "neromainwindown.h"
#include "ui_neroMainwindown.h"
#include "neroapp.h"
#include <gamemanager.h>
#include <chessplayer.h>
#include <QDebug>
#include <chessgame.h>
#include <board/boardfactory.h>
#include <board/result.h>

NeroMainWindown::NeroMainWindown(ChessGame *game) : QWidget()
  ,ui(new Ui::NeroMainwindown)
{
    ui->setupUi(this);
    mygame = game;
    connect(NeroApp::instance()->gameManager(),SIGNAL(finished()), this, SLOT(onGameManagerFinished()),
        Qt::QueuedConnection);
    connect(mygame, SIGNAL(started(ChessGame*)),
        this, SLOT(onGameStarted(ChessGame*)));

}

void NeroMainWindown::onGameManagerFinished()
{
    qDebug("onGameFinish");
}

void NeroMainWindown::onGameStarted(ChessGame *game)
{
    qDebug()<<"MainWinDown-chessgame:"<<game->board()->fenString();
    qDebug()<<"-chessgame-WhiteName:"<<game->player(Chess::Side::White)->name();
    qDebug()<<"-chessgame-BlackName:"<<game->player(Chess::Side::Black)->name();
}

void NeroMainWindown::on_btn_start_clicked()
{

}

void NeroMainWindown::on_btn_move_clicked()
{
    Chess::Move move(9,17);
    Chess::Square source(0,1);
    Chess::Square target(0,2);
//    Chess::GenericMove mmove(source,target,0);
    Chess::Move mo(source.file()*source.rank(),target.file()*target.rank());
//    mygame->player(Chess::Side::White)->makeMove(mo);
    Chess::Result ref(Chess::Result::Draw);
//    mygame->player(Chess::Side::White)->endGame(ref);
//    mygame->kill();
//    mygame->onAdjudication(ref);
    mygame->onMoveMade(move);
}
