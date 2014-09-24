//ExperimentManagerplugin
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


#ifndef EXPERIMENTLOGGER_H
#define EXPERIMENTLOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include "experimenttimer.h"

class ExperimentLogger : public QObject
{
	Q_OBJECT

	typedef struct{
		QList<int> nObject;
		QList<int> nTimer;
		QList<QString> sFunction;
		QList<QString> sTag;
		QList<QString> sMessage;
		QList<QString> sValue;
		QList<double> dTime;
	} LoggedData;

public:
	ExperimentLogger(QObject *parent = NULL);
	~ExperimentLogger();

	int createTimer();
	bool startTimer(int nIndex);
	double restartTimer(int nIndex);
	double elapsedTimerTime(int nIndex);

	bool setLogVars(const int &nObjectIndex, const int &nTimerIndex, const QString &strFunction = "", const QString &strTag = "", const QString &strMessage = "",const QString &strValue = "");
	//bool getLogVars(QString &strWho, QString &strWhen, QString &strWhere);
	bool WriteToOutput(const QString &fileName);
	void Test();

private:
	LoggedData *internalLoggedDataStructure;
	//QList<QString> sWho;
	//QList<QString> sWhen;
	//QList<QString> sWhere;

	QList<ExperimentTimer*> tTimers;
};

#endif // EXPERIMENTLOGGER_H
