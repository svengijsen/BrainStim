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

#ifndef EXPERIMENTPARAMETERVISUALIZER_H
#define EXPERIMENTPARAMETERVISUALIZER_H

#include <QWidget>
#include <QVBoxLayout>
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

#include "experimentparameter.h"
#include "experimentparameterdefinition.h"
#include "parameterpropertyextensions.h"

namespace Ui {class ExperimentParameterVisualizer;};

#define EXPPARAMVIS_ENUM_SPEC_DIVIDER					"_"

class ExperimentParameterVisualizer : public QWidget
{
	Q_OBJECT

signals:
	void destroyed(QWidget*);
	//void editFinished(const QString&, const QString&);
	void rootItemEditFinished(const QString&, const QString&);
	void derivedItemEditFinished(const QString&, const QString&);

private:
	struct propertyContainerItem
	{
		QtProperty *lGroupProperty;
		QList<propertyContainerItem> *pSubItems;
		propertyContainerItem()
		{
			lGroupProperty = NULL;
			pSubItems = NULL;
		}
	};

	struct propertyContainerItems
	{
		QList<propertyContainerItem> propItem;
	};

	struct propertyDependencyStruct
	{
		QtVariantProperty *vProperty;
		QList<ExperimentParameterDefinitionDependencyStrc> definitions;
		propertyDependencyStruct()
		{
			vProperty = NULL;
		}
	};

	struct propertyParameterValueDef
	{
		QtVariantProperty* vProperty;
		QVariant::Type vType;
		bool bIsDerived;
		propertyParameterValueDef()
		{
			vProperty = NULL;
			vType = QVariant::Invalid;
			bIsDerived = false;
		}
	};
	
	struct enumValueStruct
	{
		int nEnumControlIndex;
		QVariant vTranslationValue;
		enumValueStruct()
		{
			nEnumControlIndex = -1;
			vTranslationValue = QVariant::Invalid;
		}
	};

public:
	explicit ExperimentParameterVisualizer(QWidget *parent = NULL);
	ExperimentParameterVisualizer(const ExperimentParameterVisualizer& other);
	~ExperimentParameterVisualizer();

	bool checkIfParameterExists(const QString &sName);
	bool addParameterProperty(const ExperimentParameterDefinitionStrc *expParamDef, const QVariant &vValue);
	bool removeParameterProperty(const QString &sUniquePropertyIdentifier);
	bool configurePropertyEditSignaling(const bool &bEnable);
	bool hasPropertyEditSignaling() {return bPropertyEditSignaling;};
	bool setParameter(const QString &sName, const QString &sValue, const bool &bSetModified = true, const bool &bIsInitializing = false);
	bool addGroupProperties(const QList<ExperimentGroupDefinitionStrc> *expParamDef);
	bool parseDependencies(QtVariantProperty *variantProperty = NULL);
	bool addDependency(QtVariantProperty *variantProperty, const ExperimentParameterDefinitionDependencyStrc &dependencyParamDef);
	void setAutoDepencyParsing(bool bEnable);
	bool resetParameterModifiedFlags(const bool &bOnlyNonDerivedParameters = true);
	bool setAllowedParameterValues(const QString &sName, const QStringList &lAllowedValues, const QVariantList &vTranslatedValues = QVariantList());

	QWidget *getParameterEditWidget(const QString &sName, const QString &sDerivedPrefixName, QString &sReturnUniquePropertyIdentifier, const QVariant &vValue, const bool &bDoInitWithValue, const bool &bIsScriptable);
	bool setWidgetParameter(const QString &sUniquePropertyIdentifier, const QString &sValue, const bool &bSetModified = true);
	VariantExtensionPropertyFactory *getVariantPropertyFactory() {return variantExtensionFactory;};
	bool getEnumeratedParameterPropertyValue(const QString &sFullEnumValueName, QVariant &vEnumValue);
	bool getEnumeratedParameterPropertyValue(QtProperty *pProperty, const QString &sEnumString, QVariant &vEnumValue);

public slots:
	void resizeParameterView(const int &nWidth, const int &nHeight);

private slots:
	void propertyValueChanged(QtProperty *property, const QVariant &value);
	void itemEditedHandler(const QString &sParamName, const QString &sNewValue, const bool &bIsDerivedProperty);

private:
	Ui::ExperimentParameterVisualizer *ui;
	QVBoxLayout *mainLayout;
	QtTreePropertyBrowser *propertyEditor;
	QtGroupPropertyManager* groupManager;
	propertyContainerItems lGroupPropertyCollection;
	VariantExtensionPropertyManager* lVariantPropertyManager;
	VariantExtensionPropertyFactory *variantExtensionFactory;
	QList<propertyDependencyStruct> lPropertyDependencies;
	QHash<QString, propertyParameterValueDef> lParameterPropertyNamedHash;
	QHash<QString, enumValueStruct> lEnumeratedParameterPropertyValuesHash;
	QHash<QtProperty*, ExperimentParameterDefinitionStrc*> lVariantPropertyDefinitionHash;
	bool bAutoDepencyParsing;
	bool bPropertyEditSignaling;

	//void ExperimentParameterVisualizerDefaultConstruct();
	bool addPropertyToSubGroup(const QString &sPropertyGroupNames, QtVariantProperty *item1, QList<propertyContainerItem> *pRootGroupPropertyItemList, QString &sSandPath = QString(""));
	void deleteSubGroupProperties(QList<propertyContainerItem> *pRootGroupPropertyItemList);

	bool registerDerivedParameterProperty(const propertyParameterValueDef &baseVPropertyDef, QtVariantProperty *derivedProperty, QString &sUniqueDerivedPropertyIdentifier);
};

#endif // EXPERIMENTPARAMETERVISUALIZER_H
