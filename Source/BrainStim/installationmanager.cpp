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

#include "installationmanager.h"
#include "installationmanagerbase.h"
#include "mainappinfo.h"
#include "customquestiondialog.h"
#include "archiver.h"
#include <QMessageBox>

installationManager::installationManager(QObject *parent) : QObject(parent)
{

}

installationManager::~installationManager()
{

}

QString installationManager::registerPlugin(QObject *plugin, const QString &sRootDirectory, const QString &sFileName, const bool &bIsEnabled, const bool &bIsStatic)
{
	if (plugin == NULL)
		return "";
	strcPluginDefinition tmpStrcPluginDefinition;
	DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);//For each plugin (static or dynamic), we check which interfaces it implements using qobject_cast()
	if (iDevice)
	{
		tmpStrcPluginDefinition.pType = PluginInterface::DevicePlugin;
		tmpStrcPluginDefinition.pInterface = iDevice;
	}
	ExtensionInterface *iExtension = qobject_cast<ExtensionInterface *>(plugin);//For each plugin (static or dynamic), we check which interfaces it implements using qobject_cast()
	if (iExtension)
	{
		tmpStrcPluginDefinition.pType = PluginInterface::ExtensionPlugin;
		tmpStrcPluginDefinition.pInterface = iExtension;
	}
	if (tmpStrcPluginDefinition.pInterface == NULL)
		return "";
	tmpStrcPluginDefinition.bIsEnabled = bIsEnabled;
	tmpStrcPluginDefinition.bIsStatic = bIsStatic;
	tmpStrcPluginDefinition.sName = tmpStrcPluginDefinition.pInterface->GetPluginInternalName();
	if (sFileName.isEmpty()==false)
	{
		tmpStrcPluginDefinition.sFileName = sFileName;
		tmpStrcPluginDefinition.sFileBaseName = QFileInfo(sFileName).baseName();
		QString sPluginAbsoluteFilePath = sRootDirectory + "/" + tmpStrcPluginDefinition.sFileName;
		QString sPluginIniFilePath = getPluginIniFilePath(sPluginAbsoluteFilePath);
		if (QFile(sPluginIniFilePath).exists())
		{
			QSettings pluginSettings(sPluginIniFilePath, QSettings::IniFormat);
			pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
			QStringList lFileList;
			tmpStrcPluginDefinition.lInstallFileList = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_FILES, QStringList()).toStringList();
			pluginSettings.endGroup();
		}
	}
	if (tmpStrcPluginDefinition.sName.isEmpty()==false)
		mapRegisteredPluginNametoDef.insert(tmpStrcPluginDefinition.sName, tmpStrcPluginDefinition);
	return tmpStrcPluginDefinition.sName;
}

bool installationManager::unregisterPlugin(const QString &sFileName)
{
	QString sFoundKey = "";
	QMap<QString, strcPluginDefinition>::iterator iterItem = mapRegisteredPluginNametoDef.begin();
	while (iterItem != mapRegisteredPluginNametoDef.end())
	{
		if (iterItem.value().sFileName == sFileName)
		{
			sFoundKey = iterItem.key();
			break;
		}
		++iterItem;
	}
	if (sFoundKey.isEmpty() == false)
	{
		mapRegisteredPluginNametoDef.remove(sFoundKey);
	}
	return false;
}

bool installationManager::isRegisteredPlugin(const QString &sFileBaseName)
{
	if (sFileBaseName.isEmpty() == false)
	{
		foreach(strcPluginDefinition pluginDef, mapRegisteredPluginNametoDef.values())
		{
			if (pluginDef.sFileBaseName == sFileBaseName)
				return true;
		}
	}
	return false;
}

QMap<QString, bool> installationManager::getRegisteredPluginNamesAndStates()
{
	QMap<QString, bool> mapRegisteredStatesItems;
	for (int i = 0; i < mapRegisteredPluginNametoDef.keys().count(); i++)
	{
		mapRegisteredStatesItems.insert(mapRegisteredPluginNametoDef.keys()[i], mapRegisteredPluginNametoDef.value(mapRegisteredPluginNametoDef.keys()[i]).bIsEnabled);
	}
	return mapRegisteredStatesItems;
}

QStringList installationManager::getRegisteredAndEnabledPluginNames()
{
	QStringList lRegisteredItems;
	for (int i = 0; i < mapRegisteredPluginNametoDef.keys().count(); i++)
	{
		if (mapRegisteredPluginNametoDef.value(mapRegisteredPluginNametoDef.keys()[i]).bIsEnabled)
		{
			lRegisteredItems.append(mapRegisteredPluginNametoDef.keys()[i]);
		}
	}
	return lRegisteredItems;
}

PluginInterface *installationManager::getRegisteredPluginInterface(const QString &sRegisteredPluginName)
{
	if (mapRegisteredPluginNametoDef.contains(sRegisteredPluginName))
		return mapRegisteredPluginNametoDef.value(sRegisteredPluginName).pInterface;
	return NULL;
}

QString installationManager::getRegisteredPluginName(const QString &sPluginFileName)
{
	foreach(strcPluginDefinition pluginDef, mapRegisteredPluginNametoDef.values())
	{
		if (pluginDef.sFileName == sPluginFileName)
		{
			return pluginDef.sName;
		}
	}
	return "";
}

int installationManager::configureRegisteredPluginScriptEngine(const QString &sRegisteredPluginName, QScriptEngine *pEngine)
{//returns meta script id or -1 in case it fails
	int nNewMetaID = -1;
	if ((mapRegisteredPluginNametoDef.contains(sRegisteredPluginName)) && (pEngine))
	{
		mapRegisteredPluginNametoDef[sRegisteredPluginName].pScriptEngine = pEngine;
		nNewMetaID = mapRegisteredPluginNametoDef.value(sRegisteredPluginName).pInterface->ConfigureScriptEngine(*pEngine);
		if (nNewMetaID >= 0)
			mapRegisteredPluginNametoDef[sRegisteredPluginName].nScriptMetaID = nNewMetaID;
	}
	return nNewMetaID;
}

PluginInterface::PluginType installationManager::getPluginType(const QString &sRegisteredPluginName)
{
	if (mapRegisteredPluginNametoDef.contains(sRegisteredPluginName))
	{
		return mapRegisteredPluginNametoDef.value(sRegisteredPluginName).pType;
	}
	return PluginInterface::UnknownPlugin;
}

QString installationManager::getPluginAbsFilePath(const QString &sRegisteredPluginName)
{
	if (mapRegisteredPluginNametoDef.contains(sRegisteredPluginName))
	{
		return QDir(MainAppInfo::userPluginsDirPath()).absoluteFilePath(mapRegisteredPluginNametoDef.value(sRegisteredPluginName).sFileName);
	}
	return "";
}

QStringList installationManager::getPluginInstallFiles(const QString &sRegisteredPluginName)
{
	if (mapRegisteredPluginNametoDef.contains(sRegisteredPluginName))
	{
		return mapRegisteredPluginNametoDef.value(sRegisteredPluginName).lInstallFileList;
	}
	return QStringList();
}

QString installationManager::getPluginFileName(const QString &sRegisteredPluginName)
{
	if (mapRegisteredPluginNametoDef.contains(sRegisteredPluginName))
	{
		return mapRegisteredPluginNametoDef.value(sRegisteredPluginName).sFileName;
	}
	return "";
}

QString installationManager::getPluginIniFilePath(const QString &sPluginFilePath)
{
	QFileInfo fiPluginPath(sPluginFilePath);
	if (fiPluginPath.exists())
	{
		QString sPluginSettingsFilePath = fiPluginPath.canonicalPath() + "/" + fiPluginPath.completeBaseName() + ".ini";
		if (QFile(sPluginSettingsFilePath).exists())
			return sPluginSettingsFilePath;
	}
	return "";
}

bool installationManager::isEnabledPlugin(const QString &sPluginFilePath)
{
	QString sPluginSettingsFilePath = getPluginIniFilePath(sPluginFilePath);
	if (sPluginSettingsFilePath.isEmpty() == false)
	{
		QSettings pluginSettings(sPluginSettingsFilePath, QSettings::IniFormat);
		pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
		bool bPluginsIsEnabled = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_ENABLED, false).toBool();
		pluginSettings.endGroup();
		return bPluginsIsEnabled;
	}
	return false;
}

bool installationManager::createRegisteredPluginIniFile(const QString &sRegisteredPluginName)
{
	//QString sPluginFileName = getPluginFileName(sRegisteredPluginName);
	QString sPluginAbsFilePath = getPluginAbsFilePath(sRegisteredPluginName);
	QString sPluginSettingsFilePath = getPluginIniFilePath(sPluginAbsFilePath);
	QDir dirRoot(MainAppInfo::userPluginsDirPath());

	if (sPluginSettingsFilePath.isEmpty() == false)
	{
		QSettings pluginSettings(sPluginSettingsFilePath, QSettings::IniFormat);
		QStringList lFileList;
		lFileList << dirRoot.relativeFilePath(sPluginSettingsFilePath)
			<< dirRoot.relativeFilePath(sPluginAbsFilePath)
			<< dirRoot.relativeFilePath(MainAppInfo::appDirPath() + "/fbclient.dll")
			<< dirRoot.relativeFilePath(MainAppInfo::appDirPath() + "/firebird.msg")
			<< dirRoot.relativeFilePath(MainAppInfo::appDirPath() + "/msvcr80.dll")
			<< dirRoot.relativeFilePath(MainAppInfo::appDirPath() + "/icuuc30.dll");
		pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
		pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_NAME, sRegisteredPluginName);
		pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_ENABLED, true);
		pluginSettings.endGroup();
		pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
		//pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_ROOT, getPluginRootDirectory(sRegisteredPluginName));
		pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_FILES, lFileList);
		pluginSettings.endGroup();
		return true;
	}
	return false;
}

//QStringList getPluginInstallFilesFromIniFile(const QString &sPluginIniFilePath)
//{
//	QStringList lReturnList;
//	if (sPluginIniFilePath.isEmpty() == false)
//	{	
//		if (QFileInfo(sPluginIniFilePath).exists())
//		{
//			QSettings pluginSettings(sPluginIniFilePath, QSettings::IniFormat);
//			pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
//			lReturnList = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_FILES, QStringList()).toStringList();
//			pluginSettings.endGroup();
//		}
//	}
//	return lReturnList;
//}

bool installationManager::changePluginEnabledSetting(const QString &sPluginIniFilePath, const bool &bEnable)
{
	if (sPluginIniFilePath.isEmpty())
		return false;
	if (QFileInfo(sPluginIniFilePath).exists())
	{
		QSettings pluginSettings(sPluginIniFilePath, QSettings::IniFormat);
		pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
		pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_ENABLED, bEnable);
		pluginSettings.endGroup();
		return true;
	}
	return false;
}

bool installationManager::isStaticPlugin(const QString &sRegisteredPluginName)
{
	QMap<QString, strcPluginDefinition>::iterator iterItem = mapRegisteredPluginNametoDef.begin();
	while (iterItem != mapRegisteredPluginNametoDef.end())
	{
		if (iterItem.value().sName == sRegisteredPluginName)
			return iterItem.value().bIsStatic;
		++iterItem;
	}
	return false;
}

bool installationManager::changeRegisteredPlugin(const QString &sRegisteredPluginName, const bool &bEnable)
{
	QMap<QString, strcPluginDefinition>::iterator iterItem = mapRegisteredPluginNametoDef.begin();
	while (iterItem != mapRegisteredPluginNametoDef.end())
	{
		if (iterItem.value().sName == sRegisteredPluginName)
		{
			QString sPluginAbsFilePath = getPluginAbsFilePath(sRegisteredPluginName);
			if (sPluginAbsFilePath.isEmpty() == false)
			{
				if (isStaticPlugin(sRegisteredPluginName))
					return false;
				QString sPluginAbsIniFilePath = getPluginIniFilePath(sPluginAbsFilePath);
				if (changePluginEnabledSetting(sPluginAbsIniFilePath, bEnable))
				{
					iterItem.value().bIsEnabled = bEnable;
					bool bRetval = false;
					if (QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_UNLOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval)))
					{
						if (bRetval)
						{
							if (QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_LOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval))) // , Q_ARG(QString, sMenuActionPath), Q_ARG(QString*, &sTmpResult)))
								return bRetval;
						}
					}
				}
			}
		}
		++iterItem;
	}
	return false;
}

bool installationManager::unistallRegisteredPlugin(const QString &sRegisteredPluginName)
{
	if (sRegisteredPluginName.isEmpty())
		return false;
	foreach(strcPluginDefinition pluginDef, mapRegisteredPluginNametoDef.values())
	{
		if (pluginDef.sName == sRegisteredPluginName)
		{
			if (pluginDef.bIsStatic)
				return false;
			QString sPluginAbsFilePath = getPluginAbsFilePath(sRegisteredPluginName);
			if (sPluginAbsFilePath.isEmpty() == false)
			{
				QString sPluginAbsIniFilePath = getPluginIniFilePath(sPluginAbsFilePath);
				if (changePluginEnabledSetting(sPluginAbsIniFilePath, false))
				{
					QSettings pluginSettings(sPluginAbsIniFilePath, QSettings::IniFormat);
					//pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
					//QString sPluginInternalName = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_NAME, "").toString();
					//pluginSettings.endGroup();
					pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
					QStringList lFileList;
					lFileList = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_FILES, QStringList()).toStringList();
					pluginSettings.endGroup();
					bool bRetval = false;
					if (QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_UNLOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval)))
					{
						if (bRetval)
						{
							if (lFileList.isEmpty() == false)
							{
								for (int i = 0; i < lFileList.count(); i++)
								{
									QString sFileName = QFileInfo(lFileList[i]).fileName();
									QString sFilePath = MainAppInfo::userPluginsDirPath() + "/" + lFileList[i];
									QFile sInstallationFilePath(sFilePath);
									if (sInstallationFilePath.exists())
									{
										//bool bRemoveSucceeded = 
										QFile::remove(sFilePath);
										if (sInstallationFilePath.exists())
										{
											qDebug() << __FUNCTION__ << "Could not remove the file: " << sFilePath;
											return false;
										}
									}
								}
							}
							else
							{
								return true;
							}
							if (QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_LOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval))) // , Q_ARG(QString, sMenuActionPath), Q_ARG(QString*, &sTmpResult)))
								return bRetval;
						}
					}
				}
			}
			return false;
		}
	}
	return false;
}

bool installationManager::createPluginConfigurationSetting(const QString &sPluginInstallFilePath, const QString &sInternalName, const bool &bIsEnabled, const QStringList lInstallationFiles)
{
	QFileInfo fiPluginIniFile(sPluginInstallFilePath);
	if (fiPluginIniFile.absoluteDir().exists() == false)
		return false;
	QSettings pluginSettings(sPluginInstallFilePath, QSettings::IniFormat);
	pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
	pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_NAME, sInternalName);
	pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_ENABLED, bIsEnabled);
	pluginSettings.endGroup();
	pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
	pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_FILES, lInstallationFiles);
	pluginSettings.endGroup();
	return true;
}

int installationManager::installPlugin(const QString &sPluginInstallFilePath)
{
	QString sMainAppDirPath = MainAppInfo::appDirPath();
	QString sMainAppsDefaultPluginDirPath = MainAppInfo::defaultPluginsDirPath();
	QString sMainAppsUserPluginDirPath = MainAppInfo::userPluginsDirPath();
	QString sUserAppPath = MainAppInfo::appUserPath();
	bool bOverWriteExistingFiles = true;

	installationManagerBase::InstallResult installResult = installationManagerBase::installPlugin(sMainAppDirPath, sMainAppsDefaultPluginDirPath, sMainAppsUserPluginDirPath, sPluginInstallFilePath, sUserAppPath, bOverWriteExistingFiles);


	QFileInfo fiPluginInstallFilePath(sPluginInstallFilePath);
	QString sIniFilePath = "";
	if (fiPluginInstallFilePath.completeSuffix() == "ini")
	{
		sIniFilePath = sPluginInstallFilePath;
	}
	else if (fiPluginInstallFilePath.completeSuffix() == "zip")
	{
		Archiver *pArchiver = new Archiver(this);
		QStringList lArchiverFiles = pArchiver->getFileList(sPluginInstallFilePath);
		delete pArchiver;
		pArchiver = NULL;
		QStringList lArchiverIniFiles;
		foreach(QString sTmpFileArchiverPath, lArchiverFiles)
		{
			if (QFileInfo(sTmpFileArchiverPath).completeSuffix() == "ini")
				lArchiverIniFiles.append(sTmpFileArchiverPath);
		}
		if (lArchiverIniFiles.isEmpty())
		{
			qDebug() << __FUNCTION__ << "The compressed file doesn't seem to be a valid installer package, no configuration (*.ini) file found!";
			return false;
		}
		QTemporaryDir dir;
		if (dir.isValid() == false)
		{
			qDebug() << __FUNCTION__ << "Could not create a temporarily directory for the extraction of the of the installer package, please try to extract it yourself and use the extracted configuration (*.ini) file for the installation process!";
			return false;
		}
		QStringList lExtractedFiles = JlCompress::extractDir(sPluginInstallFilePath, dir.path());
		bool bRetVal = true;
		foreach(QString sTmpExtractedFilePath, lExtractedFiles)
		{
			if (QFileInfo(sTmpExtractedFilePath).completeSuffix() == "ini")
			{
				if (installPlugin(sTmpExtractedFilePath) < 0)
					bRetVal = false;
			}
		}
		dir.remove();
		return bRetVal;
	}
	else
	{
		return false;
	}

	QFile fPluginIniFile(sIniFilePath);
	if (fPluginIniFile.exists())
	{
		QString sIniFileAbsDir = QFileInfo(sIniFilePath).absolutePath();
		if (QDir(MainAppInfo::userPluginsDirPath()).canonicalPath() == QDir(sIniFileAbsDir).canonicalPath())
			return false;

		QSettings pluginSettings(sIniFilePath, QSettings::IniFormat);
		pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
		QString sPluginInternalName = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_NAME, "").toString();
		pluginSettings.endGroup();
		pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_INSTALLATION);
		QStringList lFileList;
		lFileList = pluginSettings.value(INSTALLMNGR_SETTING_SETTING_FILES, QStringList()).toStringList();
		pluginSettings.endGroup();

		if (isRegisteredPlugin(QFileInfo(sIniFilePath).baseName()))
		{
			int confirm = QMessageBox::question(NULL, "Uninstall already present plugin?", "A plugin with the same name (" + sPluginInternalName + ") is already in use.\nTo proceed this installation that active plugin first needs to be uninstalled, do you wish to do this?", QMessageBox::Ok | QMessageBox::Cancel);
			if (confirm == QMessageBox::Ok)
			{
				if (unistallRegisteredPlugin(sPluginInternalName) == false)
				{
					qDebug() << __FUNCTION__ << "Could not uninstall the plugin: " << sPluginInternalName;
					return false;
				}
			}
			else if (QMessageBox::Cancel)
			{
				return false;
			}
		}

		if (lFileList.isEmpty() == false)
		{
			//Check whether all files are present and mark those indexes of files which already exist in installation directory
			bool bAllFilesCheck = true;
			QList<int> nPresentInCurrentInstallIndexes;
			for (int i = 0; i < lFileList.count(); i++)
			{
				QString sFileName = QFileInfo(lFileList[i]).fileName();
				QFile sSourceFilePath(sIniFileAbsDir + "/" + sFileName);
				lFileList[i] = installationManagerBase::resolveFileDirectoryPath(lFileList[i], MainAppInfo::userPluginsDirPath(), MainAppInfo::appDirPath(), QFileInfo(sIniFilePath).completeBaseName(), MainAppInfo::defaultPluginsDirPath(), MainAppInfo::appUserPath());
				QFile sInstallationFilePath(lFileList[i]);// MainAppInfo::userPluginsDirPath() + "/" + lFileList[i]);
				if (sSourceFilePath.exists() == false)
				{
						bAllFilesCheck = false;
						break;
				}
				if (QFileInfo(lFileList[i]).exists())// MainAppInfo::userPluginsDirPath() + "/" + lFileList[i]).exists())
					nPresentInCurrentInstallIndexes.append(i);
			}
			if(bAllFilesCheck == false)
				return false;

			if (nPresentInCurrentInstallIndexes.isEmpty() == false)
			{
				//Todo handle shared files here!!, first disable all and then reboot, install and enable all
				QString sSkippedFiles;
				foreach(int nIndex, nPresentInCurrentInstallIndexes)
				{
					sSkippedFiles = sSkippedFiles + "\n" + lFileList[nIndex];
				}
				qDebug() << __FUNCTION__ << "Some installation files (for the " << sPluginInternalName << " plugin) were skipped because they were already present and in use: " << sSkippedFiles;
				//see below uncommented copy step!!!




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
				if (nPresentInCurrentInstallIndexes.contains(i) == false)
				{
					QString sFileName = QFileInfo(lFileList[i]).fileName();
					QString sSourceFilePath = sIniFileAbsDir + "/" + sFileName;
					QString sInstallationFilePath = lFileList[i];//MainAppInfo::userPluginsDirPath() + "/" + lFileList[i];
					QString sInstallationDirectory = QFileInfo(sInstallationFilePath).absolutePath();
					QDir dirInstallationFile;
					if (dirInstallationFile.mkpath(sInstallationDirectory))
					{
						if (QFile::copy(sSourceFilePath, sInstallationFilePath) == false)
							qDebug() << __FUNCTION__ << "Could not copy a plugin installation file to the plugin folder: " << sSourceFilePath;
					}
					else
					{
						qDebug() << __FUNCTION__ << "Could not create a plugin installation sub directory: " << sInstallationDirectory;
					}
				}
			}
			//bool bRetval = false;
			//if (QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_LOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval)))
			//	return bRetval;
			return true;
		}
	}
	return false;
}
