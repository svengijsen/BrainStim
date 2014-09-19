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


#include "ppcapturethread.h"
#include "parallelport.h"

ppCaptureThread::ppCaptureThread(short baseAddress, short mask, int method,int postLHDelay, int postHLDelay , QObject *parent) : QThread(parent)
{
	isRunning = false;
	abortRunning = false;
	nBaseAddress = baseAddress;
	dMethod = (ParallelPortNameSpace::CaptureMethod)method;
	nMask = mask;
	nPostLHDelay = postLHDelay;
	nPostHLDelay = postHLDelay;
	portDev = new ParallelPort(nBaseAddress,this);
}

ppCaptureThread::~ppCaptureThread()
{
	this->stop();
	if (portDev)
	{
		delete portDev;
		portDev = NULL;
	}
}

void ppCaptureThread::stop()
{
	if(this==NULL)
		return;
	if (isRunning)
	{
		//this->terminate();
		abortRunning = true;
		int nRetries = 50;
		while (isRunning && (nRetries > 0))
		{
			QThread::msleep(1);
			qApp->processEvents(QEventLoop::ExcludeSocketNotifiers,5);
			nRetries--;
		}
		QThread::msleep(0);//Just to let the other thread completely finish
		QThread::msleep(0);//Just to let the other thread completely finish
		//if (nRetries > 0)
		//{
			//couldn't stop the thread, should we implement a false boolean return so that TerminateThread can be called?
		//}		
	}	
}

void ppCaptureThread::run()
{
	isRunning = true;
	
	short currentValue;
	short oldValue;
	QString result = "";
	//ParallelPort portDev(nBaseAddress,this);
	//portDev = new ParallelPort(nBaseAddress,this);
	//portDev->setBaseAddress()

	currentValue = (portDev->PortRead() & nMask);
	oldValue = currentValue;
	emit recieveThreadStarted(QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
	switch (dMethod)
	{
	case ParallelPortNameSpace::MaskedValueChanged :
		do 
		{
			if (oldValue != currentValue)//any change within masked measurement
			{
				//result = QString::number(currentValue);
				emit recieveThreadTriggered(currentValue);
				if (nPostLHDelay > 0)
				{
					if (currentValue > 0)//low->high was triggered
					{
						QThread::msleep(nPostLHDelay);
					}
				}
				if (nPostHLDelay > 0)
				{
					if (currentValue == 0)//high->low was triggered
					{
						QThread::msleep(nPostHLDelay);
					}
				}
				oldValue = currentValue;
			}
			if (abortRunning==false)
			{
				QThread::msleep(1);
				QThread::msleep(1);			
				currentValue = (portDev->PortRead() & nMask);
			}
		} while (abortRunning==false);
		break;	
	case ParallelPortNameSpace::MaskedValueChangedHigh :
		do 
		{
			if (oldValue != currentValue)
			{
				if (currentValue == nMask)//low->high was triggered
				{
					//result = QString::number(currentValue);
					emit recieveThreadTriggered(currentValue);	
					if (nPostLHDelay > 0) QThread::msleep(nPostLHDelay);
				}
				oldValue = currentValue;
			}
			if (abortRunning==false)
			{
				QThread::msleep(1);
				QThread::msleep(1);
				currentValue = (portDev->PortRead() & nMask);
			}
		} while (abortRunning==false);
		break;
	case ParallelPortNameSpace::MaskedValueChangedLow :
		do 
		{
			if (oldValue != currentValue)
			{
				if (currentValue == 0)//high->low was triggered
				{
					//result = QString::number(currentValue);
					emit recieveThreadTriggered(currentValue);	
					if (nPostHLDelay > 0) QThread::msleep(nPostHLDelay);
				}
				oldValue = currentValue;
			}
			if (abortRunning==false)
			{
				QThread::msleep(1);
				QThread::msleep(1);			
				currentValue = (portDev->PortRead() & nMask);
			}
		} while (abortRunning==false);
		break;
	default :
		break;	
	}
	abortRunning = false;	
	emit recieveThreadStopped(	QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
	isRunning = false;
	return;
}

