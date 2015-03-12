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

#ifndef PROPERTYSETTINGSWIDGETCONTAINER_H
#define PROPERTYSETTINGSWIDGETCONTAINER_H

#include <QObject>
#include <QMutex>
#include <QVariant>
#include "propertysettingdefinition.h"

class PropertySettingsWidget;

#define PROPSETTWIDGETS_UNIQUEPARAM_SPLITTER				";"
#define PROPSETTWIDGETS_UNIQUEPARAM_PARAMTYPE_INDEX			0
#define PROPSETTWIDGETS_UNIQUEPARAM_OBJECTID_INDEX			1
#define PROPSETTWIDGETS_UNIQUEPARAM_PARAMID_HEX_INDEX		2
#define PROPSETTWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX			3

struct ExperimentParameterDefinitionCollection
{
	PropertySettingDefinition *cExperimentParameterDefinition;
	PropertySettingsWidget *wWidget;
	QString sCollectionName;
	

	ExperimentParameterDefinitionCollection()
	{
		this->cExperimentParameterDefinition = NULL;
		this->wWidget = NULL;
	}
};

struct ExperimentControlDefinitionLocationInfo
{
	QString sPathToDefFile;
	QString sName;
};

class PropertySettingsWidgetContainer : public QObject
{
	Q_OBJECT

signals:
	void ParameterWidgetChanged(QWidget *, const QString&);

public:
	static PropertySettingsWidgetContainer* instance()
	{
		static QMutex mutex;
		if (!m_Instance)
		{
			mutex.lock();

			if (!m_Instance)
			{
				m_Instance = new PropertySettingsWidgetContainer;

			}
			mutex.unlock();
		}
		return m_Instance;
	}

	static void drop()
	{
		static QMutex mutex;
		mutex.lock();
		delete m_Instance;
		m_Instance = 0;
		mutex.unlock();
	}

	bool loadExperimentParameterDefinition(const QString &sFilePath, const QString &sCollectionName, const bool &bOnlyIfNotPresent = true, const QString &sCurrentDirectory = "");
	bool createCustomExperimentParameterWidgetCollection(const QString &sCollectionName, const bool &bClearExisting = false);
	PropertySettingsWidget *insertCustomExperimentParameterWidget(const QString &sCollectionName, QList<PropertySettingDefinitionStrc> *lExpParDefs);
	PropertySettingsWidget *getExperimentParameterWidget(const QString &sCollectionName);
	PropertySettingsWidget *cloneExperimentParameterWidget(const QString &sCollectionName, const QString &sCloneCollectionName);
	//QWidget *getPropertySettingsWidgetContainerubControl(const QString &sCollectionName, const QString &sParameterName, const QString &sDerivedPrefixName, QString &sReturnUniquePropertyIdentifier, const QVariant &vValue, const bool &bDoInitWithValue);
	bool setWidgetParameter(const QString &sUniquePropertyIdentifier, const QString &sCollectionName, const QString &sParameterValue, const bool &bSetModified);
	bool hasExperimentParameterDefinition(const QString &sCollectionName);
	PropertySettingDefinition *getExperimentParameterDefinition(const QString &sCollectionName);
	static QString getUniqueParameterIndentifier(const PropertySettingEditingType &cParamEditType, const int &nObjectID, const int &nParamDefinitionID, const QString &sParamName);

	int registerCustomVariabeleType(const QVariant::Type &typeVariantType, const QString &sName);

private:
	PropertySettingsWidgetContainer();
	~PropertySettingsWidgetContainer();

	PropertySettingsWidgetContainer(const PropertySettingsWidgetContainer &); // hide copy constructor
	PropertySettingsWidgetContainer& operator=(const PropertySettingsWidgetContainer &); // hide assign operator
	//We leave just the declarations, so the compiler will warn us if we try to use those two functions by accident

	static PropertySettingsWidgetContainer* m_Instance;

	//void fetchExperimentParameterDefinitions();
	void createPropertySettingsWidgetContainer();	

	QList<ExperimentParameterDefinitionCollection> *lExperimentParameterDefinitions;
	QHash<int, strcCustomVariantMetaTypeDescription> hashRegisteredCustomVariabeleTypeToCustomMetaTypeDescription;
	QMap<QString, QString> mapDefaultCollectionNameToFilePath;

};

#endif // PROPERTYSETTINGSWIDGETCONTAINER_H
