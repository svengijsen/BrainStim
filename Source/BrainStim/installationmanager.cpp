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

installationManager::installationManager(QObject *parent) : QObject(parent)
{

}

installationManager::~installationManager()
{

}

QString installationManager::registerPlugin(QObject *plugin, const QString &sAbsoluteFilePath)
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
	tmpStrcPluginDefinition.sAbsoluteFilePath = sAbsoluteFilePath;
	tmpStrcPluginDefinition.sName = tmpStrcPluginDefinition.pInterface->GetPluginInternalName();
	if (tmpStrcPluginDefinition.sName.isEmpty()==false)
		mapRegisteredPluginNametoDef.insert(tmpStrcPluginDefinition.sName, tmpStrcPluginDefinition);
	return tmpStrcPluginDefinition.sName;
}

QStringList installationManager::getRegisteredPluginNames()
{
	return mapRegisteredPluginNametoDef.keys();
}

PluginInterface *installationManager::getRegisteredPluginInterface(const QString &sRegisteredPluginName)
{
	if (mapRegisteredPluginNametoDef.contains(sRegisteredPluginName))
		return mapRegisteredPluginNametoDef.value(sRegisteredPluginName).pInterface;
	return NULL;
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
		return mapRegisteredPluginNametoDef.value(sRegisteredPluginName).sAbsoluteFilePath;
	}
	return "";
}

bool installationManager::unistallRegisteredPlugin(const QString &sRegisteredPluginName)
{ 
	QFileInfo fiPluginPath(getPluginAbsFilePath(sRegisteredPluginName));
	if (fiPluginPath.exists())
	{
		QString sPluginSettingsFilePath = fiPluginPath.canonicalPath() + "/" + fiPluginPath.completeBaseName() + ".ini";
		if (QFile(sPluginSettingsFilePath).exists())
		{
			QSettings pluginSettings(sPluginSettingsFilePath, QSettings::IniFormat);
			pluginSettings.beginGroup(INSTALLMNGR_SETTING_SECTION_CONFIGURATION);
			pluginSettings.setValue(INSTALLMNGR_SETTING_SETTING_ENABLED, true);
			pluginSettings.endGroup();
			return true;
		}
	}
	return false;
}

bool installationManager::installRegisteredPlugin(const QString &sRegisteredPluginName)
{
	return true;
}
