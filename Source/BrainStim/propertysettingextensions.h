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

#ifndef PROPERTYSETTINGEXTENSIONS_H
#define PROPERTYSETTINGEXTENSIONS_H

#include "qtvariantproperty.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QDialog>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QToolButton>
#include "propertysettingdefinition.h"

#define EXPERIMENT_PARAMETER_ARRAYSEP_CHAR		","
#define EXPERIMENT_PARAMETER_DERIVED_CHAR		"_"

class PropertySettingBase : public QWidget
{
	Q_OBJECT

signals:
	void PropertyWidgetChanged(const QString&);

public:
	PropertySettingBase(QWidget *parent = NULL);
	~PropertySettingBase();

	struct strcCustomFixedWidget
	{
		int nStretch;
		QWidget* pCustomWidget;
		strcCustomFixedWidget()
		{
			nStretch = 0;
			pCustomWidget = NULL;
		}
	};

	virtual void setFixedValue(const QString &sValue) = 0;
	
	void setCustomFixedWidgetList(const QList<strcCustomFixedWidget> &lCustFixWidgets);
	void setValue(const QString &sValue);
	void setScriptability(const bool &bIsScriptable = true);
	QString getValue();
	void setAndEmitEditedValue(const QString &sValue);
	QHBoxLayout *getMainLayout();
	QWidget *getParentWidget() {return parentWidget;};
	void installEventFilterFromWidget(QWidget *pWidget);
	
protected:
	bool eventFilter(QObject* object, QEvent* event);

private slots:
	void changeParamValueTypeClicked();
	void variabeleParameterNameChanged(const QString &sParamName);

private:
	void createNewLayout();
	void changeParamValueType(const bool &bNewParamValueIsFixed, const bool &bForceUpdate = false);
	void addCustomFixedWidgetsToLayout();
	void removeCustomFixedWidgetsFromLayout();

	QList<strcCustomFixedWidget> lCustomFixedWidgets;
	QWidget *parentWidget;
	QWidget *pEventFilterWidget;
	QHBoxLayout *layout;
	QToolButton *changeParamValueButton;
	QPixmap *fixedPixmapIcon;
	QPixmap *variabelePixmapIcon;
	QIcon iFixedIcon;
	QIcon iVariabeleIcon;
	QLineEdit *leVariabeleParameter;
	QString sCurrentFixedValue;
	QString sCurrentVariabeleValue;
	QSize *sIconSize;
	bool bIsScriptableParameter;
	bool bIsFixedParamValue;
};

class StringArrayPropertyType{};
Q_DECLARE_METATYPE(StringArrayPropertyType)
class FilePathPropertyType{};
Q_DECLARE_METATYPE(FilePathPropertyType)
class Scriptable_QVariantPropertyType{};
Q_DECLARE_METATYPE(Scriptable_QVariantPropertyType)

class ScriptableVariantPropertySetting : public PropertySettingBase 
{
	Q_OBJECT

public:
	ScriptableVariantPropertySetting(QWidget *parent = NULL);
	~ScriptableVariantPropertySetting();

	void setDerivedEditorAndManager(QtVariantPropertyManager *pManager, QWidget *pWidget, const int &nPropertyType, QtProperty *pDerivedProp);
	void setFixedValue(const QString &sValue);

private slots:
	void derivedEditorSelectionChanged(const int &nIndex);

private:
	QWidget *derivedQVariantSubWidget;
	int derivedQVariantSubWidgetType;
	QtProperty *derivedQVariantSubWidgetProperty;
	QtVariantPropertyManager *pDerivedQVariantSubWidgetManager;
};

class StringArrayPropertySetting : public PropertySettingBase 
{
	Q_OBJECT

public:
	StringArrayPropertySetting(QWidget *parent = NULL);
	~StringArrayPropertySetting();

	void setFixedValue(const QString &sValue);
	void setSeperator(const QString &sSeperator);

private slots:
	void currentIndexChangedSlot(int nIndex);
	void checkAcception();

private:
	void createEditorComponents();
	void destroyEditorComponents();

	QDialog *tmpDialog;
	QVBoxLayout *dialogLayout;
	QPlainTextEdit *tmpTextEdit;
	QDialogButtonBox *buttonBox;
	QComboBox *cmbStringArrayConf;
	QString sCurrentSeperator;
};

class FilePathPropertySetting : public PropertySettingBase
{
	Q_OBJECT

public:
	FilePathPropertySetting(QWidget *parent = NULL);
	~FilePathPropertySetting();

	void setFixedValue(const QString &sValue);

private slots:
	void currentTextChangedSlot(const QString &sText);
	void browseForFile();

private:
	void createEditorComponents();
	void destroyEditorComponents();

	QToolButton *browseButton;
	QLineEdit *leFilePath;
	QPixmap *pixmapIcon;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class PropertySettingsWidget;
class VariantExtensionPropertySettingManager :	public QtVariantPropertyManager
{
	Q_OBJECT

public:

	VariantExtensionPropertySettingManager(PropertySettingsWidget *parentPropSettWidget = NULL);
	~VariantExtensionPropertySettingManager();

	static int stringArrayTypeId()
	{
		return qMetaTypeId<StringArrayPropertyType>();
	}
	static int filePathTypeId()
	{
		return qMetaTypeId<FilePathPropertyType>();
	}
	static int scriptable_QVariantTypeId()
	{
		return qMetaTypeId<Scriptable_QVariantPropertyType>();
	}

	bool isPropertyTypeSupported(int nPropertyType) const;
	bool isCustomProperty(const QtProperty *pProperty) const;
	bool isScriptableProperty(const QtProperty *pProperty) const;
	bool isScriptReferenceValue(const QVariant &vValue) const;
	static bool isScriptReferenceString(const QString &sValue);
	bool hasScriptReferenceValue(const QtProperty *pProperty) const;
	bool isManagedProperty(const QtProperty *pProperty) const;
	bool isCustomPropertyType(const int &nPropertyType) const;
	bool shouldPullMappedData(const QtProperty *pProperty) const;
	bool shouldPushMappedData(const QtProperty *pProperty, const QVariant &sNewValue) const;

	QVariant value(const QtProperty *property) const;
	QtVariantProperty *addVariantExtensionProperty(int propertyType, const bool &bIsScriptable, const QString &name = QString());

public slots:
	void setValue(QtProperty *property, const QVariant &val);
	static QVariant resolveParameterValueType(const QVariant &vInput, const int &nVariantType, const bool &bToView);

protected:
	QString valueText(const QtProperty *property) const;
	void initializeProperty(QtProperty *property);
	void uninitializeProperty(QtProperty *property);

private:

	struct VariantExtensionPropertyData 
	{
		QString sValue;
		bool bIsScriptable;
		VariantExtensionPropertyData()
		{
			sValue = "";
			bIsScriptable = false;
		}
	};
	QMap<const QtProperty *, VariantExtensionPropertyData> mapVariantExtensionPropertyData;
	PropertySettingsWidget *parentPropertySettingsWidget;

};

//////////////////////////////////////////////////////////////////////////////////

class VariantExtensionPropertySettingFactory :	public QtVariantEditorFactory
{
	Q_OBJECT

public:
	VariantExtensionPropertySettingFactory(PropertySettingsWidget *pPropSettWidget = NULL) : QtVariantEditorFactory((QObject*)pPropSettWidget), parentPropSettWidget(pPropSettWidget) {};
	~VariantExtensionPropertySettingFactory(){};

	QWidget *getEditorWidget(QtVariantPropertyManager *manager, QtVariantProperty *vProperty, const QString &sDerivedPrefixName, QWidget *parent, QString &sReturnUniquePropertyIdentifier, QtVariantProperty *&pDerivedVariantProperty, const QVariant &vValue, const bool &bDoInitWithValue, const bool &bIsScriptable);
	bool setPropertyValue(QtVariantPropertyManager *manager, const QString &sUniquePropertyIdentifier, const QString &sValue, const bool &bSetModified = true);

private slots:
	void slotCustomPropertyChanged(const QString &val);
	void slotEditorDestroyed(QObject *obj);

private:

	QMap<QtProperty *, QList<QWidget *>> createdEditors;
	QMap<QWidget *, QtProperty *> editorToProperty;
	PropertySettingsWidget *parentPropSettWidget;

protected:
	QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent);
};

#endif//PROPERTYSETTINGEXTENSIONS_H

