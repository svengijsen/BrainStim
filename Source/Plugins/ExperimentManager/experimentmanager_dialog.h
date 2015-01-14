//ExperimentManagerplugin
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


//This file implements the UI functionality

#ifndef ExperimentManager_DIALOG_H
#define ExperimentManager_DIALOG_H

#include <QDialog>
#include "ui_ExperimentManager_dialog.h"
#include <QtWidgets>
#include <QThread>

#include "qml2viewer.h"
//#include "ContainerDlg.h"
#include "globalapplicationinformation.h"

class ExperimentManager_Dialog : public QDialog, private Ui::ExperimentManager_DialogClass
{
	Q_OBJECT

public:
	ExperimentManager_Dialog(QWidget *parent = 0);
	~ExperimentManager_Dialog();

	bool setContentToExecute(GlobalApplicationInformation::DocContentInfoStructure docContentStruct);
	bool executeActiveDocument();

protected:
	void closeEvent(QCloseEvent *event);
	//void showEvent (QShowEvent * event);

private:
	Ui::ExperimentManager_DialogClass ui;
	void connectSignalSlots(bool bDisconnect = false);
	void cleanUp();
	
private slots:
	void on_cancelButton_clicked();
	void exampleButton_Pressed();
	void exampleButton_2_Pressed();
	void exampleButton_3_Pressed();
	void exampleButton_4_Pressed();
	void exampleButton_5_Pressed();
	void on_okButton_clicked();

	void changeExperimentSubObjectState(ExperimentSubObjectState nState);
	void ExperimentStateHasChanged(int nExperimentMainState, QString timeTextStamp);
	void LogMessage(QString sMessage);

private:
	QML2Viewer *Qml2ViewerObject;
	ExperimentManager *ExperimentManagerObj;
	//ContainerDlg *tmpContainerDlg;
	//QVBoxLayout *tmpLayout;
	ExperimentSubObjectState currentExperimentSubObjectState;
	ExperimentManager::ExperimentState currentExperimentState;
	GlobalApplicationInformation::DocContentInfoStructure docContentStructToRun;
};

#endif // ExperimentManager_DIALOG_H
