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


#ifndef EXPERIMENTTIMER_H
#define EXPERIMENTTIMER_H

#include <QObject>
#include <QtScript>

#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

//!  The ExperimentTimer class. 
/*!
  The ExperimentTimer can be used to create a accurate/fast triggered non-threaded (interval) timer for keeping track of the elapsed time.
*/
class ExperimentTimer : public QObject
{
	Q_OBJECT

signals:
	//! The timeout Signal.
	/*!
		This signal is emitted automatically at each period time after the ExperimentTimer is started, see ExperimentTimer.startTriggeredTimerLoop().
	*/
	void triggeredTimerLoopInvoked();

public:
	ExperimentTimer(QObject *parent = NULL);
	ExperimentTimer(const ExperimentTimer& other ){Q_UNUSED(other);};//TODO fill in copy constructor, should be declared for the Q_DECLARE_METATYPE macro
	~ExperimentTimer();

	static QScriptValue ctor__experimentTimer(QScriptContext* context, QScriptEngine* engine);

	static bool SleepMSecAccurate_old(double mSecs);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	/*! \brief Starts the internal timer in the ExperimentTimer that keeps track of the elapsed time.
	*  This function starts the internal timer in the ExperimentTimer for keeping track of the elapsed time.
	*  See also ExperimentTimer.getElapsedTime(), ExperimentTimer.restart(), ExperimentTimer.stop().
	*/
	void start();									// start timer
	/*! \brief Stops the internal timer in the ExperimentTimer that keeps track of the elapsed time.
	*  This function stops the internal timer in the ExperimentTimer for keeping track of the elapsed time.
	*  See also ExperimentTimer.getElapsedTime(), ExperimentTimer.restart(), ExperimentTimer.start().
	*/
	void stop();									// stop the timer
	/*! \brief Restarts the internal timer in the ExperimentTimer that keeps track of the elapsed time.
	*  This function restarts the internal timer in the ExperimentTimer for keeping track of the elapsed time.
	*  See also ExperimentTimer.getElapsedTime(), ExperimentTimer.start(), ExperimentTimer.stop().
	*/
	void restart();									// restart the timer
	/*! \brief See ExperimentTimer.getElapsedTimeInSec() */
	double getElapsedTime();						// get elapsed time in seconds
	/*! \brief Returns the elapsed time since the internal timer was started in seconds.
	*  This function returns the elapsed time since the internal timer was started in seconds, see ExperimentTimer.start(), ExperimentTimer.restart() and ExperimentTimer.stop().
	*  @return a double value holding the elapsed time since the internal timer was started in seconds.
	*/
	double getElapsedTimeInSec();					// get elapsed time in second (same as getElapsedTime)
	/*! \brief Returns the elapsed time since the internal timer was started in milli-seconds.
	*  This function returns the elapsed time since the internal timer was started in milli-seconds, see ExperimentTimer.start(), ExperimentTimer.restart() and ExperimentTimer.stop().
	*  @return a double value holding the elapsed time since the internal timer was started in milli-seconds.
	*/
	double getElapsedTimeInMilliSec();				// get elapsed time in milli seconds
	/*! \brief Returns the elapsed time since the internal timer was started in micro-seconds.
	*  This function returns the elapsed time since the internal timer was started in micro-seconds, see ExperimentTimer.start(), ExperimentTimer.restart() and ExperimentTimer.stop().
	*  @return a double value holding the elapsed time since the internal timer was started in micro-seconds.
	*/
	double getElapsedTimeInMicroSec();				// get elapsed time in micro-second
	/*! \brief Makes the current thread go to sleep accurately for a specified amount of time.
	*  This function makes the current thread go to sleep accurately for a specified amount of provided time.
	*  @param mSecs a double value determing the time in milli-seconds the thread should go to sleep.
	*  @return a boolean value that determines whether the function executed successfully.
	*/
	static bool SleepMSecAccurate(double mSecs);

	/*! \brief Starts the ExperimentTimer at a periodic interval time.
	*  This function starts the Experiment Timer and then automatically emits a ExperimentTimer::triggeredTimerLoopInvoked() signal when it timed out.\n
	*  ! The ExperimentTimer implementation is non-threaded and takes place in the GUI thread, please consider to use the TriggerTimer TriggerTimer.startTimer() implementation for better results.
	* @param nIntervalTimeMSecs a double value representing the period timeout time in milliseconds.
	*/
	void startTriggeredTimerLoop(double nIntervalTimeMSecs);
	/*! \brief Stops the running periodic interval thread from the ExperimentTimer.
	*  This function stops the running periodic interval thread from the ExperimentTimer, see ExperimentTimer.startTriggeredTimerLoop().\n
	*  ! The ExperimentTimer implementation is non-threaded and takes place in the GUI thread, please consider to use the TriggerTimer TriggerTimer.startTimer() implementation for better results.
	*/
	void stopTriggeredTimerLoop();

private:
	QScriptEngine* currentScriptEngine;

	double startTimeInMicroSec;						// starting time in micro-second
	double endTimeInMicroSec;						// ending time in micro-second
	int    stopped;									// stop flag 
#ifdef WIN32
	LARGE_INTEGER frequency;						// ticks per second
	LARGE_INTEGER startCount;                   
	LARGE_INTEGER endCount;                     
#else
	timeval startCount;                         
	timeval endCount;                           
#endif

	bool terminateTriggeredTimerLoop;
	
};

#endif // EXPERIMENTTIMER_H
