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

#include "experimentobjectsdialog.h"
#include "ui_ExperimentObjectsDialog.h"
#include "experimentmanager.h"
#include "propertysettingswidgetcontainer.h"
#include "experimentmethodparamsconfiguration.h"

ExperimentObjectsDialog::ExperimentObjectsDialog(QWidget *parent) : QDialog(parent), bIsObjectDeleting(false), bIsParsing(false), bDisableEditUpdates(false), pCurrentExpStructure(NULL), twCurrentObjects(NULL), layoutObjectDeclarationTreeWidgetParent(NULL), layoutObjectInitTreeWidgetParent(NULL), layoutObjectFinitTreeWidgetParent(NULL), pExpParamWidgets(NULL), pObjectDeclarationWidget(NULL), pObjectInitsWidget(NULL), pObjectFinitsWidget(NULL), pObjectInitFinitWidget(NULL), pExperimentTreeModel(NULL), nCurrentObjectSelectionIdentifier(-1), nCurrentObjectInitSelectionIdentifier(-1), nCurrentObjectFinitSelectionIdentifier(-1)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui = new Ui::ExperimentObjectsDialog();
	ui->setupUi(this);
	setWindowTitle("Configure experiment object(s)");
	ui->pbInitMoveUp->setIcon((QIcon(":/resources/moveup.png")));
	ui->pbInitMoveDown->setIcon((QIcon(":/resources/movedown.png")));
	ui->pbFinitMoveUp->setIcon((QIcon(":/resources/moveup.png")));
	ui->pbFinitMoveDown->setIcon((QIcon(":/resources/movedown.png")));
	twCurrentObjects = new QTreeWidget(ui->groupBox_2);
	ui->layCurrentObjects->addWidget(twCurrentObjects);
	bool bConnectResult = false;
	bConnectResult = connect(twCurrentObjects, SIGNAL(itemSelectionChanged()), this, SLOT(onObjectSelectionChanged()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(addSelectedObject()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbRemove, SIGNAL(clicked()), this, SLOT(removeSelectedObject()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->leObjectName, SIGNAL(textEdited(QString)), this, SLOT(onNewObjectNameChanged(QString)), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->lwObjectInitUsed, SIGNAL(itemSelectionChanged()), this, SLOT(onObjectInitSelectionChanged()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->lwObjectFinitUsed, SIGNAL(itemSelectionChanged()), this, SLOT(onObjectFinitSelectionChanged()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbInitRemove, SIGNAL(clicked()), this, SLOT(onRemoveSelectedObjectInit()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbFinitRemove, SIGNAL(clicked()), this, SLOT(onRemoveSelectedObjectFinit()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbInitConfigureParameters, SIGNAL(clicked()), this, SLOT(onConfigureSelectedObjectInitParams()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbFinitConfigureParameters, SIGNAL(clicked()), this, SLOT(onConfigureSelectedObjectFinitParams()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbInitAdd, SIGNAL(clicked()), this, SLOT(addSelectedInit()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbFinitAdd, SIGNAL(clicked()), this, SLOT(addSelectedFinit()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbInitMoveUp, SIGNAL(clicked()), this, SLOT(moveUpSelectedInit()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbInitMoveDown, SIGNAL(clicked()), this, SLOT(moveDownSelectedInit()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbFinitMoveUp, SIGNAL(clicked()), this, SLOT(moveUpSelectedFinit()), Qt::ConnectionType(Qt::UniqueConnection));
	bConnectResult = connect(ui->pbFinitMoveDown, SIGNAL(clicked()), this, SLOT(moveDownSelectedFinit()), Qt::ConnectionType(Qt::UniqueConnection));

	ui->tabWidget->setVisible(false);
	fetchAvailableExperimentObjectClasses();
}

ExperimentObjectsDialog::~ExperimentObjectsDialog()
{
	clearAllParsedStructures();
	if(pExpParamWidgets)
		pExpParamWidgets = NULL;
	if(pObjectDeclarationWidget)
	{
		pObjectDeclarationWidget->setParent(NULL);
		if(pObjectDeclarationWidget->isEnabled()==false)
			pObjectDeclarationWidget->setEnabled(true);
		pObjectDeclarationWidget = NULL;
	}
	if(pObjectInitFinitWidget)
	{
		pObjectInitFinitWidget = NULL;
	}
	if(pObjectInitsWidget)
	{
		pObjectInitsWidget->setParent(NULL);
		if(pObjectInitsWidget->isEnabled()==false)
			pObjectInitsWidget->setEnabled(true);
		pObjectInitsWidget = NULL;
	}
	if(pObjectFinitsWidget)
	{
		pObjectFinitsWidget->setParent(NULL);
		if(pObjectFinitsWidget->isEnabled()==false)
			pObjectFinitsWidget->setEnabled(true);
		pObjectFinitsWidget = NULL;
	}
	if(layoutObjectDeclarationTreeWidgetParent)
	{
		delete layoutObjectDeclarationTreeWidgetParent;
		layoutObjectDeclarationTreeWidgetParent = NULL;
	}
	if(layoutObjectInitTreeWidgetParent)
	{
		delete layoutObjectInitTreeWidgetParent;
		layoutObjectInitTreeWidgetParent = NULL;
	}
	if(layoutObjectFinitTreeWidgetParent)
	{
		delete layoutObjectFinitTreeWidgetParent;
		layoutObjectFinitTreeWidgetParent = NULL;
	}
	if(pExperimentTreeModel)
		pExperimentTreeModel = NULL;
	delete ui;
}

void ExperimentObjectsDialog::fetchAvailableExperimentObjectClasses()
{
	ui->cmbObjects->clear();
	//ui->cmbObjects->setInsertPolicy(QComboBox::InsertAlphabetically);
	QList<int> lRegisteredMetaTypeIds = MainAppInfo::getRegisteredMetaTypeIdList();
	QString sMetaClassName;
	QMap<QString,QString> tmpMappedList;
	foreach(int nMetaTypeID,lRegisteredMetaTypeIds)
	{
		sMetaClassName = QMetaType::typeName(nMetaTypeID);
		tmpMappedList.insert(sMetaClassName.toLower(), sMetaClassName);
	}
	QMapIterator<QString, QString> iter(tmpMappedList);
	while (iter.hasNext())
	{
		iter.next();
		ui->cmbObjects->addItem("Class: " + iter.value(), iter.value());
	}
}

void ExperimentObjectsDialog::clearAllParsedStructures()
{
	if(pCurrentExpStructure)
	{
		delete pCurrentExpStructure;
		pCurrentExpStructure = NULL;
	}
	twCurrentObjects->clear();
	mapObjectIdToObjectTreeWidgetItems.clear();
	hashIntToExperimentObjectInfo.clear();
	nCurrentObjectSelectionIdentifier = -1;
	nCurrentObjectInitSelectionIdentifier = -1;
	nCurrentObjectFinitSelectionIdentifier = -1;
}

void ExperimentObjectsDialog::onNewObjectNameChanged(const QString &sText)
{
	if((sText.isEmpty()) || (sText.contains(" ")) || (sText.contains("{")) || (sText.contains("}")) || (sText.contains("\\")) || (sText.contains("/")  || (sText.contains("(")) || (sText.contains(")"))))
		ui->pbAdd->setEnabled(false);
	else
		ui->pbAdd->setEnabled(true);
}

void ExperimentObjectsDialog::onObjectSelectionChanged()
{
	prepareAndHideObjectConfiguration();
	QList<QTreeWidgetItem *> lSelectedItems = twCurrentObjects->selectedItems();
	bool bConvertDataResult = false;
	if(lSelectedItems.count() == 1)
	{
		int nUniqueObjectSelectionIdentifier = lSelectedItems.at(0)->data(0, Qt::UserRole).toInt(&bConvertDataResult);
		if((bConvertDataResult) && (nUniqueObjectSelectionIdentifier >= 0))
		{			 
			strcExperimentObjectInfo tmpObjectInfo = hashIntToExperimentObjectInfo.value(nUniqueObjectSelectionIdentifier);
			bool bShowWidgetResult = showObjectWidgets(tmpObjectInfo);
			if(bShowWidgetResult)
			{
				if(pObjectDeclarationWidget)
				{
					nCurrentObjectSelectionIdentifier = nUniqueObjectSelectionIdentifier;
					pObjectDeclarationWidget->setVisible(true);
					pObjectDeclarationWidget->setEnabled(true);
					ui->pbRemove->setEnabled(true);
					ui->pbInitMoveUp->setEnabled(true);
					ui->pbInitMoveDown->setEnabled(true);
					ui->pbFinitMoveUp->setEnabled(true);
					ui->pbFinitMoveDown->setEnabled(true);
					ui->tabWidget->setVisible(true);
					if(ui->lwObjectInitUsed->count() > 0)
						ui->lwObjectInitUsed->setCurrentRow(0);
					if(ui->lwObjectFinitUsed->count() > 0)
						ui->lwObjectFinitUsed->setCurrentRow(0);
					configureAvailableObjectInitFinits();
				}
				if(pObjectInitsWidget)
				{
					pObjectInitsWidget->setVisible(true);
					pObjectInitsWidget->setEnabled(true);
				}
				if(pObjectFinitsWidget)
				{
					pObjectFinitsWidget->setVisible(true);
					pObjectFinitsWidget->setEnabled(true);
				}
			}
		}
	}
}

void ExperimentObjectsDialog::prepareAndHideObjectConfiguration()
{
	nCurrentObjectSelectionIdentifier = -1;
	nCurrentObjectFinitSelectionIdentifier = -1;
	nCurrentObjectFinitSelectionIdentifier = -1;
	if(layoutObjectDeclarationTreeWidgetParent == NULL)
	{
		layoutObjectDeclarationTreeWidgetParent = new QHBoxLayout();
		layoutObjectDeclarationTreeWidgetParent->setContentsMargins(0,0,0,0);
		layoutObjectDeclarationTreeWidgetParent->setMargin(0);
		layoutObjectDeclarationTreeWidgetParent->setSpacing(0);
		layoutObjectDeclarationTreeWidgetParent->setObjectName(QStringLiteral("layoutObjectNameTreeWidgetParent"));
		ui->layObjectWidget->addLayout(layoutObjectDeclarationTreeWidgetParent);
	}
	if(layoutObjectInitTreeWidgetParent == NULL)
	{
		layoutObjectInitTreeWidgetParent = new QHBoxLayout();
		layoutObjectInitTreeWidgetParent->setContentsMargins(0,0,0,0);
		layoutObjectInitTreeWidgetParent->setMargin(0);
		layoutObjectInitTreeWidgetParent->setSpacing(0);
		layoutObjectInitTreeWidgetParent->setObjectName(QStringLiteral("layoutObjectInitTreeWidgetParent"));
		ui->layObjectInitWidget->addLayout(layoutObjectInitTreeWidgetParent);
	}
	if(layoutObjectFinitTreeWidgetParent == NULL)
	{
		layoutObjectFinitTreeWidgetParent = new QHBoxLayout();
		layoutObjectFinitTreeWidgetParent->setContentsMargins(0,0,0,0);
		layoutObjectFinitTreeWidgetParent->setMargin(0);
		layoutObjectFinitTreeWidgetParent->setSpacing(0);
		layoutObjectFinitTreeWidgetParent->setObjectName(QStringLiteral("layoutObjectFinitTreeWidgetParent"));
		ui->layObjectFinitWidget->addLayout(layoutObjectFinitTreeWidgetParent);
	}
	if(pObjectDeclarationWidget)
	{
		pObjectDeclarationWidget->setVisible(false);
		pObjectDeclarationWidget->setEnabled(false);
		ui->pbRemove->setEnabled(false);
		ui->pbInitMoveUp->setEnabled(false);
		ui->pbInitMoveDown->setEnabled(false);
		ui->pbFinitMoveUp->setEnabled(false);
		ui->pbFinitMoveDown->setEnabled(false);
		ui->tabWidget->setVisible(false);
	}
	if(pObjectInitsWidget)
	{
		pObjectInitsWidget->setVisible(false);
		pObjectInitsWidget->setEnabled(false);
	}
	if(pObjectFinitsWidget)
	{
		pObjectFinitsWidget->setVisible(false);
		pObjectFinitsWidget->setEnabled(false);
	}
}

bool ExperimentObjectsDialog::showObjectWidgets(const strcExperimentObjectInfo &objectInfoStrc)
{
	ui->lwObjectInitUsed->clear();
	ui->lwObjectFinitUsed->clear();
	ui->cmbObjectInitAvailable->clear();
	ui->cmbObjectFinitAvailable->clear();
	ui->lwObjectInitUsed->setEnabled(false);
	ui->lwObjectFinitUsed->setEnabled(false);

	if(pExpParamWidgets == NULL)
		pExpParamWidgets = PropertySettingsWidgetContainer::instance();
	if(pObjectDeclarationWidget == NULL)
	{
		pObjectDeclarationWidget = pExpParamWidgets->getExperimentParameterWidget(DECLARATIONS_OBJECT_TAG);
		if(pObjectDeclarationWidget)
		{
			if(layoutObjectDeclarationTreeWidgetParent)
				layoutObjectDeclarationTreeWidgetParent->insertWidget(0,pObjectDeclarationWidget);
			connect((QObject*)pObjectDeclarationWidget, SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(experimentObjectDefinitionItemEditFinished(const QString&, const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
		}
		else
		{
			qDebug() << __FUNCTION__ << "Could not fetch the Object info widget.";
			return false;
		}
	}

	if(objectInfoStrc.pObjectStruct)
	{
		pObjectDeclarationWidget->setParameter(NAME_TAG,objectInfoStrc.pObjectStruct->getObjectName(),true,true);
		pObjectDeclarationWidget->setParameter(CLASS_TAG,objectInfoStrc.pObjectStruct->getObjectClass(),true,true);

		for(int nInitFinitType=0;nInitFinitType<2;nInitFinitType++)
		{
			QList<cMethodStructure*> *pCurrentObjectInitFinitList = NULL;
			QListWidget *lwCurrentWidget = NULL;
			if(nInitFinitType == 0)//Initializations
			{
				pCurrentObjectInitFinitList = objectInfoStrc.pObjectInitializationList;
				lwCurrentWidget = ui->lwObjectInitUsed;
			}
			else//Finalizations
			{
				pCurrentObjectInitFinitList = objectInfoStrc.pObjectFinalizationList;
				lwCurrentWidget = ui->lwObjectFinitUsed;
			}

			if((pCurrentObjectInitFinitList) && (pCurrentObjectInitFinitList->isEmpty() == false))
			{
				if(nInitFinitType == 0)//Initializations
				{
					ui->gbDefinedInits->setVisible(true);
					ui->pbInitRemove->setEnabled(true);
					ui->pbInitConfigureParameters->setEnabled(true);
				}
				else//Finalizations
				{
					ui->gbDefinedFinits->setVisible(true);
					ui->pbFinitRemove->setEnabled(true);
					ui->pbFinitConfigureParameters->setEnabled(true);
				}
				int nIndexForItem = 0;
				foreach(cMethodStructure* pTmpMethStruct,*pCurrentObjectInitFinitList)
				{
					QListWidgetItem* pTmpListWidgetItem = new QListWidgetItem();
					pTmpListWidgetItem->setText(cMethodStructure::methodTypeToString(pTmpMethStruct->getMethodType()) + ": " + pTmpMethStruct->getMethodSignature());
					pTmpListWidgetItem->setData(Qt::UserRole, pTmpMethStruct->getMethodID());
					lwCurrentWidget->insertItem(nIndexForItem, pTmpListWidgetItem);
					nIndexForItem++;
				}
				lwCurrentWidget->setEnabled(true);
			}
			else
			{
				if(nInitFinitType == 0)//Initializations
				{
					ui->gbDefinedInits->setVisible(false);
					ui->pbInitRemove->setEnabled(false);
				}
				else//Finalizations
				{
					ui->gbDefinedFinits->setVisible(false);
					ui->pbFinitRemove->setEnabled(false);
				}
			}
		}
	}
	return true;
}

void ExperimentObjectsDialog::experimentObjectInitItemEditFinished(const QString &sParamName, const QString &sNewValue)
{
	handleExperimentObjectInitFinitEditFinished(sParamName, sNewValue, true);
}

void ExperimentObjectsDialog::experimentObjectFinitItemEditFinished(const QString &sParamName, const QString &sNewValue)
{
	handleExperimentObjectInitFinitEditFinished(sParamName, sNewValue, false);
}

void ExperimentObjectsDialog::handleExperimentObjectInitFinitEditFinished(const QString &sParamName, const QString &sNewValue, const bool bIsInit)
{
	if(bIsParsing || bDisableEditUpdates || bIsObjectDeleting)
		return;

	if(hashIntToExperimentObjectInfo.contains(nCurrentObjectSelectionIdentifier))
	{
		int nInitFinitSpecifierID = -1;
		cMethodStructure *pTempSelectedMethodStruct = NULL;
		QList<cMethodStructure*> *pTempSelectedObjectMethods = NULL;
		if(bIsInit)
		{
			nInitFinitSpecifierID = nCurrentObjectInitSelectionIdentifier;
			pTempSelectedObjectMethods = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectInitializationList;
		}
		else
		{
			nInitFinitSpecifierID = nCurrentObjectFinitSelectionIdentifier;
			pTempSelectedObjectMethods = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectFinalizationList;
		}
		if(nInitFinitSpecifierID<0)
			return;
		if(pTempSelectedObjectMethods)
		{
			foreach(cMethodStructure *pTempMethodStruct,*pTempSelectedObjectMethods)
			{
				if(pTempMethodStruct->getMethodID() == nInitFinitSpecifierID)
				{
					pTempSelectedMethodStruct = pTempMethodStruct;
					break;
				}
			}
		}
		else
		{
			return;
		}
		if(pTempSelectedMethodStruct==NULL)
			return;

		ExperimentTreeModel::strcObjectInitFinitSpecifier cObjectInitFinitSpecifier;
		cObjectInitFinitSpecifier.nObjectID = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).nObjectID;
		cObjectInitFinitSpecifier.nInitFinitID = nInitFinitSpecifierID;
		cObjectInitFinitSpecifier.sSignature = pTempSelectedMethodStruct->getMethodSignature();
		cObjectInitFinitSpecifier.sType = cMethodStructure::methodTypeToString(pTempSelectedMethodStruct->getMethodType());
		//?? pTempSelectedMethodStruct->getMethodParameterList()
		cObjectInitFinitSpecifier.sObjectDefinitionName = sParamName;

		QString sInitFinitString;
		if(bIsInit)
			sInitFinitString = "initialization";
		else
			sInitFinitString = "finalization";

		if(pExperimentTreeModel)
		{
			if(pExperimentTreeModel->updateExperimentObjectInitFinitParameter(cObjectInitFinitSpecifier, sNewValue, bIsInit) == false)
			{
				qDebug() << __FUNCTION__ << "Could not add/update the new object" << sInitFinitString;
			}
		}
	}
	return;
}

void ExperimentObjectsDialog::experimentObjectDefinitionItemEditFinished(const QString &sParamName, const QString &sNewValue)
{
	if(bIsParsing || bDisableEditUpdates || bIsObjectDeleting)
		return;
	if(hashIntToExperimentObjectInfo.contains(nCurrentObjectSelectionIdentifier))
	{
		ExperimentTreeModel::strcObjectSpecifier cObjectSpecifier;
		cObjectSpecifier.nObjectID = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).nObjectID;
		cObjectSpecifier.sObjectName = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectStruct->getObjectName();
		cObjectSpecifier.sObjectClass = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectStruct->getObjectClass();
		cObjectSpecifier.sObjectDefinitionName = sParamName;
		if(pExperimentTreeModel)
		{
			if(pExperimentTreeModel->updateExperimentObject(cObjectSpecifier, sNewValue) == false)
			{
				qDebug() << __FUNCTION__ << "Could not add/update the new object configuration";
			}
		}
	}
	return;
}

void ExperimentObjectsDialog::onConfigureSelectedObjectInitParams()
{
	configureSelectedObjectInitFinitParam(true);
}

void ExperimentObjectsDialog::onConfigureSelectedObjectFinitParams()
{
	configureSelectedObjectInitFinitParam(false);
}

void ExperimentObjectsDialog::onRemoveSelectedObjectInit()
{
	removeSelectedObjectInitFinit(true);
}

void ExperimentObjectsDialog::onRemoveSelectedObjectFinit()
{
	removeSelectedObjectInitFinit(false);
}	

void ExperimentObjectsDialog::removeSelectedObjectInitFinit(const bool &bIsInit)
{
	int nInitFinitIdentifier = -1;
	if(bIsInit)
	{
		if(nCurrentObjectInitSelectionIdentifier<0)
			return;
		ui->pbInitRemove->setEnabled(false);
		ui->pbInitConfigureParameters->setEnabled(false);
		nInitFinitIdentifier = nCurrentObjectInitSelectionIdentifier;
	}
	else
	{
		if(nCurrentObjectFinitSelectionIdentifier<0)
			return;
		ui->pbFinitRemove->setEnabled(false);
		ui->pbFinitConfigureParameters->setEnabled(true);
		nInitFinitIdentifier = nCurrentObjectFinitSelectionIdentifier;
	}

	if(pExperimentTreeModel)
	{
		if(pExperimentTreeModel->removeExperimentObjectInitFinit(nInitFinitIdentifier,bIsInit) == false)
		{
			qDebug() << __FUNCTION__ << "Could not remove the selected object method";
		}
	}
	//if(bIsInit)
	//	ui->pbInitRemove->setEnabled(true);
	//else
	//	ui->pbFinitRemove->setEnabled(true);
}

void ExperimentObjectsDialog::configureSelectedObjectInitFinitParam(const bool &bIsInit)
{
	int nInitFinitIdentifier = -1;
	if (bIsInit)
	{
		if (nCurrentObjectInitSelectionIdentifier < 0)
			return;
		//ui->pbInitConfigureParameters->setEnabled(false);
		nInitFinitIdentifier = nCurrentObjectInitSelectionIdentifier;
	}
	else
	{
		if (nCurrentObjectFinitSelectionIdentifier < 0)
			return;
		//ui->pbFinitConfigureParameters->setEnabled(false);
		nInitFinitIdentifier = nCurrentObjectFinitSelectionIdentifier;
	}
	ExperimentMethodParamsConfigurationDialog *tmpExperimentMethodParamsConfigurationDialog = new ExperimentMethodParamsConfigurationDialog(this);
	tmpExperimentMethodParamsConfigurationDialog->setInitFinitParameter(nInitFinitIdentifier, bIsInit);
	if (tmpExperimentMethodParamsConfigurationDialog->parseExperimentTreeModel(pExperimentTreeModel))
	{
		int returnVal = tmpExperimentMethodParamsConfigurationDialog->exec();
		switch (returnVal)
		{
		case QDialog::Accepted:
			break;
		case QDialog::Rejected:
			break;
		default:
			break;
		}
	}
}

void ExperimentObjectsDialog::removeSelectedObject()
{
	if(bIsObjectDeleting)
		return;
	bIsObjectDeleting = true;
	if(hashIntToExperimentObjectInfo.contains(nCurrentObjectSelectionIdentifier))
	{
		if(pExperimentTreeModel)
		{
			if (pExperimentTreeModel->removeExperimentObject(hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).nObjectID)==false)
				qDebug() << __FUNCTION__ << "Could not remove the selected object";
		}
	}
	bIsObjectDeleting = false;
}

void ExperimentObjectsDialog::newObjectConfigurationChanged(int nChangedIndex)
{
	Q_UNUSED(nChangedIndex);
}

void ExperimentObjectsDialog::checkReparseModel()
{
	bool bIsInit = (ui->tabWidget->currentWidget() == ui->tab_Initializations);
	bool bIsFinit = (ui->tabWidget->currentWidget() == ui->tab_Finalizations);
	int nLastSelectedInitMethodRow = ui->lwObjectInitUsed->currentRow();
	int nLastSelectedFinitMethodRow = ui->lwObjectFinitUsed->currentRow();
	reCreateAndParseExperimentStructure();
	if(pCurrentExpStructure->getObjectCount() <= 0)
		twCurrentObjects->clear();//invokes onObjectSelectionChanged();
	else
		onObjectSelectionChanged();
	if ((bIsInit) && (nLastSelectedInitMethodRow < ui->lwObjectInitUsed->count()))
		ui->lwObjectInitUsed->setCurrentItem(ui->lwObjectInitUsed->item(nLastSelectedInitMethodRow));
	if ((bIsFinit) && (nLastSelectedFinitMethodRow < ui->lwObjectFinitUsed->count()))
		ui->lwObjectFinitUsed->setCurrentItem(ui->lwObjectFinitUsed->item(nLastSelectedFinitMethodRow));
}

bool ExperimentObjectsDialog::setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel)
{
	if(pExpTreeModel == NULL)
		return false;
	bIsParsing = true;
	clearAllParsedStructures();
	pExperimentTreeModel = pExpTreeModel;
	bool bResult = connect(pExpTreeModel, SIGNAL(modelModified()), this, SLOT(checkReparseModel()),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
	bResult = reCreateAndParseExperimentStructure();
	bIsParsing = false;
	return bResult;
}

bool ExperimentObjectsDialog::reCreateAndParseExperimentStructure()
{
	QList<ExperimentTreeItem*> tmpFoundExpTreeItemList;
	if(pCurrentExpStructure)
	{
		delete pCurrentExpStructure;
		pCurrentExpStructure = NULL;
	}
	pCurrentExpStructure = new cExperimentStructure();
	bool bResult = ExperimentManager::createExperimentStructure(tmpFoundExpTreeItemList,pExperimentTreeModel,pCurrentExpStructure);
	if(bResult)
	{	
		return parseExperimentStructure();
	}
	return false;
}

bool ExperimentObjectsDialog::parseExperimentStructure() 
{
	bool bRetval = false;
	int nExpObjectCount = pCurrentExpStructure->getObjectCount();
	if(nExpObjectCount > 0)
	{
		QList<cObjectStructure *> pObjectStructureList = pCurrentExpStructure->getObjectList();
		int nUniqueSelectedObjectIdentifier = 0;
		QString sMainItemString;
		QString sSubItemString;
		QList<QTreeWidgetItem *> lProcessedObjectTreeWidgetItems;

		for (int nObjectCounter=0;nObjectCounter<pObjectStructureList.count();nObjectCounter++)
		{
			if(pObjectStructureList.at(nObjectCounter)) 
			{
				sMainItemString = "Object: " + pObjectStructureList.at(nObjectCounter)->getObjectName();//(Class: " + pObjectStructureList.at(nObjectCounter)->getObjectClass() + "), " + 
				//do not change the below, because it's used for the view sorting
				sSubItemString = pObjectStructureList.at(nObjectCounter)->getObjectName() + ": " + pObjectStructureList.at(nObjectCounter)->getObjectClass() + ": " + QString::number(pObjectStructureList.at(nObjectCounter)->getObjectID());		
				bool bTreeItemAlreadyPresent = false;
				strcExperimentObjectInfo *pObjectInfo = NULL;
				QHash<int, strcExperimentObjectInfo>::iterator iterObjectLoop = hashIntToExperimentObjectInfo.begin();
				while (iterObjectLoop != hashIntToExperimentObjectInfo.end()) 
				{
					if(iterObjectLoop.value().nObjectID == pObjectStructureList.at(nObjectCounter)->getObjectID())
					{
						bTreeItemAlreadyPresent = true;
						break;
					}
					++iterObjectLoop;
				}
				if(bTreeItemAlreadyPresent)
				{
					pObjectInfo = &iterObjectLoop.value();
					pObjectInfo->pObjectStruct = pObjectStructureList.at(nObjectCounter);
				}
				else
				{
					pObjectInfo = new strcExperimentObjectInfo();
					pObjectInfo->nObjectID = pObjectStructureList.at(nObjectCounter)->getObjectID();
					pObjectInfo->pObjectStruct = pObjectStructureList.at(nObjectCounter);
				}
				pObjectInfo->pObjectInitializationList = pCurrentExpStructure->getObjectInitializationList(pObjectInfo->nObjectID);
				pObjectInfo->pObjectFinalizationList = pCurrentExpStructure->getObjectFinalizationList(pObjectInfo->nObjectID);

				if((bTreeItemAlreadyPresent == false) || (pObjectInfo->pObjectTreeWidgetItem == NULL))
				{
					pObjectInfo->pObjectTreeWidgetItem = new QTreeWidgetItem((QTreeWidget*)NULL, QStringList(sMainItemString));
					while(hashIntToExperimentObjectInfo.keys().contains(nUniqueSelectedObjectIdentifier))
						nUniqueSelectedObjectIdentifier++;
					pObjectInfo->pObjectTreeWidgetItem->setData(0, Qt::UserRole, nUniqueSelectedObjectIdentifier);
					hashIntToExperimentObjectInfo.insert(nUniqueSelectedObjectIdentifier,*pObjectInfo);
				}
				else
				{
					pObjectInfo->pObjectTreeWidgetItem->setText(0,sMainItemString);
				}
				lProcessedObjectTreeWidgetItems.append(pObjectInfo->pObjectTreeWidgetItem);
				if(pObjectInfo->pObjectTreeWidgetItem)
				{
					mapObjectIdToObjectTreeWidgetItems.insert(pObjectInfo->nObjectID, pObjectInfo->pObjectTreeWidgetItem);
				}
			}
		}
		//Update the hash/map(s) in case of removal
		if(lProcessedObjectTreeWidgetItems.count() < hashIntToExperimentObjectInfo.count())
		{
			QMap<int, QTreeWidgetItem *>::iterator iterTreeItem = mapObjectIdToObjectTreeWidgetItems.begin();
			while (iterTreeItem != mapObjectIdToObjectTreeWidgetItems.end()) 
			{
				if(lProcessedObjectTreeWidgetItems.contains(iterTreeItem.value()) == false)
				{
					QHash<int, strcExperimentObjectInfo>::iterator iterBlockLoop = hashIntToExperimentObjectInfo.begin();
					while (iterBlockLoop != hashIntToExperimentObjectInfo.end()) 
					{
						if(iterBlockLoop->pObjectTreeWidgetItem == iterTreeItem.value())
						{
							for(int nTopLevelItemCounter=0;nTopLevelItemCounter<twCurrentObjects->topLevelItemCount();nTopLevelItemCounter++)
							{
								if(twCurrentObjects->topLevelItem(nTopLevelItemCounter) == iterBlockLoop->pObjectTreeWidgetItem)
								{
									twCurrentObjects->takeTopLevelItem(nTopLevelItemCounter);
									break;
								}
							}
							hashIntToExperimentObjectInfo.remove(iterBlockLoop.key());
							break;
						}
						++iterBlockLoop;
					}
					mapObjectIdToObjectTreeWidgetItems.remove(iterTreeItem.key());
					break;
				}
				++iterTreeItem;
			}
		}
		twCurrentObjects->headerItem()->setText(0, "Declared Objects");
		twCurrentObjects->insertTopLevelItems(0, mapObjectIdToObjectTreeWidgetItems.values());
	}
	else
	{
		hashIntToExperimentObjectInfo.clear();
		mapObjectIdToObjectTreeWidgetItems.clear();
	}
	bRetval = true;
	return bRetval;
}

void ExperimentObjectsDialog::addSelectedInit()
{
	addInitFinit(true);
}

void ExperimentObjectsDialog::addSelectedFinit()
{
	addInitFinit(false);
}

bool ExperimentObjectsDialog::addInitFinit(const bool &bIsInit)
{
	bool bResult = false;
	QString sNewSignature;
	if(bIsInit)
	{
		ui->pbInitAdd->setEnabled(false);
		sNewSignature = ui->cmbObjectInitAvailable->currentText();
	}
	else
	{
		ui->pbFinitAdd->setEnabled(false);
		sNewSignature = ui->cmbObjectFinitAvailable->currentText();
	}
	if(pExperimentTreeModel && (sNewSignature.isEmpty()==false) && (nCurrentObjectSelectionIdentifier>=0))
	{
		int nLeftBracePos = sNewSignature.indexOf("(");
		int nRightBracePos = sNewSignature.indexOf(")");
		QStringList lParamsList;
		if((nLeftBracePos>1) && (nRightBracePos>(nLeftBracePos+1)))
			lParamsList = sNewSignature.mid(nLeftBracePos+1,nRightBracePos-nLeftBracePos-1).split(",",QString::SkipEmptyParts);
		QString sInitFinitString;
		//int nInitFinitSpecifierID = 0;
		strcExperimentObjectInfo tmpObjectInfo = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier);
		ExperimentTreeModel::strcObjectInitFinitSpecifier tmpObjectInitFinitSpecifier;
		//QList<cMethodStructure*> *pTempInitFinitMethStrcList = NULL;
		if(bIsInit)
		{
			//nInitFinitSpecifierID = nCurrentObjectInitSelectionIdentifier;
			sInitFinitString = "initialization";
			//pTempInitFinitMethStrcList = tmpObjectInfo.pObjectInitializationList;
		}
		else
		{
			//nInitFinitSpecifierID = nCurrentObjectFinitSelectionIdentifier;
			sInitFinitString = "finalization";
			//pTempInitFinitMethStrcList = tmpObjectInfo.pObjectFinalizationList;
		}
		//if(pTempInitFinitMethStrcList)
		//{
		//	foreach(cMethodStructure* pTmpMethStrc,*pTempInitFinitMethStrcList)
		//	{
		//		if(pTmpMethStrc->getMethodID() >= nInitFinitSpecifierID)
		//			nInitFinitSpecifierID = pTmpMethStrc->getMethodID()+1;
		//	}
		//}
		tmpObjectInitFinitSpecifier.nInitFinitID = 0;//optional, not needed here (can be commented) see below;
		tmpObjectInitFinitSpecifier.nObjectID = nCurrentObjectSelectionIdentifier;
		tmpObjectInitFinitSpecifier.sObjectDefinitionName = "";
		tmpObjectInitFinitSpecifier.sSignature = sNewSignature.left(nLeftBracePos);
		tmpObjectInitFinitSpecifier.sType = METHOD_TYPE_SLOT_TAG;
		tmpObjectInitFinitSpecifier.nOrderNumber = -1;//use next available number

		ExperimentTreeModel::strcObjectInitFinitParameterSpecifier tmpObjectInitFinitParam;
		for(int nParamCounter=0;nParamCounter<lParamsList.count();nParamCounter++)
		{
			QStringList tmpSplittedParamDefList = lParamsList.at(nParamCounter).simplified().split(" ",QString::SkipEmptyParts);
			tmpObjectInitFinitParam.nParamID = nParamCounter;
			tmpObjectInitFinitParam.sName = tmpSplittedParamDefList.last();//"variable_" + QString::number(nParamCounter);
			tmpObjectInitFinitParam.sType = tmpSplittedParamDefList.at(0);
			tmpObjectInitFinitParam.sValue = "";
			tmpObjectInitFinitSpecifier.lParameters.append(tmpObjectInitFinitParam);
		}

		if(pExperimentTreeModel->addExperimentObjectInitFinit(tmpObjectInitFinitSpecifier, bIsInit, -1) == false)
		{
			qDebug() << __FUNCTION__ << "Could not add the new" << sInitFinitString << "method";
		}
		else
		{
			bResult = true;
			//onObjectSelectionChanged();
		}
	}
	if(bIsInit)
		ui->pbInitAdd->setEnabled(true);
	else
		ui->pbFinitAdd->setEnabled(true);
	return bResult;
}

void ExperimentObjectsDialog::moveUpSelectedInit()
{
	moveSelectedInitFinit(true, true);
}

void ExperimentObjectsDialog::moveDownSelectedInit()
{
	moveSelectedInitFinit(false, true);
}

void ExperimentObjectsDialog::moveUpSelectedFinit()
{
	moveSelectedInitFinit(true, false);
}

void ExperimentObjectsDialog::moveDownSelectedFinit()
{
	moveSelectedInitFinit(false, false);
}

bool ExperimentObjectsDialog::moveSelectedInitFinit(const bool &bMoveUp, const bool &bIsInit)
{
	QListWidgetItem *lwChangedSelectedItem = NULL;
	QListWidget *InitFinitListWidget = NULL;
	int *pSelectionIdentifier;
	int nTotalItems;
	int nCurrentIndex;
	bool bResult = false;

	if (pExperimentTreeModel == NULL)
		return false;
	if (bIsInit)
	{
		InitFinitListWidget = ui->lwObjectInitUsed;
		pSelectionIdentifier = &nCurrentObjectInitSelectionIdentifier;
	}
	else
	{
		InitFinitListWidget = ui->lwObjectFinitUsed;
		pSelectionIdentifier = &nCurrentObjectFinitSelectionIdentifier;
	}
	lwChangedSelectedItem = InitFinitListWidget->currentItem();
	nTotalItems = InitFinitListWidget->count();
	nCurrentIndex = InitFinitListWidget->currentRow();

	if (nTotalItems < 2)
		return false;
	if (bMoveUp)	//Up
	{
		if (nCurrentIndex == 0)//First index?
			return false;
		int nIndexToSwitch = InitFinitListWidget->item(nCurrentIndex - 1)->data(Qt::UserRole).toInt();
		bResult = pExperimentTreeModel->moveExperimentObjectInitFinit(QList<int>() << *pSelectionIdentifier, nIndexToSwitch, -1, bIsInit);
		if (bResult)
			InitFinitListWidget->setCurrentRow(nCurrentIndex - 1);
	}
	else			//Down
	{
		if (nCurrentIndex == (nTotalItems - 1))//Last index?
			return false;
		int nIndexToSwitch = InitFinitListWidget->item(nCurrentIndex + 1)->data(Qt::UserRole).toInt();
		bResult = pExperimentTreeModel->moveExperimentObjectInitFinit(QList<int>() << *pSelectionIdentifier, nIndexToSwitch, 1, bIsInit);
		if (bResult)
			InitFinitListWidget->setCurrentRow(nCurrentIndex + 1);
	}
	return bResult;
}

void ExperimentObjectsDialog::addSelectedObject()
{
	ui->pbAdd->setEnabled(false);
	if(pExperimentTreeModel)
	{
		QString sObjectName = ui->leObjectName->text();
		QString sClassName = ui->cmbObjects->itemData(ui->cmbObjects->currentIndex(),Qt::UserRole).toString();
		if(pExperimentTreeModel->addExperimentObject(sObjectName, sClassName, -1) == false)
		{
			qDebug() << __FUNCTION__ << "Could not add the new object";
		}
		else
		{
			onObjectSelectionChanged();
		}
	}
	ui->pbAdd->setEnabled(true);
}

void ExperimentObjectsDialog::onObjectInitSelectionChanged()
{
	bDisableEditUpdates = true;
	handleObjectInitFinitSelectionChanged(true);
	bDisableEditUpdates = false;
}

void ExperimentObjectsDialog::onObjectFinitSelectionChanged()
{
	bDisableEditUpdates = true;
	handleObjectInitFinitSelectionChanged(false);
	bDisableEditUpdates = false;
}

void ExperimentObjectsDialog::handleObjectInitFinitSelectionChanged(bool bIsInit)
{
	QListWidgetItem *lwChangedSelectedItem = NULL;
	int *pSelectionIdentifier;
	if(bIsInit)
	{
		lwChangedSelectedItem = ui->lwObjectInitUsed->currentItem();
		pSelectionIdentifier = &nCurrentObjectInitSelectionIdentifier;
	}
	else
	{
		lwChangedSelectedItem = ui->lwObjectFinitUsed->currentItem();
		pSelectionIdentifier = &nCurrentObjectFinitSelectionIdentifier;
	}
	if(lwChangedSelectedItem)
		*pSelectionIdentifier = lwChangedSelectedItem->data(Qt::UserRole).toInt();
	else
		*pSelectionIdentifier = -1;

	if(((ui->lwObjectInitUsed->isEnabled() == false) && bIsInit) || ((ui->lwObjectFinitUsed->isEnabled() == false) && (bIsInit == false)))
		return;
	int nObjectID = -1;
	if(hashIntToExperimentObjectInfo.contains(nCurrentObjectSelectionIdentifier))
		nObjectID = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).nObjectID;
	if(nObjectID<0)
		return;

	QListWidget *lwCurrentWidget = NULL;
	QHBoxLayout *layoutObjectInitFinitTreeWidgetParent = NULL;
	QString sParamWidgetTag;
	QString sInitFinitString;
	if(bIsInit)
	{
		lwCurrentWidget = ui->lwObjectInitUsed;
		pObjectInitFinitWidget = &pObjectInitsWidget;
		sParamWidgetTag = INITIALIZATION_OBJECT_TAG;
		layoutObjectInitFinitTreeWidgetParent = layoutObjectInitTreeWidgetParent;
		sInitFinitString = "initialization";
	}
	else
	{
		lwCurrentWidget = ui->lwObjectFinitUsed;
		pObjectInitFinitWidget = &pObjectFinitsWidget;
		sParamWidgetTag = FINALIZATION_OBJECT_TAG;
		layoutObjectInitFinitTreeWidgetParent = layoutObjectFinitTreeWidgetParent;
		sInitFinitString = "finalization";
	}

	if(*pObjectInitFinitWidget == NULL)
	{
		*pObjectInitFinitWidget = pExpParamWidgets->getExperimentParameterWidget(sParamWidgetTag);
		if(*pObjectInitFinitWidget)
		{
			(*pObjectInitFinitWidget)->configurePropertyEditSignaling(false);
			if(layoutObjectInitFinitTreeWidgetParent)
				layoutObjectInitFinitTreeWidgetParent->insertWidget(0,*pObjectInitFinitWidget);
			if(bIsInit)
				connect((QObject*)*pObjectInitFinitWidget, SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(experimentObjectInitItemEditFinished(const QString&, const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
			else
				connect((QObject*)*pObjectInitFinitWidget, SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(experimentObjectFinitItemEditFinished(const QString&, const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
		}
		else
		{
			qDebug() << __FUNCTION__ << "Could not fetch the Object " << sInitFinitString << " widget.";
			return;
		}
	}

	(*pObjectInitFinitWidget)->setParameter(OBJECT_TAG,QString::number(nObjectID),true,true);
	//Remove already defined arguments 
	int nAddParamCntr=3;
	int nParamIndexToRemove = nAddParamCntr;
	while((*pObjectInitFinitWidget)->removeParameterProperty(QString::number(nParamIndexToRemove)))
		nParamIndexToRemove++;

	QList<cMethodParameterStructure*> tmpMethodParamStrcList;
	QList<cMethodStructure*> *pCurrentObjectMethodStructList = NULL;
	if(bIsInit)
		pCurrentObjectMethodStructList = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectInitializationList;
	else
		pCurrentObjectMethodStructList = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectFinalizationList;

	if((pCurrentObjectMethodStructList) && (pCurrentObjectMethodStructList->isEmpty() == false))
	{
		cMethodStructure* pCurrentObjectMethodStruct = NULL;
		for(int i=0;i<pCurrentObjectMethodStructList->count();i++)
		{
			if(lwCurrentWidget)
			{
				QListWidgetItem *tmpListWidgetItem = lwCurrentWidget->currentItem();
				if (tmpListWidgetItem)
				{
					if (pCurrentObjectMethodStructList->at(i)->getMethodID() == tmpListWidgetItem->data(Qt::UserRole).toInt())
					{
						pCurrentObjectMethodStruct = pCurrentObjectMethodStructList->at(i);
						break;
					}
				}
			}
		}
		if(pCurrentObjectMethodStruct)
		{
			(*pObjectInitFinitWidget)->setParameter(INIT_FINIT_SIGNATURE_TAG,pCurrentObjectMethodStruct->getMethodSignature(),true,true);
			(*pObjectInitFinitWidget)->setParameter(INIT_FINIT_TYPE_TAG,cMethodStructure::methodTypeToString(pCurrentObjectMethodStruct->getMethodType()),true,true);
			(*pObjectInitFinitWidget)->setParameter(INIT_FINIT_NUMBER_TAG, QString::number(pCurrentObjectMethodStruct->getMethodOrderNumber()), true, true);
			tmpMethodParamStrcList = pCurrentObjectMethodStruct->getMethodParameterList();
		}
		else
		{
			return;
		}
	}
	else
	{
		(*pObjectInitFinitWidget)->setParameter(INIT_FINIT_SIGNATURE_TAG,"",true,true);
		(*pObjectInitFinitWidget)->setParameter(INIT_FINIT_TYPE_TAG,"",true,true);
	}
	if(tmpMethodParamStrcList.isEmpty()==false)
	{
		mapPropertySettingDefinitionsTypesStrc mParamDefinitionsTypesMap;
		foreach(cMethodParameterStructure *pTmpExpParamStrc, tmpMethodParamStrcList)
		{
			//Add defined arguments
			PropertySettingDefinitionStrc *tmpExpParamDefStrc = new PropertySettingDefinitionStrc();
			tmpExpParamDefStrc->bCanBeScriptReference = false;
			tmpExpParamDefStrc->bEnabled = true;
			tmpExpParamDefStrc->bIsEditable = false;
			tmpExpParamDefStrc->eType = mParamDefinitionsTypesMap[pTmpExpParamStrc->getMethodParameterType().remove("const ", Qt::CaseInsensitive).toLower()];
			tmpExpParamDefStrc->nId = nAddParamCntr;
			tmpExpParamDefStrc->sDefaultValue = "0";
			tmpExpParamDefStrc->sName = pTmpExpParamStrc->getMethodParameterName();
			tmpExpParamDefStrc->sDisplayName = tmpExpParamDefStrc->sName;// + "(" + pTmpExpParamStrc->getMethodParameterType() + ")"
			tmpExpParamDefStrc->sGroupPath = "Arguments";
			tmpExpParamDefStrc->sInformation = "an Argument of Type " + pTmpExpParamStrc->getMethodParameterType();
			(*pObjectInitFinitWidget)->addParameterProperty(tmpExpParamDefStrc,pTmpExpParamStrc->getMethodParameterValue());
			nAddParamCntr++;
		}
	}
}

void ExperimentObjectsDialog::configureAvailableObjectInitFinits()
{
	ui->pbInitAdd->setEnabled(false);
	ui->cmbObjectInitAvailable->clear();
	ui->pbFinitAdd->setEnabled(false);
	ui->cmbObjectFinitAvailable->clear();	

	if(hashIntToExperimentObjectInfo.contains(nCurrentObjectSelectionIdentifier))
	{
		//cObjectSpecifier.nObjectID = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).nObjectID;
		//cObjectSpecifier.sObjectName = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectStruct->getObjectName();
		QString sClassName = hashIntToExperimentObjectInfo.value(nCurrentObjectSelectionIdentifier).pObjectStruct->getObjectClass();
		int metaID = QMetaType::type(sClassName.toLatin1());
		if (metaID > 0)
		{
			QObject *pObject = static_cast< QObject* > ( QMetaType::create(metaID) );//( QMetaType::construct(metaID) );
			const QMetaObject* metaObject = pObject->metaObject();
			QStringList lMetaSlots;
			QString sTmpString;
			//QStringList lMetaSignals;
			//checkConnectArgs
			for(int i = 0; i < metaObject->methodCount(); ++i)//metaObject->methodOffset()
			{
				if(metaObject->method(i).methodType()== QMetaMethod::Slot)
				{
					//metaObject->method(i).methodSignature());
					sTmpString = QString::fromLatin1(metaObject->method(i).name()) + "(";
					for (int n=0;n<metaObject->method(i).parameterCount();n++)
					{
						if(n>0)
							sTmpString = sTmpString + ",";
						sTmpString = sTmpString + metaObject->method(i).parameterTypes().at(n) + " " + metaObject->method(i).parameterNames().at(n);
					}
					sTmpString = sTmpString + ")";
					lMetaSlots << sTmpString;
				}
				//else if(metaObject->method(i).methodType()== QMetaMethod::Signal)
				//{
				//	lMetaSignals << QString::fromLatin1(metaObject->method(i).methodSignature());
				//}
			}
			if(lMetaSlots.isEmpty() == false)
			{
				lMetaSlots.sort(Qt::CaseInsensitive);
				ui->pbInitAdd->setEnabled(true);
				ui->cmbObjectInitAvailable->addItems(lMetaSlots);
				ui->pbFinitAdd->setEnabled(true);
				ui->cmbObjectFinitAvailable->addItems(lMetaSlots);
			}
		}
	}
}