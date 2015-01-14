//parallelportplugin
//Copyright (C) 2015  Sven Gijsen
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


#include "ppgeneratethread.h"
#include "parallelport.h"

ppGenerateThread::ppGenerateThread(short baseAddress, int method, short outputMask, short activeValue, short inActiveValue, int activePulseTime, int repetitionTime, QObject *parent)	: QThread(parent)
{
	isRunning = false;
	abortRunning = false;
	nBaseAddress = baseAddress;
	dMethod = (ParallelPortNameSpace::GenerationMethod)method;
	nOutputMask = outputMask;
	nActiveValue = activeValue;
	nInActiveValue = inActiveValue;
	nActivePulseTime =activePulseTime;
	nRepetitionTime = repetitionTime;
	portDev = new ParallelPort(nBaseAddress,this);
}

ppGenerateThread::~ppGenerateThread()
{
	this->stop();
	if (portDev)
	{
		delete portDev;
		portDev = NULL;
	}
}

void ppGenerateThread::stop()
{
	if(this==NULL)
		return;
	if (isRunning)
	{
		//this->terminate();
		abortRunning = true;
		int nRetries = 25;
		while (isRunning && (nRetries > 0))
		{
			QThread::msleep(1);
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

void ppGenerateThread::run()
{
	isRunning = true;

	short currentValue;
	short newActiveMaskedValue;
	short newInActiveMaskedValue;
	//ParallelPort portDev(nBaseAddress,this);

	currentValue = portDev->PortRead();
	newActiveMaskedValue = ((currentValue & (255 - nOutputMask)) | (nActiveValue & nOutputMask));
	emit generateThreadStarted(	QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
	
	switch (dMethod)
	{
	case ParallelPortNameSpace::Value :	
		if (newActiveMaskedValue != currentValue)
		{
			portDev->PortWrite(newActiveMaskedValue);			
		}
		emit generateThreadTriggered(newActiveMaskedValue);
		break;	
	case ParallelPortNameSpace::Pulse:
		newInActiveMaskedValue = ((currentValue & (255 - nOutputMask)) | (nInActiveValue & nOutputMask));
		if (newInActiveMaskedValue != currentValue)
		{
			portDev->PortWrite(newInActiveMaskedValue);
			//emit generateThreadTriggered(newInActiveMaskedValue);
		}
		portDev->PortWrite(newActiveMaskedValue);
		emit generateThreadTriggered(newActiveMaskedValue);
		QThread::msleep(nActivePulseTime);
		portDev->PortWrite(newInActiveMaskedValue);
		//emit generateThreadTriggered(newInActiveMaskedValue);
		break;
	case ParallelPortNameSpace::Periodical:
		newInActiveMaskedValue = ((currentValue & (255 - nOutputMask)) | (nInActiveValue & nOutputMask));
		int nInActivePulseTime;
		if (nRepetitionTime>nActivePulseTime)
		{
			nInActivePulseTime = nRepetitionTime-nActivePulseTime;
		}
		else
		{
			nInActivePulseTime = 1;//Weird situation!!
		}
		if (newInActiveMaskedValue != currentValue)
		{
			portDev->PortWrite(newInActiveMaskedValue);
			//emit generateThreadTriggered(newInActiveMaskedValue);
		}
		do 
		{
			portDev->PortWrite(newActiveMaskedValue);
			emit generateThreadTriggered(newActiveMaskedValue);
			QThread::msleep(nActivePulseTime);
			portDev->PortWrite(newInActiveMaskedValue);
			//emit generateThreadTriggered(newInActiveMaskedValue);
			if (abortRunning) break;
			QThread::msleep(nInActivePulseTime);
		} while (abortRunning==false);
		break;
	default :
		break;	
	}

	abortRunning = false;
	emit generateThreadStopped(	QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
	isRunning = false;
	return;
}
