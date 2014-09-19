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


#include "keyboard_dialog.h"

KeyBoard_Dialog::KeyBoard_Dialog(QWidget *parent)	: QDialog(parent)
{
	captureThread = NULL;
	ui.setupUi(this);
	captureThreadRunning = false;
}

KeyBoard_Dialog::~KeyBoard_Dialog()
{
	cleanUp();
}

void KeyBoard_Dialog::on_okButton_clicked()
{
	cleanUp();
	accept();
}

void KeyBoard_Dialog::on_cancelButton_clicked()
{
	cleanUp();
	reject();
}

void KeyBoard_Dialog::on_btnClear_clicked()
{
	ui.edtFeedBack->clear();
}

void KeyBoard_Dialog::cleanUp()
{
	stopCaptureThread();
	return;
}

void KeyBoard_Dialog::detectedKeyPress(quint32 Value)
{
	ui.edtFeedBack->appendPlainText(QString(Value) + " Pressed");
	UpdateCaptureThreadUI(Value);
}

void KeyBoard_Dialog::detectedKeyRelease(quint32 Value)
{
	ui.edtFeedBack->appendPlainText(QString(Value) + " Released");
	UpdateCaptureThreadUI(Value);
}

void KeyBoard_Dialog::UpdateCaptureThreadUI(quint32 Value)
{
	ui.txtFeedBackTriggVal->setText(QString::number(Value));
	ui.txt_NrOfFeedBacks->setText(QString::number((ui.txt_NrOfFeedBacks->text().toInt()+1)));
}

void KeyBoard_Dialog::CaptureThreadStarted(QString dateTimeStampText)
{
	ui.edtFeedBack->clear();
	captureThreadRunning = true;
	ui.rdb_CaptureThreadRunning->setChecked(true);
	ui.gb_CaptureThread->setEnabled(false);
	ui.txt_NrOfFeedBacks->setText("0");
	ui.txtFeedBackTriggVal->setText("");
	ui.btnStartCaptureThread->setEnabled(false);
	ui.txtFeedBackStopped->setText("");
	ui.txtFeedBackStarted->setText(dateTimeStampText);
}

void KeyBoard_Dialog::CaptureThreadStopped(QString dateTimeStampText)
{
	ui.rdb_CaptureThreadRunning->setChecked(false);
	ui.gb_CaptureThread->setEnabled(true);
	ui.btnStartCaptureThread->setEnabled(true);
	ui.txtFeedBackStopped->setText(dateTimeStampText);
	captureThreadRunning = false;
}

KeyBoardNameSpace::CaptureKeyMethod KeyBoard_Dialog::getCaptureKeyBoardMethod()
{
    if (ui.rbDecMethod_Pressed->isChecked())  
		return KeyBoardNameSpace::KeyPressed;
    else if (ui.rbDecMethod_Released->isChecked())  
		return KeyBoardNameSpace::KeyReleased;
    else if (ui.rbDecMethod_PressedReleased->isChecked())  
		return KeyBoardNameSpace::KeyPressedReleased;
	else
		return KeyBoardNameSpace::KeyPressed;
}

void KeyBoard_Dialog::on_btnStartCaptureThread_clicked()
{
	captureThread = new keyboardCaptureThread(getCaptureKeyBoardMethod(),this);
	connect(captureThread, SIGNAL(recieveThreadKeyPressed(quint32)), this, SLOT(detectedKeyPress(quint32)));
	connect(captureThread, SIGNAL(recieveThreadKeyReleased(quint32)), this, SLOT(detectedKeyRelease(quint32)));
	connect(captureThread, SIGNAL(recieveThreadStarted(QString)), this, SLOT(CaptureThreadStarted(QString)));
	connect(captureThread, SIGNAL(recieveThreadStopped(QString)), this, SLOT(CaptureThreadStopped(QString)));
	captureThread->start();
}

void KeyBoard_Dialog::on_btnStopCaptureThread_clicked()
{
	stopCaptureThread();
}

void KeyBoard_Dialog::stopCaptureThread()
{
	if (captureThread)
	{
		captureThread->stop();
		disconnect(captureThread, SIGNAL(recieveThreadKeyPressed(quint32)), this, SLOT(detectedKeyPress(quint32)));
		disconnect(captureThread, SIGNAL(recieveThreadKeyReleased(quint32)), this, SLOT(detectedKeyRelease(quint32)));
		disconnect(captureThread, SIGNAL(recieveThreadStarted(QString)), this, SLOT(CaptureThreadStarted(QString)));
		disconnect(captureThread, SIGNAL(recieveThreadStopped(QString)), this, SLOT(CaptureThreadStopped(QString)));
		delete captureThread;
		captureThread = NULL;
	}	
}

//void KeyBoard_Dialog::on_rbDecMethod_Pressed_toggled(bool)
//{
//	if (ui.rbDecMethod_Pressed->isChecked())
//	{
//	}
//}

//void KeyBoard_Dialog::on_rbDecMethod_Released_toggled(bool)
//{
//	if (ui.rbDecMethod_Released->isChecked())
//	{
//	}
//}

//void KeyBoard_Dialog::on_rbDecMethod_PressedReleased_toggled(bool)
//{
//	if (ui.rbDecMethod_PressedReleased->isChecked())
//	{
//	}
//}
