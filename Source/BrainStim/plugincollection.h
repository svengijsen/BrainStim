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


#ifndef PLUGINCOLLECTION_H
#define PLUGINCOLLECTION_H

#include <QObject>
#include <QStringList>
#include <QRegExp>
#include "plugininterface.h"

class PluginCollection : public QObject
{
	Q_OBJECT

public:

	enum PluginType
	{
		DevicePlugin		    = 0,
		ExtensionPlugin			= 1
	};
	Q_DECLARE_FLAGS(PluginTypes, PluginType)

	PluginCollection(QObject *parent);
	~PluginCollection();

	int Add(QObject *plugin);
	bool IsEmpty();
	int Count();
	QString GetLoaderName(int index);
	int GetIndex(QString name);
	PluginInterface *GetInterface(int index);

private:
	bool Cleanup();

	int MaxPlugins;
	int NrofPlugins;
	QList<QObject *> PlgInterfaces;
	QList<int> PlgTypes;
	QStringList PlgNames;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(PluginCollection::PluginTypes)

#endif // PLUGINCOLLECTION_H
