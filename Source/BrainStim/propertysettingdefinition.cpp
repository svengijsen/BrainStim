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

#include "propertysettingdefinition.h"
#include "maindefines.h"
#include <QUrl>
#include <QFile>
#include <QDebug>

PropertySettingDefinition::PropertySettingDefinition()
{

}

PropertySettingDefinition::~PropertySettingDefinition()
{
	if(expParamDefinitions.isEmpty() == false)
	{
		//for (int i=0;i<expParamDefinitions.count();i++)
		//{
		//	expParamDefinitions[i].Restrictions.clear();
		//}
		expParamDefinitions.clear();
	}
	if(expGroupDefinitions.isEmpty() == false)
	{
		expGroupDefinitions.clear();
	}	
}

int PropertySettingDefinition::getFirstParameterID(const QString &sName)
{
	if(expParamDefinitions.isEmpty())
		return -1;
	QString sNameToLower = sName.toLower();
	for (int i=0;i<expParamDefinitions.count();i++)
	{
		if(expParamDefinitions.at(i).sName == sNameToLower)
			return expParamDefinitions.at(i).nId;
	}
	return -1;
}

bool PropertySettingDefinition::getParameterIDList(const QString &sName, QList<int> &sList)
{
	if(expParamDefinitions.isEmpty())
		return false;
	QString sNameToLower = sName.toLower();
	bool bValueFound = false;
	for (int i=0;i<expParamDefinitions.count();i++)
	{
		if(expParamDefinitions.at(i).sName == sNameToLower)
		{
			sList.append(expParamDefinitions.at(i).nId);
			bValueFound = true;
		}
	}
	return bValueFound;
}

QString PropertySettingDefinition::getParameterName(const int &nId)
{
	if((expParamDefinitions.isEmpty()) || (nId<0))
		return "";
	for (int i=0;i<expParamDefinitions.count();i++)
	{
		if(expParamDefinitions.at(i).nId == nId)
			return expParamDefinitions.at(i).sName;
	}
	return "";
}

PropertySettingDefinitionStrc *PropertySettingDefinition::getParameterDefinition(const int &nId)
{
	if((expParamDefinitions.isEmpty()) || (nId<0))
		return NULL;
	for (int i=0;i<expParamDefinitions.count();i++)
	{
		if(expParamDefinitions.at(i).nId == nId)
			return &expParamDefinitions[i];
	}
	return NULL;
}

bool PropertySettingDefinition::loadFromFile(const QString &sFilePath)
{
	QFile* file = new QFile(sFilePath);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) 
	{
        qDebug() << __FUNCTION__ << "Couldn't open the file " << sFilePath;
        return false;
    }
    QXmlStreamReader xml(file);
	/* We'll parse the XML until we reach end of it.*/
    while(!xml.atEnd() && !xml.hasError()) 
	{
        /* Read next element.*/
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument) 
            continue;
        else if(token == QXmlStreamReader::StartElement) 
		{
			if (xml.name() == PROPERTYSETTINGS_GROUPS_TAG)
			{
				continue;
			}
			else if (xml.name() == PROPERTYSETTINGS_GROUP_TAG)
			{
				expGroupDefinitions.append(*(this->parseGroupDefinition(xml)));
			}
            else if(xml.name() == PROPERTYSETTINGS_PARAMETERS_TAG)
			{
                continue;
			}
            else if(xml.name() == PROPERTYSETTINGS_PARAMETER_TAG)
			{
                expParamDefinitions.append(*(this->parseParameterDefinition(xml)));
			}
        }
    }
    /* Error handling. */
    if(xml.hasError()) 
		qDebug() << __FUNCTION__ << xml.errorString();

    /* Removes any device() or data from the reader and resets its internal state to the initial state. */
    xml.clear();
	return true;
}

PropertySettingGroupDefinitionStrc *PropertySettingDefinition::parseGroupDefinition(QXmlStreamReader& xml)
{
	PropertySettingGroupDefinitionStrc *tmpGroupDef = NULL;
    /* Let's check that we're really getting a group definition. */
	if ((xml.tokenType() != QXmlStreamReader::StartElement) || (xml.name() != PROPERTYSETTINGS_GROUP_TAG))
        return NULL;
	PropertySettingDefinitionSection expParamCurrectSection = PropertySetting_GroupSection_Group;
    /* Let's get the attributes for group definition */
    QXmlStreamAttributes attributes = xml.attributes();
    /* Let's check that the group definition has a id attribute. */
	int nID = -1;
	//QVariant::Type vType;
	if (attributes.hasAttribute(PROPERTYSETTINGS_ID_TAG))
	{
        /* We'll add it to the map. */		
		nID = attributes.value(PROPERTYSETTINGS_ID_TAG).toInt();
		if(nID < 0)
			return NULL;
		tmpGroupDef = new PropertySettingGroupDefinitionStrc;
		tmpGroupDef->nId = nID;
    }
    /* Next element... */
    xml.readNext();
    /* We're going to loop because the order might change.
     * We'll continue the loop until we hit an EndElement property definition.  */
	QStringRef tmpStringRef = xml.name();
	QXmlStreamReader::TokenType tmpToken = xml.tokenType();
	while (!((tmpToken == QXmlStreamReader::EndElement) && (tmpStringRef == PROPERTYSETTINGS_GROUP_TAG)))
	{		
        if(tmpToken == QXmlStreamReader::StartElement)
		{
			if (tmpStringRef == PROPERTYSETTINGS_DEPENDENCY_TAG)
			{
				expParamCurrectSection = PropertySetting_GroupSection_Dependency;
				QXmlStreamAttributes depAttributes = xml.attributes();
				/* Let's check that the parameter dependency has a id attribute. */
				int nDepID = -1;
				if (depAttributes.hasAttribute(PROPERTYSETTINGS_ID_TAG))
				{
					/* We'll add it to the map. */		
					nDepID = depAttributes.value(PROPERTYSETTINGS_ID_TAG).toInt();
					if((nDepID < 0) || (tmpGroupDef == NULL))
						return NULL;
					PropertySettingDefinitionDependencyStrc tmpStruct;
					tmpStruct.nId = nDepID;					
					tmpGroupDef->Dependencies.append(tmpStruct);
				}
			}
			else if ((expParamCurrectSection == PropertySetting_GroupSection_Dependency) && (tmpStringRef == PROPERTYSETTINGS_REGEXP_TAG))
			{
				expParamCurrectSection = PropertySetting_GroupSection_Dependency_RegExp;
			}			
			else
			{
				this->addGroupDataToStructure(xml, *tmpGroupDef,expParamCurrectSection);
			}
		}
		else if(tmpToken == QXmlStreamReader::EndElement)
		{
			if (tmpStringRef == PROPERTYSETTINGS_DEPENDENCY_TAG)
			{
				expParamCurrectSection = PropertySetting_GroupSection_Group;
			}			
			else if (tmpStringRef == PROPERTYSETTINGS_REGEXP_TAG)
			{
				if(expParamCurrectSection == PropertySetting_DefinitionSection_Dependency_RegExp)
					expParamCurrectSection = PropertySetting_DefinitionSection_Dependency;
			}			
		}
        xml.readNext();
		tmpStringRef = xml.name();
		tmpToken = xml.tokenType();
    }
    return tmpGroupDef;
}

PropertySettingDefinitionStrc *PropertySettingDefinition::parseParameterDefinition(QXmlStreamReader& xml)
{
	PropertySettingDefinitionStrc *tmpParamDef = NULL;
    /* Let's check that we're really getting a parameter definition. */
    if((xml.tokenType() != QXmlStreamReader::StartElement) || (xml.name() != PROPERTYSETTINGS_PARAMETER_TAG))
        return NULL;
	PropertySettingDefinitionSection expParamCurrectSection = PropertySetting_DefinitionSection_Parameter;
    /* Let's get the attributes for parameter definition */
    QXmlStreamAttributes attributes = xml.attributes();
    /* Let's check that the parameter definition has a id attribute. */
	int nID = -1;
	//QVariant::Type vType;
	if (attributes.hasAttribute(PROPERTYSETTINGS_ID_TAG))
	{
        /* We'll add it to the map. */		
		nID = attributes.value(PROPERTYSETTINGS_ID_TAG).toInt();
		if(nID < 0)
			return NULL;
		tmpParamDef = new PropertySettingDefinitionStrc;
		tmpParamDef->nId = nID;
    }
    /* Next element... */
    xml.readNext();
    /* We're going to loop because the order might change.
     * We'll continue the loop until we hit an EndElement property definition.  */
	QStringRef tmpStringRef = xml.name();
	QXmlStreamReader::TokenType tmpToken = xml.tokenType();
    while(!((tmpToken == QXmlStreamReader::EndElement) && (tmpStringRef == PROPERTYSETTINGS_PARAMETER_TAG))) 
	{		
        if(tmpToken == QXmlStreamReader::StartElement)
		{
			if (tmpStringRef == PROPERTYSETTINGS_RESTRICTION_TAG)
			{
				expParamCurrectSection = PropertySetting_DefinitionSection_Restriction;
			}
			else if (tmpStringRef == PROPERTYSETTINGS_DEPENDENCY_TAG)
			{
				expParamCurrectSection = PropertySetting_DefinitionSection_Dependency;
				QXmlStreamAttributes depAttributes = xml.attributes();
				/* Let's check that the parameter dependency has a id attribute. */
				int nDepID = -1;
				if (depAttributes.hasAttribute(PROPERTYSETTINGS_ID_TAG))
				{
					/* We'll add it to the map. */		
					nDepID = depAttributes.value(PROPERTYSETTINGS_ID_TAG).toInt();
					if((nDepID < 0) || (tmpParamDef == NULL))
						return NULL;
					PropertySettingDefinitionDependencyStrc tmpStruct;
					tmpStruct.nId = nDepID;
					tmpParamDef->Dependencies.append(tmpStruct);
				}
			}
			else if ((expParamCurrectSection == PropertySetting_DefinitionSection_Dependency) && (tmpStringRef == PROPERTYSETTINGS_REGEXP_TAG))
			{
				expParamCurrectSection = PropertySetting_DefinitionSection_Dependency_RegExp;
			}
			else if ((expParamCurrectSection == PropertySetting_DefinitionSection_Restriction) && (tmpStringRef == PROPERTYSETTINGS_MINIMAL_TAG))
			{
				expParamCurrectSection = PropertySetting_DefinitionSection_Restriction_Minimal;
			}
			else if ((expParamCurrectSection == PropertySetting_DefinitionSection_Restriction) && (tmpStringRef == PROPERTYSETTINGS_MAXIMAL_TAG))
			{
				expParamCurrectSection = PropertySetting_DefinitionSection_Restriction_Maximal;
			}
			else if((expParamCurrectSection == PropertySetting_DefinitionSection_Restriction) && (tmpStringRef == PROPERTYSETTINGS_REGEXP_TAG))
			{
				expParamCurrectSection = PropertySetting_DefinitionSection_Restriction_RegExp;
			}
			else
			{
				this->addParameterDataToStructure(xml, *tmpParamDef,expParamCurrectSection);
			}
		}
		else if(tmpToken == QXmlStreamReader::EndElement)
		{
			if ((tmpStringRef == PROPERTYSETTINGS_RESTRICTION_TAG) || (tmpStringRef == PROPERTYSETTINGS_DEPENDENCY_TAG))
			{
				expParamCurrectSection = PropertySetting_DefinitionSection_Parameter;
			}			
			else if(tmpStringRef == PROPERTYSETTINGS_REGEXP_TAG) 
			{
				if(expParamCurrectSection == PropertySetting_DefinitionSection_Dependency_RegExp)
					expParamCurrectSection = PropertySetting_DefinitionSection_Dependency;
				else if(expParamCurrectSection == PropertySetting_DefinitionSection_Restriction_RegExp)
					expParamCurrectSection = PropertySetting_DefinitionSection_Restriction;
			}
			else if (tmpStringRef == PROPERTYSETTINGS_MINIMAL_TAG)
			{
				if(expParamCurrectSection == PropertySetting_DefinitionSection_Restriction_Minimal)
					expParamCurrectSection = PropertySetting_DefinitionSection_Restriction;
			}
			else if (tmpStringRef == PROPERTYSETTINGS_MAXIMAL_TAG)
			{
				if(expParamCurrectSection == PropertySetting_DefinitionSection_Restriction_Maximal)
					expParamCurrectSection = PropertySetting_DefinitionSection_Restriction;
			}
		}
        xml.readNext();
		tmpStringRef = xml.name();
		tmpToken = xml.tokenType();
    }
    return tmpParamDef;
}

bool PropertySettingDefinition::addGroupDataToStructure(QXmlStreamReader& xml, PropertySettingGroupDefinitionStrc &expGroupDefStrc, PropertySettingDefinitionSection &expParamSection) const
{
    /* We need a start element, like <foo> */
    if(xml.tokenType() != QXmlStreamReader::StartElement)
        return false;
    /* Let's read the property definition... */
    QString elementName = xml.name().toString();
    /* ...go to the next. */
    xml.readNext();
    /* This elements needs to contain Characters so we know it's
     * actually data, if it's not we'll leave. */
    if(xml.tokenType() != QXmlStreamReader::Characters)
        return false;  

	if(expParamSection == PropertySetting_GroupSection_Group)
	{
		/* Now we can add it to the map.*/
		if (elementName == PROPERTYSETTINGS_ENABLED_TAG)
		{
			bool bNewValue = false;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_TRUE)
			{
				bNewValue = true;
			}
			else if (tmpString != TYPE_BOOL_FALSE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			expGroupDefStrc.bEnabled = bNewValue;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_GROUPPATH_TAG)
		{
			expGroupDefStrc.sGroupPath = xml.text().trimmed().toString();
			return true;
		}
	}	
	else if(expParamSection == PropertySetting_GroupSection_Dependency)
	{
		if (elementName == PROPERTYSETTINGS_RELATION_TAG)
		{
			const QString tmpString = xml.text().trimmed().toString();
			if(tmpString.isEmpty())
			{
				qDebug() << __FUNCTION__ << "non defined string value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			if(expGroupDefStrc.Dependencies.isEmpty())
			{
				qDebug() << __FUNCTION__ << "could not define Group Dependency (Doesn't exist!)" << "(> " << tmpString << ")";
				return false;
			}
			if(tmpString == "0")
			{
				expGroupDefStrc.Dependencies.last().nDependencyParameterID = 0;
			}
			else
			{
				int tmpInteger = tmpString.toInt();
				if(tmpInteger > 0)
					expGroupDefStrc.Dependencies.last().nDependencyParameterID = tmpInteger;
				else
					expGroupDefStrc.Dependencies.last().nDependencyParameterID = -1;
			}			
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_AUTOHIDE_TAG)
		{
			bool bNewValue = false;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_TRUE)
			{
				bNewValue = true;
			}
			else if (tmpString != TYPE_BOOL_FALSE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			if(bNewValue)
			{
				if(expGroupDefStrc.Dependencies.isEmpty())
				{
					qDebug() << __FUNCTION__ << "could not define Group Dependency (Doesn't exist!)" << "(> " << tmpString << ")";
					return false;
				}
				expGroupDefStrc.Dependencies.last().bHideWhenInactive = true;
			}
			return true;
		}
	}
	else if(expParamSection == PropertySetting_GroupSection_Dependency_RegExp)
	{
		if (elementName == PROPERTYSETTINGS_PATTERN_TAG)
		{
			if(expParamSection == PropertySetting_GroupSection_Dependency_RegExp)
				expGroupDefStrc.Dependencies.last().rRegularExpression.setPattern(xml.text().trimmed().toString());
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_CASESENSITIVE_TAG)
		{
			QRegularExpression::PatternOptions tmpOptions = QRegularExpression::NoPatternOption;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_FALSE)
			{
				tmpOptions = QRegularExpression::CaseInsensitiveOption;
			}
			else if (tmpString != TYPE_BOOL_TRUE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			if(expParamSection == PropertySetting_GroupSection_Dependency_RegExp)
				expGroupDefStrc.Dependencies.last().rRegularExpression.setPatternOptions(tmpOptions);
			return true;
		}
	}
	return false;
}

bool PropertySettingDefinition::addParameterDataToStructure(QXmlStreamReader& xml, PropertySettingDefinitionStrc &expParamDefStrc, PropertySettingDefinitionSection &expParamSection) const
{
    /* We need a start element, like <foo> */
    if(xml.tokenType() != QXmlStreamReader::StartElement)
        return false;
    /* Let's read the property definition... */
    QString elementName = xml.name().toString();
    /* ...go to the next. */
    xml.readNext();
    /* This elements needs to contain Characters so we know it's
     * actually data, if it's not we'll leave. */
    if(xml.tokenType() != QXmlStreamReader::Characters)
        return false;  

	if(expParamSection == PropertySetting_DefinitionSection_Parameter)
	{
		/* Now we can add it to the map.*/
		if (elementName == PROPERTYSETTINGS_ENABLED_TAG)
		{
			bool bNewValue = false;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_TRUE)
			{
				bNewValue = true;
			}
			else if (tmpString != TYPE_BOOL_FALSE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			expParamDefStrc.bEnabled = bNewValue;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_EDITABLE_TAG)
		{
			bool bNewValue = false;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_TRUE)
			{
				bNewValue = true;
			}
			else if (tmpString != TYPE_BOOL_FALSE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			expParamDefStrc.bIsEditable = bNewValue;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_NAME_TAG)
		{
			const QString tmpString = xml.text().trimmed().toString();
			if(tmpString.isEmpty())
			{
				qDebug() << __FUNCTION__ << "non defined string value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			expParamDefStrc.sName = tmpString;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_DISPLAYNAME_TAG)
		{
			const QString tmpString = xml.text().trimmed().toString();
			if(tmpString.isEmpty())
			{
				qDebug() << __FUNCTION__ << "non defined string value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			expParamDefStrc.sDisplayName = tmpString;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_GROUPPATH_TAG)
		{
			expParamDefStrc.sGroupPath = xml.text().trimmed().toString();//tmpString;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_INFORMATION_TAG)
		{
			expParamDefStrc.sInformation = xml.text().trimmed().toString();//tmpString;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_TYPE_TAG)
		{
			const QString tmpString = xml.text().trimmed().toString();
			if (mCustomParamDefinitionsTypesMap.contains(tmpString))
			{
				expParamDefStrc.eType = mCustomParamDefinitionsTypesMap.value(tmpString);
				return true;
			}
			PropertySettingTypeName tmpExpParTypeName = mParamDefinitionsTypesMap[tmpString];
			if(tmpExpParTypeName == PropertySetting_Type_Unknown)
			{
				qDebug() << __FUNCTION__ << "unknown defined string value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			expParamDefStrc.eType = tmpExpParTypeName;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_SCRIPTABLE_TAG)
		{
			bool bNewValue = false;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_TRUE)
			{
				bNewValue = true;
			}
			else if (tmpString != TYPE_BOOL_FALSE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			expParamDefStrc.bCanBeScriptReference = bNewValue;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_DEFAULTVALUE_TAG)
		{
			expParamDefStrc.sDefaultValue = xml.text().trimmed().toString();//tmpString;
			return true;
		}
	}
	else if((expParamSection == PropertySetting_DefinitionSection_Restriction_Minimal) || (expParamSection == PropertySetting_DefinitionSection_Restriction_Maximal))
	{
		if (elementName == PROPERTYSETTINGS_ENABLED_TAG)
		{
			bool bNewValue = false;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_TRUE)
			{
				bNewValue = true;
			}
			else if (tmpString != TYPE_BOOL_FALSE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			if(expParamSection == PropertySetting_DefinitionSection_Restriction_Minimal)
				expParamDefStrc.Restriction.MinimalValue.bEnabled = bNewValue;
			else if(expParamSection == PropertySetting_DefinitionSection_Restriction_Maximal)
				expParamDefStrc.Restriction.MaximalValue.bEnabled = bNewValue;
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_VALUE_TAG)
		{
			if(expParamSection == PropertySetting_DefinitionSection_Restriction_Minimal)
				expParamDefStrc.Restriction.MinimalValue.vValue = xml.text().trimmed().toString();
			else if(expParamSection == PropertySetting_DefinitionSection_Restriction_Maximal)
				expParamDefStrc.Restriction.MaximalValue.vValue = xml.text().trimmed().toString();
			return true;
		}
	}
	else if(expParamSection == PropertySetting_DefinitionSection_Restriction)
	{
		if (elementName == PROPERTYSETTINGS_ALLOWED_TAG)
		{
			QString tmpString = xml.text().trimmed().toString();
			if(tmpString.isEmpty())
				return true;
			const QStringList tmpStringList = tmpString.split(PROPERTYSETTINGS_LISTSEP_CHAR, QString::SkipEmptyParts);
			if(tmpStringList.isEmpty())
				return true;
			expParamDefStrc.Restriction.lAllowedValues = tmpStringList;
			return true;
		}
	}
	else if(expParamSection == PropertySetting_DefinitionSection_Dependency)
	{
		if (elementName == PROPERTYSETTINGS_RELATION_TAG)
		{
			const QString tmpString = xml.text().trimmed().toString();
			if(tmpString.isEmpty())
			{
				qDebug() << __FUNCTION__ << "non defined string value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			if(expParamDefStrc.Dependencies.isEmpty())
			{
				qDebug() << __FUNCTION__ << "could not define Parameter Dependency (Doesn't exist!)" << "(> " << tmpString << ")";
				return false;
			}
			if(tmpString == "0")
			{
				expParamDefStrc.Dependencies.last().nDependencyParameterID = 0;
			}
			else
			{
				int tmpInteger = tmpString.toInt();
				if(tmpInteger > 0)
					expParamDefStrc.Dependencies.last().nDependencyParameterID = tmpInteger;
				else
					expParamDefStrc.Dependencies.last().nDependencyParameterID = -1;
			}			
			return true;
		}
	}
	else if((expParamSection == PropertySetting_DefinitionSection_Dependency_RegExp) || (expParamSection == PropertySetting_DefinitionSection_Restriction_RegExp))
	{
		if (elementName == PROPERTYSETTINGS_PATTERN_TAG)
		{
			if(expParamSection == PropertySetting_DefinitionSection_Dependency_RegExp)
				expParamDefStrc.Dependencies.last().rRegularExpression.setPattern(xml.text().trimmed().toString());
			else if(expParamSection == PropertySetting_DefinitionSection_Restriction_RegExp)
				expParamDefStrc.Restriction.rRegularExpression.setPattern(xml.text().trimmed().toString());
			return true;
		}
		else if (elementName == PROPERTYSETTINGS_CASESENSITIVE_TAG)
		{
			QRegularExpression::PatternOptions tmpOptions = QRegularExpression::NoPatternOption;
			const QString tmpString = xml.text().trimmed().toString();
			if (tmpString == TYPE_BOOL_FALSE)
			{
				tmpOptions = QRegularExpression::CaseInsensitiveOption;
			}
			else if (tmpString != TYPE_BOOL_TRUE)
			{
				qDebug() << __FUNCTION__ << "wrong defined boolean value for parameter " << elementName << "(> " << tmpString << ")";
				return false;
			}
			if(expParamSection == PropertySetting_DefinitionSection_Dependency_RegExp)
				expParamDefStrc.Dependencies.last().rRegularExpression.setPatternOptions(tmpOptions);
			else if(expParamSection == PropertySetting_DefinitionSection_Restriction_RegExp)
				expParamDefStrc.Restriction.rRegularExpression.setPatternOptions(tmpOptions);
			return true;
		}
	}
	return false;
}

bool PropertySettingDefinition::registerCustomPropertySettingDefinitionsTypes(const QHash<int, strcCustomVariantMetaTypeDescription> &hashCustomParamTypeToVariantMetaTypeInfo)
{
	QHashIterator<int, strcCustomVariantMetaTypeDescription> iter(hashCustomParamTypeToVariantMetaTypeInfo);
	while (iter.hasNext()) 
	{
		iter.next();
		if (mParamDefinitionsTypesMap.contains(iter.value().sName) == false)
			mCustomParamDefinitionsTypesMap.insert(iter.value().sName, (int)iter.key());
	}
	return true;
}