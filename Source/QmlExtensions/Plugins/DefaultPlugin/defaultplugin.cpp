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
#include "brainstimitem.h"
#include "brainstiminterface.h"
//#include "../../../Plugins/ExperimentManager/qtquick2applicationviewer.h"
#include "../../../Plugins/ExperimentManager/qml2viewer.h"

QDefaultQmlPlugin::QDefaultQmlPlugin()
{
	pBrainStimInterfaceObject = NULL;
}

void QDefaultQmlPlugin::registerTypes(const char *uri)
{
	Q_ASSERT(uri == QLatin1String(DEFAULT_BRAINSTIM_PLUGIN_QML_NAME));
	int nTypeID = -1;
	//nTypeID = qmlRegisterType<TimeModel, TIMEMODEL_VERSION_STRING_REVISION>(uri, QML_VERSION_STRING_MAJOR, QML_VERSION_STRING_MINOR, TIMEMODEL_TYPE_QML_NAME);
	nTypeID = qmlRegisterType<OgreItem, OGRE3DITEM_VERSION_STRING_REVISION>(uri, QML_VERSION_STRING_MAJOR, QML_VERSION_STRING_MINOR, OGRE3DITEM_TYPE_QML_NAME);
	//nTypeID = qmlRegisterType<BrainStimItem, BRAINSTIMITEM_VERSION_STRING_REVISION>(uri, QML_VERSION_STRING_MAJOR, QML_VERSION_STRING_MINOR, BRAINSTIMITEM_TYPE_QML_NAME);
	nTypeID = qmlRegisterSingletonType<BrainStimInterface>(uri, PLUGIN_BRAINSTIMINTERFACE_PROVIDER_MAJOR, PLUGIN_BRAINSTIMINTERFACE_PROVIDER_MINOR, PLUGIN_BRAINSTIM_PROVIDER_APINAME, interface_singletontype_provider);
}

void QDefaultQmlPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{

	QObject *engineParent = engine->parent();
	if (engineParent)
	{
		/*
		QString sName = engineParent->objectName();
		//QtQuick2ApplicationViewer *tmpParentViewer = qobject_cast<QtQuick2ApplicationViewer*>(engineParent);
		QtQuick2ApplicationViewer *tmpParentViewer = (QtQuick2ApplicationViewer*)engineParent;
		QObject *parentQMLViewerObj = tmpParentViewer->getParentObject();
		QML2Viewer *parentQMLViewer = (QML2Viewer*)parentQMLViewerObj;//qobject_cast<QML2Viewer*>(parentQMLViewerObj);
		if (parentQMLViewer)
		{
			QObject* singletonBSInterfaceObject = interface_singletontype_provider(NULL, NULL);
			if (singletonBSInterfaceObject)
			{
				((BrainStimInterface*)singletonBSInterfaceObject)->setQMLViewerObject(parentQMLViewer);
			}
		}
		*/
	}
}