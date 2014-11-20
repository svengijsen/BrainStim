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

#include "experimentstructurevisualizer.h"
#include "ui_ExperimentStructureVisualizer.h"
#include "qgraphicsviewec.h"
#include "../experimentstructures.h"
#include "experimenttreemodel.h"
#include "experimentmanager.h"
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QGraphicsTextItem>
#include <QComboBox>
#include <QLabel>

ExperimentStructureVisualizer::ExperimentStructureVisualizer(QWidget *parent) : QWidget(parent)
{
	action_Quit = NULL;
	action_ToggleViewState = NULL;
	//action_Test = NULL;
	toolBar = NULL;
	menuFile = NULL;
	menuEdit = NULL;
	menuView = NULL;
	buttonFile = NULL;
	buttonEdit = NULL;
	buttonView = NULL;
	graphViewLayout = NULL;
	gScene = NULL;
	parsedExpStruct = NULL;
	pExperimentTreeModel = NULL;
	nWidgetMarginWidth = 0;
	nWidgetMarginHeight = 0;
	dGraphViewScale = 0.95;//zoom till 95%
	eConnDrawOrder = GRAPHLOOP_DRAW_ORDER_NUMBER_MASTERSIDE; //GRAPHLOOP_DRAW_ORDER_NUMBER_MASTERSIDE;//GRAPHLOOP_DRAW_ORDER_NUMBER_MASTERSIDE; GRAPHLOOP_DRAW_ORDER_UNNESTED;
	pLoopConnections.setColor(QColor("#303030"));
	pLoopConnections.setWidth(5);

	ui = new Ui::ExperimentStructureVisualizer();
	ui->setupUi(this);

	resetExpScene();
	configureActions(true);
	setupMenuAndActions();
	createScene();
	setupLayout();
}

ExperimentStructureVisualizer::~ExperimentStructureVisualizer()
{
	delete ui;
    configureActions(false);
	if(parsedExpStruct)
	{
		delete parsedExpStruct;
		parsedExpStruct = NULL;
	}
	if(menuFile)
		delete menuFile;
	if(menuEdit)
		delete menuEdit;
	if(menuView)
		delete menuView;
	if(buttonFile)
		delete buttonFile;
	if(buttonEdit)
		delete buttonEdit;
	if(buttonView)
		delete buttonView;
	if(toolBar)
		delete toolBar;
	if(gScene)
		delete gScene;
	if(graphViewLayout)
		delete graphViewLayout;
	if(pExperimentTreeModel)
		pExperimentTreeModel = NULL;
	resetExpScene();
	emit destroyed(this);
}

void ExperimentStructureVisualizer::resetExpScene()
{
	expSceneItems.sExperimentName = "<undefined>";
	expSceneItems.lLoops.clear();
	expSceneItems.lBlocks.clear();
	if(expSceneItems.lObjects.isEmpty() == false)
	{
		for (int i=0;i<expSceneItems.lObjects.count();i++)
		{
			expSceneItems.lObjects[i].lObjectMethodConnections.clear();
			expSceneItems.lObjects[i].lObjectInitializations.clear();
			expSceneItems.lObjects[i].lObjectFinalizations.clear();
		}
		expSceneItems.lObjects.clear();
	}
	expSceneItems.lAutoConns.clear();
	expLoopDrawing.hBlockIDMasterSideCount.clear();
	expLoopDrawing.hBlockIDSlaveSideCount.clear();
	expLoopDrawing.hBlockIDTargetLoopBlockIDs.clear();
	expLoopDrawing.lMasterSideDrawOrderedLoops.clear();
	expLoopDrawing.lSlaveSideDrawOrderedLoops.clear();
}

void ExperimentStructureVisualizer::configureActions(bool bCreate)
{
	if(bCreate)
	{
		action_Quit = new QAction("Quit", this);
		//action_Test = new QAction("Test", this);
		action_ToggleViewState = new QAction("Toggle view state", this);
	}
	else
	{		
		if(action_Quit)
		{
			delete action_Quit;
			action_Quit = NULL;
		}	
		//if(action_Test)
		//{
		//	delete action_Test;
		//	action_Test = NULL;
		//}
		if(action_ToggleViewState)
		{
			delete action_ToggleViewState;
			action_ToggleViewState = NULL;
		}
	}
}

int ExperimentStructureVisualizer::getAdditionalMenuHeight() const
{
	if (toolBar)
	{ 
		return toolBar->height();
	}
	return 0;
}

void ExperimentStructureVisualizer::setupMenuAndActions()
{
	toolBar = new QToolBar(this);
	QComboBox *cmbViewSelection = new QComboBox(this);
	cmbViewSelection->addItem(QIcon(":/resources/blocks.png"), EXPGRAPH_VIEWSTATE_BLOCKS);
	cmbViewSelection->addItem(QIcon(":/resources/objects.png"), EXPGRAPH_VIEWSTATE_OBJECTS);
	QAction *tmpAction = toolBar->insertWidget(NULL, cmbViewSelection);
	QLabel *labViewSelection = new QLabel("Show: ", this);
	toolBar->insertWidget(tmpAction, labViewSelection);
	connect(cmbViewSelection, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(toggleViewState(const QString &)));

	QStringList mainMenuItemSpecifier;
	QString sActionText = "Switch to plain text view";
	mainMenuItemSpecifier.append("View");
	mainMenuItemSpecifier.append(sActionText);
	QAction *actionSwitchView = insertMenuAction(mainMenuItemSpecifier);
	if (actionSwitchView)
		connect(actionSwitchView, SIGNAL(triggered()), this, SLOT(switchToNativeView()), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));

	//File menu///
	//buttonFile=new QToolButton(this);
	//buttonFile->setText("File ");
	//buttonFile->setPopupMode(QToolButton::InstantPopup);
	//menuFile = new QMenu();//FileButton
	//menuFile->addAction(action_Quit);
	//buttonFile->setMenu(menuFile);
	//toolBar->addWidget(buttonFile);  
	////Edit menu///
	//buttonEdit=new QToolButton(this);
	//buttonEdit->setText("Edit ");
	//buttonEdit->setPopupMode(QToolButton::InstantPopup);
	//menuEdit = new QMenu();//EditButton
	////menuEdit->addAction(action_Test);
	//buttonEdit->setMenu(menuEdit);
	//toolBar->addWidget(buttonEdit);
	////View menu//
	//menuView = new QMenu();
	//menuView->addAction(action_ToggleViewState);
	//buttonView=new QToolButton(this);
	//buttonView->setText("View ");
	//buttonView->setPopupMode(QToolButton::InstantPopup);
	//buttonView->setMenu(menuView);
	//toolBar->addWidget(buttonView);
	//connect(action_Quit, SIGNAL(triggered()), this, SLOT(close()));
	//connect(action_ToggleViewState, SIGNAL(triggered()), this, SLOT(toggleViewState()));
	//connect(action_Test, SIGNAL(triggered()), this, SLOT(Test()));
}

QAction *ExperimentStructureVisualizer::insertMenuAction(const QStringList &sMenuActionPath)
{
	QAction *newActionItem = NULL;
	QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_REGISTERMAINMENUACTION_NAME, Qt::DirectConnection, Q_RETURN_ARG(QAction *, newActionItem), Q_ARG(QStringList, sMenuActionPath));
	if (newActionItem)
	{
		newActionItem->setStatusTip(sMenuActionPath.last());
		//newActionItem = new QAction(QIcon(":/resources/cut.png"), tr("Cu&t"), this);
		//newActionItem->setShortcuts(QKeySequence::Cut);
		return newActionItem;
	}
	return NULL;
}

void ExperimentStructureVisualizer::switchToNativeView()
{
	bool bNativeTextualView = true;
	QString sFilePath = "";//Use the active document filepath
	QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowReOpenSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, sFilePath), Q_ARG(bool, bNativeTextualView));
}

void ExperimentStructureVisualizer::toggleViewState(const QString &sNewState)
{
	if(gScene)
	{
		if (sNewState == EXPGRAPH_VIEWSTATE_BLOCKS)
		{
			if (gScene->getGraphViewState() == EXPVIS_VIEWSTATE_OBJECTS)
			{
				gScene->setGraphViewState(EXPVIS_VIEWSTATE_BLOCKTRIALS);
				drawGraph();
			}
		}
		else if (sNewState == EXPGRAPH_VIEWSTATE_OBJECTS)
		{
			if (gScene->getGraphViewState() == EXPVIS_VIEWSTATE_BLOCKTRIALS)
			{
				gScene->setGraphViewState(EXPVIS_VIEWSTATE_OBJECTS);
				drawGraph();
			}
		}
	}
}

void ExperimentStructureVisualizer::clearSelection()
{
	if (gScene)
	{
		if (gScene->selectedItems().count() > 0)
			gScene->clearSelection();
		else
			emit GraphItemSelectionChanged(QList<ExperimentVisualizerGraphItemTypeEnum>() << ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_UNDEFINED, QList<int>());
	}
}

void ExperimentStructureVisualizer::resizeStructureView(const int &nWidth, const int &nHeight)
{
	if(ui->graphicsView)
	{
		if (nWidth > nWidgetMarginWidth && nHeight > nWidgetMarginHeight)//in the case of no blocks the width is 0
		{
			ui->graphicsView->resetTransform();
			ui->graphicsView->setSceneRect(QRectF());
			ui->graphicsView->setFixedSize(nWidth - nWidgetMarginWidth, nHeight - nWidgetMarginHeight);
			QRectF currentSceneRect = gScene->itemsBoundingRect();
			ui->graphicsView->fitInView(currentSceneRect, Qt::KeepAspectRatio);
			ui->graphicsView->setSceneRect(currentSceneRect);
			ui->graphicsView->scale(dGraphViewScale, dGraphViewScale);
			ui->graphicsView->centerOn(currentSceneRect.center());
		}
	}
}

void ExperimentStructureVisualizer::createScene()
{
	gScene = new ExperimentStructureScene(this);
	ui->graphicsView->setScene(gScene);
	connect(gScene, SIGNAL(selectionChanged()), this, SLOT(itemSelectionChanged()),Qt::DirectConnection);
}

void ExperimentStructureVisualizer::itemSelectionChanged()
{
	if(gScene->items().isEmpty())
		return;
	for(int i=0;i<gScene->items().count();i++)
	{
		gScene->items().at(i)->setZValue(0);
	}
	int nSelectedItemCount = gScene->selectedItems().count();
	if(nSelectedItemCount > 0)
	{
		for(int i=0;i<nSelectedItemCount;i++)
		{
			gScene->selectedItems().at(i)->setZValue(1);

			expBlockItemStrc *pExpBlockItemStrc = getGraphBlockItemStruct((ExperimentGraphBlockItem*)gScene->selectedItems()[i]);//pExpGraphBlockItem);
			if(pExpBlockItemStrc)
			{
				QList<ExperimentVisualizerGraphItemTypeEnum> lGraphTypeEnums;
				QList<int> lItemsIds;
				lGraphTypeEnums.append(ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_BLOCK);
				lItemsIds.append(pExpBlockItemStrc->nId);
				emit GraphItemSelectionChanged(lGraphTypeEnums,lItemsIds);
				return;
			}	
			expConnItemStrc *pExpConnItemStrc = getGraphLoopItemStruct((ExperimentGraphLoopItem*)gScene->selectedItems()[i]);//
			if(pExpConnItemStrc)
			{
				QList<ExperimentVisualizerGraphItemTypeEnum> lGraphTypeEnums;
				QList<int> lItemsIds;
				lGraphTypeEnums.append(ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_BLOCK);
				lGraphTypeEnums.append(ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_LOOP);
				lItemsIds.append(pExpConnItemStrc->nSourceBlockId);
				lItemsIds.append(pExpConnItemStrc->nLoopId);
				emit GraphItemSelectionChanged(lGraphTypeEnums,lItemsIds);
				return;
			}
			expObjectItemStrc *pExpObjectItemStrc = getGraphObjectItemStruct((ExperimentGraphObjectItem*)gScene->selectedItems()[i]);//
			if(pExpObjectItemStrc)
			{
				QList<ExperimentVisualizerGraphItemTypeEnum> lGraphTypeEnums;
				QList<int> lItemsIds;
				lGraphTypeEnums.append(ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_OBJECT);
				lItemsIds.append(pExpObjectItemStrc->nId);
				emit GraphItemSelectionChanged(lGraphTypeEnums,lItemsIds);
				return;
			}
			expObjectMethodConnectionItemStrc *pExpObjectMethodConnectionItemStrc = getGraphMethodConnectionItemStruct((ExperimentGraphMethodConnectionItem*)gScene->selectedItems()[i]);//
			if(pExpObjectMethodConnectionItemStrc) 
			{
				QList<ExperimentVisualizerGraphItemTypeEnum> lGraphTypeEnums;
				QList<int> lItemsIds;
				lGraphTypeEnums.append(ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_METHOD_CONNECTION);
				lItemsIds.append(pExpObjectMethodConnectionItemStrc->nMethodConnectionID);
				emit GraphItemSelectionChanged(lGraphTypeEnums,lItemsIds);
				return;
			}
		}
	}
	emit GraphItemSelectionChanged(QList<ExperimentVisualizerGraphItemTypeEnum>() << ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_UNDEFINED,QList<int>());
}

void ExperimentStructureVisualizer::setupLayout()
{
	ui->mainLayout->setMenuBar(toolBar);
	if(graphViewLayout == NULL)
	{
		graphViewLayout = new QVBoxLayout();
		graphViewLayout->addWidget(ui->graphicsView);
	}
	ui->mainLayout->addLayout(graphViewLayout);
	ui->mainLayout->setContentsMargins(0,0,0,0);
	ui->mainLayout->setSpacing(0);
	setLayout(ui->mainLayout);
}

bool ExperimentStructureVisualizer::drawGraph()
{
	if (gScene == NULL)
	{
		createScene();
	}
	else
	{
		gScene->clear();
	}

	if(gScene->getGraphViewState() == EXPVIS_VIEWSTATE_BLOCKTRIALS)
	{
		int nCurrentBlockNumber = 0;
		QRectF nSingleBlockBoundingBox;
		int nAutoConnDistance = (int)EXPGRAPH_AUTOLOOP_DISTANCE;
		int nBlockDistance;
		int nAdditionalLoopConnsBlockDistance = 0;

		//START EXPERIMENT//
		expSceneItems.gStartGraphBlockItem = new ExperimentGraphBlockItem();
		nSingleBlockBoundingBox = expSceneItems.gStartGraphBlockItem->boundingRect();
		nBlockDistance = nSingleBlockBoundingBox.height() + nAutoConnDistance;
		expSceneItems.gStartGraphBlockItem->setType(GRAPHBLOCK_TYPE_START);
		expSceneItems.gStartGraphBlockItem->setPos(0,0);
		gScene->addItem(expSceneItems.gStartGraphBlockItem);	
		nCurrentBlockNumber++;

		//BLOCKS//
		if(expSceneItems.lBlocks.isEmpty() == false)
		{
			QString sTmpTooltip;
			for (int i=0;i<expSceneItems.lBlocks.count();i++)
			{
				sTmpTooltip = "";

				expSceneItems.lBlocks[i].nNumberOfInputLoops = getNumberOfBlockLoops(expSceneItems.lBlocks[i].nId,GRAPHBLOCK_LOOP_INPUT);
				nAdditionalLoopConnsBlockDistance = nAdditionalLoopConnsBlockDistance + (expSceneItems.lBlocks[i].nNumberOfInputLoops * EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE);

				expSceneItems.lBlocks[i].gGraphBlockItem = new ExperimentGraphBlockItem();
				expSceneItems.lBlocks[i].gGraphBlockItem->setType(GRAPHBLOCK_TYPE_BLOCK);
				expSceneItems.lBlocks[i].gGraphBlockItem->setCaption(expSceneItems.lBlocks[i].sName);
				expSceneItems.lBlocks[i].gGraphBlockItem->setPos(0,(nCurrentBlockNumber*nBlockDistance) + nAdditionalLoopConnsBlockDistance);
				gScene->addItem(expSceneItems.lBlocks[i].gGraphBlockItem);

				expSceneItems.lBlocks[i].nNumberOfOutputLoops = getNumberOfBlockLoops(expSceneItems.lBlocks[i].nId,GRAPHBLOCK_LOOP_OUTPUT);
				nAdditionalLoopConnsBlockDistance = nAdditionalLoopConnsBlockDistance + (expSceneItems.lBlocks[i].nNumberOfOutputLoops * EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE);

				//expSceneItems.lBlocks[i].gTextItem = gScene->addText(expSceneItems.lBlocks[i].sName);
				cBlockStructure *tmpBlockStruct = parsedExpStruct->getBlockPointerByID(expSceneItems.lBlocks[i].nId);
				if (tmpBlockStruct)
				{
					sTmpTooltip = "Name: " + tmpBlockStruct->getBlockName();
					sTmpTooltip = sTmpTooltip + "\nTrials: " + QString::number(tmpBlockStruct->getNumberOfTrials());
					sTmpTooltip = sTmpTooltip + "\nInt. Triggers: " + QString::number(tmpBlockStruct->getNumberOfInternalTriggers());
					sTmpTooltip = sTmpTooltip + "\nExt. Triggers: " + QString::number(tmpBlockStruct->getNumberOfExternalTriggers());				
					//sTmpTooltip = sTmpTooltip + "\nInput Loops: " + QString::number(expSceneItems.lBlocks[i].nNumberOfInputLoops);				
					//sTmpTooltip = sTmpTooltip + "\nOutput Loops: " + QString::number(expSceneItems.lBlocks[i].nNumberOfOutputLoops);				
				}
				expSceneItems.lBlocks[i].gGraphBlockItem->setToolTip(sTmpTooltip);
				nCurrentBlockNumber++;			
			}
		}
		//else
		//{
		//	return true;//no blocks defined, still true, just no blocks
		//}

		//END EXPERIMENT//
		expSceneItems.gEndGraphBlockItem = new ExperimentGraphBlockItem();
		expSceneItems.gEndGraphBlockItem->setType(GRAPHBLOCK_TYPE_END);
		expSceneItems.gEndGraphBlockItem->setPos(0,(nCurrentBlockNumber*nBlockDistance) + nAdditionalLoopConnsBlockDistance);
		gScene->addItem(expSceneItems.gEndGraphBlockItem);
		nCurrentBlockNumber++;

		//AUTO-CONNECTIONS//
		ExperimentGraphBlockItem *tmpSourceBlock = NULL;
		ExperimentGraphBlockItem *tmpTargetBlock = NULL;
		int nExtraInputConnDist;
		int nExtraOutputConnDist;
		int tmpY;
		for (int i=0;i<expSceneItems.lAutoConns.count();i++)
		{	
			tmpSourceBlock = NULL;
			tmpTargetBlock = NULL;
			if(expSceneItems.lAutoConns[i].nSourceBlockId >= 0)
			{
				if(expSceneItems.lAutoConns[i].nTargetBlockId >= 0)//Block to Block connection
				{
					tmpSourceBlock = getGraphBlockItemPointer(expSceneItems.lAutoConns[i].nSourceBlockId);
					if(tmpSourceBlock)
					{
						tmpTargetBlock = getGraphBlockItemPointer(expSceneItems.lAutoConns[i].nTargetBlockId);
						if(tmpTargetBlock)
						{
							expSceneItems.lAutoConns[i].gGraphConnectionItem = new ExperimentGraphLoopItem();
							expSceneItems.lAutoConns[i].gGraphConnectionItem->setAutoLoopType(true);
							expSceneItems.lAutoConns[i].gGraphConnectionItem->setEndPoint(QPoint(0,nAutoConnDistance));
							expSceneItems.lAutoConns[i].gGraphConnectionItem->setToolTip(EXPGRAPH_AUTOLOOP_TOOLTIP_TEXT);
							nExtraInputConnDist = (getNumberOfBlockLoops(expSceneItems.lAutoConns[i].nSourceBlockId,GRAPHBLOCK_LOOP_OUTPUT) * EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE);
							nExtraOutputConnDist = (getNumberOfBlockLoops(expSceneItems.lAutoConns[i].nTargetBlockId,GRAPHBLOCK_LOOP_INPUT) * EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE);
							tmpY = tmpSourceBlock->pos().y() + tmpSourceBlock->boundingRect().bottom();
							expSceneItems.lAutoConns[i].gGraphConnectionItem->setPos(0, tmpY + nExtraInputConnDist);
							gScene->addItem(expSceneItems.lAutoConns[i].gGraphConnectionItem);	

							if(nExtraInputConnDist > 0.0)
								gScene->addLine(0.0,tmpY,0.0,tmpY + nExtraInputConnDist,pLoopConnections);
							if(nExtraOutputConnDist > 0.0)
								gScene->addLine(0.0,tmpY + nExtraInputConnDist + nAutoConnDistance,0.0,tmpY + nExtraInputConnDist + nAutoConnDistance + nExtraOutputConnDist,pLoopConnections);
							continue;
						}
					}
				}
				else //End of experiment, no target
				{
					tmpSourceBlock = getGraphBlockItemPointer(expSceneItems.lAutoConns[i].nSourceBlockId);
					if(tmpSourceBlock && expSceneItems.gEndGraphBlockItem)
					{
						expSceneItems.lAutoConns[i].gGraphConnectionItem = new ExperimentGraphLoopItem();
						expSceneItems.lAutoConns[i].gGraphConnectionItem->setAutoLoopType(true);
						expSceneItems.lAutoConns[i].gGraphConnectionItem->setEndPoint(QPoint(0,nAutoConnDistance));
						nExtraInputConnDist = (getNumberOfBlockLoops(expSceneItems.lAutoConns[i].nSourceBlockId,GRAPHBLOCK_LOOP_OUTPUT) * EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE);
						int tmpY = tmpSourceBlock->pos().y() + tmpSourceBlock->boundingRect().bottom();
						//int tmpY = expSceneItems.gEndGraphBlockItem->pos().y() + expSceneItems.gEndGraphBlockItem->boundingRect().top();
						expSceneItems.lAutoConns[i].gGraphConnectionItem->setPos(0, tmpY + nExtraInputConnDist);
						gScene->addItem(expSceneItems.lAutoConns[i].gGraphConnectionItem);
						if(nExtraInputConnDist > 0.0)
							gScene->addLine(0.0,tmpY,0.0,tmpY + nExtraInputConnDist,pLoopConnections);
						continue;
					}
				}
			}
			else
			{
				if(expSceneItems.lAutoConns[i].nTargetBlockId >= 0)//Start of Experiment, no source
				{
					tmpTargetBlock = getGraphBlockItemPointer(expSceneItems.lAutoConns[i].nTargetBlockId);
					if(tmpTargetBlock && expSceneItems.gStartGraphBlockItem)
					{
						expSceneItems.lAutoConns[i].gGraphConnectionItem = new ExperimentGraphLoopItem();
						expSceneItems.lAutoConns[i].gGraphConnectionItem->setAutoLoopType(true);
						expSceneItems.lAutoConns[i].gGraphConnectionItem->setEndPoint(QPoint(0,nAutoConnDistance));
						nExtraOutputConnDist = (getNumberOfBlockLoops(expSceneItems.lAutoConns[i].nTargetBlockId,GRAPHBLOCK_LOOP_INPUT) * EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE);
						int tmpY = expSceneItems.gStartGraphBlockItem->pos().y() + expSceneItems.gStartGraphBlockItem->boundingRect().bottom();
						expSceneItems.lAutoConns[i].gGraphConnectionItem->setPos(0, tmpY);
						gScene->addItem(expSceneItems.lAutoConns[i].gGraphConnectionItem);
						if(nExtraOutputConnDist > 0.0)
							gScene->addLine(0.0,tmpY + nAutoConnDistance,0.0,tmpY + nAutoConnDistance + nExtraOutputConnDist,pLoopConnections);
						continue;
					}
				}
				else //probably no blocks defined, make a start->stop auto-connection
				{
					expSceneItems.lAutoConns[i].gGraphConnectionItem = new ExperimentGraphLoopItem();
					expSceneItems.lAutoConns[i].gGraphConnectionItem->setAutoLoopType(true);
					expSceneItems.lAutoConns[i].gGraphConnectionItem->setEndPoint(QPoint(0, nAutoConnDistance));
					int tmpY = expSceneItems.gStartGraphBlockItem->pos().y() + expSceneItems.gStartGraphBlockItem->boundingRect().bottom();
					expSceneItems.lAutoConns[i].gGraphConnectionItem->setPos(0, tmpY);
					gScene->addItem(expSceneItems.lAutoConns[i].gGraphConnectionItem);
					continue;
				}
			}
			//Could not handle the connection, so delete it
			expSceneItems.lAutoConns.removeAt(i);
			i--;
		}

		//LOOPS//
		expLoopDrawing.lMasterSideDrawOrderedLoops.clear();
		expLoopDrawing.lSlaveSideDrawOrderedLoops.clear();
		if(expSceneItems.lLoops.isEmpty() == false)
		{
			for (int i=0;i<expSceneItems.lLoops.count();i++)
			{
				//tmpSourceBlock = getGraphBlockItemPointer(expSceneItems.lLoops[i].nSourceBlockId);
				//tmpTargetBlock = getGraphBlockItemPointer(expSceneItems.lLoops[i].nTargetBlockId);
				cBlockStructure *pTmpBlockStructSource = parsedExpStruct->getBlockPointerByID(expSceneItems.lLoops[i].nSourceBlockId);
				cBlockStructure *pTmpBlockStructTarget = parsedExpStruct->getBlockPointerByID(expSceneItems.lLoops[i].nTargetBlockId);
				cLoopStructure *pTmpLoopStructCurrent = pTmpBlockStructSource->getLoopPointerByID(expSceneItems.lLoops[i].nLoopId);
				if((pTmpBlockStructSource) && (pTmpBlockStructTarget) && (pTmpLoopStructCurrent))
					insertLoopInGraphDrawingStruct(pTmpBlockStructSource, pTmpBlockStructTarget, pTmpLoopStructCurrent);
			}

			expConnItemStrc *tmpexpConnItemStrc;
			QList<expConnItemStrc> *pLexpConnItemStrc;
			QPen tmpPen;
			tmpPen.setColor("#00ff00");
			tmpPen.setWidth(5);
			int nLocation = 1;
			int tmpYSource;
			int tmpYTarget;
			int tmpXOffset;
			int nTmpValue;
			QString sTmpTooltip;

			QHash<int,int> hBlockIDInputConnCount;
			QHash<int,int> hBlockIDOutputConnCount;

			for (int j=0;j<2;j++)
			{
				if(j==0)
				{
					pLexpConnItemStrc = &expLoopDrawing.lMasterSideDrawOrderedLoops;
					nLocation = 1;
				}
				else if(j==1)
				{
					pLexpConnItemStrc = &expLoopDrawing.lSlaveSideDrawOrderedLoops;
					nLocation = -1;
				}

				int nCurrentLoopSourceDrawIndexExtraDistance = 0;
				int nCurrentLoopTargetDrawIndexExtraDistance = 0;
				for(int i=0;i<pLexpConnItemStrc->count();i++)
				{
					tmpexpConnItemStrc = &(*pLexpConnItemStrc)[i];
					tmpSourceBlock = getGraphBlockItemPointer(tmpexpConnItemStrc->nSourceBlockId);
					tmpTargetBlock = getGraphBlockItemPointer(tmpexpConnItemStrc->nTargetBlockId);
					expSceneItems.lLoops[i].gGraphConnectionItem = new ExperimentGraphLoopItem();
					expSceneItems.lLoops[i].gGraphConnectionItem->setAutoLoopType(false);

					if(hBlockIDInputConnCount.contains(tmpexpConnItemStrc->nTargetBlockId))
						nTmpValue = hBlockIDInputConnCount[tmpexpConnItemStrc->nTargetBlockId]+1;
					else
						nTmpValue = 0;
					hBlockIDInputConnCount.insert(tmpexpConnItemStrc->nTargetBlockId,nTmpValue);
					expSceneItems.lLoops[i].nTargetDrawIndex = nTmpValue;

					if(hBlockIDOutputConnCount.contains(tmpexpConnItemStrc->nSourceBlockId))
						nTmpValue = hBlockIDOutputConnCount[tmpexpConnItemStrc->nSourceBlockId]+1;
					else
						nTmpValue = 0;
					hBlockIDOutputConnCount.insert(tmpexpConnItemStrc->nSourceBlockId,nTmpValue);
					expSceneItems.lLoops[i].nSourceDrawIndex = nTmpValue;


					sTmpTooltip = "<undefined>";
					cBlockStructure *tmpBlockStruct = parsedExpStruct->getBlockPointerByID(tmpexpConnItemStrc->nSourceBlockId);
					if(tmpBlockStruct)
					{
						cLoopStructure *tmpLoopStruct = tmpBlockStruct->getLoopPointerByID(tmpexpConnItemStrc->nLoopId);
						if (tmpLoopStruct)
						{

							sTmpTooltip = "Name: " + tmpLoopStruct->getLoopName();
							sTmpTooltip = sTmpTooltip + "\nNumber: " + QString::number(tmpLoopStruct->getLoopNumber());
							sTmpTooltip = sTmpTooltip + "\nAmount: " + QString::number(tmpLoopStruct->getNumberOfLoops());
						}
					}

					expSceneItems.lLoops[i].gGraphConnectionItem->setToolTip(sTmpTooltip);

					nCurrentLoopSourceDrawIndexExtraDistance = (EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE * expSceneItems.lLoops[i].nSourceDrawIndex);
					tmpYSource = tmpSourceBlock->pos().y() - tmpSourceBlock->boundingRect().top() + EXPGRAPHLOOPITEM_LOOP_START_HEIGHT_DISTANCE;
					tmpXOffset = ((nLocation * EXPGRAPHLOOPITEM_LOOP_START_WIDTH_DISTANCE) + (nLocation * i*EXPGRAPHLOOPITEM_LOOP_DISTANCE));

					expSceneItems.lLoops[i].gGraphConnectionItem->setPos(tmpXOffset, tmpYSource + nCurrentLoopSourceDrawIndexExtraDistance);
					tmpYTarget = tmpYSource - (tmpTargetBlock->pos().y() + tmpTargetBlock->boundingRect().top() - EXPGRAPHLOOPITEM_LOOP_START_HEIGHT_DISTANCE);
					nCurrentLoopTargetDrawIndexExtraDistance = (EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE * expSceneItems.lLoops[i].nTargetDrawIndex);	
					if(tmpexpConnItemStrc->bReverseOrder)
						tmpXOffset = -1 * tmpXOffset;
					expSceneItems.lLoops[i].gGraphConnectionItem->setEndPoint(QPoint(0,-tmpYTarget - nCurrentLoopTargetDrawIndexExtraDistance - nCurrentLoopSourceDrawIndexExtraDistance), tmpXOffset);
					gScene->addItem(expSceneItems.lLoops[i].gGraphConnectionItem);				
				}
			}
		}
		emit GraphRedrawn();
		return true;
	}
	else if(gScene->getGraphViewState() == EXPVIS_VIEWSTATE_OBJECTS)
	{
		QHash<int,QList<strcMethodInfo>> hashObjectIDToUsedMethodConnections;
		QHash<int,QList<strcMethodInfo>> hashObjectIDToInitializations;
		QHash<int,QList<strcMethodInfo>> hashObjectIDToFinalizations;
		QHash<int,QList<strcMethodInfo>> hashObjectIDToUsedMethods;

		//OBJECT PARSING//
		if(expSceneItems.lObjects.isEmpty() == false)
		{
			QString sTmpTooltip;
			qreal rLastObjectBottomPos = 0;
			for(int i=0;i<expSceneItems.lObjects.count();i++)
			{
				if(expSceneItems.lObjects[i].lObjectMethodConnections.isEmpty() == false)
				{ //METHOD CONNECTIONS//
					foreach(expObjectMethodConnectionItemStrc* pExpObjectMethodConnectionItemStrc,expSceneItems.lObjects[i].lObjectMethodConnections)
					{
						QList<strcMethodInfo> tmpStrcMethodConnectionInfoList;
						strcMethodInfo tmpStrcMethodConnectionInfo;
						bool bSignatureFound;

						//Sources
						if(hashObjectIDToUsedMethodConnections.contains(pExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.nObjectID) == false)
							hashObjectIDToUsedMethodConnections.insert(pExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.nObjectID,QList<strcMethodInfo>());
						tmpStrcMethodConnectionInfoList = hashObjectIDToUsedMethodConnections[pExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.nObjectID];
						bSignatureFound = false;
						foreach(tmpStrcMethodConnectionInfo,tmpStrcMethodConnectionInfoList)
						{
							if(tmpStrcMethodConnectionInfo.sMethodSignature == pExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.sSignature)
							{
								bSignatureFound = true;
								break;
							}
						}
						if(bSignatureFound == false)
						{
							tmpStrcMethodConnectionInfo.sMethodSignature = pExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.sSignature;
							tmpStrcMethodConnectionInfo.mExperimentVisualizerMethodType = pExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.mType;
							hashObjectIDToUsedMethodConnections[pExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.nObjectID].append(tmpStrcMethodConnectionInfo);
						}
						//Targets
						if(hashObjectIDToUsedMethodConnections.contains(pExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.nObjectID) == false)
							hashObjectIDToUsedMethodConnections.insert(pExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.nObjectID,QList<strcMethodInfo>());
						tmpStrcMethodConnectionInfoList = hashObjectIDToUsedMethodConnections[pExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.nObjectID];
						bSignatureFound = false;
						foreach(tmpStrcMethodConnectionInfo,tmpStrcMethodConnectionInfoList)
						{
							if(tmpStrcMethodConnectionInfo.sMethodSignature == pExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.sSignature)
							{
								bSignatureFound = true;
								break;
							}
						}
						if(bSignatureFound == false)
						{
							tmpStrcMethodConnectionInfo.sMethodSignature = pExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.sSignature;
							tmpStrcMethodConnectionInfo.mExperimentVisualizerMethodType = pExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.mType;
							hashObjectIDToUsedMethodConnections[pExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.nObjectID].append(tmpStrcMethodConnectionInfo);
						}
					}
				}
				if(expSceneItems.lObjects[i].lObjectInitializations.isEmpty() == false)
				{ //INITIALIZATIONS//
					foreach(expObjectMethodItemStrc* pExpObjectInitializationItemStrc,expSceneItems.lObjects[i].lObjectInitializations)
					{

						QList<strcMethodInfo> tmpStrcMethodInfoList;
						strcMethodInfo tmpStrcMethodInfo;
						bool bSignatureFound;

						if(hashObjectIDToInitializations.contains(pExpObjectInitializationItemStrc->nObjectID) == false)
							hashObjectIDToInitializations.insert(pExpObjectInitializationItemStrc->nObjectID,QList<strcMethodInfo>());
						tmpStrcMethodInfoList = hashObjectIDToInitializations[pExpObjectInitializationItemStrc->nObjectID];
						bSignatureFound = false;
						foreach(tmpStrcMethodInfo,tmpStrcMethodInfoList)
						{
							if(tmpStrcMethodInfo.sMethodSignature == pExpObjectInitializationItemStrc->sSignature)
							{
								bSignatureFound = true;
								break;
							}
						}
						if(bSignatureFound == false)
						{
							tmpStrcMethodInfo.sMethodSignature = pExpObjectInitializationItemStrc->sSignature;
							tmpStrcMethodInfo.mExperimentVisualizerMethodType = pExpObjectInitializationItemStrc->mType;
							tmpStrcMethodInfo.bIsInitialization = true;
							hashObjectIDToInitializations[pExpObjectInitializationItemStrc->nObjectID].append(tmpStrcMethodInfo);
						}
					}
				}
				if(expSceneItems.lObjects[i].lObjectFinalizations.isEmpty() == false)
				{ //FINALIZATIONS//
					foreach(expObjectMethodItemStrc* pExpObjectFinalizationItemStrc,expSceneItems.lObjects[i].lObjectFinalizations)
					{

						QList<strcMethodInfo> tmpStrcMethodInfoList;
						strcMethodInfo tmpStrcMethodInfo;
						bool bSignatureFound;

						if(hashObjectIDToFinalizations.contains(pExpObjectFinalizationItemStrc->nObjectID) == false)
							hashObjectIDToFinalizations.insert(pExpObjectFinalizationItemStrc->nObjectID,QList<strcMethodInfo>());
						tmpStrcMethodInfoList = hashObjectIDToFinalizations[pExpObjectFinalizationItemStrc->nObjectID];
						bSignatureFound = false;
						foreach(tmpStrcMethodInfo,tmpStrcMethodInfoList)
						{
							if(tmpStrcMethodInfo.sMethodSignature == pExpObjectFinalizationItemStrc->sSignature)
							{
								bSignatureFound = true;
								break;
							}
						}
						if(bSignatureFound == false)
						{
							tmpStrcMethodInfo.sMethodSignature = pExpObjectFinalizationItemStrc->sSignature;
							tmpStrcMethodInfo.mExperimentVisualizerMethodType = pExpObjectFinalizationItemStrc->mType;
							tmpStrcMethodInfo.bIsFinalization = true;
							hashObjectIDToFinalizations[pExpObjectFinalizationItemStrc->nObjectID].append(tmpStrcMethodInfo);
						}
					}
				}
			}

			//OBJECTS//
			for (int i=0;i<expSceneItems.lObjects.count();i++)
			{
				sTmpTooltip = "";
				expSceneItems.lObjects[i].gGraphObjectItem = new ExperimentGraphObjectItem();
				expSceneItems.lObjects[i].gGraphObjectItem->setCaption(expSceneItems.lObjects[i].sName);// + " (" + expSceneItems.lObjects[i].sClass + ")");

				if(hashObjectIDToInitializations.contains(expSceneItems.lObjects[i].nId))
					hashObjectIDToUsedMethods[expSceneItems.lObjects[i].nId].append(hashObjectIDToInitializations[expSceneItems.lObjects[i].nId]);
				if(hashObjectIDToUsedMethodConnections.contains(expSceneItems.lObjects[i].nId))
					hashObjectIDToUsedMethods[expSceneItems.lObjects[i].nId].append(hashObjectIDToUsedMethodConnections[expSceneItems.lObjects[i].nId]);
				if(hashObjectIDToFinalizations.contains(expSceneItems.lObjects[i].nId))
					hashObjectIDToUsedMethods[expSceneItems.lObjects[i].nId].append(hashObjectIDToFinalizations[expSceneItems.lObjects[i].nId]);
				if(hashObjectIDToUsedMethods.contains(expSceneItems.lObjects[i].nId))
					expSceneItems.lObjects[i].gGraphObjectItem->setMethods(hashObjectIDToUsedMethods[expSceneItems.lObjects[i].nId]);

				expSceneItems.lObjects[i].gGraphObjectItem->setPos(0,rLastObjectBottomPos+EXPGRAPHOBJECTITEM_OBJECT_DISTANCE);//(nCurrentBlockNumber*nBlockDistance) + nAdditionalLoopConnsBlockDistance);
				rLastObjectBottomPos = rLastObjectBottomPos + expSceneItems.lObjects[i].gGraphObjectItem->boundingRect().height() + EXPGRAPHOBJECTITEM_OBJECT_DISTANCE;
				gScene->addItem(expSceneItems.lObjects[i].gGraphObjectItem);
				//if (tmpBlockStruct)
				//{
					sTmpTooltip = "Name: " + expSceneItems.lObjects[i].sName;
					sTmpTooltip = sTmpTooltip + "\nClass: " + expSceneItems.lObjects[i].sClass;				
				//}
				expSceneItems.lObjects[i].gGraphObjectItem->setToolTip(sTmpTooltip);				
			}
		}
		else
		{
			return true;//no objects defined..., just no objects
		}

		//OBJECTS METHOD CONNECTIONS//
		if(expSceneItems.lObjects.isEmpty() == false)
		{
			int nGlobalAllObjectMethodConnCounter = 0;
			for (int i=0;i<expSceneItems.lObjects.count();i++)
			{
				if(expSceneItems.lObjects[i].lObjectMethodConnections.isEmpty() == false)
				{
					ExperimentGraphObjectItem *pTmpSourceGraphObject = NULL;
					ExperimentGraphObjectItem *pTmpTargetGraphObject = NULL;
					int tmpXOffset;
					int tmpSourceMethodYOffsetPosition;
					int tmpTargetMethodYOffsetPosition;
					//if(hashObjectIDToUsedSignatures.contains(expSceneItems.lObjects[i].nId))
					//	expSceneItems.lObjects[i].gGraphObjectItem->setSignalsAndSlots(hashObjectIDToUsedSignatures[expSceneItems.lObjects[i].nId]);
					foreach(expObjectMethodConnectionItemStrc* pExpObjMethodConnItemStrc,expSceneItems.lObjects[i].lObjectMethodConnections)
					{
						//if(pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem)
						//{
						//	delete pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem;
						//	pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem = NULL;
						//}
						pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem = new ExperimentGraphMethodConnectionItem();
						pTmpSourceGraphObject = getGraphObjectItemPointer(pExpObjMethodConnItemStrc->cSourceConnectionMethod.nObjectID);
						pTmpTargetGraphObject = getGraphObjectItemPointer(pExpObjMethodConnItemStrc->cTargetConnectionMethod.nObjectID);
						if(pTmpSourceGraphObject && pTmpTargetGraphObject)
						{
							if((pExpObjMethodConnItemStrc->cSourceConnectionMethod.mType == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL) || (pExpObjMethodConnItemStrc->cSourceConnectionMethod.mType == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT))
								tmpSourceMethodYOffsetPosition = pTmpSourceGraphObject->getMethodLocationPosition(pExpObjMethodConnItemStrc->cSourceConnectionMethod.sSignature, pExpObjMethodConnItemStrc->cSourceConnectionMethod.mType);
							if(tmpSourceMethodYOffsetPosition >= 0)
							{
								if((pExpObjMethodConnItemStrc->cTargetConnectionMethod.mType == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL) || (pExpObjMethodConnItemStrc->cTargetConnectionMethod.mType == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT))
									tmpTargetMethodYOffsetPosition = pTmpTargetGraphObject->getMethodLocationPosition(pExpObjMethodConnItemStrc->cTargetConnectionMethod.sSignature, pExpObjMethodConnItemStrc->cTargetConnectionMethod.mType);
								if(tmpTargetMethodYOffsetPosition >= 0)
								{									
									tmpXOffset = EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_START_WIDTH_DISTANCE + (EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_DISTANCE * nGlobalAllObjectMethodConnCounter);
									pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem->setPos(tmpXOffset + (EXPGRAPHOBJECTITEM_OBJECT_WIDTH/2), tmpSourceMethodYOffsetPosition);
									pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem->setEndPoint(QPoint(0,tmpTargetMethodYOffsetPosition-tmpSourceMethodYOffsetPosition), -tmpXOffset);
									pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem->setToolTip(pExpObjMethodConnItemStrc->cSourceConnectionMethod.sSignature + " --> " + pExpObjMethodConnItemStrc->cTargetConnectionMethod.sSignature);
									gScene->addItem(pExpObjMethodConnItemStrc->gGraphObjectMethodConnItem);
									nGlobalAllObjectMethodConnCounter++;
								}
							}
						}
					}
				}
			}
		}
		emit GraphRedrawn();
		return true;
	}
	return false;
}

int ExperimentStructureVisualizer::getNumberOfBlockLoops(const int &nBlockID, const ExperimentGraphLoopTypeEnum &nConnectionType)
{
	int nFoundConnections = 0;
	if(expSceneItems.lLoops.isEmpty() == false)
	{
		if(nConnectionType == GRAPHBLOCK_LOOP_INPUT)
		{
			for (int i=0;i<expSceneItems.lLoops.count();i++)
			{
				if(expSceneItems.lLoops[i].nTargetBlockId == nBlockID)
					nFoundConnections++;
			}
		}
		else if(nConnectionType == GRAPHBLOCK_LOOP_OUTPUT)
		{
			for (int i=0;i<expSceneItems.lLoops.count();i++)
			{
				if(expSceneItems.lLoops[i].nSourceBlockId == nBlockID)
					nFoundConnections++;
			}
		}
	}
	return nFoundConnections;
}

bool ExperimentStructureVisualizer::insertLoopInGraphDrawingStruct(const cBlockStructure *pStrcSourceBlock, const cBlockStructure *pStrcTargetBlock, const cLoopStructure *pStrcLoop)
{
	if((pStrcSourceBlock==NULL) || (pStrcTargetBlock==NULL) || (pStrcLoop == NULL))
		return false;

	struct strcLastFit
	{
		int nLastFitIndex;
		int nMin;
		int nMax;
		strcLastFit()
		{
			nLastFitIndex = -1;
			nMin = -1;
			nMax = -1;
		}
	} sLastFit;
	QList<expConnItemStrc> *lDrawOrderedLoops = NULL;
	expConnItemStrc appendExpConnItem;
	bool bFitIndexDetermined = false;

	if(eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_UNNESTED_BOTHSIDES)
	{
		expConnItemStrc *tmpExpConnItem = NULL;
		bool bFitsInsideResult = false;
		bool bFitsOverResult = false;
		bool bFitsAsideResult = false;
		bool bIsCrossed = false;
		int nCurrentIndex;
		int nTestMin;
		int nTestMax;
		int nParsedMin;
		int nParsedMax;
		
		for (int j=0;j<2;j++)
		{
			if(bFitIndexDetermined)
				break;
		
			if(j==0)
				lDrawOrderedLoops = &expLoopDrawing.lMasterSideDrawOrderedLoops;
			else if(j==1)
				lDrawOrderedLoops = &expLoopDrawing.lSlaveSideDrawOrderedLoops;

			if(lDrawOrderedLoops->isEmpty())
			{
				appendExpConnItem.nSourceBlockId = pStrcSourceBlock->getBlockID();
				appendExpConnItem.nTargetBlockId = pStrcTargetBlock->getBlockID();
				appendExpConnItem.nLoopId = pStrcLoop->getLoopID();
				if(pStrcSourceBlock->getBlockNumber() < pStrcTargetBlock->getBlockNumber())
					appendExpConnItem.bReverseOrder = true;
				lDrawOrderedLoops->append(appendExpConnItem);
				return true;
			}
			else
			{
				sLastFit.nLastFitIndex = -1;
				sLastFit.nMin = -1;
				sLastFit.nMax = -1;
				bFitsInsideResult = false;
				bFitsOverResult = false;
				bFitsAsideResult = false;
				bIsCrossed = false;
				nTestMin = qMin(pStrcSourceBlock->getBlockID(),pStrcTargetBlock->getBlockID());
				nTestMax = qMax(pStrcSourceBlock->getBlockID(),pStrcTargetBlock->getBlockID());
				for(int i=0;i<lDrawOrderedLoops->count();i++)
				{
					nCurrentIndex = lDrawOrderedLoops->count()-(i+1);
					tmpExpConnItem = &((*lDrawOrderedLoops)[nCurrentIndex]);
					nParsedMin = qMin(tmpExpConnItem->nSourceBlockId, tmpExpConnItem->nTargetBlockId);
					nParsedMax = qMax(tmpExpConnItem->nSourceBlockId, tmpExpConnItem->nTargetBlockId);

					//Next: Does the Loop fit inside?
					if((nTestMin>=nParsedMin) && (nTestMin<=nParsedMax) && (nTestMax>=nParsedMin) && (nTestMax<=nParsedMax))
					{
						bFitsInsideResult = true;
						sLastFit.nLastFitIndex = nCurrentIndex;
						sLastFit.nMin = nParsedMin;
						sLastFit.nMax = nParsedMax;
						continue;//Let's see how deep we can go...
					}
					else
					{
						if(bFitsInsideResult)//Was it already previously fitted inside?
						{
							//Next: Does the Loop not cross?
							if(!(((nTestMin<=nParsedMin) && (nTestMax<nParsedMax)) || ((nTestMin>nParsedMin) && (nTestMax>=nParsedMax))))
								bFitIndexDetermined = true;
							else
								bIsCrossed = true;
							break;
						}
						else
						{
							//Next: Does the Loop fit completely over?
							if((nTestMin<=nParsedMin) && (nTestMax>=nParsedMax))
							{
								//Did it not yet fit completely over?
								if(bFitsOverResult == false)
								{
									sLastFit.nLastFitIndex = nCurrentIndex+1;
									sLastFit.nMin = nParsedMin;
									sLastFit.nMax = nParsedMax;
									bFitsOverResult = true;
								}
								continue;//Let's check the remaining...
							}
							//Next: Does the Loop occur aside parallel?	
							if(((nTestMin<=nParsedMin) && (nTestMax<=nParsedMin)) || ((nTestMin>=nParsedMax) && (nTestMax>=nParsedMax)))
							{
								sLastFit.nLastFitIndex = nCurrentIndex+1;
								sLastFit.nMin = nParsedMin;
								sLastFit.nMax = nParsedMax;
								bFitsAsideResult = true;
								continue;//Let's check the remaining...
							}
							break;
						}
					}				
				}
			}
			if((bIsCrossed == false) && (bFitsInsideResult || bFitsOverResult || bFitsAsideResult))
				bFitIndexDetermined = true;
		}
	}
	else if((eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_MASTERSIDE) || (eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_SLAVESIDE))
	/*{
		if(eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_MASTERSIDE)
			lDrawOrderedLoops = &expLoopDrawing.lMasterSideDrawOrderedLoops;
		else if(eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_SLAVESIDE)
			lDrawOrderedLoops = &expLoopDrawing.lSlaveSideDrawOrderedLoops;

		if(lDrawOrderedLoops->isEmpty())
		{
			appendExpConnItem.nSourceBlockId = nSourceBlockID;
			appendExpConnItem.nTargetBlockId = nTargetBlockID;
			appendExpConnItem.nLoopId = nLoopID;
			lDrawOrderedLoops->append(appendExpConnItem);
			return true;
		}
		else
		{
			sLastFit.nLastFitIndex = -1;
			sLastFit.nMin = -1;
			sLastFit.nMax = -1;
			/*bFitsInsideResult = false;
			bFitsOverResult = false;
			bFitsAsideResult = false;
			bIsCrossed = false;
			nTestMin = qMin(nSourceBlockID,nTargetBlockID);
			nTestMax = qMax(nSourceBlockID,nTargetBlockID);
			for(int i=0;i<lDrawOrderedLoops->count();i++)
			{
				if(lDrawOrderedLoops[i]->nSourceBlockId == nSourceBlockID)
				{
					if(lDrawOrderedLoops[i]->nLoopNumber<)
				}
			//	lDrawOrderedLoops
		//		sLastFit.nLastFitIndex = lDrawOrderedLoops->count();
		//		bFitIndexDetermined = true;
			}
		}
	}
	else if((eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_LOOPID_MASTERSIDE) || (eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_LOOPID_SLAVESIDE))else if((eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_MASTERSIDE) || (eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_SLAVESIDE))
	*/
	{
		if(eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_MASTERSIDE)
			lDrawOrderedLoops = &expLoopDrawing.lMasterSideDrawOrderedLoops;
		else if(eConnDrawOrder == GRAPHLOOP_DRAW_ORDER_NUMBER_SLAVESIDE)
			lDrawOrderedLoops = &expLoopDrawing.lSlaveSideDrawOrderedLoops;

		appendExpConnItem.nSourceBlockId = pStrcSourceBlock->getBlockID();
		appendExpConnItem.nTargetBlockId = pStrcTargetBlock->getBlockID();
		if(pStrcSourceBlock->getBlockNumber() < pStrcTargetBlock->getBlockNumber())
			appendExpConnItem.bReverseOrder = true;
		appendExpConnItem.nLoopId = pStrcLoop->getLoopID();
		lDrawOrderedLoops->append(appendExpConnItem);
		return true;
	}

	bool bRetVal = false;
	appendExpConnItem.nSourceBlockId = pStrcSourceBlock->getBlockID();
	appendExpConnItem.nTargetBlockId = pStrcTargetBlock->getBlockID();
	if(pStrcSourceBlock->getBlockNumber() < pStrcTargetBlock->getBlockNumber())
		appendExpConnItem.bReverseOrder = true;
	appendExpConnItem.nLoopId = pStrcLoop->getLoopID();
	if(bFitIndexDetermined)
	{
		lDrawOrderedLoops->insert(sLastFit.nLastFitIndex,appendExpConnItem);
		bRetVal = true;
	}
	else
	{
		lDrawOrderedLoops->insert(0,appendExpConnItem);
		bRetVal = false;
	}
	//expConnItemStrc *tmpAppendedExpConnItem;
	//if(bRetVal)
	//	tmpAppendedExpConnItem = &(*lDrawOrderedLoops)[sLastFit.nLastFitIndex];
	//else
	//	tmpAppendedExpConnItem = &(*lDrawOrderedLoops)[0];
	return bRetVal;
}

bool ExperimentStructureVisualizer::setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel) 
{
	pExperimentTreeModel = pExpTreeModel;
	connect(pExperimentTreeModel, SIGNAL(modelModified()), this, SLOT(reCreateAndParseExperimentStructure()), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
	//bool bResult = connect(pExperimentTreeModel, ExperimentTreeModel::modelModified, this, SLOT(reCreateAndParseExperimentStructure()), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
	if(gScene)
	{
		gScene->setExperimentTreeModel(pExperimentTreeModel);
	}
	return reCreateAndParseExperimentStructure();
}

bool ExperimentStructureVisualizer::reCreateAndParseExperimentStructure()
{
	QList<ExperimentTreeItem*> tmpFoundExpTreeItemList;
	if(parsedExpStruct)
	{
		delete parsedExpStruct;
		parsedExpStruct = NULL;
	}
	parsedExpStruct = new cExperimentStructure();
	bool bResult = ExperimentManager::createExperimentStructure(tmpFoundExpTreeItemList,pExperimentTreeModel,parsedExpStruct);
	if(bResult)
	{	
		return parseExperimentStructure();
	}
	return false;
}

bool ExperimentStructureVisualizer::parseExperimentStructure()
{
	if(parsedExpStruct==NULL)
		return false;

	//if (bPreserveSelection)
	//{
	//	if (gScene->items().isEmpty() == false)
	//	{
	//		int nSelectedItemCount = gScene->selectedItems().count();
	//		if (nSelectedItemCount > 0)
	//		{
	//			for (int i = 0; i < nSelectedItemCount; i++)
	//			{
	//				expBlockItemStrc *pExpBlockItemStrc = getGraphBlockItemStruct((ExperimentGraphBlockItem*)gScene->selectedItems()[i]);//pExpGraphBlockItem);
	//				if (pExpBlockItemStrc)
	//				{
	//					int nSelectedBlock = pExpBlockItemStrc->nId;
	//				}
	//			}
	//		}
	//	}
	//}

	resetExpScene();

	int nExpBlockCount = parsedExpStruct->getBlockCount();
	cBlockStructure *tmpBlock = NULL;
	cBlockStructure *lastBlock = NULL;
	int nNextSearchBlockNumber = 0;
	int nNextSearchLoopNumber = 0;
	expBlockItemStrc tmpBlockItem;
	expConnItemStrc tmpLoopItem;
	expConnItemStrc tmpAutoConnItem;

	expSceneItems.sExperimentName = parsedExpStruct->getExperimentName();
	if (nExpBlockCount > 0)
	{
		for (int i = 0; i < nExpBlockCount; i++)
		{
			//tmpAutoConnItem.gvEdge = NULL;
			tmpAutoConnItem.nSourceBlockId = -1;
			tmpAutoConnItem.nTargetBlockId = -1;
			tmpAutoConnItem.nLoopId = -1;

			lastBlock = tmpBlock;
			tmpBlock = NULL;
			tmpBlock = parsedExpStruct->getNextClosestBlockNumberByFromNumber(nNextSearchBlockNumber);
			if (tmpBlock)
			{
				if (lastBlock)
					tmpAutoConnItem.nSourceBlockId = lastBlock->getBlockID();
				tmpAutoConnItem.nTargetBlockId = tmpBlock->getBlockID();

				nNextSearchBlockNumber = tmpBlock->getBlockNumber() + 1;
				tmpBlockItem.nId = tmpBlock->getBlockID();
				tmpBlockItem.nNumber = tmpBlock->getBlockNumber();
				tmpBlockItem.sName = tmpBlock->getBlockName();
				//tmpBlockItem.gvNode = NULL;
				expSceneItems.lBlocks.append(tmpBlockItem);
			}
			expSceneItems.lAutoConns.append(tmpAutoConnItem);
			if (i == (nExpBlockCount - 1))
			{
				tmpAutoConnItem.nSourceBlockId = tmpBlock->getBlockID();
				tmpAutoConnItem.nTargetBlockId = -1;
				tmpAutoConnItem.nLoopId = -1;
				expSceneItems.lAutoConns.append(tmpAutoConnItem);
			}
		}
	}
	else //no blocks, make a start -> stop auto-connection
	{
		tmpAutoConnItem.nSourceBlockId = -1;
		tmpAutoConnItem.nTargetBlockId = -1;
		tmpAutoConnItem.nLoopId = -1;
		expSceneItems.lAutoConns.append(tmpAutoConnItem);
	}
	nNextSearchBlockNumber = 0;
	int nExpBlockLoopCount;
	for (int i=0;i<nExpBlockCount;i++)
	{
		tmpBlock = NULL;
		tmpBlock = parsedExpStruct->getNextClosestBlockNumberByFromNumber(nNextSearchBlockNumber);
		if(tmpBlock)
		{
			nNextSearchBlockNumber = tmpBlock->getBlockNumber() + 1;
			nExpBlockLoopCount = tmpBlock->getLoopCount();
			if(nExpBlockLoopCount > 0)
			{
				cLoopStructure *tmpLoop;
				int nTargetBlockID = -1;
				nNextSearchLoopNumber = 0;
				for (int j=0;j<nExpBlockLoopCount;j++)
				{
					tmpLoop = NULL;
					tmpLoop = tmpBlock->getNextClosestLoopIDByFromLoopNumber(nNextSearchLoopNumber);//getNextClosestLoopIDByFromID(nNextSearchLoopID);
					if (tmpLoop)
					{
						nNextSearchLoopNumber = tmpLoop->getLoopNumber() + 1;
						nTargetBlockID = tmpLoop->getTargetBlockID();
						tmpLoopItem.nLoopId = tmpLoop->getLoopID();
						tmpLoopItem.nLoopNumber = tmpLoop->getLoopNumber();
						tmpLoopItem.sName = tmpLoop->getLoopName();
						tmpLoopItem.nNumberOfLoops = tmpLoop->getNumberOfLoops();
						tmpLoopItem.nSourceBlockId = tmpBlock->getBlockID();
						tmpLoopItem.nTargetBlockId = nTargetBlockID;
						//tmpLoopItem.gvEdge = NULL;						
						expSceneItems.lLoops.append(tmpLoopItem);
					}
				}
			}
		}
	}

	int nExpObjectCount = parsedExpStruct->getObjectCount();
	if(nExpObjectCount>0)
	{
		QList<cObjectStructure *>tmpObjectList = parsedExpStruct->getObjectList();
		foreach(cObjectStructure *tmpObjectStrc, tmpObjectList)
		{
			if(tmpObjectStrc) 
			{
				expObjectItemStrc tmpObjectItem;
				tmpObjectItem.nId = tmpObjectStrc->getObjectID();
				tmpObjectItem.sName = tmpObjectStrc->getObjectName();
				tmpObjectItem.sClass = tmpObjectStrc->getObjectClass();
				//CONNECTIONS//
				QList<cMethodConnectionStructure*> *lTmpMethodConnStrc = parsedExpStruct->getObjectMethodConnectionList(tmpObjectStrc->getObjectID());
				if(lTmpMethodConnStrc)
				{
					if(lTmpMethodConnStrc->isEmpty() == false)
					{
						QList<expObjectMethodConnectionItemStrc*> lTmpexpObjectMethodConnectionItemStrc;
						foreach(cMethodConnectionStructure* pTmpMethodConnectionStructure,*lTmpMethodConnStrc)
						{
							expObjectMethodConnectionItemStrc* pTmpExpObjectMethodConnectionItemStrc = new expObjectMethodConnectionItemStrc;
							pTmpExpObjectMethodConnectionItemStrc->nMethodConnectionID = pTmpMethodConnectionStructure->getMethodConnectionID();
							pTmpExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.nObjectID = pTmpMethodConnectionStructure->getSourceObjectID();
							pTmpExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.nObjectID = pTmpMethodConnectionStructure->getTargetObjectID();
							pTmpExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.mType = (ExperimentStructuresNameSpace::MethodType)pTmpMethodConnectionStructure->getSourceMethodType();
							pTmpExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.mType = (ExperimentStructuresNameSpace::MethodType)pTmpMethodConnectionStructure->getTargetMethodType();
							pTmpExpObjectMethodConnectionItemStrc->cSourceConnectionMethod.sSignature = pTmpMethodConnectionStructure->getSourceSignature();
							pTmpExpObjectMethodConnectionItemStrc->cTargetConnectionMethod.sSignature = pTmpMethodConnectionStructure->getTargetSignature();
							tmpObjectItem.lObjectMethodConnections.append(pTmpExpObjectMethodConnectionItemStrc);
						}
					}
				}
				//INITIALIZATIONS//
				QList<cMethodStructure*> *lTmpInitStrc = parsedExpStruct->getObjectInitializationList(tmpObjectStrc->getObjectID());
				if(lTmpInitStrc)
				{
					if(lTmpInitStrc->isEmpty() == false)
					{
						QList<expObjectMethodItemStrc*> lTmpexpObjectMethodItemStrc;
						foreach(cMethodStructure* pTmpMethodStructure,*lTmpInitStrc)
						{
							expObjectMethodItemStrc* pTmpExpObjectMethodItemStrc = new expObjectMethodItemStrc;
							pTmpExpObjectMethodItemStrc->nMethodID = pTmpMethodStructure->getMethodID();
							pTmpExpObjectMethodItemStrc->nObjectID = pTmpMethodStructure->getMethodObjectID();
							pTmpExpObjectMethodItemStrc->mType = (ExperimentStructuresNameSpace::MethodType)pTmpMethodStructure->getMethodType();
							pTmpExpObjectMethodItemStrc->sSignature = pTmpMethodStructure->getMethodSignature();
							tmpObjectItem.lObjectInitializations.append(pTmpExpObjectMethodItemStrc);
						}
					}
				}
				//FINALIZATIONS//
				QList<cMethodStructure*> *lTmpFinalStrc = parsedExpStruct->getObjectFinalizationList(tmpObjectStrc->getObjectID());
				if(lTmpFinalStrc)
				{
					if(lTmpFinalStrc->isEmpty() == false)
					{
						QList<expObjectMethodItemStrc*> lTmpexpObjectMethodItemStrc;
						foreach(cMethodStructure* pTmpMethodStructure,*lTmpFinalStrc)
						{
							expObjectMethodItemStrc* pTmpExpObjectMethodItemStrc = new expObjectMethodItemStrc;
							pTmpExpObjectMethodItemStrc->nMethodID = pTmpMethodStructure->getMethodID();
							pTmpExpObjectMethodItemStrc->nObjectID = pTmpMethodStructure->getMethodObjectID();
							pTmpExpObjectMethodItemStrc->mType = (ExperimentStructuresNameSpace::MethodType)pTmpMethodStructure->getMethodType();
							pTmpExpObjectMethodItemStrc->sSignature = pTmpMethodStructure->getMethodSignature();
							tmpObjectItem.lObjectFinalizations.append(pTmpExpObjectMethodItemStrc);
						}
					}
				}
				expSceneItems.lObjects.append(tmpObjectItem);
			}
		}
	}

	if(gScene)
	{
		gScene->setExperimentStructure(parsedExpStruct);
	}
	return drawGraph();
}

ExperimentGraphBlockItem *ExperimentStructureVisualizer::getGraphBlockItemPointer(const int &nBlockID)
{
	for(int i=0;i<expSceneItems.lBlocks.count();i++)
	{
		if(expSceneItems.lBlocks[i].nId == nBlockID)
		{
			return expSceneItems.lBlocks[i].gGraphBlockItem;
		}
	}
	return NULL;
}

ExperimentGraphObjectItem *ExperimentStructureVisualizer::getGraphObjectItemPointer(const int &nObjectID)
{
	for(int i=0;i<expSceneItems.lObjects.count();i++)
	{
		if(expSceneItems.lObjects[i].nId == nObjectID)
		{
			return expSceneItems.lObjects[i].gGraphObjectItem;
		}
	}
	return NULL;
}

ExperimentStructureVisualizer::expBlockItemStrc *ExperimentStructureVisualizer::getGraphBlockItemStruct(const ExperimentGraphBlockItem *pExpGraphBlockItem)
{
	for(int i=0;i<expSceneItems.lBlocks.count();i++)
	{
		if(expSceneItems.lBlocks[i].gGraphBlockItem == pExpGraphBlockItem)
		{
			return &expSceneItems.lBlocks[i];
		}
	}
	return NULL;
}

ExperimentStructureVisualizer::expConnItemStrc *ExperimentStructureVisualizer::getGraphLoopItemStruct(const ExperimentGraphLoopItem *pExpGraphConnItem)
{
	for(int i=0;i<expSceneItems.lLoops.count();i++)
	{
		if(expSceneItems.lLoops[i].gGraphConnectionItem == pExpGraphConnItem)
		{
			return &expSceneItems.lLoops[i];
		}
	}
	return NULL;
}

ExperimentStructureVisualizer::expObjectMethodConnectionItemStrc *ExperimentStructureVisualizer::getGraphMethodConnectionItemStruct(const ExperimentGraphMethodConnectionItem *pExpGraphMethodConnItem)
 {
	 for(int j=0;j<expSceneItems.lObjects.count();j++)
	 {
		 for(int i=0;i<expSceneItems.lObjects[j].lObjectMethodConnections.count();i++)
		 {
			 if(expSceneItems.lObjects[j].lObjectMethodConnections[i]->gGraphObjectMethodConnItem == pExpGraphMethodConnItem)
			 {
				 return expSceneItems.lObjects[j].lObjectMethodConnections[i];
			 }
		 }
	 }
	 return NULL;
 }

ExperimentStructureVisualizer::expObjectItemStrc *ExperimentStructureVisualizer::getGraphObjectItemStruct(const ExperimentGraphObjectItem *pExpGraphObjectItem)
{
	for(int i=0;i<expSceneItems.lObjects.count();i++)
	{
		if(expSceneItems.lObjects[i].gGraphObjectItem == pExpGraphObjectItem)
		{
			return &expSceneItems.lObjects[i];
		}
	}
	return NULL;
}