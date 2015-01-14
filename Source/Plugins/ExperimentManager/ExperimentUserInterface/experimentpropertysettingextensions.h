//ExperimentManagerplugin
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

#ifndef EXPERIMENTPROPERTYSETTINGEXTENSIONS_H
#define EXPERIMENTPROPERTYSETTINGEXTENSIONS_H

#include "qtvariantproperty.h"
#include <QComboBox>
#include "../../BrainStim/propertysettingextensions.h"
#include "experimentstructures.h"

class RotationDirectionPropertySetting : public PropertySettingBase
{
	Q_OBJECT

private slots:
	void currentIndexChangedSlot(int nIndex);

public:
	RotationDirectionPropertySetting(QWidget *parent = NULL);
	~RotationDirectionPropertySetting();

	enum RotationDirectionEnum
	{
		ROTATION_DIR_COUNTERCLOCKWISE = -1,
		ROTATION_DIR_UNDEFINED = 0,
		ROTATION_DIR_CLOCKWISE = 1
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

class RotationDirectionPropertyType : public QObject
{
	Q_OBJECT
public:
	RotationDirectionPropertyType(QObject *parent = NULL) : QObject(parent) { ; };
	RotationDirectionPropertyType(const RotationDirectionPropertyType& other){ Q_UNUSED(other); }//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~RotationDirectionPropertyType() { ; };
	Q_INVOKABLE QVariant resolveParameterValueType(const QVariant &vInput, const bool &bToView)
	{
		bool bIsInteger = false;
		int nIntegerValue = vInput.toInt(&bIsInteger);
		if (bToView)
		{
			if (bIsInteger)
				return RotationDirectionPropertySetting::rotationDirectionString((RotationDirectionPropertySetting::RotationDirectionEnum)nIntegerValue);
			else
				return vInput.toString();
		}
		else
		{
			if (bIsInteger)
				return nIntegerValue;
			else if (VariantExtensionPropertySettingManager::isScriptReferenceString(vInput.toString()))
				return vInput.toString();
			else
				return (int)RotationDirectionPropertySetting::rotationDirectionEnum(vInput.toString());
		}
	};

	Q_INVOKABLE QWidget* getNewEditorObject()
	{
		return new RotationDirectionPropertySetting();//parent??
	}
};
Q_DECLARE_METATYPE(RotationDirectionPropertyType)

class MovementDirectionPropertySetting : public PropertySettingBase
{
	Q_OBJECT

		private slots:
	void currentIndexChangedSlot(int nIndex);

public:
	MovementDirectionPropertySetting(QWidget *parent = NULL);
	~MovementDirectionPropertySetting();

	enum MovementDirectionEnum
	{
		MOVEMENT_DIR_DOWNUP = -1,
		MOVEMENT_DIR_UNDEFINED = 0,
		MOVEMENT_DIR_UPDOWN = 1
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

class MovementDirectionPropertyType : public QObject
{
	Q_OBJECT

public:
	MovementDirectionPropertyType(QObject *parent = NULL) : QObject(parent) { ; };
	MovementDirectionPropertyType(const MovementDirectionPropertyType& other){ Q_UNUSED(other); }//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~MovementDirectionPropertyType() { ; };
	Q_INVOKABLE QVariant resolveParameterValueType(const QVariant &vInput, const bool &bToView)
	{
		bool bIsInteger = false;
		int nIntegerValue = vInput.toInt(&bIsInteger);
		if (bToView)
		{
			if (bIsInteger)
				return MovementDirectionPropertySetting::movementDirectionString((MovementDirectionPropertySetting::MovementDirectionEnum)vInput.toInt());
			else
				return vInput.toString();
		}
		else
		{
			if (bIsInteger)
				return nIntegerValue;
			else if (VariantExtensionPropertySettingManager::isScriptReferenceString(vInput.toString()))
				return vInput.toString();
			else
				return (int)MovementDirectionPropertySetting::movementDirectionEnum(vInput.toString());
		}
	};

	Q_INVOKABLE QWidget* getNewEditorObject()
	{
		return new MovementDirectionPropertySetting();//parent??
	}
};
Q_DECLARE_METATYPE(MovementDirectionPropertyType)

class EccentricityDirectionPropertyWidget : public PropertySettingBase
{
	Q_OBJECT

		private slots:
	void currentIndexChangedSlot(int nIndex);

public:
	EccentricityDirectionPropertyWidget(QWidget *parent = NULL);
	~EccentricityDirectionPropertyWidget();

	enum EccentricityDirectionEnum
	{
		ECCENTRICITY_DIR_DECREASE = -1,
		ECCENTRICITY_DIR_UNDEFINED = 0,
		ECCENTRICITY_DIR_INCREASE = 1
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
class EccentricityDirectionPropertyType : public QObject
{
	Q_OBJECT

public:
	EccentricityDirectionPropertyType(QObject *parent = NULL) : QObject(parent) { ; };
	EccentricityDirectionPropertyType(const EccentricityDirectionPropertyType& other){ Q_UNUSED(other); }//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~EccentricityDirectionPropertyType() { ; };
	Q_INVOKABLE QVariant resolveParameterValueType(const QVariant &vInput, const bool &bToView)
	{
		bool bIsInteger = false;
		int nIntegerValue = vInput.toInt(&bIsInteger);
		if (bToView)
		{
			if (bIsInteger)
				return EccentricityDirectionPropertyWidget::eccentricityDirectionString((EccentricityDirectionPropertyWidget::EccentricityDirectionEnum)vInput.toInt());
			else
				return vInput.toString();
		}
		else
		{
			if (bIsInteger)
				return nIntegerValue;
			else if (VariantExtensionPropertySettingManager::isScriptReferenceString(vInput.toString()))
				return vInput.toString();
			else
				return (int)EccentricityDirectionPropertyWidget::eccentricityDirectionEnum(vInput.toString());
		}
	};

	Q_INVOKABLE QWidget* getNewEditorObject()
	{
		return new EccentricityDirectionPropertyWidget();//parent??
	}
};
Q_DECLARE_METATYPE(EccentricityDirectionPropertyType)

class MethodTypePropertyWidget : public PropertySettingBase
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
class MethodTypePropertyType : public QObject
{
	Q_OBJECT
public:
	MethodTypePropertyType(QObject *parent = NULL) : QObject(parent) { ; };
	MethodTypePropertyType(const MethodTypePropertyType& other){ Q_UNUSED(other); }//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~MethodTypePropertyType() { ; };
	Q_INVOKABLE QVariant resolveParameterValueType(const QVariant &vInput, const bool &bToView)
	{
		if (bToView)
			return vInput;
		else
			return vInput;
	};

	Q_INVOKABLE QWidget* getNewEditorObject()
	{
		return new MethodTypePropertyWidget();//parent??
	}
};
Q_DECLARE_METATYPE(MethodTypePropertyType)

static int rotationDirectionTypeId() {return qMetaTypeId<RotationDirectionPropertyType>();}
static int movementDirectionTypeId(){return qMetaTypeId<MovementDirectionPropertyType>();}
static int eccentricityDirectionTypeId(){return qMetaTypeId<EccentricityDirectionPropertyType>();}
static int methodTypeTypeId(){return qMetaTypeId<MethodTypePropertyType>();}

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif//EXPERIMENTPROPERTYSETTINGEXTENSIONS_H

