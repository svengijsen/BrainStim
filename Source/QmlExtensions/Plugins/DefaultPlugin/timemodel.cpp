//DefaultQMLPlugin
//Copyright (C) 2014  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "timemodel.h"

MinuteTimer::MinuteTimer(QObject *parent) : QObject(parent)
{

}

void MinuteTimer::start()
{
	if (!timer.isActive()) 
	{
		time = QTime::currentTime();
		timer.start(60000-time.second()*1000, this);
	}
}

void MinuteTimer::stop()
{
	timer.stop();
}

int MinuteTimer::hour() const 
{ 
	return time.hour(); 
}

int MinuteTimer::minute() const 
{ 
	return time.minute(); 
}

void MinuteTimer::timerEvent(QTimerEvent *)
{
	QTime now = QTime::currentTime();
	if (now.second() == 59 && now.minute() == time.minute() && now.hour() == time.hour()) 
	{
		// just missed time tick over, force it, wait extra 0.5 seconds
		time.addSecs(60);
		timer.start(60500, this);
	} 
	else 
	{
		time = now;
		timer.start(60000-time.second()*1000, this);
	}
	emit timeChanged();
}

//////////////////////////////////////////////////////////////////////////

int TimeModel::instances=0;
MinuteTimer *TimeModel::timer=0;

TimeModel::TimeModel(QObject *parent) : QObject(parent)
{
	if (++instances == 1) 
	{
		if (!timer)
			timer = new MinuteTimer(qApp);
		connect(timer, SIGNAL(timeChanged()), this, SIGNAL(timeChanged()));
		timer->start();
	}
}

TimeModel::~TimeModel()
{
	if (--instances == 0) 
	{
		timer->stop();
	}
}

int TimeModel::minute() const
{ 
	return timer->minute(); 
}

int TimeModel::hour() const
{ 
	return timer->hour(); 
}
