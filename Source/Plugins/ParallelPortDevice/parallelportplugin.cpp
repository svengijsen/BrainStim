//parallelportplugin
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


#include "parallelportplugin.h"
#include "defines.h"

Q_DECLARE_METATYPE(ParallelPort*)
Q_DECLARE_METATYPE(ParallelPort)

ParallelPortPlugin::ParallelPortPlugin(short BaseAddress, QObject *parent)
{
	nBaseAddress = BaseAddress;
	PPortObject = new ParallelPort(nBaseAddress,parent);
	PPortDiagObject = new PPort_Dialog();
	strPluginInformation = PLUGIN_INFORMATION;
}

ParallelPortPlugin::~ParallelPortPlugin()
{
	delete PPortDiagObject;
	delete PPortObject;
}

QScriptValue ctor_ParallelPort(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	//	if (context->isCalledAsConstructor()) {
	//		// initialize the new object
	//		//context->thisObject().setProperty("bar", ...);
	//		// ...
	//		// return a non-object value to indicate that the
	//		// thisObject() should be the result of the "new Foo()" expression
	//		//return engine->undefinedValue();

	return engine->newQObject(new ParallelPort(), QScriptEngine::ScriptOwnership);
} 

int ParallelPortPlugin::ConfigureScriptEngine(QScriptEngine &engine)
{
	// example for letting the script do the work...
	//QScriptEngine eng;
	//testScriptObject *to = new testScriptObject(&eng);
	//QString scriptFile = "e:/Active Projects/ParallelPortDevicePlugin/Resources/testScriptObject.js";
	//to->runScript(scriptFile);

	// create and register the ParallelPort prototype and constructor in the engine
	//ParallelPort *ParallelPortObject = new ParallelPort();
	QScriptValue ParallelPortProto = engine.newQObject(PPortObject);
	engine.setDefaultPrototype(qMetaTypeId<ParallelPort*>(), ParallelPortProto);
	QScriptValue ParallelPortCtor = engine.newFunction(ctor_ParallelPort, ParallelPortProto);
	engine.globalObject().setProperty(PLUGIN_SCRIPTOBJECT_NAME, ParallelPortCtor);
	int nMetaType = qRegisterMetaType<ParallelPort>(PLUGIN_SCRIPTOBJECT_CLASS);
	//QScriptEngine eng;
	//QScriptValue obj = engine.newObject();
	//ParallelPortProto.setProperty("BaseAddress", engine.newFunction(getSetBaseAddress), 
	//	QScriptValue::PropertyGetter|QScriptValue::PropertySetter);
	return nMetaType;
}

bool ParallelPortPlugin::ShowGUI()
{
	int returnVal;
	PPortDiagObject->setWindowTitle(strPluginInformation);
	returnVal = PPortDiagObject->exec();

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

