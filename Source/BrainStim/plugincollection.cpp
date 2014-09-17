//BrainStim
//Copyright (C) 2014  Sven Gijsen
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


#include "plugincollection.h"

PluginCollection::PluginCollection(QObject *parent): QObject(parent)
{
	MaxPlugins = 100;
	NrofPlugins = 0;
}
PluginCollection::~PluginCollection()
{
	if (!IsEmpty())
	{
		Cleanup();
	}
}
PluginInterface *PluginCollection::GetInterface(int index)
{
	if ((index<NrofPlugins) && (index >= 0))
	{
		switch (PlgTypes.at(index))
		{
		case DevicePlugin:
			{
				return qobject_cast<DeviceInterface *>(PlgInterfaces.at(index));
				break;
			}
		case ExtensionPlugin:
			{
				return qobject_cast<ExtensionInterface *>(PlgInterfaces.at(index));
				break;
			}
		default:
			{
				return 0;
				break;
			}	
		}
	} else {
		return 0;
	}
}

bool PluginCollection::Cleanup()
{
	if (NrofPlugins==0)
		return true;
	for (int i=0;i<NrofPlugins;i++)
	{
		if(PlgTypes.at(i) == DevicePlugin)
		{
			qobject_cast<DeviceInterface *>(PlgInterfaces.at(i))->deleteGlobalAppInfo();
		}
		else if(PlgTypes.at(i) == ExtensionPlugin)
		{
			qobject_cast<ExtensionInterface *>(PlgInterfaces.at(i))->deleteGlobalAppInfo();
		}
		delete PlgInterfaces.at(i);
	}
	PlgNames.clear();
	NrofPlugins=0;
	return true;
}

int PluginCollection::Add(QObject *plugin)
{
	QString name;
	DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);//For each plugin (static or dynamic), we check which interfaces it implements using qobject_cast()
	if (iDevice) 
	{
		name = "Device";
		PlgTypes.append(DevicePlugin);
	}

	ExtensionInterface *iExtension = qobject_cast<ExtensionInterface *>(plugin);//For each plugin (static or dynamic), we check which interfaces it implements using qobject_cast()
	if (iExtension) 
	{
		name = "Extension";
		PlgTypes.append(ExtensionPlugin);
	}

	if (NrofPlugins!=MaxPlugins)
	{
		int LastItem;
		PlgInterfaces.append(plugin);
		QRegExp rx(name + " *",Qt::CaseInsensitive,QRegExp::Wildcard); //+ "[*]"    
		LastItem = PlgNames.lastIndexOf(rx);
		if (LastItem == -1)
		{
			PlgNames.append(name + " [0]");
		} else {
			int LastIndex;
			int TempIndex;
			TempIndex = PlgNames.at(LastItem).indexOf("[");
			LastIndex = PlgNames.at(LastItem).mid(TempIndex+1,(PlgNames.at(LastItem).length() - TempIndex-2)).toUInt();
			PlgNames.append(name + " [" + QString::number(LastIndex+1) + "]");
		}				
		NrofPlugins++;
		return NrofPlugins-1;
	} else {
		return -1;
	}
}

bool PluginCollection::IsEmpty()
{
	return (NrofPlugins == 0);
}

int PluginCollection::Count()
{
	return (NrofPlugins);
}

QString PluginCollection::GetLoaderName(int index)//the uniquely generated name!! not the device name
{
	if ((index<NrofPlugins) && (index >= 0))
	{
		return PlgNames.at(index);
	} else {
		return "";
	}
}

int PluginCollection::GetIndex(QString name)//the uniquely generated name!! not the device name
{
	return (PlgNames.indexOf(name));  
}

