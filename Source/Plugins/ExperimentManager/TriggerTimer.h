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

#ifndef TRIGGERTIMER_H
#define TRIGGERTIMER_H

#include <QTimer>
#include <QThread>
#include <QtScript>
#include "ExperimentTimer.h"
#include "FastThreadedTriggerTimer.h"
#include "CurrentTime.h"
#include "maindefines.h"
#include "Global.h"

//!  The TriggerTimer class. 
/*!
  The TriggerTimer can be used to create a accurate/fast triggered threaded (interval) timer.
*/
class TriggerTimer : public QObject, protected QScriptable
{
	Q_OBJECT
	//Q_CLASSINFO("ScriptAPIClassName", "TriggerTimer");//Can't use defines here!, moc doesn't handle defines, not needed here

	struct strcIntervalTest
	{
		/*		strcIntervalTest()
		{
		dFirstSample = 0.0;
		dSecondSample = 0.0;
		dMinSampleSpeed = 0.0;
		}*/	
		bool bSampleSpeedDetermined;
		double dFirstSample;
		double dMinSampleSpeed;
		double dSampleSpeedToUse;
	};

signals:
	//! The timeout Signal.
	/*!
		This signal is emitted automatically at each period time after the trigger is started.
	*/
	void timeout();
	//! The started Signal.
	/*!
		This signal is emitted automatically after the Trigger Timer has been started, see TriggerTimer::startTimer().
	*/
	void started();
	//! The goingAccurate Signal.
	/*!
		This signal is emitted just before the TriggerTimer::timeout signal, the Trigger Timer is then more actively observed (higher CPU load) in order to achieve a more higher accuracy.
		This signal is only emitted when using the ExperimentManagerNameSpace::QPC_TriggerTimerType or ExperimentManagerNameSpace::QPC_TriggerTimerType_Old type, see TriggerTimer.setTimerType(const int).
		@param dRemainingTime a double value that represents the remaining time it should take for the Trigger Timer to timeout.
	*/
	void goingAccurate(double dRemainingTime);
	//! The stopTimerSignal Signal.
	/*!
		This signal is emitted when the TriggerTimer is stopped.		
	*/	
	void stopTimerSignal();

public:
	TriggerTimer();
	TriggerTimer(const TriggerTimer& other ){Q_UNUSED(other);};//TODO fill in copy constructor, should be declared for the Q_DECLARE_METATYPE macro
	~TriggerTimer();

	static QScriptValue ctor__triggerTimer(QScriptContext* context, QScriptEngine* engine);
	bool setTimerType(ExperimentManagerNameSpace::TriggerTimerType newTimerType);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	/*! \brief Starts the Trigger Timer.
	*  This function starts the Trigger Timer and then automatically emits a TriggerTimer::timeout() signal when triggered.
	* @param dMSec the period trigger time in milliseconds.
	*/
	void startTimer(double nMSec);
	/*! \brief Stops the Trigger Timer.
	*  This function immediately stops the TriggerTimer, see TriggerTimer.startTimer.
	*/
	void stopTimer();
	/*! \brief Returns the current UTC time.
	*   This function returns the current UTC time as a double value in seconds, counted from January 1, 1970.
	*   Precision varies depending on platform but is usually as good or better than a millisecond.
	*/
	static double currentTime();
	/*! \brief Sets the timer type to use for the TriggerTimer.
	*   This function Sets the timer type to use for the TriggerTimer, see ExperimentManagerNameSpace::TriggerTimerType.
	*   @param nNewTimerType a integer value holding the type of the timer, see ExperimentManagerNameSpace::TriggerTimerType.
	*   @return a boolean value representing whether the function could execute successfully.
	*/
	bool setTimerType(const int nNewTimerType);
	
	//bool setTimerType(const QString &sNewTimerType);
	//QString getTimerType() const;
	//void handleThreadFinished();
	//QObject *GetScriptMetaObject(int nIndex) {if(nIndex == 0) return (QObject *)this->metaObject(); else return NULL;};

private:
	void resetIntervalTestResults();
	//void createTimerTypeHashTable();

	QScriptEngine* currentScriptEngine;
	//QHash<QString, int> timerTypeHash;
	int nThreadIdealCount;
	QThread _thread;
	QTimer ThreadActivationTriggerQTimer;
	FastThreadedTriggerTimer *fastExperimentThreadedTimer;
	ExperimentTimer eTimer;
	QTimer qtTimer;
	bool bDoStopTimer;
	double dTriggerInterval;
	double dElapsed;
	double dStartTime;
	double dNextThreshold;
	double dRemainingTime;
	strcIntervalTest intervalTest;
	ExperimentManagerNameSpace::TriggerTimerType currentTimerType;
	bool bTimerIsRunning;

private slots:
	void runThreadedTimerFunction();

};

Q_DECLARE_METATYPE(TriggerTimer*)
Q_DECLARE_FLAGS(TriggerTimerTypes, ExperimentManagerNameSpace::TriggerTimerType)
Q_DECLARE_OPERATORS_FOR_FLAGS(TriggerTimerTypes)

#endif // TRIGGERTIMER_H
