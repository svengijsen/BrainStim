//ExperimentManagerplugin
//Copyright (C) 2014  Sven Gijsen and Michael Luehrs
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


#include "ExperimentTimer.h"
#include "CurrentTime.h"
#include <QDebug>

ExperimentTimer::ExperimentTimer(QObject *parent) : QObject(parent)
{
	currentScriptEngine = NULL;
#ifdef WIN32
	if (!QueryPerformanceFrequency(&frequency))// Save the performance counter frequency for later use.
		qDebug() << __FUNCTION__ << "QueryPerformanceFrequency() failed with error " << GetLastError();
	startCount.QuadPart = 0;
	endCount.QuadPart = 0;
#else
	startCount.tv_sec = startCount.tv_usec = 0;
	endCount.tv_sec = endCount.tv_usec = 0;
#endif
	stopped = 0;
	startTimeInMicroSec = 0;
	endTimeInMicroSec = 0;
	terminateTriggeredTimerLoop = false;
}

ExperimentTimer::~ExperimentTimer()
{
	terminateTriggeredTimerLoop = true;
}

QScriptValue ExperimentTimer::ctor__experimentTimer(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new ExperimentTimer(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

bool ExperimentTimer::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

void ExperimentTimer::restart()
{
	stop();
	start();
}

void ExperimentTimer::start()
{
	stopped = 0; // reset stop flag
#ifdef WIN32
	if (!QueryPerformanceCounter(&startCount))// Save the performance counter frequency for later use.
		qDebug() << __FUNCTION__ << "QueryPerformanceCounter() failed with error " << GetLastError();
#else
	gettimeofday(&startCount, NULL);
#endif
}

void ExperimentTimer::stop()
{
	stopped = 1; // set timer stopped flag
#ifdef WIN32
	if (!QueryPerformanceCounter(&endCount))// Save the performance counter frequency for later use.
		qDebug() << __FUNCTION__ << "QueryPerformanceCounter() failed with error " << GetLastError();
#else
	gettimeofday(&endCount, NULL);
#endif
}

double ExperimentTimer::getElapsedTimeInMicroSec()
{
#ifdef WIN32
	if(!stopped)
		if (!QueryPerformanceCounter(&endCount))// Save the performance counter frequency for later use.
			qDebug() << __FUNCTION__ << "QueryPerformanceCounter() failed with error " << GetLastError();
	return (endCount.QuadPart - startCount.QuadPart)*(1000000.0/frequency.QuadPart);
#else
	if(!stopped)
		gettimeofday(&endCount, NULL);

	startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
	endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
	return endTimeInMicroSec - startTimeInMicroSec;
#endif	
}

double ExperimentTimer::getElapsedTimeInMilliSec()
{
	return this->getElapsedTimeInMicroSec() * 0.001;
}

double ExperimentTimer::getElapsedTimeInSec()
{
	return this->getElapsedTimeInMicroSec() * 0.000001;
}

double ExperimentTimer::getElapsedTime()
{
	return this->getElapsedTimeInSec();
}

bool ExperimentTimer::SleepMSecAccurate_old(double mSecs)
{
	// note: BE SURE YOU CALL timeBeginPeriod(1) at program startup!!!
	// note: BE SURE YOU CALL timeEndPeriod(1) at program exit!!!
	// note: that will require linking to winmm.lib
	// note: never use static initializers (like this) with Winamp plug-ins!

	LARGE_INTEGER freq;
	LARGE_INTEGER start_tick;
	if (!QueryPerformanceFrequency(&freq))// Save the performance counter frequency for later use.
		qDebug() << __FUNCTION__ << "QueryPerformanceFrequency() failed with error " << GetLastError();
	//else
	//	qDebug() << __FUNCTION__ << "QueryPerformanceFrequency(freq) gives: " << freq.QuadPart;
	if (!QueryPerformanceCounter(&start_tick))
		qDebug() << __FUNCTION__ << "QueryPerformanceCounter() failed with error " << GetLastError();
	//else
	//	qDebug() << __FUNCTION__ << "QueryPerformanceCounter(start) gives: " << start_tick.QuadPart;
	
	if (mSecs > 0.5)
	{
		int ticks_passed = 0;
		int ticks_left = 0;
		LARGE_INTEGER current_tick;
		int ticks_to_wait = (int)(freq.QuadPart / (1000.0 / mSecs));
		bool doneWaiting = false;

		do
		{
			if (!QueryPerformanceCounter(&current_tick))
				qDebug() << __FUNCTION__ << "QueryPerformanceCounter() failed with error " << GetLastError();
			//else
			//	qDebug() << __FUNCTION__ << "QueryPerformanceCounter(current) gives: " << current_tick.QuadPart;
			ticks_passed = (int)((__int64)current_tick.QuadPart - (__int64)start_tick.QuadPart);
			ticks_left = ticks_to_wait - ticks_passed;
			if (ticks_passed >= ticks_to_wait)
				doneWaiting = true;
			if (!doneWaiting)
			{
				// if > 0.002s left, do Sleep(1), which will actually sleep some 
				//   steady amount, probably 1-2 ms,
				//   and do so in a nice way (cpu meter drops; laptop battery spared).
				//   otherwise, do a few Sleep(0)'s, which just give up the timeslice,
				//   but don't really save cpu or battery, but do pass a tiny
				//   amount of time.
				if (ticks_left > (int)((freq.QuadPart*2)/1000.0))
					Sleep(1);
				else                        
					for (int i=0; i<10; i++) 
						Sleep(0);  // causes thread to give up its timeslice
			}
		}
		while (!doneWaiting); 
	}
	else
	{
		Sleep(0);
	}
	return true;
}

bool ExperimentTimer::SleepMSecAccurate(double mSecs)
{
	double staticStartTimeInMicroSec = WTF::currentTimeMS();
	double staticRemainingTimeToWait = mSecs;

	if (mSecs > 0.5)
	{
		bool doneWaiting = false;
		do
		{
			staticRemainingTimeToWait = mSecs - (WTF::currentTimeMS() - staticStartTimeInMicroSec);
			if(staticRemainingTimeToWait > 0.05)
			{                      
				Sleep(0);  // causes thread to give up its timeslice
			}
			else
			{
				doneWaiting = true;
			}
		}
		while (!doneWaiting); 
	}
	return true;
}

void ExperimentTimer::startTriggeredTimerLoop(double nIntervalTimeMSecs)
{
	terminateTriggeredTimerLoop = false;
	while(!terminateTriggeredTimerLoop)
	{
		SleepMSecAccurate_old(nIntervalTimeMSecs);
		//Sleep(interval);
		emit triggeredTimerLoopInvoked();
		QCoreApplication::processEvents();
	}
	terminateTriggeredTimerLoop = false;
}

void ExperimentTimer::stopTriggeredTimerLoop()
{
	terminateTriggeredTimerLoop = true;
}