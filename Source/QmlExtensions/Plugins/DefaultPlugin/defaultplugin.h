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

#ifndef QDEFAULTQMLPLUGIN_H
#define QDEFAULTQMLPLUGIN_H

#include <QtQml/QtQml>

//class QtQuick2ApplicationViewer;

class QDefaultQmlPlugin : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "BrainStim.QML.Plugins.Default" "DefaultQMLPlugin")
	//Q_PLUGIN_METADATA(IID "BrainStim.QML.Plugins.Default" FILE defaultqmlplugin.json)

public:
	QDefaultQmlPlugin();

	void registerTypes(const char *uri);
	void initializeEngine(QQmlEngine * engine, const char * uri);

private:
	QObject *pBrainStimInterfaceObject;

};
#endif QDEFAULTQMLPLUGIN_H

