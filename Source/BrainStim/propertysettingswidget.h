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

#ifndef PROPERTYSETTINGSWIDGET_H
#define PROPERTYSETTINGSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "mainappinfo.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

#include "propertysetting.h"
#include "propertysettingdefinition.h"
#include "propertysettingextensions.h"

namespace Ui { class PropertySettingsWidget; };

#define PROPSETTWIDGET_ENUM_SPEC_DIVIDER					"_"

class PropertySettingsWidget : public QWidget
{
	Q_OBJECT

signals:
	void destroyed(QWidget*);
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
		QList<PropertySettingDefinitionDependencyStrc> definitions;
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
	explicit PropertySettingsWidget(QWidget *parent = NULL);
	PropertySettingsWidget(const PropertySettingsWidget& other);
	~PropertySettingsWidget();

	bool checkIfParameterExists(const QString &sName);
	bool resetParameterProperties();
	bool addParameterProperty(const PropertySettingDefinitionStrc *expParamDef, const QVariant &vValue);
	bool removeParameterProperty(const QString &sUniquePropertyIdentifier);
	bool configurePropertyEditSignaling(const bool &bEnable);
	bool hasPropertyEditSignaling() {return bPropertyEditSignaling;};
	bool setParameter(const QString &sName, const QString &sValue, const bool &bSetModified = true, const bool &bIsInitializing = false);
	bool addGroupProperties(const QList<PropertySettingGroupDefinitionStrc> *expParamDef);
	bool parseDependencies(QtVariantProperty *variantProperty = NULL);
	bool addDependency(QtVariantProperty *variantProperty, const PropertySettingDefinitionDependencyStrc &dependencyParamDef);
	void setAutoDepencyParsing(bool bEnable);
	bool resetParameterModifiedFlagsAndValues(const bool &bOnlyNonDerivedParameters = true);
	bool setAllowedParameterValues(const QString &sName, const QStringList &lAllowedValues, const QVariantList &vTranslatedValues = QVariantList());

	QWidget *getParameterEditWidget(const QString &sName, const QString &sDerivedPrefixName, QString &sReturnUniquePropertyIdentifier, const QVariant &vValue, const bool &bDoInitWithValue, const bool &bIsScriptable);
	bool setWidgetParameter(const QString &sUniquePropertyIdentifier, const QString &sValue, const bool &bSetModified = true);
	VariantExtensionPropertySettingFactory *getVariantPropertyFactory() {return variantExtensionFactory;};
	bool getEnumeratedParameterPropertyValue(const QString &sFullEnumValueName, QVariant &vEnumValue);
	bool getEnumeratedParameterPropertyValue(QtProperty *pProperty, const QString &sEnumString, QVariant &vEnumValue);

	bool registerCustomVariabeleTypes(const QHash<int, strcCustomVariantMetaTypeDescription> &hashCustomVarTypeToVariantMetaType);
	bool isRegisteredCustomVariabeleType(QVariant::Type typeCustomVariant);
	int getRegisteredCustomVariantMetaType(QVariant::Type typeCustomVariant);

public slots:
	void resizeParameterView(const int &nWidth, const int &nHeight);

private slots:
	void propertyValueChanged(QtProperty *property, const QVariant &value);
	void itemEditedHandler(const QString &sParamName, const QString &sNewValue, const bool &bIsDerivedProperty);

//protected:
//	void focusOutEvent(QFocusEvent* event);

private:
	Ui::PropertySettingsWidget *ui;
	QVBoxLayout *mainLayout;
	QtTreePropertyBrowser *propertyEditor;
	QtGroupPropertyManager* groupManager;
	propertyContainerItems lGroupPropertyCollection;
	VariantExtensionPropertySettingManager* lVariantPropertyManager;
	VariantExtensionPropertySettingFactory *variantExtensionFactory;
	QList<propertyDependencyStruct> lPropertyDependencies;
	QHash<QString, propertyParameterValueDef> lParameterPropertyNamedHash;
	QHash<QString, enumValueStruct> lEnumeratedParameterPropertyValuesHash;
	QHash<QtProperty*, PropertySettingDefinitionStrc*> lVariantPropertyDefinitionHash;
	QHash<int, strcCustomVariantMetaTypeDescription> hashRegisteredCustomVariabeleTypeToVariantMetaTypes;
	bool bAutoDepencyParsing;
	bool bPropertyEditSignaling;

	bool addPropertyToSubGroup(const QString &sPropertyGroupNames, QtVariantProperty *item1, QList<propertyContainerItem> *pRootGroupPropertyItemList, QString &sSandPath = QString(""));
	void deleteSubGroupProperties(QList<propertyContainerItem> *pRootGroupPropertyItemList);
	bool registerDerivedParameterProperty(const propertyParameterValueDef &baseVPropertyDef, QtVariantProperty *derivedProperty, QString &sUniqueDerivedPropertyIdentifier);
};

#endif // PROPERTYSETTINGSWIDGET_H
