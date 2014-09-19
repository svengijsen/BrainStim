//parallelportplugin
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


#ifndef PPGENERATETHREAD_H
#define PPGENERATETHREAD_H

#include <QThread>
#include <QDateTime>
#include "Global.h"

class ParallelPort;
class ppGenerateThread : public QThread
{
	Q_OBJECT

protected:
	void run();

public:

	ppGenerateThread(short baseAddress, int method, short outputMask, short activeValue, short inActiveValue = 0, int activePulseTime = 10, int repetitionTime = 2000, QObject *parent = 0);
	~ppGenerateThread();

	public slots:
		void stop();

private:
	bool isRunning;
	bool abortRunning;
	short nBaseAddress;
	int dMethod;//Type of GenerationMethod
	short nOutputMask;
	short nActiveValue;
	short nInActiveValue;
	int nActivePulseTime;
	int nRepetitionTime;
	ParallelPort *portDev;

signals:
	void generateThreadTriggered(int Value);
	void generateThreadStarted(QString timeTextStamp);
	void generateThreadStopped(QString timeTextStamp);
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(ppGenerateThread::GenerationMethods)

#endif // PPGENERATETHREAD_H
