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

#ifndef EXPERIMENTSTRUCTUREVISUALIZER_H
#define EXPERIMENTSTRUCTUREVISUALIZER_H

#include <QWidget>
#include "experimentstructures.h"
#include "experimentstructurescene.h"
#include "experimentgraphobjectitem.h"
#include "experimentgraphblockitem.h"
#include "experimentgraphloopitem.h"
#include "experimentgraphmethodconnectionitem.h"

#define EXPGRAPH_AUTOLOOP_DISTANCE						100.0
#define EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE		EXPGRAPH_AUTOLOOP_DISTANCE
#define EXPGRAPH_AUTOLOOP_TOOLTIP_TEXT					"Auto-connection"
#define EXPGRAPH_VIEWSTATE_BLOCKS						"Blocks"
#define EXPGRAPH_VIEWSTATE_OBJECTS						"Objects"

enum ExperimentVisualizerGraphItemTypeEnum
{
	EXPVIS_TYPE_UNDEFINED			= 0,
	EXPVIS_TYPE_BLOCK				= 1,
	EXPVIS_TYPE_LOOP				= 2,
	EXPVIS_TYPE_OBJECT				= 3,
	EXPVIS_TYPE_METHOD_CONNECTION	= 4
};

class QMenu;
class QToolBar;
class QToolButton;
class QVBoxLayout;
class QGraphicsViewEc;
class ExperimentTreeModel;
class ExperimentStructureScene;

namespace Ui {class ExperimentStructureVisualizer;};

class ExperimentStructureVisualizer : public QWidget
{
	Q_OBJECT

signals:
	void destroyed(QWidget*);
	void GraphItemSelectionChanged(QList<ExperimentVisualizerGraphItemTypeEnum>, QList<int>);
	void GraphRedrawn();

public:
	explicit ExperimentStructureVisualizer(QWidget *parent = NULL);
	~ExperimentStructureVisualizer();

	bool setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel);
	void setVisualMargin(const int &nNewMarginWidth, const int &nNewMarginHeight) { nWidgetMarginWidth = nNewMarginWidth; nWidgetMarginHeight = nNewMarginHeight; };
	int getAdditionalMenuHeight() const;

public slots:
	void resizeStructureView(const int &nWidth, const int &nHeight);
	void clearSelection();

private slots:
	//void nodeContextMenu(QGVNode* node);
	//void nodeDoubleClick(QGVNode* node);
	//void Test();
	void itemSelectionChanged();
	bool reCreateAndParseExperimentStructure();
	void toggleViewState(const QString &sNewState);
	void switchToNativeView();

private:
	Ui::ExperimentStructureVisualizer *ui;

	struct expBlockItemStrc 
	{
		QString sName;
		int nId;
		int nNumber;
		int nNumberOfInputLoops;
		int nNumberOfOutputLoops;
		ExperimentGraphBlockItem *gGraphBlockItem;
		//QGraphicsTextItem *gTextItem;
		expBlockItemStrc()
		{
			sName = "";
			nId = -1;
			nNumber = -1;
			gGraphBlockItem = NULL;
			//gTextItem = NULL;
			nNumberOfInputLoops = 0;
			nNumberOfOutputLoops = 0;
		}
	};

	struct expObjectMethodItemStrc
	{
		int nMethodID;
		int nObjectID;
		int nOrderNumber;
		QString sSignature;
		ExperimentStructuresNameSpace::MethodType mType;
		expObjectMethodItemStrc()
		{
			nMethodID = -1;
			nObjectID = -1;
			nOrderNumber = -1;
			sSignature = "";
			mType = ExperimentStructuresNameSpace::METHOD_TYPE_UNDEFINED;
		}
	};

	struct expObjectMethodConnectionItemStrc
	{
		int nMethodConnectionID;
		expObjectMethodItemStrc cSourceConnectionMethod;
		expObjectMethodItemStrc cTargetConnectionMethod;
		ExperimentGraphMethodConnectionItem *gGraphObjectMethodConnItem;
		expObjectMethodConnectionItemStrc()
		{
			nMethodConnectionID = -1;
			gGraphObjectMethodConnItem = NULL;
		}
	};

	struct expObjectItemStrc
	{
		QString sName;
		QString sClass;
		int nId;
		QList<expObjectMethodConnectionItemStrc*> lObjectMethodConnections;
		QList<expObjectMethodItemStrc*> lObjectInitializations;
		QList<expObjectMethodItemStrc*> lObjectFinalizations;
		ExperimentGraphObjectItem *gGraphObjectItem;
		expObjectItemStrc()
		{
			sName = "";
			sClass = "";
			nId = -1;
			gGraphObjectItem = NULL;
		}
	};

	struct expConnItemStrc
	{
		QString sName;
		int nLoopId;
		int nLoopNumber;
		int nNumberOfLoops;
		ExperimentGraphLoopItem *gGraphConnectionItem;
		int nSourceBlockId;
		int nTargetBlockId;
		int nSourceDrawIndex;
		int nTargetDrawIndex;
		bool bReverseOrder;
		expConnItemStrc()
		{
			sName = "";
			nLoopId = -1;
			nLoopNumber = -1;
			nNumberOfLoops = -1;
			gGraphConnectionItem = NULL;
			nSourceBlockId = -1;
			nTargetBlockId = -1;
			nSourceDrawIndex = 0;
			nTargetDrawIndex = 0;
			bReverseOrder = false;//In case of a forward loop (by loop number defined) this is true...
		}
	};

	struct expSceneItemStrc 
	{
		QString sExperimentName;
		QList<expBlockItemStrc> lBlocks;
		QList<expConnItemStrc> lLoops;
		QList<expConnItemStrc> lAutoConns;
		QList<expObjectItemStrc> lObjects;
		ExperimentGraphBlockItem *gStartGraphBlockItem;
		ExperimentGraphBlockItem *gEndGraphBlockItem;
		expSceneItemStrc()
		{
			gStartGraphBlockItem = NULL;
			gEndGraphBlockItem = NULL;
		}
	};

	struct expParsedLoopDrawing
	{		
		QHash<int, QList<int>> hBlockIDTargetLoopBlockIDs;
		QList<expConnItemStrc> lMasterSideDrawOrderedLoops;
		QHash<int, int> hBlockIDMasterSideCount;
		QList<expConnItemStrc> lSlaveSideDrawOrderedLoops;
		QHash<int, int> hBlockIDSlaveSideCount;
	};

	bool parseExperimentStructure();
	bool drawGraph();
	void configureActions(bool bCreate);
	void setupMenuAndActions();
	void createScene();
	void setupLayout();
	void resetExpScene();
	bool insertLoopInGraphDrawingStruct(const cBlockStructure *pStrcSourceBlock, const cBlockStructure *pStrcTargetBlock, const cLoopStructure *pStrcLoop);
	int getNumberOfBlockLoops(const int &nBlockID, const ExperimentGraphLoopTypeEnum &nConnectionType);
	QAction *insertMenuAction(const QStringList &sMenuActionPath, QIcon *iMenuIcon);

	int nWidgetMarginWidth;
	int nWidgetMarginHeight;
	qreal dGraphViewScale;
	ExperimentGraphLoopItemDrawOrder eConnDrawOrder;
	QAction *action_Quit;
	QAction *action_ToggleViewState;
	//QAction *action_Test;
	QToolBar *toolBar;
	QMenu *menuFile;
	QMenu *menuEdit;
	QMenu *menuView;
	QToolButton *buttonFile;
	QToolButton *buttonEdit;
	QToolButton *buttonView;
	QVBoxLayout *graphViewLayout;
	ExperimentStructureScene *gScene;
	cExperimentStructure *parsedExpStruct;
	ExperimentTreeModel *pExperimentTreeModel;
	expSceneItemStrc expSceneItems;
	expParsedLoopDrawing expLoopDrawing;
	QPen pLoopConnections;

public:
	//void showEvent ( QShowEvent * event );
	//void resizeEvent(QResizeEvent *event);
	ExperimentGraphBlockItem *getGraphBlockItemPointer(const int &nBlockID);
	ExperimentGraphObjectItem *getGraphObjectItemPointer(const int &nObjectID);
	expBlockItemStrc *getGraphBlockItemStruct(const ExperimentGraphBlockItem *pExpGraphBlockItem);
	expConnItemStrc *getGraphLoopItemStruct(const ExperimentGraphLoopItem *pExpGraphBlockItem);
	expObjectItemStrc *getGraphObjectItemStruct(const ExperimentGraphObjectItem *pExpGraphObjectItem);
	expObjectMethodConnectionItemStrc *getGraphMethodConnectionItemStruct(const ExperimentGraphMethodConnectionItem *pExpGraphMethodConnItem);
};

#endif // EXPERIMENTSTRUCTUREVISUALIZER_H
