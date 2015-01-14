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


#ifndef QTSCRIPTENGINEAGENT_H
#define QTSCRIPTENGINEAGENT_H

#include <QScriptEngineAgent>

class QTScriptEngineAgent : public QObject, public QScriptEngineAgent
{
    Q_OBJECT

public:
	QTScriptEngineAgent(QScriptEngine *parent);//QScriptEngine
	~QTScriptEngineAgent();

	void scriptLoad(qint64 id, const QString &program, const QString &fileName, int baseLineNumber);
	void scriptUnload(qint64 id);
	void contextPush();
	void contextPop();
	void functionEntry(qint64 scriptId);
	void functionExit(qint64 scriptId,const QScriptValue &returnValue);
	void positionChange(qint64 scriptId,int lineNumber, int columnNumber);
	void exceptionThrow(qint64 scriptId,const QScriptValue &exception,bool hasHandler);
	void exceptionCatch(qint64 scriptId,const QScriptValue &exception);
	bool supportsExtension(Extension extension) const;
	QVariant extension(Extension extension,	const QVariant &argument = QVariant());

signals:
	void ScriptUnloaded(qint64 id);
	void ScriptLoaded(qint64 id);
};

#endif // QTSCRIPTENGINEAGENT_H
