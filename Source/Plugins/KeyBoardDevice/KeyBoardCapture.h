//KeyBoardplugin
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


#ifndef KEYBOARDCAPTURE_H
#define KEYBOARDCAPTURE_H
#include <QObject>
#include <QString>
#include <QtScript/QtScript>
#include <QtScript/QScriptable>
#include <QMap>
#include "keyboardCaptureThread.h"
#include "./../../BrainStim/mainappinfo.h"

class keyboardCaptureThread;
class QTScriptEngine;

//!  The KeyBoard Device class. 
/*!
  The KeyBoard Device can be used to capture the KeyBoard.
*/
class KeyBoardCapture : public QObject, protected QScriptable
{
	Q_OBJECT
	//Q_CLASSINFO("ScriptAPIClassName", "ExperimentManager");//Can't use defines here!, moc doesn't handle defines, not needed here

signals:
	//void TestSignal(QPixmap *);

	//! The CaptureThreadKeyPressed Signal.
	/*!
		You can use this Signal for receiving a notification when the capture thread gets triggered by a KeyBoard key-press.
		The Key parameter holds the pressed Key.
	*/
	void CaptureThreadKeyPressed(quint32 Key);
	//! The CaptureThreadKeyReleased Signal.
	/*!
		You can use this Signal for receiving a notification when the capture thread gets triggered by a KeyBoard key-release.
		The Key parameter holds the released Key.
	*/
	void CaptureThreadKeyReleased(quint32 Key);
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
	/*! \brief The KeyBoard Device constructor.
	*
	*   You do not need to specify the parent object, 
	*	the BrainStim script engine automatically retrieves the parent role
	*/
	KeyBoardCapture(QObject *parent = NULL);
	KeyBoardCapture(const KeyBoardCapture& other ){Q_UNUSED(other);}//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	/*! \brief The KeyBoard Device destructor.
	*
	*   You do not need call the destructor. 
	*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
	*/
	~KeyBoardCapture();
	void setScriptEngine(QScriptEngine *scriptEngine) {parentScriptEngine = scriptEngine;};

public slots:
	//! For starting a new Keyboard Capture thread.
	/*!
		\param method defines the method use for the capture (see the KeyBoardNameSpace::CaptureKeyMethod).
		\param keyForwarding a boolean value determining whether the detected key should be forwarded or not.
		\param lKeyList a list (Array) of key-code values that can cause a capture detection signal (CaptureThreadKeyPressed, CaptureThreadKeyReleased) to occur.
		\return Whether the thread could be successfully started.
		\sa StopCaptureThread()
	*/
	bool StartCaptureThread(const short method, bool keyForwarding = true, const QList<int> &lKeyList = QList<int>());
	/*! Stops the Capture Thread(if started), see #StartCaptureThread. */
	void StopCaptureThread();
	//QString getVirtualKeyString(const quint32 &keyCode);

private:
	void setupKeyCodeTable();
	QMap<int,QString> mKeyCodes;
	keyboardCaptureThread *captureThread;

	static QScriptEngine *parentScriptEngine;
};

#endif // KEYBOARDCAPTURE_H
