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

#ifndef PROPERTYSETTING_H
#define PROPERTYSETTING_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QColor>
#include <QScriptEngine>


#define PROPERTYSETTINGS_PARAMETERS_TAG					"parameters"
#define PROPERTYSETTINGS_PARAMETER_TAG					"parameter"
#define PROPERTYSETTINGS_GROUP_TAG						"group"
#define PROPERTYSETTINGS_GROUPS_TAG						"groups"
#define PROPERTYSETTINGS_ID_TAG							"id"
#define PROPERTYSETTINGS_ENABLED_TAG					"enabled"
#define PROPERTYSETTINGS_EDITABLE_TAG					"editable"
#define PROPERTYSETTINGS_SCRIPTABLE_TAG					"scriptable"
#define PROPERTYSETTINGS_NAME_TAG						"name"
#define PROPERTYSETTINGS_DISPLAYNAME_TAG				"displayname"
#define PROPERTYSETTINGS_GROUPPATH_TAG					"grouppath"
#define PROPERTYSETTINGS_AUTOHIDE_TAG					"autohide"
#define PROPERTYSETTINGS_INFORMATION_TAG				"information"
#define PROPERTYSETTINGS_TYPE_TAG						"type"
#define PROPERTYSETTINGS_DEFAULTVALUE_TAG				"defaultvalue"
#define PROPERTYSETTINGS_RESTRICTION_TAG				"restriction"
#define PROPERTYSETTINGS_MINIMAL_TAG					"minimal"
#define PROPERTYSETTINGS_MAXIMAL_TAG					"maximal"
#define PROPERTYSETTINGS_VALUE_TAG						"value"
#define PROPERTYSETTINGS_ALLOWED_TAG					"allowed"
#define PROPERTYSETTINGS_DEPENDENCIES_TAG				"dependencies"
#define PROPERTYSETTINGS_DEPENDENCY_TAG					"dependency"
#define PROPERTYSETTINGS_RELATION_TAG					"relationid"
#define PROPERTYSETTINGS_REGEXP_TAG						"regularexpression"
#define PROPERTYSETTINGS_PATTERN_TAG					"pattern"
#define PROPERTYSETTINGS_CASESENSITIVE_TAG				"casesensitive"
#define PROPERTYSETTINGS_LISTSEP_CHAR					";"
#define PROPERTYSETTINGS_PARAM_GROUPSEP_CHAR			";"

enum PropertySettingEditingType
{
	PSET_DEFINED = 0,
	PSET_CUSTOM = 1
};

enum PropertySettingDefinitionSection
{
	PropertySetting_DefinitionSection_Root = 0,
	PropertySetting_DefinitionSection_Parameter = 1,
	PropertySetting_DefinitionSection_Restriction = 2,
	PropertySetting_DefinitionSection_Restriction_Minimal = 3,
	PropertySetting_DefinitionSection_Restriction_Maximal = 4,
	PropertySetting_DefinitionSection_Restriction_RegExp = 5,
	PropertySetting_DefinitionSection_Dependency = 6,
	PropertySetting_DefinitionSection_Dependency_RegExp = 7,
	PropertySetting_GroupSection_Group = 8,
	PropertySetting_GroupSection_Dependency = 9,
	PropertySetting_GroupSection_Dependency_RegExp = 10
};

enum PropertySettingTypeName
{
	PropertySetting_Type_Unknown = 0,
	PropertySetting_Type_String = 1,
	PropertySetting_Type_StringArray = 2,
	PropertySetting_Type_Color = 3,
	PropertySetting_Type_Integer = 4,
	PropertySetting_Type_Float = 5,
	PropertySetting_Type_Double = 6,
	PropertySetting_Type_Boolean = 7,
	PropertySetting_Type_FilePath = 8,
	PropertySetting_Type_DynamicStringEnum = 9,
	PropertySetting_Type_DynamicIntegerEnum = 10,//Filled at runtime

	//<<...insert here...>>
	
	PropertySetting_Type_FirstCustom = 11
	//See also below here!!!	
};

struct mapPropertySettingDefinitionsTypesStrc : QMap < QString, PropertySettingTypeName >
{
	mapPropertySettingDefinitionsTypesStrc()
	{
		this->operator[]("string") = PropertySetting_Type_String;
		this->operator[]("stringarray") = PropertySetting_Type_StringArray;
		this->operator[]("color") = PropertySetting_Type_Color;
		this->operator[]("integer") = PropertySetting_Type_Integer;
		this->operator[]("short") = PropertySetting_Type_Integer;
		this->operator[]("int") = PropertySetting_Type_Integer;
		this->operator[]("float") = PropertySetting_Type_Float;
		this->operator[]("double") = PropertySetting_Type_Double;
		this->operator[]("boolean") = PropertySetting_Type_Boolean;
		this->operator[]("filepath") = PropertySetting_Type_FilePath;
		this->operator[]("dynamicenumstring") = PropertySetting_Type_DynamicStringEnum;
		this->operator[]("dynamicenuminteger") = PropertySetting_Type_DynamicIntegerEnum;
	};
	~mapPropertySettingDefinitionsTypesStrc(){};
};

struct PropertySettingMinMaxRestrictionStrc
{
	bool bEnabled;
	QVariant vValue;
	PropertySettingMinMaxRestrictionStrc()
	{
		this->bEnabled = false;
		this->vValue = NULL;
	};
	~PropertySettingMinMaxRestrictionStrc() {};
};

struct PropertySettingDefinitionRestrictionStrc
{
	QStringList lAllowedValues;
	PropertySettingMinMaxRestrictionStrc MinimalValue;
	PropertySettingMinMaxRestrictionStrc MaximalValue;
	QRegularExpression rRegularExpression;
};

struct PropertySettingDefinitionDependencyStrc
{
	int nId;
	int nDependencyParameterID;
	bool bHideWhenInactive;
	QRegularExpression rRegularExpression;
	PropertySettingDefinitionDependencyStrc()
	{
		this->nId = -1;
		this->nDependencyParameterID = -1;
		this->bHideWhenInactive = false;
	};
	~PropertySettingDefinitionDependencyStrc() {};
};

struct PropertySettingDefinitionStrc
{
	int nId;
	bool bEnabled;
	bool bIsEditable;
	QString sName;
	QString sDisplayName;
	QString sGroupPath;//Can also be a directory like "Root;Item 1;SubItem A"
	QString sInformation;
	int eType;//see(PropertySettingTypeName + custom?)
	//PropertySettingEditingType eEditType;
	bool bCanBeScriptReference;
	QString sDefaultValue;
	//QList<PropertySettingDefinitionRestrictionStrc> Restrictions;
	PropertySettingDefinitionRestrictionStrc Restriction;
	QList<PropertySettingDefinitionDependencyStrc> Dependencies;

	PropertySettingDefinitionStrc()
	{
		nId = -1;
		bEnabled = false;
		bIsEditable = false;
		sName = "";
		sDisplayName = "";
		sGroupPath = "";
		sInformation = "";
		eType = PropertySetting_Type_Unknown;
		sDefaultValue = "";
		bCanBeScriptReference = false;
		//eEditType = PSET_DEFINED;
	};
	~PropertySettingDefinitionStrc() {};
};

struct PropertySettingGroupDefinitionStrc
{
	int nId;
	bool bEnabled;
	QString sGroupPath;
	QList<PropertySettingDefinitionDependencyStrc> Dependencies;

	PropertySettingGroupDefinitionStrc()
	{
		nId = -1;
		bEnabled = false;
		sGroupPath = "";
	};
	~PropertySettingGroupDefinitionStrc() {};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
QVariantList toVariantList( const QList<T> &list )
{
	QVariantList newList;
	foreach( const T &item, list )
		newList << item;
	return newList;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TypedPropertySettingContainer
{
public:
	TypedPropertySettingContainer() {};
	~TypedPropertySettingContainer() 
	{
		for (int i=0;i<lCustomAllocatedSettingNames.count();i++)
		{
			if(hIntContainer.contains(lCustomAllocatedSettingNames[i]))
			{
				delete hIntContainer.take(lCustomAllocatedSettingNames[i]);
				continue;
			}
			else if(hDoubleContainer.contains(lCustomAllocatedSettingNames[i]))
			{
				delete hDoubleContainer.take(lCustomAllocatedSettingNames[i]);
				continue;
			}
			else if(hFloatContainer.contains(lCustomAllocatedSettingNames[i]))
			{
				delete hFloatContainer.take(lCustomAllocatedSettingNames[i]);
				continue;
			}
			else if(hBoolContainer.contains(lCustomAllocatedSettingNames[i]))
			{
				delete hBoolContainer.take(lCustomAllocatedSettingNames[i]);
				continue;
			}
			else if(hQStringContainer.contains(lCustomAllocatedSettingNames[i]))
			{
				delete hQStringContainer.take(lCustomAllocatedSettingNames[i]);
				continue;
			}
			else if(hQStringListContainer.contains(lCustomAllocatedSettingNames[i]))
			{
				delete hQStringListContainer.take(lCustomAllocatedSettingNames[i]);
				continue;
			}
			else if(hQColorContainer.contains(lCustomAllocatedSettingNames[i]))
			{
				delete hQColorContainer.take(lCustomAllocatedSettingNames[i]);
				continue;
			}
		}
		lCustomAllocatedSettingNames.clear();

		hIntContainer.clear();
		hDoubleContainer.clear();
		hFloatContainer.clear();
		hBoolContainer.clear();
		hQStringContainer.clear();
		hQStringListContainer.clear();
		hQColorContainer.clear();
	};

	template< typename T1 > bool createPropertySetting(const QString &strKeyName, QVariant pExpParam)//These are custom defined parameters
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
				lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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
				lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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
				lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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
				lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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
				lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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
				lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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
				lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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
					lCustomAllocatedSettingNames.append(strKeyName.toLower());//Store the custom allocated parameter name
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

	template< typename T1 > bool insertPropertySetting(const QString &strKeyName, T1 *pExpParam = NULL)
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

	template< typename T2 > T2 *getPropertySetting(const QString &strKeyName)
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

	bool getPropertySetting(const QString &strKeyName, QScriptValue &scriptVal, QScriptEngine *currentScriptEngine = NULL)
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
			if (tmpStringList)
			{
				if (tmpStringList->isEmpty() == false)
				{
					QVariantList list = toVariantList(*tmpStringList);
					if (currentScriptEngine)
					{
						scriptVal = currentScriptEngine->newArray(tmpStringList->count());//scriptVal.engine()->newArray(tmpStringList->length());//QScriptValueList();// scriptVal->construct(list);//
						for (int i = 0; i<tmpStringList->count(); i++)
						{
							scriptVal.setProperty(i, QScriptValue(tmpStringList->at(i)));
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					scriptVal = QScriptValue(QScriptValue::NullValue);
				}
				return true;
			}
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

	bool setPropertySetting(const QString &strKeyName, const QScriptValue &scriptVal)
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

	QStringList lCustomAllocatedSettingNames;
};

#endif // PROPERTYSETTING_H