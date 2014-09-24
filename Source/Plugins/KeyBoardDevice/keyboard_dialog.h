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


#ifndef KEYBOARD_DIALOG_H
#define KEYBOARD_DIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "ui_keyboard_dialog.h"
#include <QtWidgets>
#include <QDialog>
#include "keyboardcapturethread.h"

class KeyBoard_Dialog : public QDialog, private Ui::KeyBoard_DialogClass
{
	Q_OBJECT

public:
	KeyBoard_Dialog(QWidget *parent = 0);
	~KeyBoard_Dialog();

private:
	Ui::KeyBoard_DialogClass ui;
	keyboardCaptureThread *captureThread;
	bool captureThreadRunning;
	KeyBoardNameSpace::CaptureKeyMethod getCaptureKeyBoardMethod();
	void stopCaptureThread();
	void cleanUp();

private slots:
	//void on_rbDecMethod_PressedReleased_toggled(bool);
	//void on_rbDecMethod_Released_toggled(bool);
	//void on_rbDecMethod_Pressed_toggled(bool);
	void on_btnStopCaptureThread_clicked();
	void on_btnStartCaptureThread_clicked();
	void on_cancelButton_clicked();
	void on_okButton_clicked();
	void on_btnClear_clicked();

	void detectedKeyPress(quint32 Value);
	void detectedKeyRelease(quint32 Value);
	void UpdateCaptureThreadUI(quint32 Value);
	void CaptureThreadStarted(QString dateTimeStampText);
	void CaptureThreadStopped(QString dateTimeStampText);
};

#endif // KEYBOARD_DIALOG_H
