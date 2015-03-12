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

#include "experimentpropertysettingextensions.h"

QMap<int, QString> RotationDirectionPropertySetting::mRotationDirection;
QMap<int, RotationDirectionPropertySetting::RotationDirectionEnum> RotationDirectionPropertySetting::indexToEnumHash;
QMap<int, QString> MovementDirectionPropertySetting::mMovementDirection;
QMap<int, MovementDirectionPropertySetting::MovementDirectionEnum> MovementDirectionPropertySetting::indexToEnumHash;
QMap<int, QString> EccentricityDirectionPropertyWidget::mEccentricityDirection;
QMap<int, EccentricityDirectionPropertyWidget::EccentricityDirectionEnum> EccentricityDirectionPropertyWidget::indexToEnumHash;
QMap<int, QString> MethodTypePropertyWidget::mapMethodType;
QMap<int, ExperimentStructuresNameSpace::MethodType> MethodTypePropertyWidget::indexToEnumMap;

///////////////////////////////////////////////////////////////////////////////

RotationDirectionPropertySetting::RotationDirectionPropertySetting(QWidget *parent) : PropertySettingBase(parent) 
{
	cmbSelection = NULL;
	createEditorComponents();
}

RotationDirectionPropertySetting::~RotationDirectionPropertySetting()
{
	destroyEditorComponents();
}

void RotationDirectionPropertySetting::createEditorComponents()
{
	destroyEditorComponents();
	cmbSelection = new QComboBox(getParentWidget());
	int nIndex = 0;
	cmbSelection->addItem(QIcon(":/resources/clockwise.png"),rotationDirectionString(ROTATION_DIR_CLOCKWISE), ROTATION_DIR_CLOCKWISE);
	indexToEnumHash[nIndex] = ROTATION_DIR_CLOCKWISE;
	nIndex++;
	cmbSelection->addItem(QIcon(":/resources/counterclockwise.png"),rotationDirectionString(ROTATION_DIR_COUNTERCLOCKWISE), ROTATION_DIR_COUNTERCLOCKWISE);
	indexToEnumHash[nIndex] = ROTATION_DIR_COUNTERCLOCKWISE;
	nIndex++;
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)), Qt::ConnectionType(Qt::UniqueConnection));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = cmbSelection;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void RotationDirectionPropertySetting::destroyEditorComponents()
{
	if(cmbSelection)
	{
		delete cmbSelection;
		cmbSelection = NULL;
	}
}

void RotationDirectionPropertySetting::setFixedValue(const QString &sValue)
{
	bool bIsInteger = false;
	int nIntValue = sValue.toInt(&bIsInteger);
	if (bIsInteger)
		cmbSelection->setCurrentText(rotationDirectionString((RotationDirectionEnum)nIntValue));
	else
		cmbSelection->setCurrentText(sValue);
}

QString RotationDirectionPropertySetting::rotationDirectionString(enum RotationDirectionEnum eValue)
{
	if(mRotationDirection.isEmpty())
	{
		mRotationDirection[RotationDirectionPropertySetting::ROTATION_DIR_UNDEFINED] = "Undefined";
		mRotationDirection[RotationDirectionPropertySetting::ROTATION_DIR_CLOCKWISE] = "Clockwise";	
		mRotationDirection[RotationDirectionPropertySetting::ROTATION_DIR_COUNTERCLOCKWISE] = "CounterClockwise";
	}
	return mRotationDirection[eValue];
}

RotationDirectionPropertySetting::RotationDirectionEnum RotationDirectionPropertySetting::rotationDirectionEnum(const QString &sName)
{
	if(QString::compare(mRotationDirection[RotationDirectionPropertySetting::ROTATION_DIR_CLOCKWISE], sName, Qt::CaseInsensitive) == 0)
		return ROTATION_DIR_CLOCKWISE;
	else if(QString::compare(mRotationDirection[RotationDirectionPropertySetting::ROTATION_DIR_COUNTERCLOCKWISE], sName, Qt::CaseInsensitive) == 0)
		return ROTATION_DIR_COUNTERCLOCKWISE;
	//else if(QString::compare(mRotationDirection[RotationDirectionPropertySetting::ROTATION_DIR_UNDEFINED], sName, Qt::CaseInsensitive) == 0)
	//	return ROTATION_DIR_UNDEFINED;
	return ROTATION_DIR_UNDEFINED; 
}

void RotationDirectionPropertySetting::currentIndexChangedSlot(int nIndex)
{
	emit PropertyWidgetChanged(QString::number((int)indexToEnumHash[nIndex]));
}

////////////////////////////////////////////////////////////////////////////////////////

MovementDirectionPropertySetting::MovementDirectionPropertySetting(QWidget *parent) : PropertySettingBase(parent) 
{
	cmbSelection = NULL;
	createEditorComponents();
}

MovementDirectionPropertySetting::~MovementDirectionPropertySetting()
{
	destroyEditorComponents();
}

void MovementDirectionPropertySetting::createEditorComponents()
{
	destroyEditorComponents();
	cmbSelection = new QComboBox(getParentWidget());
	int nIndex = 0;
	cmbSelection->addItem(QIcon(":/resources/upwards.png"),movementDirectionString(MOVEMENT_DIR_DOWNUP), MOVEMENT_DIR_DOWNUP);
	indexToEnumHash[nIndex] = MOVEMENT_DIR_DOWNUP;
	nIndex++;
	cmbSelection->addItem(QIcon(":/resources/downwards.png"),movementDirectionString(MOVEMENT_DIR_UPDOWN), MOVEMENT_DIR_UPDOWN);
	indexToEnumHash[nIndex] = MOVEMENT_DIR_UPDOWN;
	nIndex++;
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)), Qt::ConnectionType(Qt::UniqueConnection));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = cmbSelection;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void MovementDirectionPropertySetting::destroyEditorComponents()
{
	if(cmbSelection)
	{
		delete cmbSelection;
		cmbSelection = NULL;
	}
}

void MovementDirectionPropertySetting::setFixedValue(const QString &sValue)
{
	bool bIsInteger = false;
	int nIntValue = sValue.toInt(&bIsInteger);
	if (bIsInteger)
		cmbSelection->setCurrentText(movementDirectionString((MovementDirectionEnum)nIntValue));
	else
		cmbSelection->setCurrentText(sValue);
}

QString MovementDirectionPropertySetting::movementDirectionString(enum MovementDirectionEnum eValue)
{
	if(mMovementDirection.isEmpty())
	{
		mMovementDirection[MovementDirectionPropertySetting::MOVEMENT_DIR_UNDEFINED] = "Undefined";
		mMovementDirection[MovementDirectionPropertySetting::MOVEMENT_DIR_DOWNUP] = "Upwards";	
		mMovementDirection[MovementDirectionPropertySetting::MOVEMENT_DIR_UPDOWN] = "Downwards";
	}
	return mMovementDirection[eValue];
}

MovementDirectionPropertySetting::MovementDirectionEnum MovementDirectionPropertySetting::movementDirectionEnum(const QString &sName)
{
	if(QString::compare(mMovementDirection[MovementDirectionPropertySetting::MOVEMENT_DIR_DOWNUP], sName, Qt::CaseInsensitive) == 0)
		return MOVEMENT_DIR_DOWNUP;
	else if(QString::compare(mMovementDirection[MovementDirectionPropertySetting::MOVEMENT_DIR_UPDOWN], sName, Qt::CaseInsensitive) == 0)
		return MOVEMENT_DIR_UPDOWN;
	//else if(QString::compare(mMovementDirection[MovementDirectionPropertySetting::MOVEMENT_DIR_UNDEFINED], sName, Qt::CaseInsensitive) == 0)
	//	return MOVEMENT_DIR_UNDEFINED;
	return MOVEMENT_DIR_UNDEFINED; 
}

void MovementDirectionPropertySetting::currentIndexChangedSlot(int nIndex)
{
	emit PropertyWidgetChanged(QString::number((int)indexToEnumHash[nIndex]));
}

////////////////////////////////////////////////////////////////////////////////////////

MethodTypePropertyWidget::MethodTypePropertyWidget(QWidget *parent) : PropertySettingBase(parent) 
{
	cmbSelection = NULL;
	createEditorComponents();
}

MethodTypePropertyWidget::~MethodTypePropertyWidget()
{
	destroyEditorComponents();
}

void MethodTypePropertyWidget::createEditorComponents()
{
	destroyEditorComponents();
	cmbSelection = new QComboBox(getParentWidget());
	int nIndex = 0;
	cmbSelection->addItem(QIcon(":/resources/signal.png"),cMethodStructure::methodTypeToString((int)ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL), ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL);
	indexToEnumMap[nIndex] =  ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL;
	nIndex++;
	cmbSelection->addItem(QIcon(":/resources/slot.png"),cMethodStructure::methodTypeToString((int)ExperimentStructuresNameSpace::METHOD_TYPE_SLOT), ExperimentStructuresNameSpace::METHOD_TYPE_SLOT);
	indexToEnumMap[nIndex] = ExperimentStructuresNameSpace::METHOD_TYPE_SLOT;
	nIndex++;
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)), Qt::ConnectionType(Qt::UniqueConnection));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = cmbSelection;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void MethodTypePropertyWidget::destroyEditorComponents()
{
	if(cmbSelection)
	{
		delete cmbSelection;
		cmbSelection = NULL;
	}
}

void MethodTypePropertyWidget::setFixedValue(const QString &sValue)
{
	cmbSelection->setCurrentText(sValue);
}

void MethodTypePropertyWidget::currentIndexChangedSlot(int nIndex)
{
	emit PropertyWidgetChanged(cMethodStructure::methodTypeToString((int)indexToEnumMap[nIndex]));
}

////////////////////////////////////////////////////////////////////////////////////////

EccentricityDirectionPropertyWidget::EccentricityDirectionPropertyWidget(QWidget *parent) : PropertySettingBase(parent) 
{
	cmbSelection = NULL;
	createEditorComponents();
}

EccentricityDirectionPropertyWidget::~EccentricityDirectionPropertyWidget()
{
	destroyEditorComponents();
}

void EccentricityDirectionPropertyWidget::createEditorComponents()
{
	destroyEditorComponents();
	cmbSelection = new QComboBox(getParentWidget());
	int nIndex = 0;
	cmbSelection->addItem(QIcon(":/resources/decrease.png"),eccentricityDirectionString(ECCENTRICITY_DIR_DECREASE), ECCENTRICITY_DIR_DECREASE);
	indexToEnumHash[nIndex] = ECCENTRICITY_DIR_DECREASE;
	nIndex++;
	cmbSelection->addItem(QIcon(":/resources/increase.png"),eccentricityDirectionString(ECCENTRICITY_DIR_INCREASE), ECCENTRICITY_DIR_INCREASE);
	indexToEnumHash[nIndex] = ECCENTRICITY_DIR_INCREASE;
	nIndex++;
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)), Qt::ConnectionType(Qt::UniqueConnection));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = cmbSelection;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void EccentricityDirectionPropertyWidget::destroyEditorComponents()
{
	if(cmbSelection)
	{
		delete cmbSelection;
		cmbSelection = NULL;
	}
}

void EccentricityDirectionPropertyWidget::setFixedValue(const QString &sValue)
{
	bool bIsInteger = false;
	int nIntValue = sValue.toInt(&bIsInteger);
	if (bIsInteger)
		cmbSelection->setCurrentText(eccentricityDirectionString((EccentricityDirectionEnum)nIntValue));
	else
		cmbSelection->setCurrentText(sValue);
}

QString EccentricityDirectionPropertyWidget::eccentricityDirectionString(enum EccentricityDirectionEnum eValue)
{
	if(mEccentricityDirection.isEmpty())
	{
		mEccentricityDirection[EccentricityDirectionPropertyWidget::ECCENTRICITY_DIR_UNDEFINED] = "Undefined";
		mEccentricityDirection[EccentricityDirectionPropertyWidget::ECCENTRICITY_DIR_DECREASE] = "Decrease";	
		mEccentricityDirection[EccentricityDirectionPropertyWidget::ECCENTRICITY_DIR_INCREASE] = "Increase";
	}
	return mEccentricityDirection[eValue];
}

EccentricityDirectionPropertyWidget::EccentricityDirectionEnum EccentricityDirectionPropertyWidget::eccentricityDirectionEnum(const QString &sName)
{
	if(QString::compare(mEccentricityDirection[EccentricityDirectionPropertyWidget::ECCENTRICITY_DIR_INCREASE], sName, Qt::CaseInsensitive) == 0)
		return ECCENTRICITY_DIR_INCREASE;
	else if(QString::compare(mEccentricityDirection[EccentricityDirectionPropertyWidget::ECCENTRICITY_DIR_DECREASE], sName, Qt::CaseInsensitive) == 0)
		return ECCENTRICITY_DIR_DECREASE;
	//else if(QString::compare(mEccentricityDirection[EccentricityDirectionPropertyWidget::ECCENTRICITY_DIR_UNDEFINED], sName, Qt::CaseInsensitive) == 0)
	//	return ECCENTRICITY_DIR_UNDEFINED;
	return ECCENTRICITY_DIR_UNDEFINED; 
}

void EccentricityDirectionPropertyWidget::currentIndexChangedSlot(int nIndex)
{
	emit PropertyWidgetChanged(QString::number((int)indexToEnumHash[nIndex]));
}
