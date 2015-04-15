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

#ifndef EXPERIMENTOBJECTSDIALOG_H
#define EXPERIMENTOBJECTSDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QHBoxLayout>
#include "experimenttreemodel.h"

namespace Ui {class ExperimentObjectsDialog;};

class PropertySettingsWidgetContainer;
class PropertySettingsWidget;

class ExperimentObjectsDialog : public QDialog
{
	Q_OBJECT

public:
	ExperimentObjectsDialog(QWidget *parent = NULL);
	~ExperimentObjectsDialog();

	bool setExperimentTreeModel(ExperimentTreeModel *pExpTreeModel);

private slots:
	void onObjectSelectionChanged();
	void onNewObjectNameChanged(const QString &sText);
	void moveUpSelectedInit();
	void moveDownSelectedInit();
	void moveUpSelectedFinit();
	void moveDownSelectedFinit();
	void addSelectedObject();
	void addSelectedInit();
	void addSelectedFinit();
	void removeSelectedObject();
	void configureSelectedObjectInitFinitParam(const bool &bIsInit);
	void removeSelectedObjectInitFinit(const bool &bIsInit);
	void newObjectConfigurationChanged(int nChangedIndex);
	void experimentObjectDefinitionItemEditFinished(const QString &sParamName, const QString &sNewValue);
	void experimentObjectInitItemEditFinished(const QString &sParamName, const QString &sNewValue);
	void experimentObjectFinitItemEditFinished(const QString &sParamName, const QString &sNewValue);
	void checkReparseModel();
	void onObjectInitSelectionChanged();
	void onObjectFinitSelectionChanged();
	void onRemoveSelectedObjectInit();
	void onRemoveSelectedObjectFinit();
	void onConfigureSelectedObjectInitParams();
	void onConfigureSelectedObjectFinitParams();

private:
	struct strcExperimentObjectInfo 
	{
		int nObjectID;
		cObjectStructure *pObjectStruct;
		QList<cMethodStructure*> *pObjectInitializationList;
		QList<cMethodStructure*> *pObjectFinalizationList;
		QTreeWidgetItem *pObjectTreeWidgetItem;
		strcExperimentObjectInfo()
		{
			nObjectID = -1;
			pObjectStruct = NULL;
			pObjectTreeWidgetItem = NULL;
			pObjectInitializationList = NULL;
			pObjectFinalizationList = NULL;
		}
	};

	bool parseExperimentStructure();
	void clearAllParsedStructures();
	void prepareAndHideObjectConfiguration();
	bool showObjectWidgets(const strcExperimentObjectInfo &objectInfoStrc);
	bool reCreateAndParseExperimentStructure();
	void fetchAvailableExperimentObjectClasses();
	void handleObjectInitFinitSelectionChanged(bool bIsInit);
	void configureAvailableObjectInitFinits();
	void handleExperimentObjectInitFinitEditFinished(const QString &sParamName, const QString &sNewValue, const bool bIsInit);
	bool addInitFinit(const bool &bIsInit);
	bool moveSelectedInitFinit(const bool &bMoveUp, const bool &bIsInit);

	Ui::ExperimentObjectsDialog *ui;

	bool bIsObjectDeleting;
	bool bIsParsing;
	bool bDisableEditUpdates;
	QTreeWidget *twCurrentObjects;
	ExperimentTreeModel *pExperimentTreeModel;
	cExperimentStructure *pCurrentExpStructure;
	QMap<int, QTreeWidgetItem *> mapObjectIdToObjectTreeWidgetItems;
	QHash<int, strcExperimentObjectInfo> hashIntToExperimentObjectInfo;
	QHBoxLayout *layoutObjectDeclarationTreeWidgetParent;
	QHBoxLayout *layoutObjectInitTreeWidgetParent;
	QHBoxLayout *layoutObjectFinitTreeWidgetParent;
	PropertySettingsWidgetContainer *pExpParamWidgets;
	PropertySettingsWidget *pObjectDeclarationWidget;
	PropertySettingsWidget *pObjectInitsWidget;
	PropertySettingsWidget *pObjectFinitsWidget;
	PropertySettingsWidget **pObjectInitFinitWidget;
	int nCurrentObjectSelectionIdentifier;
	int nCurrentObjectInitSelectionIdentifier;
	int nCurrentObjectFinitSelectionIdentifier;
};

#endif // EXPERIMENTOBJECTSDIALOG_H
