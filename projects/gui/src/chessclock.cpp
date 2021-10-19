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

#include "chessclock.h"
#include <QTime>
#include <QTimerEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>


ChessClock::ChessClock(bool posUp,QWidget* parent)
	: QWidget(parent),
	  m_totalTime(0),
	  m_timerId(-1),
	  m_infiniteTime(false),
	  m_nameLabel(new QLabel()),
      m_timeLabel(new QLabel())
{
    taskinfo = new TaskInfo();
	m_defaultPalette = m_timeLabel->palette();
    m_headerPalette = m_timeLabel->palette();
    m_defaultPalette.setColor(QPalette::Window,Qt::blue);
    m_headerPalette.setColor(QPalette::Background,QColor(43,23,0));
    m_headerPalette.setColor(QPalette::WindowText,QColor(78,224,207));
    m_timeLabel->setAutoFillBackground(true);

	m_nameLabel->setTextFormat(Qt::RichText);
    m_nameLabel->setAlignment(Qt::AlignHCenter |Qt::AlignVCenter);
    m_nameLabel->setAutoFillBackground(true);
    m_nameLabel->setPalette(m_headerPalette);
    setPalette(m_headerPalette);

	m_timeLabel->setTextFormat(Qt::RichText);
    m_timeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    m_nameLabel->setMinimumWidth(208);
    m_timeLabel->setMinimumWidth(208);
    m_nameLabel->setMaximumWidth(480);
    m_timeLabel->setMaximumWidth(480);
    m_nameLabel->setFixedHeight(45);
    m_timeLabel->setFixedHeight(45);

    m_timeLabel->setMargin(5);
    m_nameLabel->setMargin(5);


	QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignTop);
    layout->setSpacing(0);
    if(posUp){
        taskinfo->setParent(m_nameLabel);
        taskinfo->move(350,0);
        layout->addWidget(m_nameLabel);
        layout->addWidget(m_timeLabel);
    }else{
        layout->addWidget(m_timeLabel);
        layout->addWidget(m_nameLabel);
    }

	setLayout(layout);
    setMinimumWidth(208);
    setMaximumWidth(480);
    setFixedHeight(90);
}

void ChessClock::setPlayerName(const QString& name)
{
	if (name.isEmpty())
		m_nameLabel->clear();
	else
		m_nameLabel->setText(QString("<h3>%1</h3>").arg(name));
}

void ChessClock::setInfiniteTime(bool infinite)
{
	m_infiniteTime = infinite;
	if (!infinite)
		return;

	stopTimer();
	m_timeLabel->setText(QString::fromUtf8("<h1>\xE2\x88\x9E</h1>"));
}

void ChessClock::setTime(int totalTime)
{
	if (m_infiniteTime)
		return;

	QTime timeLeft = QTime(0, 0).addMSecs(abs(totalTime + 500));

	QString format;
	if (timeLeft.hour() > 0)
		format = "hh:mm:ss";
	else
		format = "mm:ss";

	QString str;
	if (totalTime <= -500)
		str.append("-");
	str.append(timeLeft.toString(format));

	m_timeLabel->setText(QString("<h1>%1</h1>").arg(str));
}

void ChessClock::start(int totalTime)
{
	QPalette tmp = m_timeLabel->palette();
    tmp.setColor(QPalette::WindowText,Qt::white);
    tmp.setColor(QPalette::Window,Qt::red);
	m_timeLabel->setPalette(tmp);

	if (!m_infiniteTime)
	{
		m_time.start();
		m_totalTime = totalTime;
		m_timerId = startTimer(1000);
		setTime(totalTime);
	}
    qDebug()<<"clock-start";
}

void ChessClock::stop()
{
	m_timeLabel->setPalette(m_defaultPalette);

	stopTimer();
	if (!m_infiniteTime)
		setTime(m_totalTime - m_time.elapsed());
}

void ChessClock::timerEvent(QTimerEvent* event)
{
	if (!event)
		return;
	
	if (event->timerId() == m_timerId)
		setTime(m_totalTime - m_time.elapsed());
}

void ChessClock::stopTimer()
{
	if (m_timerId != -1)
	{
		killTimer(m_timerId);
		m_timerId = -1;
	}
}
