//ExperimentManagerplugin
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


#include "experimentlogger.h"

ExperimentLogger::ExperimentLogger(QObject *parent) : QObject(parent)
{
	internalLoggedDataStructure = NULL;
}

ExperimentLogger::~ExperimentLogger()
{
	if (internalLoggedDataStructure)
	{
		delete internalLoggedDataStructure;
	}
	if (!tTimers.isEmpty())
	{
		for (int i=0;i<tTimers.count();i++)
		{
			delete tTimers[i];
		}
	}
}

bool ExperimentLogger::setLogVars(const int &nObjectIndex, const int &nTimerIndex, const QString &strFunction, const QString &strTag, const QString &strMessage, const QString &strValue)
{
	if (internalLoggedDataStructure == NULL)
	{
		internalLoggedDataStructure = new LoggedData();
	}
	internalLoggedDataStructure->nObject.append(nObjectIndex);
	internalLoggedDataStructure->nTimer.append(nTimerIndex);
	internalLoggedDataStructure->sFunction.append(strFunction);
	internalLoggedDataStructure->sTag.append(strTag);
	internalLoggedDataStructure->sMessage.append(strMessage);
	internalLoggedDataStructure->sValue.append(strValue);
	if (nTimerIndex >= 0)
		internalLoggedDataStructure->dTime.append(tTimers.at(nTimerIndex)->getElapsedTimeInMilliSec());
	else
		internalLoggedDataStructure->dTime.append(0.0f);
	return true;
}

bool ExperimentLogger::WriteToOutput(const QString &fileName)
{
	if (internalLoggedDataStructure)
	{
		if (fileName.isEmpty())
			return false;

		QFile file(fileName);
		if (!file.open(QFile::WriteOnly | QFile::Text | QIODevice::Truncate)) 
			return false;
		QTextStream out(&file);//QDataStream
		out << "Object" << "\t" << "Timer" << "\t" << "Function" << "\t" << "Tag" << "\t" << "Message" << "\t" << "Value" << "\t" << "Time" << "\n";
		int nCount = 0;
		nCount = internalLoggedDataStructure->nObject.count();
		if(nCount > 0)
		{
			for (int i=0; i < nCount; i++)
				out << internalLoggedDataStructure->nObject.at(i) << "\t" <<
						internalLoggedDataStructure->nTimer.at(i) << "\t" <<
						internalLoggedDataStructure->sFunction.at(i) << "\t" <<
						internalLoggedDataStructure->sTag.at(i) << "\t" <<
						internalLoggedDataStructure->sMessage.at(i) << "\t" <<
						internalLoggedDataStructure->sValue.at(i) << "\t" <<
						internalLoggedDataStructure->dTime.at(i) << "\n";
		}
		file.close();
		return true;
	}
	return false;
}

int ExperimentLogger::createTimer()
{
	tTimers.append(new ExperimentTimer());
	return tTimers.count()-1;
}

bool ExperimentLogger::startTimer(int nIndex)
{
	if (!tTimers.isEmpty())
	{
		if (tTimers.count() > nIndex)
		{
			tTimers[nIndex]->start();
			return true;
		}
	}
	return false;
}

double ExperimentLogger::restartTimer(int nIndex)
{
	if (!tTimers.isEmpty())
	{
		if (tTimers.count() > nIndex)
		{
			double RetVal = tTimers[nIndex]->getElapsedTimeInMilliSec();
			tTimers[nIndex]->stop();
			tTimers[nIndex]->start();
			return RetVal;
		}
	}
	return -1;
}

double ExperimentLogger::elapsedTimerTime(int nIndex)
{
	if (!tTimers.isEmpty())
	{
		if (tTimers.count() > nIndex)
		{
			return tTimers[nIndex]->getElapsedTimeInMilliSec();
		}
	}
	return -1;
}


