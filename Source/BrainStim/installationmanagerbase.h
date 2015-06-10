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


#ifndef INSTALLATIONMANAGERBASE_H
#define INSTALLATIONMANAGERBASE_H

#include <QFileInfo>
#include <QMessageBox>
#include <QObject>
#include <QDebug>
// #include <QSettings>
// #include "plugininterface.h"
#include "mainappinfo.h"
#include "quazip/JlCompress.h"
#include <QTemporaryDir>
//#include <QtScript>
//#include <QScriptable>
//#include "archiver.h"
 
 #define INSTALLMNGR_SETTING_SECTION_CONFIGURATION	"Configuration"
 #define INSTALLMNGR_SETTING_SECTION_INSTALLATION	"Installation"
 
 #define INSTALLMNGR_SETTING_SETTING_ENABLED		"Enabled"
 #define INSTALLMNGR_SETTING_SETTING_NAME			"Name"
 //#define INSTALLMNGR_SETTING_SETTING_ROOT			"RootDirectory"
 #define INSTALLMNGR_SETTING_SETTING_FILES			"Files"
 
#define INSTALLMNGR_SETTING_MACRO_PLUGINNAME		"<PluginName>"
#define INSTALLMNGR_SETTING_MACRO_PLUGINPATH		"<PluginPath>"
#define INSTALLMNGR_SETTING_MACRO_APPLICATIONPATH	"<MainAppPath>"

//class Archiver;
class installationManagerBase// : public QObject
{
	//Q_OBJECT

	public:
 
		installationManagerBase(QObject *parent = NULL) {};
		~installationManagerBase() {};

// 	QString registerPlugin(QObject *plugin, const QString &sRootDirectory, const QString &sFileName, const bool &bIsEnabled, const bool &bIsStatic);
// 	bool unregisterPlugin(const QString &sFileName);
// 	bool isRegisteredPlugin(const QString &sFileBaseName);
// 	QStringList getRegisteredAndEnabledPluginNames();
// 	QMap<QString, bool> getRegisteredPluginNamesAndStates();
// 	QString getRegisteredPluginName(const QString &sPluginFileName);
// 	PluginInterface *getRegisteredPluginInterface(const QString &sRegisteredPluginName);
// 	int configureRegisteredPluginScriptEngine(const QString &sRegisteredPluginName, QScriptEngine *pEngine);
// 	PluginInterface::PluginType getPluginType(const QString &sRegisteredPluginName);
// 	QString getPluginAbsFilePath(const QString &sRegisteredPluginName);
// 	bool isEnabledPlugin(const QString &sPluginFilePath);
// 	bool isStaticPlugin(const QString &sRegisteredPluginName);
	bool static unistallRegisteredPlugin(const QString &sRegisteredPluginName){ return true; };
	int installPlugin(const QString &sMainAppsPluginDirPath, const QString &sPluginInstallFilePath, const bool &bOverWriteExistingFiles)
	{
		QFileInfo fiPluginInstallFilePath(sPluginInstallFilePath);
		QString sIniFilePath = "";
		if (fiPluginInstallFilePath.completeSuffix() == "ini")
		{
			sIniFilePath = sPluginInstallFilePath;
		}
		else if (fiPluginInstallFilePath.completeSuffix() == "zip")
		{			
			QStringList lArchiverFiles = JlCompress::getFileList(sPluginInstallFilePath);
			QStringList lArchiverIniFiles;
			foreach(QString sTmpFileArchiverPath, lArchiverFiles)
			{
				if (QFileInfo(sTmpFileArchiverPath).completeSuffix() == "ini")
					lArchiverIniFiles.append(sTmpFileArchiverPath);
			}
			if (lArchiverIniFiles.isEmpty())
			{
				qDebug() << __FUNCTION__ << "The compressed file doesn't seem to be a valid installer package, no configuration (*.ini) file found!";
				return -1;
			}
			QTemporaryDir dir;
			if (dir.isValid() == false)
			{
				qDebug() << __FUNCTION__ << "Could not create a temporarily directory for the extraction of the of the installer package, please try to extract it yourself and use the extracted configuration (*.ini) file for the installation process!";
				return -2;
			}
			QStringList lExtractedFiles = JlCompress::extractDir(sPluginInstallFilePath, dir.path());
			int nRetVal = 0;
			foreach(QString sTmpExtractedFilePath, lExtractedFiles)
			{
				if (QFileInfo(sTmpExtractedFilePath).completeSuffix() == "ini")
				{
					int nTempRetVal = installPlugin(sMainAppsPluginDirPath, sTmpExtractedFilePath, bOverWriteExistingFiles);
					if (nTempRetVal < 0)
					{
						nRetVal = nTempRetVal;
						break;
					}
					nRetVal = nRetVal + nTempRetVal;
				}
			}
			dir.remove();
			return nRetVal;
		}
		else
		{
			return -3;
		}

		QFile fPluginIniFile(sIniFilePath);
		if (fPluginIniFile.exists())
		{
			QString sIniFileAbsDir = QFileInfo(sIniFilePath).absolutePath();
			if (QDir(sMainAppsPluginDirPath).canonicalPath() == QDir(sIniFileAbsDir).canonicalPath())//Is a ini file inside the plugins directory? 
				return -4;

			QSettings pluginSettings(sIniFilePath, QSettings::IniFormat);
			pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
			QString sPluginInternalName = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_NAME, "").toString();
			pluginSettings.endGroup();
			pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
			QStringList lFileList;
			lFileList = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_FILES, QStringList()).toStringList();
			pluginSettings.endGroup();

			QString sIniFileName = QFileInfo(sIniFilePath).fileName();
			//if (isRegisteredPlugin(QFileInfo(sIniFilePath).baseName()))
			if (QFile(sMainAppsPluginDirPath + "/" + sIniFileName).exists())
			{
				//int confirm = QMessageBox::question(NULL, "Uninstall already present plugin?", "A plugin with the same name (" + sPluginInternalName + ") is already in use.\nTo proceed this installation that active plugin first needs to be un-installed, do you wish to proceed?", QMessageBox::Ok | QMessageBox::Cancel);
				if (bOverWriteExistingFiles==false)//(confirm == QMessageBox::Ok)
				{
					//if (unistallRegisteredPlugin(sPluginInternalName) == false)
					//{
					//	qDebug() << __FUNCTION__ << "Could not uninstall the plugin: " << sPluginInternalName;
					//	return -5;
					//}
				//}
				//else// if (QMessageBox::Cancel)
				//{
					qDebug() << __FUNCTION__ << "File already exists (" + sMainAppsPluginDirPath + "/" + sIniFileName + ")!" << sPluginInternalName;
					return -5;
				}
			}

			if (lFileList.isEmpty() == false)
			{
				//Check whether all files are present and mark those indexes of files which already exist in installation directory
				bool bAllFilesCheck = true;
				QList<int> nPresentInCurrentInstallIndexes;
				for (int i = 0; i < lFileList.count(); i++)
				{
					lFileList[i].replace(INSTALLMNGR_SETTING_MACRO_PLUGINPATH, sMainAppsPluginDirPath, Qt::CaseInsensitive);
					lFileList[i].replace(INSTALLMNGR_SETTING_MACRO_APPLICATIONPATH, sMainAppsPluginDirPath, Qt::CaseInsensitive);
					lFileList[i].replace(INSTALLMNGR_SETTING_MACRO_PLUGINNAME, QFileInfo(sIniFileName).completeBaseName(), Qt::CaseInsensitive);
				}
				for (int i = 0; i < lFileList.count(); i++)
				{
					QString sFileName = QFileInfo(lFileList[i]).fileName();
					QFile fSourceFilePath(sIniFileAbsDir + "/" + sFileName);
					QFile fInstallationFilePath(lFileList[i]); //sMainAppsPluginDirPath + "/" + lFileList[i]);
					if (fSourceFilePath.exists() == false)
					{
						bAllFilesCheck = false;
						break;
					}
					if (fInstallationFilePath.exists())// QFileInfo(sMainAppsPluginDirPath + "/" + lFileList[i]).exists())
						nPresentInCurrentInstallIndexes.append(i);
				}
				if (bAllFilesCheck == false)
					return -6;

				if (nPresentInCurrentInstallIndexes.isEmpty() == false)
				{
					//Todo handle shared files here!!, first disable all and then reboot, install and enable all
					if (bOverWriteExistingFiles == false)
					{
						QString sSkippedFiles;
						foreach(int nIndex, nPresentInCurrentInstallIndexes)
						{
							sSkippedFiles = sSkippedFiles + "\n" + lFileList[nIndex];
						}
						qDebug() << __FUNCTION__ << "Some installation files (for the " << sPluginInternalName << " plugin) were skipped because they were already present and in use: " << sSkippedFiles;
						//see below uncommented copy step!!!
					}



					//bool bSkipAll = false;
					//bool bOverwriteAll = false;
					//foreach(int nIndex, nPresentInCurrentInstallIndexes)
					//{
					//	bool bReplace = false;
					//	bool bCancel = false;
					//	if (bOverwriteAll == false)
					//	{
					//		customQuestionDialog questionDialog;
					//		QStringList lQuestions;
					//		lQuestions << "Overwrite" << "Overwrite All" << "Skip" << "Skip All" << "Cancel";
					//		questionDialog.setOptions("File(s) already present", "The current installation already contains some of the files that are included by this plugin.\nWhat do you wish to do?", lQuestions, 2);
					//		int nReply = questionDialog.exec();
					//		switch (nReply)
					//		{
					//		case 0://Overwrite
					//			bReplace = true;
					//			break;
					//		case 1://Overwrite All
					//			bOverwriteAll = true;
					//			bReplace = true;
					//			break;
					//		case 2://Skip
					//			break;
					//		case 3://Skip All
					//			bSkipAll = true;
					//			break;
					//		case 4://Cancel
					//			bCancel = true;
					//			break;
					//		default:
					//			break;
					//		}
					//	}
					//	else
					//	{
					//		bReplace = true;
					//	}
					//	if (bCancel || bSkipAll)
					//		break;
					//	if (bReplace)
					//	{
					//		//QFileInfo(MainAppInfo::pluginsDirPath() + "/" + lFileList[i]).exists())
					//		bool bRemoveSucceeded = QFile::remove(MainAppInfo::pluginsDirPath() + "/" + lFileList[nIndex]);
					//		if (QFileInfo(MainAppInfo::pluginsDirPath() + "/" + lFileList[nIndex]).exists())
					//			bRemoveSucceeded = false;

					//		//QFile::copy("/home/user/src.txt", "/home/user/dst.txt");
					//		bReplace = bReplace;
					//	}
					//}
				}
				//Copy the files to the installation folder
				for (int i = 0; i < lFileList.count(); i++)
				{
					if ((nPresentInCurrentInstallIndexes.contains(i) == false) || (bOverWriteExistingFiles))
					{
						QString sFileName = QFileInfo(lFileList[i]).fileName();
						QString sSourceFilePath = sIniFileAbsDir + "/" + sFileName;
						//QString sInstallationFilePath;
						//sInstallationFilePath  = sMainAppsPluginDirPath + "/" + lFileList[i];

						if (QFile(lFileList[i]).exists())
						{
							//here we can be sure that we may remove the file...
								//		//QFileInfo(MainAppInfo::pluginsDirPath() + "/" + lFileList[i]).exists())
							bool bRemoveSucceeded = QFile::remove(lFileList[i]);
							if (QFileInfo(lFileList[i]).exists())
								bRemoveSucceeded = false;
							if (bRemoveSucceeded == false)
							{
								qDebug() << __FUNCTION__ << "Could not replace the already present file (" << lFileList[i] << ")!";
								return -7;
							}
						}
						//Make sure that the destination folder exists...
						if (QFileInfo(lFileList[i]).absoluteDir().exists() == false)
						{
							if (QDir().mkpath(QFileInfo(lFileList[i]).absolutePath()) == false)
							{
								qDebug() << __FUNCTION__ << "Could not create the destination directory path: " << QFileInfo(lFileList[i]).absolutePath();
								return -8;
							}
						}
						if (QFile::copy(sSourceFilePath, lFileList[i]) == false)
						{
							qDebug() << __FUNCTION__ << "Could not copy a plugin installation file to the plugin folder: " << sSourceFilePath;
							return -9;
						}
					}
				}
				//bool bRetval = false;
				//if (QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_LOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval)))
				//	return bRetval;
				return 1;
			}
		}
		return -99;
	};
// 	bool changeRegisteredPlugin(const QString &sRegisteredPluginName, const bool &bEnable);
// 	QString getPluginIniFilePath(const QString &sPluginFilePath);
// 	bool createPluginConfigurationSetting(const QString &sPluginInstallFilePath, const QString &sInternalName, const bool &bIsEnabled, const QStringList lInstallationFiles);
// 	QStringList getPluginInstallFiles(const QString &sRegisteredPluginName);
// 
 //private:

	 //Archiver *pluginArchiver;

	 // 	bool changePluginEnabledSetting(const QString &sPluginIniFilePath, const bool &bEnable);
// 	QString getPluginFileName(const QString &sRegisteredPluginName);
// 	bool createRegisteredPluginIniFile(const QString &sRegisteredPluginName);
// 
// 	struct strcPluginDefinition
// 	{
// 		QString sName;
// 		PluginInterface::PluginType pType;
// 		PluginInterface *pInterface;
// 		int nScriptMetaID;
// 		QScriptEngine *pScriptEngine;
// 		QString sFileBaseName;
// 		QString sFileName;
// 		QStringList lInstallFileList;
// 		bool bIsEnabled;
// 		bool bIsStatic;
// 		strcPluginDefinition()
// 		{
// 			bIsStatic = false;
// 			bIsEnabled = false;
// 			sName = "";
// 			sFileBaseName = "";
// 			sFileName = "";
// 			pInterface = NULL;
// 			pScriptEngine = NULL;
// 			nScriptMetaID = -1;
// 			pType = PluginInterface::UnknownPlugin;
// 		}
// 	};
// 
// 	QMap<QString, strcPluginDefinition> mapRegisteredPluginNametoDef;
 };

#endif // INSTALLATIONMANAGERBASE_H
