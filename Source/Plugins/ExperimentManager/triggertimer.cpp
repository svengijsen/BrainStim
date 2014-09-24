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


#include "triggertimer.h"

#define THREADACTIVATIONTRIGGERTIME		5 //This is the minimal trigger period, the time resolution/accuracy can be higher!

/*! \brief The TriggerTimer constructor.
*
*   No parameter
*/
TriggerTimer::TriggerTimer() : QObject(NULL)
{
	currentScriptEngine = NULL;
	fastExperimentThreadedTimer = NULL;
	nThreadIdealCount = QThread::idealThreadCount();
	//bool bResult = 
		connect(this,SIGNAL(stopTimerSignal()),&ThreadActivationTriggerQTimer,SLOT(stop()));//ThreadActivationTrigger = QTimer
	//bool bResult = connect(&_thread, SIGNAL(finished()), this, SLOT(handleThreadFinished()), Qt::DirectConnection);//QueuedConnection);
	this->moveToThread(&_thread);
	_thread.start();
	ThreadActivationTriggerQTimer.setInterval(THREADACTIVATIONTRIGGERTIME);
	ThreadActivationTriggerQTimer.moveToThread(&_thread);
	bDoStopTimer = false;
	dTriggerInterval = 0.0;
	dElapsed = 0.0;
	dNextThreshold = 0.0;
	dStartTime = 0.0;
	currentTimerType = ExperimentManagerNameSpace::QPC_TriggerTimerType;
	//createTimerTypeHashTable();
	bTimerIsRunning = false;
}

/*! \brief The TriggerTimer destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
TriggerTimer::~TriggerTimer()
{	
	bDoStopTimer = true;
	if(bTimerIsRunning)
	{
		//if ((currentTimerType == QPC_TriggerTimerType_Old) || (currentTimerType == QPC_TriggerTimerType))
		//{
			emit stopTimerSignal();
		//}
		//else 
		if (currentTimerType == ExperimentManagerNameSpace::QTimer_TriggerTimerType)
		{
			eTimer.stop();
		}
	}

	if(fastExperimentThreadedTimer)
	{
		fastExperimentThreadedTimer->stopTimer();
		delete fastExperimentThreadedTimer;
	}

	_thread.quit();
	int nForceTerminateRetries = 0;
	if(_thread.isRunning())
	{
		_thread.wait(THREADACTIVATIONTRIGGERTIME*2);
		while(_thread.isRunning())
		{
			_thread.terminate();//The hard way...
			_thread.wait(THREADACTIVATIONTRIGGERTIME);
			nForceTerminateRetries++;
			if(nForceTerminateRetries == 10)
				break;
		}
	}
}

QScriptValue TriggerTimer::ctor__triggerTimer(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new TriggerTimer(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

bool TriggerTimer::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
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

//void TriggerTimer::handleThreadFinished()
//{
//	return;
//}

//void TriggerTimer::createTimerTypeHashTable()
//{
//	timerTypeHash.clear();
//	timerTypeHash.insert("QPC_TriggerTimerType_Old", (int)QPC_TriggerTimerType);
//	timerTypeHash.insert("QTimer_TriggerTimerType", (int)QTimer_TriggerTimerType);
//	timerTypeHash.insert("QPC_TriggerTimerType", (int)QPCNew_TriggerTimerType);
//	timerTypeHash.insert("Fast_TriggerTimerType", (int)Fast_TriggerTimerType);
//}

bool TriggerTimer::setTimerType(const int nNewTimerType)
{
	return setTimerType((ExperimentManagerNameSpace::TriggerTimerType)nNewTimerType);
}

//bool TriggerTimer::setTimerType(const QString &sNewTimerType)
//{
//	int nTmpEnumIndex =  timerTypeHash.value(sNewTimerType,-1);
//	if (nTmpEnumIndex>=0)
//	{
//		return setTimerType((TriggerTimerType)nTmpEnumIndex);
//	}
//	return false;
//}

bool TriggerTimer::setTimerType(ExperimentManagerNameSpace::TriggerTimerType newTimerType)
{
	if (bTimerIsRunning == false)
	{
		currentTimerType = newTimerType;
		return true;
	}
	return false;	
}

//QString TriggerTimer::getTimerType() const
//{
//	return timerTypeHash.key((int)currentTimerType,UNKNOWNENUMSTRING);
//}

void TriggerTimer::startTimer(double dMSec)//Do not change the function name without changing the below!
{
	if(((currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType_Old) || (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType)) && (QThread::currentThread() != &_thread))//Wrong thread
	{
		QMetaObject::invokeMethod(this, "startTimer", Qt::QueuedConnection,Q_ARG(double, dMSec));//Cannot be a DirectConnection !!
	}
	else//Right thread
	{
		bTimerIsRunning = true;
		
		if (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType_Old)
		{
#ifdef Q_OS_WIN32 //Are we on Windows?
			//use the SetThreadAffinityMask() function to make your thread stick to one core or the other, so that 'rdtsc' and 
			//QueryPerformanceCounter() don't have timing issues in dual core systems.
			if(nThreadIdealCount>0)
				SetThreadAffinityMask(GetCurrentThread(),1<<(nThreadIdealCount-1));//Last Core
			else
				SetThreadAffinityMask(GetCurrentThread(),1);//First core
#endif
			//qDebug() << __FUNCTION__ << "Right thread";
			resetIntervalTestResults();
			eTimer.start();
			emit started();				
			connect(&ThreadActivationTriggerQTimer, SIGNAL(timeout()), this, SLOT(runThreadedTimerFunction()));
			//emit timeout();
			bDoStopTimer = false;
			dTriggerInterval = dMSec;
			dNextThreshold = dTriggerInterval;
			ThreadActivationTriggerQTimer.start();
		}
		else if (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType)
		{
#ifdef Q_OS_WIN32 //Are we on Windows?
			//use the SetThreadAffinityMask() function to make your thread stick to one core or the other, so that 'rdtsc' and 
			//QueryPerformanceCounter() don't have timing issues in dual core systems.
			if(nThreadIdealCount>0)
				SetThreadAffinityMask(GetCurrentThread(),1<<(nThreadIdealCount-1));//Last Core
			else
				SetThreadAffinityMask(GetCurrentThread(),1);//First core
#endif
			//qDebug() << __FUNCTION__ << "Right thread";
			resetIntervalTestResults();
			eTimer.start();
			dStartTime = WTF::currentTimeMS();
			emit started();
			connect(&ThreadActivationTriggerQTimer, SIGNAL(timeout()), this, SLOT(runThreadedTimerFunction()));
			//emit timeout();
			bDoStopTimer = false;
			dTriggerInterval = dMSec;
			dNextThreshold = dTriggerInterval;
			ThreadActivationTriggerQTimer.start();
		}
		else if (currentTimerType == ExperimentManagerNameSpace::QTimer_TriggerTimerType)
		{
			qtTimer.start(dMSec);
			emit started();
			connect(&qtTimer, SIGNAL(timeout()), this, SIGNAL(timeout()));
			bDoStopTimer = false;
		}
		else if (currentTimerType == ExperimentManagerNameSpace::Fast_TriggerTimerType)
		{
			fastExperimentThreadedTimer = new FastThreadedTriggerTimer();
			connect(fastExperimentThreadedTimer, &FastThreadedTriggerTimer::timeout, this, &TriggerTimer::timeout);
			bDoStopTimer = false;
			fastExperimentThreadedTimer->startTimer(dMSec);				
		}
	}
}

void TriggerTimer::resetIntervalTestResults()
{
	intervalTest.bSampleSpeedDetermined = false;
	intervalTest.dFirstSample = 0.0;
	intervalTest.dMinSampleSpeed = 0.0;
	intervalTest.dSampleSpeedToUse = THREADACTIVATIONTRIGGERTIME;
}

double TriggerTimer::currentTime() 
{
	return WTF::currentTime();
}

void TriggerTimer::stopTimer()//Do not change the function name without changing the below!
{
	if(((currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType_Old) || (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType)) && (QThread::currentThread() != &_thread))//Wrong thread
	//if((QThread::currentThread() != &_thread))//Wrong thread
	{
		QMetaObject::invokeMethod(this, "stopTimer", Qt::QueuedConnection);//Cannot be a DirectConnection !!
	}
	else//Right thread
	{
		if (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType_Old)
		{
			ThreadActivationTriggerQTimer.stop();
			bDoStopTimer = true;
			eTimer.stop();
			bTimerIsRunning = false;
		}
		else if (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType)
		{
			ThreadActivationTriggerQTimer.stop();
			bDoStopTimer = true;
			eTimer.stop();
			bTimerIsRunning = false;
		}
		else if (currentTimerType == ExperimentManagerNameSpace::QTimer_TriggerTimerType)
		{
			bDoStopTimer = true;
			qtTimer.stop();
			bTimerIsRunning = false;
		}
		else if (currentTimerType == ExperimentManagerNameSpace::Fast_TriggerTimerType)
		{
			bDoStopTimer = true;
			if(fastExperimentThreadedTimer)
			{
				fastExperimentThreadedTimer->stopTimer();
			}
			bTimerIsRunning = false;
		}
		_thread.quit();
	}
}

void TriggerTimer::runThreadedTimerFunction()
{
	if(bDoStopTimer)
		return;
	if (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType)
	{
		//dElapsed = eTimer.getElapsedTimeInMilliSec();
		dElapsed = WTF::currentTimeMS() - dStartTime;
		if ((((int)(dElapsed+1)) + intervalTest.dSampleSpeedToUse) >= ((int)dNextThreshold))//Can we dismiss the below?
		{
			dRemainingTime = dNextThreshold - dElapsed;			
			if(dRemainingTime > 0.0)
			{
				emit goingAccurate(dRemainingTime);
				// = WTF::currentTimeMS() - dStartTime;
				//eTimer.SleepMSecAccurate(dRemainingTime);
				eTimer.SleepMSecAccurate(dRemainingTime);
			}
			emit timeout();
			dNextThreshold = dNextThreshold + dTriggerInterval;
		}
		else//Time left, now we try to increase the intervalTest.dSampleSpeedToUse
		{
			if(intervalTest.bSampleSpeedDetermined == false)
			{
				if(intervalTest.dFirstSample == 0.0)
				{
					intervalTest.dFirstSample = dElapsed;
				}
				else
				{
					if(intervalTest.dMinSampleSpeed == 0.0)
					{
						intervalTest.dMinSampleSpeed = dElapsed - intervalTest.dFirstSample;		
						if (intervalTest.dMinSampleSpeed > THREADACTIVATIONTRIGGERTIME)
						{
							intervalTest.dSampleSpeedToUse = intervalTest.dMinSampleSpeed;
						}
						intervalTest.bSampleSpeedDetermined = true;
						//qDebug() << "intervalTest.dMinSampleSpeed set to: " << intervalTest.dMinSampleSpeed;
					}
				}
			}
		}
	}
	else if (currentTimerType == ExperimentManagerNameSpace::QPC_TriggerTimerType_Old)
	{
		dElapsed = eTimer.getElapsedTimeInMilliSec();
		if ((((int)(dElapsed+1)) + intervalTest.dSampleSpeedToUse) >= ((int)dNextThreshold))//Can we dismiss the below?
		{
			dRemainingTime = dNextThreshold - dElapsed;
			if(dRemainingTime > 0.0)
			{
				emit goingAccurate(dRemainingTime);
				eTimer.SleepMSecAccurate_old(dRemainingTime);
			}
			emit timeout();
			dNextThreshold = dNextThreshold + dTriggerInterval;
		}
		else//Time left, now we try to increase the intervalTest.dSampleSpeedToUse
		{
			if(intervalTest.bSampleSpeedDetermined == false)
			{
				if(intervalTest.dFirstSample == 0.0)
				{
					intervalTest.dFirstSample = dElapsed;
				}
				else
				{
					if(intervalTest.dMinSampleSpeed == 0.0)
					{
						intervalTest.dMinSampleSpeed = dElapsed - intervalTest.dFirstSample;		
						if (intervalTest.dMinSampleSpeed > THREADACTIVATIONTRIGGERTIME)
						{
							intervalTest.dSampleSpeedToUse = intervalTest.dMinSampleSpeed;
						}
						intervalTest.bSampleSpeedDetermined = true;
					}
				}
			}
		}
	}
}
