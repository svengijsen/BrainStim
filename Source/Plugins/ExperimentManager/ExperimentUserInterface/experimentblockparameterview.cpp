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

#include "experimentblockparameterview.h"
#include "experimentparameterwidgets.h"
#include "parameterpropertyextensions.h"
#include "experimentparametervisualizer.h"
#include "objectparameterdialog.h"
#include "../experimentmanager.h"
#include "../global.h"
#include "experimenttreeitem.h"
#include "experimenttreemodel.h"
#include <QMenu>
#include <QHeaderView>
#include <QInputDialog>

ExperimentBlockParameterView::ExperimentBlockParameterView(QWidget *parent, ExperimentTreeModel *pExperimentTreeModel) : CustomChildDockTableWidget(parent), pExpTreeModel(pExperimentTreeModel)
{
	CustomChildDockTableWidget::setGroupName(PLUGIN_EXMLDOC_EXTENSION);
	parsedExpStruct = NULL;
	pObjectParameterDialog = NULL;
	leCustomParamEdit = NULL;
	bEditHandlingEnabled = false;
	bDoReparseModel = true;
	bVerticalViewEnabled = false;
	bCellOpenedForEdit = false;
	cChangedParameter = QColor(Qt::red);
	cChangedCustomParameter = QColor(Qt::darkMagenta);
	cInheritedParameter = QColor(Qt::darkGray);
	this->setStyleSheet("QHeaderView::section { background-color:white; color:blue }");
	configureEditHandling(true);
	setContextMenuPolicy(Qt::CustomContextMenu);
	this->horizontalHeader()->setDefaultAlignment(Qt::AlignTop | Qt::AlignLeft);
	this->verticalHeader()->setDefaultAlignment(Qt::AlignTop | Qt::AlignLeft);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
	connect(this, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(cellOpenedForEdit(int,int)), Qt::DirectConnection);
	if(pExpTreeModel)
	{
		connect(pExpTreeModel, SIGNAL(modelModified()), this, SLOT(checkReparseModel()));
		reparseModel();
	}
}

void ExperimentBlockParameterView::checkReparseModel()
{
	if(bDoReparseModel)
		return reparseModel();
	return;
}

void ExperimentBlockParameterView::reparseModel()
{
	cExperimentStructure *tmpExpStructure = new cExperimentStructure();
	QList<ExperimentTreeItem*> tmpFoundExpTreeItemList;
	bool bResult = ExperimentManager::createExperimentStructure(tmpFoundExpTreeItemList,pExpTreeModel,tmpExpStructure);
	if(bResult)
	{
		this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		bResult = bResult && parseExperimentStructure(tmpExpStructure);
		if(bResult)
		{
			QList<ExperimentStructuresNameSpace::strcExperimentObject> tmpExperimentObjectList;
			tmpExperimentObjectList = pExpTreeModel->getDefinedExperimentObjectInfoList(NULL);
			bResult = bResult && setExperimentObjects(tmpExperimentObjectList);
			if(bResult)
			{
				//tmpFoundExpTreeItemList = pExpTreeModel->getFilteredItemList()
				QStringList strSearchPathList;
				strSearchPathList.clear();
				strSearchPathList.append(ACTIONS_TAG);
				strSearchPathList.append(BLOCKTRIALS_TAG);
				strSearchPathList.append(BLOCK_TAG);
				QList<ExperimentTreeItem*> tmpFoundExpBlocksTreeItemList;
				if (ExperimentTreeModel::getStaticTreeElements(strSearchPathList, tmpFoundExpBlocksTreeItemList, tmpFoundExpTreeItemList.at(0)) > 0)
				{
					bResult = bResult && parseExperimentBlockParameters(tmpFoundExpBlocksTreeItemList);
				}
				else
				{
					bResult = false;
				}
			}
		}
		if(bCellOpenedForEdit==false)
		{
			this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
			this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		}
	}
}

void ExperimentBlockParameterView::showContextMenu(const QPoint& pos)
{
	QPoint globalPos = this->mapToGlobal(pos);
	QModelIndex mIndex = this->indexAt(pos);
	QModelIndexList mSelectedIndexes = selectedIndexes();
	QMenu mContexMenu;
	QAction* selectedItemAction;
	QAction* addBlockAction;
	QAction* addMultipleBlocksAction;
	QAction* removeBlockAction;
	QAction* removeBlocksAction;
	QAction* moveUpLeftBlocksAction;
	QAction* moveDownRightBlocksAction;
	QAction* toggleViewAction;
	QAction* resetParametersAction;
	QAction* addParametersAction;
	int nSelectionCount = mSelectedIndexes.count();
	QSet<int> bUsedColumnIndexesInSelectionRanges;
	QSet<int> bUsedRowIndexesInSelectionRanges;
	QSet<int> bUsedBlockIDsInSelectionRanges;
	QList<int> lstUsedBlockIDsInSelectionRanges;
	QModelIndexList lstDefinedParamIndexesInSelectionRanges;
	QRect outerSelectionIndexes; 
	QMenu* mViewMenu = mContexMenu.addMenu(QIcon(":/resources/view.png"), "View");
	QMenu* mBlocksMenu = mContexMenu.addMenu(QIcon(":/resources/blocks.png"), "Blocks");
	QMenu* mParametersMenu = mContexMenu.addMenu(QIcon(":/resources/parameters.png"), "Parameters");
	int nTempRow;
	int nTempColumn;	

	outerSelectionIndexes.setLeft(-1);
	outerSelectionIndexes.setRight(-1);
	outerSelectionIndexes.setTop(-1);
	outerSelectionIndexes.setBottom(-1);

	for(int i=0;i<nSelectionCount;i++)
	{
		nTempRow = mSelectedIndexes.at(i).row();
		nTempColumn = mSelectedIndexes.at(i).column();

		if((item(nTempRow,nTempColumn)->textColor() == cChangedParameter) || (item(nTempRow,nTempColumn)->textColor() == cChangedCustomParameter))
		{
			if(((bVerticalViewEnabled)&&(nTempRow >= BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT)) || ((bVerticalViewEnabled==false)&&(nTempColumn >= BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT)))
				lstDefinedParamIndexesInSelectionRanges.append(mSelectedIndexes.at(i));
		}

		if(i==0)
		{
			outerSelectionIndexes.setLeft(nTempColumn);
			outerSelectionIndexes.setRight(nTempColumn);
			outerSelectionIndexes.setTop(nTempRow);
			outerSelectionIndexes.setBottom(nTempRow);
		}
		else
		{
			if(nTempColumn > outerSelectionIndexes.right())
				outerSelectionIndexes.setRight(nTempColumn);
			else if(nTempColumn < outerSelectionIndexes.left())
				outerSelectionIndexes.setLeft(nTempColumn);
			if(nTempRow > outerSelectionIndexes.bottom())
				outerSelectionIndexes.setBottom(nTempRow);
			else if(nTempRow < outerSelectionIndexes.top())
				outerSelectionIndexes.setTop(nTempRow);
		}

		bUsedColumnIndexesInSelectionRanges.insert(nTempColumn);
		bUsedRowIndexesInSelectionRanges.insert(nTempRow);
		if(bVerticalViewEnabled)
			bUsedBlockIDsInSelectionRanges.insert(hashRowOrColumnIndexBlockId[nTempColumn]);
		else
			bUsedBlockIDsInSelectionRanges.insert(hashRowOrColumnIndexBlockId[nTempRow]);
	}
	lstUsedBlockIDsInSelectionRanges = bUsedBlockIDsInSelectionRanges.toList();

	toggleViewAction = mViewMenu->addAction(QIcon(":/resources/orientation.png"), "Toggle Orientation");
	if(nSelectionCount>0)
	{
		if(bVerticalViewEnabled)
		{
			if(bUsedColumnIndexesInSelectionRanges.contains(0) == false)
				moveUpLeftBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/moveleft.png"), "Move Left (Decrease BlockNumber(s)");
			if(bUsedColumnIndexesInSelectionRanges.contains(columnCount()-1) == false)
				moveDownRightBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/moveright.png"), "Move Right (Increase BlockNumber(s))");
			
		}
		else
		{
			if(bUsedRowIndexesInSelectionRanges.contains(0) == false)
				moveUpLeftBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/moveup.png"), "Move Up (Decrease BlockNumber(s))");
			if(bUsedRowIndexesInSelectionRanges.contains(rowCount()-1) == false)
				moveDownRightBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/movedown.png"), "Move Down (Increase BlockNumber(s))");
		}
	}

	addBlockAction = mBlocksMenu->addAction(QIcon(":/resources/add.png"), "Add New");
	addMultipleBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/addmultiple.png"), "Add New(multiple)");
	if(bVerticalViewEnabled)
	{
		if(bUsedColumnIndexesInSelectionRanges.count() == 1)
			removeBlockAction = mBlocksMenu->addAction(QIcon(":/resources/remove.png"), "Remove Selected");
		else if(bUsedColumnIndexesInSelectionRanges.count() > 1)
			removeBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/remove.png"), "Remove Multiple Selected");
	}
	else
	{
		if(bUsedRowIndexesInSelectionRanges.count() == 1)
			removeBlockAction = mBlocksMenu->addAction(QIcon(":/resources/remove.png"), "Remove Selected");
		else if(bUsedRowIndexesInSelectionRanges.count() > 1)
			removeBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/remove.png"), "Remove Multiple Selected");
	}

	addParametersAction = mParametersMenu->addAction(QIcon(":/resources/configure.png"), "Configure Parameter(s)");
	if(lstDefinedParamIndexesInSelectionRanges.isEmpty() == false)
	{		
		if (lstDefinedParamIndexesInSelectionRanges.count() == 1)
			resetParametersAction = mParametersMenu->addAction(QIcon(":/resources/reset.png"), "Reset Selected");
		else
			resetParametersAction = mParametersMenu->addAction(QIcon(":/resources/reset.png"), "Reset Multiple Selected");
	}
	
	selectedItemAction = mContexMenu.exec(globalPos);
	if (selectedItemAction)
	{
		if((selectedItemAction == addBlockAction)||(selectedItemAction == addMultipleBlocksAction))
		{
			if(pExpTreeModel)
			{
				int nBlocksToAdd;
				if(selectedItemAction == addMultipleBlocksAction)
				{
					bool bDialogResult;
					nBlocksToAdd = QInputDialog::getInt(this, tr("How many blocks?"),	tr("Blocks to add:"), 0, 0, 1000, 1, &bDialogResult);
					if (bDialogResult == false)
						nBlocksToAdd = 0;
				}
				else if(selectedItemAction == addBlockAction)
				{
					nBlocksToAdd = 1;
				}
				bool bResult = false;
				if(nBlocksToAdd>0)
				{
					bResult = pExpTreeModel->addExperimentBlocks(nBlocksToAdd);
				}			
			}
		}
		else if((selectedItemAction == removeBlockAction) || (selectedItemAction == removeBlocksAction))
		{
			if(pExpTreeModel)
				pExpTreeModel->removeExperimentBlocks(lstUsedBlockIDsInSelectionRanges);
		}
		else if(selectedItemAction == toggleViewAction)
		{
			bVerticalViewEnabled = !bVerticalViewEnabled;
			reparseModel();
		}
		else if(selectedItemAction == moveUpLeftBlocksAction)
		{			
			if(bVerticalViewEnabled)
				pExpTreeModel->moveExperimentBlocks(lstUsedBlockIDsInSelectionRanges, hashRowOrColumnIndexBlockId[outerSelectionIndexes.left()-1], -1);
			else
				pExpTreeModel->moveExperimentBlocks(lstUsedBlockIDsInSelectionRanges, hashRowOrColumnIndexBlockId[outerSelectionIndexes.top()-1], -1);
		}
		else if(selectedItemAction == moveDownRightBlocksAction)
		{
			if(bVerticalViewEnabled)
				pExpTreeModel->moveExperimentBlocks(lstUsedBlockIDsInSelectionRanges, hashRowOrColumnIndexBlockId[outerSelectionIndexes.right()+1], 1);
			else
				pExpTreeModel->moveExperimentBlocks(lstUsedBlockIDsInSelectionRanges, hashRowOrColumnIndexBlockId[outerSelectionIndexes.bottom()+1], 1);
		}
		else if(selectedItemAction == addParametersAction)
		{
			pObjectParameterDialog = new ObjectParameterDialog();
			if(pObjectParameterDialog->parseExperimentTreeModel(pExpTreeModel))
				pObjectParameterDialog->exec();
		}
		else if(selectedItemAction == resetParametersAction)
		{
			int nBlockID;
			QString sBlockParamName;
			QStringList sSplittedBlockParamName;
			QList<ExperimentTreeModel::strcParameterSpecifier> lstParameterSpecifiers;
			ExperimentTreeModel::strcParameterSpecifier tmpParameterSpecifier;
			foreach(QModelIndex tmpModelIndex,lstDefinedParamIndexesInSelectionRanges)
			{
				if(bVerticalViewEnabled)
				{
					nBlockID = hashRowOrColumnIndexBlockId[tmpModelIndex.column()];
					sBlockParamName = hashRowOrColumnIndexObjectIDParamName[tmpModelIndex.row()];
				}
				else
				{
					nBlockID = hashRowOrColumnIndexBlockId[tmpModelIndex.row()];
					sBlockParamName = hashRowOrColumnIndexObjectIDParamName[tmpModelIndex.column()];
				}
				sSplittedBlockParamName = sBlockParamName.split(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER,QString::SkipEmptyParts);
				if(sSplittedBlockParamName.count() > 2)
				{
					tmpParameterSpecifier.nBlockID = nBlockID;
					tmpParameterSpecifier.nObjectID = sSplittedBlockParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_OBJECTID_INDEX).toInt();
					tmpParameterSpecifier.sParamHexID = sSplittedBlockParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMID_HEX_INDEX);
					tmpParameterSpecifier.sParamName = sSplittedBlockParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX);
					tmpParameterSpecifier.cParamEditType = (ParameterEditingType)(sSplittedBlockParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMTYPE_INDEX).toInt());
					lstParameterSpecifiers.append(tmpParameterSpecifier);					
				}
			}
			if(lstParameterSpecifiers.isEmpty() == false)
				pExpTreeModel->removeExperimentParameters(lstParameterSpecifiers);
		}
	}
}

ExperimentBlockParameterView::~ExperimentBlockParameterView()
{
	configureEditHandling(false);
	if (parsedExpStruct)
		parsedExpStruct = NULL;
	if(pObjectParameterDialog)
	{
		delete pObjectParameterDialog;
		pObjectParameterDialog = NULL;
	}
	if(leCustomParamEdit)
	{
		delete leCustomParamEdit;
		leCustomParamEdit = NULL;
	}
	emit destroyed(this);
}

void ExperimentBlockParameterView::initTableSetup()
{
	this->clear();
	lColumnHeaders.clear();
	lRowHeaders.clear();

	ExperimentParameterWidgets *expParamWidgets = ExperimentParameterWidgets::instance();
	ExperimentParameterDefinitionContainer *tmpDefs = expParamWidgets->getExperimentParameterDefinition(BLOCK_TAG);
	int nParamID;
	for (int i=0;i<BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT;i++)
	{
		//if(i==BLOCKPARAMVIEW_BLOCKNUMBER_COLUMNINDEX)
		//	nParamID = tmpDefs->getFirstParameterID(BLOCKNUMBER_TAG);
		//else 
		if(i==BLOCKPARAMVIEW_BLOCKNAME_ROWORCOLUMNINDEX)
			nParamID = tmpDefs->getFirstParameterID(NAME_TAG);
		else if(i==BLOCKPARAMVIEW_BLOCKTRIALS_ROWORCOLUMNINDEX)
			nParamID = tmpDefs->getFirstParameterID(TRIALAMOUNT_TAG);
		else if(i==BLOCKPARAMVIEW_BLOCKINTTRGS_ROWORCOLUMNINDEX)
			nParamID = tmpDefs->getFirstParameterID(INTERNALTRIGGERAMOUNT_TAG);
		else if(i==BLOCKPARAMVIEW_BLOCKEXTTRGS_ROWORCOLUMNINDEX)
			nParamID = tmpDefs->getFirstParameterID(EXTERNALTRIGGERAMOUNT_TAG);
		if(nParamID >= 0)
		{
			QString sTempHeaderCaption = BLOCKPARAMVIEW_BLOCKPARAMS_PREHEADERSTRING ": " + tmpDefs->getParameterDefinition(nParamID)->sDisplayName;
			if(bVerticalViewEnabled)
				lRowHeaders.append(sTempHeaderCaption);
			else
				lColumnHeaders.append(sTempHeaderCaption);
		}
		else
		{
			if(bVerticalViewEnabled)
				lRowHeaders.append("<unknown header>");
			else
				lColumnHeaders.append("<unknown header>");
		}
	}
	if(bVerticalViewEnabled)
	{
		this->setRowCount(lRowHeaders.count());
		this->setVerticalHeaderLabels(lRowHeaders);
	}
	else
	{
		this->setColumnCount(lColumnHeaders.count());
		this->setHorizontalHeaderLabels(lColumnHeaders);
	}
}

bool ExperimentBlockParameterView::parseExperimentStructure(cExperimentStructure *ExpStruct)
{
	int nExpBlockCount = ExpStruct->getBlockCount();
	if(bVerticalViewEnabled)
		this->setColumnCount(nExpBlockCount);
	else
		this->setRowCount(nExpBlockCount);
	initTableSetup();		
	cBlockStructure *tmpBlock = NULL;
	int nNextSearchBlockNumber = 0;
	QString sExpName = ExpStruct->getExperimentName();
	QTableWidgetItem *tmpItem = NULL;
	configureEditHandling(false);
	hashBlockIdRowOrColumnIndex.clear();
	hashRowOrColumnIndexBlockId.clear();
	for (int i=0;i<nExpBlockCount;i++)
	{
		tmpBlock = NULL;
		tmpBlock = ExpStruct->getNextClosestBlockNumberByFromNumber(nNextSearchBlockNumber);
		if(tmpBlock) 
		{
			if(tmpItem == NULL)
			{
				tmpItem = new QTableWidgetItem();
				tmpItem->setTextColor(cChangedParameter);
				tmpItem->setTextAlignment(Qt::AlignCenter);
			}
			if(bVerticalViewEnabled)
			{
				lColumnHeaders.append(tr("%1%2").arg(BLOCKPARAMVIEW_BLOCKNUMBER_HEADERSTRING).arg(tmpBlock->getBlockNumber()));
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getBlockName()));
				this->setItem(BLOCKPARAMVIEW_BLOCKNAME_ROWORCOLUMNINDEX, i, tmpItem);
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getNumberOfTrials()));
				this->setItem(BLOCKPARAMVIEW_BLOCKTRIALS_ROWORCOLUMNINDEX, i, tmpItem);
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getNumberOfInternalTriggers()));
				this->setItem(BLOCKPARAMVIEW_BLOCKINTTRGS_ROWORCOLUMNINDEX, i, tmpItem);
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getNumberOfExternalTriggers()));
				this->setItem(BLOCKPARAMVIEW_BLOCKEXTTRGS_ROWORCOLUMNINDEX, i, tmpItem);
			}
			else
			{
				lRowHeaders.append(tr("%1%2").arg(BLOCKPARAMVIEW_BLOCKNUMBER_HEADERSTRING).arg(tmpBlock->getBlockNumber()));
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getBlockName()));
				this->setItem(i, BLOCKPARAMVIEW_BLOCKNAME_ROWORCOLUMNINDEX, tmpItem);
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getNumberOfTrials()));
				this->setItem(i, BLOCKPARAMVIEW_BLOCKTRIALS_ROWORCOLUMNINDEX, tmpItem);
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getNumberOfInternalTriggers()));
				this->setItem(i, BLOCKPARAMVIEW_BLOCKINTTRGS_ROWORCOLUMNINDEX, tmpItem);
				tmpItem = tmpItem->clone();
				tmpItem->setText(tr("%1").arg(tmpBlock->getNumberOfExternalTriggers()));
				this->setItem(i, BLOCKPARAMVIEW_BLOCKEXTTRGS_ROWORCOLUMNINDEX, tmpItem);
			}
			hashBlockIdRowOrColumnIndex[tmpBlock->getBlockID()] = i;
			hashRowOrColumnIndexBlockId[i] = tmpBlock->getBlockID();
			nNextSearchBlockNumber = tmpBlock->getBlockNumber() + 1;
		}
	}
	if(bVerticalViewEnabled)
		this->setHorizontalHeaderLabels(lColumnHeaders);
	else
		this->setVerticalHeaderLabels(lRowHeaders);
	parsedExpStruct = ExpStruct;
	this->resizeColumnsToContents();
	configureEditHandling(true);	
	return true;
}

bool ExperimentBlockParameterView::parseExperimentBlockParameters(const QList<ExperimentTreeItem*> &tmpExpTreeItemList)
{
	mapUniqueParamIDExpParamBlockChanges.clear();
	ParameterEditingType currentParamEditType;
	int nBlockCount = tmpExpTreeItemList.count();
	configureEditHandling(false);
	if(nBlockCount>0)
	{		
		ExperimentTreeItem* pTmpExpTreeItem;
		QMap<QString, TreeItemDefinition> tTmpTreeItemDefs;
		int nBlockID = -1;
		int nNrOfObjects = -1;
		int nObjectID = -1;
		int nParameterID = -1;
		int nNrOfParameters = -1;
		QString sParamName;
		QString tmpString;
		QString uniqueObjParamIdentifier;
		QStringList lObjectSearchPath;
		QStringList lObjectParameterSearchPath;
		lObjectSearchPath << OBJECT_TAG;

		for (int nParEditTypeCntr=0;nParEditTypeCntr<2;nParEditTypeCntr++) //for each block parameter type
		{
			if(nParEditTypeCntr == 0)
			{
				currentParamEditType = PEM_DEFINED;
				lObjectParameterSearchPath.clear();
				lObjectParameterSearchPath << PARAMETERS_TAG << PARAMETER_TAG;
			}
			else if(nParEditTypeCntr == 1)
			{
				currentParamEditType = PEM_CUSTOM;
				lObjectParameterSearchPath.clear();
				lObjectParameterSearchPath << CUSTOM_PARAMETERS_TAG << PARAMETER_TAG;
			}
			else
			{
				return false;
			}

			for (int i=0;i<nBlockCount;i++) //for each block
			{
				nBlockID = -1;
				nNrOfObjects = -1;
				nObjectID = -1;
				nParameterID = -1;
				nNrOfParameters = -1;

				//BlockID
				pTmpExpTreeItem = tmpExpTreeItemList.at(i);
				tTmpTreeItemDefs = pTmpExpTreeItem->getDefinitions();
				if(tTmpTreeItemDefs.contains(ID_TAG))
				{
					nBlockID = tTmpTreeItemDefs[ID_TAG].value.toInt();
				}
				else
				{
					continue;
				}

				//Objects
				QList<ExperimentTreeItem*> lExpTreeObjectItems;
				nNrOfObjects = ExperimentTreeModel::getStaticTreeElements(lObjectSearchPath, lExpTreeObjectItems, tmpExpTreeItemList.at(i));
				if(nNrOfObjects > 0)
				{
					for (int j=0;j<nNrOfObjects;j++)//For each Object
					{

						//ObjectID
						tTmpTreeItemDefs = lExpTreeObjectItems.at(j)->getDefinitions();
						if(tTmpTreeItemDefs.contains(ID_TAG))
						{
							nObjectID = tTmpTreeItemDefs[ID_TAG].value.toInt();

							//Parameters
							QList<ExperimentTreeItem*> lExpTreeObjectParameterItems;
							nNrOfParameters = ExperimentTreeModel::getStaticTreeElements(lObjectParameterSearchPath, lExpTreeObjectParameterItems, lExpTreeObjectItems.at(j));
							if(nNrOfParameters > 0)
							{
								for (int k=0;k<nNrOfParameters;k++)//For each Parameter
								{
									//ParameterID
									tTmpTreeItemDefs = lExpTreeObjectParameterItems.at(k)->getDefinitions();
									if(tTmpTreeItemDefs.contains(ID_TAG))
										nParameterID = tTmpTreeItemDefs[ID_TAG].value.toInt();
									else
										continue;

									//ParameterName
									sParamName = "";
									pTmpExpTreeItem = lExpTreeObjectParameterItems.at(k)->firstChild(NAME_TAG);
									if(pTmpExpTreeItem)
										sParamName = pTmpExpTreeItem->getValue().toLower();
									else
										continue;

									//ParameterValue
									pTmpExpTreeItem = lExpTreeObjectParameterItems.at(k)->firstChild(VALUE_TAG);
									if(pTmpExpTreeItem)
									{
										strcParameterBlockChanges paramBlockChanges;
										if(currentParamEditType == PEM_DEFINED)
										{
											if(hashObjectIdExperimentObjectInfo[nObjectID].pObjectParamDefContainer == NULL)
											continue;
										}
										paramBlockChanges.sValue = pTmpExpTreeItem->getValue();
										//expandExperimentBlockParameterValue(tmpValue);
										paramBlockChanges.nBlockID = nBlockID;
										int nTmpParamID = -1;
										int nTmpParamDefID = -1;
										if(hashObjectIdExperimentObjectInfo[nObjectID].pObjectParamDefContainer)
										{
											nTmpParamID = hashObjectIdExperimentObjectInfo[nObjectID].pObjectParamDefContainer->getFirstParameterID(sParamName.toLower());
											nTmpParamDefID = -1;
											if(nTmpParamID>=0)
												nTmpParamDefID = hashObjectIdExperimentObjectInfo[nObjectID].pObjectParamDefContainer->getParameterDefinition(nTmpParamID)->nId;
											else
												nTmpParamDefID = k;
										}
										else
										{
											nTmpParamDefID = k;
										}
										uniqueObjParamIdentifier = ExperimentParameterWidgets::getUniqueParameterIndentifier(currentParamEditType,nObjectID,nTmpParamDefID,sParamName);
										if(mapUniqueParamIDExpParamBlockChanges.contains(uniqueObjParamIdentifier))
										{
											mapUniqueParamIDExpParamBlockChanges[uniqueObjParamIdentifier].append(paramBlockChanges);
										}
										else
										{
											QList<strcParameterBlockChanges> tmpParameterBlockChangesList;
											tmpParameterBlockChangesList.append(paramBlockChanges);
											mapUniqueParamIDExpParamBlockChanges.insert(uniqueObjParamIdentifier, tmpParameterBlockChangesList);
										}
									}
								}
							}
						}				
					}
				}
			}
		}
	}
	bool bResult = appendExperimentBlockParameterChanges();
	configureEditHandling(true);
	return bResult;
}

bool ExperimentBlockParameterView::setExperimentObjects(const QList<ExperimentStructuresNameSpace::strcExperimentObject> &lExperimentObjects) 
{
	hashObjectIdExperimentObjectInfo.clear();
	ExperimentParameterWidgets *expParamWidgets = ExperimentParameterWidgets::instance();
	for (int i = 0; i < lExperimentObjects.count(); i++)
	{
		strcExperimentObjectInfo tmpObjectInfo;
		tmpObjectInfo.ObjectGlobalInfo = lExperimentObjects[i];
		tmpObjectInfo.pObjectParamDefContainer = expParamWidgets->getExperimentParameterDefinition(lExperimentObjects[i].sClass);
		tmpObjectInfo.sObjectName = lExperimentObjects[i].sName;
		hashObjectIdExperimentObjectInfo[lExperimentObjects[i].nID] = tmpObjectInfo;
	}
	strcExperimentObjectInfo tmpObjectInfo;
	tmpObjectInfo.ObjectGlobalInfo.nID = EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID;
	tmpObjectInfo.ObjectGlobalInfo.sClass = BLOCK_TAG;
	tmpObjectInfo.ObjectGlobalInfo.sName = "<undefined>";
	tmpObjectInfo.pObjectParamDefContainer = expParamWidgets->getExperimentParameterDefinition(tmpObjectInfo.ObjectGlobalInfo.sClass);
	tmpObjectInfo.sObjectName = BLOCKPARAMVIEW_BLOCKPARAMS_PREHEADERSTRING;
	hashObjectIdExperimentObjectInfo[tmpObjectInfo.ObjectGlobalInfo.nID] = tmpObjectInfo;
	return true;
}

bool ExperimentBlockParameterView::appendExperimentBlockParameterChanges()
{
	hashObjectParameterRowOrColumnIndex.clear();
	hashRowOrColumnIndexObjectIDParamName.clear();

	ExperimentParameterDefinitionContainer *pTempObjectParamDefContainer;
	//////////////DEFAULT BLOCK SECTION START//////////////////////
	strcRowOrColumnInfo tmpColumnInfo;
	QString sObjectParamIdentifier;
	int nParamID;
	pTempObjectParamDefContainer = hashObjectIdExperimentObjectInfo[EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID].pObjectParamDefContainer;
	for (int i = 0; i < BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT; i++)
	{
		if (i == BLOCKPARAMVIEW_BLOCKNAME_ROWORCOLUMNINDEX)
			sObjectParamIdentifier = tr("%1%2%3%4%5%6%7").arg((int)PEM_DEFINED).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg("00").arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(NAME_TAG);
		else if (i == BLOCKPARAMVIEW_BLOCKTRIALS_ROWORCOLUMNINDEX)
			sObjectParamIdentifier = tr("%1%2%3%4%5%6%7").arg((int)PEM_DEFINED).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg("01").arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(TRIALAMOUNT_TAG);
		else if (i == BLOCKPARAMVIEW_BLOCKINTTRGS_ROWORCOLUMNINDEX)
			sObjectParamIdentifier = tr("%1%2%3%4%5%6%7").arg((int)PEM_DEFINED).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg("02").arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(INTERNALTRIGGERAMOUNT_TAG);
		else if (i == BLOCKPARAMVIEW_BLOCKEXTTRGS_ROWORCOLUMNINDEX)
			sObjectParamIdentifier = tr("%1%2%3%4%5%6%7").arg((int)PEM_DEFINED).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg("03").arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(EXTERNALTRIGGERAMOUNT_TAG);

		if (bVerticalViewEnabled)
			tmpColumnInfo.sHeader = lRowHeaders.at(i);
		else
			tmpColumnInfo.sHeader = lColumnHeaders.at(i);
		tmpColumnInfo.nRowOrColumnIndex = i;
		tmpColumnInfo.nObjectID = EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID;
		if (pTempObjectParamDefContainer)
		{
			nParamID = pTempObjectParamDefContainer->getFirstParameterID(NAME_TAG);
			if (nParamID >= 0)
				tmpColumnInfo.strcParamDef = pTempObjectParamDefContainer->getParameterDefinition(nParamID);
		}
		hashObjectParameterRowOrColumnIndex[sObjectParamIdentifier] = tmpColumnInfo;
		hashObjectParameterRowOrColumnIndex[sObjectParamIdentifier].eParamEditType = PEM_DEFINED;
		hashRowOrColumnIndexObjectIDParamName[tmpColumnInfo.nRowOrColumnIndex] = sObjectParamIdentifier;
	}
	//////////////DEFAULT BLOCK SECTION STOP////////////////////////

	if(mapUniqueParamIDExpParamBlockChanges.count() > 0)
	{		
		int nParamCounter = 0;
		int nObjectID;
		QString sParamName;
		QString sObjectIDGroupDefParamName;
		QStringList lObjectIDGroupDefParamName;
		QTableWidgetItem *tmpItem = NULL;
		int nRowOrColumnIndex;
		ParameterEditingType cParamEditType;
	
		foreach(QList<strcParameterBlockChanges> tmpParamBlockChangesList,mapUniqueParamIDExpParamBlockChanges)//For each unique Parameter
		{
			foreach(strcParameterBlockChanges tmpParamBlockChanges,tmpParamBlockChangesList)//For each Parameter change (per block)
			{
				sObjectIDGroupDefParamName = mapUniqueParamIDExpParamBlockChanges.keys().at(nParamCounter);
				lObjectIDGroupDefParamName = sObjectIDGroupDefParamName.split(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER,QString::KeepEmptyParts);
				if(lObjectIDGroupDefParamName.count() >= EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX)
				{
					nObjectID = lObjectIDGroupDefParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_OBJECTID_INDEX).toInt();
					cParamEditType = (ParameterEditingType)lObjectIDGroupDefParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMTYPE_INDEX).toInt();
					pTempObjectParamDefContainer = hashObjectIdExperimentObjectInfo[nObjectID].pObjectParamDefContainer;
					if(pTempObjectParamDefContainer)
					{
						nParamID = pTempObjectParamDefContainer->getFirstParameterID(lObjectIDGroupDefParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX));
						sParamName = hashObjectIdExperimentObjectInfo[nObjectID].sObjectName + ":\n";
						ExperimentParameterDefinitionStrc *tmpStruct = pTempObjectParamDefContainer->getParameterDefinition(nParamID);

						if (tmpStruct)
						{
							if (cParamEditType == PEM_CUSTOM)
							{
								sParamName = sParamName + lObjectIDGroupDefParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX);
							}
							else if (tmpStruct->sGroupPath.isEmpty())
							{
								sParamName = sParamName + tmpStruct->sDisplayName;
							}
							else
							{
								sParamName = sParamName + tmpStruct->sGroupPath + EXPERIMENT_PARAM_GROUPSEP_CHAR + tmpStruct->sDisplayName;
								sParamName = sParamName.replace(EXPERIMENT_PARAM_GROUPSEP_CHAR, "\n");
							}
						}
					}
					else
					{
						sParamName = lObjectIDGroupDefParamName.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX);
					}
					if(hashObjectParameterRowOrColumnIndex.contains(sObjectIDGroupDefParamName))
					{
						nRowOrColumnIndex = hashObjectParameterRowOrColumnIndex[sObjectIDGroupDefParamName].nRowOrColumnIndex;
					}
					else
					{
						if(bVerticalViewEnabled)
						{
							this->setRowCount(lRowHeaders.count()+1);
							lRowHeaders.append(sParamName);
						}
						else
						{
							this->setColumnCount(lColumnHeaders.count()+1);
							lColumnHeaders.append(sParamName);
						}
						strcRowOrColumnInfo tmpColumnInfo;
						tmpColumnInfo.sHeader = sParamName;
						if(bVerticalViewEnabled)
							tmpColumnInfo.nRowOrColumnIndex = nParamCounter + BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT;//lRowHeaders.count()-1;
						else
							tmpColumnInfo.nRowOrColumnIndex = nParamCounter + BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT;//lColumnHeaders.count()-1;
						tmpColumnInfo.nObjectID = nObjectID;
						if(pTempObjectParamDefContainer)
						{
							if(nParamID >=0)
								tmpColumnInfo.strcParamDef = pTempObjectParamDefContainer->getParameterDefinition(nParamID);
						}
						hashObjectParameterRowOrColumnIndex[sObjectIDGroupDefParamName] = tmpColumnInfo;
						hashObjectParameterRowOrColumnIndex[sObjectIDGroupDefParamName].eParamEditType = cParamEditType;
						hashRowOrColumnIndexObjectIDParamName[tmpColumnInfo.nRowOrColumnIndex] = sObjectIDGroupDefParamName;
						nRowOrColumnIndex = tmpColumnInfo.nRowOrColumnIndex;
					}
					if(hashBlockIdRowOrColumnIndex.contains(tmpParamBlockChanges.nBlockID))
					{
						if(tmpItem == NULL)
						{
							tmpItem = new QTableWidgetItem();
							tmpItem->setTextColor(cChangedParameter);
							tmpItem->setTextAlignment(Qt::AlignCenter);
						}
						else
						{
							tmpItem = tmpItem->clone();
						}
						ExperimentParameterTypeName eExpParamTypeName;
						if(hashObjectParameterRowOrColumnIndex[sObjectIDGroupDefParamName].strcParamDef)
							eExpParamTypeName = hashObjectParameterRowOrColumnIndex[sObjectIDGroupDefParamName].strcParamDef->eType;
						else
							eExpParamTypeName = Experiment_ParameterType_String;
						tmpItem->setText(tr("%1").arg(VariantExtensionPropertyManager::resolveParameterValueType(tmpParamBlockChanges.sValue, eExpParamTypeName, true).toString()));
						
						if(cParamEditType == PEM_CUSTOM)
							tmpItem->setTextColor(cChangedCustomParameter);
						else
							tmpItem->setTextColor(cChangedParameter);

						if(bVerticalViewEnabled)
							this->setItem(nRowOrColumnIndex, hashBlockIdRowOrColumnIndex[tmpParamBlockChanges.nBlockID], tmpItem);	
						else
							this->setItem(hashBlockIdRowOrColumnIndex[tmpParamBlockChanges.nBlockID], nRowOrColumnIndex, tmpItem);						
					}
				}
			}
			nParamCounter++;
		}
		if(bVerticalViewEnabled)
			this->setVerticalHeaderLabels(lRowHeaders);
		else
			this->setHorizontalHeaderLabels(lColumnHeaders);

		//Fill-in remaining default and buffered Parameter values. We can expect here that the Block Numbers are ordered!
		int nColumnCount = columnCount();
		int nRowCount = rowCount();
		if(((bVerticalViewEnabled == false) && (nColumnCount > BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT)) || ((bVerticalViewEnabled) && (nRowCount > BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT)))
		{
			ExperimentParameterDefinitionStrc *tmpExpParDefStruct = NULL;
			QString sLatestValue;
			int nRowOrColumnCountOuter;
			if(bVerticalViewEnabled)
				nRowOrColumnCountOuter = nRowCount;
			else
				nRowOrColumnCountOuter = nColumnCount;

			int nRowOrColumnCountInner;
			if(bVerticalViewEnabled)
				nRowOrColumnCountInner = nColumnCount;
			else
				nRowOrColumnCountInner = nRowCount;
			tmpItem = NULL;
			int nRow;
			int nColumn;
			for (int nOut=(BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT);nOut<nRowOrColumnCountOuter;nOut++)
			{
				sLatestValue = "";
				for (int nIn=0;nIn<nRowOrColumnCountInner;nIn++)
				{
					if(bVerticalViewEnabled)
					{
						nRow = nOut;
						nColumn = nIn;
					}
					else
					{
						nRow = nIn;
						nColumn = nOut;						
					}	
					tmpItem = item(nRow,nColumn);
					if(tmpItem == NULL)
					{
						if(nIn==0)
						{
							tmpExpParDefStruct = hashObjectParameterRowOrColumnIndex[hashRowOrColumnIndexObjectIDParamName[nOut]].strcParamDef;
							if(tmpExpParDefStruct)
								sLatestValue = VariantExtensionPropertyManager::resolveParameterValueType(tmpExpParDefStruct->sDefaultValue,tmpExpParDefStruct->eType,true).toString();
						}
						if(tmpItem == NULL)
						{
							tmpItem = new QTableWidgetItem();
							tmpItem->setTextColor(cInheritedParameter);
							tmpItem->setTextAlignment(Qt::AlignCenter);
						}
						else
						{
							tmpItem = tmpItem->clone();
						}
						tmpItem->setText(sLatestValue);
						this->setItem(nRow,nColumn,tmpItem);
					}
					else
					{
						sLatestValue = item(nRow,nColumn)->text();
					}
				}
			}
		}
	}
	return true;
}

void ExperimentBlockParameterView::configureEditHandling(const bool &bEnable) 
{
	mutexEditHandlingEnabled.lock();
	if(bEnable && (bEditHandlingEnabled==false))
		bEditHandlingEnabled = true;
	else if((bEnable==false) && bEditHandlingEnabled)
		bEditHandlingEnabled = false;
	mutexEditHandlingEnabled.unlock();
}

void ExperimentBlockParameterView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(current);
	removeCellWidget(previous.row(), previous.column());
	bCellOpenedForEdit = false;
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void ExperimentBlockParameterView::focusInEvent(QFocusEvent* event)
{
	Q_UNUSED(event);
	emit onFocusTable();
}

void ExperimentBlockParameterView::cellOpenedForEdit(const int &nRow, const int &nColumn)
{
	Q_UNUSED(nRow);
	Q_UNUSED(nColumn);
	bCellOpenedForEdit = true;
	QModelIndexList tmpModelIndexList = this->selectedIndexes();
	if(tmpModelIndexList.count()==1)
	{
		QStringList tmpObjectIDParamNameList;
		if(bVerticalViewEnabled)
			tmpObjectIDParamNameList = hashRowOrColumnIndexObjectIDParamName[tmpModelIndexList.at(0).row()].split(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER);
		else
			tmpObjectIDParamNameList = hashRowOrColumnIndexObjectIDParamName[tmpModelIndexList.at(0).column()].split(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER);
		if(tmpObjectIDParamNameList.count() > 1)
		{
			ExperimentParameterWidgets *expParamWidgets = ExperimentParameterWidgets::instance();
			int tmpInt = tmpObjectIDParamNameList.at(EXPPARAMWIDGETS_UNIQUEPARAM_OBJECTID_INDEX).toInt();
			if((tmpInt<hashObjectIdExperimentObjectInfo.count()) || (tmpInt==EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID))
			{
				QString sDerivedPrefixName;
				QString sUniqueGeneratedPropertyIdentifier = "";
				QTableWidgetItem *tmpItem;
				if(bVerticalViewEnabled)
					sDerivedPrefixName = QString::number(hashObjectIdExperimentObjectInfo[tmpInt].ObjectGlobalInfo.nID) + ":" + QString::number(hashRowOrColumnIndexBlockId[tmpModelIndexList.at(0).column()]);
				else
					sDerivedPrefixName = QString::number(hashObjectIdExperimentObjectInfo[tmpInt].ObjectGlobalInfo.nID) + ":" + QString::number(hashRowOrColumnIndexBlockId[tmpModelIndexList.at(0).row()]);
				tmpItem = item(tmpModelIndexList.at(0).row(), tmpModelIndexList.at(0).column());
				QVariant vResolvedParameterValue = "";
				bool bDoValueInit = false;
				if(tmpItem)
				{
					ExperimentParameterDefinitionStrc *tmpDef;
					if(bVerticalViewEnabled)
						tmpDef = hashObjectParameterRowOrColumnIndex[hashRowOrColumnIndexObjectIDParamName[tmpModelIndexList.at(0).row()]].strcParamDef;
					else
						tmpDef = hashObjectParameterRowOrColumnIndex[hashRowOrColumnIndexObjectIDParamName[tmpModelIndexList.at(0).column()]].strcParamDef;
					if(tmpDef)
					{
						vResolvedParameterValue = VariantExtensionPropertyManager::resolveParameterValueType(tmpItem->text(),tmpDef->eType,false);
					}
					else
					{
						vResolvedParameterValue = tmpItem->text();
					}
					bDoValueInit = true;
				}
				configureEditHandling(false);

				QWidget *tmpWidget = NULL;
				ExperimentParameterVisualizer *tmpParamVis = expParamWidgets->getExperimentParameterWidget(hashObjectIdExperimentObjectInfo[tmpInt].ObjectGlobalInfo.sClass);
				
				int nIndex1;
				int nIndex2;
				if(bVerticalViewEnabled)
				{
					nIndex1 = tmpModelIndexList.at(0).column();	
					nIndex2 = tmpModelIndexList.at(0).row();												
				}
				else
				{
					nIndex1 = tmpModelIndexList.at(0).row();
					nIndex2 = tmpModelIndexList.at(0).column();
				}
				ParameterEditingType tmpParamEditType = hashObjectParameterRowOrColumnIndex[hashRowOrColumnIndexObjectIDParamName[nIndex2]].eParamEditType;
				if(tmpParamVis && (tmpParamEditType != PEM_CUSTOM))
				{
					bool bTempPropertyEditSignaling = tmpParamVis->hasPropertyEditSignaling();
					QString sTmpDefaultValue = "";
					bool bIsScriptable = false;
					if(bTempPropertyEditSignaling)
						tmpParamVis->configurePropertyEditSignaling(false);
					ExperimentParameterDefinitionStrc *tmpExpParmStrc = hashObjectParameterRowOrColumnIndex[hashRowOrColumnIndexObjectIDParamName[nIndex2]].strcParamDef;
					if(tmpExpParmStrc)
					{
						sTmpDefaultValue = tmpExpParmStrc->sDefaultValue;
						bIsScriptable = tmpExpParmStrc->bCanBeScriptReference;
					}
					tmpWidget = tmpParamVis->getParameterEditWidget(tmpObjectIDParamNameList.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX), sDerivedPrefixName, sUniqueGeneratedPropertyIdentifier, vResolvedParameterValue, bDoValueInit, bIsScriptable);
					if(tmpWidget)
					{					
						tmpWidget->setAutoFillBackground(true);
						if(bVerticalViewEnabled)
							setCellWidget(nIndex2, nIndex1, tmpWidget);
						else
							setCellWidget(nIndex1, nIndex2, tmpWidget);
						//if (PropertyWidgetBase *pPropWidgetBase = qobject_cast<PropertyWidgetBase *>(tmpWidget))
						//	pPropWidgetBase->initFixedDefaultValue(sTmpDefaultValue);
						if(tmpItem)
						{
							if(tmpExpParmStrc)
							{
								expParamWidgets->setWidgetParameter(sUniqueGeneratedPropertyIdentifier, hashObjectIdExperimentObjectInfo[tmpInt].ObjectGlobalInfo.sClass, VariantExtensionPropertyManager::resolveParameterValueType(tmpItem->text(),tmpExpParmStrc->eType,false).toString(), true);
							}						
						}					
						ExperimentParameterVisualizer* tmpVisualizer = expParamWidgets->getExperimentParameterWidget(hashObjectIdExperimentObjectInfo[tmpInt].ObjectGlobalInfo.sClass);
						connect((QObject*)tmpVisualizer, SIGNAL(derivedItemEditFinished(const QString&, const QString&)), this, SLOT(cellItemEditFinished(const QString&, const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));					
					}
					if(bTempPropertyEditSignaling)
						tmpParamVis->configurePropertyEditSignaling(true);
				}
				else if(tmpParamEditType == PEM_CUSTOM)
				{
					//if(leCustomParamEdit)
					//	delete leCustomParamEdit;
					leCustomParamEdit = new QLineEdit();
					leCustomParamEdit->setAutoFillBackground(true);
					if(bVerticalViewEnabled)
						setCellWidget(nIndex2, nIndex1, leCustomParamEdit);
					else
						setCellWidget(nIndex1, nIndex2, leCustomParamEdit);
					leCustomParamEdit->setText(VariantExtensionPropertyManager::resolveParameterValueType(tmpItem->text(),Experiment_ParameterType_String,false).toString());
					bool bResult = connect((QObject*)leCustomParamEdit, SIGNAL(textEdited(const QString&)), this, SLOT(customCellItemEditFinished(const QString&)),Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
					bResult = bResult;
				}
				configureEditHandling(true);
			}
		}
	}
}

void ExperimentBlockParameterView::customCellItemEditFinished(const QString& sNewValue)
{
	if(bEditHandlingEnabled)
	{
		int nRow = currentRow();
		int nColumn = currentColumn();
		int nTempRow;
		int nTempColumn;
		int nIndex1;
		int nIndex2;
		if(bVerticalViewEnabled)
		{
			nIndex1 = nColumn;
			nIndex2 = nRow;
		}
		else
		{
			nIndex1 = nRow;
			nIndex2 = nColumn;
		}
		QString sParamNameFull = hashRowOrColumnIndexObjectIDParamName[nIndex2];
		QStringList lParamNameParts = sParamNameFull.split(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER);
		QString sParamName = "";
		if(lParamNameParts.count() >= EXPPARAMWIDGETS_UNIQUEPARAM_PARAMTYPE_INDEX)
			sParamName = lParamNameParts.at(EXPPARAMWIDGETS_UNIQUEPARAM_PARAMNAME_INDEX);
		int nObjectID = hashObjectParameterRowOrColumnIndex[sParamNameFull].nObjectID;
		int nBlockID = hashRowOrColumnIndexBlockId[nIndex1];
		if(pExpTreeModel)
		{
			bDoReparseModel = false;
			pExpTreeModel->saveNewData(nBlockID,nObjectID,sParamName,sNewValue,true);
			bDoReparseModel = true;
		}
		QString sNewResolvedValue = VariantExtensionPropertyManager::resolveParameterValueType(sNewValue, Experiment_ParameterType_String, true).toString();
		QTableWidgetItem *tmpItem = new QTableWidgetItem(tr("%1").arg(sNewResolvedValue));
		tmpItem->setTextColor(cChangedCustomParameter);
		tmpItem->setTextAlignment(Qt::AlignCenter);
		this->setItem(nRow,nColumn, tmpItem);	

		//Fill-in remaining default and buffered Parameter values. We can expect here that the Block Numbers are ordered!
		int nBlockOrRowCount;
		if(bVerticalViewEnabled)
			nBlockOrRowCount = columnCount();
		else
			nBlockOrRowCount = rowCount();
		if((nIndex2 >= BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT) && (nIndex1 < (nBlockOrRowCount-1)))
		{
			QString sLatestValue = sNewResolvedValue;
			tmpItem = NULL;
			for (int r=nIndex1+1;r<nBlockOrRowCount;r++)
			{
				if(bVerticalViewEnabled)
				{
					nTempRow = nIndex2;
					nTempColumn = r;
				}
				else
				{
					nTempRow = r;
					nTempColumn = nIndex2;
				}

				if(item(nTempRow,nTempColumn)->textColor() == cInheritedParameter)
				{
					if(tmpItem == NULL)
					{
						tmpItem = new QTableWidgetItem();
						tmpItem->setTextColor(cInheritedParameter);
						tmpItem->setTextAlignment(Qt::AlignCenter);
					}
					else
					{
						tmpItem = tmpItem->clone();
					}
					tmpItem->setText(sLatestValue);
					this->setItem(nTempRow,nTempColumn,tmpItem);
				}
				else
				{
					break;
				}
			}
		}
	}
}

void ExperimentBlockParameterView::cellItemEditFinished(const QString& sParamName, const QString& sNewValue)
{
	if(bEditHandlingEnabled)
	{
		int nRow = currentRow();
		int nColumn = currentColumn();
		int nTempRow;
		int nTempColumn;
		int nIndex1;
		int nIndex2;
		if(bVerticalViewEnabled)
		{
			nIndex1 = nColumn;
			nIndex2 = nRow;
		}
		else
		{
			nIndex1 = nRow;
			nIndex2 = nColumn;
		}
		int nObjectID = hashObjectParameterRowOrColumnIndex[hashRowOrColumnIndexObjectIDParamName[nIndex2]].nObjectID;
		int nBlockID = hashRowOrColumnIndexBlockId[nIndex1];
		if(pExpTreeModel)
		{
			bDoReparseModel = false;
			pExpTreeModel->saveNewData(nBlockID,nObjectID,sParamName,sNewValue);
			bDoReparseModel = true;
		}
		QString sNewResolvedValue = VariantExtensionPropertyManager::resolveParameterValueType(sNewValue, hashObjectParameterRowOrColumnIndex[hashRowOrColumnIndexObjectIDParamName[nIndex2]].strcParamDef->eType, true).toString();
		QTableWidgetItem *tmpItem = new QTableWidgetItem(tr("%1").arg(sNewResolvedValue));
		tmpItem->setTextColor(cChangedParameter);
		tmpItem->setTextAlignment(Qt::AlignCenter);
		this->setItem(nRow,nColumn, tmpItem);	

		//Fill-in remaining default and buffered Parameter values. We can expect here that the Block Numbers are ordered!
		int nBlockOrRowCount;
		if(bVerticalViewEnabled)
			nBlockOrRowCount = columnCount();
		else
			nBlockOrRowCount = rowCount();
		if((nIndex2 >= BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT) && (nIndex1 < (nBlockOrRowCount-1)))
		{
			QString sLatestValue = sNewResolvedValue;
			tmpItem = NULL;
			for (int r=nIndex1+1;r<nBlockOrRowCount;r++)
			{
				if(bVerticalViewEnabled)
				{
					nTempRow = nIndex2;
					nTempColumn = r;
				}
				else
				{
					nTempRow = r;
					nTempColumn = nIndex2;
				}

				if(item(nTempRow,nTempColumn)->textColor() == cInheritedParameter)
				{
					if(tmpItem == NULL)
					{
						tmpItem = new QTableWidgetItem();
						tmpItem->setTextColor(cInheritedParameter);
						tmpItem->setTextAlignment(Qt::AlignCenter);
					}
					else
					{
						tmpItem = tmpItem->clone();
					}
					tmpItem->setText(sLatestValue);
					this->setItem(nTempRow,nTempColumn,tmpItem);
				}
				else
				{
					break;
				}
			}
		}
	}
}