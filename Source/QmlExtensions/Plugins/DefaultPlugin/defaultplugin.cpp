//DefaultQMLPlugin
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

#include "defaultplugin.h"
#include "defines.h"
#include "timemodel.h"
#include "ogreitem.h"

void QDefaultQmlPlugin::registerTypes(const char *uri)
{
	Q_ASSERT(uri == QLatin1String(DEFAULT_BRAINSTIM_PLUGIN_QML_NAME));
	int nTypeID=-1;
	//nTypeID = qmlRegisterType<TimeModel,TIMEMODEL_VERSION_STRING_REVISION>(uri, QML_VERSION_STRING_MAJOR, QML_VERSION_STRING_MINOR, TIMEMODEL_TYPE_QML_NAME);
	nTypeID = qmlRegisterType<OgreItem,OGRE3DITEM_VERSION_STRING_REVISION>(uri, QML_VERSION_STRING_MAJOR, QML_VERSION_STRING_MINOR, OGRE3DITEM_TYPE_QML_NAME);
}