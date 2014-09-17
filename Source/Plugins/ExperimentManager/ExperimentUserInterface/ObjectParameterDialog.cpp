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

#include "ObjectParameterDialog.h"
#include "ui_ObjectParameterDialog.h"
#include "ExperimentManager.h"
#include "ExperimentParameterWidgets.h"
#include "ParameterPropertyExtensions.h"
#include "ExperimentParameterVisualizer.h"

ObjectParameterDialog::ObjectParameterDialog(QWidget *parent) : QDialog(parent), pCurrentExpTree(NULL), pCurrentExpStructure(NULL), pParameterEditWidget(NULL), bIsParsing(false)
{
	setAttribute(Qt::WA_DeleteOnClose);

	ui = new Ui::ObjectParameterDialog();
	ui->setupUi(this);
	setWindowTitle("Configure object/block parameter(s)");

	connect(ui->pbUpdate, SIGNAL(clicked()), this, SLOT(updateSetParameter()));
	connect(ui->pbReset, SIGNAL(clicked()), this, SLOT(resetDeleteParameter()));
	connect(ui->pbRename, SIGNAL(clicked()), this, SLOT(renameParameter()));
	connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()));
	//connect(ui->cmbBlock, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedBlockChanged(int)));
	connect(ui->cmbBlock, SIGNAL(activated(int)), this, SLOT(selectedBlockChanged(int)));
	//connect(ui->cmbObject, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedObjectChanged(int)));
	connect(ui->cmbObject, SIGNAL(activated(int)), this, SLOT(selectedObjectChanged(int)));
	//connect(ui->cmbParameter, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedParameterChanged(int)));
	connect(ui->cmbParameter, SIGNAL(activated(int)), this, SLOT(selectedParameterChanged(int)));

	connect(ui->rdbParamType_1, SIGNAL(clicked()), this, SLOT(parameterTypeChanged()));
	connect(ui->rdbParamType_2, SIGNAL(clicked()), this, SLOT(parameterTypeChanged()));
	connect(ui->leName, SIGNAL(textEdited(QString)), this, SLOT(parameterNameChanged(QString)));
	
	leCustomParamValue = NULL;
	
	clearAllParsedStructures();
	eCurrentParameterEditingMode = PEM_DEFINED;
}

ObjectParameterDialog::~ObjectParameterDialog()
{
	clearAllParsedStructures();
	delete ui;
}

void ObjectParameterDialog::parameterNameChanged(const QString &sNewName)
{
	Q_UNUSED(sNewName);
	if(eCurrentParameterEditingMode == PEM_CUSTOM)
	{
		if(ui->cmbParameter->currentIndex() >= 0)
		{
			ui->pbRename->setEnabled(true);
			ui->pbUpdate->setEnabled(true);
			ui->pbUpdate->setText("New");
		}
		ui->pbReset->setEnabled(false);
	}
}

void ObjectParameterDialog::parameterTypeChanged()
{
	bIsParsing = true;
	if(ui->rdbParamType_1->isChecked() && (eCurrentParameterEditingMode != PEM_DEFINED))//pre-defined
	{
		eCurrentParameterEditingMode = PEM_DEFINED;
		ui->leName->setReadOnly(true);
		clearParameterWidgetLayout();
	}
	else if (ui->rdbParamType_2->isChecked() && (eCurrentParameterEditingMode != PEM_CUSTOM))//custom
	{
		eCurrentParameterEditingMode = PEM_CUSTOM;
		ui->leName->setReadOnly(false);
		ui->leValue->clear();
		ui->leName->clear();
		clearParameterWidgetLayout();
		leCustomParamValue = new QLineEdit();
		ui->lEditParameter->insertWidget(0,leCustomParamValue);
	}
	else
	{
		return;
	}
	parseParameters(hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()],hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()], eCurrentParameterEditingMode);
	bIsParsing = false;
	selectedParameterChanged(ui->cmbParameter->currentIndex());
} 

void ObjectParameterDialog::clearParameterWidgetLayout()
{
	if(ui->lEditParameter)
	{
		delete ui->lEditParameter;
		ui->lEditParameter = NULL;
		ui->lEditParameter = new QHBoxLayout(ui->horizontalLayoutWidget);
		//ui->lEditParameter->setDirection(QHBoxLayout::RightToLeft);
		ui->lEditParameter->setContentsMargins(11, 11, 11, 11);//(0,0,0,0);
		ui->lEditParameter->setMargin(0);
		ui->lEditParameter->setSpacing(6);//(0);
		ui->lEditParameter->setObjectName(QStringLiteral("lEditParameter"));
	}
}

void ObjectParameterDialog::renameParameter()
{
	ExperimentTreeModel::strcParameterSpecifier cParameterSpecifier;
	cParameterSpecifier.nBlockID = hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()];
	cParameterSpecifier.nObjectID = hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()];
	cParameterSpecifier.sParamHexID = hashParamControlIndexToUniqueHexParamID[ui->cmbParameter->currentIndex()];
	cParameterSpecifier.sParamName = mapUniqueHexParamIDToParamDef[cParameterSpecifier.sParamHexID].ParamDef.sName;
	if(pCurrentExpTree->renameExperimentParameter(cParameterSpecifier,ui->leName->text()))
	{
		int nCurrentParamIndex = ui->cmbParameter->currentIndex();
		parseParameters(cParameterSpecifier.nObjectID, cParameterSpecifier.nBlockID, eCurrentParameterEditingMode);
		ui->cmbParameter->setCurrentIndex(nCurrentParamIndex);
	}
}

void ObjectParameterDialog::resetDeleteParameter()
{
	ExperimentTreeModel::strcParameterSpecifier cParameterSpecifier;
	cParameterSpecifier.nBlockID = hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()];
	cParameterSpecifier.nObjectID = hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()];
	cParameterSpecifier.sParamHexID = hashParamControlIndexToUniqueHexParamID[ui->cmbParameter->currentIndex()];
	cParameterSpecifier.sParamName = mapUniqueHexParamIDToParamDef[cParameterSpecifier.sParamHexID].ParamDef.sName;
	QList<ExperimentTreeModel::strcParameterSpecifier> lParameterSpecifiers;
	lParameterSpecifiers.append(cParameterSpecifier);
	if(pCurrentExpTree->removeExperimentParameters(lParameterSpecifiers))
	{
		parseParameters(cParameterSpecifier.nObjectID, cParameterSpecifier.nBlockID, eCurrentParameterEditingMode);
		int nNewParamIndex;
		if(ui->cmbParameter->count() > 0)
			nNewParamIndex = 0;
		else
			nNewParamIndex = -1;
		ui->cmbParameter->setCurrentIndex(nNewParamIndex);
		selectedParameterChanged(nNewParamIndex);
	}
}

void ObjectParameterDialog::updateSetParameter()
{
	ExperimentTreeModel::strcParameterSpecifier cParameterSpecifier;
	int nCurrentParameter = ui->cmbParameter->currentIndex();
	cParameterSpecifier.nBlockID = hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()];
	if(cParameterSpecifier.nBlockID < 0)
		return;
	cParameterSpecifier.nObjectID = hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()];
	if(cParameterSpecifier.nObjectID < 0)
		return;
	cParameterSpecifier.sParamHexID = hashParamControlIndexToUniqueHexParamID[nCurrentParameter];
	if(eCurrentParameterEditingMode == PEM_CUSTOM)
		cParameterSpecifier.sParamName = ui->leName->text();//mapUniqueHexParamIDToParamDef[cParameterSpecifier.sParamHexID].ParamDef.sName.toLower();
	else
		cParameterSpecifier.sParamName = mapUniqueHexParamIDToParamDef[cParameterSpecifier.sParamHexID].ParamDef.sName.toLower();
	if(cParameterSpecifier.sParamName.isEmpty())
		return;
	if(eCurrentParameterEditingMode == PEM_CUSTOM)
	{
		if(ui->lEditParameter->itemAt(0))
		{
			QWidget* pTmpWidget = ui->lEditParameter->itemAt(0)->widget();
			if(pTmpWidget)
			{
				QLineEdit *pTmpLineEdit = qobject_cast<QLineEdit *>(pTmpWidget);
				if(pTmpLineEdit)
				{
					bool bShouldOnlyUpdate = (ui->leName->text().toLower() == mapUniqueHexParamIDToParamDef[cParameterSpecifier.sParamHexID].ParamDef.sName.toLower()); 
					if(pCurrentExpTree->addUpdateExperimentBlockParameter(cParameterSpecifier, pTmpLineEdit->text(),(eCurrentParameterEditingMode == PEM_CUSTOM)))
					{
						parseParameters(cParameterSpecifier.nObjectID, cParameterSpecifier.nBlockID, eCurrentParameterEditingMode);
						if(bShouldOnlyUpdate)
						{
							selectedParameterChanged(nCurrentParameter);
						}
						else
						{
							int nNewLatestIndex = ui->cmbParameter->count()-1;
							ui->cmbParameter->setCurrentIndex(nNewLatestIndex);
							selectedParameterChanged(nNewLatestIndex);
						}
					}
				}
			}
		}
	}
	else if(eCurrentParameterEditingMode == PEM_DEFINED)
	{
		if(pCurrentExpTree)
		{
			if((cLastUserChange.sParamNewValue.isEmpty()) && (cLastUserChange.sParamName.isEmpty()))
			{
				cLastUserChange.eChangeEditingMode = eCurrentParameterEditingMode;
				cLastUserChange.sParamName = cParameterSpecifier.sParamName;
				cLastUserChange.sParamNewValue = VariantExtensionPropertyManager::resolveParameterValueType(ui->leValue->text(), mapUniqueHexParamIDToParamDef[cParameterSpecifier.sParamHexID].ParamDef.eType,false).toString();
				cLastUserChange.bSettingFinished = true;
			}
			if((cLastUserChange.bSettingFinished == true) && (cLastUserChange.sParamName.toLower() == cParameterSpecifier.sParamName) && (cLastUserChange.sParamNewValue.isEmpty() == false))
			{
				if(pCurrentExpTree->saveNewData(cParameterSpecifier.nBlockID, cParameterSpecifier.nObjectID, cParameterSpecifier.sParamName, cLastUserChange.sParamNewValue))
				{
					parseParameters(cParameterSpecifier.nObjectID, cParameterSpecifier.nBlockID, eCurrentParameterEditingMode);
					int nParamControlIndex = hashParamControlIndexToUniqueHexParamID.key(cParameterSpecifier.sParamHexID,-1);
					if(nParamControlIndex>=0)
					{
						ui->cmbParameter->setCurrentIndex(nParamControlIndex);
						selectedParameterChanged(nParamControlIndex);
						cLastUserChange.Reset();
					}
				}
			}
		}
	}
}

void ObjectParameterDialog::selectedObjectChanged(int nIndex)
{
	Q_UNUSED(nIndex)
	if(bIsParsing == false)
	{
		if(pCurrentExpTree)
		{
			parseParameters(hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()],hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()],eCurrentParameterEditingMode);
			selectedParameterChanged(ui->cmbParameter->currentIndex());
		}
	}
}

void ObjectParameterDialog::selectedBlockChanged(int nIndex)
{
	Q_UNUSED(nIndex)
	if(bIsParsing == false)
	{
		if(pCurrentExpTree)
		{
			parseParameters(hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()],hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()],eCurrentParameterEditingMode);
			selectedParameterChanged(ui->cmbParameter->currentIndex());
		}
	}
}

void ObjectParameterDialog::derivedItemEditFinished(const QString& sParamName, const QString& sNewValue)
{
	cLastUserChange.bSettingFinished = false;
	cLastUserChange.sParamName = sParamName;
	cLastUserChange.sParamNewValue = sNewValue;
	cLastUserChange.eChangeEditingMode = eCurrentParameterEditingMode;
	cLastUserChange.bSettingFinished = true;
	return;
}

void ObjectParameterDialog::selectedParameterChanged(int nIndex)
{
	if(bIsParsing == false)
	{
		ui->leValue->setText("<undefined>");
		ui->pbUpdate->setEnabled(false);
		ui->pbUpdate->setText("New/Update");
		ui->pbReset->setEnabled(false);
		ui->pbReset->setText("Remove");
		if(pParameterEditWidget)
		{
			pParameterEditWidget->setParent(NULL);
			pParameterEditWidget = NULL;
		}
		clearParameterWidgetLayout();
		if(nIndex >= 0)
		{
			if(hashParamControlIndexToUniqueHexParamID.contains(nIndex))
			{
				if(mapUniqueHexParamIDToParamDef.contains(hashParamControlIndexToUniqueHexParamID[nIndex]))
				{
					QString sResolvedValue;
					if(mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].bIsChanged)
						sResolvedValue = VariantExtensionPropertyManager::resolveParameterValueType(mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].sValue,mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].ParamDef.eType,true).toString();
					else
						sResolvedValue = VariantExtensionPropertyManager::resolveParameterValueType(mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].ParamDef.sDefaultValue,mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].ParamDef.eType,true).toString();
					ui->leValue->setText(sResolvedValue);

					if(expParamVizualizer && (ui->rdbParamType_2->isChecked() == false))
					{
						bool bTempPropertyEditSignaling = expParamVizualizer->hasPropertyEditSignaling();
						if(bTempPropertyEditSignaling)
							expParamVizualizer->configurePropertyEditSignaling(false);
							
						QString sUniqueGeneratedPropertyIdentifier = "";
						int nObjectID = hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()];
						int nBlockID = hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()];
						QString sDerivedPrefixName = tr("%1:%2").arg(nObjectID).arg(nBlockID);
						QString sParamName = mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].ParamDef.sName;
						bool bIsScriptable = mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].ParamDef.bCanBeScriptReference;
						pParameterEditWidget = expParamVizualizer->getParameterEditWidget(sParamName, sDerivedPrefixName, sUniqueGeneratedPropertyIdentifier, sResolvedValue, true, bIsScriptable);
						if(pParameterEditWidget)
						{					
							pParameterEditWidget->setAutoFillBackground(true);
							connect((QObject*)expParamVizualizer, SIGNAL(derivedItemEditFinished(const QString&, const QString&)), this, SLOT(derivedItemEditFinished(const QString&, const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));					
							ui->lEditParameter->insertWidget(0,pParameterEditWidget);
						
							if(mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].bIsChanged)
							{
								ui->pbUpdate->setEnabled(true);
								ui->pbUpdate->setText("Update");
								ui->pbReset->setEnabled(true);
								ui->pbReset->setText("Remove");
							}
							else
							{
								ui->pbReset->setEnabled(false);
								ui->pbUpdate->setEnabled(true);
								ui->pbUpdate->setText("Set");
							}
						}
						if(bTempPropertyEditSignaling)
							expParamVizualizer->configurePropertyEditSignaling(true);
					}
					else if(ui->rdbParamType_2->isChecked())//custom
					{
						if(ui->lEditParameter->itemAt(0))
						{
							QWidget* pTmpWidget = ui->lEditParameter->itemAt(0)->widget();
							if(pTmpWidget)
							{
								QLineEdit *pTmpLineEdit = qobject_cast<QLineEdit *>(pTmpWidget);
								if(pTmpLineEdit)
								{
									pTmpLineEdit->setText(sResolvedValue);
								}
							}
						}
						//if(mapUniqueHexParamIDToParamDef[hashParamControlIndexToUniqueHexParamID[nIndex]].bIsChanged)//always the case here for custom parameters...
						//{
						ui->pbUpdate->setEnabled(true);
						ui->pbUpdate->setText("Update");
						ui->pbReset->setEnabled(true);
						ui->pbReset->setText("Remove");
						//}
					}
					ui->leName->setText(ui->cmbParameter->currentText());
					ui->pbRename->setEnabled(false);
				}				
			}
		}
		else
		{
			ui->leName->clear();
			if(ui->lEditParameter->itemAt(0))
			{
				QWidget* pTmpWidget = ui->lEditParameter->itemAt(0)->widget();
				if(pTmpWidget)
				{
					QLineEdit *pTmpLineEdit = qobject_cast<QLineEdit *>(pTmpWidget);
					if(pTmpLineEdit)
					{
						pTmpLineEdit->clear();
						if(eCurrentParameterEditingMode == PEM_CUSTOM)
						{
							ui->pbUpdate->setText("New");
							ui->pbUpdate->setEnabled(true);
						}
					}
				}
			}
		}
	}
	cLastUserChange.Reset();
}

bool ObjectParameterDialog::parseExperimentTreeModel(ExperimentTreeModel *pExperimentTreeModel)
{
	bIsParsing = true;
	clearAllParsedStructures();
	pCurrentExpTree = pExperimentTreeModel;
	pCurrentExpStructure = new cExperimentStructure();	
	bool bResult;
	
	bResult = ExperimentManager::createExperimentStructure(lCurrentExpTreeItems,pCurrentExpTree,pCurrentExpStructure);
	if(bResult)
	{
		int nExpBlockCount = pCurrentExpStructure->getBlockCount();
		cBlockStructure *tmpBlock = NULL;
		int nNextSearchBlockNumber = 0;
		QString sExpName = pCurrentExpStructure->getExperimentName();
		QString tmpString;
		int i;
		for (i=0;i<nExpBlockCount;i++)
		{
			tmpBlock = NULL;
			tmpBlock = pCurrentExpStructure->getNextClosestBlockNumberByFromNumber(nNextSearchBlockNumber);
			if(tmpBlock) 
			{
				tmpString = QString::number(tmpBlock->getBlockNumber()) + ": " + tmpBlock->getBlockName();
				ui->cmbBlock->addItem(tmpString);				
				hBlockControlIndexToBlockID.insert(i,tmpBlock->getBlockID());
				nNextSearchBlockNumber = tmpBlock->getBlockNumber() + 1;
			}
		}
				
		lCurrentExpObjects = pCurrentExpTree->getDefinedExperimentObjectInfoList(NULL);
		if(lCurrentExpObjects.isEmpty() == false)
		{
			for(i=0;i<lCurrentExpObjects.count();i++)
			{
				tmpString = lCurrentExpObjects[i].sName;
				ui->cmbObject->addItem(tmpString);
				hObjectControlIndexToObjectID.insert(i,lCurrentExpObjects[i].nID);
			}
		}
	}
	if(bResult)
	{
		parseParameters(hObjectControlIndexToObjectID[ui->cmbObject->currentIndex()],hBlockControlIndexToBlockID[ui->cmbBlock->currentIndex()],eCurrentParameterEditingMode);
		bIsParsing = false;	
		selectedParameterChanged(ui->cmbParameter->currentIndex());
	}
	else
	{
		clearAllParsedStructures();
		bIsParsing = false;	
	}			
	return bResult;
}

void ObjectParameterDialog::clearAllParsedStructures()
{
	pCurrentExpTree = NULL;
	expParamVizualizer = NULL;
	if(pCurrentExpStructure)
	{
		delete pCurrentExpStructure;
		pCurrentExpStructure = NULL;
	}	
	clearParameterWidgetLayout();
	if(pParameterEditWidget)
	{
		pParameterEditWidget->setParent(NULL);
		pParameterEditWidget = NULL;
	}
	if(leCustomParamValue)
	{
		delete leCustomParamValue;
		leCustomParamValue = NULL;
	}
	lCurrentExpObjects.clear();
	lCurrentExpTreeItems.clear();
	hObjectControlIndexToObjectID.clear();
	hBlockControlIndexToBlockID.clear();
	mapUniqueHexParamIDToParamDef.clear();
	hashParamControlIndexToUniqueHexParamID.clear();
	hashParamNameToParamID.clear();
	ui->pbRename->setEnabled(false);
	ui->pbReset->setEnabled(false);
	ui->pbUpdate->setEnabled(false);
	ui->pbUpdate->setText("New/Update value");
}
				
bool ObjectParameterDialog::parseParameters(const int &nObjectID, const int &nBlockID, const ParameterEditingType &eParamEditingMode)
{
	bool bWasParsing = bIsParsing;
	bIsParsing = true;	
	ui->cmbParameter->clear();
	hashParamControlIndexToUniqueHexParamID.clear();
	mapUniqueHexParamIDToParamDef.clear();
	hashParamNameToParamID.clear();
	bool bResult = false;
	if((nObjectID<0) || (nBlockID<0))
	{
		bIsParsing = bWasParsing;
		return bResult;
	}
	int nBlockCount = pCurrentExpStructure->getBlockCount();
	int nObjectCount = lCurrentExpObjects.count();

	if((nObjectCount<1) || (nBlockCount<1) || (lCurrentExpTreeItems.isEmpty()))
	{
		bIsParsing = bWasParsing;
		return bResult;	
	}
	ExperimentParameterWidgets *expParamWidgets = ExperimentParameterWidgets::instance();
	ExperimentParameterDefinitionContainer *expParamDefContainer = NULL;
	QList<ExperimentParameterDefinitionStrc> *lTmpExpParamDefStrc = NULL;
	ExperimentParameterDefinitionStrc tmpExpParamDefStrc;
	QString tmpString;

	for(int i=0;i<lCurrentExpObjects.count();i++)
	{
		if(lCurrentExpObjects[i].nID == nObjectID)
		{
			QString sClassName = lCurrentExpObjects[i].sClass;
			if(expParamVizualizer)
				pParameterEditWidget = NULL;//Always correct?
			expParamVizualizer = expParamWidgets->getExperimentParameterWidget(sClassName);
			expParamDefContainer = expParamWidgets->getExperimentParameterDefinition(sClassName);
			if(expParamDefContainer)
			{
				lTmpExpParamDefStrc = expParamDefContainer->getParameterDefinitions();
				break;
			}
			break;
		}
	}

	if(eParamEditingMode != PEM_CUSTOM)
	{
		if(lTmpExpParamDefStrc == NULL)
		{
			bIsParsing = bWasParsing;
			return bResult;
		}
		foreach(tmpExpParamDefStrc,*lTmpExpParamDefStrc)
		{
			if(tmpExpParamDefStrc.bEnabled)
			{
				tmpString = tmpExpParamDefStrc.sGroupPath + ";" + tmpExpParamDefStrc.sDisplayName;
				tmpString = tmpString.replace(";"," / ");
				strcParameterDefinition tmpParamDef;
				tmpParamDef.ParamDef = tmpExpParamDefStrc;
				tmpParamDef.sFullDisplayName = tmpString;
				mapUniqueHexParamIDToParamDef[QString::number((int)PEM_DEFINED) + QString(tmpExpParamDefStrc.nId).toLatin1().toHex()] = tmpParamDef;
				hashParamNameToParamID.insert(tmpExpParamDefStrc.sName,tmpExpParamDefStrc.nId);
			}
		}
	}
		
	ExperimentTreeItem* pTmpExpTreeItem;
	QMap<QString, TreeItemDefinition> tTmpTreeItemDefs;
	int nTempBlockID = -1;
	int nTempObjectID = -1;
	int nTempParameterID = -1;
	int nNrOfParameters = -1;
	int nTempCurrentBlockObjectCount = -1;
	QString sTempParamName;
	QString sTempParamValue;
	QString uniqueObjParamIdentifier;
	QStringList lObjectSearchPath;
	QStringList lObjectParameterSearchPath;
	lObjectSearchPath << OBJECT_TAG;
	if(eParamEditingMode == PEM_CUSTOM)
		lObjectParameterSearchPath << CUSTOM_PARAMETERS_TAG << PARAMETER_TAG;
	else //if(eParamEditingMode == PEM_DEFINED)
		lObjectParameterSearchPath << PARAMETERS_TAG << PARAMETER_TAG;
	for (int i=0;i<nBlockCount;i++) //for each block
	{
		nTempBlockID = -1;
		nTempObjectID = -1;
		nTempParameterID = -1;
		nNrOfParameters = -1;
		nTempCurrentBlockObjectCount = -1;

		//BlockID
		pTmpExpTreeItem = lCurrentExpTreeItems.at(i);
		tTmpTreeItemDefs = pTmpExpTreeItem->getDefinitions();
		if(tTmpTreeItemDefs.contains(ID_TAG))
		{
			nTempBlockID = tTmpTreeItemDefs[ID_TAG].value.toInt();
			if(nTempBlockID == nBlockID)
			{
				//Objects
				QList<ExperimentTreeItem*> lExpTreeObjectItems;
				nTempCurrentBlockObjectCount = ExperimentTreeModel::getStaticTreeElements(lObjectSearchPath, lExpTreeObjectItems, lCurrentExpTreeItems.at(i));
				if(nTempCurrentBlockObjectCount == 0)
					continue;
				for (int j=0;j<nTempCurrentBlockObjectCount;j++)//For each Object
				{
					//ObjectID
					tTmpTreeItemDefs = lExpTreeObjectItems.at(j)->getDefinitions();
					if(tTmpTreeItemDefs.contains(ID_TAG))
					{
						nTempObjectID = tTmpTreeItemDefs[ID_TAG].value.toInt();
						if(nTempObjectID != nObjectID)
							continue;

						//Pre-defined or Custom Parameters
						QList<ExperimentTreeItem*> lExpTreeObjectParameterItems;
						nNrOfParameters = ExperimentTreeModel::getStaticTreeElements(lObjectParameterSearchPath, lExpTreeObjectParameterItems, lExpTreeObjectItems.at(j));
						if(nNrOfParameters > 0)
						{
							for (int k=0;k<nNrOfParameters;k++)//For each Parameter
							{
								//ParameterID
								tTmpTreeItemDefs = lExpTreeObjectParameterItems.at(k)->getDefinitions();
								if(tTmpTreeItemDefs.contains(ID_TAG))
									nTempParameterID = tTmpTreeItemDefs[ID_TAG].value.toInt();
								else
									continue;
									
								//ParameterName
								sTempParamName = "";
								pTmpExpTreeItem = lExpTreeObjectParameterItems.at(k)->firstChild(NAME_TAG);
								if(pTmpExpTreeItem)
									sTempParamName = pTmpExpTreeItem->getValue();
								else
									continue;

								//ParameterValue
								pTmpExpTreeItem = lExpTreeObjectParameterItems.at(k)->firstChild(VALUE_TAG);
								if((pTmpExpTreeItem) && (sTempParamName.isEmpty() == false))
								{
									sTempParamValue = pTmpExpTreeItem->getValue();
									if((eParamEditingMode == PEM_CUSTOM) && (hashParamNameToParamID.contains(sTempParamName.toLower())==false))
									{
										tmpString = "Custom: " + sTempParamName;
										ExperimentParameterDefinitionStrc tmpExpCustomParamStruct;
										tmpExpCustomParamStruct.bEnabled = true;
										tmpExpCustomParamStruct.eType = Experiment_ParameterType_String;
										tmpExpCustomParamStruct.nId = k;
										tmpExpCustomParamStruct.sDefaultValue = "";
										tmpExpCustomParamStruct.sName = sTempParamName;
										tmpExpCustomParamStruct.sDisplayName = tmpExpCustomParamStruct.sName;
										tmpExpCustomParamStruct.bCanBeScriptReference = true;
										tmpExpCustomParamStruct.sInformation = EXPERIMENT_CUSTOMPARAM_INFOSTRING;
										strcParameterDefinition tmpParamDef;
										tmpParamDef.ParamDef = tmpExpCustomParamStruct;
										tmpParamDef.sFullDisplayName = tmpString;
										mapUniqueHexParamIDToParamDef[QString::number((int)eParamEditingMode) + QString(tmpExpCustomParamStruct.nId).toLatin1().toHex()] = tmpParamDef;
										hashParamNameToParamID.insert(tmpExpCustomParamStruct.sName.toLower(),tmpExpCustomParamStruct.nId);
									}
									if(hashParamNameToParamID.contains(sTempParamName.toLower()))
										tmpString = QString::number((int)eParamEditingMode) + QString(hashParamNameToParamID[sTempParamName.toLower()]).toLatin1().toHex();
									else
										tmpString = "";
									bResult = true;
									if((mapUniqueHexParamIDToParamDef.contains(tmpString)) && (eParamEditingMode == PEM_DEFINED))
									{
										ui->cmbParameter->addItem(mapUniqueHexParamIDToParamDef[tmpString].sFullDisplayName);
										mapUniqueHexParamIDToParamDef[tmpString].bIsChanged = true;
										mapUniqueHexParamIDToParamDef[tmpString].sValue = sTempParamValue;
										hashParamControlIndexToUniqueHexParamID.insert(ui->cmbParameter->count()-1,tmpString);										
										ui->cmbParameter->setItemData(ui->cmbParameter->count()-1, QColor(Qt::red), Qt::DecorationRole);
									}
									else if((mapUniqueHexParamIDToParamDef.contains(tmpString)) && (eParamEditingMode == PEM_CUSTOM))
									{
										ui->cmbParameter->addItem(sTempParamName);
										mapUniqueHexParamIDToParamDef[tmpString].sFullDisplayName = sTempParamName;
										mapUniqueHexParamIDToParamDef[tmpString].bIsChanged = true;
										mapUniqueHexParamIDToParamDef[tmpString].sValue = sTempParamValue;										
										hashParamControlIndexToUniqueHexParamID.insert(ui->cmbParameter->count()-1,tmpString);										
										ui->cmbParameter->setItemData(ui->cmbParameter->count()-1, QColor(Qt::darkMagenta), Qt::DecorationRole);
									}
									else
									{
										bResult = false;
									}
								}
							}
						}
						else if(nNrOfParameters == 0)
						{
							bResult = true;
						}
					}						
				}
			}
		}
	}

	if(bResult)//Fill remaining undefined block parameters
	{
		for (int i=0;i<mapUniqueHexParamIDToParamDef.count();i++)
		{
			if(mapUniqueHexParamIDToParamDef.values().at(i).bIsChanged == false)
			{
				ui->cmbParameter->addItem(mapUniqueHexParamIDToParamDef.values().at(i).sFullDisplayName);
				tmpString = mapUniqueHexParamIDToParamDef.keys().at(i);
				hashParamControlIndexToUniqueHexParamID.insert(ui->cmbParameter->count()-1,tmpString);
			}			
		}
	}
	bIsParsing = bWasParsing;
	return bResult;
}
