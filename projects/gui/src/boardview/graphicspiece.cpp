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

#include "graphicspiece.h"
#include <QPainter>
#include <QDebug>
#include <QPixmap>
#include <QImage>
GraphicsPiece::GraphicsPiece(const Chess::Piece& piece,
			     qreal squareSize,
			     const QString& elementId,
			     QGraphicsItem* parent)
	: QGraphicsObject(parent),
	  m_piece(piece),
      m_rect(-squareSize / 2, -squareSize / 2,
          squareSize, squareSize),
	  m_elementId(elementId),
	  m_container(nullptr)
{
	setAcceptedMouseButtons(Qt::LeftButton);
	setCacheMode(DeviceCoordinateCache);
}

int GraphicsPiece::type() const
{
	return Type;
}

QRectF GraphicsPiece::boundingRect() const
{
    return m_rect;
}

void GraphicsPiece::paint(QPainter* painter,
			  const QStyleOptionGraphicsItem* option,
			  QWidget* widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

    QPixmap map;
    if(m_elementId =="R"){
        map.load(":/piece/wrook.png");
    }else if(m_elementId == "r"){
        map.load(":/piece/brook.png");
    }else if(m_elementId == "N"){
        map.load(":/piece/wknight.png");
    }else if(m_elementId == "n"){
        map.load(":/piece/bknight.png");
    }else if(m_elementId == "B"){
        map.load(":/piece/wbishop.png");
    }else if(m_elementId == "b"){
        map.load(":/piece/bbishop.png");
    }else if(m_elementId == "Q"){
        map.load(":/piece/wqueen.png");
    }else if(m_elementId == "q"){
        map.load(":/piece/bqueen.png");
    }else if(m_elementId == "K"){
        map.load(":/piece/wking.png");
    }else if(m_elementId == "k"){
        map.load(":/piece/bking.png");
    }else if(m_elementId == "P"){
        map.load(":/piece/wpawn.png");
    }else if(m_elementId == "p"){
        map.load(":/piece/bpawn.png");
    }
    painter->drawPixmap(-17,-17,34,34,map);
}

Chess::Piece GraphicsPiece::pieceType() const
{
	return m_piece;
}

QGraphicsItem* GraphicsPiece::container() const
{
	return m_container;
}

void GraphicsPiece::setContainer(QGraphicsItem* item)
{
	m_container = item;
}

void GraphicsPiece::restoreParent()
{
	if (parentItem() == nullptr && m_container != nullptr)
	{
		QPointF point(m_container->mapFromScene(pos()));
		setParentItem(m_container);
		setPos(point);
	}
}
