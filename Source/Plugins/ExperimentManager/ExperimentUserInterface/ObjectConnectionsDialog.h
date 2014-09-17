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

#ifndef OBJECTCONNECTIONSDIALOG_H
#define OBJECTCONNECTIONSDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QHBoxLayout>
#include "ExperimentTreeModel.h"
#include "ExperimentParameterDefinition.h"

namespace Ui {class ObjectConnectionsDialog;};

class ExperimentParameterWidgets;
class ExperimentParameterVisualizer;

class ObjectConnectionsDialog : public QDialog
{
	Q_OBJECT

public:
	ObjectConnectionsDialog(QWidget *parent = NULL);
	~ObjectConnectionsDialog();

	bool setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel);

private slots:
	void onTreeWidgetSelectionChanged();
	void addSelectedConnection();
	void removeSelectedConnection();
	void fromObjectChanged(int nChangedIndex);
	void toObjectChanged(int nChangedIndex);
	void ObjectConnectionDefinitionItemEditFinished(const QString &sParamName, const QString &sNewValue);
	void checkReparseModel();
	void validatePossibleConnection();

private:
	struct strcObjectConnectionInfo 
	{
		int nConnectionID;
		int nSourceObjectID;
		cMethodConnectionStructure *pConnectionStruct;
		QTreeWidgetItem *pConnectionTreeWidgetItem;
		strcObjectConnectionInfo()
		{
			nConnectionID = -1;
			nSourceObjectID = -1;
			pConnectionStruct = NULL;
			pConnectionTreeWidgetItem = NULL;
		}
	};

	struct strcObjectInfoStruct 
	{
		int nObjectID;
		cObjectStructure *pObjectStruct;
		strcObjectInfoStruct()
		{
			nObjectID = -1;
			pObjectStruct = NULL;
		}
	};

	bool parseExperimentStructure();
	void clearAllParsedStructures();
	void prepareAndHideObjectConnectionConfiguration();
	bool showConnectionWidget(const strcObjectConnectionInfo &ObjectConnectionInfoStrc);
	bool reCreateAndParseExperimentStructure();
	QStringList getObjectMethodsList(const QString &sObjectClass, QHash<int, QMetaMethod> &hashIndexToMetataMethod);

	Ui::ObjectConnectionsDialog *ui;

	bool bIsParsing;
	QTreeWidget *twCurrentConnections;
	ExperimentTreeModel *pExperimentTreeModel;
	cExperimentStructure *pCurrentExpStructure;
	QMap<int, QTreeWidgetItem *> mapBlockIdToBlockTreeWidgetItems;
	QHash<int, strcObjectConnectionInfo> hashIntToObjectConnectionInfo;
	QHash<int, strcObjectInfoStruct> hashComboObjectIndexToObjectStructure;
	QHash<int, QMetaMethod> hashFromComboMethodIndexToMetaMethod;
	QHash<int, QMetaMethod> hashToComboMethodIndexToMetaMethod;
	QHBoxLayout *layoutTreeWidgetParent;
	ExperimentParameterWidgets *pExpParamWidgets;
	ExperimentParameterVisualizer *pParametersWidget;
	int nCurrentObjectConnectionSelectionIdentifier;
};

#endif // OBJECTCONNECTIONSDIALOG_H
