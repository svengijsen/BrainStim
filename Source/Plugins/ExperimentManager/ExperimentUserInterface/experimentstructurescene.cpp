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

#include "experimentstructurescene.h"
#include <QEvent>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QInputDialog>
#include "experimentgraphblockitem.h"
#include "experimentgraphloopitem.h"
#include "experimentgraphobjectitem.h"
#include "experimentgraphmethodconnectionitem.h"
#include "blockloopsdialog.h"
#include "experimentobjectsdialog.h"
#include "objectconnectionsdialog.h"
//#include "PropertySettingsWidgetContainer.h"

ExperimentStructureScene::ExperimentStructureScene(ExperimentStructureVisualizer *parent) : QGraphicsScene((QObject*)parent), pBlockLoopsDialog(NULL), pExperimentObjectsDialog(NULL), pObjectConnectionsDialog(NULL), pExpStruct(NULL), pExperimentTreeModel(NULL), parentExpStructVis(parent)
{
	currentGraphViewState = EXPVIS_VIEWSTATE_BLOCKTRIALS;
}

ExperimentStructureScene::~ExperimentStructureScene()
{
	if(pBlockLoopsDialog)
		pBlockLoopsDialog = NULL;
	if(pExperimentObjectsDialog)
		pExperimentObjectsDialog = NULL;
}

bool ExperimentStructureScene::event(QEvent *event)
{
	bool bResult = false;
	//if(event->type() == QEvent::GraphicsSceneContextMenu) 
	//{
	//	//QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
	//	//qDebug() << "Ate key press" << keyEvent->key();
	//	return true;
	//}
	//else 
	if((event->type() == QEvent::Enter) || (event->type() == QEvent::GraphicsSceneMousePress) || (event->type() == QEvent::GraphicsSceneMouseRelease))
	{
		bResult = QGraphicsScene::event(event);
		foreach(QGraphicsView *pView,views())
			pView->viewport()->setCursor(Qt::ArrowCursor);
		return bResult;
	}
	//QEvent::GraphicsSceneContextMenu
	return QGraphicsScene::event(event);
}

//void ExperimentStructureScene::itemSelectionChanged()
//{
//	int nSelectedItemCount = selectedItems().count();
//	for(int i=0;i<nSelectedItemCount;i++)
//	{
//		selectedItems().at()
//	}
//}

void ExperimentStructureScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
	QMenu mContexMenu;
	QAction* initialParametersAction = mContexMenu.addAction(QIcon(":/resources/parameters.png"), "Initialized Parameters");
	//QMenu* mViewMenu = mContexMenu.addMenu(QIcon(":/resources/view.png"), "View");
	QMenu* mBlocksMenu = mContexMenu.addMenu(QIcon(":/resources/blocks.png"), "Blocks");
	QMenu* mLoopsMenu = mContexMenu.addMenu(QIcon(":/resources/loops.png"), "Loops");
	QMenu* mObjectsMenu = mContexMenu.addMenu(QIcon(":/resources/objects.png"), "Objects");
	QMenu* mConnectionsMenu = mContexMenu.addMenu(QIcon(":/resources/connections.png"), "Connections");
	QAction *selectedItemAction = NULL;
	//QAction *toggleViewAction;
	QAction* configureLoopsAction;
	QAction* configureObjectsAction;
	QAction *addBlockAction;
	QAction *addMultipleBlocksAction;
	//QAction *removeBlockAction;
	QAction *removeBlocksAction;
	//QAction *addObjectAction;
	QAction *removeObjectConnectionAction;
	QAction *configureConnectionsAction;

	mBlocksMenu->setEnabled(false);
	mLoopsMenu->setEnabled(false);
	mObjectsMenu->setEnabled(false);
	mConnectionsMenu->setEnabled(false);
	initialParametersAction->setEnabled(false);

	QList<QGraphicsItem *> gItemList = selectedItems();
	//QGraphicsItem *gItem = itemAt(contextMenuEvent->scenePos(),QTransform());//item at mouse
	QList<ExperimentGraphBlockItem*> gSelectedBlockItems;
	QList<int> lSelectedBlockIds;
	QList<int> lSelectedObjectIds;
	QList<ExperimentGraphLoopItem*> gSelectedAutoConnectionItems;
	QList<ExperimentGraphLoopItem*> gSelectedLoopConnectionItems;
	QList<ExperimentGraphObjectItem*> gSelectedObjectItems;
	QList<ExperimentGraphMethodConnectionItem*> gSelectedMethodConnectionItems;
	
	PropertySettingsWidgetContainer *expParamWidgets = NULL;
	QString sSelectedObjectClassName = "";

	//if(gItem)
	//if(gItemList.isEmpty() == false)
	//{
	foreach(QGraphicsItem *graphItem,gItemList)
	{
		if (graphItem->type() == ExperimentManagerNameSpace::TypeBlockItem)
			gSelectedBlockItems.append(qgraphicsitem_cast<ExperimentGraphBlockItem*>(graphItem));
		else if (graphItem->type() == ExperimentManagerNameSpace::TypeAutoLoopConnectionItem)
			gSelectedAutoConnectionItems.append(qgraphicsitem_cast<ExperimentGraphLoopItem*>(graphItem));
		else if (graphItem->type() == ExperimentManagerNameSpace::TypeLoopConnectionItem)
			gSelectedLoopConnectionItems.append(qgraphicsitem_cast<ExperimentGraphLoopItem*>(graphItem));
		else if (graphItem->type() == ExperimentManagerNameSpace::TypeObjectItem)
			gSelectedObjectItems.append(qgraphicsitem_cast<ExperimentGraphObjectItem*>(graphItem));
		else if (graphItem->type() == ExperimentManagerNameSpace::TypeMethodConnectionItem)
			gSelectedMethodConnectionItems.append(qgraphicsitem_cast<ExperimentGraphMethodConnectionItem*>(graphItem));
	}
	if (parentExpStructVis)
	{
		foreach(ExperimentGraphBlockItem *tmpExpGraphBlockItem, gSelectedBlockItems)
		{
			if (tmpExpGraphBlockItem)
				lSelectedBlockIds.append(parentExpStructVis->getGraphBlockItemStruct(tmpExpGraphBlockItem)->nId);
		}
		foreach(ExperimentGraphObjectItem *tmpExpGraphObjectItem, gSelectedObjectItems)
		{
			if (tmpExpGraphObjectItem)
				lSelectedObjectIds.append(parentExpStructVis->getGraphObjectItemStruct(tmpExpGraphObjectItem)->nId);
		}
	}

	if(getGraphViewState() == EXPVIS_VIEWSTATE_BLOCKTRIALS)
	{
		addBlockAction = mBlocksMenu->addAction(QIcon(":/resources/add.png"), "Add New");
		addMultipleBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/addmultiple.png"), "Add New(multiple)");
		if(gSelectedBlockItems.isEmpty() == false)//Blocks selected?
		{
			//if(bUsedColumnIndexesInSelectionRanges.count() == 1)
			//removeBlockAction = mBlocksMenu->addAction(QIcon(":/resources/remove.png"), "Remove Selected");
			//else if(bUsedColumnIndexesInSelectionRanges.count() > 1)
			removeBlocksAction = mBlocksMenu->addAction(QIcon(":/resources/remove.png"), "Remove Selected");
		}
		mBlocksMenu->setEnabled(true);

		mLoopsMenu->setEnabled(true);
		if(gSelectedAutoConnectionItems.isEmpty() == false)//Auto-Connections selected?
		{

		}
		else
		{

		}

		if(gSelectedLoopConnectionItems.isEmpty() == false)//Loop-Connections selected?
		{

		}		
		else
		{

		}
		configureLoopsAction = mLoopsMenu->addAction(QIcon(":/resources/configure.png"), "Configure Loop(s)");
	}
	else if(getGraphViewState() == EXPVIS_VIEWSTATE_OBJECTS)
	{
		mObjectsMenu->setEnabled(true);
		if(gSelectedObjectItems.isEmpty() == false)//Objects selected?
		{
			if (gSelectedObjectItems.count() == 1)//Single Object selected
			{
				if (parentExpStructVis)
				{
					if (gSelectedObjectItems[0])
					{
						expParamWidgets = PropertySettingsWidgetContainer::instance();
						sSelectedObjectClassName = parentExpStructVis->getGraphObjectItemStruct(gSelectedObjectItems[0])->sClass;
						if (expParamWidgets->hasExperimentParameterDefinition(sSelectedObjectClassName))
							initialParametersAction->setEnabled(true);
					}
				}
			}
		}
		//addObjectAction = mObjectsMenu->addAction(QIcon(":/resources/add.png"), "Add New");
		configureObjectsAction = mObjectsMenu->addAction(QIcon(":/resources/configure.png"), "Configure Object(s)");
		
		mConnectionsMenu->setEnabled(true);
		if(gSelectedMethodConnectionItems.isEmpty() == false)//Method Connections selected?
		{
			removeObjectConnectionAction = mConnectionsMenu->addAction(QIcon(":/resources/remove.png"), "Remove Selected");
		}
		configureConnectionsAction = mConnectionsMenu->addAction(QIcon(":/resources/configure.png"), "Configure Connection(s)");
	}

	//toggleViewAction = mViewMenu->addAction(QIcon(":/resources/orientation.png"), "Toggle Orientation");

	selectedItemAction = mContexMenu.exec(contextMenuEvent->screenPos());//contextMenuEvent->globalPos()// contextMenuEvent->scenePos() is available
	if (selectedItemAction)
	{
		if((selectedItemAction == addBlockAction)||(selectedItemAction == addMultipleBlocksAction))
		{
			//if(pExpTreeModel)
			//{
				int nBlocksToAdd;
				if(selectedItemAction == addMultipleBlocksAction)
				{
					bool bDialogResult;
					nBlocksToAdd = QInputDialog::getInt(NULL, tr("How many blocks?"),	tr("Blocks to add:"), 0, 0, 1000, 1, &bDialogResult);
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
					if(pExperimentTreeModel)
						bResult = pExperimentTreeModel->addExperimentBlocks(nBlocksToAdd);
				}			
			//}
		}
		else if(selectedItemAction == removeBlocksAction)//(selectedItemAction == removeBlockAction) || 
		{
			if (pExperimentTreeModel)
			{
				pExperimentTreeModel->removeExperimentBlocks(lSelectedBlockIds);
				if (parentExpStructVis)
					parentExpStructVis->clearSelection();
			}
		}
		//else if(selectedItemAction == toggleViewAction)
		//{
			//bVerticalViewEnabled = !bVerticalViewEnabled;
			//reparseModel();
		//}
		else if(selectedItemAction == configureLoopsAction)
		{
			pBlockLoopsDialog = new BlockLoopsDialog();
			if(pBlockLoopsDialog->setExperimentTreeModel(pExperimentTreeModel))
				pBlockLoopsDialog->exec();
		}
		else if(selectedItemAction == configureObjectsAction)
		{
			pExperimentObjectsDialog = new ExperimentObjectsDialog();
			if(pExperimentObjectsDialog->setExperimentTreeModel(pExperimentTreeModel))
				pExperimentObjectsDialog->exec();
		}
		else if(selectedItemAction == configureConnectionsAction)
		{
			pObjectConnectionsDialog = new ObjectConnectionsDialog();
			if(pObjectConnectionsDialog->setExperimentTreeModel(pExperimentTreeModel))
				pObjectConnectionsDialog->exec();
		}
		else if (selectedItemAction == initialParametersAction)
		{
			if (parentExpStructVis && pExpStruct)
			{
				cBlockStructure *tmpBlockStrcPointer = pExpStruct->getNextClosestBlockNumberByFromNumber(0);
				if (tmpBlockStrcPointer)
				{
					int nFirstBlockID = tmpBlockStrcPointer->getBlockID();
					emit parentExpStructVis->GraphItemSelectionChanged(QList<ExperimentVisualizerGraphItemTypeEnum>() << ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_BLOCK << ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_OBJECT << ExperimentVisualizerGraphItemTypeEnum::EXPVIS_TYPE_PARAMETERS, QList<int>() << nFirstBlockID << lSelectedObjectIds[0]);
				}
			}
			/*
			PropertySettingsWidget *tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sSelectedObjectClassName);
			tmpParametersWidget->resetParameterModifiedFlagsAndValues();
			if (tmpParametersWidget)
			{
				tmpParametersWidget->setWindowModality(Qt::WindowModal);
				tmpParametersWidget->parseDependencies();
				tmpParametersWidget->setAutoDepencyParsing(true);
				//tmpParametersWidget->resizeParameterView(200,1000);
				tmpParametersWidget->show();
				//if (child->hasChildren())
				//{
				//	//Set the values
				//	int nChildCount = child->childCount();
				//	QString sName = "";
				//	QString sValue = "";
				//	for (int j = 0; j < nChildCount; j++)
				//	{
				//		if (item->child(i)->child(j)->getName().toLower() == NAME_TAG)
				//		{
				//			sName = item->child(i)->child(j)->getValue();
				//		}
				//		else if (item->child(i)->child(j)->getName().toLower() == VALUE_TAG)
				//		{
				//			sValue = item->child(i)->child(j)->getValue();
				//		}
				//		if ((sName.isEmpty() || sValue.isEmpty()) == false)
				//		{
				//			//bool bResult = 
				//			tmpParametersWidget->setParameter(sName, sValue, true, true);
				//		}
				//	}
				//}
			}*/
		}
	}
}

void ExperimentStructureScene::setExperimentStructure(cExperimentStructure *pExperimentStruct) 
{
	pExpStruct = pExperimentStruct;
}

void ExperimentStructureScene::setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel)
{
	pExperimentTreeModel = pExpTreeModel;
}