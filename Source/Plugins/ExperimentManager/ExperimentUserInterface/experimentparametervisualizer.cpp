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

#include "experimentparametervisualizer.h"
#include <QDebug>
#include "ui_ExperimentParameterVisualizer.h"

ExperimentParameterVisualizer::ExperimentParameterVisualizer(QWidget *parent) : QWidget(parent)
{
	propertyEditor = NULL;
	mainLayout = NULL;
	groupManager = NULL;
	lVariantPropertyManager = NULL;
	variantExtensionFactory = NULL;
	bAutoDepencyParsing = false;
	bPropertyEditSignaling = false;

	ui = new Ui::ExperimentParameterVisualizer();
	ui->setupUi(this);

	mainLayout = new QVBoxLayout(this);
	this->setLayout(mainLayout);
	propertyEditor = new QtTreePropertyBrowser();
	mainLayout->addWidget(propertyEditor);
	//connect(this, SIGNAL(editFinished(const QString&, const QString&)), this, SLOT(itemEditedSignalHandler(const QString&, const QString&)), Qt::UniqueConnection);
}

ExperimentParameterVisualizer::ExperimentParameterVisualizer(const ExperimentParameterVisualizer& other)
{
	Q_UNUSED(other);
};

ExperimentParameterVisualizer::~ExperimentParameterVisualizer()
{
	lParameterPropertyNamedHash.clear();
	lVariantPropertyDefinitionHash.clear();
	if(lVariantPropertyManager)
	{
		delete lVariantPropertyManager;
		lVariantPropertyManager = NULL;
	}
	if(variantExtensionFactory == NULL)
	{
		delete variantExtensionFactory;
		variantExtensionFactory = NULL;
	}
	deleteSubGroupProperties(&lGroupPropertyCollection.propItem);
	if(groupManager)
		delete groupManager;
	if(propertyEditor)
	{
		delete propertyEditor;
		propertyEditor = NULL;
	}
	if(mainLayout)
	{
		delete mainLayout;
		mainLayout = NULL;
	}
	delete ui;
	emit destroyed(this);
}

void ExperimentParameterVisualizer::deleteSubGroupProperties(QList<propertyContainerItem> *pRootGroupPropertyItem)
{
	if(pRootGroupPropertyItem == NULL) 
		return;
	if(pRootGroupPropertyItem->isEmpty() == false)
	{
		for (QList<propertyContainerItem>::iterator it=pRootGroupPropertyItem->begin();it!=pRootGroupPropertyItem->end();++it)
		{
			if(it->pSubItems)
			{
				deleteSubGroupProperties(it->pSubItems);
				it->pSubItems->clear();
			}
			delete it->lGroupProperty;
			delete it->pSubItems;
		}
		pRootGroupPropertyItem->clear();
		return;
	}
}

bool ExperimentParameterVisualizer::addPropertyToSubGroup(const QString &sPropertyGroupNames, QtVariantProperty *item1, QList<propertyContainerItem> *pRootGroupPropertyItemList, QString &sSandPath)
{	
	if(groupManager == NULL)
		groupManager = new QtGroupPropertyManager(this);
	if(sPropertyGroupNames.isEmpty())
		return false;
	if(pRootGroupPropertyItemList == NULL)
		return false;
	
	QStringList tmpPropGroups = sPropertyGroupNames.split(EXPERIMENT_LISTSEP_CHAR, QString::SkipEmptyParts);
	propertyContainerItem tmpPropertyContainerItem;
	
	if(pRootGroupPropertyItemList->isEmpty() == false)
	{
		for (int j=0;j<pRootGroupPropertyItemList->count();j++)
		{
			if(pRootGroupPropertyItemList->at(j).lGroupProperty->propertyId() == sSandPath + tmpPropGroups.at(0))//Case sensitive!
			{
				if(tmpPropGroups.count() == 1)
				{
					pRootGroupPropertyItemList->at(j).lGroupProperty->addSubProperty(item1);
					return true;
				}
				else
				{
					sSandPath = sSandPath + tmpPropGroups.first() + EXPERIMENT_PARAM_GROUPSEP_CHAR;
					tmpPropGroups.removeFirst();
					if(pRootGroupPropertyItemList->at(j).pSubItems == NULL)
						(*pRootGroupPropertyItemList)[j].pSubItems = new QList<propertyContainerItem>;
					
					bool bGroupIsPresent = false;
					for (QSet<QtProperty*>::const_iterator it=groupManager->properties().cbegin();it!=groupManager->properties().cend();++it)
					{
						if((*it)->propertyId() == sSandPath + tmpPropGroups.at(0))//Case sensitive!
						{
							bGroupIsPresent = true;
							break;
						}
					}
					if(bGroupIsPresent == false)
					{
						QtProperty *tmpProperty = groupManager->addProperty(tmpPropGroups.at(0));
						tmpProperty->setPropertyId(sSandPath + tmpPropGroups.at(0));
						pRootGroupPropertyItemList->at(j).lGroupProperty->addSubProperty(tmpProperty);
						tmpPropertyContainerItem.pSubItems = NULL;
						tmpPropertyContainerItem.lGroupProperty = tmpProperty;
						pRootGroupPropertyItemList->at(j).pSubItems->append(tmpPropertyContainerItem);
					}
					return addPropertyToSubGroup(tmpPropGroups.join(EXPERIMENT_LISTSEP_CHAR), item1, pRootGroupPropertyItemList->at(j).pSubItems,sSandPath);
				}
			}
		}
	}	
	tmpPropertyContainerItem.pSubItems = NULL;
	tmpPropertyContainerItem.lGroupProperty = groupManager->addProperty(sPropertyGroupNames);
	tmpPropertyContainerItem.lGroupProperty->setPropertyId(sSandPath + sPropertyGroupNames);
	pRootGroupPropertyItemList->append(tmpPropertyContainerItem);
	int nPropertyGroupIndex = pRootGroupPropertyItemList->count() - 1;	
	pRootGroupPropertyItemList->at(nPropertyGroupIndex).lGroupProperty->addSubProperty(item1);
	propertyEditor->addProperty(pRootGroupPropertyItemList->at(nPropertyGroupIndex).lGroupProperty);
	return true;
}

bool ExperimentParameterVisualizer::addGroupProperties(const QList<ExperimentGroupDefinitionStrc> *expParamDef)
{
	if(expParamDef == NULL)
		return false;
	if(expParamDef->isEmpty())
		return false;
	if(lVariantPropertyManager == NULL)
		lVariantPropertyManager = new VariantExtensionPropertyManager(this);
	if(groupManager == NULL)
		groupManager = new QtGroupPropertyManager(this);	

	int nPropertyGroupIndex;
	QList<propertyContainerItem> *pCurrentRootGroupPropertyItemList = NULL;
	QString sSandPath;
	QStringList sCurrentRelativePathItems;	
	QtProperty *parentProperty;
	bool bFirstLoop;
	bool bItemAlreadyExists;

	for (QList<ExperimentGroupDefinitionStrc>::const_iterator it=expParamDef->cbegin();it!=expParamDef->cend();++it)
	{
		pCurrentRootGroupPropertyItemList = &lGroupPropertyCollection.propItem;
		bFirstLoop = true;
		bItemAlreadyExists;
		parentProperty = NULL;
		if(it->sGroupPath.isEmpty() == false)
		{
			sSandPath = "";
			sCurrentRelativePathItems = it->sGroupPath.split(EXPERIMENT_LISTSEP_CHAR,QString::SkipEmptyParts);
			if(sCurrentRelativePathItems.isEmpty())
				continue;
			while(sCurrentRelativePathItems.isEmpty() == false)
			{
				//Does it already exist in the current level?
				bItemAlreadyExists = false;
				for (QList<propertyContainerItem>::iterator itProp=pCurrentRootGroupPropertyItemList->begin();itProp!=pCurrentRootGroupPropertyItemList->end();++itProp)
				{
					QString sPropID = itProp->lGroupProperty->propertyId();
					if(sPropID.startsWith(sSandPath + sCurrentRelativePathItems.at(0)))
					{
						bItemAlreadyExists = true;
						//Make sure that a sub-group list is available
						if(itProp->pSubItems == NULL)
							itProp->pSubItems = new QList<propertyContainerItem>;
						//Set Parent and Increment the current property group pointer
						parentProperty = itProp->lGroupProperty;
						pCurrentRootGroupPropertyItemList = itProp->pSubItems; 
						break;
					}
				}
				if(bItemAlreadyExists == false)
				{
					propertyContainerItem tmpPropertyContainerItem;
					tmpPropertyContainerItem.pSubItems = NULL;
					tmpPropertyContainerItem.lGroupProperty = groupManager->addProperty(sCurrentRelativePathItems.at(0));
					tmpPropertyContainerItem.lGroupProperty->setPropertyId(sSandPath + sCurrentRelativePathItems.at(0));
					tmpPropertyContainerItem.lGroupProperty->setEnabled(it->bEnabled);

					//Append this newly created group property
					pCurrentRootGroupPropertyItemList->append(tmpPropertyContainerItem);		
					nPropertyGroupIndex = pCurrentRootGroupPropertyItemList->count() - 1;
					QtProperty *tmpProperty = pCurrentRootGroupPropertyItemList->at(nPropertyGroupIndex).lGroupProperty;

					if(bFirstLoop)
						propertyEditor->addProperty(tmpProperty);
					else
						parentProperty->addSubProperty(tmpProperty);

					if(sCurrentRelativePathItems.count() > 1)//Are there more sub-group's to check/add?
					{
						//Make sure that a sub-group list is available
						if(pCurrentRootGroupPropertyItemList->at(nPropertyGroupIndex).pSubItems == NULL)
							(*pCurrentRootGroupPropertyItemList)[nPropertyGroupIndex].pSubItems = new QList<propertyContainerItem>;
						//Set Parent and Increment the current property group pointer
						parentProperty = pCurrentRootGroupPropertyItemList->at(nPropertyGroupIndex).lGroupProperty;
						pCurrentRootGroupPropertyItemList = pCurrentRootGroupPropertyItemList->at(nPropertyGroupIndex).pSubItems;
					}
					else
					{
						//Configure Dependencies?
						if(it->Dependencies.isEmpty() == false)
						{
							for (QList<ExperimentParameterDefinitionDependencyStrc>::const_iterator itDep=it->Dependencies.cbegin();itDep!=it->Dependencies.cend();++itDep)
							{
								addDependency((QtVariantProperty *)tmpProperty,(*itDep));
							}
						}
					}
				}
				sSandPath = sSandPath + sCurrentRelativePathItems.takeFirst() + EXPERIMENT_LISTSEP_CHAR;
				bFirstLoop = false;
			}
		}		
	}
	return true;
}

bool ExperimentParameterVisualizer::getEnumeratedParameterPropertyValue(QtProperty *pProperty, const QString &sEnumString, QVariant &vEnumValue)
{
	const ExperimentParameterDefinitionStrc *tmpExpParamStruct = lVariantPropertyDefinitionHash[pProperty];
	if(tmpExpParamStruct)
	{
		QString sSearchVal = QString(tmpExpParamStruct->sName + EXPPARAMVIS_ENUM_SPEC_DIVIDER + sEnumString).toLower();
		return getEnumeratedParameterPropertyValue(sSearchVal,vEnumValue);
	}
	return false;
}

bool ExperimentParameterVisualizer::getEnumeratedParameterPropertyValue(const QString &sFullEnumValueName, QVariant &vEnumValue)
{	
	if(lEnumeratedParameterPropertyValuesHash.contains(sFullEnumValueName))
	{
		enumValueStruct tmpEnumValueStruct = lEnumeratedParameterPropertyValuesHash.value(sFullEnumValueName);
		if(tmpEnumValueStruct.vTranslationValue == QVariant::Invalid)
			vEnumValue = lEnumeratedParameterPropertyValuesHash.value(sFullEnumValueName).nEnumControlIndex;
		else
			vEnumValue = lEnumeratedParameterPropertyValuesHash.value(sFullEnumValueName).vTranslationValue;
		return true;
	}
	return false;
}

bool ExperimentParameterVisualizer::resetParameterModifiedFlags(const bool &bOnlyNonDerivedParameters)
{	
	foreach (propertyParameterValueDef tmpPropParamValDef,lParameterPropertyNamedHash)
	{
		if((bOnlyNonDerivedParameters && (tmpPropParamValDef.bIsDerived == false)) || (bOnlyNonDerivedParameters==false))
		{
			tmpPropParamValDef.vProperty->setModified(false);
		}		
	}
	return true;
}

bool ExperimentParameterVisualizer::checkIfParameterExists(const QString &sName)
{
	QList<propertyParameterValueDef> tmpParamValueDefs;
	tmpParamValueDefs = lParameterPropertyNamedHash.values(sName.toLower());
	if(tmpParamValueDefs.isEmpty())
		return false;
	return true;
}

bool ExperimentParameterVisualizer::setAllowedParameterValues(const QString &sName, const QStringList &lAllowedValues, const QVariantList &vTranslatedValues)
{
	bool bResult = false;
	QList<propertyParameterValueDef> tmpParamValueDefs;
	tmpParamValueDefs = lParameterPropertyNamedHash.values(sName.toLower());
	if(checkIfParameterExists(sName)==false)
		return false;
	foreach(propertyParameterValueDef tmpParamValueDef, tmpParamValueDefs)
	{
		if(tmpParamValueDef.vProperty != NULL)
		{
			if(lVariantPropertyDefinitionHash.contains(tmpParamValueDef.vProperty))
			{
				lVariantPropertyDefinitionHash[tmpParamValueDef.vProperty]->Restriction.lAllowedValues = lAllowedValues;
			}
			for(int nTempEnumValue = 0;nTempEnumValue<lAllowedValues.count();nTempEnumValue++)
			{
				enumValueStruct tmpEnumValueStruct;
				tmpEnumValueStruct.nEnumControlIndex = nTempEnumValue;
				if(vTranslatedValues.count() > nTempEnumValue)
				{
					tmpEnumValueStruct.vTranslationValue = vTranslatedValues.at(nTempEnumValue);
					lEnumeratedParameterPropertyValuesHash.insert(QString(sName + EXPPARAMVIS_ENUM_SPEC_DIVIDER + QString::number(nTempEnumValue)).toLower(),tmpEnumValueStruct);
				}
				else
				{
					lEnumeratedParameterPropertyValuesHash.insert(QString(sName + EXPPARAMVIS_ENUM_SPEC_DIVIDER + lAllowedValues.at(nTempEnumValue)).toLower(),tmpEnumValueStruct);
				}
			}
			//if(lParameterPropertyNamedHash.contains(sName.toLower()))
			//{
			//	lParameterPropertyNamedHash[sName.toLower()].vType = (QVariant::Type) QtVariantPropertyManager::enumStringsTypeId();
			//}
			//if(lVariantPropertyManager->properties().begin()->set get setProperty(setpropertyType())
			//varProperty = manager->addProperty(vProperty->propertyType(),vProperty->propertyName());
			tmpParamValueDef.vProperty->setAttribute(QLatin1String("enumNames"), lAllowedValues);
			bResult = true;
		}
	}
	return bResult;
}

bool ExperimentParameterVisualizer::setParameter(const QString &sName, const QString &sValue, const bool &bSetModified, const bool &bIsInitializing)
{
	QList<propertyParameterValueDef> tmpParamValueDefs;
	tmpParamValueDefs = lParameterPropertyNamedHash.values(sName.toLower());
	if(checkIfParameterExists(sName)==false)
		return false;
	foreach(propertyParameterValueDef tmpParamValueDef, tmpParamValueDefs)
	{
		if(tmpParamValueDef.vProperty != NULL)
		{
			bool bFailed = false;
			bool bTempPropertyEditSignalingValue = bPropertyEditSignaling;
			if(bTempPropertyEditSignalingValue && bIsInitializing)
				configurePropertyEditSignaling(false);

			if(VariantExtensionPropertyManager::isScriptReferenceString(sValue))
			{
				tmpParamValueDef.vProperty->setValue(sValue);
			}
			else if(tmpParamValueDef.vType == (QVariant::Type) QtVariantPropertyManager::enumTypeId())
			{
				QString tmpString = QString(sName + EXPPARAMVIS_ENUM_SPEC_DIVIDER + sValue).toLower();
				QVariant vEnumValue;
				if(getEnumeratedParameterPropertyValue(tmpString,vEnumValue))
				{
					tmpParamValueDef.vProperty->setValue(vEnumValue);
				}
				else
				{
					//Enum not registered...
					qDebug() << __FUNCTION__ << "Enumerated value(" << tmpString << ") not registered.";
					bFailed = true;
				}
			}
			else if(tmpParamValueDef.vType == QVariant::Color)
			{
				tmpParamValueDef.vProperty->setValue(QColor(sValue));
			}
			else if(tmpParamValueDef.vType == QVariant::Bool)
			{
				if(sValue.toLower() == "true")
				{
					tmpParamValueDef.vProperty->setValue(true);
				}
				else if(sValue.toLower() == "false")
				{
					tmpParamValueDef.vProperty->setValue(false);
				}
				else
				{
					qDebug() << __FUNCTION__ << "Wrong defined boolean value(" << sValue << ").";
					bFailed = true;
				}
			}
			else if(tmpParamValueDef.vType == QVariant::Int)
			{
				tmpParamValueDef.vProperty->setValue(sValue.toInt());
			}
			else if(tmpParamValueDef.vType == QVariant::String)
			{
				tmpParamValueDef.vProperty->setValue(sValue);
			}
			else if(tmpParamValueDef.vType == QVariant::Double)
			{
				tmpParamValueDef.vProperty->setValue(sValue.toDouble());
			}
			else if(tmpParamValueDef.vType == (QVariant::Type) VariantExtensionPropertyManager::stringArrayTypeId())
			{
				tmpParamValueDef.vProperty->setValue(sValue);
			}
			else if(tmpParamValueDef.vType == (QVariant::Type) VariantExtensionPropertyManager::rotationDirectionTypeId())
			{
				tmpParamValueDef.vProperty->setValue(sValue.toInt());
			}
			else if(tmpParamValueDef.vType == (QVariant::Type) VariantExtensionPropertyManager::eccentricityDirectionTypeId())
			{
				tmpParamValueDef.vProperty->setValue(sValue);
			}
			else if(tmpParamValueDef.vType == (QVariant::Type) VariantExtensionPropertyManager::movementDirectionTypeId())
			{
				tmpParamValueDef.vProperty->setValue(sValue);
			}
			else if(tmpParamValueDef.vType == (QVariant::Type) VariantExtensionPropertyManager::filePathTypeId())
			{
				tmpParamValueDef.vProperty->setValue(sValue);
			}
			else if(tmpParamValueDef.vType == (QVariant::Type) VariantExtensionPropertyManager::methodTypeTypeId())
			{
				tmpParamValueDef.vProperty->setValue(sValue);
			}
			//else if(tmpParamValueDef.vType == (QVariant::Type) VariantExtensionPropertyManager::scriptable_QVariantTypeId())
			//{
			//	tmpParamValueDef.vProperty->setValue(sValue);
			//}
			else
			{
				//Unknown Type..
				qDebug() << __FUNCTION__ << "Unknown Type(" << tmpParamValueDef.vType << ").";
				bFailed = true;
			}
			if(bSetModified && (bFailed == false))
				tmpParamValueDef.vProperty->setModified(true);
			if(bTempPropertyEditSignalingValue && bIsInitializing)
				configurePropertyEditSignaling(true);
			if(bFailed)
				return false;
		}
		else
		{
			return false;
		}
	}
	return true;
}

QWidget *ExperimentParameterVisualizer::getParameterEditWidget(const QString &sName, const QString &sDerivedPrefixName, QString &sReturnUniquePropertyIdentifier, const QVariant &vValue, const bool &bDoInitWithValue, const bool &bIsScriptable)
{
	if(variantExtensionFactory)
	{
		QtVariantProperty* tmpVariantProperty = NULL;
		//QVariant::Type vType = (QVariant::Type)lParameterPropertyNamedHash[sName].vProperty->propertyType();
		QWidget* tmpWidget = variantExtensionFactory->getEditorWidget(lVariantPropertyManager,lParameterPropertyNamedHash[sName].vProperty,sDerivedPrefixName,this, sReturnUniquePropertyIdentifier, tmpVariantProperty, vValue, bDoInitWithValue, bIsScriptable);
		if(tmpWidget)
		{
			if(tmpVariantProperty)//new derived variant property?
			{
				registerDerivedParameterProperty(lParameterPropertyNamedHash[sName], tmpVariantProperty, sReturnUniquePropertyIdentifier);
			}			
			return tmpWidget;
		}
	}
	return NULL;
}

bool ExperimentParameterVisualizer::registerDerivedParameterProperty(const propertyParameterValueDef &baseVPropertyDef, QtVariantProperty *derivedProperty, QString &sUniqueDerivedPropertyIdentifier)
{
	if(derivedProperty == NULL)
		return false;
		
	propertyParameterValueDef tmpParamValueDef;
	tmpParamValueDef.vType = baseVPropertyDef.vType;
	tmpParamValueDef.vProperty = derivedProperty;
	tmpParamValueDef.bIsDerived = true;
	lParameterPropertyNamedHash.insertMulti(sUniqueDerivedPropertyIdentifier,tmpParamValueDef);
	ExperimentParameterDefinitionStrc *tmpExpParStruct = lVariantPropertyDefinitionHash[baseVPropertyDef.vProperty];
	lVariantPropertyDefinitionHash.insertMulti(derivedProperty,tmpExpParStruct);
	
	if(tmpExpParStruct->Dependencies.isEmpty() == false)
	{
		for (QList<ExperimentParameterDefinitionDependencyStrc>::const_iterator it=tmpExpParStruct->Dependencies.cbegin();it!=tmpExpParStruct->Dependencies.cend();++it)
		{
			addDependency(derivedProperty,(*it));
		}
	}
	//if(bAutoDepencyParsing == false)
	//	return true;
	return parseDependencies();
	
	//return true;
}

bool ExperimentParameterVisualizer::setWidgetParameter(const QString &sUniquePropertyIdentifier, const QString &sValue, const bool &bSetModified)
{
	if(variantExtensionFactory)
	{
		return variantExtensionFactory->setPropertyValue(lVariantPropertyManager, sUniquePropertyIdentifier, sValue, bSetModified);
	}
	return false;
}

bool ExperimentParameterVisualizer::configurePropertyEditSignaling(const bool &bEnable)
{
	bool bResult = false;
	if(lVariantPropertyManager)
	{
		if(bEnable && (bPropertyEditSignaling==false))
		{
			bResult = connect(lVariantPropertyManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)), Qt::UniqueConnection);
			bPropertyEditSignaling = true;
		}
		else if((bEnable==false) && bPropertyEditSignaling)
		{
			bResult = disconnect(lVariantPropertyManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(propertyValueChanged(QtProperty *, const QVariant &)));
			bPropertyEditSignaling = false;
		}
		return true;
	}
	return false;
}

bool ExperimentParameterVisualizer::removeParameterProperty(const QString &sUniquePropertyIdentifier)
{
	QtProperty *tmpProperty = lVariantPropertyManager->qtProperty(sUniquePropertyIdentifier);
	if(tmpProperty)
	{
		propertyEditor->removeProperty(tmpProperty);
		if(lVariantPropertyDefinitionHash.contains(tmpProperty))
		{
			lParameterPropertyNamedHash.remove(lVariantPropertyDefinitionHash[tmpProperty]->sName.toLower());
			lVariantPropertyDefinitionHash.remove(tmpProperty);
		}
		delete tmpProperty;
		tmpProperty = NULL;
		return true;
	}
	return false;
}

bool ExperimentParameterVisualizer::addParameterProperty(const ExperimentParameterDefinitionStrc *expParamDef, const QVariant &vValue)
{
	if(expParamDef == NULL)
		return false;
	if(expParamDef->bEnabled == false)
		return true;

	bool bDoMinMax = false;
	bool bDoEnumeratedList = false;

	if(lVariantPropertyManager == NULL)
		lVariantPropertyManager = new VariantExtensionPropertyManager(this);		

	QVariant::Type varType;
	if(expParamDef->eType == Experiment_ParameterType_Color)
	{
		varType = QVariant::Color;
	}
	else if(expParamDef->eType == Experiment_ParameterType_Boolean)
	{
		varType = QVariant::Bool;		
	}
	else if(expParamDef->eType == Experiment_ParameterType_Integer)
	{
		varType = QVariant::Int;
		bDoMinMax = true;
	}
	else if(expParamDef->eType == Experiment_ParameterType_RotationDirection)
	{
		varType = (QVariant::Type) VariantExtensionPropertyManager::rotationDirectionTypeId();
	}
	else if(expParamDef->eType == Experiment_ParameterType_EccentricityDirection)
	{
		varType = (QVariant::Type) VariantExtensionPropertyManager::eccentricityDirectionTypeId();
	}
	else if(expParamDef->eType == Experiment_ParameterType_MovementDirection)
	{
		varType = (QVariant::Type) VariantExtensionPropertyManager::movementDirectionTypeId();
	}	
	else if(expParamDef->eType == Experiment_ParameterType_String)
	{		
		if(expParamDef->Restriction.lAllowedValues.isEmpty() == false)
		{
			varType = (QVariant::Type) QtVariantPropertyManager::enumTypeId();
			bDoEnumeratedList = true;
		}
		else
		{
			varType = QVariant::String;
		}
	}
	else if(expParamDef->eType == Experiment_ParameterType_DynamicStringEnum)
	{		
		varType = (QVariant::Type) QtVariantPropertyManager::enumTypeId();
		bDoEnumeratedList = true;
	}
	else if(expParamDef->eType == Experiment_ParameterType_DynamicIntegerEnum)
	{		
		varType = (QVariant::Type) QtVariantPropertyManager::enumTypeId();
		bDoEnumeratedList = true;
	}
	else if ((expParamDef->eType == Experiment_ParameterType_Float) || (expParamDef->eType == Experiment_ParameterType_Double))
	{
		varType = QVariant::Double;
		bDoMinMax = true;
	}
	else if(expParamDef->eType == Experiment_ParameterType_StringArray)
	{
		varType = (QVariant::Type) VariantExtensionPropertyManager::stringArrayTypeId();
	}
	else if(expParamDef->eType == Experiment_ParameterType_FilePath)
	{
		varType = (QVariant::Type) VariantExtensionPropertyManager::filePathTypeId();
	}
	else if(expParamDef->eType == Experiment_ParameterType_MethodType)
	{
		varType = (QVariant::Type) VariantExtensionPropertyManager::methodTypeTypeId();
	}
	else
	{
		varType = QVariant::String;
	}
	QtVariantProperty *tmpVariantProperty = lVariantPropertyManager->addVariantExtensionProperty(varType,expParamDef->bCanBeScriptReference,expParamDef->sDisplayName);
	tmpVariantProperty->setPropertyId(QString::number(expParamDef->nId));
	propertyParameterValueDef tmpParamValueDef;
	tmpParamValueDef.vType = varType;
	tmpParamValueDef.vProperty = tmpVariantProperty;
	tmpParamValueDef.bIsDerived = false;
	tmpVariantProperty->setEnabled(expParamDef->bIsEditable);
	lParameterPropertyNamedHash.insertMulti(expParamDef->sName.toLower(),tmpParamValueDef);
	lVariantPropertyDefinitionHash.insertMulti(tmpVariantProperty,(ExperimentParameterDefinitionStrc *)expParamDef);

	if(bDoEnumeratedList)
	{
		if(expParamDef->Restriction.lAllowedValues.isEmpty() == false)
		{
			tmpVariantProperty->setAttribute(QLatin1String("enumNames"), expParamDef->Restriction.lAllowedValues);
			int nTempEnumValue = 0;
			QString sAllowedValue = "";
			enumValueStruct tmpEnumValueStruct;
			foreach(sAllowedValue, expParamDef->Restriction.lAllowedValues)
			{
				tmpEnumValueStruct.nEnumControlIndex = nTempEnumValue;
				//tmpEnumValueStruct.vTranslationValue
				lEnumeratedParameterPropertyValuesHash.insert(QString(expParamDef->sName + EXPPARAMVIS_ENUM_SPEC_DIVIDER + sAllowedValue).toLower(),tmpEnumValueStruct);
				//QMap<int, QIcon> enumIcons;
				//enumIcons[0] = QIcon(":/demo/images/up.png");
				//enumIcons[1] = QIcon(":/demo/images/right.png");
				//enumIcons[2] = QIcon(":/demo/images/down.png");
				//enumIcons[3] = QIcon(":/demo/images/left.png");
				//tmpVariantProperty->setAttribute(QLatin1String("enumIcons"), enumIcons);
				//enumManager->setEnumIcons(item8, enumIcons);
				//set value?
				nTempEnumValue++;
			}
		}
	}
	if(bDoMinMax)
	{
		if(expParamDef->Restriction.MinimalValue.bEnabled)
			tmpVariantProperty->setAttribute(QLatin1String("minimum"), expParamDef->Restriction.MinimalValue.vValue);
		if(expParamDef->Restriction.MaximalValue.bEnabled)
			tmpVariantProperty->setAttribute(QLatin1String("maximum"), expParamDef->Restriction.MaximalValue.vValue);
	}

	if(variantExtensionFactory == NULL)
	{
		variantExtensionFactory = new VariantExtensionPropertyFactory(this);
		//VariantExtensionPropertyFactory *tmpFactory = new VariantExtensionPropertyFactory(this);
		propertyEditor->setFactoryForManager((QtVariantPropertyManager*)lVariantPropertyManager, (QtVariantEditorFactory*)variantExtensionFactory);
	}
		
	if (expParamDef->sGroupPath.isEmpty())
	{
		propertyEditor->addProperty(tmpVariantProperty);//Here we do not use a subgroup, use root location
	}
	else
	{
		addPropertyToSubGroup(expParamDef->sGroupPath, tmpVariantProperty, &lGroupPropertyCollection.propItem);
	}

	if(varType == QVariant::Color)
	{
		lVariantPropertyManager->setValue(tmpVariantProperty,QColor(vValue.toString()));
	}
	else if(varType == QVariant::Bool)
	{
		lVariantPropertyManager->setValue(tmpVariantProperty,vValue.toBool());
	}
	else if(varType == QVariant::Int)
	{
		lVariantPropertyManager->setValue(tmpVariantProperty,vValue.toInt());
	}
	else if((varType == QVariant::String) || (varType == (QVariant::Type) QtVariantPropertyManager::enumTypeId()))
	{
		if(bDoEnumeratedList)
		{
			QString sSearchVal = vValue.toString().toLower();
			for (int i=0;i<expParamDef->Restriction.lAllowedValues.count();i++)
			{
				if(expParamDef->Restriction.lAllowedValues.at(i).toLower() == sSearchVal)
				{
					if(expParamDef->bCanBeScriptReference)
						lVariantPropertyManager->setValue(tmpVariantProperty,expParamDef->Restriction.lAllowedValues.at(i));
					else
						lVariantPropertyManager->setValue(tmpVariantProperty,i);
					break;
				}
			}
		}
		else
		{
			//if(vValue.type() == )
			//QString tmpStr = vValue.toString();
			lVariantPropertyManager->setValue(tmpVariantProperty,vValue.toString());
		}		
	}
	else if(varType == QVariant::Double)
	{
		lVariantPropertyManager->setValue(tmpVariantProperty,vValue.toString());
	}
	else
	{
		lVariantPropertyManager->setValue(tmpVariantProperty,vValue.toString());
	}
	
	tmpVariantProperty->setToolTip(expParamDef->sInformation);
	tmpVariantProperty->setWhatsThis("");
	tmpVariantProperty->setStatusTip("");

	if(expParamDef->Dependencies.isEmpty() == false)
	{
		for (QList<ExperimentParameterDefinitionDependencyStrc>::const_iterator it=expParamDef->Dependencies.cbegin();it!=expParamDef->Dependencies.cend();++it)
		{
			addDependency(tmpVariantProperty,(*it));
		}
	}

	if(bAutoDepencyParsing == false)
		return true;
	return parseDependencies();
}

void ExperimentParameterVisualizer::itemEditedHandler(const QString &sParamName, const QString &sNewValue, const bool &bIsDerivedProperty)
{
	if(bIsDerivedProperty)
		emit derivedItemEditFinished(sParamName, sNewValue);
	else
		emit rootItemEditFinished(sParamName,sNewValue);
}

//void ExperimentParameterVisualizer::focusOutEvent(QFocusEvent* event)
//{ 
//	emit outOfFocus(); 
//}

void ExperimentParameterVisualizer::propertyValueChanged(QtProperty *property, const QVariant &value)
{
	Q_UNUSED(value);
	bool bIsDerivedProperty = false;
	QString sPropertyID = property->propertyId();
	QList<propertyParameterValueDef> tmpParamValueDefList = lParameterPropertyNamedHash.values(sPropertyID);
	if(tmpParamValueDefList.count() > 0)
	{
		bIsDerivedProperty = tmpParamValueDefList.at(0).bIsDerived;
	}
	//else if(tmpParamValueDefList.count() <> 1)
	//{
	//	qDebug() << __FUNCTION__ << "Could not retrieve a single unique property definition (Derived setting), number of results = " << QString::number(tmpParamValueDefList.count());
	//}

	property->setModified(true);

	if(bAutoDepencyParsing && (bIsDerivedProperty==false))
		parseDependencies((QtVariantProperty*) property);
	QList<ExperimentParameterDefinitionStrc*> lParamDefs = lVariantPropertyDefinitionHash.values(property);
	foreach(ExperimentParameterDefinitionStrc* paramDef, lParamDefs)
	{
		if(paramDef == NULL)
			continue;

		if(VariantExtensionPropertyManager::isScriptReferenceString(value.toString()))// && (value.toString().size() > 2))
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_String)
		{
			if(paramDef->Restriction.lAllowedValues.isEmpty() == true)
			{				
				itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
			}
			else
			{
				bool bIsInteger = false;
				int nIndex = value.toInt(&bIsInteger);
				if(bIsInteger==false)
				{
					itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
				}
				else if(paramDef->Restriction.lAllowedValues.count() > nIndex)
				{
					itemEditedHandler(QString(paramDef->sName), paramDef->Restriction.lAllowedValues.at(nIndex), bIsDerivedProperty);
				}
				else
				{
					qDebug() << __FUNCTION__ << "::Could not cast integer to enum (" + paramDef->sName + ")!";
				}
			}			
		}
		else if(paramDef->eType == Experiment_ParameterType_DynamicStringEnum)
		{
			if(paramDef->Restriction.lAllowedValues.isEmpty() == true)
			{				
				itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
			}
			else
			{
				bool bIsInteger = false;
				int nIndex = value.toInt(&bIsInteger);
				if(bIsInteger==false)
				{
					itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
				}
				else if(paramDef->Restriction.lAllowedValues.count() > nIndex)
				{
					itemEditedHandler(QString(paramDef->sName), paramDef->Restriction.lAllowedValues.at(nIndex), bIsDerivedProperty);
				}
				else
				{
					qDebug() << __FUNCTION__ << "::Could not cast integer to string enum (" + paramDef->sName + ")!";
				}
			}			
		}
		else if(paramDef->eType == Experiment_ParameterType_DynamicIntegerEnum)
		{
			if(paramDef->Restriction.lAllowedValues.isEmpty() == true)
			{				
				itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
			}
			else
			{
				bool bIsInteger = false;
				int nIndex = value.toInt(&bIsInteger);
				if(bIsInteger==false)
				{
					qDebug() << __FUNCTION__ << "::Could not cast integer(" + value.toString() + ") to integer enum (" + paramDef->sName + ")!";
				}
				else if(paramDef->Restriction.lAllowedValues.count() > nIndex)
				{
					QString sSearchVal = QString(paramDef->sName + EXPPARAMVIS_ENUM_SPEC_DIVIDER + QString::number(nIndex)).toLower();
					QVariant vEnumValue;
					if(getEnumeratedParameterPropertyValue(sSearchVal,vEnumValue))
					{
						itemEditedHandler(QString(paramDef->sName), vEnumValue.toString(), bIsDerivedProperty);
					}
					else
					{
						itemEditedHandler(QString(paramDef->sName), paramDef->Restriction.lAllowedValues.at(nIndex), bIsDerivedProperty);
					}
				}
				else
				{
					qDebug() << __FUNCTION__ << "::Could not cast integer to integer enum (" + paramDef->sName + ")!";
				}
			}			
		}
		else if(paramDef->eType == Experiment_ParameterType_StringArray)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_Color)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_Integer)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_Float)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_Double)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_Boolean)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_RotationDirection)
		{
			itemEditedHandler(QString(paramDef->sName), QString::number((int)RotationDirectionPropertyWidget::rotationDirectionEnum(value.toString())), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_MovementDirection)
		{
			itemEditedHandler(QString(paramDef->sName), QString::number((int)MovementDirectionPropertyWidget::movementDirectionEnum(value.toString())), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_EccentricityDirection)
		{
			itemEditedHandler(QString(paramDef->sName), QString::number((int)EccentricityDirectionPropertyWidget::eccentricityDirectionEnum(value.toString())), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_FilePath)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else if(paramDef->eType == Experiment_ParameterType_MethodType)
		{
			itemEditedHandler(QString(paramDef->sName), value.toString(), bIsDerivedProperty);
		}
		else
		{
			qDebug() << __FUNCTION__ << "Unknown Type(" << paramDef->eType << " for parameter " << paramDef->sName << ").";
		}
	}
}

bool ExperimentParameterVisualizer::addDependency(QtVariantProperty *variantProperty, const ExperimentParameterDefinitionDependencyStrc &dependencyParamDef)
{
	for (QList<propertyDependencyStruct>::iterator it=lPropertyDependencies.begin();it!=lPropertyDependencies.end();++it)
	{
		if(it->vProperty == variantProperty)
		{
			it->definitions.append(dependencyParamDef);
			return true;
		}
	}
	propertyDependencyStruct tmpPropertyDependency;
	tmpPropertyDependency.vProperty = variantProperty;
	lPropertyDependencies.append(tmpPropertyDependency);
	lPropertyDependencies.last().definitions.append(dependencyParamDef);
	return true;
}

bool ExperimentParameterVisualizer::parseDependencies(QtVariantProperty *variantProperty)
{
	if(lPropertyDependencies.isEmpty())
		return true;
	if(lVariantPropertyManager == NULL)
		return true;

	QList<propertyDependencyStruct>::iterator itDependency;
	//First make sure everything is enabled
	for (itDependency = lPropertyDependencies.begin(); itDependency != lPropertyDependencies.end(); ++itDependency)
	{
		if(variantProperty != NULL)
		{
			for (QList<ExperimentParameterDefinitionDependencyStrc>::const_iterator itDef=itDependency->definitions.cbegin();itDef!=itDependency->definitions.cend();++itDef)
			{
				if(QString::number(itDef->nDependencyParameterID) == variantProperty->propertyId())
				{
					itDependency->vProperty->setEnabled(true);
					QList<QtBrowserItem*> tmpList = propertyEditor->items(itDependency->vProperty);
					foreach(QtBrowserItem *bItem,tmpList)
						propertyEditor->setItemVisible(bItem,true);	
					break;
				}
			}
		}
		else
		{
			if(itDependency->vProperty)
			{
				itDependency->vProperty->setEnabled(true);
				QList<QtBrowserItem*> tmpList = propertyEditor->items(itDependency->vProperty);
				foreach(QtBrowserItem *bItem,tmpList)
					propertyEditor->setItemVisible(bItem,true);
			}
		}
	}
	//Now we need to check what to disable
	for (itDependency = lPropertyDependencies.begin(); itDependency != lPropertyDependencies.end(); ++itDependency)
	{
		if(variantProperty != NULL)
		{
			bool bPropertyFound = false;
			for (QList<ExperimentParameterDefinitionDependencyStrc>::const_iterator itDef=itDependency->definitions.cbegin();itDef!=itDependency->definitions.cend();++itDef)
			{
				if(QString::number(itDef->nDependencyParameterID) == variantProperty->propertyId())
				{
					//QString sName = variantProperty->propertyName();
					//int n1 = itDef->nDependencyParameterID;
					//QString s2 = variantProperty->propertyId().toLower();
					bPropertyFound = true;
					break;  
				}
			}
			if(bPropertyFound == false)
				continue;
		}
		for (QSet<QtProperty*>::const_iterator it=lVariantPropertyManager->properties().cbegin();it!=lVariantPropertyManager->properties().cend();++it)
		{
			int nID = -1;
			QString tmpS = (*it)->propertyId();
			if(tmpS.isEmpty())
				continue;
			if(tmpS == "0")
			{
				nID = 0;
			}
			else
			{
				nID = tmpS.toInt();
				if(nID == 0)
					continue;
			}
			for (QList<ExperimentParameterDefinitionDependencyStrc>::const_iterator itDef=itDependency->definitions.cbegin();itDef!=itDependency->definitions.cend();++itDef)
			{
				if(itDef->nDependencyParameterID == nID)
				{
					QString aa = (*it)->valueText();
					QString bb = itDef->rRegularExpression.pattern();

					if(itDef->rRegularExpression.match((*it)->valueText()).hasMatch() == false)
					{
						itDependency->vProperty->setEnabled(false);
						if(itDef->bHideWhenInactive)
						{							
							QList<QtBrowserItem*> tmpList = propertyEditor->items(itDependency->vProperty);
							foreach(QtBrowserItem *bItem,tmpList)
								propertyEditor->setItemVisible(bItem,false);						
						}
						break;
					}
				}
			}
		}
	}	
	return true;
}

void ExperimentParameterVisualizer::setAutoDepencyParsing(bool bEnable)
{
	bAutoDepencyParsing = bEnable;
}

void ExperimentParameterVisualizer::resizeParameterView(const int &nWidth, const int &nHeight)
{	
	Q_UNUSED(nWidth);
	this->setFixedWidth(300);
	this->setFixedHeight(nHeight - 50);
}


