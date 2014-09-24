//KeyBoardplugin
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


#ifndef KeyBoardPLUGIN_H
#define KeyBoardPLUGIN_H

#include <QObject>
#include <QtWidgets>
#include <QString>
#include <Qlibrary>
#include "../../BrainStim/plugininterface.h"
#include "keyboard_dialog.h"
#include "keyboardcapture.h"
#include "defines.h"
#include "global.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class KeyBoardPlugin : public QObject, public DeviceInterface
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "BrainStim.Plugins.Devices" "keyboardcapture.json")
    Q_INTERFACES(DeviceInterface)

public:
	KeyBoardPlugin(QObject *parent = 0);
	~KeyBoardPlugin();

	int ConfigureScriptEngine(QScriptEngine &engine);
	QString GetMinimalMainProgramVersion() {return PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION;};

private:
	int nMethod;
	KeyBoardCapture *KeyBoardObject; 
	KeyBoard_Dialog *KeyBoardDiagObject;

public slots:
    bool ShowGUI();
	bool IsCompatible() {return PluginInterface::IsCompatible();};
	QObject *GetScriptMetaObject(int nIndex) {if(nIndex == 0) return (QObject *)KeyBoardObject->metaObject(); else return NULL;};
};

#endif//KeyBoardPLUGIN_H
