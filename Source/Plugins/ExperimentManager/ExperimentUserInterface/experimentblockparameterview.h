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

#ifndef EXPERIMENTBLOCKPARAMETERVIEW_H
#define EXPERIMENTBLOCKPARAMETERVIEW_H

#include <QTableWidgetItem>
#include "mainappinfo.h"
#include "experimentstructures.h"
//sven #include "experimentparameterdefinition.h"
#include "propertysettingdefinition.h"//sven 


#define BLOCKPARAMVIEW_DEFAULTBLOCKHEADER_COUNT			4
//#define BLOCKPARAMVIEW_BLOCKNUMBER_ROWORCOLUMNINDEX	0
#define BLOCKPARAMVIEW_BLOCKNAME_ROWORCOLUMNINDEX		0
#define BLOCKPARAMVIEW_BLOCKTRIALS_ROWORCOLUMNINDEX		1
#define BLOCKPARAMVIEW_BLOCKINTTRGS_ROWORCOLUMNINDEX	2
#define BLOCKPARAMVIEW_BLOCKEXTTRGS_ROWORCOLUMNINDEX	3
#define BLOCKPARAMVIEW_BLOCKPARAMS_PREHEADERSTRING		"Block"
#define BLOCKPARAMVIEW_BLOCKNUMBER_HEADERSTRING			"Block Nr. "

class ExperimentTreeModel;
class ExperimentTreeItem;
class cExperimentStructure;
class ObjectParameterDialog;

class ExperimentBlockParameterView : public CustomChildDockTableWidget
{
	Q_OBJECT

signals:
	void destroyed(QWidget*);
	void onFocusTable();

public:
	ExperimentBlockParameterView(QWidget *parent = NULL, ExperimentTreeModel *pExperimentTreeModel = NULL);
	~ExperimentBlockParameterView();

	bool parseExperimentStructure(cExperimentStructure *ExpStruct);
	bool parseExperimentBlockParameters(const QList<ExperimentTreeItem*> &tmpExpTreeItemList);
	bool setExperimentObjects(const QList<ExperimentStructuresNameSpace::strcExperimentObject> &lExperimentObjects);

private slots:
	void cellItemEditFinished(const QString&sParamName, const QString&sNewValue);
	void customCellItemEditFinished(const QString& sNewValue);
	void showContextMenu(const QPoint& pos);
	void cellOpenedForEdit(const int &nRow, const int &nColumn);
	void checkReparseModel();
	void reparseModel();
	
protected:
	void currentChanged(const QModelIndex &current, const QModelIndex &previous);
	void focusInEvent(QFocusEvent* event);

private:
	void configureEditHandling(const bool &bEnable);
	bool appendExperimentBlockParameterChanges();
	void initTableSetup();

	struct strcParameterBlockChanges
	{
		int nBlockID;
		QString sValue;
		strcParameterBlockChanges()
		{
			nBlockID = -1;
			sValue = "";
		}
	};

	struct strcRowOrColumnInfo
	{
		int nRowOrColumnIndex;
		int nObjectID;
		QString sHeader;
		PropertySettingEditingType eParamEditType;
		PropertySettingDefinitionStrc *strcParamDef;
		strcRowOrColumnInfo()
		{
			nRowOrColumnIndex = -1;
			nObjectID = -1;
			sHeader = "";
			strcParamDef = NULL;
			eParamEditType = PSET_DEFINED;
		}
	};

	struct strcExperimentObjectInfo
	{
		ExperimentStructuresNameSpace::strcExperimentObject ObjectGlobalInfo;
		PropertySettingDefinition *pObjectParamDefContainer;
		QString sObjectName;
		strcExperimentObjectInfo()
		{
			pObjectParamDefContainer = NULL;
			sObjectName = "<undefined>";
		}
	};

	ObjectParameterDialog *pObjectParameterDialog;
	ExperimentTreeModel *pExpTreeModel;
	cExperimentStructure *parsedExpStruct;
	QStringList lColumnHeaders;	
	QStringList lRowHeaders;	
	bool bVerticalViewEnabled;
	bool bEditHandlingEnabled;
	QMutex mutexEditHandlingEnabled;
	bool bDoReparseModel;	
	bool bCellOpenedForEdit;

	QHash<int, strcExperimentObjectInfo> hashObjectIdExperimentObjectInfo;
	QMap<QString, QList<strcParameterBlockChanges>> mapUniqueParamIDExpParamBlockChanges;
	QHash<int, int> hashBlockIdRowOrColumnIndex;
	QHash<int, int> hashRowOrColumnIndexBlockId;
	QHash<int, QString> hashRowOrColumnIndexObjectIDParamName;
	QHash<QString, strcRowOrColumnInfo> hashObjectParameterRowOrColumnIndex;
	QColor cChangedParameter;
	QColor cChangedCustomParameter;
	QColor cInheritedParameter;
	QLineEdit *leCustomParamEdit;
};

#endif // EXPERIMENTBLOCKPARAMETERVIEW_H
