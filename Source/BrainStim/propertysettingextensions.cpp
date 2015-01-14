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

#include "propertysettingextensions.h"
#include "propertysettingswidget.h"
#include <QFileDialog>
#include <QCheckBox>
#include <QMetaMethod>

///////////////////////////////////////////////////////////////////////////////

PropertySettingBase::PropertySettingBase(QWidget *parent) : parentWidget(parent), QWidget(parent)
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

PropertySettingBase::~PropertySettingBase()
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

void PropertySettingBase::installEventFilterFromWidget(QWidget *pWidget)
{
	pWidget->installEventFilter((PropertySettingBase*)this);
	pEventFilterWidget = pWidget;
}

bool PropertySettingBase::eventFilter(QObject* object, QEvent* event)
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

void PropertySettingBase::createNewLayout()
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

QHBoxLayout *PropertySettingBase::getMainLayout()
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

void PropertySettingBase::setCustomFixedWidgetList(const QList<strcCustomFixedWidget> &lCustFixWidgets)
{
	lCustomFixedWidgets = lCustFixWidgets;
	addCustomFixedWidgetsToLayout();
}

void PropertySettingBase::setScriptability(const bool &bIsScriptable) 
{
	if(bIsScriptableParameter != bIsScriptable)
	{
		bIsScriptableParameter = bIsScriptable;
		changeParamValueType(bIsFixedParamValue,true);//force an update
	}
}

void PropertySettingBase::setValue(const QString &sValue)
{
	bool bSomethingChanged = false;
	bool bIsScriptRef = VariantExtensionPropertySettingManager::isScriptReferenceString(sValue);
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

QString PropertySettingBase::getValue()
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

void PropertySettingBase::setAndEmitEditedValue(const QString &sValue)
{
	setValue(sValue);
	emit PropertyWidgetChanged(sValue);
}

void PropertySettingBase::variabeleParameterNameChanged(const QString &sParamName)
{
	if(sParamName.isEmpty())
		return;
	setAndEmitEditedValue("{" + sParamName + "}");
}

void PropertySettingBase::changeParamValueTypeClicked()
{
	changeParamValueType(!bIsFixedParamValue);
	setAndEmitEditedValue(getValue());
}

void PropertySettingBase::changeParamValueType(const bool &bNewParamValueIsFixed, const bool &bForceUpdate)
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
			leVariabeleParameter->removeEventFilter((PropertySettingBase*)this);
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
		leVariabeleParameter->installEventFilter((PropertySettingBase*)this);
		QString sNewExtractedValue = sCurrentVariabeleValue.mid(1,sCurrentVariabeleValue.length()-2);
		leVariabeleParameter->setText(sNewExtractedValue);
		layout->addWidget(leVariabeleParameter, 9);		
	}
}

void PropertySettingBase::addCustomFixedWidgetsToLayout()
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

void PropertySettingBase::removeCustomFixedWidgetsFromLayout()
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

ScriptableVariantPropertySetting::ScriptableVariantPropertySetting(QWidget *parent) : PropertySettingBase(parent), derivedQVariantSubWidget(NULL), derivedQVariantSubWidgetProperty(NULL), derivedQVariantSubWidgetType(-1), pDerivedQVariantSubWidgetManager(NULL)
{
	//this->setFocusPolicy(Qt::ClickFocus);//Important!!!
}

ScriptableVariantPropertySetting::~ScriptableVariantPropertySetting()
{
	//if(derivedQVariantSubWidget)//is destructed in base class?
	//if(parentLayout)//is destructed in base class!
}

void ScriptableVariantPropertySetting::setDerivedEditorAndManager(QtVariantPropertyManager *pManager, QWidget *pWidget, const int &nPropertyType, QtProperty *pDerivedProp)
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

void ScriptableVariantPropertySetting::derivedEditorSelectionChanged(const int &nIndex)
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

void ScriptableVariantPropertySetting::setFixedValue(const QString &sValue)
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

StringArrayPropertySetting::StringArrayPropertySetting(QWidget *parent) : PropertySettingBase(parent)
{
	tmpDialog = NULL;
	dialogLayout = NULL;
	tmpTextEdit = NULL;
	buttonBox = NULL;
	cmbStringArrayConf = NULL;
	sCurrentSeperator = EXPERIMENT_PARAMETER_ARRAYSEP_CHAR;//Default value...	
	createEditorComponents();
}

void StringArrayPropertySetting::createEditorComponents()
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

void StringArrayPropertySetting::destroyEditorComponents()
{
	if(cmbStringArrayConf)
	{
		delete cmbStringArrayConf;
		cmbStringArrayConf = NULL;
	}
}

void StringArrayPropertySetting::setFixedValue(const QString &sValue)
{
	if(cmbStringArrayConf->count() > 1)
	{
		if(cmbStringArrayConf->itemText(0) != sValue)
			cmbStringArrayConf->setItemText(0,sValue);
	}
}

StringArrayPropertySetting::~StringArrayPropertySetting()
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

void StringArrayPropertySetting::setSeperator(const QString &sSeperator)
{
	sCurrentSeperator = sSeperator;
}

void StringArrayPropertySetting::currentIndexChangedSlot(int nIndex)
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

void StringArrayPropertySetting::checkAcception()
{	
	if(tmpDialog)
	{
		tmpDialog->accept();
	}	
}

/////////////////////////////////////////////////////////////////////////////////

FilePathPropertySetting::FilePathPropertySetting(QWidget *parent): PropertySettingBase(parent)
{
	browseButton = NULL;
	leFilePath = NULL;
	createEditorComponents();
	//this->setFocusPolicy(Qt::ClickFocus);//Important!!!
}

FilePathPropertySetting::~FilePathPropertySetting()
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

void FilePathPropertySetting::createEditorComponents()
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

void FilePathPropertySetting::destroyEditorComponents()
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

void FilePathPropertySetting::browseForFile()
{
	QString sFileName = QFileDialog::getOpenFileName(this, tr("Select File"), "", tr("All Files (*.*)"));//tr("Experiment Files (*.exml);;XML Files (*.xml)"));
	if(sFileName.isEmpty()==false)
	{
		setAndEmitEditedValue(sFileName);
	}
	return;
}

void FilePathPropertySetting::setFixedValue(const QString &sValue)
{
	if(leFilePath->text() != sValue)
		leFilePath->setText(sValue);
}

void FilePathPropertySetting::currentTextChangedSlot(const QString &sText)
{
	setAndEmitEditedValue(sText);
}

////////////////////////////////////////////////////////////////////////////////////////

VariantExtensionPropertySettingManager::VariantExtensionPropertySettingManager(PropertySettingsWidget *parentPropSettWidget) : QtVariantPropertyManager((QObject*)parentPropSettWidget), parentPropertySettingsWidget(parentPropSettWidget)
{

}

VariantExtensionPropertySettingManager::~VariantExtensionPropertySettingManager(void)
{
}

bool VariantExtensionPropertySettingManager::isPropertyTypeSupported(int nPropertyType) const
{
	if(isCustomPropertyType(nPropertyType))
		return true;
	return QtVariantPropertyManager::isPropertyTypeSupported(nPropertyType);
}

bool VariantExtensionPropertySettingManager::isCustomProperty(const QtProperty *pProperty) const
{
	if(isCustomPropertyType(propertyType(pProperty)))
		return true;
	return false;
}

bool VariantExtensionPropertySettingManager::isScriptableProperty(const QtProperty *pProperty) const
{
	if(mapVariantExtensionPropertyData.contains(pProperty))
		return mapVariantExtensionPropertyData[pProperty].bIsScriptable;
	return false;
}

bool VariantExtensionPropertySettingManager::hasScriptReferenceValue(const QtProperty *pProperty) const
{
	if(isScriptableProperty(pProperty))
	{
		return isScriptReferenceString(mapVariantExtensionPropertyData[pProperty].sValue);
	}
	return false;
}

bool VariantExtensionPropertySettingManager::isScriptReferenceString(const QString &sValue)
{
	if(sValue.startsWith("{") && sValue.endsWith("}"))
		return true;
	return false;
}

bool VariantExtensionPropertySettingManager::isScriptReferenceValue(const QVariant &vValue) const
{
	if(vValue.type() == QVariant::String)
	{
		return isScriptReferenceString(vValue.toString());
	}
	return false;
}

bool VariantExtensionPropertySettingManager::isManagedProperty(const QtProperty *pProperty) const
{
	if(mapVariantExtensionPropertyData.contains(pProperty))
	{
		return true;
	}
	return false;
}

bool VariantExtensionPropertySettingManager::shouldPullMappedData(const QtProperty *pProperty) const
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

bool VariantExtensionPropertySettingManager::shouldPushMappedData(const QtProperty *pProperty, const QVariant &sNewValue) const
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
					if(VariantExtensionPropertySettingManager::isScriptReferenceString(sNewValue.toString()))
						_bNewDataIsScriptRef = true;
				}
				if(_bNewDataIsScriptRef)
					_bDoPushMappedData = true;
			}
		}
	}
	return _bDoPushMappedData;
}

bool VariantExtensionPropertySettingManager::isCustomPropertyType(const int &nPropertyType) const
{
	if ((nPropertyType == (QVariant::Type) VariantExtensionPropertySettingManager::filePathTypeId()) ||
		(nPropertyType == (QVariant::Type) VariantExtensionPropertySettingManager::stringArrayTypeId()))
	{
		return true;
	}
	else if (parentPropertySettingsWidget)
	{
		if (parentPropertySettingsWidget->isRegisteredCustomVariabeleType((QVariant::Type) nPropertyType))
			return true;
	}
	return false;
}

QtVariantProperty *VariantExtensionPropertySettingManager::addVariantExtensionProperty(int propertyType, const bool &bIsScriptable, const QString &name)
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

QVariant VariantExtensionPropertySettingManager::value(const QtProperty *property) const
{
	if(shouldPullMappedData(property))
		return mapVariantExtensionPropertyData[property].sValue;
	return QtVariantPropertyManager::value(property);
}

QVariant VariantExtensionPropertySettingManager::resolveParameterValueType(const QVariant &vInput, const int &nVariantType, const bool &bToView)
{
	bool bIsScriptDefined = false;
	if (nVariantType >= PropertySetting_Type_FirstCustom)
	{
		QObject *tmpObject = MainAppInfo::getCustomPropertySettingObject(nVariantType);
		if (tmpObject)
		{
			QVariant vRetVal;
			QByteArray normalizedSignature = QMetaObject::normalizedSignature(PLUGIN_CUSTOMTYPE_RESOLVEPARAM_METHOD_FULL);
			int methodIndex = tmpObject->metaObject()->indexOfMethod(normalizedSignature);
			QMetaMethod method = tmpObject->metaObject()->method(methodIndex);
			bool bInvokeResult = method.invoke(tmpObject,
				Qt::DirectConnection,
				Q_RETURN_ARG(QVariant, vRetVal),
				Q_ARG(QVariant, vInput),
				Q_ARG(bool, bToView));
			if (bInvokeResult)
				return vRetVal;
		}
		return NULL;
	}

	if(bToView)
	{
		if(vInput.canConvert<QString>())
		{
			QString tmpString = vInput.toString();
			if(VariantExtensionPropertySettingManager::isScriptReferenceString(tmpString))// && (tmpString.size() > 2))
				bIsScriptDefined = true;
		}
		if(bIsScriptDefined)
		{
			return vInput;
		}
		else if (nVariantType == PropertySetting_Type_Boolean)
		{
			if(vInput.toString().contains("true",Qt::CaseInsensitive))
				return "True";
			else if(vInput.toString().contains("false",Qt::CaseInsensitive))
				return "False";
			else
				return NULL;
		}
		else if (nVariantType == PropertySetting_Type_FilePath)
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
			if(VariantExtensionPropertySettingManager::isScriptReferenceString(tmpString))// && (tmpString.size() > 2))
				bIsScriptDefined = true;
		}
		if(bIsScriptDefined)
		{
			return vInput;
		}
		else if (nVariantType == PropertySetting_Type_FilePath)
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

void VariantExtensionPropertySettingManager::setValue(QtProperty *property, const QVariant &val)
{
	int nPropertyType = propertyType(property);
	QString sFinalStringValue = val.toString();
	if (shouldPushMappedData(property,val))
	{
		VariantExtensionPropertyData data = mapVariantExtensionPropertyData[property];
		if (data.sValue == val) 
			return;	
		bool bIsInteger = false;
		val.toInt(&bIsInteger);
		if(VariantExtensionPropertySettingManager::isScriptReferenceValue(val))
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
		else if ((parentPropertySettingsWidget) && (parentPropertySettingsWidget->isRegisteredCustomVariabeleType(((QVariant::Type)nPropertyType))))//or use alternatively (MainAppInfo::getCustomPropertySettingObject((QVariant::Type)nPropertyType))//or use alternatively 
		{
			int nCustMetaType = parentPropertySettingsWidget->getRegisteredCustomVariantMetaType((QVariant::Type)nPropertyType);
			if (nCustMetaType >= 0)
				data.sValue = resolveParameterValueType(val, nCustMetaType, false).toString();
			else
				data.sValue = val.toString();
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
				if (parentPropertySettingsWidget)
				{
					QVariant vEnumValue;
					if (parentPropertySettingsWidget->getEnumeratedParameterPropertyValue(property, val.toString().toLower(), vEnumValue))
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

QString VariantExtensionPropertySettingManager::valueText(const QtProperty *property) const
{
	int nPropertyType = propertyType(property);
	if (shouldPullMappedData(property))
	{
		QVariant vValue = value(property);
		if ((parentPropertySettingsWidget) && (parentPropertySettingsWidget->isRegisteredCustomVariabeleType(((QVariant::Type)nPropertyType))))//or use alternatively (MainAppInfo::getCustomPropertySettingObject((QVariant::Type)nPropertyType))//or use alternatively 
		{
			int nCustMetaType = parentPropertySettingsWidget->getRegisteredCustomVariantMetaType((QVariant::Type)nPropertyType);
			if (nCustMetaType >= 0)
				return resolveParameterValueType(vValue, nCustMetaType, true).toString();
		}
		return vValue.toString();
	}
	return QtVariantPropertyManager::valueText(property);		
}

void VariantExtensionPropertySettingManager::initializeProperty(QtProperty *property)
{ 	
	mapVariantExtensionPropertyData[property] = VariantExtensionPropertyData();
	if(isCustomProperty(property))
	{
		return;
	}
	return QtVariantPropertyManager::initializeProperty(property);
}

void VariantExtensionPropertySettingManager::uninitializeProperty(QtProperty *property)
{
	if(isManagedProperty(property))
		mapVariantExtensionPropertyData.remove(property); 
}

///////////////////////////////////////////////////////////////////////////

void VariantExtensionPropertySettingFactory::slotEditorDestroyed(QObject *obj)
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

bool VariantExtensionPropertySettingFactory::setPropertyValue(QtVariantPropertyManager *manager, const QString &sUniquePropertyIdentifier, const QString &sValue, const bool &bSetModified)
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

QWidget *VariantExtensionPropertySettingFactory::getEditorWidget(QtVariantPropertyManager *manager, QtVariantProperty *vProperty, const QString &sDerivedPrefixName, QWidget *parent, QString &sReturnUniquePropertyIdentifier, QtVariantProperty *&pDerivedVariantProperty, const QVariant &vValue, const bool &bDoInitWithValue, const bool &bIsScriptable)
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
				if (VariantExtensionPropertySettingManager *tmpVariantExtPropMngr = qobject_cast<VariantExtensionPropertySettingManager *>(manager)) 
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
				if (PropertySettingBase *pPropWidgetBase = qobject_cast<PropertySettingBase *>(tmpWidget))
				{
					pPropWidgetBase->setScriptability(bIsScriptable);
				}
			}
			return tmpWidget;
		}
	}
	return NULL;
}

QWidget *VariantExtensionPropertySettingFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent)
{
	bool bResult;
	int nPropertyType = manager->propertyType(property);
	if (nPropertyType == VariantExtensionPropertySettingManager::stringArrayTypeId()) 
	{
		StringArrayPropertySetting *editor = new StringArrayPropertySetting(parent);
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		editor->setSeperator(EXPERIMENT_PARAMETER_ARRAYSEP_CHAR);
		editor->setValue(manager->value(property).toString());
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	else if (nPropertyType == VariantExtensionPropertySettingManager::filePathTypeId()) 
	{
		FilePathPropertySetting *editor = new FilePathPropertySetting(parent);
		createdEditors[property].append(editor);
		editorToProperty[editor] = property;
		bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
		//editor->setSeperator(EXPERIMENT_PARAMETER_ARRAYSEP_CHAR);
		editor->setValue(manager->value(property).toString());
		bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
		return editor;
	}
	else if ((parentPropSettWidget) && (parentPropSettWidget->isRegisteredCustomVariabeleType(((QVariant::Type)nPropertyType))))//or use alternatively (MainAppInfo::getCustomPropertySettingObject((QVariant::Type)nPropertyType))//or use alternatively 
	{
		QString sPropValue = manager->value(property).toString();
		int nCustMetaType = parentPropSettWidget->getRegisteredCustomVariantMetaType((QVariant::Type)nPropertyType);
		if (nCustMetaType >= 0)
		{
			QWidget *editor = MainAppInfo::retrieveCustomPropertySettingEditorWidget(nCustMetaType);// , manager->value(property).toString());
			if (editor)
			{
				editor->setParent(parent);
				PropertySettingBase *customEditor = qobject_cast<PropertySettingBase*>(editor);
				if (customEditor)
				{
					QString sPropValue = manager->value(property).toString();
					customEditor->setValue(sPropValue);
					createdEditors[property].append(editor);
					editorToProperty[editor] = property;
					bResult = connect(editor, SIGNAL(PropertyWidgetChanged(const QString&)), this, SLOT(slotCustomPropertyChanged(const QString &)), Qt::UniqueConnection);
					bResult = connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)), Qt::UniqueConnection);
					return editor;
				}
			}
		}
		return NULL;
	}
	//else if (nPropertyType == VariantExtensionPropertySettingManager::scriptable_QVariantTypeId()) {}
	else
	{
		QWidget* tmpDefaultWidget;
		bool bIsScriptable = false;
		if (VariantExtensionPropertySettingManager *tmpVariantExtPropMngr = qobject_cast<VariantExtensionPropertySettingManager *>(manager)) 
		{
			bIsScriptable = tmpVariantExtPropMngr->isScriptableProperty(property);
		}
		if(bIsScriptable)
		{
			QWidget* tmpDefaultWidget = QtVariantEditorFactory::createEditor(manager,property,parent);
			ScriptableVariantPropertySetting *editor = new ScriptableVariantPropertySetting(parent);
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

void VariantExtensionPropertySettingFactory::slotCustomPropertyChanged(const QString &val)
{
	QObject *sendObj = sender();
	if(sendObj->isWidgetType())
	{
		QWidget *sendWidget = qobject_cast<QWidget *>(sendObj);
		QtProperty *property = editorToProperty[sendWidget];
		if(property)
		{
			this->propertyManager(property)->setValue(property, val);
			//property->setModified(true);
			return;
		}
	}
	return;
}