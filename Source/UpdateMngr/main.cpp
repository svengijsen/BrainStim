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
	if (QFile(MAIN_PROGRAM_INI_FILENAME).exists() == false)
	{
		QString sMainProgramFileName = MAIN_PROGRAM_FILENAME;
		if (QFile(sMainProgramFileName).exists() == false)
		{
			if (QFile(sAlternativeMainProgramFileName).exists() == false)
			{
				qDebug() << __FUNCTION__ << "ERROR: " << "no *.ini file (" << MAIN_PROGRAM_INI_FILENAME << ") or main program file (" << MAIN_PROGRAM_FILENAME << ") found, exiting...";
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
		if (QFile(MAIN_PROGRAM_INI_FILENAME).exists() == false)
		{
			qDebug() << __FUNCTION__ << "Could not locate *.ini file (" << MAIN_PROGRAM_INI_FILENAME << "), exiting...";
			return 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	QVariant tmpVariant;
	QString sCustomPluginsRootDir = "";
	QSettings sSettingsObj(MAIN_PROGRAM_INI_FILENAME, QSettings::IniFormat);
	if (getSettingsInformation(sSettingsObj, REGISTRY_CUSTOMPLUGINSROOTDIRECTORY, tmpVariant) == false)
	{
		qDebug() << __FUNCTION__ << "Could not retrieve plugin root directory setting from *.ini file (" << MAIN_PROGRAM_INI_FILENAME << "), exiting...";
		return 0;
	}
	sCustomPluginsRootDir = tmpVariant.toString();
	//////////////////////////////////////////////////////////////////////////////////////////////
	if (argc > 1)//arguments available?
	{
		installationManagerBase *baseInstallManager = new installationManagerBase(NULL);

		QString tempArgStr = "";
		tempArgStr = argv[1];
		if ((argc > 3) && ((tempArgStr == "-p") || (tempArgStr == "-P")))//plugin related and minimal two more arguments available?
		{
			QString sIniFilePath = argv[2];//second argument should be the plugin *.ini filepath
			if (QFile(sIniFilePath).exists())
			{
				tempArgStr = argv[3];
				if (tempArgStr == "install")
				{
					//installationManager *installMngrObject = new installationManager(NULL);
					bool bRetval = false;
					if (baseInstallManager->installPlugin(sCustomPluginsRootDir, sIniFilePath))
					//	QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_LOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval));
					//if (bRetval)
						qDebug() << __FUNCTION__ << "Plugin successfully installed as defined in (" << sIniFilePath << ")";
					else
						qDebug() << __FUNCTION__ << "Could not install the requested plugin defined in (" << sIniFilePath << "), exiting...";
						//delete installMngrObject;
				}
			}
		}
		delete baseInstallManager;
	}
	else
	{
		//Start User Interface
		QApplication a(argc, argv);
		UpdateMngr w;
		w.show();
		return a.exec();
	}
	return 0;
}
