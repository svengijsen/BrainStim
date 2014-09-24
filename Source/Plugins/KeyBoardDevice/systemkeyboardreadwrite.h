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

#ifndef SYSTEMKEYBOARDREADWRITE_H
#define SYSTEMKEYBOARDREADWRITE_H

#pragma once
#include <QObject>
#include <QDebug>
#include <Windows.h>
#include "global.h"

class SystemKeyboardReadWrite : public QObject
{
	Q_OBJECT

signals:	
	void keyPressedSignal(quint32 key);																		// Broadcasts a key has been pressed	
	void keyReleasedSignal(quint32 key);																	// Broadcasts a key has been released

public:
	SystemKeyboardReadWrite();																				// Class constructor
	static SystemKeyboardReadWrite * instance();															// Returns singleton instance
	bool connected();																						// Returns whether the keyboard hook is connected
	bool setConnected(bool state, bool forwardKeyEvents = true);											// Connects / Disconnects the keyboard hook

private:
	bool bOnlyForward;
	HHOOK keyboardHook;																						// Keyboard hook
	static LRESULT CALLBACK keyboardProcedure_Forward(int nCode, WPARAM wParam, LPARAM lParam);				// Identifies hook activity
	static LRESULT CALLBACK keyboardProcedure_NoForward(int nCode, WPARAM wParam, LPARAM lParam);			// Identifies hook activity
	static void keyboardProcedure_Main(int nCode, WPARAM wParam, LPARAM lParam, bool bForwardKeys);			// Identifies hook activity
};

#endif // SYSTEMKEYBOARDREADWRITE_H
