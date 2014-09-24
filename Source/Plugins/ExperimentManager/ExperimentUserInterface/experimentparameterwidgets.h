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

#ifndef EXPERIMENTPARAMETERWIDGETS_H
#define EXPERIMENTPARAMETERWIDGETS_H

#include <QObject>
#include <QMutex>
#include "experimentparameterdefinition.h"

class ExperimentParameterVisualizer;

#define EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER				";"
#define EXPPARAMWIDGETS_UNIQUEPARAM_PARAMTYPE_INDEX			0
#define EXPPARAMWIDGETS_UNIQUEPARAM_OBJECTID_INDEX			1
#define EXPPARAMWIDGETS_UNIQUEPARAM_PARAMID_HEX_INDEX		2
#define EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX			3

struct ExperimentParameterDefinitionCollection
{
	ExperimentParameterDefinitionContainer *cExperimentParameterDefinition;
	ExperimentParameterVisualizer *wWidget;
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

class ExperimentParameterWidgets : public QObject
{
	Q_OBJECT

signals:
	void ParameterWidgetChanged(QWidget *, const QString&);

public:
	static ExperimentParameterWidgets* instance()
	{
		static QMutex mutex;
		if (!m_Instance)
		{
			mutex.lock();

			if (!m_Instance)
			{
				m_Instance = new ExperimentParameterWidgets;

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
	ExperimentParameterVisualizer *insertCustomExperimentParameterWidget(const QString &sCollectionName, QList<ExperimentParameterDefinitionStrc> *lExpParDefs);
	ExperimentParameterVisualizer *getExperimentParameterWidget(const QString &sCollectionName);
	//QWidget *getExperimentParameterWidgetSubControl(const QString &sCollectionName, const QString &sParameterName, const QString &sDerivedPrefixName, QString &sReturnUniquePropertyIdentifier, const QVariant &vValue, const bool &bDoInitWithValue);
	bool setWidgetParameter(const QString &sUniquePropertyIdentifier, const QString &sCollectionName, const QString &sParameterValue, const bool &bSetModified);
	ExperimentParameterDefinitionContainer *getExperimentParameterDefinition(const QString &sCollectionName);
	static QString getUniqueParameterIndentifier(const ParameterEditingType &cParamEditType, const int &nObjectID, const int &nParamDefinitionID, const QString &sParamName);

private:
	ExperimentParameterWidgets();
	~ExperimentParameterWidgets();

	ExperimentParameterWidgets(const ExperimentParameterWidgets &); // hide copy constructor
	ExperimentParameterWidgets& operator=(const ExperimentParameterWidgets &); // hide assign operator
	//We leave just the declarations, so the compiler will warn us if we try to use those two functions by accident

	static ExperimentParameterWidgets* m_Instance;

	void fetchExperimentParameterDefinitions();
	void createExperimentParameterWidgets();	

	QList<ExperimentParameterDefinitionCollection> *lExperimentParameterDefinitions;
};

#endif // EXPERIMENTPARAMETERWIDGETS_H
