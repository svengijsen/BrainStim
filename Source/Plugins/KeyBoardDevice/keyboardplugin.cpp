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


#include "keyboardplugin.h"
#include "defines.h"

Q_DECLARE_METATYPE(KeyBoardCapture*)
Q_DECLARE_METATYPE(KeyBoardCapture)

KeyBoardPlugin::KeyBoardPlugin(QObject *parent)
{
	KeyBoardObject = new KeyBoardCapture(parent);
	KeyBoardDiagObject = new KeyBoard_Dialog();
	strPluginInformation = PLUGIN_INFORMATION;
}

KeyBoardPlugin::~KeyBoardPlugin()
{
	delete KeyBoardDiagObject;
	delete KeyBoardObject;
}

QScriptValue ctor_KeyBoard(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	//	if (context->isCalledAsConstructor()) {
	//		// initialize the new object
	//		//context->thisObject().setProperty("bar", ...);
	//		// ...
	//		// return a non-object value to indicate that the
	//		// thisObject() should be the result of the "new Foo()" expression
	//		//return engine->undefinedValue();
	KeyBoardCapture *KeyBoardCaptureObj = new KeyBoardCapture();
	QScriptValue sVal = engine->newQObject(KeyBoardCaptureObj, QScriptEngine::ScriptOwnership);
	KeyBoardCaptureObj->setScriptEngine(engine);
	return sVal;
} 

int KeyBoardPlugin::ConfigureScriptEngine(QScriptEngine &engine)
{
	// example for letting the script do the work...
	//QScriptEngine eng;
	//testScriptObject *to = new testScriptObject(&eng);
	//QString scriptFile = "e:/Active Projects/KeyBoardDevicePlugin/Resources/testScriptObject.js";
	//to->runScript(scriptFile);

	// create and register the KeyBoard prototype and constructor in the engine
	//KeyBoard *KeyBoardObject = new KeyBoard();
	QScriptValue KeyBoardProto = engine.newQObject(KeyBoardObject);
	engine.setDefaultPrototype(qMetaTypeId<KeyBoardCapture*>(), KeyBoardProto);
	QScriptValue KeyBoardCtor = engine.newFunction(ctor_KeyBoard, KeyBoardProto);
	engine.globalObject().setProperty(PLUGIN_SCRIPTOBJECT_NAME, KeyBoardCtor);

	int nMetaType = qRegisterMetaType<KeyBoardCapture>(PLUGIN_SCRIPTOBJECT_CLASS);
	return nMetaType;
	//QScriptEngine eng;
	//QScriptValue obj = engine.newObject();
	//KeyBoardProto.setProperty("BaseAddress", engine.newFunction(getSetBaseAddress), 
	//	QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
}

bool KeyBoardPlugin::ShowGUI()
{
	int returnVal;
	KeyBoardDiagObject->setWindowTitle(strPluginInformation);
	returnVal = KeyBoardDiagObject->exec();

	switch (returnVal) {
	case QMessageBox::Save:
	   // Save was clicked
	   break;
	case QMessageBox::Discard:
		// Don't Save was clicked
	    break;
	case QMessageBox::Cancel:
	    // Cancel was clicked
	    break;
	default:
	    // should never be reached
	    break;
	}		
	return true;
}

//Q_PLUGIN_METADATA(keyboardplugin, KeyBoardPlugin);
