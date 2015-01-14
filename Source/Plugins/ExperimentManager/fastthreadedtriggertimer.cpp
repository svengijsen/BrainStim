//ExperimentManagerplugin
//Copyright (C) 2015  Michael Luehrs and Sven Gijsen
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

#include "fastthreadedtriggertimer.h"
#include "experimenttimer.h"

FastThreadedTriggerTimer::FastThreadedTriggerTimer(QObject *parent) : QObject(parent)
{
	threadFinished = false;

	ExperimentTimer *privateTimer = new ExperimentTimer();
	privateTimer->moveToThread(&workerThread);
	connect(&workerThread, &QThread::finished, privateTimer, &QObject::deleteLater);
	connect(this, &FastThreadedTriggerTimer::startTimerSignal, privateTimer, &ExperimentTimer::startTriggeredTimerLoop);
	connect(privateTimer, &ExperimentTimer::triggeredTimerLoopInvoked, this, &FastThreadedTriggerTimer::timerInvoked);
	connect(this, &FastThreadedTriggerTimer::stopTimerSignal, privateTimer, &ExperimentTimer::stopTriggeredTimerLoop);
	workerThread.start();
}

FastThreadedTriggerTimer::~FastThreadedTriggerTimer()
{
	workerThread.quit();
	workerThread.wait();
}

void FastThreadedTriggerTimer::startTimer(double shotTime)
{
	threadFinished = false;
	emit startTimerSignal(shotTime);
}

void FastThreadedTriggerTimer::timerInvoked()
{
	if(!threadFinished)
		emit timeout();
}

void FastThreadedTriggerTimer::stopTimer()
{
	threadFinished = true;
	emit stopTimerSignal();
}