//BrainStim
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


/****************************************************************************
**
****************************************************************************/

#include <QApplication>
#include <QString>
#include <QtWidgets>
#ifndef QT_NO_OPENGL
#include <QGLFormat>
#endif
#include <iostream>
#include "mainwindow.h"
#include "mainappinfo.h"
#include "mainappexchange.h"
#include "logindialog.h"

using namespace std;

class MainWindow;

void centerWidget(QWidget *w, bool useSizeHint)
{
	if (w->isFullScreen())
		return;
	QSize size;
	if (useSizeHint)
		size = w->sizeHint();
	else
		size = w->size();
	QDesktopWidget *d = QApplication::desktop();
	int ws = d->width();   // returns screen width
	int h = d->height();  // returns screen height
	int mw = size.width();
	int mh = size.height();
	int cw = (ws / 2) - (mw / 2);
	int ch = (h / 2) - (mh / 2);
	w->move(cw, ch);
}

int main(int argc, char **argv)
{
// Check windows
#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

	QProcess *process = new QProcess();
	QString sDebugMode;		//Release mode
	QString sArchitecture;	//Win32 architecture
	QStringList lDebugMessages;
	int nRetVal = 0;

#ifdef _DEBUG
	sDebugMode = "d";
#else
	sDebugMode = "r";
#endif
#ifdef ENVIRONMENT32
	sArchitecture = "1";
#else
	sArchitecture = "2";
#endif
	QString sConfigureFileName = "AutoStartConfigure.bat";
	QString sConfigureFilePath = QDir::currentPath() + QDir::separator() + sConfigureFileName;
	if (QFile(sConfigureFilePath).exists())
	{
		process->start(sConfigureFileName, QStringList() << sArchitecture << sDebugMode << QT_VERSION_STR);
		if(process->waitForStarted())
		{ 
			if (process->waitForFinished())
			{
				lDebugMessages.append(QString(__FUNCTION__) + ": " + sConfigureFileName + ", output: \n" + process->readAll());
				qDebug() << lDebugMessages.last();
				while(process->waitForReadyRead())
				{
					lDebugMessages.append(QString(__FUNCTION__) + ": " + process->readAll());
					qDebug() << lDebugMessages.last();
				}
			}
		}
	}
	else
	{
		lDebugMessages.append(QString(__FUNCTION__) + ": " + "Configuration file not found: " + sConfigureFileName);
		qDebug() << lDebugMessages.last();
	}

	bool bProceed = true;
	MainAppExchange appExchange(argc, argv, MAIN_PROGRAM_SHARED_MEM_KEY);

	bool bFirstUserInitialization = false;
	QVariant tmpVariant;
	GlobalApplicationInformation *globAppInformation = appExchange.getGlobalAppInformationObjectPointer(bFirstUserInitialization);
	//Now the BrainStim.ini file is initialized/created if it did not yet exist.
	QString sConfigurationFilePath = globAppInformation->getConfigurationFilePath();
	if (bFirstUserInitialization)
	{
		lDebugMessages.append(QString(__FUNCTION__) + ": " + "First time this account initializes BrainStim: " + sConfigurationFilePath);
		qDebug() << lDebugMessages.last();
	}
	
	if (argc >= 2)
	{
		QString sArg = QString(argv[1]);
		if (sArg == "-i" || sArg == "-I")//only initialization, to make sure INI file exists?
			goto EXITAPP;
	}

	if (globAppInformation->getSettingsInformation(REGISTRY_ENABLECUSTOMUSERLOGINS, tmpVariant) == true)
	{
		if(tmpVariant.toBool() == true)
		{
			QStringList lRegisteredUsers;
			QList<QByteArray> lRegisteredPasswordHashes;
			globAppInformation->getRegisteredUserCredentials(lRegisteredUsers, lRegisteredPasswordHashes);
			LoginDialog* loginDialog = new LoginDialog(NULL);
			lRegisteredUsers.sort(Qt::CaseInsensitive);
			QList <LoginDialog::strcUserCredential> lUserCredentials;
			for (int i = 0; i < lRegisteredUsers.count(); i++)
			{
				LoginDialog::strcUserCredential tmpUserCredential;
				tmpUserCredential.sUserName = lRegisteredUsers.at(i);
				tmpUserCredential.baPasswordHash = lRegisteredPasswordHashes.at(i);
				lUserCredentials.append(tmpUserCredential);
			}
			loginDialog->insertUsers(lUserCredentials);
			loginDialog->exec();
			QString sLogonName = "";
			QByteArray baLogonHashedPass;
			loginDialog->getCurrentUserCredentials(sLogonName, baLogonHashedPass);
			//bool bResult = 
				globAppInformation->setCurrentUserCredentials(sLogonName, baLogonHashedPass);
		}
	}

	bool bDoAllowMultipleInstances = false;
	if (argc >= 2)
	{
		QString tempStr = "";
		for (int i = 1; i < argc; i++)
		{
			tempStr = argv[i];
			if ((tempStr == "-m") || (tempStr == "-M"))//allow multiple instances?
			{
				bDoAllowMultipleInstances = true;
				break;
			}
			else if ((tempStr == "-o") || (tempStr == "-O"))//options defined?
			{
				if (i < (argc - 1))//another argument available?
				{
					i = i + 1;
					tempStr = argv[i];
					GlobalApplicationInformation::MainProgramModeFlags flags = GlobalApplicationInformation::Default;
					flags = (flags | GlobalApplicationInformation::MainProgramModeFlag(tempStr.toInt()));
					if (flags & GlobalApplicationInformation::AllowMultipleInstances)
					{
						bDoAllowMultipleInstances = true;
						break;
					}
				}
			}
		}
	}

	if (appExchange.isRunning())
	{
		appExchange.sendMessage("Log(\"New BrainStim Instance Initializing...\");");
		//if (appExchange.getSharedDataSegment("AllowMultipleInstance") == "false")
		QVariant tmpVariant;
		if ((globAppInformation->getSettingsInformation(REGISTRY_ALLOWMULTIPLEINHERITANCE, tmpVariant) == false) && (bDoAllowMultipleInstances == false))
			bProceed = false;
		if ((tmpVariant == false) && (bDoAllowMultipleInstances == false))
		{
			QStringList sFilesToOpen;
			int i;
			bool bExecuteDocument = false;
			if (argc > 2)
			{
				QString tempStr = "";
				for (i = 1; i<argc;i++)
				{
					tempStr = argv[i];
					if ((tempStr == "-f") || (tempStr == "-F"))//path exists?
					{				
						if (i<(argc-1))//another argument available?
						{
							i = i + 1;
							sFilesToOpen.append(QString(argv[i]).split(";"));//separate multiple files (cmd) using a ';' Character!
						}
					}
					else if ((tempStr == "-e") || (tempStr == "-E"))//Immediately execute the loaded (-f <document) document?
					{
						bExecuteDocument = true;
					}
				}
			}
			else if (argc == 2)//only path declared!
			{
				sFilesToOpen.append(QString(argv[1]).split(";"));
			}
			if(sFilesToOpen.isEmpty() == false)
			{
				for(i=0;i<sFilesToOpen.count();i++)
				{
					sFilesToOpen[i] = "\"" + QDir::cleanPath(sFilesToOpen.at(i)) + "\"";					
				}
				//BrainStim.openFiles(null,["E:/temp/1.txt","E:/temp/2.txt"]);
				QString sFileCombination = sFilesToOpen.join(",");
				appExchange.sendMessage("BrainStim.openFiles(null,[" + sFileCombination  + "]);");
			}
			appExchange.sendMessage("BrainStim.activateMainWindow();");
			if(bExecuteDocument)
				appExchange.sendMessage("BrainStim.executeActiveDocument();");
			bProceed = false;
		}
	}
	//else//First occurrence
	//{
	//	//globAppInformation->setMainAppInformationStructure();
	//}

	if(bProceed)
	{
		Q_INIT_RESOURCE(brainstim);
		MainWindow *appWindow = new MainWindow();
		appWindow->appendDebugMessages(lDebugMessages);
		appWindow->setGlobalApplicationInformationObject(globAppInformation);
		MainAppInfo::setMainWindow(appWindow);
		GlobalApplicationInformation::MainProgramModeFlags flags = GlobalApplicationInformation::Default;
		QString tempStr = "";
		if (argc > 2)
		{
			for (int i = 1; i<argc;i++)
			{
				tempStr = argv[i];
				if ((tempStr == "-f") || (tempStr == "-F"))//path exists?
				{				
					if (i<(argc-1))//another argument available?
					{
						i = i + 1;
						appWindow->setStartupFiles(argv[i]);//separate multiple files (cmd) using a ';' Character!
					}
				}
				else if ((tempStr == "-m") || (tempStr == "-M"))//allow multiple instances?
				{
					//see above handling
				}
				else if ((tempStr == "-o") || (tempStr == "-O"))//valid argument?
				{
					if (i<(argc-1))//another argument available?
					{
						i = i + 1;
						tempStr = argv[i];					
						flags = (flags | GlobalApplicationInformation::MainProgramModeFlag(tempStr.toInt()));
					}				
				}
				else if ((tempStr == "-v") || (tempStr == "-V"))//verbose mode?
				{
					flags = (flags | GlobalApplicationInformation::VerboseMode);
				}
				else if ((tempStr == "-e") || (tempStr == "-E"))//Immediately execute the loaded (-f <document) document?
				{
					flags = (flags | GlobalApplicationInformation::ExecuteDocument);
				}
			}
		}
		else if (argc == 2)//only one parameter defined!
		{
			tempStr = argv[1];
			if ((tempStr == "-v") || (tempStr == "-V"))//verbose mode?
			{
				flags = (flags | GlobalApplicationInformation::VerboseMode);
			}
			else if ((tempStr == "-m") || (tempStr == "-M"))//allow multiple instances?
			{
				//see above handling
			}
			//else if (tempStr == "-e" | tempStr == "-E")//Immediately execute the loaded (-f <document) document? --> doesn't work here because there's no file loaded, too few parameters
			//{
				//flags = (flags | GlobalApplicationInformation::ExecuteDocument);
			//}
			else
			{
				appWindow->setStartupFiles(argv[1]);
			}			
		}
		if (appExchange.isRunning())
			flags = flags | GlobalApplicationInformation::DisableNetworkServer;
		if (bFirstUserInitialization)
			flags = flags | GlobalApplicationInformation::UserFirstTimeInit;

		appWindow->initialize(flags);
		// connect message queue to the main window.
		bool bConnectResult = false;
		bConnectResult = QObject::connect(&appExchange, SIGNAL(messageAvailable(QString)), appWindow, SLOT(receiveExchangeMessage(QString)));
		bConnectResult = QObject::connect(appWindow, SIGNAL(TerminateAppExchange()), &appExchange, SLOT(stopRunning()));
		appWindow->show();//showMaximized() creates weird behaviour!;
		centerWidget(appWindow, false);
		appWindow->recoverLastScreenWindowSettings();
		appWindow->preExecuteTask();
		nRetVal = appExchange.exec();
		delete appWindow;
		appWindow = NULL;
	}
EXITAPP:
	return nRetVal;
}
