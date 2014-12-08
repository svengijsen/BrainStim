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

#include "objectconnectionsdialog.h"
#include "ui_ObjectConnectionsDialog.h"
#include "experimentmanager.h"
#include "propertysettingswidgetcontainer.h"
#include "propertysettingswidget.h"

ObjectConnectionsDialog::ObjectConnectionsDialog(QWidget *parent) : QDialog(parent), bIsParsing(false), pCurrentExpStructure(NULL), twCurrentConnections(NULL), layoutTreeWidgetParent(NULL), pExpParamWidgets(NULL), pParametersWidget(NULL), pExperimentTreeModel(NULL), nCurrentObjectConnectionSelectionIdentifier(-1)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui = new Ui::ObjectConnectionsDialog();
	ui->setupUi(this);
	setWindowTitle("Configure Object Connection(s)");
	twCurrentConnections = new QTreeWidget(ui->groupBox_2);
	ui->layCurrentConnections->addWidget(twCurrentConnections);
	connect(twCurrentConnections,SIGNAL(itemSelectionChanged()), this, SLOT(onTreeWidgetSelectionChanged()));
	connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(addSelectedConnection()));
	connect(ui->pbRemove, SIGNAL(clicked()), this, SLOT(removeSelectedConnection()));
	connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->cmbFromMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(validatePossibleConnection()));
	connect(ui->cmbToMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(validatePossibleConnection()));
	connect(ui->cmbFromObject, SIGNAL(currentIndexChanged(int)), this, SLOT(fromObjectChanged(int)));
	connect(ui->cmbToObject, SIGNAL(currentIndexChanged(int)), this, SLOT(toObjectChanged(int)));
}

ObjectConnectionsDialog::~ObjectConnectionsDialog()
{
	clearAllParsedStructures();
	if(pExpParamWidgets)
		pExpParamWidgets = NULL;
	if(pParametersWidget)
	{
		pParametersWidget->setParent(NULL);
		if(pParametersWidget->isEnabled()==false)
			pParametersWidget->setEnabled(true);
		pParametersWidget = NULL;
	}
	if(layoutTreeWidgetParent)
	{
		delete layoutTreeWidgetParent;
		layoutTreeWidgetParent = NULL;
	}
	if(pExperimentTreeModel)
		pExperimentTreeModel = NULL;
	delete ui;
}

void ObjectConnectionsDialog::clearAllParsedStructures()
{
	if(pCurrentExpStructure)
	{
		delete pCurrentExpStructure;
		pCurrentExpStructure = NULL;
	}
	twCurrentConnections->clear();
	mapBlockIdToBlockTreeWidgetItems.clear();
	hashIntToObjectConnectionInfo.clear();
	hashFromComboMethodIndexToMetaMethod.clear();
	hashToComboMethodIndexToMetaMethod.clear();
	nCurrentObjectConnectionSelectionIdentifier = -1;
}

void ObjectConnectionsDialog::onTreeWidgetSelectionChanged()
{
	prepareAndHideObjectConnectionConfiguration();
	QList<QTreeWidgetItem *> lSelectedItems = twCurrentConnections->selectedItems();
	bool bConvertDataResult = false;
	if(lSelectedItems.count() == 1)
	{
		int nUniqueSelectionIdentifier = lSelectedItems.at(0)->data(0, Qt::UserRole).toInt(&bConvertDataResult);
		if((bConvertDataResult) && (nUniqueSelectionIdentifier >= 0))
		{			 
			strcObjectConnectionInfo tmpObjectConnectionInfo = hashIntToObjectConnectionInfo.value(nUniqueSelectionIdentifier);
			bool bShowWidgetResult = showConnectionWidget(tmpObjectConnectionInfo);
			if(pParametersWidget && bShowWidgetResult)
			{
				nCurrentObjectConnectionSelectionIdentifier = nUniqueSelectionIdentifier;
				pParametersWidget->setVisible(true);
				pParametersWidget->setEnabled(true);
				ui->pbRemove->setEnabled(true);
			}
			QTreeWidgetItem *pParentTreeWidgetItem = lSelectedItems.at(0)->parent();
			if(pParentTreeWidgetItem)
			{
				QString sUniqueBlockSelectionIdentifier = pParentTreeWidgetItem->data(0, Qt::UserRole).toString();
				if(sUniqueBlockSelectionIdentifier.isEmpty() == false)
					ui->cmbFromObject->setCurrentText(sUniqueBlockSelectionIdentifier);
			}
			return;
		}
		else//a object was selected?
		{
			QString sUniqueBlockSelectionIdentifier = lSelectedItems.at(0)->data(0, Qt::UserRole).toString();
			if(sUniqueBlockSelectionIdentifier.isEmpty() == false)
				ui->cmbFromObject->setCurrentText(sUniqueBlockSelectionIdentifier);
		}
	}
	return;
}

void ObjectConnectionsDialog::prepareAndHideObjectConnectionConfiguration()
{
	nCurrentObjectConnectionSelectionIdentifier = -1;
	if(layoutTreeWidgetParent == NULL)
	{
		layoutTreeWidgetParent = new QHBoxLayout();
		layoutTreeWidgetParent->setContentsMargins(0,0,0,0);
		layoutTreeWidgetParent->setMargin(0);
		layoutTreeWidgetParent->setSpacing(0);
		layoutTreeWidgetParent->setObjectName(QStringLiteral("layoutTreeWidgetParent"));
		ui->layConnectionWidget->addLayout(layoutTreeWidgetParent);
	}
	if(pParametersWidget)
	{
		pParametersWidget->setVisible(false);
		pParametersWidget->setEnabled(false);
		ui->pbRemove->setEnabled(false);
	}
}

bool ObjectConnectionsDialog::showConnectionWidget(const strcObjectConnectionInfo &ObjectConnectionInfoStrc)
{
	if(pExpParamWidgets == NULL)
		pExpParamWidgets = PropertySettingsWidgetContainer::instance();
	if(pParametersWidget == NULL)
	{
		pParametersWidget = pExpParamWidgets->getExperimentParameterWidget(CONNECTION_OBJECT_TAG);
		if(pParametersWidget)
		{
			if(layoutTreeWidgetParent)
				layoutTreeWidgetParent->insertWidget(0,pParametersWidget);
			connect((QObject*)pParametersWidget, SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(ObjectConnectionDefinitionItemEditFinished(const QString&, const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
		}
		else
		{
			qDebug() << __FUNCTION__ << "Could not fetch the Connection widget.";
			return false;
		}
	}
	if(ObjectConnectionInfoStrc.pConnectionStruct)
	{
		pParametersWidget->setParameter(CONNECTION_TARGET_TAG "/" OBJECT_TAG,QString::number(ObjectConnectionInfoStrc.pConnectionStruct->getTargetObjectID()),true,true);
		pParametersWidget->setParameter(CONNECTION_SOURCE_TAG "/" OBJECT_TAG,QString::number(ObjectConnectionInfoStrc.pConnectionStruct->getSourceObjectID()),true,true);
		pParametersWidget->setParameter(CONNECTION_TARGET_TAG "/" CONNECTION_SIGNATURE_TAG,ObjectConnectionInfoStrc.pConnectionStruct->getTargetSignature(),true,true);
		pParametersWidget->setParameter(CONNECTION_SOURCE_TAG "/" CONNECTION_SIGNATURE_TAG,ObjectConnectionInfoStrc.pConnectionStruct->getSourceSignature(),true,true);
		pParametersWidget->setParameter(CONNECTION_TARGET_TAG "/" CONNECTION_TYPE_TAG,cMethodStructure::methodTypeToString(ObjectConnectionInfoStrc.pConnectionStruct->getTargetMethodType()),true,true);
		pParametersWidget->setParameter(CONNECTION_SOURCE_TAG "/" CONNECTION_TYPE_TAG,cMethodStructure::methodTypeToString(ObjectConnectionInfoStrc.pConnectionStruct->getSourceMethodType()),true,true);
	}
	return true;
}

void ObjectConnectionsDialog::ObjectConnectionDefinitionItemEditFinished(const QString &sParamName, const QString &sNewValue)
{
	if(hashIntToObjectConnectionInfo.contains(nCurrentObjectConnectionSelectionIdentifier))
	{
		ExperimentTreeModel::strcObjectConnectionSpecifier cObjectConnectionSpecifier;
		cObjectConnectionSpecifier.nConnectionID = hashIntToObjectConnectionInfo.value(nCurrentObjectConnectionSelectionIdentifier).nConnectionID;
		cObjectConnectionSpecifier.sConnectionDefinitionName = sParamName;
		if(pExperimentTreeModel)
		{
			if(pExperimentTreeModel->updateObjectConnection(cObjectConnectionSpecifier, sNewValue) == false)
			{
				qDebug() << __FUNCTION__ << "Could not add/update the new block Connection configuration";
			}
		}
	}
	return;
}

void ObjectConnectionsDialog::removeSelectedConnection()
{
	ui->pbRemove->setEnabled(false);
	if(hashIntToObjectConnectionInfo.contains(nCurrentObjectConnectionSelectionIdentifier))
	{
		if(pExperimentTreeModel)
		{
			if(pExperimentTreeModel->removeObjectConnection(hashIntToObjectConnectionInfo.value(nCurrentObjectConnectionSelectionIdentifier).nConnectionID) == false)
			{
				qDebug() << __FUNCTION__ << "Could not add/update the new block Connection configuration";
			}
		}
	}
	ui->pbRemove->setEnabled(true);
}

void ObjectConnectionsDialog::fromObjectChanged(int nChangedIndex)
{
	ui->cmbFromMethod->clear();
	hashFromComboMethodIndexToMetaMethod.clear();
	cObjectStructure *tmpObjectStruct = hashComboObjectIndexToObjectStructure.value(nChangedIndex).pObjectStruct;
	if(tmpObjectStruct)
	{
		QStringList lMethods = getObjectMethodsList(tmpObjectStruct->getObjectClass(),hashFromComboMethodIndexToMetaMethod);
		for(int i=0;i<lMethods.count();i++)
		{
			if(hashFromComboMethodIndexToMetaMethod[i].methodType()== QMetaMethod::Signal)
				ui->cmbFromMethod->addItem(QIcon(":/resources/signal.png"), lMethods.at(i));
			else if(hashFromComboMethodIndexToMetaMethod[i].methodType()== QMetaMethod::Slot)
				ui->cmbFromMethod->addItem(QIcon(":/resources/slot.png"), lMethods.at(i));
			else
				ui->cmbFromMethod->addItem(lMethods.at(i));
		}
	}
}

void ObjectConnectionsDialog::toObjectChanged(int nChangedIndex)
{
	ui->cmbToMethod->clear();
	hashToComboMethodIndexToMetaMethod.clear();
	cObjectStructure *tmpObjectStruct = hashComboObjectIndexToObjectStructure.value(nChangedIndex).pObjectStruct;
	if(tmpObjectStruct)
	{
		QStringList lMethods = getObjectMethodsList(tmpObjectStruct->getObjectClass(),hashToComboMethodIndexToMetaMethod);
		for(int i=0;i<lMethods.count();i++)
		{
			if(hashToComboMethodIndexToMetaMethod[i].methodType()== QMetaMethod::Signal)
				ui->cmbToMethod->addItem(QIcon(":/resources/signal.png"), lMethods.at(i));
			else if(hashToComboMethodIndexToMetaMethod[i].methodType()== QMetaMethod::Slot)
				ui->cmbToMethod->addItem(QIcon(":/resources/slot.png"), lMethods.at(i));
			else
				ui->cmbToMethod->addItem(lMethods.at(i));
		}
	}
}

void ObjectConnectionsDialog::validatePossibleConnection()
{
	ui->pbAdd->setEnabled(false);
	if((ui->cmbFromMethod->currentText().isEmpty() == false) && (ui->cmbToMethod->currentText().isEmpty() == false))
	{
		if(QMetaObject::checkConnectArgs(hashFromComboMethodIndexToMetaMethod[ui->cmbFromMethod->currentIndex()],hashToComboMethodIndexToMetaMethod[ui->cmbToMethod->currentIndex()]))
		{
			//QString tmpa = hashSourceComboMethodIndexToMetaMethod[ui->cmbFromMethod->currentIndex()].methodSignature() + " >> " + hashToComboMethodIndexToMetaMethod[ui->cmbToMethod->currentIndex()].methodSignature();
			ui->pbAdd->setEnabled(true);
		}
	}
	return;
}

QStringList ObjectConnectionsDialog::getObjectMethodsList(const QString &sObjectClass, QHash<int, QMetaMethod> &hashIndexToMetataMethod)
{
	int metaID = QMetaType::type(sObjectClass.toLatin1());
	if (metaID > 0)
	{
		QObject *pObject = static_cast< QObject* > ( QMetaType::create(metaID) );//( QMetaType::construct(metaID) );
		const QMetaObject* metaObject = pObject->metaObject();

		QStringList lMetaMethods;

		for(int i = 0; i < metaObject->methodCount(); ++i)//metaObject->methodOffset()
		{
			if(metaObject->method(i).methodType()== QMetaMethod::Signal)
			{
				lMetaMethods << QString::fromLatin1(metaObject->method(i).methodSignature());
				hashIndexToMetataMethod.insert(hashIndexToMetataMethod.count(),metaObject->method(i));
			}
		}
		for(int i = 0; i < metaObject->methodCount(); ++i)//metaObject->methodOffset()
		{
			if(metaObject->method(i).methodType()== QMetaMethod::Slot)
			{
				lMetaMethods << QString::fromLatin1(metaObject->method(i).methodSignature());
				hashIndexToMetataMethod.insert(hashIndexToMetataMethod.count(),metaObject->method(i));
			}
		}
		return lMetaMethods;
	}
	return QStringList();
}

void ObjectConnectionsDialog::checkReparseModel()
{
	reCreateAndParseExperimentStructure();
}

bool ObjectConnectionsDialog::setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel)
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

bool ObjectConnectionsDialog::reCreateAndParseExperimentStructure()
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

bool ObjectConnectionsDialog::parseExperimentStructure() 
{
	bool bRetval = false;
	bool bObjectsAvailable = false;
	ui->cmbFromObject->clear();
	ui->cmbToObject->clear();
	hashComboObjectIndexToObjectStructure.clear();
	QTreeWidgetItem *pNextCurrentTreeWidgetItem = NULL;
	int nExpObjectCount = pCurrentExpStructure->getObjectCount();
	if(nExpObjectCount > 0)
	{
		cObjectStructure *pSourceObjectStructure = NULL;
		int nUniqueSelectedObjectConnectionIdentifier = 0;
		QString sMainItemString;
		QString sSubItemString;
		QList<QTreeWidgetItem *> lProcessedObjectConnectionTreeWidgetItems;
		QList<cObjectStructure *> lExpObjectList = pCurrentExpStructure->getObjectList();

		for (int nObjectCounter=0;nObjectCounter<nExpObjectCount;nObjectCounter++)
		{
			pSourceObjectStructure = lExpObjectList.at(nObjectCounter);
			if(pSourceObjectStructure) 
			{
				sMainItemString = "Object (" +  pSourceObjectStructure->getObjectName() + " (Class: " + pSourceObjectStructure->getObjectClass() + "))";
				ui->cmbFromObject->addItem(sMainItemString);//Do not change see mapping with From Connection Combobox
				ui->cmbToObject->addItem(sMainItemString);
				strcObjectInfoStruct strcObjectInfo;
				strcObjectInfo.nObjectID = pSourceObjectStructure->getObjectID();
				strcObjectInfo.pObjectStruct = pSourceObjectStructure;
				hashComboObjectIndexToObjectStructure.insert(ui->cmbFromObject->count()-1, strcObjectInfo);
				bObjectsAvailable = true;

				QList<cMethodConnectionStructure*> *pObjectMethodConnectionsList = pCurrentExpStructure->getObjectMethodConnectionList(pSourceObjectStructure->getObjectID());
				if(pObjectMethodConnectionsList)
				{
					int nCurrentObjectConnectionCount = pObjectMethodConnectionsList->count();
					if(nCurrentObjectConnectionCount > 0)
					{
						QTreeWidgetItem *pTmpBlockTreeWidgetItem = NULL;
						for(int nConnectionCounter=0; nConnectionCounter<nCurrentObjectConnectionCount; nConnectionCounter++)
						{
							cMethodConnectionStructure *pTmpMethodConnectionStruct = pObjectMethodConnectionsList->at(nConnectionCounter);//pSourceObjectStructure->getNextClosestConnectionIDByFromConnectionNumber(nNextSearchConnectionNumber);
							if(pTmpMethodConnectionStruct)
							{
								cObjectStructure *pTargetObjectStructure = pCurrentExpStructure->getObjectPointerByID(pTmpMethodConnectionStruct->getTargetObjectID());
		//						//do not change the below, because it's used for the view sorting
								sSubItemString = cMethodStructure::methodTypeToString((int)pTmpMethodConnectionStruct->getSourceMethodType()) + ": " + pTmpMethodConnectionStruct->getSourceSignature();						
								bool bTreeItemAlreadyPresent = false;
								strcObjectConnectionInfo *pObjectConnectionInfo = NULL;

								QHash<int, strcObjectConnectionInfo>::iterator iterObjectConnection = hashIntToObjectConnectionInfo.begin();
								while (iterObjectConnection != hashIntToObjectConnectionInfo.end()) 
								{
									if((iterObjectConnection.value().nSourceObjectID == pSourceObjectStructure->getObjectID()) && (iterObjectConnection.value().nConnectionID == pTmpMethodConnectionStruct->getMethodConnectionID()))
									{
										bTreeItemAlreadyPresent = true;
										break;
									}
									++iterObjectConnection;
								}
								if(bTreeItemAlreadyPresent)
								{
									pObjectConnectionInfo = &iterObjectConnection.value();
									pObjectConnectionInfo->pConnectionStruct = pTmpMethodConnectionStruct;
								}
								else
								{
									pObjectConnectionInfo = new strcObjectConnectionInfo();
									pObjectConnectionInfo->nSourceObjectID = pSourceObjectStructure->getObjectID();
									pObjectConnectionInfo->nConnectionID = pTmpMethodConnectionStruct->getMethodConnectionID();
								}
								pObjectConnectionInfo->pConnectionStruct = pTmpMethodConnectionStruct;
								QString sCompleteItemString;
								if(pTargetObjectStructure)
									sCompleteItemString = QString(sSubItemString + ", to %1 %2::%3 (Class: %4)").arg(cMethodStructure::methodTypeToString(pTmpMethodConnectionStruct->getTargetMethodType())).arg(pTargetObjectStructure->getObjectName()).arg(pTmpMethodConnectionStruct->getTargetSignature()).arg(pTargetObjectStructure->getObjectClass());
								else
									sCompleteItemString = sSubItemString;

								if((bTreeItemAlreadyPresent == false) || (pObjectConnectionInfo->pConnectionTreeWidgetItem == NULL))
								{
									pObjectConnectionInfo->pConnectionTreeWidgetItem = new QTreeWidgetItem((QTreeWidget*)NULL, QStringList(sCompleteItemString));
									while(hashIntToObjectConnectionInfo.keys().contains(nUniqueSelectedObjectConnectionIdentifier))
										nUniqueSelectedObjectConnectionIdentifier++;
									pObjectConnectionInfo->pConnectionTreeWidgetItem->setData(0, Qt::UserRole, nUniqueSelectedObjectConnectionIdentifier);
									hashIntToObjectConnectionInfo.insert(nUniqueSelectedObjectConnectionIdentifier,*pObjectConnectionInfo);
									if(pTmpBlockTreeWidgetItem == NULL)
									{
										if(mapBlockIdToBlockTreeWidgetItems.contains(pSourceObjectStructure->getObjectID()))
										{
											pTmpBlockTreeWidgetItem = mapBlockIdToBlockTreeWidgetItems.value(pSourceObjectStructure->getObjectID());
										}
										else
										{
											pTmpBlockTreeWidgetItem = new QTreeWidgetItem((QTreeWidget*)NULL, QStringList("From " + sMainItemString));
										}
									}
									pTmpBlockTreeWidgetItem->insertChild(nConnectionCounter, pObjectConnectionInfo->pConnectionTreeWidgetItem);
									pTmpBlockTreeWidgetItem->setData(0, Qt::UserRole, sMainItemString);
									if(pNextCurrentTreeWidgetItem == NULL)
										pNextCurrentTreeWidgetItem = pObjectConnectionInfo->pConnectionTreeWidgetItem;
									nUniqueSelectedObjectConnectionIdentifier++;
								}
								else
								{
									pObjectConnectionInfo->pConnectionTreeWidgetItem->setText(0,sCompleteItemString);
								}
								lProcessedObjectConnectionTreeWidgetItems.append(pObjectConnectionInfo->pConnectionTreeWidgetItem);
							}
						}
						if(pTmpBlockTreeWidgetItem)
						{
							mapBlockIdToBlockTreeWidgetItems.insert(pSourceObjectStructure->getObjectID(), pTmpBlockTreeWidgetItem);
						}
					}
				}
			}
		}
		//Update the hash/map(s) in case of removal
		if(lProcessedObjectConnectionTreeWidgetItems.count() < hashIntToObjectConnectionInfo.count())
		{
			foreach(QTreeWidgetItem *pTmpTreeWidgetItem,mapBlockIdToBlockTreeWidgetItems.values())
			{
				for(int nChildId=0;nChildId<pTmpTreeWidgetItem->childCount();nChildId++)
				{
					if(lProcessedObjectConnectionTreeWidgetItems.contains(pTmpTreeWidgetItem->child(nChildId)) == false)
					{
						QHash<int, strcObjectConnectionInfo>::iterator iterObjectConnection = hashIntToObjectConnectionInfo.begin();
						while (iterObjectConnection != hashIntToObjectConnectionInfo.end()) 
						{
							if(iterObjectConnection->pConnectionTreeWidgetItem == pTmpTreeWidgetItem->child(nChildId))
							{
								hashIntToObjectConnectionInfo.remove(iterObjectConnection.key());
								break;
							}
							++iterObjectConnection;
						}
						pTmpTreeWidgetItem->removeChild(pTmpTreeWidgetItem->child(nChildId));
						pNextCurrentTreeWidgetItem = NULL;//Undo item selection
						break;
					}
				}
			}
		}
		//else if(lProcessedObjectConnectionTreeWidgetItems.count() == hashIntToObjectConnectionInfo.count())
		//{//Maybe a block number change? Do we perhaps need to sort the items again?
		//	foreach(QTreeWidgetItem *pTmpTreeWidgetItem,mapBlockIdToBlockTreeWidgetItems.values())
		//	{
		//		pTmpTreeWidgetItem->sortChildren(0,Qt::AscendingOrder);
		//	}
		//}

		twCurrentConnections->headerItem()->setText(0, "Object Connections");
		twCurrentConnections->insertTopLevelItems(0, mapBlockIdToBlockTreeWidgetItems.values());
		fromObjectChanged(0);
		toObjectChanged(0);
	}
	if(pNextCurrentTreeWidgetItem)
	{
		twCurrentConnections->setCurrentItem(pNextCurrentTreeWidgetItem);//Select the item
	}
	bRetval = true;
	return bRetval;
}

void ObjectConnectionsDialog::addSelectedConnection()
{
	if(hashComboObjectIndexToObjectStructure.contains(ui->cmbFromObject->currentIndex())==false)
	{
		qDebug() << __FUNCTION__ << "Could not add the Object Connection, no indexed Object structure found.";
		return;
	}
	bool bProceed = true;
	ui->pbAdd->setEnabled(false);
	int nFromObjectComboIndex = ui->cmbFromObject->currentIndex();
	int nToObjectComboIndex = ui->cmbToObject->currentIndex();
	int nFromMethodComboIndex = ui->cmbFromMethod->currentIndex();
	int nToMethodComboIndex = ui->cmbToMethod->currentIndex();
	if(pExperimentTreeModel)
	{
		ExperimentTreeModel::strcObjectConnectionSpecifier tmpConnSpecifier;
		if(hashFromComboMethodIndexToMetaMethod[nFromMethodComboIndex].methodType()== QMetaMethod::Signal)
			tmpConnSpecifier.mSourceMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL;
		else if(hashFromComboMethodIndexToMetaMethod[nFromMethodComboIndex].methodType()== QMetaMethod::Slot)
			tmpConnSpecifier.mSourceMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_SLOT;
		else
			bProceed = false;
		if(hashToComboMethodIndexToMetaMethod[nToMethodComboIndex].methodType()== QMetaMethod::Signal)
			tmpConnSpecifier.mTargetMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL;
		else if(hashToComboMethodIndexToMetaMethod[nToMethodComboIndex].methodType()== QMetaMethod::Slot)
			tmpConnSpecifier.mTargetMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_SLOT;
		else
			bProceed = false;

		if(bProceed)
		{
			tmpConnSpecifier.sSourceSignature = hashFromComboMethodIndexToMetaMethod[nFromMethodComboIndex].methodSignature();
			tmpConnSpecifier.sTargetSignature = hashToComboMethodIndexToMetaMethod[nToMethodComboIndex].methodSignature();
			tmpConnSpecifier.nSourceObjectID = hashComboObjectIndexToObjectStructure[nFromObjectComboIndex].nObjectID;
			tmpConnSpecifier.nTargetObjectID = hashComboObjectIndexToObjectStructure[nToObjectComboIndex].nObjectID;
			if(pExperimentTreeModel->addObjectConnection(tmpConnSpecifier) == false)
			{
				qDebug() << __FUNCTION__ << "Could not add the new Object Connection";
			}
		}
		else
		{
			qDebug() << __FUNCTION__ << "Could determine the method types for the new Object Connection";
		}
	}
	ui->cmbFromObject->setCurrentIndex(nFromObjectComboIndex);
	ui->cmbToObject->setCurrentIndex(nToObjectComboIndex);
	ui->pbAdd->setEnabled(true);
}