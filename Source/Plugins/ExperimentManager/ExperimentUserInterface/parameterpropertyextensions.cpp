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

#include "parameterpropertyextensions.h"
#include "experimentparametervisualizer.h"
#include <QFileDialog>
#include <QCheckBox>

QMap<int, QString> RotationDirectionPropertyWidget::mRotationDirection;
QMap<int, RotationDirectionPropertyWidget::RotationDirectionEnum> RotationDirectionPropertyWidget::indexToEnumHash;
QMap<int, QString> MovementDirectionPropertyWidget::mMovementDirection;
QMap<int, MovementDirectionPropertyWidget::MovementDirectionEnum> MovementDirectionPropertyWidget::indexToEnumHash;
QMap<int, QString> EccentricityDirectionPropertyWidget::mEccentricityDirection;
QMap<int, EccentricityDirectionPropertyWidget::EccentricityDirectionEnum> EccentricityDirectionPropertyWidget::indexToEnumHash;
QMap<int, QString> MethodTypePropertyWidget::mapMethodType;
QMap<int, ExperimentStructuresNameSpace::MethodType> MethodTypePropertyWidget::indexToEnumMap;

///////////////////////////////////////////////////////////////////////////////

PropertyWidgetBase::PropertyWidgetBase(QWidget *parent) : parentWidget(parent), QWidget(parent)
{
	layout = NULL;
	sIconSize = NULL;
	pEventFilterWidget = NULL;
	leVariabeleParameter = NULL;
	bIsScriptableParameter = true;//default should be true
	bIsFixedParamValue = true;
	sCurrentFixedValue = "";
	sCurrentVariabeleValue = "{}";

	changeParamValueButton = new QToolButton(parent);
	fixedPixmapIcon = new QPixmap(":/resources/locked.png");
	variabelePixmapIcon = new QPixmap(":/resources/unlocked.png");
	iFixedIcon.addPixmap(*fixedPixmapIcon);
	iVariabeleIcon.addPixmap(*variabelePixmapIcon);
	changeParamValueButton->setIcon(iFixedIcon);
	changeParamValueButton->setCursor(Qt::PointingHandCursor);
	changeParamValueButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	connect(changeParamValueButton, SIGNAL(clicked()), this, SLOT(changeParamValueTypeClicked()));
	
	createNewLayout();
	this->setFocusPolicy(Qt::ClickFocus);//Important!!!
}

PropertyWidgetBase::~PropertyWidgetBase()
{
	if (layout)
	{
		delete layout;
		layout = NULL;
	}
	if(sIconSize)
	{
		delete sIconSize;
		sIconSize = NULL;
	}
	if(leVariabeleParameter)
	{
		delete leVariabeleParameter;
		leVariabeleParameter = NULL;
	}
}

void PropertyWidgetBase::installEventFilterFromWidget(QWidget *pWidget)
{
	pWidget->installEventFilter((PropertyWidgetBase*)this);
	pEventFilterWidget = pWidget;
}

bool PropertyWidgetBase::eventFilter(QObject* object, QEvent* event)
{
	if(((object == pEventFilterWidget) || (object == leVariabeleParameter)) && event->type() == QEvent::FocusOut)//QEvent::MouseClick) 
	{
		// bring up your custom edit
		QPoint p = this->mapFromGlobal(QCursor::pos());
		if((p.y() > this->size().height()) || (p.x() > this->size().width()) || (p.y() < 0) || (p.x() < 0))
			this->deleteLater();
		return false; // lets the event continue to the edit
	}
	return false;
}

void PropertyWidgetBase::createNewLayout()
{
	if(layout)
	{
		delete layout;
		layout = NULL;
	}
	if(getMainLayout())
	{
		if(bIsScriptableParameter)
		{
			layout->addWidget(changeParamValueButton);
			if(sIconSize == NULL)
				sIconSize = new QSize(this->sizeHint().height(),this->sizeHint().height());
			changeParamValueButton->setIconSize(*sIconSize);
		}
	}
}

QHBoxLayout *PropertyWidgetBase::getMainLayout()
{ 
	if(layout == NULL)
	{
		layout = new QHBoxLayout(this);
		layout->setDirection(QHBoxLayout::RightToLeft);
		layout->setContentsMargins(0,0,0,0);
		layout->setMargin(0);
		layout->setSpacing(0);
		setLayout(layout);
	}
	return layout; 
}

void PropertyWidgetBase::setCustomFixedWidgetList(const QList<strcCustomFixedWidget> &lCustFixWidgets)
{
	lCustomFixedWidgets = lCustFixWidgets;
	addCustomFixedWidgetsToLayout();
}

void PropertyWidgetBase::setScriptability(const bool &bIsScriptable) 
{
	if(bIsScriptableParameter != bIsScriptable)
	{
		bIsScriptableParameter = bIsScriptable;
		changeParamValueType(bIsFixedParamValue,true);//force an update
	}
}

void PropertyWidgetBase::setValue(const QString &sValue)
{
	bool bSomethingChanged = false;
	bool bIsScriptRef = VariantExtensionPropertyManager::isScriptReferenceString(sValue);
	if(bIsFixedParamValue != (bIsScriptRef == false))//Is the stored bIsFixedParamValue value incorrect?
	{
		changeParamValueType(bIsScriptRef == false);
		bSomethingChanged = true;
	}

	if(bIsFixedParamValue)
	{
		if((sCurrentFixedValue != sValue) || (bSomethingChanged))
		{
			sCurrentFixedValue = sValue;
			setFixedValue(sValue);
		}
	}
	else
	{
		if((sCurrentVariabeleValue != sValue) || (bSomethingChanged))
		{
			sCurrentVariabeleValue = sValue;
			QString sNewExtractedValue = sCurrentVariabeleValue.mid(1,sCurrentVariabeleValue.length()-2);
			if(leVariabeleParameter)
			{
				if(leVariabeleParameter->text() != sNewExtractedValue)
					leVariabeleParameter->setText(sNewExtractedValue);
			}
		}
	}
}

QString PropertyWidgetBase::getValue()
{
	if(bIsFixedParamValue)
	{
		return sCurrentFixedValue;
	}
	else
	{
		if(sCurrentVariabeleValue.isEmpty())
			sCurrentVariabeleValue = "{}";
		return sCurrentVariabeleValue;
	}
}

void PropertyWidgetBase::setAndEmitEditedValue(const QString &sValue)
{
	setValue(sValue);
	emit PropertyWidgetChanged(sValue);
}

void PropertyWidgetBase::variabeleParameterNameChanged(const QString &sParamName)
{
	if(sParamName.isEmpty())
		return;
	setAndEmitEditedValue("{" + sParamName + "}");
}

void PropertyWidgetBase::changeParamValueTypeClicked()
{
	changeParamValueType(!bIsFixedParamValue);
	setAndEmitEditedValue(getValue());
}

void PropertyWidgetBase::changeParamValueType(const bool &bNewParamValueIsFixed, const bool &bForceUpdate)
{
	if(bForceUpdate == false)
	{
		if(bIsScriptableParameter == false)
		{
			bIsFixedParamValue = true;
			return;
		}
		if(bIsFixedParamValue == bNewParamValueIsFixed)
			return;
	}
	bIsFixedParamValue = bNewParamValueIsFixed;
	if(bIsFixedParamValue)
	{
		changeParamValueButton->setIcon(iFixedIcon);
		if(leVariabeleParameter)
		{
			leVariabeleParameter->removeEventFilter((PropertyWidgetBase*)this);
			delete leVariabeleParameter;
			leVariabeleParameter = NULL;
		}
		createNewLayout();
		addCustomFixedWidgetsToLayout();
	}
	else
	{
		changeParamValueButton->setIcon(iVariabeleIcon);
		removeCustomFixedWidgetsFromLayout();
		createNewLayout();
		leVariabeleParameter = new QLineEdit();
		connect(leVariabeleParameter, SIGNAL(textEdited(QString)), this, SLOT(variabeleParameterNameChanged(QString)));
		leVariabeleParameter->installEventFilter((PropertyWidgetBase*)this);
		QString sNewExtractedValue = sCurrentVariabeleValue.mid(1,sCurrentVariabeleValue.length()-2);
		leVariabeleParameter->setText(sNewExtractedValue);
		layout->addWidget(leVariabeleParameter, 9);		
	}
}

void PropertyWidgetBase::addCustomFixedWidgetsToLayout()
{
	if(lCustomFixedWidgets.isEmpty()==false)
	{
		getMainLayout();
		for (int i=0;i<lCustomFixedWidgets.count();i++)
		{
			layout->addWidget(lCustomFixedWidgets.at(i).pCustomWidget,lCustomFixedWidgets.at(i).nStretch);
			if (QComboBox *cmbBox = qobject_cast<QComboBox *>(lCustomFixedWidgets.at(i).pCustomWidget))
			{
				setValue(cmbBox->currentText());
			}
			lCustomFixedWidgets.at(i).pCustomWidget->setVisible(true);
			lCustomFixedWidgets.at(i).pCustomWidget->setEnabled(true);
		}
	}
}

void PropertyWidgetBase::removeCustomFixedWidgetsFromLayout()
{
	if(lCustomFixedWidgets.isEmpty()==false)
	{
		getMainLayout();
		for (int i=0;i<lCustomFixedWidgets.count();i++)
		{
			layout->removeWidget(lCustomFixedWidgets.at(i).pCustomWidget);
			lCustomFixedWidgets.at(i).pCustomWidget->setVisible(false);
			lCustomFixedWidgets.at(i).pCustomWidget->setEnabled(false);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

ScriptableQVariantPropertyWidget::ScriptableQVariantPropertyWidget(QWidget *parent) : PropertyWidgetBase(parent), derivedQVariantSubWidget(NULL), derivedQVariantSubWidgetProperty(NULL), derivedQVariantSubWidgetType(-1), pDerivedQVariantSubWidgetManager(NULL)
{
	//this->setFocusPolicy(Qt::ClickFocus);//Important!!!
}

ScriptableQVariantPropertyWidget::~ScriptableQVariantPropertyWidget()
{
	//if(derivedQVariantSubWidget)//is destructed in base class?
	//if(parentLayout)//is destructed in base class!
}

void ScriptableQVariantPropertyWidget::setDerivedEditorAndManager(QtVariantPropertyManager *pManager, QWidget *pWidget, const int &nPropertyType, QtProperty *pDerivedProp)
{
	if(pManager)
	{
		pDerivedQVariantSubWidgetManager = pManager;
	}
	if(pWidget)
	{
		derivedQVariantSubWidget = pWidget;
		derivedQVariantSubWidgetType = nPropertyType;
		derivedQVariantSubWidgetProperty = pDerivedProp;
		QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
		strcCustomFixedWidget sTmpCustomFixedWidget;
		sTmpCustomFixedWidget.pCustomWidget = derivedQVariantSubWidget;
		sTmpCustomFixedWidget.nStretch = 9;
		lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
		setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
		installEventFilterFromWidget(derivedQVariantSubWidget);
		if(derivedQVariantSubWidgetType>0)
		{
			if(derivedQVariantSubWidgetType == (QVariant::Type) QtVariantPropertyManager::enumTypeId())
			{
				if (QComboBox *cmbBox = qobject_cast<QComboBox *>(derivedQVariantSubWidget))
					connect(cmbBox, SIGNAL(currentIndexChanged(int)), this, SLOT(derivedEditorSelectionChanged(int)),Qt::UniqueConnection);
			}
		}
	}
	return;
}

void ScriptableQVariantPropertyWidget::derivedEditorSelectionChanged(const int &nIndex)
{
	if(derivedQVariantSubWidgetType>0)
	{
		if(derivedQVariantSubWidgetType == (QVariant::Type) QtVariantPropertyManager::enumTypeId())
		{
			if (QComboBox *cmbBox = qobject_cast<QComboBox *>(derivedQVariantSubWidget))
			{
				setAndEmitEditedValue(cmbBox->itemText(nIndex));
			}
		}
	}
}

void ScriptableQVariantPropertyWidget::setFixedValue(const QString &sValue)
{
	QString sObjectName = derivedQVariantSubWidget->objectName();
	if(derivedQVariantSubWidget)
	{
		if(derivedQVariantSubWidgetType>0)
		{
			if(derivedQVariantSubWidgetType == (QVariant::Type) QtVariantPropertyManager::enumTypeId())
			{
				if (QComboBox *cmbBox = qobject_cast<QComboBox *>(derivedQVariantSubWidget))
				{
					QString sLowerValue = sValue.toLower();
					for(int i=0;i<cmbBox->count();i++)
					{
						if(cmbBox->itemText(i).toLower() == sLowerValue)
						{
							cmbBox->setCurrentIndex(i);
							return;
						}
					}
				}
			}
		}		
	}
}

///////////////////////////////////////////////////////////////////////////////

StringArrayPropertyWidget::StringArrayPropertyWidget(QWidget *parent) : PropertyWidgetBase(parent)
{
	tmpDialog = NULL;
	dialogLayout = NULL;
	tmpTextEdit = NULL;
	buttonBox = NULL;
	cmbStringArrayConf = NULL;
	sCurrentSeperator = EXPERIMENT_PARAMETER_ARRAYSEP_CHAR;//Default value...	
	createEditorComponents();
}

void StringArrayPropertyWidget::createEditorComponents()
{
	destroyEditorComponents();
	cmbStringArrayConf = new QComboBox(getParentWidget());
	//installEventFilterFromWidget(cmbStringArrayConf);//do not use this code here, dialog is shown modal!
	//cmbStringArrayConf->setEditable(true);
	cmbStringArrayConf->setFrame(false);
	cmbStringArrayConf->addItem("");
	cmbStringArrayConf->addItem("<Edit...>");
	cmbStringArrayConf->setCurrentText("");
	connect(cmbStringArrayConf, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = cmbStringArrayConf;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void StringArrayPropertyWidget::destroyEditorComponents()
{
	if(cmbStringArrayConf)
	{
		delete cmbStringArrayConf;
		cmbStringArrayConf = NULL;
	}
}

void StringArrayPropertyWidget::setFixedValue(const QString &sValue)
{
	if(cmbStringArrayConf->count() > 1)
	{
		if(cmbStringArrayConf->itemText(0) != sValue)
			cmbStringArrayConf->setItemText(0,sValue);
	}
}

StringArrayPropertyWidget::~StringArrayPropertyWidget()
{
	if(cmbStringArrayConf)
	{
		delete cmbStringArrayConf;
		cmbStringArrayConf = NULL;
	}
	if(tmpTextEdit)
	{
		delete tmpTextEdit;
		tmpTextEdit = NULL;
	}
	if(buttonBox)
	{
		delete buttonBox;
		buttonBox = NULL;
	}
	if(dialogLayout)
	{
		delete dialogLayout;
		dialogLayout = NULL;
	}
	if(tmpDialog)
	{
		delete tmpDialog;
		tmpDialog = NULL;
	}
}

void StringArrayPropertyWidget::setSeperator(const QString &sSeperator)
{
	sCurrentSeperator = sSeperator;
}

void StringArrayPropertyWidget::currentIndexChangedSlot(int nIndex)
{
	if(nIndex==1)//Edit
	{
		if(tmpDialog == NULL)
		{
			tmpDialog = new QDialog(this);
			dialogLayout = new QVBoxLayout(tmpDialog);
			tmpTextEdit = new QPlainTextEdit(this);
			dialogLayout->addWidget(tmpTextEdit,9);
			buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
			dialogLayout->addWidget(buttonBox);
			connect(buttonBox, SIGNAL(accepted()), this, SLOT(checkAcception()));
			connect(buttonBox, SIGNAL(rejected()), tmpDialog, SLOT(reject()));
		}
		tmpTextEdit->setPlainText(getValue().replace(sCurrentSeperator,"\n"));
		if(tmpDialog->exec() == 1)
		{
			QStringList tmpStrings = tmpTextEdit->toPlainText().split("\n",QString::SkipEmptyParts);
			setAndEmitEditedValue(tmpStrings.join(sCurrentSeperator));
		}			
		cmbStringArrayConf->setCurrentIndex(0);
	}
}

void StringArrayPropertyWidget::checkAcception()
{	
	if(tmpDialog)
	{
		tmpDialog->accept();
	}	
}

/////////////////////////////////////////////////////////////////////////////////

FilePathPropertyWidget::FilePathPropertyWidget(QWidget *parent): PropertyWidgetBase(parent)
{
	browseButton = NULL;
	leFilePath = NULL;
	createEditorComponents();
	//this->setFocusPolicy(Qt::ClickFocus);//Important!!!
}

FilePathPropertyWidget::~FilePathPropertyWidget()
{
	if(browseButton)
	{
		delete browseButton;
		browseButton = NULL;
	}
	if(leFilePath)
	{
		delete leFilePath;
		leFilePath = NULL;
	}
	if(pixmapIcon)
	{
		delete pixmapIcon;
		pixmapIcon = NULL;
	}
}

void FilePathPropertyWidget::createEditorComponents()
{
	destroyEditorComponents();
	browseButton = new QToolButton(getParentWidget());
	pixmapIcon = new QPixmap(":/resources/browse.png");
	browseButton->setIcon(QIcon(*pixmapIcon));
	browseButton->setIconSize(QSize(this->sizeHint().height(),this->sizeHint().height()));
	browseButton->setCursor(Qt::PointingHandCursor);
	browseButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");

	leFilePath = new QLineEdit(getParentWidget());
	installEventFilterFromWidget(leFilePath);
	connect(leFilePath, SIGNAL(textEdited(const QString &)), this, SLOT(currentTextChangedSlot(const QString &)));
	connect(browseButton, SIGNAL(clicked()), this, SLOT(browseForFile()));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = browseButton;
	sTmpCustomFixedWidget.nStretch = 0;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	sTmpCustomFixedWidget.pCustomWidget = leFilePath;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void FilePathPropertyWidget::destroyEditorComponents()
{
	if(browseButton)
	{
		delete browseButton;
		browseButton = NULL;
	}
	if(leFilePath)
	{
		removeEventFilter(leFilePath);
		delete leFilePath;
		leFilePath = NULL;
	}
}

void FilePathPropertyWidget::browseForFile()
{
	QString sFileName = QFileDialog::getOpenFileName(this, tr("Select File"), "", tr("All Files (*.*)"));//tr("Experiment Files (*.exml);;XML Files (*.xml)"));
	if(sFileName.isEmpty()==false)
	{
		setAndEmitEditedValue(sFileName);
	}
	return;
}

void FilePathPropertyWidget::setFixedValue(const QString &sValue)
{
	if(leFilePath->text() != sValue)
		leFilePath->setText(sValue);
}

void FilePathPropertyWidget::currentTextChangedSlot(const QString &sText)
{
	setAndEmitEditedValue(sText);
}

/////////////////////////////////////////////////////////////////////////////////

RotationDirectionPropertyWidget::RotationDirectionPropertyWidget(QWidget *parent) : PropertyWidgetBase(parent) 
{
	cmbSelection = NULL;
	createEditorComponents();
}

RotationDirectionPropertyWidget::~RotationDirectionPropertyWidget()
{
	destroyEditorComponents();
}

void RotationDirectionPropertyWidget::createEditorComponents()
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
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = cmbSelection;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void RotationDirectionPropertyWidget::destroyEditorComponents()
{
	if(cmbSelection)
	{
		delete cmbSelection;
		cmbSelection = NULL;
	}
}

void RotationDirectionPropertyWidget::setFixedValue(const QString &sValue)
{
	cmbSelection->setCurrentText(sValue);
}

QString RotationDirectionPropertyWidget::rotationDirectionString(enum RotationDirectionEnum eValue)
{
	if(mRotationDirection.isEmpty())
	{
		mRotationDirection[RotationDirectionPropertyWidget::ROTATION_DIR_UNDEFINED] = "Undefined";
		mRotationDirection[RotationDirectionPropertyWidget::ROTATION_DIR_CLOCKWISE] = "Clockwise";	
		mRotationDirection[RotationDirectionPropertyWidget::ROTATION_DIR_COUNTERCLOCKWISE] = "CounterClockwise";
	}
	return mRotationDirection[eValue];
}

RotationDirectionPropertyWidget::RotationDirectionEnum RotationDirectionPropertyWidget::rotationDirectionEnum(const QString &sName)
{
	if(QString::compare(mRotationDirection[RotationDirectionPropertyWidget::ROTATION_DIR_CLOCKWISE], sName, Qt::CaseInsensitive) == 0)
		return ROTATION_DIR_CLOCKWISE;
	else if(QString::compare(mRotationDirection[RotationDirectionPropertyWidget::ROTATION_DIR_COUNTERCLOCKWISE], sName, Qt::CaseInsensitive) == 0)
		return ROTATION_DIR_COUNTERCLOCKWISE;
	//else if(QString::compare(mRotationDirection[RotationDirectionPropertyWidget::ROTATION_DIR_UNDEFINED], sName, Qt::CaseInsensitive) == 0)
	//	return ROTATION_DIR_UNDEFINED;
	return ROTATION_DIR_UNDEFINED; 
}

void RotationDirectionPropertyWidget::currentIndexChangedSlot(int nIndex)
{
	emit PropertyWidgetChanged(QString::number((int)indexToEnumHash[nIndex]));
}

////////////////////////////////////////////////////////////////////////////////////////

MovementDirectionPropertyWidget::MovementDirectionPropertyWidget(QWidget *parent) : PropertyWidgetBase(parent) 
{
	cmbSelection = NULL;
	createEditorComponents();
}

MovementDirectionPropertyWidget::~MovementDirectionPropertyWidget()
{
	destroyEditorComponents();
}

void MovementDirectionPropertyWidget::createEditorComponents()
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
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));

	QList<strcCustomFixedWidget> lTmpCustomFixedWidgetList;
	strcCustomFixedWidget sTmpCustomFixedWidget;
	sTmpCustomFixedWidget.pCustomWidget = cmbSelection;
	sTmpCustomFixedWidget.nStretch = 9;
	lTmpCustomFixedWidgetList.append(sTmpCustomFixedWidget);
	setCustomFixedWidgetList(lTmpCustomFixedWidgetList);
}

void MovementDirectionPropertyWidget::destroyEditorComponents()
{
	if(cmbSelection)
	{
		delete cmbSelection;
		cmbSelection = NULL;
	}
}

void MovementDirectionPropertyWidget::setFixedValue(const QString &sValue)
{
	cmbSelection->setCurrentText(sValue);
}

QString MovementDirectionPropertyWidget::movementDirectionString(enum MovementDirectionEnum eValue)
{
	if(mMovementDirection.isEmpty())
	{
		mMovementDirection[MovementDirectionPropertyWidget::MOVEMENT_DIR_UNDEFINED] = "Undefined";
		mMovementDirection[MovementDirectionPropertyWidget::MOVEMENT_DIR_DOWNUP] = "Upwards";	
		mMovementDirection[MovementDirectionPropertyWidget::MOVEMENT_DIR_UPDOWN] = "Downwards";
	}
	return mMovementDirection[eValue];
}

MovementDirectionPropertyWidget::MovementDirectionEnum MovementDirectionPropertyWidget::movementDirectionEnum(const QString &sName)
{
	if(QString::compare(mMovementDirection[MovementDirectionPropertyWidget::MOVEMENT_DIR_DOWNUP], sName, Qt::CaseInsensitive) == 0)
		return MOVEMENT_DIR_DOWNUP;
	else if(QString::compare(mMovementDirection[MovementDirectionPropertyWidget::MOVEMENT_DIR_UPDOWN], sName, Qt::CaseInsensitive) == 0)
		return MOVEMENT_DIR_UPDOWN;
	//else if(QString::compare(mMovementDirection[MovementDirectionPropertyWidget::MOVEMENT_DIR_UNDEFINED], sName, Qt::CaseInsensitive) == 0)
	//	return MOVEMENT_DIR_UNDEFINED;
	return MOVEMENT_DIR_UNDEFINED; 
}

void MovementDirectionPropertyWidget::currentIndexChangedSlot(int nIndex)
{
	emit PropertyWidgetChanged(QString::number((int)indexToEnumHash[nIndex]));
}

////////////////////////////////////////////////////////////////////////////////////////

MethodTypePropertyWidget::MethodTypePropertyWidget(QWidget *parent) : PropertyWidgetBase(parent) 
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
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));

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

EccentricityDirectionPropertyWidget::EccentricityDirectionPropertyWidget(QWidget *parent) : PropertyWidgetBase(parent) 
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
	connect(cmbSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChangedSlot(int)));	

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

////////////////////////////////////////////////////////////////////////////////////////

VariantExtensionPropertyManager::VariantExtensionPropertyManager(ExperimentParameterVisualizer *parentParamVisualizer) : QtVariantPropertyManager((QObject*)parentParamVisualizer), parentParameterVisualizer(parentParamVisualizer)
{
}

VariantExtensionPropertyManager::~VariantExtensionPropertyManager(void)
{
}

bool VariantExtensionPropertyManager::isPropertyTypeSupported(int nPropertyType) const
{
	if(isCustomPropertyType(nPropertyType))
		return true;
	return QtVariantPropertyManager::isPropertyTypeSupported(nPropertyType);
}

bool VariantExtensionPropertyManager::isCustomProperty(const QtProperty *pProperty) const
{
	if(isCustomPropertyType(propertyType(pProperty)))
		return true;
	return false;
}

bool VariantExtensionPropertyManager::isScriptableProperty(const QtProperty *pProperty) const
{
	if(mapVariantExtensionPropertyData.contains(pProperty))
		return mapVariantExtensionPropertyData[pProperty].bIsScriptable;
	return false;
}

bool VariantExtensionPropertyManager::hasScriptReferenceValue(const QtProperty *pProperty) const
{
	if(isScriptableProperty(pProperty))
	{
		return isScriptReferenceString(mapVariantExtensionPropertyData[pProperty].sValue);
	}
	return false;
}

bool VariantExtensionPropertyManager::isScriptReferenceString(const QString &sValue)
{
	if(sValue.startsWith("{") && sValue.endsWith("}"))
		return true;
	return false;
}

bool VariantExtensionPropertyManager::isScriptReferenceValue(const QVariant &vValue) const
{
	if(vValue.type() == QVariant::String)
	{
		return isScriptReferenceString(vValue.toString());
	}
	return false;
}

bool VariantExtensionPropertyManager::isManagedProperty(const QtProperty *pProperty) const
{
	if(mapVariantExtensionPropertyData.contains(pProperty))
	{
		return true;
	}
	return false;
}

bool VariantExtensionPropertyManager::shouldPullMappedData(const QtProperty *pProperty) const
{
	bool _bDoPullMappedData = false;
	bool _bIsManaged = isManagedProperty(pProperty);
	if(_bIsManaged)
	{
		bool _bIsCustom = isCustomProperty(pProperty);
		if(_bIsCustom)
		{
			_bDoPullMappedData = true;
		}
		else
		{
			bool _bHasScriptValue = hasScriptReferenceValue(pProperty);
			if((_bIsCustom) || (_bHasScriptValue))
				_bDoPullMappedData = true;
		}
	}
	return _bDoPullMappedData;
}

bool VariantExtensionPropertyManager::shouldPushMappedData(const QtProperty *pProperty, const QVariant &sNewValue) const
{
	bool _bDoPushMappedData = false;
	bool _bIsManaged = isManagedProperty(pProperty);
	if(_bIsManaged)
	{
		bool _bIsCustom = isCustomProperty(pProperty);
		bool _bIsScriptable = isScriptableProperty(pProperty);
		if(_bIsScriptable)
		{
			if(_bIsCustom)
			{
				_bDoPushMappedData = true;
			}
			else
			{
				bool _bNewDataIsScriptRef = false;
				if(sNewValue.type() == QVariant::String)
				{
					if(VariantExtensionPropertyManager::isScriptReferenceString(sNewValue.toString()))
						_bNewDataIsScriptRef = true;
				}
				if(_bNewDataIsScriptRef)
					_bDoPushMappedData = true;
			}
		}
	}
	return _bDoPushMappedData;
}

bool VariantExtensionPropertyManager::isCustomPropertyType(const int &nPropertyType) const
{
	if( (nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::movementDirectionTypeId()) ||
		(nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::eccentricityDirectionTypeId()) ||
		(nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::rotationDirectionTypeId()) ||
		(nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::filePathTypeId()) ||
		(nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::stringArrayTypeId()) ||
		(nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::methodTypeTypeId()) 	)
		return true;
	return false;
}

QtVariantProperty *VariantExtensionPropertyManager::addVariantExtensionProperty(int propertyType, const bool &bIsScriptable, const QString &name)
{
	QtVariantProperty *tmpVarProp = QtVariantPropertyManager::addProperty(propertyType,name);//call the base class property
	if(tmpVarProp)
	{
		if(mapVariantExtensionPropertyData.contains((QtProperty *)tmpVarProp))
		{
			mapVariantExtensionPropertyData[(QtProperty *)tmpVarProp].bIsScriptable = bIsScriptable;
		}
	}
	return tmpVarProp;
}

QVariant VariantExtensionPropertyManager::value(const QtProperty *property) const
{
	if(shouldPullMappedData(property))
		return mapVariantExtensionPropertyData[property].sValue;
	return QtVariantPropertyManager::value(property);
}

QVariant VariantExtensionPropertyManager::resolveParameterValueType(const QVariant &vInput, const ExperimentParameterTypeName &sType, const bool &bToView)
{
	bool bIsScriptDefined = false;
	if(bToView)
	{
		if(vInput.canConvert<QString>())
		{
			QString tmpString = vInput.toString();
			if(VariantExtensionPropertyManager::isScriptReferenceString(tmpString))// && (tmpString.size() > 2))
				bIsScriptDefined = true;
		}
		if(bIsScriptDefined)
		{
			return vInput;
		}
		else if(sType == Experiment_ParameterType_Boolean)
		{
			if(vInput.toString().contains("true",Qt::CaseInsensitive))
				return "True";
			else if(vInput.toString().contains("false",Qt::CaseInsensitive))
				return "False";
			else
				return NULL;
		}
		else if(sType == Experiment_ParameterType_RotationDirection)
		{
			return RotationDirectionPropertyWidget::rotationDirectionString((RotationDirectionPropertyWidget::RotationDirectionEnum)vInput.toInt());
		}
		else if(sType == Experiment_ParameterType_EccentricityDirection)
		{
			return EccentricityDirectionPropertyWidget::eccentricityDirectionString((EccentricityDirectionPropertyWidget::EccentricityDirectionEnum)vInput.toInt());
		}
		else if(sType == Experiment_ParameterType_MovementDirection)
		{
			return MovementDirectionPropertyWidget::movementDirectionString((MovementDirectionPropertyWidget::MovementDirectionEnum)vInput.toInt());
		}
		else if(sType == Experiment_ParameterType_FilePath)
		{
			return vInput;
		}
		else if(sType == Experiment_ParameterType_MethodType)
		{
			return vInput;
		}
		else
		{
			return vInput;
		}
	}
	else
	{
		if(vInput.canConvert<QString>())
		{
			QString tmpString = vInput.toString();
			if(VariantExtensionPropertyManager::isScriptReferenceString(tmpString))// && (tmpString.size() > 2))
				bIsScriptDefined = true;
		}
		if(bIsScriptDefined)
		{
			return vInput;
		}
		else if(sType == Experiment_ParameterType_RotationDirection)
		{
			return (int)RotationDirectionPropertyWidget::rotationDirectionEnum(vInput.toString());
		}
		else if(sType == Experiment_ParameterType_EccentricityDirection)
		{
			return (int)EccentricityDirectionPropertyWidget::eccentricityDirectionEnum(vInput.toString());
		}
		else if(sType == Experiment_ParameterType_MovementDirection)
		{
			return (int)MovementDirectionPropertyWidget::movementDirectionEnum(vInput.toString());
		}
		else if(sType == Experiment_ParameterType_FilePath)
		{
			return vInput;
		}
		else if(sType == Experiment_ParameterType_MethodType)
		{
			return vInput;
		}
		else
		{
			return vInput;
		}
	}
	return NULL;
}

void VariantExtensionPropertyManager::setValue(QtProperty *property, const QVariant &val)
{
	int nPropertyType = propertyType(property);
	QString sFinalStringValue = val.toString();
	if (shouldPushMappedData(property,val))
	{
		VariantExtensionPropertyData data = mapVariantExtensionPropertyData[property];
		if (data.sValue == val) 
			return;	
		bool bIsInteger = false;
		int nEnumValue = val.toInt(&bIsInteger);
		if(VariantExtensionPropertyManager::isScriptReferenceValue(val))
		{
			if(nPropertyType == QVariant::Color)
			{
				QColor tmpColor(0,0,0,0);//let's reset this one first to (0,0,0,0) just for the view update...
				QtVariantPropertyManager::setValue(property, tmpColor);
				foreach(QtProperty* tmpProperty, property->subProperties())
				{
					if(tmpProperty->isEnabled() == true)
					{
						QString sTmpNameString = tmpProperty->propertyName();
						if((sTmpNameString == "Red") || (sTmpNameString == "Green") || (sTmpNameString == "Blue") || (sTmpNameString == "Alpha"))
						{
							tmpProperty->setEnabled(false);
							tmpProperty->setModified(false);
						}
					}
				}
			}
			data.sValue = val.toString();
		}
		else if((bIsInteger) && (nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::rotationDirectionTypeId()))
		{
			RotationDirectionPropertyWidget::RotationDirectionEnum tmpEnum = (RotationDirectionPropertyWidget::RotationDirectionEnum)val.toInt();
			data.sValue = RotationDirectionPropertyWidget::rotationDirectionString(tmpEnum);
		}
		else if((bIsInteger) && (nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::movementDirectionTypeId()))
		{
			MovementDirectionPropertyWidget::MovementDirectionEnum tmpEnum = (MovementDirectionPropertyWidget::MovementDirectionEnum)val.toInt();
			data.sValue = MovementDirectionPropertyWidget::movementDirectionString(tmpEnum);
		}
		else if((bIsInteger) && (nPropertyType == (QVariant::Type) VariantExtensionPropertyManager::eccentricityDirectionTypeId()))
		{
			EccentricityDirectionPropertyWidget::EccentricityDirectionEnum tmpEnum = (EccentricityDirectionPropertyWidget::EccentricityDirectionEnum)nEnumValue;
			data.sValue = EccentricityDirectionPropertyWidget::eccentricityDirectionString(tmpEnum);
		}
		else
		{
			data.sValue = val.toString();
		}		
		mapVariantExtensionPropertyData[property] = data;
		emit propertyChanged(property);
		emit valueChanged(property, data.sValue);
		return;
	}
	if(isManagedProperty(property))
	{
		VariantExtensionPropertyData data = mapVariantExtensionPropertyData[property];
		if(nPropertyType == (QVariant::Type) QtVariantPropertyManager::enumTypeId())
		{
			if(val.type() == QVariant::String)//This special case we need to perform a type conversion
			{
				if(parentParameterVisualizer)
				{
					QVariant vEnumValue;
					if(parentParameterVisualizer->getEnumeratedParameterPropertyValue(property,val.toString().toLower(),vEnumValue))
					{	
						sFinalStringValue = vEnumValue.toString();
					}
				}
			}
		}
		if(nPropertyType == QVariant::Color)
		{
			foreach(QtProperty* tmpProperty, property->subProperties())
			{
				if(tmpProperty->isEnabled() == false)
				{
					QString sTmpNameString = tmpProperty->propertyName();
					if((sTmpNameString == "Red") || (sTmpNameString == "Green") || (sTmpNameString == "Blue") || (sTmpNameString == "Alpha"))
					{
						tmpProperty->setEnabled(true);
						//tmpProperty->setModified(false);
					}
				}
			}
		}
		data.sValue = sFinalStringValue;		
		mapVariantExtensionPropertyData[property] = data;
		emit propertyChanged(property);
		emit valueChanged(property, data.sValue);
	}
	QtVariantPropertyManager::setValue(property, sFinalStringValue);
}

QString VariantExtensionPropertyManager::valueText(const QtProperty *property) const
{
	if (shouldPullMappedData(property))
		return value(property).toString(); 
	return QtVariantPropertyManager::valueText(property);		
}

void VariantExtensionPropertyManager::initializeProperty(QtProperty *property)
{ 	
	mapVariantExtensionPropertyData[property] = VariantExtensionPropertyData();
	if(isCustomProperty(property))
	{
		return;
	}
	return QtVariantPropertyManager::initializeProperty(property);
}

void VariantExtensionPropertyManager::uninitializeProperty(QtProperty *property)
{
	if(isManagedProperty(property))
		mapVariantExtensionPropertyData.remove(property); 
}

///////////////////////////////////////////////////////////////////////////

void VariantExtensionPropertyFactory::slotEditorDestroyed(QObject *obj)
{
	Q_UNUSED(obj);
	QWidget* pTmpWidget = (QWidget*)obj;
	for (int i=0;i<createdEditors.keys().count();i++)
	{
		if(createdEditors[createdEditors.keys().at(i)].contains(pTmpWidget))
		{
			createdEditors[createdEditors.keys().at(i)].removeAll(pTmpWidget);
		}
	}
	while(editorToProperty.contains(pTmpWidget))
	{
		editorToProperty.remove(pTmpWidget);
	}
}

bool VariantExtensionPropertyFactory::setPropertyValue(QtVariantPropertyManager *manager, const QString &sUniquePropertyIdentifier, const QString &sValue, const bool &bSetModified)
{
	if(manager)
	{
		QSet<QtProperty*> setProperties = manager->properties();
		QSet<QtProperty*>::const_iterator it;
		for (it = setProperties.cbegin(); it != setProperties.cend(); ++it)
		{				
			if((*it)->propertyId() == sUniquePropertyIdentifier)
			{
				manager->setValue((*it),sValue);
				(*it)->setModified(bSetModified);
				return true;
			}			
		}
	}
	return false;
}

QWidget *VariantExtensionPropertyFactory::getEditorWidget(QtVariantPropertyManager *manager, QtVariantProperty *vProperty, const QString &sDerivedPrefixName, QWidget *parent, QString &sReturnUniquePropertyIdentifier, QtVariantProperty *&pDerivedVariantProperty, const QVariant &vValue, const bool &bDoInitWithValue, const bool &bIsScriptable)
{
	sReturnUniquePropertyIdentifier = "";
	pDerivedVariantProperty = NULL;
	if(vProperty)
	{		
		if(manager)
		{
			QtVariantProperty *varProperty = NULL;
			QSet<QtProperty*> setProperties = manager->properties();
			QSet<QtProperty*>::const_iterator it;
			for (it = setProperties.cbegin(); it != setProperties.cend(); ++it)//setProperties.find(vProperty)
			{				
				//QString tmpTest = (*it)->propertyId();
				if((sDerivedPrefixName.isEmpty()))
				{
					if((*it)->propertyId() == vProperty->propertyId())
					{
						varProperty = vProperty;
						break;
					}
				}
				else if((*it)->propertyId().startsWith(sDerivedPrefixName))
				{
					if((*it)->propertyId().endsWith(vProperty->propertyId()))
					{
						varProperty = (QtVariantProperty *)(*it);
						break;
					}
				}
			}
			if(varProperty == NULL)
			{		
				if (VariantExtensionPropertyManager *tmpVariantExtPropMngr = qobject_cast<VariantExtensionPropertyManager *>(manager)) 
				{
					varProperty = tmpVariantExtPropMngr->addVariantExtensionProperty(vProperty->propertyType(),bIsScriptable,vProperty->propertyName());
				}
				else
				{
					varProperty = manager->addProperty(vProperty->propertyType(),vProperty->propertyName()); 
				}
				varProperty->setPropertyId(sDerivedPrefixName + EXPERIMENT_PARAMETER_DERIVED_CHAR + vProperty->propertyId());	
				varProperty->setAttribute(QLatin1String("enumNames"), vProperty->attributeValue(QLatin1String("enumNames")).toStringList());
				varProperty->setAttribute(QLatin1String("minimum"), vProperty->attributeValue(QLatin1String("minimum")));
				varProperty->setAttribute(QLatin1String("maximum"), vProperty->attributeValue(QLatin1String("maximum")));
				varProperty->setToolTip(vProperty->toolTip());
				varProperty->setWhatsThis(vProperty->whatsThis());
				varProperty->setStatusTip(vProperty->statusTip());
				pDerivedVariantProperty = varProperty;
			}
			if(bDoInitWithValue)
			{
				varProperty->setValue(vValue);
				varProperty->setModified(true);
			}
			sReturnUniquePropertyIdentifier = varProperty->propertyId();
			QWidget *tmpWidget = createEditor(manager,varProperty,parent);
			if(tmpWidget)
			{
				if (PropertyWidgetBase *pPropWidgetBase = qobject_cast<PropertyWidgetBase *>(tmpWidget))
				{
					pPropWidgetBase->setScriptability(bIsScriptable);
				}
			}
			return tmpWidget;
		}
	}
	return NULL;
}

QWidget *VariantExtensionPropertyFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent)
{
	bool bResult;
	int nPropertyType = manager->propertyType(property);
	//int nTest = QVariant::String;
	if (nPropertyType == VariantExtensionPropertyManager::rotationDirectionTypeId()) 
	{
		RotationDirectionPropertyWidget *editor = new RotationDirectionPropertyWidget(parent);
		editor->setValue(manager->value(property).toString());
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	else if (nPropertyType == VariantExtensionPropertyManager::movementDirectionTypeId()) 
	{
		MovementDirectionPropertyWidget *editor = new MovementDirectionPropertyWidget(parent);
		editor->setValue(manager->value(property).toString());
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	else if (nPropertyType == VariantExtensionPropertyManager::eccentricityDirectionTypeId()) 
	{
		EccentricityDirectionPropertyWidget *editor = new EccentricityDirectionPropertyWidget(parent);
		editor->setValue(manager->value(property).toString());
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	else if (nPropertyType == VariantExtensionPropertyManager::stringArrayTypeId()) 
	{
		StringArrayPropertyWidget *editor = new StringArrayPropertyWidget(parent);
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		editor->setSeperator(EXPERIMENT_PARAMETER_ARRAYSEP_CHAR);
		editor->setValue(manager->value(property).toString());
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	else if (nPropertyType == VariantExtensionPropertyManager::filePathTypeId()) 
	{
		FilePathPropertyWidget *editor = new FilePathPropertyWidget(parent);
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		//editor->setSeperator(EXPERIMENT_PARAMETER_ARRAYSEP_CHAR);
		editor->setValue(manager->value(property).toString());
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	else if (nPropertyType == VariantExtensionPropertyManager::methodTypeTypeId()) 
	{
		MethodTypePropertyWidget *editor = new MethodTypePropertyWidget(parent);
		editor->setValue(manager->value(property).toString());
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	//else if (nPropertyType == VariantExtensionPropertyManager::scriptable_QVariantTypeId()) {}
	else
	{
		QWidget* tmpDefaultWidget;
		bool bIsScriptable = false;
		if (VariantExtensionPropertyManager *tmpVariantExtPropMngr = qobject_cast<VariantExtensionPropertyManager *>(manager)) 
		{
			bIsScriptable = tmpVariantExtPropMngr->isScriptableProperty(property);
		}
		if(bIsScriptable)
		{
			QWidget* tmpDefaultWidget = QtVariantEditorFactory::createEditor(manager,property,parent);
			ScriptableQVariantPropertyWidget *editor = new ScriptableQVariantPropertyWidget(parent);
			editor->setDerivedEditorAndManager(manager, tmpDefaultWidget, nPropertyType, property);
			if(editor)
			{
				createdEditors[property].append(editor);
				editorToProperty[editor] = property;

				//createdEditors[property].append(tmpDefaultWidget);
				//editorToProperty[tmpDefaultWidget] = property;	

				editor->setValue(manager->value(property).toString());
				bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
				bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
			}
			return editor;
		}
		else
		{
			tmpDefaultWidget = QtVariantEditorFactory::createEditor(manager,property,parent);
			if(tmpDefaultWidget)
			{
				createdEditors[property].append(tmpDefaultWidget);
				editorToProperty[tmpDefaultWidget] = property;		
				//Do not do this, too much triggers... also when initializing --> bool bResult = connect(manager, SIGNAL(propertyChanged(QtProperty*)), this, SLOT(slotNonCustomPropertyChanged(QtProperty*)), Qt::UniqueConnection);
			}
			return tmpDefaultWidget;
		}
	}
}

void VariantExtensionPropertyFactory::slotCustomPropertyChanged(const QString &val)
{
	QObject *sendObj = sender();
	if(sendObj->isWidgetType())
	{
		QWidget *sendWidget = qobject_cast<QWidget *>(sendObj);
		QtProperty *property = editorToProperty[sendWidget];
		if(property)
		{
			this->propertyManager(property)->setValue(property,val);
			//property->setModified(true);
			return;
		}
	}
	return;
}