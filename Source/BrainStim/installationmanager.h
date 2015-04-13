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


#ifndef INSTALLATIONMANAGER_H
#define INSTALLATIONMANAGER_H

#include <QObject>
#include <QSettings>
#include "plugininterface.h"
#include "archiver.h"

#define INSTALLMNGR_SETTING_SECTION_CONFIGURATION	"Configuration"
#define INSTALLMNGR_SETTING_SECTION_INSTALLATION	"Installation"

#define INSTALLMNGR_SETTING_SETTING_ENABLED			"Enabled"
#define INSTALLMNGR_SETTING_SETTING_NAME			"Name"
//#define INSTALLMNGR_SETTING_SETTING_ROOT			"RootDirectory"
#define INSTALLMNGR_SETTING_SETTING_FILES			"Files"

class installationManager : public QObject
{
	Q_OBJECT

public:

	installationManager(QObject *parent);
	~installationManager();

	QString registerPlugin(QObject *plugin, const QString &sRootDirectory, const QString &sFileName, const bool &bIsEnabled, const bool &bIsStatic);
	bool unregisterPlugin(const QString &sFileName);
	bool isRegisteredPlugin(const QString &sFileBaseName);
	QStringList getRegisteredAndEnabledPluginNames();
	QMap<QString, bool> getRegisteredPluginNamesAndStates();
	QString getRegisteredPluginName(const QString &sPluginFileName);
	PluginInterface *getRegisteredPluginInterface(const QString &sRegisteredPluginName);
	int configureRegisteredPluginScriptEngine(const QString &sRegisteredPluginName, QScriptEngine *pEngine);
	PluginInterface::PluginType getPluginType(const QString &sRegisteredPluginName);
	QString getPluginAbsFilePath(const QString &sRegisteredPluginName);
	bool isEnabledPlugin(const QString &sPluginFilePath);
	bool isStaticPlugin(const QString &sRegisteredPluginName);
	bool unistallRegisteredPlugin(const QString &sRegisteredPluginName);
	bool installPlugin(const QString &sPluginInstallFilePath);
	bool changeRegisteredPlugin(const QString &sRegisteredPluginName, const bool &bEnable);
	QString getPluginIniFilePath(const QString &sPluginFilePath);
	bool createPluginConfigurationSetting(const QString &sPluginInstallFilePath, const QString &sInternalName, const bool &bIsEnabled, const QStringList lInstallationFiles);
	QStringList getPluginInstallFiles(const QString &sRegisteredPluginName);

private:
	bool changePluginEnabledSetting(const QString &sPluginIniFilePath, const bool &bEnable);
	QString getPluginFileName(const QString &sRegisteredPluginName);
	bool createRegisteredPluginIniFile(const QString &sRegisteredPluginName);

	struct strcPluginDefinition
	{
		QString sName;
		PluginInterface::PluginType pType;
		PluginInterface *pInterface;
		int nScriptMetaID;
		QScriptEngine *pScriptEngine;
		QString sFileBaseName;
		QString sFileName;
		QStringList lInstallFileList;
		bool bIsEnabled;
		bool bIsStatic;
		strcPluginDefinition()
		{
			bIsStatic = false;
			bIsEnabled = false;
			sName = "";
			sFileBaseName = "";
			sFileName = "";
			pInterface = NULL;
			pScriptEngine = NULL;
			nScriptMetaID = -1;
			pType = PluginInterface::UnknownPlugin;
		}
	};

	QMap<QString, strcPluginDefinition> mapRegisteredPluginNametoDef;
};

#endif // INSTALLATIONMANAGER_H
