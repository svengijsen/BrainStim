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


#include "keyboardcapturethread.h"
#include "keyboardcapture.h"


keyboardCaptureThread::keyboardCaptureThread(int method, QObject *parent) : QThread(parent)
{
	systemKeyCapture = NULL;
	isRunning = false;
	abortRunning = false;
	bForwardKeyEvents = true;
	dMethod = (KeyBoardNameSpace::CaptureKeyMethod)method;
}

keyboardCaptureThread::~keyboardCaptureThread()
{
	this->stop();
	if(systemKeyCapture)
	{
		delete systemKeyCapture;
		systemKeyCapture = NULL;
	}
}

bool keyboardCaptureThread::setKeyEventForwarding(bool bForward)
{
	bForwardKeyEvents = bForward;
	return bForwardKeyEvents;
}

bool keyboardCaptureThread::setMaskedKeyList(const QList<int> &keyList)
{
	lMaskedKeys = keyList;
	return true;
}

bool keyboardCaptureThread::getKeyEventForwarding()
{
	return bForwardKeyEvents;
}

void keyboardCaptureThread::stop()
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
 
void keyboardCaptureThread::run()
{
	isRunning = true;

	if(systemKeyCapture)
	{
		delete systemKeyCapture;
		systemKeyCapture = NULL;
	}
	systemKeyCapture = new SystemKeyboardReadWrite();
	if (systemKeyCapture)
	{
		if (systemKeyCapture->setConnected(true, bForwardKeyEvents))
		{
			bool bDoHandleKeyPress = ((dMethod == KeyBoardNameSpace::KeyPressed) || (dMethod == KeyBoardNameSpace::KeyPressedReleased));
			bool bDoHandleKeyRelease = ((dMethod == KeyBoardNameSpace::KeyReleased) || (dMethod == KeyBoardNameSpace::KeyPressedReleased));
			bool bKeyPressConnected = false;
			bool bKeyReleaseConnected = false;
			emit recieveThreadStarted(QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
			if(bDoHandleKeyPress)
				bKeyPressConnected = connect(systemKeyCapture->instance(), SIGNAL(keyPressedSignal(quint32)), this, SLOT(recieveThreadKeyPressedHandler(quint32)));
			if(bDoHandleKeyRelease)
				bKeyReleaseConnected = connect(systemKeyCapture->instance(), SIGNAL(keyReleasedSignal(quint32)), this, SLOT(recieveThreadKeyReleasedHandler(quint32)));
			if ((bKeyPressConnected)||(bKeyReleaseConnected))
			{
				do 
				{
					this->thread()->msleep(1);
					qApp->processEvents(QEventLoop::ExcludeSocketNotifiers,1);
				} 
				while (abortRunning==false);
				if(bKeyPressConnected)
					disconnect(systemKeyCapture->instance(), SIGNAL(keyPressedSignal(quint32)), this, SLOT(recieveThreadKeyPressedHandler(quint32)));
				if(bKeyReleaseConnected)
					disconnect(systemKeyCapture->instance(), SIGNAL(keyReleasedSignal(quint32)), this, SLOT(recieveThreadKeyReleasedHandler(quint32)));
			}
			emit recieveThreadStopped(QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()));
			systemKeyCapture->setConnected(false, bForwardKeyEvents);
		}
		delete systemKeyCapture;
		systemKeyCapture = NULL;
	}	
	isRunning = false;
	return;
}

void keyboardCaptureThread::recieveThreadKeyPressedHandler(quint32 nKeycode)
{
	if(lMaskedKeys.isEmpty() == false)
	{
		if(lMaskedKeys.contains(nKeycode) == false)
		{
			if((bForwardKeyEvents) && systemKeyCapture)
			{
				//systemKeyCapture->instance()->keyboardProcedure_Main(49,WM_KEYDOWN,NULL);
				//CallNextHookEx( HHOOK, nCode, wParam, lParam );
			}
			return;
		}
	}
	emit recieveThreadKeyPressed(nKeycode);
}

void keyboardCaptureThread::recieveThreadKeyReleasedHandler(quint32 nKeycode)
{
	if(lMaskedKeys.isEmpty() == false)
	{
		if(lMaskedKeys.contains(nKeycode) == false)
		{
			//return;
		}
	}
	emit recieveThreadKeyReleased(nKeycode);
}
