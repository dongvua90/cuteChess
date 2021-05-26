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

#include "gameviewer.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QSlider>
#include <QMessageBox>
#include <QSettings>
#include <pgngame.h>
#include <chessgame.h>
#include <chessplayer.h>
#include "boardview/boardscene.h"
#include "boardview/boardview.h"
#include "chessclock.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>

#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include "boardview/graphicsboard.h"
#include "boardview/graphicspiece.h"
#include <board/board.h>


GameViewer::GameViewer(QWidget* parent)
	: QWidget(parent),
      m_viewPreviousMoveBtn(new QToolButton),
      m_viewNextMoveBtn(new QToolButton),
	  m_moveIndex(0),
      m_humanGame(false)
{
    menu = new MenuScreen(this);
	m_boardScene = new BoardScene(this);
	m_boardView = new BoardView(m_boardScene, this);
	m_boardView->setEnabled(false);

    m_viewPreviousMoveBtn->setEnabled(false);
    m_viewPreviousMoveBtn->setAutoRaise(true);
    m_viewPreviousMoveBtn->setMinimumSize(32, 32);
    m_viewPreviousMoveBtn->setToolTip(tr("Previous move"));
    m_viewPreviousMoveBtn->setIcon(QIcon(":/icons/toolbutton/previous_16x16"));
    connect(m_viewPreviousMoveBtn, SIGNAL(clicked()),
        this, SLOT(viewPreviousMoveClicked()));

    m_viewNextMoveBtn->setEnabled(false);
    m_viewNextMoveBtn->setAutoRaise(true);
    m_viewNextMoveBtn->setMinimumSize(32, 32);
    m_viewNextMoveBtn->setToolTip(tr("Next move"));
    m_viewNextMoveBtn->setIcon(QIcon(":/icons/toolbutton/next_16x16"));
    connect(m_viewNextMoveBtn, SIGNAL(clicked()),
        this, SLOT(viewNextMoveClicked()));

	QHBoxLayout* controls = new QHBoxLayout();
	controls->setContentsMargins(0, 0, 0, 0);
	controls->setSpacing(0);
    controls->addWidget(m_viewPreviousMoveBtn);
    controls->addWidget(m_viewNextMoveBtn);
//    clockLayout->addLayout(controls);


    //begin add layout main
    setFixedSize(550,272);

    QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft);

    m_boardView->setFixedSize(272,272);
    layout->addWidget(m_boardView);

    QVBoxLayout* clockLayout = new QVBoxLayout();

    m_chessClock[0] = new ChessClock(true);
    m_chessClock[0]->setMaximumWidth(480);
    clockLayout->addWidget(m_chessClock[0]);
// begin add mid layout
    QVBoxLayout *midLayout = new QVBoxLayout();
    midLayout->setMargin(0);
    midLayout->setContentsMargins(0,0,0,0);
    midLayout->setSpacing(0);

    //MoveList
    QHBoxLayout *layout_info = new QHBoxLayout();
    mvlist =new MoveList();
    lb_info =   new QLabel();
    QFont lbinfofont = lb_info->font();
    lbinfofont.setPointSize(12);
//    lbinfofont.setBold(true);
    lb_info->setFont(lbinfofont);
    lb_info->setText("");
    layout_info->addWidget(lb_info);
    layout_info->addWidget(mvlist);
    mvlist->setVisible(false);

    QHBoxLayout *layout_tool = new QHBoxLayout();
    // btn extern
    btn_extern = new QPushButton();
    btn_extern->setFixedWidth(30);
    btn_extern->setFixedHeight(30);
    btn_extern->setCheckable(true);
    btn_extern->setIcon(QIcon(":/icons/toolbutton/previous_16x16"));
    connect(btn_extern,&QPushButton::clicked,this,&GameViewer::onExternClicked);
    //btn menu
    btn_menu = new QPushButton();
    btn_menu->setText("M");
    btn_menu->setFixedSize(30,30);
//    btn_menu->setStyleSheet("QPushButton{background-color:pink; border-radius:0px;font:20px;}QPushButton:pressed{background-color:red; border-radius:0px;}");
    connect(btn_menu,SIGNAL(clicked()),this,SLOT(onMenuClicked()));
    //btn change info
    btn_change_info = new QPushButton();
    btn_change_info->setFixedSize(30,30);
    btn_change_info->setCheckable(true);
    btn_change_info->setIcon(QIcon(":/icons/toolbutton/clone_16x16"));
    connect(btn_change_info,&QPushButton::clicked,this,&GameViewer::onChangeView);
    layout_tool->addWidget(btn_extern);
    layout_tool->addWidget(btn_menu);
    layout_tool->addWidget(btn_change_info);


    midLayout->addLayout(layout_info);
    midLayout->addLayout(layout_tool);




    clockLayout->addLayout(midLayout);
//end of mid layout

    m_chessClock[1] = new ChessClock(false);
    clockLayout->addWidget(m_chessClock[1]);
    clockLayout->setContentsMargins(0,0,0,0);
    clockLayout->setSpacing(0);
    clockLayout->setMargin(0);

    layout->addLayout(clockLayout);
    layout->setSpacing(0);
    //debug
    QVBoxLayout *layout_debug = new QVBoxLayout();
    layout_debug->setSpacing(0);
    layout_debug->setMargin(0);
    btn_move=new QPushButton();
    btn_move->setText("move");
    btn_move->setMaximumWidth(70);
    le_move = new QLineEdit();
    le_move->setMaximumWidth(70);
    layout_debug->addWidget(le_move);
    layout_debug->addWidget(btn_move);
    layout->addLayout(layout_debug);
    connect(btn_move,SIGNAL(clicked()),this,SLOT(onMakemove()));
    //end debug
	setLayout(layout);
}

ChessClock* GameViewer::chessClock(Chess::Side side)
{
    return m_chessClock[side];
}

MenuScreen *GameViewer::getMenu()
{
    return menu;
}

void GameViewer::setNotify(QString notify)
{
    lb_info->setText(notify);
}

QString GameViewer::getNotify()
{
    return lb_info->text();
}

void GameViewer::autoFlip()
{
	if (QSettings().value("ui/auto_flip_board_for_human_games",false).toBool())
		m_boardScene->flip();
}

void GameViewer::setGame(ChessGame* game)
{
	Q_ASSERT(game != nullptr);
	setGame(game->pgn());
	m_game = game;

	connect(m_game, SIGNAL(fenChanged(QString)),
		this, SLOT(onFenChanged(QString)));
	connect(m_game, SIGNAL(moveMade(Chess::GenericMove, QString, QString)),
		this, SLOT(onMoveMade(Chess::GenericMove)));
	connect(m_game, SIGNAL(humanEnabled(bool)),
		m_boardView, SLOT(setEnabled(bool)));

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_game->player(Chess::Side::Type(i)));
		if (player->isHuman())
			connect(m_boardScene, SIGNAL(humanMove(Chess::GenericMove, Chess::Side)),
				player, SLOT(onHumanMove(Chess::GenericMove, Chess::Side)));
	}

	connect(m_game, SIGNAL(finished(ChessGame*, Chess::Result)),
		m_boardScene, SLOT(onGameFinished(ChessGame*, Chess::Result)));
	m_boardView->setEnabled(!m_game->isFinished() &&
				m_game->playerToMove()->isHuman());
	m_humanGame = !m_game.isNull()
		    && m_game->playerToMove()->isHuman()
		    && m_game->playerToWait()->isHuman();

	if (m_humanGame && m_game->playerToMove() != m_game->player(Chess::Side::White))
		autoFlip();
}

void GameViewer::setGame(const PgnGame* pgn)
{
	Q_ASSERT(pgn != nullptr);

	disconnectGame();

	auto board = pgn->createBoard();
	if (board)
	{
		m_boardScene->setBoard(board);
		m_boardScene->populate();
	}
	else
	{
		QMessageBox::critical(
			this, tr("Cannot show game"),
			tr("This game is incompatible with Cute Chess and cannot be shown."));
	}
	m_moveIndex = 0;

	m_moves.clear();
	for (const PgnGame::MoveData& md : pgn->moves())
		m_moves.append(md.move);
    m_viewPreviousMoveBtn->setEnabled(false);
    m_viewNextMoveBtn->setEnabled(!m_moves.isEmpty());

    viewLastMove();
}

void GameViewer::disconnectGame()
{
	m_boardView->setEnabled(false);
	if (m_game.isNull())
		return;

	disconnect(m_game, nullptr, m_boardScene, nullptr);
	disconnect(m_game, nullptr, m_boardView, nullptr);
	disconnect(m_game, nullptr, this, nullptr);

	for (int i = 0; i < 2; i++)
	{
		ChessPlayer* player(m_game->player(Chess::Side::Type(i)));
		if (player != nullptr)
			disconnect(m_boardScene, nullptr, player, nullptr);
	}

	m_game = nullptr;
}

Chess::Board* GameViewer::board() const
{
	return m_boardScene->board();
}

BoardScene* GameViewer::boardScene() const
{
	return m_boardScene;
}

void GameViewer::viewFirstMoveClicked()
{
    viewFirstMove();
    emit moveSelected(m_moveIndex - 1);
}

void GameViewer::viewFirstMove()
{
    while (m_moveIndex > 0)
        viewPreviousMove();
}

void GameViewer::viewPreviousMoveClicked()
{
    viewPreviousMove();
    emit moveSelected(m_moveIndex - 1);
}

void GameViewer::viewPreviousMove()
{
    m_moveIndex--;

    if (m_moveIndex == 0)
    {
        m_viewPreviousMoveBtn->setEnabled(false);
    }

    m_boardScene->undoMove();

    m_viewNextMoveBtn->setEnabled(true);
    m_boardView->setEnabled(false);
}

void GameViewer::viewNextMoveClicked()
{
    viewNextMove();
    emit moveSelected(m_moveIndex - 1);
}

void GameViewer::viewNextMove()
{
    m_boardScene->makeMove(m_moves.at(m_moveIndex++));

    m_viewPreviousMoveBtn->setEnabled(true);

    if (m_moveIndex >= m_moves.count())
    {
        m_viewNextMoveBtn->setEnabled(false);

        if (!m_game.isNull()
        &&  !m_game->isFinished() && m_game->playerToMove()->isHuman())
            m_boardView->setEnabled(true);
    }
}

void GameViewer::viewLastMoveClicked()
{
    viewLastMove();
    emit moveSelected(m_moveIndex - 1);
}

void GameViewer::viewLastMove()
{
    while (m_moveIndex < m_moves.count())
        viewNextMove();
}

void GameViewer::viewPositionClicked(int index)
{
    viewPosition(index);
    emit moveSelected(m_moveIndex - 1);
}

void GameViewer::viewPosition(int index)
{
    if (m_moves.isEmpty())
        return;

    while (index < m_moveIndex)
        viewPreviousMove();
    while (index > m_moveIndex)
        viewNextMove();
}

void GameViewer::viewMove(int index, bool keyLeft)
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(!m_moves.isEmpty());

    if (keyLeft && index == m_moveIndex - 2)
        viewPreviousMove();
    else if (index < m_moveIndex)
    {
        // We backtrack one move too far and then make one
        // move forward to highlight the correct move
        while (index < m_moveIndex)
            viewPreviousMove();
        viewNextMove();
    }
    else
    {
        while (index + 1 > m_moveIndex)
            viewNextMove();
    }
}

void GameViewer::onFenChanged(const QString& fen)
{
	m_moves.clear();
	m_moveIndex = 0;

    m_viewPreviousMoveBtn->setEnabled(false);
    m_viewNextMoveBtn->setEnabled(false);
	m_boardScene->setFenString(fen);
}

void GameViewer::onMoveMade(const Chess::GenericMove& move)
{
//    qDebug()<<"onMoveMake:"<<move.sourceSquare().file()<<move.sourceSquare().rank()
//           <<" to "<<move.targetSquare().file()<<move.targetSquare().rank();
	m_moves.append(move);

    if(m_moveIndex == m_moves.count() - 1){
        viewNextMove();
    }

    if(m_humanGame){
       autoFlip();
    }
}

void GameViewer::onExternClicked(bool checked)
{
    if(checked){
        btn_extern->setIcon(QIcon(":/icons/toolbutton/next_16x16"));
        m_boardView->setFixedWidth(0);
    }else{
        btn_extern->setIcon(QIcon(":/icons/toolbutton/previous_16x16"));
        m_boardView->setFixedWidth(272);
    }
}

void GameViewer::onMenuClicked()
{
    menu->show();
}

void GameViewer::onChangeView(bool checked)
{
    if(checked)
    {
        mvlist->setVisible(true);
        lb_info->setVisible(false);
    }else{
        mvlist->setVisible(false);
        lb_info->setVisible(true);
    }
}

void GameViewer::makemove(QString move)
{
    m_boardScene->movePiece(move);
}

void GameViewer::onDebug()
{

   qDebug()<<"Fen:";
    m_boardScene->populate();
    GraphicsPiece *mpiece = m_boardScene->pieceAt(QPoint(85,85));
    mpiece->setOpacity(0.3f);
}

void GameViewer::onMakemove()
{
//    qDebug()<<"gameview_makemove";
    emit debugMakeMove(le_move->text());
}

