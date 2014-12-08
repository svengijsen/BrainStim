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

	QList<QGraphicsItem *> gItemList = selectedItems();
	//QGraphicsItem *gItem = itemAt(contextMenuEvent->scenePos(),QTransform());//item at mouse
	QList<ExperimentGraphBlockItem*> gSelectedBlockItems;
	QList<int> lSelectedBlockIds;
	QList<ExperimentGraphLoopItem*> gSelectedAutoConnectionItems;
	QList<ExperimentGraphLoopItem*> gSelectedLoopConnectionItems;
	QList<ExperimentGraphObjectItem*> gSelectedObjectItems;
	QList<ExperimentGraphMethodConnectionItem*> gSelectedMethodConnectionItems;

	//if(gItem)
	//if(gItemList.isEmpty() == false)
	//{
	foreach(QGraphicsItem *graphItem,gItemList)
	{
		if(graphItem->type() == ExperimentStructureItemType::TypeBlockItem)
			gSelectedBlockItems.append(qgraphicsitem_cast<ExperimentGraphBlockItem*>(graphItem));
		else if(graphItem->type() == ExperimentStructureItemType::TypeAutoLoopConnectionItem)
			gSelectedAutoConnectionItems.append(qgraphicsitem_cast<ExperimentGraphLoopItem*>(graphItem));
		else if(graphItem->type() == ExperimentStructureItemType::TypeLoopConnectionItem)
			gSelectedLoopConnectionItems.append(qgraphicsitem_cast<ExperimentGraphLoopItem*>(graphItem));
		else if(graphItem->type() == ExperimentStructureItemType::TypeObjectItem)
			gSelectedObjectItems.append(qgraphicsitem_cast<ExperimentGraphObjectItem*>(graphItem));
		else if(graphItem->type() == ExperimentStructureItemType::TypeMethodConnectionItem)
			gSelectedMethodConnectionItems.append(qgraphicsitem_cast<ExperimentGraphMethodConnectionItem*>(graphItem));
	}
	if (parentExpStructVis)
	{
		foreach(ExperimentGraphBlockItem *tmpExpGraphBlockItem, gSelectedBlockItems)
		{
			if (tmpExpGraphBlockItem)
				lSelectedBlockIds.append(parentExpStructVis->getGraphBlockItemStruct(tmpExpGraphBlockItem)->nId);
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