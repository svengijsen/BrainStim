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
#define INSTALLMNGR_SETTING_SETTING_ENABLED			"Enabled"

class installationManager : public QObject
{
	Q_OBJECT

public:

	installationManager(QObject *parent);
	~installationManager();

	QString registerPlugin(QObject *plugin, const QString &sAbsoluteFilePath);
	QStringList getRegisteredPluginNames();
	PluginInterface *getRegisteredPluginInterface(const QString &sRegisteredPluginName);
	int configureRegisteredPluginScriptEngine(const QString &sRegisteredPluginName, QScriptEngine *pEngine);
	PluginInterface::PluginType getPluginType(const QString &sRegisteredPluginName);
	QString getPluginAbsFilePath(const QString &sRegisteredPluginName);

	bool unistallRegisteredPlugin(const QString &sRegisteredPluginName);
	bool installRegisteredPlugin(const QString &sRegisteredPluginName);

private:
	struct strcPluginDefinition
	{
		QString sName;
		PluginInterface::PluginType pType;
		PluginInterface *pInterface;
		int nScriptMetaID;
		QScriptEngine *pScriptEngine;
		QString sAbsoluteFilePath;
		strcPluginDefinition()
		{
			sName = "";
			sAbsoluteFilePath = "";
			pInterface = NULL;
			pScriptEngine = NULL;
			nScriptMetaID = -1;
			pType = PluginInterface::UnknownPlugin;
		}
	};

	QMap<QString, strcPluginDefinition> mapRegisteredPluginNametoDef;
};

#endif // INSTALLATIONMANAGER_H
