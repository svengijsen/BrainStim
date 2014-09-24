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

#ifndef EXPERIMENTPARAMETER_H
#define EXPERIMENTPARAMETER_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QColor>
#include <QScriptEngine>


template <typename T>
QVariantList toVariantList( const QList<T> &list )
{
	QVariantList newList;
	foreach( const T &item, list )
		newList << item;
	return newList;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TypedExperimentParameterContainer
{
public:
	TypedExperimentParameterContainer() {};
	~TypedExperimentParameterContainer() 
	{
		for (int i=0;i<lCustomAllocatedParameterNames.count();i++)
		{
			if(hIntContainer.contains(lCustomAllocatedParameterNames[i]))
			{
				delete hIntContainer.take(lCustomAllocatedParameterNames[i]);
				continue;
			}
			else if(hDoubleContainer.contains(lCustomAllocatedParameterNames[i]))
			{
				delete hDoubleContainer.take(lCustomAllocatedParameterNames[i]);
				continue;
			}
			else if(hFloatContainer.contains(lCustomAllocatedParameterNames[i]))
			{
				delete hFloatContainer.take(lCustomAllocatedParameterNames[i]);
				continue;
			}
			else if(hBoolContainer.contains(lCustomAllocatedParameterNames[i]))
			{
				delete hBoolContainer.take(lCustomAllocatedParameterNames[i]);
				continue;
			}
			else if(hQStringContainer.contains(lCustomAllocatedParameterNames[i]))
			{
				delete hQStringContainer.take(lCustomAllocatedParameterNames[i]);
				continue;
			}
			else if(hQStringListContainer.contains(lCustomAllocatedParameterNames[i]))
			{
				delete hQStringListContainer.take(lCustomAllocatedParameterNames[i]);
				continue;
			}
			else if(hQColorContainer.contains(lCustomAllocatedParameterNames[i]))
			{
				delete hQColorContainer.take(lCustomAllocatedParameterNames[i]);
				continue;
			}
		}
		lCustomAllocatedParameterNames.clear();

		hIntContainer.clear();
		hDoubleContainer.clear();
		hFloatContainer.clear();
		hBoolContainer.clear();
		hQStringContainer.clear();
		hQStringListContainer.clear();
		hQColorContainer.clear();
	};

	template< typename T1 > bool createExperimentParameter(const QString &strKeyName, QVariant pExpParam)//These are custom defined parameters
	{
		if (typeid(T1) == typeid(int))
		{
			int *tmpTypeVar;
			if(hIntContainer.contains(strKeyName.toLower()))
			{
				tmpTypeVar = hIntContainer.take(strKeyName.toLower());//get type pointer
				*tmpTypeVar = pExpParam.toInt();//Change the value	
			}
			else
			{
				tmpTypeVar = new int(pExpParam.toInt());//allocate a new type
				lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
			}			
			hIntContainer.insert(strKeyName.toLower(),tmpTypeVar);
			return true;
		}
		else if (typeid(T1) == typeid(double))
		{
			double *tmpTypeVar;
			if(hDoubleContainer.contains(strKeyName.toLower()))
			{
				tmpTypeVar = hDoubleContainer.take(strKeyName.toLower());//get type pointer
				*tmpTypeVar = pExpParam.toDouble();//Change the value	
			}
			else
			{
				tmpTypeVar = new double(pExpParam.toDouble());//allocate a new type
				lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
			}			
			hDoubleContainer.insert(strKeyName.toLower(),tmpTypeVar);
			return true;
		}
		else if (typeid(T1) == typeid(float))
		{
			float *tmpTypeVar;
			if(hFloatContainer.contains(strKeyName.toLower()))
			{
				tmpTypeVar = hFloatContainer.take(strKeyName.toLower());//get type pointer
				*tmpTypeVar = pExpParam.toFloat();//Change the value	
			}
			else
			{
				tmpTypeVar = new float(pExpParam.toFloat());//allocate a new type
				lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
			}			
			hFloatContainer.insert(strKeyName.toLower(),tmpTypeVar);
			return true;
		}
		else if (typeid(T1) == typeid(bool))
		{
			bool *tmpTypeVar;
			if(hBoolContainer.contains(strKeyName.toLower()))
			{
				tmpTypeVar = hBoolContainer.take(strKeyName.toLower());//get type pointer
				*tmpTypeVar = pExpParam.toBool();//Change the value	
			}
			else
			{
				tmpTypeVar = new bool(pExpParam.toBool());//allocate a new type
				lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
			}			
			hBoolContainer.insert(strKeyName.toLower(),tmpTypeVar);
			return true;
		}
		else if (typeid(T1) == typeid(QString))
		{
			QString *tmpTypeVar;
			if(hQStringContainer.contains(strKeyName.toLower()))
			{
				tmpTypeVar = hQStringContainer.take(strKeyName.toLower());//get type pointer
				*tmpTypeVar = pExpParam.toString();//Change the value				
				//todo
				//- check 4 custom {scriptvar} possibility?
				//- check retino...
				//- search for insert get set script function that dissapeared?	
				//dfdsf
			}
			else
			{
				tmpTypeVar = new QString(pExpParam.toString());//allocate a new type
				lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
			}
			hQStringContainer.insert(strKeyName.toLower(),tmpTypeVar);
			return true;
		}
		else if (typeid(T1) == typeid(QStringList))
		{
			QStringList *tmpTypeVar;
			if(hQStringListContainer.contains(strKeyName.toLower()))
			{
				tmpTypeVar = hQStringListContainer.take(strKeyName.toLower());//get type pointer
				*tmpTypeVar = pExpParam.toStringList();//Change the value	
			}
			else
			{
				tmpTypeVar = new QStringList(pExpParam.toStringList());//allocate a new type
				lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
			}			
			hQStringListContainer.insert(strKeyName.toLower(),tmpTypeVar);
			return true;
		}
		else if (typeid(T1) == typeid(QColor))
		{
			QColor *tmpTypeVar;
			if(hQColorContainer.contains(strKeyName.toLower()))
			{
				tmpTypeVar = hQColorContainer.take(strKeyName.toLower());//get type pointer
				*tmpTypeVar = pExpParam.value<QColor>();//Change the value	
			}
			else
			{
				tmpTypeVar = new QColor(pExpParam.value<QColor>());//allocate a new type
				lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
			}			
			hQColorContainer.insert(strKeyName.toLower(),tmpTypeVar);
			return true;
		}
		else
		{			
			QString strUnknownTypeName = typeid(T1).name();
			if (strUnknownTypeName.contains("char",Qt::CaseInsensitive))
			{
				QString *tmpTypeVar;
				if(hQStringContainer.contains(strKeyName.toLower()))
				{
					tmpTypeVar = hQStringContainer.take(strKeyName.toLower());//get type pointer
					*tmpTypeVar = pExpParam.toString();//Change the value	
				}
				else
				{
					tmpTypeVar = new QString(pExpParam.toString());//allocate a new type
					lCustomAllocatedParameterNames.append(strKeyName.toLower());//Store the custom allocated parameter name
				}				
				hQStringContainer.insert(strKeyName.toLower(),tmpTypeVar);
				return true;
			}
			else
			{
				qDebug() << __FUNCTION__ << "::Could not cast type (" + strUnknownTypeName + ")!";
				return false;
			}
		}
		return false;
	}

	template< typename T1 > bool insertExperimentParameter(const QString &strKeyName, T1 *pExpParam = NULL)
	{
		if(pExpParam == NULL)
			return false;
		if (typeid(T1) == typeid(int))
		{
			hIntContainer.insert(strKeyName.toLower(),(int *)pExpParam);
			return true;
		}
		else if (typeid(T1) == typeid(double))
		{
			hDoubleContainer.insert(strKeyName.toLower(),(double *)pExpParam);
			return true;
		}
		else if (typeid(T1) == typeid(float))
		{
			hFloatContainer.insert(strKeyName.toLower(),(float *)pExpParam);
			return true;
		}
		else if (typeid(T1) == typeid(bool))
		{
			hBoolContainer.insert(strKeyName.toLower(),(bool *)pExpParam);
			return true;
		}
		else if (typeid(T1) == typeid(QString))
		{
			hQStringContainer.insert(strKeyName.toLower(),(QString *)pExpParam);
			return true;
		}
		else if (typeid(T1) == typeid(QStringList))
		{
			hQStringListContainer.insert(strKeyName.toLower(),(QStringList *)pExpParam);
			return true;
		}
		else if (typeid(T1) == typeid(QColor))
		{
			hQColorContainer.insert(strKeyName.toLower(),(QColor *)pExpParam);
			return true;
		}
		else
		{
			QString strUnknownTypeName = typeid(T1).name();
			if (strUnknownTypeName.contains("char",Qt::CaseInsensitive))
			{
				hQStringContainer.insert(strKeyName.toLower(),(QString *)pExpParam);
				return true;
			}
			else
			{
				qDebug() << __FUNCTION__ << "::Could not cast type (" + strUnknownTypeName + ")!";
				return false;
			}
		}
		return false;
	}

	template< typename T2 > T2 *getExperimentParameter(const QString &strKeyName)
	{
		if (typeid(T2) == typeid(int))
		{
			return (T2*)hIntContainer.value(strKeyName.toLower());
		}
		else if (typeid(T2) == typeid(double))
		{
			return (T2*)hDoubleContainer.value(strKeyName.toLower());
		}
		else if (typeid(T2) == typeid(float))
		{
			return (T2*)hFloatContainer.value(strKeyName.toLower());
		}
		else if (typeid(T2) == typeid(bool))
		{
			return (T2*)hBoolContainer.value(strKeyName.toLower());
		}
		else if (typeid(T2) == typeid(QString))
		{
			return (T2*)hQStringContainer.value(strKeyName.toLower());
		}
		else if (typeid(T2) == typeid(QStringList))
		{
			return (T2*)hQStringListContainer.value(strKeyName.toLower());
		}
		else if (typeid(T2) == typeid(QColor))
		{
			return (T2*)hQColorContainer.value(strKeyName.toLower());
		}
		else
		{
			return NULL;
		}
		return NULL;
	};

	bool getExperimentParameter(const QString &strKeyName, QScriptValue &scriptVal)
	{
		QString strKeyNameLow = strKeyName.toLower();
		if (hIntContainer.contains(strKeyNameLow))
		{
			scriptVal = QScriptValue(*hIntContainer.value(strKeyNameLow));
			return true;
		} 
		else if (hDoubleContainer.contains(strKeyNameLow))
		{
			scriptVal = QScriptValue(*hDoubleContainer.value(strKeyNameLow));
			return true;
		}
		else if (hFloatContainer.contains(strKeyNameLow))
		{
			scriptVal = QScriptValue(*hFloatContainer.value(strKeyNameLow));
			return true;
		}
		else if (hBoolContainer.contains(strKeyNameLow))
		{
			scriptVal = QScriptValue(*hBoolContainer.value(strKeyNameLow));
			return true;
		}
		else if (hQStringContainer.contains(strKeyNameLow))
		{
			scriptVal = QScriptValue(*hQStringContainer.value(strKeyNameLow));
			return true;
		}
		else if (hQStringListContainer.contains(strKeyNameLow))
		{
			QStringList *tmpStringList = hQStringListContainer.value(strKeyNameLow);
			//int sListLength = scriptVal.property("length").toInteger();
			//tmpStringList->clear();
			//for(int i = 0; i < sListLength; i++)
			//	tmpStringList->append(scriptVal.property(i).toString());
			////*tmpStringList = scriptVal.toQObject();

			QVariantList list = toVariantList(*tmpStringList);
			scriptVal = scriptVal.engine()->newArray(tmpStringList->length());
			for(int i=0; i<tmpStringList->count(); i++)
			{
				scriptVal.setProperty(i, QScriptValue(tmpStringList->at(i)));  
			}
			return true;
		}
		else if (hQColorContainer.contains(strKeyNameLow))
		{
			scriptVal = QScriptValue((*hQColorContainer.value(strKeyNameLow)).name());
			return true;
		}
		else
		{
			return false;
		}
		return false;
	};

	bool setExperimentParameter(const QString &strKeyName, const QScriptValue &scriptVal)
	{
		QString strKeyNameLow = strKeyName.toLower();
		if (hIntContainer.contains(strKeyNameLow))
		{
			int *tmpInt = hIntContainer.value(strKeyNameLow);
			*tmpInt = scriptVal.toInteger();
			return true;
		} 
		else if (hDoubleContainer.contains(strKeyNameLow))
		{
			double *tmpDouble = hDoubleContainer.value(strKeyNameLow);
			*tmpDouble = scriptVal.toNumber();
			return true;
		}
		else if (hFloatContainer.contains(strKeyNameLow))
		{
			float *tmpFloat = hFloatContainer.value(strKeyNameLow);
			*tmpFloat = scriptVal.toNumber();//Creates a double
			return true;
		}
		else if (hBoolContainer.contains(strKeyNameLow))
		{
			bool *tmpBool = hBoolContainer.value(strKeyNameLow);
			*tmpBool = scriptVal.toBool();
			return true;
		}
		else if (hQStringContainer.contains(strKeyNameLow))
		{
			QString *tmpString = hQStringContainer.value(strKeyNameLow);
			*tmpString = scriptVal.toString();
			return true;
		}
		else if (hQStringListContainer.contains(strKeyNameLow))
		{
			QStringList *tmpStringList = hQStringListContainer.value(strKeyNameLow);
			int sListLength = scriptVal.property("length").toInteger();
			tmpStringList->clear();
			for(int i = 0; i < sListLength; i++)
				tmpStringList->append(scriptVal.property(i).toString());
			//*tmpStringList = scriptVal.toQObject();
			//QString sTemp = hQStringListContainer.value(strKeyNameLow)->join(",");
			return true;
		}
		else if (hQColorContainer.contains(strKeyNameLow))
		{
			QColor tmpColor;
			QString test = scriptVal.toString();
			tmpColor.setNamedColor(test);
			if(tmpColor.isValid())
			{
				*hQColorContainer.value(strKeyNameLow) = tmpColor;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
		return false;
	};

private:
	QHash<QString, int*> hIntContainer;
	QHash<QString, double*> hDoubleContainer;
	QHash<QString, float*> hFloatContainer;
	QHash<QString, bool*> hBoolContainer;
	QHash<QString, QColor*> hQColorContainer;
	QHash<QString, QString*> hQStringContainer;
	QHash<QString, QStringList*> hQStringListContainer;

	QStringList lCustomAllocatedParameterNames;
};

#endif // EXPERIMENTPARAMETER_H