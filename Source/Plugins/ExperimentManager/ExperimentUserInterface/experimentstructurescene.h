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

#ifndef EXPERIMENTSTRUCTURESCENE_H
#define EXPERIMENTSTRUCTURESCENE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include "experimentstructurevisualizer.h"

class BlockLoopsDialog;
class ExperimentObjectsDialog;
class cExperimentStructure;
class ExperimentTreeModel;
class ObjectConnectionsDialog;
class ExperimentStructureVisualizer;
class PropertySettingsWidget;

enum ExperimentVisualizerGraphViewState
{
	EXPVIS_VIEWSTATE_BLOCKTRIALS	= 0,
	EXPVIS_VIEWSTATE_OBJECTS		= 1
};

class ExperimentStructureScene : public QGraphicsScene
{
	public:
		ExperimentStructureScene(ExperimentStructureVisualizer *parent);
		~ExperimentStructureScene();

		void setExperimentStructure(cExperimentStructure *pExperimentStruct);
		void setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel);
		void setGraphViewState(const ExperimentVisualizerGraphViewState &newGraphViewState) {currentGraphViewState = newGraphViewState;};
		ExperimentVisualizerGraphViewState getGraphViewState() {return currentGraphViewState;};

	protected:
		bool event(QEvent *event);
		virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent);

	//private slots:
	//	void itemSelectionChanged();
private:
	BlockLoopsDialog *pBlockLoopsDialog; 
	ExperimentObjectsDialog *pExperimentObjectsDialog;
	ObjectConnectionsDialog *pObjectConnectionsDialog;
	cExperimentStructure *pExpStruct;
	ExperimentTreeModel *pExperimentTreeModel;
	ExperimentVisualizerGraphViewState currentGraphViewState;
	ExperimentStructureVisualizer *parentExpStructVis;
};

#endif // EXPERIMENTSTRUCTURESCENE_H
