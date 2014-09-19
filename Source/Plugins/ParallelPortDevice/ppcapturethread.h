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


#ifndef PPCAPTURETHREAD_H
#define PPCAPTURETHREAD_H

#include <QThread>
#include <QDateTime>
#include "Global.h"

class ParallelPort;
class ppCaptureThread : public QThread
{
	Q_OBJECT

protected:
	void run();

public:
	ppCaptureThread(short baseAddress, short mask, int method,int postLHDelay = 0, int postHLDelay = 0 ,QObject *parent = 0);
	~ppCaptureThread();

public slots:
	void stop();

private:
	bool isRunning;
	bool abortRunning;
	short nBaseAddress;
	int dMethod;//type of CaptureMethod
	short nMask;
	int nPostLHDelay;
	int nPostHLDelay; 
	ParallelPort *portDev;

signals:
	void recieveThreadTriggered(int Value);
	void recieveThreadStarted(QString timeTextStamp);
	void recieveThreadStopped(QString timeTextStamp);
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(ppCaptureThread::CaptureMethods)

#endif // PPCAPTURETHREAD_H
