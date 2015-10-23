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

#include "updatemngr.h"
#include "../BrainStim/maindefines.h"
#include "../BrainStim/installationManagerBase.h"
#include "../BrainStim/mainappinfo.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <QSettings>

bool getSettingsInformation(QSettings &settingsObj, const QString &sName, QVariant &vCurrentValue)
{
	bool bRetval = false;
	QStringList lChilgGroups = settingsObj.childGroups();
	if (lChilgGroups.contains(REGISTRY_SETTINGS_SYSTEM))
	{
		settingsObj.beginGroup(QString(REGISTRY_SETTINGS_SYSTEM));
		bRetval = settingsObj.contains(sName);
		if (bRetval)
			vCurrentValue = settingsObj.value(sName);
		else
			vCurrentValue = NULL;
		settingsObj.endGroup();
	}
	return bRetval;
}

void appendLogMessage(const QString &sMessage, bool &bErrorPassed, const installationManagerBase::MessageType nMessageType = installationManagerBase::Informative, QList<installationManagerBase::strcMessage>* lMessages = NULL)
{
	qDebug() << sMessage;
	if (lMessages)
	{
		installationManagerBase::strcMessage tmpMessageStruct;
		tmpMessageStruct.sMessage = sMessage;
		tmpMessageStruct.nMessageType = nMessageType;
		if (nMessageType == installationManagerBase::Failed)
			bErrorPassed = true;
		lMessages->append(tmpMessageStruct);
	}
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
	QString sDebugMode;		//Build mode
	QString sArchitecture;	//Architecture
	QList<installationManagerBase::strcMessage> lDebugMessages;
	bool bOperationHasFailed = false;
	QApplication a(argc, argv);
	UpdateMngr w;
	QString sLogFilePath = "";
	bool bAutoCloseWhenSuccessfullyFinished = false;

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
	QString sConfigureFileName = "AutoStartConfigure_UpdateMngr.bat";
	QString sConfigureFilePath = a.applicationDirPath() + QDir::separator() + sConfigureFileName;
	if (QFile(sConfigureFilePath).exists())
	{
		appendLogMessage(sConfigureFilePath + " found", bOperationHasFailed, installationManagerBase::Succeeded, &lDebugMessages);
		process->start(sConfigureFileName, QStringList() << sArchitecture << sDebugMode << QT_VERSION_STR);
		if (process->waitForStarted())
		{
			appendLogMessage(sConfigureFilePath + " started", bOperationHasFailed, installationManagerBase::Succeeded, &lDebugMessages);
			if (process->waitForFinished())
			{
				appendLogMessage(sConfigureFileName + " output: \n" + process->readAll(), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
				while (process->waitForReadyRead())
				{
					appendLogMessage(process->readAll(), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
				}
			}
		}
	}
	else
	{
		appendLogMessage(sConfigureFilePath + " not found", bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	QString sAlternativeMainProgramFileName = MAIN_PROGRAM_INTERNAL_NAME;
#ifdef ENVIRONMENT64
	sAlternativeMainProgramFileName = sAlternativeMainProgramFileName + "_x64";
#else
	sAlternativeMainProgramFileName = sAlternativeMainProgramFileName + "_Win32";
#endif
#ifdef _DEBUG
	sAlternativeMainProgramFileName = sAlternativeMainProgramFileName + "_Debug.exe";
#else
	sAlternativeMainProgramFileName = sAlternativeMainProgramFileName + "_Release.exe";
#endif
	//////////////////////////////////////////////////////////////////////////////////////////////
	QString sUserIniFilePath = "";
	//Is the configuration INI path provided?
	bool bErrorWithCustomIniFile = false;
	if (argc > 1)//arguments available?
	{
		QString tmpString;
		for (int i = 1; i < argc; i++)
		{
			tmpString = QString(argv[i]);
			if ((tmpString.toLower() == "-i") || (tmpString.toLower() == "-ini"))
			{
				appendLogMessage("Initialization file flag set", bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
				if ((i + 1) < argc)
				{
					tmpString = QString(argv[i+1]).trimmed();
					if (tmpString.isEmpty() == false)
					{
						sUserIniFilePath = tmpString;
						if (QFile(sUserIniFilePath).exists() == false)
						{
							appendLogMessage("Initialization file path does not exist: " + sUserIniFilePath, bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
							bErrorWithCustomIniFile = true;
						}
					}
				}
			}
		}
	}
	if (bErrorWithCustomIniFile == false)
	{
		if (QFile(sUserIniFilePath).exists() == false)
		{
#ifdef ENVIRONMENT64
			sUserIniFilePath = QDir::homePath() + "/" + MAIN_PROGRAM_INTERNAL_NAME + "(v" + MAIN_PROGRAM_FILE_VERSION_STRING + ", " + MAIN_PROGRAM_64BIT_STRING + ")" + "/" + MAIN_PROGRAM_INI_FILENAME;
#else
			sUserIniFilePath = QDir::homePath() + "/" + MAIN_PROGRAM_INTERNAL_NAME + "(v" + MAIN_PROGRAM_FILE_VERSION_STRING + ", " + MAIN_PROGRAM_32BIT_STRING + ")" + "/" + MAIN_PROGRAM_INI_FILENAME;
#endif
		}

		appendLogMessage("Searching for " + sUserIniFilePath + " file", bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
		//BrainStim(v1.0.0.1, 32-bits)
		if (QFile(sUserIniFilePath).exists() == false)
		{
			appendLogMessage("Initialization file(" + sUserIniFilePath + ") not yet found", bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
			QString sMainProgramFileName = MAIN_PROGRAM_ORIGINAL_FILENAME;
			if (QFile(sMainProgramFileName).exists() == false)
			{
				if (QFile(sAlternativeMainProgramFileName).exists() == false)
				{
					//qDebug() << __FUNCTION__ << "ERROR: " << "no *.ini file (" << sUserIniFilePath << ") or main program file (" << MAIN_PROGRAM_ORIGINAL_FILENAME << ") found, exiting...";
					appendLogMessage(QString("ERROR: No Initialization file(") + sUserIniFilePath + ") or main program file (" + MAIN_PROGRAM_ORIGINAL_FILENAME + ") found, exiting...", bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
					return 0;
				}
				sMainProgramFileName = sAlternativeMainProgramFileName;
			}
			appendLogMessage("Going to initialize BrainStim for the first time", bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
			QProcess *process = new QProcess();
			process->start(sMainProgramFileName, QStringList() << "-I");
			if (process->waitForStarted())
			{
				appendLogMessage("BrainStim started for the first time", bOperationHasFailed, installationManagerBase::Succeeded, &lDebugMessages);
				if (process->waitForFinished())
				{
					QString sTempString = process->readAll();
					if (sTempString.isEmpty() == false)
					{
						//qDebug() << __FUNCTION__ << MAIN_PROGRAM_ORIGINAL_FILENAME << ": " << process->readAll();
						appendLogMessage(QString(MAIN_PROGRAM_ORIGINAL_FILENAME) + " : " + process->readAll(), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
					}
					while (process->waitForReadyRead())
					{
						//qDebug() << __FUNCTION__ << MAIN_PROGRAM_ORIGINAL_FILENAME << ": " << process->readAll();
						appendLogMessage(QString(MAIN_PROGRAM_ORIGINAL_FILENAME) + " : " + process->readAll(), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
					}
				}
			}
			delete process;
			//Check again...
			if (QFile(sUserIniFilePath).exists() == false)
			{
				//qDebug() << __FUNCTION__ << "Could not locate *.ini file (" << sUserIniFilePath << "), exiting...";
				appendLogMessage(QString("Error: Could not locate Initialization file (") + sUserIniFilePath + "), exiting...", bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
				return 0;
			}
		}
		appendLogMessage("Initialization file(" + sUserIniFilePath + ") found", bOperationHasFailed, installationManagerBase::Succeeded, &lDebugMessages);

		//////////////////////////////////////////////////////////////////////////////////////////////
		QVariant tmpVariant;
		QString sCustomPluginRootDir = "";
		QString sDefaultPluginsRootDir = "";
		QSettings sSettingsObj(sUserIniFilePath, QSettings::IniFormat);
		if (getSettingsInformation(sSettingsObj, REGISTRY_USERPLUGINSROOTDIRECTORY, tmpVariant) == false)
		{
			//qDebug() << __FUNCTION__ << "Could not retrieve custom plugins root directory setting from *.ini file (" << sUserIniFilePath << "), exiting...";
			appendLogMessage(QString("Error: Could not retrieve custom plugins root directory setting from Initialization file (") + sUserIniFilePath + "), exiting...", bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
			return 0;
		}
		sCustomPluginRootDir = tmpVariant.toString();
		if (getSettingsInformation(sSettingsObj, REGISTRY_DEFAULTPLUGINSROOTDIRECTORY, tmpVariant) == false)
		{
			//qDebug() << __FUNCTION__ << "Could not retrieve default plugins root directory setting from *.ini file (" << sUserIniFilePath << "), exiting...";
			appendLogMessage(QString("Error: Could not retrieve default plugins root directory setting from Initialization file (") + sUserIniFilePath + "), exiting...", bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
			return 0;
		}
		sDefaultPluginsRootDir = tmpVariant.toString();
		if (getSettingsInformation(sSettingsObj, REGISTRY_MAINAPPUSERDIRECTORY, tmpVariant) == false)
		{
			//qDebug() << __FUNCTION__ << "Could not retrieve user documents root directory setting from *.ini file (" << sUserIniFilePath << "), exiting...";
			appendLogMessage(QString("Error: Could not retrieve user documents root directory setting from Initialization file (") + sUserIniFilePath + "), exiting...", bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
			return 0;
		}
		QString sUserAppRootDir = tmpVariant.toString();

		//////////////////////////////////////////////////////////////////////////////////////////////
		//Do we need to elevate?

		if (installationManagerBase::IsRunAsAdministrator() == false)
		{
			appendLogMessage(QString("Administrator mode is not active."), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
			/*
			QStringList lParams;
			if (argc > 1)//arguments available?
			{
			for (int i = 1; i < argc; i++)
			lParams << argv[i];
			}
			installationManagerBase::ExecuteUpdateMngr(true, lParams);
			return 0;
			*/
		}
		else
		{
			appendLogMessage(QString("Administrator mode is active."), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
		}


		//////////////////////////////////////////////////////////////////////////////////////////////
		if (argc > 1)//arguments available?
		{
			QString tmpArgString = "";
			for (int i = 1; i < argc; i++)
			{
				tmpArgString = tmpArgString + argv[i] + " ";
			}
			appendLogMessage(QString("Arguments passed:\n") + tmpArgString, bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);

			QString tempArgStr = "";
			tempArgStr = argv[1];
			if ((argc > 3) && ((tempArgStr.toLower() == "-p") || (tempArgStr.toLower() == "-plugin")))//plugin related and minimal two more arguments available?
			{
				QString sSourceFilePathsCombined = argv[2];//second argument should be the plugin *.ini or *.zip filepath
				QStringList lSourceFilePaths = sSourceFilePathsCombined.split(INSTALLMNGR_INSTALL_ARG_FILESEP, QString::SkipEmptyParts);
				if (lSourceFilePaths.isEmpty() == false)
				{
					bool bFirstSourceFile = true;
					foreach(QString sSingleSourceFilePath, lSourceFilePaths)
					{
						if (bOperationHasFailed == true)
							break;
						sSingleSourceFilePath = sSingleSourceFilePath.trimmed();
						if (sSingleSourceFilePath.isEmpty())
							continue;
						if (QFile(sSingleSourceFilePath).exists())
						{
							appendLogMessage(QString("Provided Plugin installation file (" + sSingleSourceFilePath + ") found"), bOperationHasFailed, installationManagerBase::Succeeded, &lDebugMessages);
							tempArgStr = argv[3];
							if (tempArgStr == "install")
							{
								if (bFirstSourceFile)
									appendLogMessage(QString("Going to install one or more Plugin(s)"), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
								bool bOverwriteExistingFiles = false;
								bool bRenameSourceInstallFileOnSuccess = false;
								if (argc > 4)//other flags/parameters available?
								{
									for (int i = 4; i < argc; i++)
									{
										tempArgStr = argv[i];
										if ((tempArgStr.toLower() == "-o") || (tempArgStr.toLower() == "-overwrite"))
										{
											if (bFirstSourceFile)
												appendLogMessage(QString("Overwrite flag enabled"), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
											bOverwriteExistingFiles = true;
										}
										else if ((tempArgStr.toLower() == "-r") || (tempArgStr.toLower() == "-rename"))
										{
											if (bFirstSourceFile)
												appendLogMessage(QString("Rename flag enabled"), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
											bRenameSourceInstallFileOnSuccess = true;
										}
										else if ((tempArgStr.toLower() == "-c") || (tempArgStr.toLower() == "-close"))
										{
											if (bFirstSourceFile)
												appendLogMessage(QString("(Auto) close flag enabled"), bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
											bAutoCloseWhenSuccessfullyFinished = true;
										}
										else if ((tempArgStr.toLower() == "-l") || (tempArgStr.toLower() == "-logfile"))
										{
											if (sLogFilePath.isEmpty() && bFirstSourceFile)
											{
												if ((i + 1) < argc)
												{
													tempArgStr = argv[i + 1];
													if (tempArgStr.isEmpty() == false)
													{
														sLogFilePath = tempArgStr;
														appendLogMessage(QString("Logfile defined: ") + sLogFilePath, bOperationHasFailed, installationManagerBase::Informative, &lDebugMessages);
													}
												}
											}
										}
									}
								}
								bool bRetval = false;
								QString sMessage;
								installationManagerBase::InstallResult resInstall = installationManagerBase::installPlugin(a.applicationDirPath(), sDefaultPluginsRootDir, sCustomPluginRootDir, sSingleSourceFilePath, sUserAppRootDir, bOverwriteExistingFiles, bRenameSourceInstallFileOnSuccess, &lDebugMessages);
								if (resInstall > 0)
								{
									appendLogMessage(QString("Installation succeeded (" + sSingleSourceFilePath + ")"), bOperationHasFailed, installationManagerBase::Succeeded, &lDebugMessages);
								}
								else
								{
									appendLogMessage(QString("Installation failed (" + sSingleSourceFilePath + ")"), bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
								}
							}
						}
						else
						{
							appendLogMessage(QString("Provided Plugin installation file (" + sSingleSourceFilePath + ") not found"), bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
						}
						bFirstSourceFile = false;
					}
				}
			}
		}
	}
	//Save the debug information if a valid log-file path is defined
	if (sLogFilePath.isEmpty() == false)
	{
		QFile fLogFile(sLogFilePath);
		if (!fLogFile.open(QIODevice::Append))
		{
			appendLogMessage(QString("Could not write/append to the provided log-file path (" + sLogFilePath + ")"), bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
			appendLogMessage(fLogFile.errorString(), bOperationHasFailed, installationManagerBase::Failed, &lDebugMessages);
		}
		else
		{
			QTextStream outLog(&fLogFile);
			foreach(installationManagerBase::strcMessageStructure msgStrc, lDebugMessages)
				outLog << msgStrc.sMessage << "\n";
			fLogFile.close();
		}
	}
	//Auto close if no error and auto close flag is enabled
	if (bAutoCloseWhenSuccessfullyFinished && (bOperationHasFailed==false))
		return 0;
	//Start the User Interface
	w.show();
	w.appendDebugMessages(lDebugMessages);
	return a.exec();
}
