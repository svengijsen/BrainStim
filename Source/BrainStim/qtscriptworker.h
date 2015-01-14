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


#ifndef QTSCRIPTWORKER_H
#define QTSCRIPTWORKER_H

#include "mainappinfo.h"
#include <QThread>
#include <QtScript>
#if QT_VERSION >= 0x040500
#include <QtScriptTools/QtScriptTools>
#endif

#include "qtscriptengineagent.h"

class QTScriptWorker : public QThread
{
	Q_OBJECT

public:
	QTScriptWorker(QObject *parent);
	~QTScriptWorker();

	void setScript(const QString &script);

private:
	QString currentScript;
	QScriptEngine *eng;
	QScriptEngineDebugger *debugger;
	QTScriptEngineAgent *agent;

	bool ImportScriptExtensions();//QDir extensionsDir);

protected:
	virtual void run();
	
};

#endif // QTSCRIPTWORKER_H
