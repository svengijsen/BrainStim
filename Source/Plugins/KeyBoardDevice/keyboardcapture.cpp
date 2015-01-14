//KeyBoardplugin
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


#include "keyboardcapture.h"
#include <QPixmap>

Q_DECLARE_METATYPE(QPixmap*)

QScriptEngine * KeyBoardCapture::parentScriptEngine = NULL;//Initialize the static variable

KeyBoardCapture::KeyBoardCapture(QObject *parent) : QObject(parent)
{
	captureThread = NULL;
	parentScriptEngine = NULL;
	//setupKeyCodeTable();
}

KeyBoardCapture::~KeyBoardCapture()
{
	if (captureThread)
	{
		delete captureThread;
		captureThread = NULL;
	}
}

bool KeyBoardCapture::StartCaptureThread(const short method, bool keyForwarding, const QList<int> &lKeyList)
{ 
	KeyBoardNameSpace::CaptureKeyMethod CapDecMethod;
	if (method == 0) 
		CapDecMethod = KeyBoardNameSpace::KeyPressed;
	else if (method == 1) 
		CapDecMethod = KeyBoardNameSpace::KeyReleased;
	else 
		CapDecMethod = KeyBoardNameSpace::KeyPressedReleased;

	captureThread = new keyboardCaptureThread(CapDecMethod,this);
	captureThread->setKeyEventForwarding(keyForwarding);
	captureThread->setMaskedKeyList(lKeyList);

	connect(captureThread, SIGNAL(recieveThreadKeyPressed(quint32)), this, SIGNAL(CaptureThreadKeyPressed(quint32)));
	connect(captureThread, SIGNAL(recieveThreadKeyReleased(quint32)), this, SIGNAL(CaptureThreadKeyReleased(quint32)));
	connect(captureThread, SIGNAL(recieveThreadStarted(QString)), this, SIGNAL(CaptureThreadStarted(QString)));
	connect(captureThread, SIGNAL(recieveThreadStopped(QString)), this, SIGNAL(CaptureThreadStopped(QString)));
	captureThread->start();
	return true;
}

void KeyBoardCapture::StopCaptureThread()
{
	disconnect(this, SIGNAL(CaptureThreadKeyPressed(quint32)));
	disconnect(this, SIGNAL(CaptureThreadKeyReleased(quint32)));
	disconnect(this, SIGNAL(CaptureThreadStarted(QString)));
	disconnect(this, SIGNAL(CaptureThreadStopped(QString)));	
	captureThread->stop();
}

//QString KeyBoardCapture::getVirtualKeyString(const quint32 &keyCode)
//{
//	if (mKeyCodes.contains(keyCode))
//		return mKeyCodes.value(keyCode);
//	else
//		return QString("");
//	return QString("");
//}

void KeyBoardCapture::setupKeyCodeTable()
{
	mKeyCodes.clear();
	//mKeyCodes.insert("Left mouse button",0x01);//VK_LBUTTON
	//mKeyCodes.insert("Right mouse button",0x02);//VK_RBUTTON
	//mKeyCodes.insert("Control-break processing",0x03);//VK_CANCEL
	//mKeyCodes.insert("Middle mouse button (three-button mouse)",0x04);//VK_MBUTTON
	//mKeyCodes.insert("X1 mouse button",0x05);//VK_XBUTTON1
	//mKeyCodes.insert("X2 mouse button",0x06);//VK_XBUTTON2
	//mKeyCodes.insert("Undefined",0x07);//-
	//mKeyCodes.insert("BACKSPACE key",0x08);//VK_BACK
	//mKeyCodes.insert("TAB key",0x09);//VK_TAB
	//mKeyCodes.insert("Reserved",0x0A);//-
	//mKeyCodes.insert("Reserved",0x0B);//-
	//mKeyCodes.insert("CLEAR key",0x0C);//VK_CLEAR
	//mKeyCodes.insert("ENTER key",0x0D);//VK_RETURN
	//mKeyCodes.insert("Undefined",0x0E);//-
	//mKeyCodes.insert("Undefined",0x0F);//-
	//mKeyCodes.insert("SHIFT key",0x10);//VK_SHIFT
	//mKeyCodes.insert("CTRL key",0x11);//VK_CONTROL
	//mKeyCodes.insert("ALT key",0x12);//VK_MENU
	//mKeyCodes.insert("PAUSE key",0x13);//VK_PAUSE
	//mKeyCodes.insert("CAPS LOCK key",0x14);//VK_CAPITAL
	//mKeyCodes.insert("IME Hanguel mode (maintained for compatibility use)", 0x15);//VK_HANGUEL
	//mKeyCodes.insert("Undefined",0x16);//-
	//mKeyCodes.insert("IME Junja mode",0x17);//VK_JUNJA
	//mKeyCodes.insert("IME final mode",0x18);//VK_FINAL
	//mKeyCodes.insert("IME Hanja mode",0x19);//VK_HANJA
	//mKeyCodes.insert("IME Kanji mode",0x19);//VK_KANJI
	//mKeyCodes.insert("Undefined",0x1A);//-
	//mKeyCodes.insert("ESC key",0x1B);//VK_ESCAPE
	//mKeyCodes.insert("IME convert",0x1C);//VK_CONVERT
	//mKeyCodes.insert("IME nonconvert",0x1D);//VK_NONCONVERT
	//mKeyCodes.insert("IME accept",0x1E);//VK_ACCEPT
	//mKeyCodes.insert("IME mode change request",0x1F);//VK_MODECHANGE
	//mKeyCodes.insert("SPACEBAR",0x20);//VK_SPACE
	//mKeyCodes.insert("PAGE UP key",0x21);//VK_PRIOR
	//mKeyCodes.insert("PAGE DOWN key",0x22);//VK_NEXT
	//mKeyCodes.insert("END key",0x23);//VK_END
	//mKeyCodes.insert("HOME key",0x24);//VK_HOME
	//mKeyCodes.insert("LEFT ARROW key",0x25);//VK_LEFT
	//mKeyCodes.insert("UP ARROW key",0x26);//VK_UP
	//mKeyCodes.insert("RIGHT ARROW key",0x27);//VK_RIGHT
	//mKeyCodes.insert("DOWN ARROW key",0x28);//VK_DOWN
	//mKeyCodes.insert("SELECT key",0x29);//VK_SELECT
	//mKeyCodes.insert("PRINT key",0x2A);//VK_PRINT
	//mKeyCodes.insert("EXECUTE key",0x2B);//VK_EXECUTE
	//mKeyCodes.insert("PRINT SCREEN key",0x2C);//VK_SNAPSHOT
	//mKeyCodes.insert("INS key",0x2D);//VK_INSERT
	//mKeyCodes.insert("DEL key",0x2E);//VK_DELETE
	//mKeyCodes.insert("HELP key",0x2F);//VK_HELP
	mKeyCodes.insert(0x30,"0 key");//0 key
	//mKeyCodes.insert("0x31",1 key);//-
	//mKeyCodes.insert("0x32",2 key);//-
	//mKeyCodes.insert("0x33",3 key);//-
	//mKeyCodes.insert("0x34",4 key);//-
	//mKeyCodes.insert("0x35",5 key);//-
	//mKeyCodes.insert("0x36",6 key);//-
	//mKeyCodes.insert("0x37",7 key);//-
	//mKeyCodes.insert("0x38",8 key);//-
	//mKeyCodes.insert("0x39",9 key);//-
	//mKeyCodes.insert("0x3A",Undefined);//-
	//mKeyCodes.insert("0x3B",Undefined);//-
	//mKeyCodes.insert("0x3C",Undefined);//-
	//mKeyCodes.insert("0x3D",Undefined);//-
	//mKeyCodes.insert("0x3E",Undefined);//-
	//mKeyCodes.insert("0x3F",Undefined);//-
	//mKeyCodes.insert("0x40",Undefined);//-
	//mKeyCodes.insert("0x41",A key);//-
	//mKeyCodes.insert("0x42",B key);//-
	//mKeyCodes.insert("0x43",C key);//-
	//mKeyCodes.insert("0x44",D key);//-
	//mKeyCodes.insert("0x45",E key);//-
	//mKeyCodes.insert("0x46",F key);//-
	//mKeyCodes.insert("0x47",G key);//-
	//mKeyCodes.insert("0x48",H key);//-
	//mKeyCodes.insert("0x49",I key);//-
	//mKeyCodes.insert("0x4A",J key);//-
	//mKeyCodes.insert("0x4B",K key);//-
	//mKeyCodes.insert("0x4C",L key);//-
	//mKeyCodes.insert("0x4D",M key);//-
	//mKeyCodes.insert("0x4E",N key);//-
	//mKeyCodes.insert("0x4F",O key);//-
	//mKeyCodes.insert("0x50",P key);//-
	//mKeyCodes.insert("0x51",Q key);//-
	//mKeyCodes.insert("0x52",R key);//-
	//mKeyCodes.insert("0x53",S key);//-
	//mKeyCodes.insert("0x54",T key);//-
	//mKeyCodes.insert("0x55",U key);//-
	//mKeyCodes.insert("0x56",V key);//-
	//mKeyCodes.insert("0x57",W key);//-
	//mKeyCodes.insert("0x58",X key);//-
	//mKeyCodes.insert("0x59",Y key);//-
	//mKeyCodes.insert("0x5A",Z key);//-
	//mKeyCodes.insert("Left Windows key (Natural keyboard)",0x5B);//VK_LWIN
	//mKeyCodes.insert("Right Windows key (Natural keyboard)",0x5C);//VK_RWIN
	//mKeyCodes.insert("Applications key (Natural keyboard)",0x5D);//VK_APPS
	//mKeyCodes.insert("Reserved",0x5E);//-
	//mKeyCodes.insert("Computer Sleep key",0x5F);//VK_SLEEP
	//mKeyCodes.insert("Numeric keypad 0 key",0x60);//VK_NUMPAD0
	//mKeyCodes.insert("Numeric keypad 1 key",0x61);//VK_NUMPAD1
	//mKeyCodes.insert("Numeric keypad 2 key",0x62);//VK_NUMPAD2
	//mKeyCodes.insert("Numeric keypad 3 key",0x63);//VK_NUMPAD3
	//mKeyCodes.insert("Numeric keypad 4 key",0x64);//VK_NUMPAD4
	//mKeyCodes.insert("Numeric keypad 5 key",0x65);//VK_NUMPAD5
	//mKeyCodes.insert("Numeric keypad 6 key",0x66);//VK_NUMPAD6
	//mKeyCodes.insert("Numeric keypad 7 key",0x67);//VK_NUMPAD7
	//mKeyCodes.insert("Numeric keypad 8 key",0x68);//VK_NUMPAD8
	//mKeyCodes.insert("Numeric keypad 9 key",0x69);//VK_NUMPAD9
	//mKeyCodes.insert("Multiply key",0x6A);//VK_MULTIPLY
	//mKeyCodes.insert("Add key",0x6B);//VK_ADD
	//mKeyCodes.insert("Separator key",0x6C);//VK_SEPARATOR
	//mKeyCodes.insert("Subtract key",0x6D);//VK_SUBTRACT
	//mKeyCodes.insert("Decimal key",0x6E);//VK_DECIMAL
	//mKeyCodes.insert("Divide key",0x6F);//VK_DIVIDE
	//mKeyCodes.insert("F1 key",0x70);//VK_F1
	//mKeyCodes.insert("F2 key",0x71);//VK_F2
	//mKeyCodes.insert("F3 key",0x72);//VK_F3
	//mKeyCodes.insert("F4 key",0x73);//VK_F4
	//mKeyCodes.insert("F5 key",0x74);//VK_F5
	//mKeyCodes.insert("F6 key",0x75);//VK_F6
	//mKeyCodes.insert("F7 key",0x76);//VK_F7
	//mKeyCodes.insert("F8 key",0x77);//VK_F8
	//mKeyCodes.insert("F9 key",0x78);//VK_F9
	//mKeyCodes.insert("F10 key",0x79);//VK_F10
	//mKeyCodes.insert("F11 key",0x7A);//VK_F11
	//mKeyCodes.insert("F12 key",0x7B);//VK_F12
	//mKeyCodes.insert("F13 key",0x7C);//VK_F13
	//mKeyCodes.insert("F14 key",0x7D);//VK_F14
	//mKeyCodes.insert("F15 key",0x7E);//VK_F15
	//mKeyCodes.insert("F16 key",0x7F);//VK_F16
	//mKeyCodes.insert("F17 key",0x80);//VK_F17
	//mKeyCodes.insert("F18 key",0x81);//VK_F18
	//mKeyCodes.insert("F19 key",0x82);//VK_F19
	//mKeyCodes.insert("F20 key",0x83);//VK_F20
	//mKeyCodes.insert("F21 key",0x84);//VK_F21
	//mKeyCodes.insert("F22 key",0x85);//VK_F22
	//mKeyCodes.insert("F23 key",0x86);//VK_F23
	//mKeyCodes.insert("F24 key",0x87);//VK_F24
	//mKeyCodes.insert("Unassigned",0x88-8F);//-
	//mKeyCodes.insert("NUM LOCK key",0x90);//VK_NUMLOCK
	//mKeyCodes.insert("SCROLL LOCK key",0x91);//VK_SCROLL
	//mKeyCodes.insert("-",OEM specific);//0x92-96
	//mKeyCodes.insert("Unassigned",0x97-9F);//-
	//mKeyCodes.insert("Left SHIFT key",0xA0);//VK_LSHIFT
	//mKeyCodes.insert("Right SHIFT key",0xA1);//VK_RSHIFT
	//mKeyCodes.insert("Left CONTROL key",0xA2);//VK_LCONTROL
	//mKeyCodes.insert("Right CONTROL key",0xA3);//VK_RCONTROL
	//mKeyCodes.insert("Left MENU key",0xA4);//VK_LMENU
	//mKeyCodes.insert("Right MENU key",0xA5);//VK_RMENU
	//mKeyCodes.insert("Browser Back key",0xA6);//VK_BROWSER_BACK
	//mKeyCodes.insert("Browser Forward key",0xA7);//VK_BROWSER_FORWARD
	//mKeyCodes.insert("Browser Refresh key",0xA8);//VK_BROWSER_REFRESH
	//mKeyCodes.insert("Browser Stop key",0xA9);//VK_BROWSER_STOP
	//mKeyCodes.insert("Browser Search key",0xAA);//VK_BROWSER_SEARCH
	//mKeyCodes.insert("Browser Favorites key",0xAB);//VK_BROWSER_FAVORITES
	//mKeyCodes.insert("Browser Start and Home key",0xAC);//VK_BROWSER_HOME
	//mKeyCodes.insert("Volume Mute key",0xAD);//VK_VOLUME_MUTE
	//mKeyCodes.insert("Volume Down key",0xAE);//VK_VOLUME_DOWN
	//mKeyCodes.insert("Volume Up key",0xAF);//VK_VOLUME_UP
	//mKeyCodes.insert("Next Track key",0xB0);//VK_MEDIA_NEXT_TRACK
	//mKeyCodes.insert("Previous Track key",0xB1);//VK_MEDIA_PREV_TRACK
	//mKeyCodes.insert("Stop Media key",0xB2);//VK_MEDIA_STOP
	//mKeyCodes.insert("Play/Pause Media key",0xB3);//VK_MEDIA_PLAY_PAUSE
	//mKeyCodes.insert("Start Mail key",0xB4);//VK_LAUNCH_MAIL
	//mKeyCodes.insert("Select Media key",0xB5);//VK_LAUNCH_MEDIA_SELECT
	//mKeyCodes.insert("Start Application 1 key",0xB6);//VK_LAUNCH_APP1
	//mKeyCodes.insert("Start Application 2 key",0xB7);//VK_LAUNCH_APP2
	//mKeyCodes.insert("Reserved",0xB8-B9);//-
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard. For the US standard keyboard, the ';:' key);//0xBA;VK_OEM_1
	//mKeyCodes.insert("For any country/region, the '+' key",0xBB);//VK_OEM_PLUS
	//mKeyCodes.insert("For any country/region, the ',' key",0xBC);//VK_OEM_COMMA
	//mKeyCodes.insert("For any country/region, the '-' key",0xBD);//VK_OEM_MINUS
	//mKeyCodes.insert("For any country/region, the '.' key",0xBE);//VK_OEM_PERIOD
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard. For the US standard keyboard, the '/?' key);//0xBF;VK_OEM_2
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard. For the US standard keyboard, the '`~' key);//0xC0;VK_OEM_3
	//mKeyCodes.insert("Reserved",0xC1-D7);//-
	//mKeyCodes.insert("Unassigned",0xD8-DA);//-
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard. For the US standard keyboard, the '[{' key);//0xDB;VK_OEM_4
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard. For the US standard keyboard, the '\|' key);//0xDC;VK_OEM_5
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard. For the US standard keyboard, the ']}' key);//0xDD;VK_OEM_6
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key);//0xDE;VK_OEM_7
	//mKeyCodes.insert("Used for miscellaneous characters", it can vary by keyboard.);//0xDF;VK_OEM_8
	//mKeyCodes.insert("Reserved",0xE0);//-
	//mKeyCodes.insert("OEM specific",0xE1);//-
	//mKeyCodes.insert("Either the angle bracket key or the backslash key on the RT 102-key keyboard",0xE2);//VK_OEM_102
	//mKeyCodes.insert("OEM specific",0xE3-E4);//-
	//mKeyCodes.insert("IME PROCESS key",0xE5);//VK_PROCESSKEY
	//mKeyCodes.insert("OEM specific",0xE6);//-
	//mKeyCodes.insert("Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP",0xE7);//VK_PACKET
	//mKeyCodes.insert("Unassigned",0xE8);//-
	//mKeyCodes.insert("OEM specific",0xE9-F5);//-
	//mKeyCodes.insert("Attn key",0xF6);//VK_ATTN
	//mKeyCodes.insert("CrSel key",0xF7);//VK_CRSEL
	//mKeyCodes.insert("ExSel key",0xF8);//VK_EXSEL
	//mKeyCodes.insert("Erase EOF key",0xF9);//VK_EREOF
	//mKeyCodes.insert("Play key",0xFA);//VK_PLAY
	//mKeyCodes.insert("Zoom key",0xFB);//VK_ZOOM
	//mKeyCodes.insert("Reserved",0xFC);//VK_NONAME
	//mKeyCodes.insert("PA1 key",0xFD);//VK_PA1
	//mKeyCodes.insert("Clear key",0xFE);//VK_OEM_CLEAR
}