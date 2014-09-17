//PrtFormatManager
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


#include "prtformatmanager.h"

QScriptValue PrtFormatManager::ctor__PrtFormatManager(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new PrtFormatManager(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
} 

/*! \brief The PrtFormatManager constructor.
*
*   You do not need to specify the parent object. 
*	The BrainStim script engine automatically retrieves the parent role
*/
PrtFormatManager::PrtFormatManager(QObject *parent)	: QObject(parent)
{
	currentScriptEngine = NULL;
	clearAll();
}

bool PrtFormatManager::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

/*! \brief The PrtFormatManager destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
PrtFormatManager::~PrtFormatManager()
{
	clearAll();
}

bool PrtFormatManager::clearParameters() 
{
	return setDefaultParameters(PRT_DEFAULT_VERSION);
}

bool PrtFormatManager::clearConditions()
{
	if (m_PRTConditions.isEmpty() == false)
	{
		for (int i=0;i<m_PRTConditions.count();i++)
		{
			if (m_PRTConditions.at(i).Intervals.isEmpty() == false)
			{
				m_PRTConditions[i].Intervals.clear();
			}
		}
		m_PRTConditions.clear();
	}
	return true;
}

bool PrtFormatManager::clearAll() 
{
	return (clearConditions() && clearParameters());
}

bool PrtFormatManager::setDefaultParameters(const int nVersion)
{
	PRTDefinitionParameters tmpPRTDefinition;
	m_AllowedParameterNames.clear();

	switch (nVersion) {
	case 2:
		m_AllowedParameterNames.append("FileVersion");
		setParameterValue(m_AllowedParameterNames.last(),QString::number(nVersion),"File version (new entry since BrainVoyager v4.2)");
		m_AllowedParameterNames.append("ResolutionOfTime");
		setParameterValue(m_AllowedParameterNames.last(),"Volumes","Volumes or msec (new entry since BrainVoyager v4.2)");
		m_AllowedParameterNames.append("Experiment");
		setParameterValue(m_AllowedParameterNames.last(),"ExperimentName","Name of conducted experiment");
		m_AllowedParameterNames.append("BackgroundColor");
		setParameterValue(m_AllowedParameterNames.last(),"0 0 0","RGB components for background color of time course plots");			
		m_AllowedParameterNames.append("TextColor");
		setParameterValue(m_AllowedParameterNames.last(),"255 255 217","RGB components for text color, i.e. axis labels");
		m_AllowedParameterNames.append("TimeCourseColor");
		setParameterValue(m_AllowedParameterNames.last(),"255 255 255","RGB components for line color of time course plots");
		m_AllowedParameterNames.append("TimeCourseThick");
		setParameterValue(m_AllowedParameterNames.last(),"3","Line thickness of time course");
		m_AllowedParameterNames.append("ReferenceFuncColor");
		setParameterValue(m_AllowedParameterNames.last(),"255 255 51","RGB components for line color of reference time course plot");
		m_AllowedParameterNames.append("ReferenceFuncThick");
		setParameterValue(m_AllowedParameterNames.last(),"2","Line thickness of reference time course");
		m_AllowedParameterNames.append(PRT_LAST_PARAM_NAME);
		setParameterValue(m_AllowedParameterNames.last(),"0","Number of conditions within run");
		return true;
		break;	
	default: 
		return false;
		break;
	}
	return true;
}

bool PrtFormatManager::loadFile(const QString sFileName)
{
	bool bProceedWithConditions = false;
	bool bRetVal = false;
	QString tmpString;
	QFile fileSource(sFileName);
	QFileInfo fileSourceInfo(sFileName);

	clearAll();
	if (!fileSource.exists())
		return bRetVal;
	if(fileSource.open(QIODevice::ReadOnly))
	{
		QStringList strLineElements;
		QTextStream input(&fileSource);
		QString strLine;
		int nNumberOfConditions = 0;
		int nCurrentConditionIndex = -1;
		int nNumberOfIntervals = 0;
		int nConditionStep = 0;
		
		do 
		{
			strLine = input.readLine();

			if (bProceedWithConditions == false)
			{
				//Read the parameters
				strLineElements = strLine.split(":",QString::SkipEmptyParts,Qt::CaseSensitive);
				if (strLineElements.count()>1)
				{
					tmpString = strLineElements.at(0).trimmed();
					if(m_AllowedParameterNames.contains(tmpString))
					{
						setParameterValue(tmpString,strLineElements.at(1).trimmed());
						if(tmpString == PRT_LAST_PARAM_NAME)
						{
							bProceedWithConditions = true;
							nNumberOfConditions = strLineElements.at(1).trimmed().toInt();
							if(nNumberOfConditions == 0)
							{
								nConditionStep = 99;
								bRetVal = true;
							}

						}
					}
				}
			}
			else
			{
				//Read the conditions
				switch (nConditionStep)
				{
				case 0://Naming
					strLineElements = strLine.trimmed().split(" ",QString::SkipEmptyParts,Qt::CaseSensitive);
					if(strLineElements.count() == 1)
					{
						nCurrentConditionIndex = appendCondition(strLineElements.at(0).trimmed());
						if(nCurrentConditionIndex >= 0)
							nConditionStep++;
					}
					break;
				case 1://NrOfIntervals
					strLineElements = strLine.trimmed().split(" ",QString::SkipEmptyParts,Qt::CaseSensitive);
					if(strLineElements.count() == 1)
					{
						nNumberOfIntervals = strLineElements.at(0).trimmed().toInt();
						nConditionStep++;
					}
					break;
				case 2://Intervals
					strLineElements = strLine.trimmed().split(" ",QString::SkipEmptyParts,Qt::CaseSensitive);
					if(strLineElements.count() == 2)
					{
						if(appendInterval(nCurrentConditionIndex,strLineElements.at(0).trimmed().toInt(),strLineElements.at(1).trimmed().toInt()) == (nNumberOfIntervals-1))
							nConditionStep++;
					}
					break;
				case 3://Coloring
					strLineElements = strLine.trimmed().split(":",QString::SkipEmptyParts,Qt::CaseSensitive);
					if((strLineElements.count() == 2) && (strLineElements.at(0).trimmed() == PRT_COLOR_PARAM_NAME))
					{
						if(setConditionColor(nCurrentConditionIndex,strLineElements.at(1).trimmed()))
						{
							nConditionStep = 0;	
							if(nNumberOfConditions == (nCurrentConditionIndex + 1))
							{
								nConditionStep = 99;
								bRetVal = true;
							}
						}
					}
					break;
				case 99://Finished nothing else to do...
					break;
				}				
			}
		} 
		while (!strLine.isNull());

		fileSource.close();
		return bRetVal;
	}
	return bRetVal;
}

bool PrtFormatManager::saveFile(const QString sFileName, const bool bOverWrite)
{
	//bool bProceedWithConditions = false;
	bool bRetVal = false;
	int i,j;
	//QString tmpString;
	QFile fileDest(sFileName);
	QFileInfo fileDestInfo(sFileName);

	if (fileDest.exists() && (bOverWrite == false))
		return bRetVal;
	if(fileDest.open(QIODevice::WriteOnly))
	{
		QTextStream output(&fileDest);

		//First write the parameters
		for (i=0;i<m_AllowedParameterNames.count();i++)
		{
			if(m_PRTParameters.contains(m_AllowedParameterNames.at(i)))
			{
				output << m_AllowedParameterNames.at(i) << QString(": ") << m_PRTParameters.value(m_AllowedParameterNames.at(i)).sValue << '\n' << flush;
			}
		}

		output << '\n' << flush;
		output << '\n' << flush;

		//Now write the conditions
		for (i=0;i<m_PRTConditions.count();i++)
		{
			output << m_PRTConditions.at(i).sName << '\n' << flush;
			output << QString::number(m_PRTConditions.at(i).Intervals.count()) << '\n' << flush;
			for (j=0;j<m_PRTConditions.at(i).Intervals.count();j++)
			{
				output << QString::number(m_PRTConditions.at(i).Intervals.at(j).nBegin) << QString(" ") << QString::number(m_PRTConditions.at(i).Intervals.at(j).nEnd) << '\n' << flush;
			}
			output << PRT_COLOR_PARAM_NAME << QString(": ") << m_PRTConditions.at(i).sColor << '\n' << flush;
			output << '\n' << flush;
		}
		bRetVal = true;
		fileDest.close();
		return bRetVal;
	}
	return bRetVal;
}

bool PrtFormatManager::setParameterValue(const QString sParamName, const QString sParamValue, const QString sParamDescription)
{
	if(m_AllowedParameterNames.contains(sParamName) == false)
		return false;
	if(sParamDescription != PRT_UNDEF_DESC_STRING)
	{
		tmpPRTDefinitionParameters.sDescription = sParamDescription;
	}
	else
	{
		tmpPRTDefinitionParameters.sDescription = getParameter(sParamName).sDescription;
	}
	tmpPRTDefinitionParameters.sValue = sParamValue;
	m_PRTParameters.insert(sParamName,tmpPRTDefinitionParameters);
	return true;
}

QString PrtFormatManager::getParameterValue(const QString sParamName)
{
	tmpPRTDefinitionParameters = getParameter(sParamName);
	return tmpPRTDefinitionParameters.sValue;
}

QString PrtFormatManager::getParameterDescription(const QString sParamName)
{
	tmpPRTDefinitionParameters = getParameter(sParamName);
	return tmpPRTDefinitionParameters.sDescription;
}

PRTDefinitionParameters PrtFormatManager::getParameter(const QString sParamName)
{
	PRTDefinitionParameters newPRTDefinition;
	if(m_PRTParameters.contains(sParamName))
	{
		return m_PRTParameters.value(sParamName,newPRTDefinition);
	}
	return newPRTDefinition;
}

int PrtFormatManager::appendCondition(const QString sConditionName, const QString sConditionColor)
{
	tmpConditionsDefinition.sName = sConditionName;
	tmpConditionsDefinition.sColor = sConditionColor;
	//tmpIntervalsDefinition.nBegin = 0;
	//tmpIntervalsDefinition.nEnd = 1;
	//tmpConditionsDefinition.Intervals.append(tmpIntervalsDefinition);
	m_PRTConditions.append(tmpConditionsDefinition);
	setParameterValue("NrOfConditions",QString::number(m_PRTConditions.count()),"Number of conditions within run");
	return m_PRTConditions.count()-1;
}

int PrtFormatManager::appendInterval(const int nConditionIndex, const int nBegin, const int nEnd)
{
	if (m_PRTConditions.isEmpty() == false)
	{
		if (m_PRTConditions.count() > nConditionIndex)
		{
			tmpIntervalsDefinition.nBegin = nBegin;
			tmpIntervalsDefinition.nEnd = nEnd;
			m_PRTConditions[nConditionIndex].Intervals.append(tmpIntervalsDefinition);
			return m_PRTConditions[nConditionIndex].Intervals.count()-1;
		}
	} 
	return -1;
	//tmpPRTDefinitionParameters = getParameter(sParamName);
	//return tmpPRTDefinitionParameters.sDescription;
}

bool PrtFormatManager::setConditionColor(const int nConditionIndex, const QString sColor)
{
	if (m_PRTConditions.isEmpty() == false)
	{
		if (m_PRTConditions.count() > nConditionIndex)
		{
			m_PRTConditions[nConditionIndex].sColor = sColor;
			return true;
		}
	}
	return false;
}

QString PrtFormatManager::getRGBPaletteColorString(const int nSteps, const int nIndex)
{
	QString sRetVal = "0 0 0";
	QColor tmpColor;

	if (nIndex < nSteps)
	{
		tmpColor.setHsv((int)((360.0/nSteps) * nIndex),255,255,255);
		sRetVal = QString::number(tmpColor.red()) + " " + QString::number(tmpColor.green()) + " " + QString::number(tmpColor.blue());
	}
	return sRetVal;
}

bool PrtFormatManager::SortandMergeAllConditionIntervals()
{
	if (m_PRTConditions.isEmpty() == false)
	{
		int nLastIndexZeroBasedPosition;
		int nTmpBegin;
		int nTmpEnd;
		int nStartSearchIndex;
		int i,j;
		QString tmpIntervals;
		bool bExitLoop;

		for (i=0;i<m_PRTConditions.count();i++)
		{
			nLastIndexZeroBasedPosition = -1;
			nStartSearchIndex = 0;
			tmpIntervals = "";
			if (m_PRTConditions.at(i).Intervals.isEmpty()== false)
			{
				for (j=0;j<m_PRTConditions.at(i).Intervals.count();j++)
				{
					nTmpBegin = m_PRTConditions.at(i).Intervals.at(j).nBegin;
					nTmpEnd = m_PRTConditions.at(i).Intervals.at(j).nEnd;
					if (nTmpEnd >= nTmpBegin)
					{
						if((nTmpEnd-1) > nLastIndexZeroBasedPosition)//Should the temp string grow?
						{
							nLastIndexZeroBasedPosition = (nTmpEnd-1);
							tmpIntervals = tmpIntervals.leftJustified(nLastIndexZeroBasedPosition+1,'0');
						}
						tmpIntervals.replace(nTmpBegin-1,nTmpEnd+1-nTmpBegin,QString(nTmpEnd+1-nTmpBegin,'1'));
					}
				}

				//Clear the Intervals..
				m_PRTConditions[i].Intervals.clear();
				
				if (tmpIntervals != "")
				{
					bExitLoop = false;
					//Fill the Intervals again but now with the correct optimized blocks
					do 
					{
						nTmpBegin = tmpIntervals.indexOf('1',nStartSearchIndex);
						if (nTmpBegin >= 0)
						{
							nStartSearchIndex = nTmpBegin;
							nTmpEnd = tmpIntervals.indexOf('0',nStartSearchIndex)-1;
							if (nTmpEnd < 0)
							{
								nTmpEnd = tmpIntervals.size()-1;
								bExitLoop = true;
							}
							nStartSearchIndex = nTmpEnd+1;
							appendInterval(i,nTmpBegin+1,nTmpEnd+1);
						}
					} 
					while ((nTmpBegin>=0) && (bExitLoop==false));
				}
			}
		}
	}
	return true;
}


