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

#ifndef GAMEVIEWER_H
#define GAMEVIEWER_H

#include <QWidget>
#include <QVector>
#include <QPointer>
#include <board/side.h>
#include <board/genericmove.h>
#include <QPushButton>
#include "menuscreen.h"
#include "movelist.h"
#include "robot.h"
#include <QLineEdit>
#include <QLabel>
class QToolButton;
class QSlider;
class ChessGame;
class PgnGame;
class BoardScene;
class BoardView;
class ChessClock;
namespace Chess
{
	class Board;
}

class GameViewer : public QWidget
{
	Q_OBJECT

	public:
        explicit GameViewer(QWidget* parent = nullptr);

		void setGame(ChessGame* game);
		void setGame(const PgnGame* pgn);
		void disconnectGame();
		Chess::Board* board() const;
		BoardScene* boardScene() const;
		ChessClock* chessClock(Chess::Side side);
        MenuScreen* getMenu();

        MoveList *mvlist;
        void setNotify(QString notify);
        QString getNotify();

	public slots:
		void viewMove(int index, bool keyLeft = false);

//         void makemove(uint8_t qFrom,uint8_t qTo,enum Robot::MOVETYPE move_type);
//         void makemove2(QString move,Chess::Side side);
         void makemove(QString move);

	signals:
		void moveSelected(int moveNumber);
        void boardMakeMove(QString move);
        void debugMakeMove(QString move);

	private slots:
        void viewFirstMoveClicked();
        void viewPreviousMoveClicked();
        void viewNextMoveClicked();
        void viewLastMoveClicked();
        void viewPositionClicked(int index);

		void onFenChanged(const QString& fen);
		void onMoveMade(const Chess::GenericMove& move);

        void onExternClicked(bool checked);
        void onMenuClicked();
        void onChangeView(bool checked);


        void onDebug();
        void onMakemove();

	private:

        void viewFirstMove();
        void viewPreviousMove();
        void viewNextMove();
        void viewLastMove();
        void viewPosition(int index);
		void autoFlip();

		BoardScene* m_boardScene;
		BoardView* m_boardView;
//		QSlider* m_moveNumberSlider;
        ChessClock* m_chessClock[2];
        QLabel *lb_info;

        QToolButton* m_viewFirstMoveBtn;
        QToolButton* m_viewPreviousMoveBtn;
        QToolButton* m_viewNextMoveBtn;
        QToolButton* m_viewLastMoveBtn;

		QPointer<ChessGame> m_game;
		QVector<Chess::GenericMove> m_moves;
		int m_moveIndex;
		bool m_humanGame;

        QPushButton *btn_extern;
        QPushButton *btn_menu;
        QPushButton *btn_change_info;

        MenuScreen *menu;
        QLineEdit *text_move;

        ChessGame *mygame;

        //debug
        QPushButton *btn_move;
        QLineEdit *le_move;

};

#endif // GAMEVIEWER_H
