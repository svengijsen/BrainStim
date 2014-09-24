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

#include "systemkeyboardreadwrite.h"

//SystemKeyboardReadWrite::SystemKeyboardReadWrite(QObject *parent)
//	: QObject(parent)
//{
//
//}

//SystemKeyboardReadWrite::~SystemKeyboardReadWrite()
//{
//
//}

SystemKeyboardReadWrite::SystemKeyboardReadWrite() : QObject()
{
	keyboardHook = NULL;// Assign to null
	bOnlyForward = false;
}

void SystemKeyboardReadWrite::keyboardProcedure_Main(int nCode, WPARAM wParam, LPARAM lParam, bool bForwardKeys)
{	
	if (nCode == HC_ACTION)// Check for a key down press
	{
		//KeyBoardNameSpace::CaptureKeyMethod tmpMethod = instance()->captureMethod;
		if ((wParam == WM_KEYDOWN))// && ((tmpMethod == KeyBoardNameSpace::KeyPressed) || (tmpMethod == KeyBoardNameSpace::KeyPressedReleased)))
		{
			KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
			if(SystemKeyboardReadWrite::instance()->bOnlyForward == false)
				emit SystemKeyboardReadWrite::instance()->keyPressedSignal((quint32) pKeyboard->vkCode);
		}
		else if ((wParam == WM_KEYUP)) //&& ((tmpMethod == KeyBoardNameSpace::KeyReleased) || (tmpMethod == KeyBoardNameSpace::KeyPressedReleased)))
		{			
			KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
			if(SystemKeyboardReadWrite::instance()->bOnlyForward == false)
				emit SystemKeyboardReadWrite::instance()->keyReleasedSignal((quint32) pKeyboard->vkCode);
		}

		instance()->setConnected( false, bForwardKeys );
		instance()->setConnected( true, bForwardKeys );//reconnect
	}
	//return !SystemKeyboardReadWrite::getForwardKeyEvents();
	//CallNextHookEx( NULL, nCode, wParam, lParam );
	//return false; //this value causes the system to call the next hook procedure in the chain anyway, and it eventually gets processed by your default window procedure.
	//If you CallNextHook() the next hook in the chain is called. If you return non-zero the message is not received by the target window (thus preventing the window from 
	//receiving the keyboard message). If you return zero the message is received by the target window.
	//Your desired behavior is to return non-zero if the key pressed is 'a', and zero otherwise.
}

LRESULT CALLBACK SystemKeyboardReadWrite::keyboardProcedure_Forward(int nCode, WPARAM wParam, LPARAM lParam)
{	
	keyboardProcedure_Main(nCode, wParam, lParam, true);
	return false;
	//CallNextHookEx( NULL, nCode, wParam, lParam );
	//return false; //this value causes the system to call the next hook procedure in the chain anyway, and it eventually gets processed by your default window procedure.
	//If you CallNextHook() the next hook in the chain is called. If you return non-zero the message is not received by the target window (thus preventing the window from 
	//receiving the keyboard message). If you return zero the message is received by the target window.
	//Your desired behavior is to return non-zero if the key pressed is 'a', and zero otherwise.
}

LRESULT CALLBACK SystemKeyboardReadWrite::keyboardProcedure_NoForward(int nCode, WPARAM wParam, LPARAM lParam)
{	
	keyboardProcedure_Main(nCode, wParam, lParam, false);
	return true;
	//return !SystemKeyboardReadWrite::getForwardKeyEvents();
	//CallNextHookEx( NULL, nCode, wParam, lParam );
	//return false; //this value causes the system to call the next hook procedure in the chain anyway, and it eventually gets processed by your default window procedure.
	//If you CallNextHook() the next hook in the chain is called. If you return non-zero the message is not received by the target window (thus preventing the window from 
	//receiving the keyboard message). If you return zero the message is received by the target window.
	//Your desired behavior is to return non-zero if the key pressed is 'a', and zero otherwise.
}

bool SystemKeyboardReadWrite::connected()
{
	return keyboardHook;
}

bool SystemKeyboardReadWrite::setConnected(bool state, bool forwardKeyEvents)
{
	if(state && keyboardHook == NULL)
	{
		if (forwardKeyEvents)
		{
			keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProcedure_Forward, GetModuleHandle(NULL), 0);
		} 
		else
		{
			keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProcedure_NoForward, GetModuleHandle(NULL), 0);
		}
		return keyboardHook;
	}
	else
	{
		UnhookWindowsHookEx(keyboardHook);
		keyboardHook = NULL;
		return keyboardHook;
	}
}

SystemKeyboardReadWrite* SystemKeyboardReadWrite::instance()
{
	static SystemKeyboardReadWrite* pKeyboardReadWriteInstance = new SystemKeyboardReadWrite();    
	//pKeyboardReadWriteInstance->setCaptureMethod(captureMethod);
	return pKeyboardReadWriteInstance;
}