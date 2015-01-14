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

#ifndef PROPERTYSETTINGDEFINITION_H
#define PROPERTYSETTINGDEFINITION_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include "propertysetting.h" 

struct strcCustomVariantMetaTypeDescription
{
	QVariant::Type vType;
	QString sName;
	strcCustomVariantMetaTypeDescription()
	{
		vType = QVariant::Invalid;
		sName = "";
	}
};

class PropertySettingDefinition
{
public:

	PropertySettingDefinition();
	~PropertySettingDefinition();
	
	bool loadFromFile(const QString &sFilePath);//const QUrl &SFileUrl);
	int getFirstParameterID(const QString &sName);
	bool getParameterIDList(const QString &sName, QList<int> &sList);
	QString getParameterName(const int &nId);
	PropertySettingDefinitionStrc *getParameterDefinition(const int &nId);
	QList<PropertySettingDefinitionStrc> *getParameterDefinitions() {return &expParamDefinitions;};
	QList<PropertySettingGroupDefinitionStrc> *getGroupDefinitions() {return &expGroupDefinitions;};
	bool registerCustomPropertySettingDefinitionsTypes(const QHash<int, strcCustomVariantMetaTypeDescription> &hashCustomParamTypeToVariantMetaTypeInfo);

private:

	PropertySettingDefinitionStrc *parseParameterDefinition(QXmlStreamReader& xml);
	PropertySettingGroupDefinitionStrc *parseGroupDefinition(QXmlStreamReader& xml);
	bool addParameterDataToStructure(QXmlStreamReader& xml, PropertySettingDefinitionStrc &expParamDefStrc, PropertySettingDefinitionSection &expParamSection) const;
	bool addGroupDataToStructure(QXmlStreamReader& xml, PropertySettingGroupDefinitionStrc &expGroupDefStrc, PropertySettingDefinitionSection &expParamSection) const;

	QList<PropertySettingDefinitionStrc> expParamDefinitions;
	QList<PropertySettingGroupDefinitionStrc> expGroupDefinitions;
	mapPropertySettingDefinitionsTypesStrc mParamDefinitionsTypesMap;
	QMap<QString, int> mCustomParamDefinitionsTypesMap;
};

#endif // PROPERTYSETTINGDEFINITION_H
