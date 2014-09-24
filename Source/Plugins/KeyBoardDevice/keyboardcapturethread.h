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


#ifndef KEYBOARDCAPTURETHREAD_H
#define KEYBOARDCAPTURETHREAD_H

#include <QThread>
#include <QDateTime>
#include "global.h"
#include "keyboardcapture.h"
#include "systemkeyboardreadwrite.h"

#define WM_KEYDOWN                0x0100
#define WM_KEYUP                  0x0101

class KeyBoardCapture;
class keyboardCaptureThread : public QThread
{
	Q_OBJECT

signals:
	void recieveThreadKeyPressed(quint32 key);
	void recieveThreadKeyReleased(quint32 key);
	void recieveThreadStarted(QString timeTextStamp);
	void recieveThreadStopped(QString timeTextStamp);

protected:
	void run();

public:
	keyboardCaptureThread(int method, QObject *parent = 0);
	~keyboardCaptureThread();

	bool setKeyEventForwarding(bool bForward);
	bool setMaskedKeyList(const QList<int> &keyList = QList<int>());
	bool getKeyEventForwarding();

public slots:
	void stop();

private slots:
	void recieveThreadKeyPressedHandler(quint32 nKeycode);
	void recieveThreadKeyReleasedHandler(quint32 nKeycode);

private:
	bool isRunning;
	bool abortRunning;
	KeyBoardNameSpace::CaptureKeyMethod dMethod;				//type of CaptureKeyMethod
	bool bForwardKeyEvents;
	QList<int> lMaskedKeys;
	SystemKeyboardReadWrite *systemKeyCapture;

};

#endif // KEYBOARDCAPTURETHREAD_H
