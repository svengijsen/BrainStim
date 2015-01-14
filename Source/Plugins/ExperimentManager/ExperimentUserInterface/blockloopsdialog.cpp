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

#include "blockloopsdialog.h"
#include "ui_BlockLoopsDialog.h"
#include "experimentmanager.h"
#include "propertysettingswidget.h"
#include "./../../BrainStim/propertysettingswidgetcontainer.h"

BlockLoopsDialog::BlockLoopsDialog(QWidget *parent) : QDialog(parent), bIsParsing(false), pCurrentExpStructure(NULL), twCurrentLoops(NULL), layoutTreeWidgetParent(NULL), pExpParamWidgets(NULL), pParametersWidget(NULL), pExperimentTreeModel(NULL), nCurrentBlockLoopSelectionIdentifier(-1)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui = new Ui::BlockLoopsDialog();
	ui->setupUi(this);
	setWindowTitle("Configure block loop(s)");
	twCurrentLoops = new QTreeWidget(ui->groupBox_2);
	ui->layCurrentLoops->addWidget(twCurrentLoops);
	connect(twCurrentLoops,SIGNAL(itemSelectionChanged()), this, SLOT(onTreeWidgetSelectionChanged()));
	connect(ui->pbDecreaseLoopNumber, SIGNAL(clicked()), this, SLOT(decreaseSelectedLoopNumber()));
	connect(ui->pbIncreaseLoopNumber, SIGNAL(clicked()), this, SLOT(increaseSelectedLoopNumber()));
	connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(addSelectedLoop()));
	connect(ui->pbRemove, SIGNAL(clicked()), this, SLOT(removeSelectedLoop()));
	connect(ui->pbClose, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui->cmbFromLoop, SIGNAL(currentIndexChanged(int)), this, SLOT(newLoopConfigurationChanged(int)));
	connect(ui->cmbToLoop, SIGNAL(currentIndexChanged(int)), this, SLOT(newLoopConfigurationChanged(int)));
}

BlockLoopsDialog::~BlockLoopsDialog()
{
	clearAllParsedStructures();
	if(pExpParamWidgets)
		pExpParamWidgets = NULL;
	if(pParametersWidget)
	{
		pParametersWidget->setParent(NULL);
		//if(pParametersWidget->isVisible()==false)
		//	pParametersWidget->setVisible(true);
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

void BlockLoopsDialog::clearAllParsedStructures()
{
	if(pCurrentExpStructure)
	{
		delete pCurrentExpStructure;
		pCurrentExpStructure = NULL;
	}
	twCurrentLoops->clear();
	mapBlockIdToBlockTreeWidgetItems.clear();
	hashIntToBlockLoopInfo.clear();
	nCurrentBlockLoopSelectionIdentifier = -1;
}

void BlockLoopsDialog::onTreeWidgetSelectionChanged()
{
	prepareAndHideBlockLoopConfiguration();
	QList<QTreeWidgetItem *> lSelectedItems = twCurrentLoops->selectedItems();
	bool bConvertDataResult = false;
	if(lSelectedItems.count() == 1)
	{
		int nUniqueSelectionIdentifier = lSelectedItems.at(0)->data(0, Qt::UserRole).toInt(&bConvertDataResult);
		if((bConvertDataResult) && (nUniqueSelectionIdentifier >= 0))
		{			 
			strcBlockLoopInfo tmpBlockLoopInfo = hashIntToBlockLoopInfo.value(nUniqueSelectionIdentifier);
			bool bShowWidgetResult = showLoopWidget(tmpBlockLoopInfo);
			if(pParametersWidget && bShowWidgetResult)
			{
				nCurrentBlockLoopSelectionIdentifier = nUniqueSelectionIdentifier;
				pParametersWidget->setVisible(true);
				pParametersWidget->setEnabled(true);
				ui->pbDecreaseLoopNumber->setEnabled(true);
				ui->pbIncreaseLoopNumber->setEnabled(true);
				ui->pbRemove->setEnabled(true);
			}
			QTreeWidgetItem *pParentTreeWidgetItem = lSelectedItems.at(0)->parent();
			if(pParentTreeWidgetItem)
			{
				QString sUniqueBlockSelectionIdentifier = pParentTreeWidgetItem->data(0, Qt::UserRole).toString();
				if(sUniqueBlockSelectionIdentifier.isEmpty() == false)
					ui->cmbFromLoop->setCurrentText(sUniqueBlockSelectionIdentifier);
			}
			return;
		}
		else//a block was selected?
		{
			QString sUniqueBlockSelectionIdentifier = lSelectedItems.at(0)->data(0, Qt::UserRole).toString();
			if(sUniqueBlockSelectionIdentifier.isEmpty() == false)
				ui->cmbFromLoop->setCurrentText(sUniqueBlockSelectionIdentifier);
		}
	}
	return;
}

void BlockLoopsDialog::prepareAndHideBlockLoopConfiguration()
{
	nCurrentBlockLoopSelectionIdentifier = -1;
	if(layoutTreeWidgetParent == NULL)
	{
		layoutTreeWidgetParent = new QHBoxLayout();
		//layoutTreeWidgetParant->setDirection(QHBoxLayout::RightToLeft);
		layoutTreeWidgetParent->setContentsMargins(0,0,0,0);
		layoutTreeWidgetParent->setMargin(0);
		layoutTreeWidgetParent->setSpacing(0);
		layoutTreeWidgetParent->setObjectName(QStringLiteral("layoutTreeWidgetParent"));
		ui->layLoopWidget->addLayout(layoutTreeWidgetParent);
	}
	if(pParametersWidget)
	{
		pParametersWidget->setVisible(false);
		pParametersWidget->setEnabled(false);
		ui->pbDecreaseLoopNumber->setEnabled(false);
		ui->pbIncreaseLoopNumber->setEnabled(false);
		ui->pbRemove->setEnabled(false);
	}
}

bool BlockLoopsDialog::showLoopWidget(const strcBlockLoopInfo &blockLoopInfoStrc)
{
	if(pExpParamWidgets == NULL)
		pExpParamWidgets = PropertySettingsWidgetContainer::instance();
	if(pParametersWidget == NULL)
	{
		pParametersWidget = pExpParamWidgets->getExperimentParameterWidget(LOOP_TAG);
		if(pParametersWidget)
		{
			if(layoutTreeWidgetParent)
				layoutTreeWidgetParent->insertWidget(0,pParametersWidget);
			connect((QObject*)pParametersWidget, SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(blockLoopDefinitionItemEditFinished(const QString&, const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
		}
		else
		{
			qDebug() << __FUNCTION__ << "Could not fetch the Loop widget.";
			return false;
		}
	}
	if(blockLoopInfoStrc.pLoopStruct)
	{
		pParametersWidget->setParameter(NAME_TAG,blockLoopInfoStrc.pLoopStruct->getLoopName(),true,true);
		pParametersWidget->setParameter(LOOP_NUMBER_TAG,QString::number(blockLoopInfoStrc.pLoopStruct->getLoopNumber()),true,true);
		pParametersWidget->setParameter(LOOP_AMOUNT_TAG,QString::number(blockLoopInfoStrc.pLoopStruct->getNumberOfLoops()),true,true);
		pParametersWidget->setParameter(LOOP_TARGETBLOCKID_TAG,QString::number(blockLoopInfoStrc.pLoopStruct->getTargetBlockID()),true,true);
	}
	return true;
}

void BlockLoopsDialog::blockLoopDefinitionItemEditFinished(const QString &sParamName, const QString &sNewValue)
{
	if(hashIntToBlockLoopInfo.contains(nCurrentBlockLoopSelectionIdentifier))
	{
		ExperimentTreeModel::strcBlockLoopSpecifier cBlockLoopSpecifier;
		cBlockLoopSpecifier.nBlockID = hashIntToBlockLoopInfo.value(nCurrentBlockLoopSelectionIdentifier).nBlockID;
		cBlockLoopSpecifier.nLoopID = hashIntToBlockLoopInfo.value(nCurrentBlockLoopSelectionIdentifier).nLoopID;
		cBlockLoopSpecifier.sLoopDefinitionName = sParamName;
		if(pExperimentTreeModel)
		{
			if(pExperimentTreeModel->updateExperimentBlockLoop(cBlockLoopSpecifier, sNewValue) == false)
			{
				qDebug() << __FUNCTION__ << "Could not add/update the new block loop configuration";
			}
		}
	}
	return;
}

void BlockLoopsDialog::decreaseSelectedLoopNumber()
{
	ui->pbDecreaseLoopNumber->setEnabled(false);
	changeSelectedLoopNumber(-1);
	ui->pbDecreaseLoopNumber->setEnabled(true);
}

void BlockLoopsDialog::increaseSelectedLoopNumber()
{
	ui->pbIncreaseLoopNumber->setEnabled(false);
	changeSelectedLoopNumber(1);
	ui->pbIncreaseLoopNumber->setEnabled(true);
}

void BlockLoopsDialog::changeSelectedLoopNumber(const int &nLoopNumberCorrection)
{
	if(hashIntToBlockLoopInfo.contains(nCurrentBlockLoopSelectionIdentifier))
	{
		ExperimentTreeModel::strcBlockLoopSpecifier cBlockLoopSpecifier;
		cBlockLoopSpecifier.nBlockID = hashIntToBlockLoopInfo.value(nCurrentBlockLoopSelectionIdentifier).nBlockID;
		cBlockLoopSpecifier.nLoopID = hashIntToBlockLoopInfo.value(nCurrentBlockLoopSelectionIdentifier).nLoopID;
		//cBlockLoopSpecifier.sLoopDefinitionName = sParamName;
		if(pExperimentTreeModel)
		{
			//	QString sLoopName;
			//	sLoopName = QString("Loop (BlockNumber(%0) -> BlockNumber(%1))").arg("a").arg("b");
			if(pExperimentTreeModel->changeExperimentBlockLoopNumber(cBlockLoopSpecifier,nLoopNumberCorrection) == false)
			{
				qDebug() << __FUNCTION__ << "Could not increase the block loop number";
			}
		}
		//ui->cmbFromLoop->setCurrentIndex(nFromLoopComboIndex);
		//ui->cmbToLoop->setCurrentIndex(nToLoopComboIndex);
	}
}

void BlockLoopsDialog::removeSelectedLoop()
{
	if(hashIntToBlockLoopInfo.contains(nCurrentBlockLoopSelectionIdentifier))
	{
		ExperimentTreeModel::strcBlockLoopSpecifier cBlockLoopSpecifier;
		cBlockLoopSpecifier.nBlockID = hashIntToBlockLoopInfo.value(nCurrentBlockLoopSelectionIdentifier).nBlockID;
		cBlockLoopSpecifier.nLoopID = hashIntToBlockLoopInfo.value(nCurrentBlockLoopSelectionIdentifier).nLoopID;
		//cBlockLoopSpecifier.sLoopDefinitionName = sParamName;
		if(pExperimentTreeModel)
		{
			if(pExperimentTreeModel->removeExperimentBlockLoop(cBlockLoopSpecifier) == false)
			{
				qDebug() << __FUNCTION__ << "Could not add/update the new block loop configuration";
			}
		}
	}
	return;
}

void BlockLoopsDialog::newLoopConfigurationChanged(int nChangedIndex)
{
	Q_UNUSED(nChangedIndex);
}

void BlockLoopsDialog::checkReparseModel()
{
	reCreateAndParseExperimentStructure();
}

bool BlockLoopsDialog::setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel)
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

bool BlockLoopsDialog::reCreateAndParseExperimentStructure()
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

bool BlockLoopsDialog::parseExperimentStructure() 
{
	bool bRetval = false;
	bool bBlocksAvailable = false;
	ui->cmbFromLoop->clear();
	ui->cmbToLoop->clear();
	hashComboBlockIndexToBlockStructure.clear();
	ui->pbAdd->setEnabled(false);
	QTreeWidgetItem *pNextCurrentTreeWidgetItem = NULL;
	int nExpBlockCount = pCurrentExpStructure->getBlockCount();
	if(nExpBlockCount > 0)
	{
		cBlockStructure *pSourceBlockStructure = NULL;
		int nNextSearchBlockNumber = 0;
		//int nNextFreeModelRow = 0;
		int nUniqueSelectedBlockLoopIdentifier = 0;
		QString sExpName = pCurrentExpStructure->getExperimentName();
		QString sMainItemString;
		QString sSubItemString;
		QList<QTreeWidgetItem *> lProcessedBlockLoopTreeWidgetItems;

		for (int nBlockCounter=0;nBlockCounter<nExpBlockCount;nBlockCounter++)
		{
			pSourceBlockStructure = NULL;
			pSourceBlockStructure = pCurrentExpStructure->getNextClosestBlockNumberByFromNumber(nNextSearchBlockNumber);
			if(pSourceBlockStructure) 
			{
				sMainItemString = "Block (" + QString::number(pSourceBlockStructure->getBlockNumber()) + ": " + pSourceBlockStructure->getBlockName() + ")";
				ui->cmbFromLoop->addItem(sMainItemString);//Do not change see mapping with From Loop Combobox
				ui->cmbToLoop->addItem(sMainItemString);
				strcBlockInfo strcSourceBlockInfo;
				strcSourceBlockInfo.nBlockID = pSourceBlockStructure->getBlockID();
				strcSourceBlockInfo.pBlockStruct = pSourceBlockStructure;
				hashComboBlockIndexToBlockStructure.insert(ui->cmbFromLoop->count()-1, strcSourceBlockInfo);
				bBlocksAvailable = true;
				nNextSearchBlockNumber = pSourceBlockStructure->getBlockNumber() + 1;

				int nNextSearchLoopNumber = 0;
				int nCurrentBlockLoopCount = pSourceBlockStructure->getLoopCount();
				if(nCurrentBlockLoopCount > 0)
				{
					QTreeWidgetItem *pTmpBlockTreeWidgetItem = NULL;
					for(int nLoopCounter=0; nLoopCounter<nCurrentBlockLoopCount; nLoopCounter++)
					{
						cLoopStructure *pTmpLoopStruct = pSourceBlockStructure->getNextClosestLoopIDByFromLoopNumber(nNextSearchLoopNumber);
						if(pTmpLoopStruct)
						{
							cBlockStructure *pTargetBlockStructure = pCurrentExpStructure->getBlockPointerByID(pTmpLoopStruct->getTargetBlockID());
							nNextSearchLoopNumber = pTmpLoopStruct->getLoopNumber() + 1;
							//do not change the below, because it's used for the view sorting
							sSubItemString = QString::number(pTmpLoopStruct->getLoopNumber()) + ": " + pTmpLoopStruct->getLoopName();
							
							bool bTreeItemAlreadyPresent = false;
							strcBlockLoopInfo *pBlockLoopInfo = NULL;

							QHash<int, strcBlockLoopInfo>::iterator iterBlockLoop = hashIntToBlockLoopInfo.begin();
							while (iterBlockLoop != hashIntToBlockLoopInfo.end()) 
							{
								if((iterBlockLoop.value().nBlockID == pSourceBlockStructure->getBlockID()) && (iterBlockLoop.value().nLoopID == pTmpLoopStruct->getLoopID()))
								{
									bTreeItemAlreadyPresent = true;
									break;
								}
								++iterBlockLoop;
							}
							if(bTreeItemAlreadyPresent)
							{
								pBlockLoopInfo = &iterBlockLoop.value();
								pBlockLoopInfo->pLoopStruct = pTmpLoopStruct;
							}
							else
							{
								pBlockLoopInfo = new strcBlockLoopInfo();
								pBlockLoopInfo->nBlockID = pSourceBlockStructure->getBlockID();
								pBlockLoopInfo->nLoopID = pTmpLoopStruct->getLoopID();
							}
							pBlockLoopInfo->pLoopStruct = pTmpLoopStruct;
							QString sCompleteItemString;
							if(pTargetBlockStructure)
								sCompleteItemString = QString(sSubItemString + ", from Block (%1: %2) to Block (%3: %4)").arg(pSourceBlockStructure->getBlockNumber()).arg(pSourceBlockStructure->getBlockName()).arg(QString::number(pTargetBlockStructure->getBlockNumber())).arg(pTargetBlockStructure->getBlockName());
							else
								sCompleteItemString = sSubItemString;

							if((bTreeItemAlreadyPresent == false) || (pBlockLoopInfo->pLoopTreeWidgetItem == NULL))
							{
								pBlockLoopInfo->pLoopTreeWidgetItem = new QTreeWidgetItem((QTreeWidget*)NULL, QStringList(sCompleteItemString));
								while(hashIntToBlockLoopInfo.keys().contains(nUniqueSelectedBlockLoopIdentifier))
									nUniqueSelectedBlockLoopIdentifier++;
								pBlockLoopInfo->pLoopTreeWidgetItem->setData(0, Qt::UserRole, nUniqueSelectedBlockLoopIdentifier);
								hashIntToBlockLoopInfo.insert(nUniqueSelectedBlockLoopIdentifier,*pBlockLoopInfo);
								if(pTmpBlockTreeWidgetItem == NULL)
								{
									if(mapBlockIdToBlockTreeWidgetItems.contains(pSourceBlockStructure->getBlockID()))
									{
										pTmpBlockTreeWidgetItem = mapBlockIdToBlockTreeWidgetItems.value(pSourceBlockStructure->getBlockID());
									}
									else
									{
										pTmpBlockTreeWidgetItem = new QTreeWidgetItem((QTreeWidget*)NULL, QStringList("From " + sMainItemString));
									}
								}
								pTmpBlockTreeWidgetItem->insertChild(nLoopCounter, pBlockLoopInfo->pLoopTreeWidgetItem);
								pTmpBlockTreeWidgetItem->setData(0, Qt::UserRole, sMainItemString);
								if(pNextCurrentTreeWidgetItem == NULL)
									pNextCurrentTreeWidgetItem = pBlockLoopInfo->pLoopTreeWidgetItem;
								nUniqueSelectedBlockLoopIdentifier++;
							}
							else
							{
								pBlockLoopInfo->pLoopTreeWidgetItem->setText(0,sCompleteItemString);
							}
							lProcessedBlockLoopTreeWidgetItems.append(pBlockLoopInfo->pLoopTreeWidgetItem);
						}
					}
					if(pTmpBlockTreeWidgetItem)
					{
						mapBlockIdToBlockTreeWidgetItems.insert(pSourceBlockStructure->getBlockID(), pTmpBlockTreeWidgetItem);
					}
				}
			}
		}
		//Update the hash/map(s) in case of removal
		if(lProcessedBlockLoopTreeWidgetItems.count() < hashIntToBlockLoopInfo.count())
		{
			foreach(QTreeWidgetItem *pTmpTreeWidgetItem,mapBlockIdToBlockTreeWidgetItems.values())
			{
				for(int nChildId=0;nChildId<pTmpTreeWidgetItem->childCount();nChildId++)
				{
					if(lProcessedBlockLoopTreeWidgetItems.contains(pTmpTreeWidgetItem->child(nChildId)) == false)
					{
						QHash<int, strcBlockLoopInfo>::iterator iterBlockLoop = hashIntToBlockLoopInfo.begin();
						while (iterBlockLoop != hashIntToBlockLoopInfo.end()) 
						{
							if(iterBlockLoop->pLoopTreeWidgetItem == pTmpTreeWidgetItem->child(nChildId))
							{
								hashIntToBlockLoopInfo.remove(iterBlockLoop.key());
								break;
							}
							++iterBlockLoop;
						}
						pTmpTreeWidgetItem->removeChild(pTmpTreeWidgetItem->child(nChildId));
						pNextCurrentTreeWidgetItem = NULL;//Undo item selection
						break;
					}
				}
			}
		}
		else if(lProcessedBlockLoopTreeWidgetItems.count() == hashIntToBlockLoopInfo.count())
		{//Maybe a block number change? Do we perhaps need to sort the items again?
			foreach(QTreeWidgetItem *pTmpTreeWidgetItem,mapBlockIdToBlockTreeWidgetItems.values())
			{
				pTmpTreeWidgetItem->sortChildren(0,Qt::AscendingOrder);
			}
		}

		twCurrentLoops->headerItem()->setText(0, "Block Loops");
		twCurrentLoops->insertTopLevelItems(0, mapBlockIdToBlockTreeWidgetItems.values());
	}
	ui->pbAdd->setEnabled(bBlocksAvailable);
	if(pNextCurrentTreeWidgetItem)
	{
		twCurrentLoops->setCurrentItem(pNextCurrentTreeWidgetItem);//Select the item
	}
	bRetval = true;
	return bRetval;
}

void BlockLoopsDialog::addSelectedLoop()
{
	if(hashComboBlockIndexToBlockStructure.contains(ui->cmbFromLoop->currentIndex())==false)
	{
		qDebug() << __FUNCTION__ << "Could not add the block loop, no indexed block structure found.";
		return;
	}
	ui->pbAdd->setEnabled(false);
	int nFromLoopComboIndex = ui->cmbFromLoop->currentIndex();
	int nToLoopComboIndex = ui->cmbToLoop->currentIndex();
	int nSourceBlockId = hashComboBlockIndexToBlockStructure.value(ui->cmbFromLoop->currentIndex()).nBlockID;
	int nTargetBlockId = hashComboBlockIndexToBlockStructure.value(ui->cmbToLoop->currentIndex()).nBlockID;
	if(pExperimentTreeModel)
	{
		QString sLoopName;
		sLoopName = QString("Loop (BlockNumber(%0) to BlockNumber(%1))").arg("a").arg("b");
		if(pExperimentTreeModel->addExperimentBlockLoop(nSourceBlockId,nTargetBlockId,1,sLoopName) == false)
		{
			qDebug() << __FUNCTION__ << "Could not add the new block loop";
		}
	}
	ui->cmbFromLoop->setCurrentIndex(nFromLoopComboIndex);
	ui->cmbToLoop->setCurrentIndex(nToLoopComboIndex);
	ui->pbAdd->setEnabled(true);
}