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

#ifndef EXPERIMENTPARAMETERDEFINITION_H
#define EXPERIMENTPARAMETERDEFINITION_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QRegularExpression>
#include <QXmlStreamReader>

#define EXPERIMENT_GROUP_TAG					"group"
#define EXPERIMENT_GROUPS_TAG					"groups"
#define EXPERIMENT_PARAMETER_TAG				"parameter"
#define EXPERIMENT_PARAMETERS_TAG				"parameters"
#define EXPERIMENT_CUSTOMPARAMETERS_TAG			"custom_parameters"
#define EXPERIMENT_ID_TAG						"id"
#define EXPERIMENT_ENABLED_TAG					"enabled"
#define EXPERIMENT_EDITABLE_TAG					"editable"
#define EXPERIMENT_SCRIPTABLE_TAG				"scriptable"
#define EXPERIMENT_NAME_TAG						"name"
#define EXPERIMENT_DISPLAYNAME_TAG				"displayname"
#define EXPERIMENT_GROUPPATH_TAG				"grouppath"
#define EXPERIMENT_AUTOHIDE_TAG					"autohide"
#define EXPERIMENT_INFORMATION_TAG				"information"
#define EXPERIMENT_TYPE_TAG						"type"
#define EXPERIMENT_DEFAULTVALUE_TAG				"defaultvalue"
#define EXPERIMENT_RESTRICTION_TAG				"restriction"
#define EXPERIMENT_MINIMAL_TAG					"minimal"
#define EXPERIMENT_MAXIMAL_TAG					"maximal"
#define EXPERIMENT_VALUE_TAG					"value"
#define EXPERIMENT_ALLOWED_TAG					"allowed"
#define EXPERIMENT_DEPENDENCIES_TAG				"dependencies"
#define EXPERIMENT_DEPENDENCY_TAG				"dependency"
#define EXPERIMENT_RELATION_TAG					"relationid"
#define EXPERIMENT_REGEXP_TAG					"regularexpression"
#define EXPERIMENT_PATTERN_TAG					"pattern"
#define EXPERIMENT_CASESENSITIVE_TAG			"casesensitive"
#define EXPERIMENT_LISTSEP_CHAR					";"
#define EXPERIMENT_PARAM_GROUPSEP_CHAR			";"
#define EXPERIMENT_CUSTOMPARAM_INFOSTRING		"Custom user defined parameter."

enum ExperimentDefinitionSection
{
	Experiment_ParameterSection_Root					=  0,
	Experiment_ParameterSection_Parameter				=  1,
	Experiment_ParameterSection_Restriction				=  2,
	Experiment_ParameterSection_Restriction_Minimal		=  3,
	Experiment_ParameterSection_Restriction_Maximal		=  4,
	Experiment_ParameterSection_Restriction_RegExp		=  5,
	Experiment_ParameterSection_Dependency				=  6,
	Experiment_ParameterSection_Dependency_RegExp		=  7,
	Experiment_GroupSection_Group						=  8,
	Experiment_GroupSection_Dependency					=  9,
	Experiment_GroupSection_Dependency_RegExp			= 10
};

enum ExperimentParameterTypeName
{
	Experiment_ParameterType_Unknown				= 0,
	Experiment_ParameterType_String					= 1,
	Experiment_ParameterType_StringArray			= 2,
	Experiment_ParameterType_Color					= 3,
	Experiment_ParameterType_Integer				= 4,
	Experiment_ParameterType_Float					= 5,
	Experiment_ParameterType_Double					= 6,
	Experiment_ParameterType_Boolean				= 7,
	Experiment_ParameterType_RotationDirection		= 8,
	Experiment_ParameterType_MovementDirection		= 9,
	Experiment_ParameterType_EccentricityDirection	= 10,
	Experiment_ParameterType_FilePath				= 11,
	Experiment_ParameterType_MethodType				= 12,
	Experiment_ParameterType_DynamicStringEnum		= 13, 
	Experiment_ParameterType_DynamicIntegerEnum		= 14//Filled at runtime
	//See also below here!!!	
};

struct mapParamDefinitionsTypesStrc : QMap<QString, ExperimentParameterTypeName>
{
	mapParamDefinitionsTypesStrc()
	{
		this->operator[]( "string" )				= Experiment_ParameterType_String;
		this->operator[]( "stringarray" )			= Experiment_ParameterType_StringArray;
		this->operator[]( "color" )					= Experiment_ParameterType_Color;
		this->operator[]( "integer" )				= Experiment_ParameterType_Integer;
		this->operator[]( "short" )					= Experiment_ParameterType_Integer;
		this->operator[]( "int" )					= Experiment_ParameterType_Integer;
		this->operator[]( "float" )					= Experiment_ParameterType_Float;
		this->operator[]( "double" )				= Experiment_ParameterType_Double;
		this->operator[]( "boolean" )				= Experiment_ParameterType_Boolean;
		this->operator[]( "rotationdirection" )		= Experiment_ParameterType_RotationDirection;
		this->operator[]( "movementdirection" )		= Experiment_ParameterType_MovementDirection;
		this->operator[]( "eccentricitydirection" )	= Experiment_ParameterType_EccentricityDirection;
		this->operator[]( "filepath" )				= Experiment_ParameterType_FilePath;
		this->operator[]( "methodtype" )			= Experiment_ParameterType_MethodType;
		this->operator[]( "dynamicenumstring" )		= Experiment_ParameterType_DynamicStringEnum;
		this->operator[]( "dynamicenuminteger" )	= Experiment_ParameterType_DynamicIntegerEnum;
	};
	~mapParamDefinitionsTypesStrc(){};
};

struct ExperimentParameterMinMaxRestrictionStrc
{
	bool bEnabled;
	QVariant vValue;
	ExperimentParameterMinMaxRestrictionStrc()
	{
		this->bEnabled = false;
		this->vValue = NULL;
	};
	~ExperimentParameterMinMaxRestrictionStrc() {};
};

struct ExperimentParameterDefinitionRestrictionStrc
{
	QStringList lAllowedValues;
	ExperimentParameterMinMaxRestrictionStrc MinimalValue;
	ExperimentParameterMinMaxRestrictionStrc MaximalValue;
	QRegularExpression rRegularExpression;
};

struct ExperimentParameterDefinitionDependencyStrc
{
	int nId;
	int nDependencyParameterID;
	bool bHideWhenInactive;
	QRegularExpression rRegularExpression;
	ExperimentParameterDefinitionDependencyStrc()
	{
		this->nId = -1;
		this->nDependencyParameterID = -1;
		this->bHideWhenInactive = false;
	};
	~ExperimentParameterDefinitionDependencyStrc() {};
};

struct ExperimentParameterDefinitionStrc
{
	int nId;
	bool bEnabled;
	bool bIsEditable;
	QString sName;
	QString sDisplayName;
	QString sGroupPath;//Can also be a directory like "Root;Item 1;SubItem A"
	QString sInformation;
	ExperimentParameterTypeName eType;
	//ParameterEditingType eEditType;
	bool bCanBeScriptReference;
	QString sDefaultValue;
	//QList<ExperimentParameterDefinitionRestrictionStrc> Restrictions;
	ExperimentParameterDefinitionRestrictionStrc Restriction;
	QList<ExperimentParameterDefinitionDependencyStrc> Dependencies;

	ExperimentParameterDefinitionStrc()
	{
		nId = -1;
		bEnabled = false;
		bIsEditable = false;
		sName = "";
		sDisplayName = "";
		sGroupPath = "";
		sInformation = "";
		eType = Experiment_ParameterType_Unknown;
		sDefaultValue = "";
		bCanBeScriptReference = false;
		//eEditType = PEM_DEFINED;
	};
	~ExperimentParameterDefinitionStrc() {};
};

struct ExperimentGroupDefinitionStrc
{
	int nId;
	bool bEnabled;
	QString sGroupPath;
	QList<ExperimentParameterDefinitionDependencyStrc> Dependencies;

	ExperimentGroupDefinitionStrc()
	{
		nId = -1;
		bEnabled = false;
		sGroupPath = "";
	};
	~ExperimentGroupDefinitionStrc() {};
};

enum ParameterEditingType
{
	PEM_DEFINED						=  0,
	PEM_CUSTOM						=  1
};

class ExperimentParameterDefinitionContainer
{
public:

	ExperimentParameterDefinitionContainer();
	~ExperimentParameterDefinitionContainer();
	
	bool loadFromFile(const QString &sFilePath);//const QUrl &SFileUrl);
	int getFirstParameterID(const QString &sName);
	bool getParameterIDList(const QString &sName, QList<int> &sList);
	QString getParameterName(const int &nId);
	ExperimentParameterDefinitionStrc *getParameterDefinition(const int &nId);
	QList<ExperimentParameterDefinitionStrc> *getParameterDefinitions() {return &expParamDefinitions;};
	QList<ExperimentGroupDefinitionStrc> *getGroupDefinitions() {return &expGroupDefinitions;};

private:

	ExperimentParameterDefinitionStrc *parseParameterDefinition(QXmlStreamReader& xml);
	ExperimentGroupDefinitionStrc *parseGroupDefinition(QXmlStreamReader& xml);
	bool addParameterDataToStructure(QXmlStreamReader& xml, ExperimentParameterDefinitionStrc &expParamDefStrc, ExperimentDefinitionSection &expParamSection) const;
	bool addGroupDataToStructure(QXmlStreamReader& xml, ExperimentGroupDefinitionStrc &expGroupDefStrc, ExperimentDefinitionSection &expParamSection) const;

	QList<ExperimentParameterDefinitionStrc> expParamDefinitions;
	QList<ExperimentGroupDefinitionStrc> expGroupDefinitions;
	mapParamDefinitionsTypesStrc mParamDefinitionsTypesMap;
};

#endif // EXPERIMENTPARAMETERDEFINITION_H
