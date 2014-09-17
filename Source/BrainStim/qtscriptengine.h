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


#ifndef QTSCRIPTENGINE_H
#define QTSCRIPTENGINE_H

#include <QObject>
#include <QtScript>
#if QT_VERSION >= 0x040500
#include <QtScriptTools/QtScriptTools>
#endif
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QApplication>
#include "qtscriptengineagent.h"

class QTScriptEngine : public QObject
{
	Q_OBJECT

public:
	QTScriptEngine(QObject *parent);
	~QTScriptEngine();

	QScriptEngine *eng;
	QTScriptEngineAgent *agent;
	QScriptEngineDebugger *debugger;

	void ConfigureDebugger(bool state);
	QMenu *createDebuggerMenu(QWidget *parent = 0);
	QMainWindow *DebuggerStandardWindow(); 
	bool ImportScriptExtensions();
	bool prepareObjectForGarbageCleanup(const QScriptValue &ObjectScriptValue, const QString &sObjectNameTrigger = QString());

protected:

public slots:
	void scriptHasUnloaded(qint64 id);
	void scriptHasLoaded(qint64 id);

private:
	void setAgent();

	void interactive(QScriptEngine *eng);
	bool attachedToDebugger;
	bool isRunning;

signals:
	void ScriptLoaded(qint64 scriptID);
	void ScriptUnloaded(qint64 scriptID);

};

#endif // QTSCRIPTENGINE_H
