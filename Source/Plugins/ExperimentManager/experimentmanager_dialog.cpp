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


#include "experimentmanager_dialog.h"
#include "globalapplicationinformation.h"

ExperimentManager_Dialog::ExperimentManager_Dialog(QWidget *parent)	: QDialog(parent)
{
	currentExperimentSubObjectState = Experiment_SubObject_Constructing;
	Qml2ViewerObject = NULL;
	ExperimentManagerObj = NULL;
	
	ui.setupUi(this);
	//connect(ui.btnExampleButton, SIGNAL(clicked()), this, SLOT(exampleButton_Pressed()));
	//connect(ui.btnExampleButton_2, SIGNAL(clicked()), this, SLOT(exampleButton_2_Pressed()));
	//connect(ui.btnExampleButton_3, SIGNAL(clicked()), this, SLOT(exampleButton_3_Pressed()));
	//connect(ui.btnExampleButton_4, SIGNAL(clicked()), this, SLOT(exampleButton_4_Pressed()));
	//connect(ui.btnExampleButton_5, SIGNAL(clicked()), this, SLOT(exampleButton_5_Pressed()));
	currentExperimentSubObjectState = Experiment_SubObject_Initialized;
	currentExperimentState = ExperimentManager::ExperimentManager_NoState;
}

ExperimentManager_Dialog::~ExperimentManager_Dialog()
{
	currentExperimentSubObjectState = Experiment_SubObject_Destructing;
	cleanUp();
}

void ExperimentManager_Dialog::closeEvent(QCloseEvent *event)
{
	cleanUp();
	event->accept();//or event->ignore();
}

void ExperimentManager_Dialog::on_okButton_clicked()
{
	cleanUp();
	accept();
}

void ExperimentManager_Dialog::on_cancelButton_clicked()
{
	cleanUp();
	reject();
}

void ExperimentManager_Dialog::cleanUp()
{
	docContentStructToRun.strDocContent = "";
	docContentStructToRun.strDocExtension = "";
	docContentStructToRun.strDocHomeDir = "";
	docContentStructToRun.bIsFile = true;
	if(ExperimentManagerObj)
	{
		if ((currentExperimentState == ExperimentManager::ExperimentManager_IsStarting) || (currentExperimentState == ExperimentManager::ExperimentManager_Started))
		{
			ExperimentManagerObj->abortExperiment();
			int nRetries = 10;
			while(currentExperimentState != ExperimentManager::ExperimentManager_Stopped)
			{
				qApp->processEvents(QEventLoop::ExcludeSocketNotifiers,50);
				nRetries--;
				if (nRetries == 0)
				{
					break;//Could not stop the experiment....
				}
			}
			nRetries = nRetries;
		}		
	}
	connectSignalSlots(true);
	if(ExperimentManagerObj)
	{
		delete ExperimentManagerObj;
		ExperimentManagerObj = NULL;
	}
	if(Qml2ViewerObject)
	{
		Qml2ViewerObject->stopObject();
		delete Qml2ViewerObject;
		Qml2ViewerObject = NULL;
	}
	return;
}

bool ExperimentManager_Dialog::setContentToExecute(GlobalApplicationInformation::DocContentInfoStructure docContentStruct)
{
	docContentStructToRun = docContentStruct;
	return true;
}

void ExperimentManager_Dialog::connectSignalSlots(bool bDisconnect)
{
	//void UserWantsToClose(void);
	//void ObjectShouldStop(void);
	//void ObjectStateHasChanged(ExperimentSubObjectState);
	//void NewInitBlockTrial();
	//void ExternalTriggerIncremented(int);
	//void ExperimentStructureChanged(int,int,int);//Block,Trial,InternalTrigger

	if (bDisconnect)
	{
		if(Qml2ViewerObject)
		{
			disconnect(Qml2ViewerObject, &QML2Viewer::UserWantsToClose, Qml2ViewerObject, &QML2Viewer::abortExperimentObject);
			disconnect(Qml2ViewerObject, &QML2Viewer::ObjectStateHasChanged, this, &ExperimentManager_Dialog::changeExperimentSubObjectState);
		}
		if(ExperimentManagerObj)
		{
			disconnect(ExperimentManagerObj, &ExperimentManager::ExperimentStateHasChanged, this, &ExperimentManager_Dialog::ExperimentStateHasChanged);
			disconnect(ExperimentManagerObj, &ExperimentManager::WriteToLogOutput, this, &ExperimentManager_Dialog::LogMessage);
		}
	}
	else
	{
		if(Qml2ViewerObject)
		{
			connect(Qml2ViewerObject, &QML2Viewer::UserWantsToClose, Qml2ViewerObject, &QML2Viewer::abortExperimentObject);
			connect(Qml2ViewerObject, &QML2Viewer::ObjectStateHasChanged, this, &ExperimentManager_Dialog::changeExperimentSubObjectState);
		}
		if(ExperimentManagerObj)
		{
			connect(ExperimentManagerObj, &ExperimentManager::ExperimentStateHasChanged, this, &ExperimentManager_Dialog::ExperimentStateHasChanged);
			connect(ExperimentManagerObj, &ExperimentManager::WriteToLogOutput, this, &ExperimentManager_Dialog::LogMessage);
		}
	}	
}

void ExperimentManager_Dialog::LogMessage(QString sMessage)
{
	QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, sMessage));
}

void ExperimentManager_Dialog::ExperimentStateHasChanged(int nExperimentMainState, QString timeTextStamp)
{
	Q_UNUSED(nExperimentMainState);
	Q_UNUSED(timeTextStamp);
	//currentExperimentState = (ExperimentManager::ExperimentState)nExperimentMainState;
	//if (currentExperimentState == ExperimentManager::ExperimentManager_Constructed)
	//{

	//} 
	//else if (currentExperimentState == ExperimentManager::ExperimentManager_Initialized)
	//{

	//}
}

void ExperimentManager_Dialog::changeExperimentSubObjectState(ExperimentSubObjectState nState)
{
	if(nState == Experiment_SubObject_Abort)
	{
		if (currentExperimentSubObjectState != Experiment_SubObject_IsAborting)//Multiple Abort events could occur, catch only first one
		{
			if (Qml2ViewerObject)
				Qml2ViewerObject->deleteLater();
			currentExperimentSubObjectState = Experiment_SubObject_IsAborting;
		}
	}
	else if(nState == Experiment_SubObject_Stop)
	{
		if (currentExperimentSubObjectState != Experiment_SubObject_IsStopping)//Multiple Stop events could occur, catch only first one
		{
			if (Qml2ViewerObject)
				Qml2ViewerObject->deleteLater();
			currentExperimentSubObjectState = Experiment_SubObject_IsStopping;
		}
	}
	else if(nState == Experiment_SubObject_Destructing)
	{
		currentExperimentSubObjectState = Experiment_SubObject_Destructing;
		Qml2ViewerObject = NULL;
		currentExperimentSubObjectState = Experiment_SubObject_Stopped;		
	}
	else//All other cases
	{
		currentExperimentSubObjectState = nState;
	}
}

bool ExperimentManager_Dialog::executeActiveDocument()
{
	if (((currentExperimentSubObjectState == Experiment_SubObject_Initialized) || (currentExperimentSubObjectState == Experiment_SubObject_Stopped)) == false)
		return false;

	QString fileSource = "";
	if(docContentStructToRun.strDocExtension == PLUGIN_EXMLDOC_EXTENSION)
	{
		if(docContentStructToRun.bIsFile)
		{
			if(docContentStructToRun.strDocContent.isEmpty())
			{
				fileSource = QFileDialog::getOpenFileName(NULL, tr("Open a Experiment file"), MainAppInfo::appExampleDirPath(), tr("Experiment Files (*.exml);;Any file (*)"));
			}
			fileSource = docContentStructToRun.strDocContent;
			if (fileSource.isEmpty())
				return false;
			QFile file(fileSource);
			if (!file.exists())
				return false;
			if(file.open(QIODevice::ReadOnly))
			{
				fileSource = file.readAll();
				file.close();
			}
			else
			{
				return false;
			}
			QFileInfo fi(file);
			QDir::setCurrent(fi.canonicalPath());	
		}
		else
		{
			fileSource = docContentStructToRun.strDocContent;
			//QString dd = MainAppInfo::appDirPath();
			QDir::setCurrent(docContentStructToRun.strDocHomeDir);
		}
		ExperimentManagerObj = new ExperimentManager(this);
		connectSignalSlots(false);
		if(ExperimentManagerObj->loadExperiment(fileSource,docContentStructToRun.bIsFile))
		{
			return ExperimentManagerObj->runExperiment();
		}
		else
		{
			LogMessage("Error: Could not execute the document!");
		}
	}
	else if (docContentStructToRun.strDocExtension == PLUGIN_QMLDOC_EXTENSION)
	{
		if(docContentStructToRun.bIsFile)
		{
			if(docContentStructToRun.strDocContent.isEmpty())
			{
				fileSource = QFileDialog::getOpenFileName(NULL, tr("Open a QtQuick File"), MainAppInfo::appExampleDirPath(), tr("QtQuick Files (*.qml);;Any file (*)"));
			}
			fileSource = docContentStructToRun.strDocContent;
			if (fileSource.isEmpty())
				return false;
			QFile file(fileSource);
			if (!file.exists())
				return false;

			if(file.open(QIODevice::ReadOnly))
			{
				fileSource = file.readAll();
				file.close();
			}
			else
			{
				return false;
			}
			QFileInfo fi(file);
			QDir::setCurrent(fi.canonicalPath());			
		}
		else
		{
			fileSource = docContentStructToRun.strDocContent;
			//QString dd = MainAppInfo::appDirPath();
			QDir::setCurrent(docContentStructToRun.strDocHomeDir);
		}

		if (fileSource.isEmpty() == false)
		{
			QRegExp tmpRegExp;
			tmpRegExp.setPatternSyntax(QRegExp::Wildcard);
			tmpRegExp.setCaseSensitivity(Qt::CaseInsensitive);
			tmpRegExp.setPattern("import QtQuick 1.");

			if(fileSource.contains(tmpRegExp))
			{
				qWarning() << __FUNCTION__ << "::QtQuick version 1.* is not supported anymore, please update the *.qml document to QtQuick version 2.* or later!";
				return false;
			}
			else
			{
				tmpRegExp.setPattern("import QtQuick 2.");
				if(fileSource.contains(tmpRegExp))
				{
					if(Qml2ViewerObject)
					{
						Qml2ViewerObject->stopObject();
						delete Qml2ViewerObject;
						Qml2ViewerObject = NULL;
					}
					Qml2ViewerObject = new QML2Viewer(this);
					connectSignalSlots(false);
					if(Qml2ViewerObject->executeQML2Document(docContentStructToRun.strDocContent,docContentStructToRun.bIsFile))
					{
						Qml2ViewerObject->startObject();
						return true;
					}
					return false;
				}
				else
				{
					qWarning() << __FUNCTION__ << "::Could not determine QtQuick version, no \"import QtQuick\" statement found!";
					return false;
				}
			}
		}
	}
	return false;
}

void ExperimentManager_Dialog::exampleButton_Pressed()
{
	docContentStructToRun.strDocContent = "";
	docContentStructToRun.bIsFile = true;
	docContentStructToRun.strDocExtension = PLUGIN_EXMLDOC_EXTENSION;
	executeActiveDocument();
}

void ExperimentManager_Dialog::exampleButton_2_Pressed()
{
	docContentStructToRun.strDocContent = "";
	docContentStructToRun.bIsFile = true;
	docContentStructToRun.strDocExtension = PLUGIN_QMLDOC_EXTENSION;
	executeActiveDocument();
}

void ExperimentManager_Dialog::exampleButton_3_Pressed()
{
	
}

void ExperimentManager_Dialog::exampleButton_4_Pressed()
{
	
}

void ExperimentManager_Dialog::exampleButton_5_Pressed()
{

}


