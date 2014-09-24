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

#ifndef PARAMETERPROPERTYEXTENSIONS_H
#define PARAMETERPROPERTYEXTENSIONS_H

#include "qtvariantproperty.h"
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QDialog>
#include <QPlainTextEdit>
#include <QDialogButtonBox>
#include <QToolButton>
#include "experimentparameterdefinition.h"
#include "experimentstructures.h"

#define EXPERIMENT_PARAMETER_ARRAYSEP_CHAR		","
#define EXPERIMENT_PARAMETER_DERIVED_CHAR		"_"

class PropertyWidgetBase : public QWidget
{
	Q_OBJECT

signals:
	void PropertyWidgetChanged(const QString&);

public:
	PropertyWidgetBase(QWidget *parent = NULL);
	~PropertyWidgetBase();

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
class RotationDirectionPropertyType{};
Q_DECLARE_METATYPE(RotationDirectionPropertyType)
class MovementDirectionPropertyType{};
Q_DECLARE_METATYPE(MovementDirectionPropertyType)
class EccentricityDirectionPropertyType{};
Q_DECLARE_METATYPE(EccentricityDirectionPropertyType)
class MethodTypePropertyType{};
Q_DECLARE_METATYPE(MethodTypePropertyType)
//class Scriptable_QVariantPropertyType{};
//Q_DECLARE_METATYPE(Scriptable_QVariantPropertyType)

class ScriptableQVariantPropertyWidget : public PropertyWidgetBase 
{
	Q_OBJECT

public:
	ScriptableQVariantPropertyWidget(QWidget *parent = NULL);
	~ScriptableQVariantPropertyWidget();

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

class StringArrayPropertyWidget : public PropertyWidgetBase 
{
	Q_OBJECT

public:
	StringArrayPropertyWidget(QWidget *parent = NULL);
	~StringArrayPropertyWidget();

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

class FilePathPropertyWidget : public PropertyWidgetBase
{
	Q_OBJECT

public:
	FilePathPropertyWidget(QWidget *parent = NULL);
	~FilePathPropertyWidget();

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

class RotationDirectionPropertyWidget : public PropertyWidgetBase 
{
	Q_OBJECT

private slots:
	void currentIndexChangedSlot(int nIndex);

public:
	RotationDirectionPropertyWidget(QWidget *parent = NULL);
	~RotationDirectionPropertyWidget();

	enum RotationDirectionEnum
	{
		ROTATION_DIR_COUNTERCLOCKWISE	= -1,
		ROTATION_DIR_UNDEFINED			=  0,
		ROTATION_DIR_CLOCKWISE			=  1
	};

	static QString rotationDirectionString(enum RotationDirectionEnum);
	static RotationDirectionEnum rotationDirectionEnum(const QString &sName);
	void setFixedValue(const QString &sValue);

private:
	void createEditorComponents();
	void destroyEditorComponents();

	static QMap<int, RotationDirectionEnum> indexToEnumHash;
	static QMap<int, QString> mRotationDirection;

	QComboBox *cmbSelection;
};

class MethodTypePropertyWidget : public PropertyWidgetBase 
{
	Q_OBJECT

		private slots:
			void currentIndexChangedSlot(int nIndex);

public:
	MethodTypePropertyWidget(QWidget *parent = NULL);
	~MethodTypePropertyWidget();

	void setFixedValue(const QString &sValue);

private:
	void createEditorComponents();
	void destroyEditorComponents();

	static QMap<int, ExperimentStructuresNameSpace::MethodType> indexToEnumMap;
	static QMap<int, QString> mapMethodType;

	QComboBox *cmbSelection;
};

class MovementDirectionPropertyWidget : public PropertyWidgetBase 
{
	Q_OBJECT

private slots:
	void currentIndexChangedSlot(int nIndex);

public:
	MovementDirectionPropertyWidget(QWidget *parent = NULL);
	~MovementDirectionPropertyWidget();

	enum MovementDirectionEnum
	{
		MOVEMENT_DIR_DOWNUP				= -1,
		MOVEMENT_DIR_UNDEFINED			=  0,
		MOVEMENT_DIR_UPDOWN				=  1
	};

	static QString movementDirectionString(enum MovementDirectionEnum eValue);
	static MovementDirectionEnum movementDirectionEnum(const QString &sName);
	void setFixedValue(const QString &sValue);

private:
	void createEditorComponents();
	void destroyEditorComponents();

	static QMap<int, MovementDirectionEnum> indexToEnumHash;
	static QMap<int, QString> mMovementDirection;

	QComboBox *cmbSelection;
};

class EccentricityDirectionPropertyWidget : public PropertyWidgetBase 
{
	Q_OBJECT

private slots:
	void currentIndexChangedSlot(int nIndex);

public:
	EccentricityDirectionPropertyWidget(QWidget *parent = NULL);
	~EccentricityDirectionPropertyWidget();

	enum EccentricityDirectionEnum
	{
		ECCENTRICITY_DIR_DECREASE		= -1,
		ECCENTRICITY_DIR_UNDEFINED		=  0,
		ECCENTRICITY_DIR_INCREASE		=  1
	};

	static QString eccentricityDirectionString(enum EccentricityDirectionEnum eValue);
	static EccentricityDirectionEnum eccentricityDirectionEnum(const QString &sName);
	void setFixedValue(const QString &sValue);

private:
	void createEditorComponents();
	void destroyEditorComponents();

	static QMap<int, EccentricityDirectionEnum> indexToEnumHash;
	static QMap<int, QString> mEccentricityDirection;

	QComboBox *cmbSelection;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ExperimentParameterVisualizer;
class VariantExtensionPropertyManager :	public QtVariantPropertyManager
{
	Q_OBJECT

public:

	VariantExtensionPropertyManager(ExperimentParameterVisualizer *parentParamVisualizer = NULL);
	~VariantExtensionPropertyManager();

	static int rotationDirectionTypeId()
	{
		return qMetaTypeId<RotationDirectionPropertyType>();
	}
	static int movementDirectionTypeId()
	{
		return qMetaTypeId<MovementDirectionPropertyType>();
	}
	static int eccentricityDirectionTypeId()
	{
		return qMetaTypeId<EccentricityDirectionPropertyType>();
	}
	static int stringArrayTypeId()
	{
		return qMetaTypeId<StringArrayPropertyType>();
	}
	static int filePathTypeId()
	{
		return qMetaTypeId<FilePathPropertyType>();
	}
	static int methodTypeTypeId()
	{
		return qMetaTypeId<MethodTypePropertyType>();
	}
	//static int scriptable_QVariantTypeId()
	//{
	//	return qMetaTypeId<Scriptable_QVariantPropertyType>();
	//}

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
	static QVariant resolveParameterValueType(const QVariant &vInput, const ExperimentParameterTypeName &sType, const bool &bToView);

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
	ExperimentParameterVisualizer *parentParameterVisualizer;
};

//////////////////////////////////////////////////////////////////////////////////

class VariantExtensionPropertyFactory :	public QtVariantEditorFactory
{
	Q_OBJECT

public:
	VariantExtensionPropertyFactory(ExperimentParameterVisualizer *pParentExpVis = NULL) : QtVariantEditorFactory((QObject*)pParentExpVis), pExperimentVisualizer(pParentExpVis) {};
	~VariantExtensionPropertyFactory(){};

	QWidget *getEditorWidget(QtVariantPropertyManager *manager, QtVariantProperty *vProperty, const QString &sDerivedPrefixName, QWidget *parent, QString &sReturnUniquePropertyIdentifier, QtVariantProperty *&pDerivedVariantProperty, const QVariant &vValue, const bool &bDoInitWithValue, const bool &bIsScriptable);
	bool setPropertyValue(QtVariantPropertyManager *manager, const QString &sUniquePropertyIdentifier, const QString &sValue, const bool &bSetModified = true);

private slots:
	void slotCustomPropertyChanged(const QString &val);
	void slotEditorDestroyed(QObject *obj);

private:

	QMap<QtProperty *, QList<QWidget *>> createdEditors;
	QMap<QWidget *, QtProperty *> editorToProperty;
	ExperimentParameterVisualizer *pExperimentVisualizer;

protected:
	QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent);
};

#endif

