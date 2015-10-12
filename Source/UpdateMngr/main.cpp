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

#define MAIN_PROGRAM_NAME							"BrainStim"
#define MAIN_PROGRAM_FILENAME						MAIN_PROGRAM_NAME ".exe"
#define MAIN_PROGRAM_INI_FILENAME					MAIN_PROGRAM_NAME ".ini"

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
	QString sConfigureFilePath = QDir::currentPath() + QDir::separator() + sConfigureFileName;
	if (QFile(sConfigureFilePath).exists())
	{
		process->start(sConfigureFileName, QStringList() << sArchitecture << sDebugMode << QT_VERSION_STR);
		if (process->waitForStarted())
		{
			if (process->waitForFinished())
			{
				qDebug() << __FUNCTION__ << sConfigureFileName << "output: \n" << process->readAll();
				while (process->waitForReadyRead())
				{
					qDebug() << __FUNCTION__ << process->readAll();
				}
			}
		}
	}
	else
	{
		qDebug() << __FUNCTION__ << sConfigureFileName << "not found!";
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	QString sAlternativeMainProgramFileName = MAIN_PROGRAM_NAME;
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
#ifdef ENVIRONMENT64
	sUserIniFilePath = QDir::homePath() + "/" + MAIN_PROGRAM_INTERNAL_NAME + "(v" + MAIN_PROGRAM_FILE_VERSION_STRING + ", " + MAIN_PROGRAM_64BIT_STRING + ")" + "/" + MAIN_PROGRAM_INI_FILENAME;
#else
	sUserIniFilePath = QDir::homePath() + "/" + MAIN_PROGRAM_INTERNAL_NAME + "(v" + MAIN_PROGRAM_FILE_VERSION_STRING + ", " + MAIN_PROGRAM_32BIT_STRING + ")" + "/" + MAIN_PROGRAM_INI_FILENAME;
#endif

	//BrainStim(v1.0.0.1, 32-bits)
	if (QFile(sUserIniFilePath).exists() == false)
	{
		QString sMainProgramFileName = MAIN_PROGRAM_FILENAME;
		if (QFile(sMainProgramFileName).exists() == false)
		{
			if (QFile(sAlternativeMainProgramFileName).exists() == false)
			{
				qDebug() << __FUNCTION__ << "ERROR: " << "no *.ini file (" << sUserIniFilePath << ") or main program file (" << MAIN_PROGRAM_FILENAME << ") found, exiting...";
				return 0;
			}
			sMainProgramFileName = sAlternativeMainProgramFileName;
		}
		QProcess *process = new QProcess();
		process->start(sMainProgramFileName, QStringList() << "-I");
		if (process->waitForStarted())
		{
			if (process->waitForFinished())
			{
				QString sTempString = process->readAll();
				if (sTempString.isEmpty()==false)
					qDebug() << __FUNCTION__ << MAIN_PROGRAM_FILENAME << ": " << process->readAll();
				while (process->waitForReadyRead())
				{
					qDebug() << __FUNCTION__ << MAIN_PROGRAM_FILENAME << ": " << process->readAll();
				}
			}
		}
		delete process;
		//Check again...
		if (QFile(sUserIniFilePath).exists() == false)
		{
			qDebug() << __FUNCTION__ << "Could not locate *.ini file (" << sUserIniFilePath << "), exiting...";
			return 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	QVariant tmpVariant;
	QString sCustomPluginRootDir = "";
	QString sDefaultPluginsRootDir = "";
	QSettings sSettingsObj(sUserIniFilePath, QSettings::IniFormat);
	if (getSettingsInformation(sSettingsObj, REGISTRY_USERPLUGINSROOTDIRECTORY, tmpVariant) == false)
	{
		qDebug() << __FUNCTION__ << "Could not retrieve custom plugins root directory setting from *.ini file (" << sUserIniFilePath << "), exiting...";
		return 0;
	}
	sCustomPluginRootDir = tmpVariant.toString();
	if (getSettingsInformation(sSettingsObj, REGISTRY_DEFAULTPLUGINSROOTDIRECTORY, tmpVariant) == false)
	{
		qDebug() << __FUNCTION__ << "Could not retrieve default plugins root directory setting from *.ini file (" << sUserIniFilePath << "), exiting...";
		return 0;
	}
	sDefaultPluginsRootDir = tmpVariant.toString();
	if (getSettingsInformation(sSettingsObj, REGISTRY_MAINAPPUSERDIRECTORY, tmpVariant) == false)
	{
		qDebug() << __FUNCTION__ << "Could not retrieve user documents root directory setting from *.ini file (" << sUserIniFilePath << "), exiting...";
		return 0;
	}
	QString sUserAppRootDir = tmpVariant.toString();

	//////////////////////////////////////////////////////////////////////////////////////////////
	//Do we need to elevate?
	if (installationManagerBase::IsRunAsAdministrator() == false)
	{
		QStringList lParams;
		if (argc > 1)//arguments available?
		{
			for (int i = 1; i < argc; i++)
				lParams << argv[i];
		}
		installationManagerBase::ExecuteUpdateMngr(true, lParams);
		return 0;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	QApplication a(argc, argv);
	UpdateMngr w;

	if (argc > 1)//arguments available?
	{
		QString tempArgStr = "";
		tempArgStr = argv[1];
		if ((argc > 3) && ((tempArgStr == "-p") || (tempArgStr == "-P")))//plugin related and minimal two more arguments available?
		{
			QString sSourceFilePath = argv[2];//second argument should be the plugin *.ini or *.zip filepath
			if (QFile(sSourceFilePath).exists())
			{
				tempArgStr = argv[3];
				if (tempArgStr == "install")
				{
					bool bOverwriteExistingFiles = false;
					bool bRenameSourceInstallFileOnSuccess = false;
					if (argc > 4)//other flags/parameters available?
					{
						for (int i = 4; i < argc; i++)
						{
							tempArgStr = argv[i];
							if ((tempArgStr.toLower() == "-o") || (tempArgStr.toLower() == "-overwrite"))
							{
								bOverwriteExistingFiles = true;
							}
							else if ((tempArgStr.toLower() == "-r") || (tempArgStr.toLower() == "-rename"))
							{
								bRenameSourceInstallFileOnSuccess = true;
							}
						}
					}
					bool bRetval = false;
					QString sMessage;
					installationManagerBase::InstallResult resInstall = installationManagerBase::installPlugin(qApp->applicationDirPath(), sDefaultPluginsRootDir, sCustomPluginRootDir, sSourceFilePath, sUserAppRootDir, bOverwriteExistingFiles, bRenameSourceInstallFileOnSuccess);
					if (resInstall > 0)
					{
						sMessage = "-Installed: " + sSourceFilePath;
						installationManagerBase::qStdOut() << sMessage << endl;
					}
					else
					{
						sMessage = "-Failed: " + sSourceFilePath + ", return code: " + QString::number((int)resInstall);
						installationManagerBase::qStdOut() << sMessage << endl;
					}
				}
			}
		}
	}
	else
	{
		//Start User Interface
		w.show();
		return a.exec();
	}
	return 0;
}
