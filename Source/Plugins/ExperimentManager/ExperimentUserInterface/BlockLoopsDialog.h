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

#ifndef BLOCKLOOPSDIALOG_H
#define BLOCKLOOPSDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QHBoxLayout>
#include "ExperimentTreeModel.h"
#include "ExperimentParameterDefinition.h"

namespace Ui {class BlockLoopsDialog;};

class ExperimentParameterWidgets;
class ExperimentParameterVisualizer;

class BlockLoopsDialog : public QDialog
{
	Q_OBJECT

public:
	BlockLoopsDialog(QWidget *parent = NULL);
	~BlockLoopsDialog();

	bool setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel);

private slots:
	void onTreeWidgetSelectionChanged();
	void decreaseSelectedLoopNumber();
	void increaseSelectedLoopNumber();
	void addSelectedLoop();
	void removeSelectedLoop();
	void newLoopConfigurationChanged(int nChangedIndex);
	void blockLoopDefinitionItemEditFinished(const QString &sParamName, const QString &sNewValue);
	void checkReparseModel();

private:
	struct strcBlockLoopInfo 
	{
		int nBlockID;
		int nLoopID;
		cLoopStructure *pLoopStruct;
		QTreeWidgetItem *pLoopTreeWidgetItem;
		strcBlockLoopInfo()
		{
			nBlockID = -1;
			nLoopID = -1;
			pLoopStruct = NULL;
			pLoopTreeWidgetItem = NULL;
		}
	};

	struct strcBlockInfo 
	{
		int nBlockID;
		cBlockStructure *pBlockStruct;
		strcBlockInfo()
		{
			nBlockID = -1;
			pBlockStruct = NULL;
		}
	};

	bool parseExperimentStructure();
	void clearAllParsedStructures();
	void prepareAndHideBlockLoopConfiguration();
	bool showLoopWidget(const strcBlockLoopInfo &blockLoopInfoStrc);
	bool reCreateAndParseExperimentStructure();
	void changeSelectedLoopNumber(const int &nLoopNumberCorrection);

	Ui::BlockLoopsDialog *ui;

	bool bIsParsing;
	QTreeWidget *twCurrentLoops;
	ExperimentTreeModel *pExperimentTreeModel;
	cExperimentStructure *pCurrentExpStructure;
	QMap<int, QTreeWidgetItem *> mapBlockIdToBlockTreeWidgetItems;
	QHash<int, strcBlockLoopInfo> hashIntToBlockLoopInfo;
	QHash<int, strcBlockInfo> hashComboBlockIndexToBlockStructure;
	QHBoxLayout *layoutTreeWidgetParent;
	ExperimentParameterWidgets *pExpParamWidgets;
	ExperimentParameterVisualizer *pParametersWidget;
	int nCurrentBlockLoopSelectionIdentifier;
};

#endif // BLOCKLOOPSDIALOG_H
