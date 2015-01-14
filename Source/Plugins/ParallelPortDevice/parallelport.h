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


#ifndef PARALLELPORT_H
#define PARALLELPORT_H
#include <QObject>
#include <QString>
#include <QtScript/QtScript>
#include <QtScript/QScriptable>
#include "inpout32.h"
#include "ppCaptureThread.h"
#include "ppGenerateThread.h"
#include "./../../BrainStim/mainappinfo.h"

//!  The ParallelPort Device class. 
/*!
  The ParallelPort Device can be used to communicate with a Parallel Port.
*/
class ParallelPort : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY( short BaseAddress WRITE setBaseAddress READ getBaseAddress )
	//Q_CLASSINFO("ScriptAPIClassName", "ParallelPort");//Can't use defines here!, moc doesn't handle defines, not needed here

signals:
	//! The GenerateThreadTriggered Signal.
	/*!
		You can use this Signal for receiving a notification when the generate thread changes the port output value.
		The PortValue parameter(one byte) holds the triggered port value when triggered.
	*/
	void GenerateThreadTriggered(int PortValue);
	//! The GenerateThreadStarted Signal.
	/*!
		You can use this Signal to keep track of when the generate thread starts.
		Parameter DateTimeStamp holds the date/time of when the generate thread started.
	*/
	void GenerateThreadStarted(QString DateTimeStamp);
	//! The GenerateThreadStopped Signal.
	/*!
		You can use this Signal to keep track of when the generate thread stops.
		Parameter DateTimeStamp holds the date/time of when the generate thread stopped.
	*/
	void GenerateThreadStopped(QString);
	//! The CaptureThreadTriggered Signal.
	/*!
		You can use this Signal for receiving a notification when the capture thread gets triggered by the current port value.
		The PortValue parameter(one byte) holds the triggered port value when it gets triggered.
	*/
	void CaptureThreadTriggered(int PortValue);
	//! The CaptureThreadStarted Signal.
	/*!
		You can use this Signal to keep track of when the capture thread starts.
		Parameter DateTimeStamp holds the date/time of when the capture thread started.
	*/
	void CaptureThreadStarted(QString);
	//! The CaptureThreadStopped Signal.
	/*!
		You can use this Signal to keep track of when the capture thread stops.
		Parameter DateTimeStamp holds the date/time of when the capture thread stopped.
	*/
	void CaptureThreadStopped(QString);

public:
	ParallelPort(short BaseAddress = 888, QObject *parent = 0);
	ParallelPort(const ParallelPort& other){Q_UNUSED(other);}//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~ParallelPort();

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	void setBaseAddress( short BaseAddress );
	short getBaseAddress() const;

	short PortRead();
	void PortWrite(const short &Value);
	bool IsPortEpp();
	bool IsPortEcp();
	bool IsPortSpp();
	bool IsPortPS2();
	bool IsPortInput();
	bool IsPortOutput();
	bool ConfigurePortForInput();
	void ConfigurePortForOutput();
	QString GetPortDescription();

	bool BitRead(const short nBitNumber);
	void BitWrite(const short nBitNumber, bool Value);
	short BitReset(const short nBitNumber);
	short BitSet(const short nBitNumber);
	short BitToggle(const short nBitNumber);
	short StatusPortRead();
	short DataPortRead();
	void DataPortWrite(const short Value);
	short ControlPortRead();
	void ControlPortWrite(const short Value);
	int GetEppTimeoutBit();

	bool StartGenerateThread(const short baseAddress,const short method, const short outputMask, const short activeValue, const short inActiveValue, const int activePulseTime, const int repetitionTime);
	void StopGenerateThread();

	bool StartCaptureThread(const short baseAddress, const short mask, const short method, const int postLHDelay = 0, const int postHLDelay = 0);
	void StopCaptureThread();

private:
	QScriptEngine* currentScriptEngine;
	InpOut32 InpOut32Object;
	short nBaseAddress;

	ppCaptureThread *captureThread;
	ppGenerateThread *generateThread;
};

#endif // PARALLELPORT_H
