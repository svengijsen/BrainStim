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


#include "qtscriptengine.h"
#include "mainappinfo.h"

static bool wantsToQuit;

static QScriptValue qtscript_quit(QScriptContext *ctx, QScriptEngine *eng)
{
	Q_UNUSED(ctx);
	wantsToQuit = true;
	return eng->undefinedValue();
}

static QScriptValue qtscript_describe(QScriptContext *ctx, QScriptEngine *eng)
{
	QStringList result;
	QScriptValue obj = ctx->argument(0);
	while (obj.isObject()) {
		QScriptValueIterator it(obj);
		while (it.hasNext()) {
			it.next();
			result.append(it.name());
		}
		obj = obj.prototype();
	}
	return eng->toScriptValue(result);
}

static QScriptValue importExtension(QScriptContext *ctx, QScriptEngine *eng)
{
	return eng->importExtension(ctx->argument(0).toString());
}

bool QTScriptEngine::ImportScriptExtensions()
{
	QStringList extensions;
	extensions = MainAppInfo::getQTScriptBindingList();
	QStringList failExtensions;
	foreach (const QString &ext, extensions) {
		QScriptValue ret = eng->importExtension(ext);
		if (ret.isError())
		{
			failExtensions.append(ext);
			qWarning() << __FUNCTION__ << "Could not load: " << failExtensions[failExtensions.size()-1];
		}	
	}
	if (!failExtensions.isEmpty()) {
		if (failExtensions.size() == extensions.size()) {
			qWarning("Failed to import all Qt bindings!\n"
				"\tPlugins directory searched: %s/script\n"
				"\tMake sure that the bindings have been built,\n"
				"\tand that this executable and the plugins are using compatible Qt libraries.");
		} else {
			qWarning("Failed to import some(%d) Qt bindings:\n"
				"\tPlugins directory searched: %s/script\n"
				"\tMake sure that the bindings have been built,\n"
				"\tand that this executable and the plugins are using compatible Qt libraries.",failExtensions.size());//,
				//qPrintable(failExtensions.join(", ")));//, qPrintable(extensionsDir.absolutePath()));
		}
	}
	else
	{
		return true;
	}
	return false;
}

QTScriptEngine::QTScriptEngine(QObject *parent): QObject(parent)
{
	//debugger.attachTo(this);
	//debugger.action(QScriptEngineDebugger::InterruptAction)->trigger();
	isRunning = true;
	eng = new QScriptEngine();
	attachedToDebugger = false;

#if QT_VERSION >= 0x040500
	debugger = new QScriptEngineDebugger();
	//debugger->attachTo(eng);
#endif

	QScriptValue globalObject = eng->globalObject();
	globalObject.setProperty("App", eng->newQObject(parent));
	QScriptValue qScriptVal = eng->newObject();
	qScriptVal.setProperty("importExtension", eng->newFunction(importExtension));
	globalObject.property("qt").setProperty("script", qScriptVal);
	setAgent();
}

void QTScriptEngine::setAgent()
{
	agent = new QTScriptEngineAgent(eng);
	eng->setAgent(agent);	
	connect(agent, SIGNAL(ScriptUnloaded(qint64)), this, SLOT(scriptHasUnloaded(qint64)));
	connect(agent, SIGNAL(ScriptLoaded(qint64)), this, SLOT(scriptHasLoaded(qint64)));
}

void QTScriptEngine::scriptHasUnloaded(qint64 id)
{
	isRunning = false;
	emit ScriptUnloaded(id);
}

void QTScriptEngine::scriptHasLoaded(qint64 id)
{
	isRunning = true;
	emit ScriptLoaded(id);
}

//void QTScriptEngine::executeScript(const QString &script)
//{
//	worker = new QTScriptWorker(this);
//	worker->setScript(script);
//	worker->start();
//}

bool QTScriptEngine::prepareObjectForGarbageCleanup(const QScriptValue &ObjectScriptValue, const QString &sObjectNameTrigger)
{
	QScriptValue currentObjectScriptValue = ObjectScriptValue;
	//QString tmpString;
	bool bDoCheckFutureValues;
	if(sObjectNameTrigger.isEmpty())
		bDoCheckFutureValues = true;
	else
		bDoCheckFutureValues = false;

	while (currentObjectScriptValue.isObject())
	{
		QScriptValueIterator it(currentObjectScriptValue);
		while (it.hasNext()) 
		{
			it.next();
			if(bDoCheckFutureValues)
			{
				//tmpString = "name: " + it.name();
				if(currentObjectScriptValue.property(it.name()).isFunction())
				{
					if((it.value().toString().contains("[native code]") == false))
					{
						//tmpString = "name: " + it.name();
						//tmpString = tmpString + ",sname: " + it.scriptName();
						//tmpString = tmpString + ",value: " + it.value().toString();
						currentObjectScriptValue.setProperty(it.name(), QScriptValue::NullValue);
					}
				}
				else if(currentObjectScriptValue.property(it.name()).isObject())
				{
					prepareObjectForGarbageCleanup(currentObjectScriptValue.property(it.name()).prototype(),"");
					currentObjectScriptValue.setProperty(it.name(), QScriptValue::NullValue);
				}
			}
			else
			{
				if(it.name() == MAIN_PROGRAM_INTERNAL_NAME)
					bDoCheckFutureValues = true;
			}
		}
		currentObjectScriptValue = currentObjectScriptValue.prototype();
	}
	return true;
}

QMainWindow *QTScriptEngine::DebuggerStandardWindow()
{
	return debugger->standardWindow();
}

QMenu *QTScriptEngine::createDebuggerMenu(QWidget *parent)
{
	return debugger->createStandardMenu(parent);
}

QTScriptEngine::~QTScriptEngine()
{

}

void QTScriptEngine::ConfigureDebugger(bool state)
{
	if (state == true)
	{
		if (!attachedToDebugger) 
		{
			debugger->attachTo(eng);
			attachedToDebugger = true;
		}
	}
	else
	{
		if (attachedToDebugger) 
		{
			debugger->detach();
			attachedToDebugger = false;
			//setAgent();
		}
	}	
}

void QTScriptEngine::interactive(QScriptEngine *eng)
{
	QScriptValue global = eng->globalObject();
	QScriptValue quitFunction = eng->newFunction(qtscript_quit);
	if (!global.property(QLatin1String("exit")).isValid())
		global.setProperty(QLatin1String("exit"), quitFunction);
	if (!global.property(QLatin1String("quit")).isValid())
		global.setProperty(QLatin1String("quit"), quitFunction);
	wantsToQuit = false;
	global.setProperty(QLatin1String("describe"), eng->newFunction(qtscript_describe));

	QTextStream qin(stdin, QFile::ReadOnly);

	const char *qscript_prompt = "qs> ";
	const char *dot_prompt = ".... ";
	const char *prompt = qscript_prompt;

	QString code;

	forever {
		QString line;

		printf("%s", prompt);
		fflush(stdout);

		line = qin.readLine();
		if (line.isNull())
			break;

		code += line;
		code += QLatin1Char('\n');

		if (line.trimmed().isEmpty()) {
			continue;

		} else if (! eng->canEvaluate(code)) {
			prompt = dot_prompt;

		} else {
			QScriptValue result = eng->evaluate(code, QLatin1String("typein"));

			code.clear();
			prompt = qscript_prompt;

			if (! result.isUndefined())
				fprintf(stderr, "%s\n", qPrintable(result.toString()));

			if (wantsToQuit)
				break;
		}
	}
}