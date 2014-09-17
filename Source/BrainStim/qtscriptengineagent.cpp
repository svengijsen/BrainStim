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


#include "qtscriptengineagent.h"
#include <QScriptValue>

QTScriptEngineAgent::QTScriptEngineAgent(QScriptEngine *parent) : QScriptEngineAgent(parent) 
{	

}

QTScriptEngineAgent::~QTScriptEngineAgent()
{

}


void QTScriptEngineAgent::scriptLoad(qint64 id, const QString &program, const QString &fileName, int baseLineNumber)
{
	Q_UNUSED(baseLineNumber);
	Q_UNUSED(fileName);
	Q_UNUSED(program);
	emit ScriptLoaded(id);
}

void QTScriptEngineAgent::scriptUnload(qint64 id)
{
	emit ScriptUnloaded(id);
}

void QTScriptEngineAgent::contextPush()
{

}

void QTScriptEngineAgent::contextPop()
{

}

void QTScriptEngineAgent::functionEntry(qint64 scriptId)
{
	QString A = QString::number(scriptId);
}

void QTScriptEngineAgent::functionExit(qint64 scriptId,const QScriptValue &returnValue)
{
	Q_UNUSED(scriptId);
	Q_UNUSED(returnValue);
}

void QTScriptEngineAgent::positionChange(qint64 scriptId,int lineNumber, int columnNumber)
{
	Q_UNUSED(scriptId);
	Q_UNUSED(lineNumber);
	Q_UNUSED(columnNumber);
}

void QTScriptEngineAgent::exceptionThrow(qint64 scriptId, const QScriptValue &exception, bool hasHandler)
{
	Q_UNUSED(scriptId);
	Q_UNUSED(hasHandler);
	Q_UNUSED(exception);
}

void QTScriptEngineAgent::exceptionCatch(qint64 scriptId, const QScriptValue &exception)
{
	Q_UNUSED(scriptId);
	Q_UNUSED(exception);
}

bool QTScriptEngineAgent::supportsExtension(Extension extension) const
{
	Q_UNUSED(extension);
	return true;
}

QVariant QTScriptEngineAgent::extension(Extension extension, const QVariant &argument)
{
	Q_UNUSED(extension);
	Q_UNUSED(argument);
	return QVariant(NULL);
}