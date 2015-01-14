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

#ifndef FASTTHREADEDTRIGGERTIMER_H
#define FASTTHREADEDTRIGGERTIMER_H

#include <QObject>
#include <QString>
#include <QThread>
#include "./../../BrainStim/mainappinfo.h"
//#include "experimenttimer.h"

class FastThreadedTriggerTimer : public QObject
{
	Q_OBJECT

public:
	FastThreadedTriggerTimer(QObject *parent = 0);
	~FastThreadedTriggerTimer();
	
public slots:
	void timerInvoked();
	void stopTimer();
	void startTimer(double shotTime);

private:
	QThread workerThread;
	bool threadFinished;

signals:
	void startTimerSignal(double shotTime);
	void stopTimerSignal();
	void timeout();
	
};

#endif // FASTTHREADEDTRIGGERTIMER_H
