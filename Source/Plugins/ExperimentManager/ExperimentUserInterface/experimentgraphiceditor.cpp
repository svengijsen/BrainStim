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

#include <QMenu>
#include <QCloseEvent>
#include <QToolButton>
#include <QAction>
#include <QHeaderView>
#include <QFileDialog>
#include <QToolBar>
#include <QTreeView>
#include <QScrollArea>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QSplitter>
#include <QMessageBox>
#include "finddialog.h"
#include "attributewidget.h"
#include "experimenttreefilterproxymodel.h"
#include "experimentmanager.h"
#include "experimentgraphiceditor.h"
#include "experimenttreeitem.h"
#include "experimentstructurevisualizer.h"
#include "propertysettingswidget.h"
#include "propertysettingswidgetcontainer.h"
#include "experimentblockparameterview.h"

ExperimentGraphicEditor::ExperimentGraphicEditor(QWidget *parent) : QWidget(parent)
{
	actionToggle_ListViewDockVisibility = NULL;
	actionToggle_TableViewDockVisibility = NULL;
	pCustomPropertiesDockWidget = NULL;
	pCustomParamTableDockWidget = NULL;
	pCustomExperimentTreeDockWidget = NULL;
	treeView = NULL;
	attWidget = NULL;
	gridLayout = NULL;
	tblWidgetView = NULL;
	toolBar = NULL;
	findDlg = NULL;
	dynamicGraphicWidget = NULL;
	pCurrentExpTreeModel = NULL;
	filterModel = NULL;
	mainLayout = NULL;
	actionNew_File = NULL;
	action_Open_File = NULL;
	action_Save = NULL;
	action_Close_File = NULL;
	actionFind = NULL;
	action_Quit = NULL;
	actionAdd_Node = NULL;
	actionAdd_Subnode = NULL;
	action_Remove_Node = NULL;
	actionAdd_Attribute = NULL;
	actionToggleBlocksView = NULL;
	//actionSwitchToDefaultView = NULL;
	menuFile = NULL;
	menuEdit = NULL;
	menuView = NULL;
	buttonView = NULL;
	buttonFile = NULL;
	buttonEdit = NULL;
	expManager = NULL;
	tmpExpStruct = NULL;
	expStructVisualizer = NULL;
	expBlockParamView = NULL;
	tmpExpObjectParamDefs = NULL;
	pCustomParamListTabWidget = NULL;
	bShowGraphicalTreeView = false;
	currentViewSettings.bSkipComments = true;
	currentViewSettings.bSkipEmptyAttributes = false;
	nLastCustomParamListTabWidgetIndex = -1;
	bCustomParamListTabWidgetConfigured = false;
	nAdditionalMenuHeight = 0;

	setAttribute(Qt::WA_DeleteOnClose);
	configureActions(true);
	setupExperimentTreeView();
	setupMenuAndActions();
	setupLayout();
	setNewFileName("");
}

ExperimentGraphicEditor::~ExperimentGraphicEditor()
{
	emit IsClosing(getCurrentFileName(), false);
	emit IsDestructing(this);
	configureActions(false);
	staticGraphicWidgetsHashTable.clear();
	if(gridLayout)
	{
		delete gridLayout;
		gridLayout = NULL;
	}
	if(treeView)
	{
		delete treeView;
		treeView = NULL;
	}
	if(attWidget)
	{
		delete attWidget;
		attWidget = NULL;
	}
	if(dynamicGraphicWidget)
	{
		delete dynamicGraphicWidget;
		dynamicGraphicWidget = NULL;
	}
	if(tblWidgetView)
	{
		delete tblWidgetView;
		tblWidgetView = NULL;
	}
	if(menuFile)
	{
		delete menuFile;
		menuFile = NULL;
	}
	if(menuEdit)
	{
		delete menuEdit;
		menuEdit = NULL;
	}
	if(menuView)
	{
		delete menuView;
		menuView = NULL;
	}
	if(buttonView)
	{
		delete buttonView;
		buttonView = NULL;
	}
	if(buttonFile)
	{
		delete buttonFile;
		buttonFile = NULL;
	}
	if(buttonEdit)
	{
		delete buttonEdit;
		buttonEdit = NULL;
	}
	if(toolBar)
	{
		delete toolBar;
		toolBar = NULL;
	}
	if(pCurrentExpTreeModel)
		pCurrentExpTreeModel = NULL;
	if(findDlg)
		findDlg = NULL;//Attribute Qt::WA_DeleteOnClose is activated!
	if(filterModel)
	{
		delete filterModel;
		filterModel = NULL;
	}
	if(mainLayout)
	{
		delete mainLayout;
		mainLayout = NULL;
	}
	if(expManager)
		expManager = NULL;
	if(tmpExpStruct)
	{
		delete tmpExpStruct;
		tmpExpStruct = NULL;
	}
	if(tmpExpObjectParamDefs)
		tmpExpObjectParamDefs = NULL;
	if(pCustomParamListTabWidget)
	{
		for(int i=0;i<pCustomParamListTabWidget->count();i++)
		{
			if(pCustomParamListTabWidget->widget(i))
			{
				pCustomParamListTabWidget->widget(i)->setParent(NULL);
				pCustomParamListTabWidget->removeTab(i);
			}
		}
		pCustomParamListTabWidget = NULL;
	}
	if(pCustomPropertiesDockWidget)
	{
		if(pCustomPropertiesDockWidget->widget())
		{
			pCustomPropertiesDockWidget->widget()->setParent(NULL);
			pCustomPropertiesDockWidget->setWidget(NULL);
		}
		delete pCustomPropertiesDockWidget;
		pCustomPropertiesDockWidget = NULL;
	}
	if(pCustomParamTableDockWidget)
	{
		if(pCustomParamTableDockWidget->widget())
		{
			pCustomParamTableDockWidget->widget()->setParent(NULL);
			pCustomParamTableDockWidget->setWidget(NULL);
		}
		delete pCustomParamTableDockWidget;
		pCustomParamTableDockWidget = NULL;
	}
	if(pCustomExperimentTreeDockWidget)
	{
		if (pCustomExperimentTreeDockWidget->widget())
		{
			pCustomExperimentTreeDockWidget->widget()->setParent(NULL);
			pCustomExperimentTreeDockWidget->setWidget(NULL);
		}
		delete pCustomExperimentTreeDockWidget;
		pCustomExperimentTreeDockWidget = NULL;
	}
	if(actionToggle_ListViewDockVisibility)
	{
		delete actionToggle_ListViewDockVisibility;
		actionToggle_ListViewDockVisibility = NULL;
	}
	if(actionToggle_TableViewDockVisibility)
	{
		delete actionToggle_TableViewDockVisibility;
		actionToggle_TableViewDockVisibility = NULL;
	}
}

void ExperimentGraphicEditor::configureCustomParameterWidgets()
{
	PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
	PropertySettingsWidget* tmpParametersWidget = NULL;
	
	//Object Declarations
	tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(DECLARATIONS_OBJECT_TAG);
	if(tmpParametersWidget == NULL)
	{
		qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << DECLARATIONS_OBJECT_TAG;
	}
	else
	{
		QList<int> lRegisteredMetaIds = MainAppInfo::getRegisteredMetaTypeIdList();
		QStringList lDefinedObjectClasses;
		foreach(int nMetaTypeID,lRegisteredMetaIds)
		{
			lDefinedObjectClasses << QMetaType::typeName(nMetaTypeID);
		}
		lDefinedObjectClasses.sort(Qt::CaseInsensitive);
		tmpParametersWidget->setAllowedParameterValues(CLASS_TAG,lDefinedObjectClasses);
		staticGraphicWidgetsHashTable.insert(DECLARATIONS_OBJECT_TAG,tmpParametersWidget);
	}

	//Object Connections, initializations
	QList<ExperimentStructuresNameSpace::strcExperimentObject> lExperimentObjects;
	QStringList lUsedObjectNames;
	QVariantList lTranslatedObjectIds;
	for(int nSectionSelector = 0;nSectionSelector<3;nSectionSelector++)
	{
		QString sCollectionName;
		if(nSectionSelector == 0)
			sCollectionName = CONNECTION_OBJECT_TAG;
		else if(nSectionSelector == 1)
			sCollectionName = INITIALIZATION_OBJECT_TAG;
		else if(nSectionSelector == 2)
			sCollectionName = FINALIZATION_OBJECT_TAG;

		tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sCollectionName);
		if(tmpParametersWidget == NULL)
		{
			qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sCollectionName;
		}
		else
		{
			if(nSectionSelector == 0)
			{
				if (pCurrentExpTreeModel)
				{
					lExperimentObjects = pCurrentExpTreeModel->getDefinedExperimentObjectInfoList(NULL);
					if (lExperimentObjects.isEmpty() == false)
					{
						int tmpCurrentObjectId;
						foreach(ExperimentStructuresNameSpace::strcExperimentObject tmpExperimentObject, lExperimentObjects)
						{
							tmpCurrentObjectId = tmpExperimentObject.nID;
							if (lTranslatedObjectIds.contains(tmpCurrentObjectId) == false)
							{
								lUsedObjectNames.append(tmpExperimentObject.sName);
								lTranslatedObjectIds.append(tmpCurrentObjectId);
							}
						}
					}
				}
			}
			if(lExperimentObjects.isEmpty() == false)
			{
				//lUsedObjectIds.sort(Qt::CaseInsensitive);
				if(nSectionSelector == 0)
				{
					tmpParametersWidget->setAllowedParameterValues(CONNECTION_SOURCE_TAG "/" OBJECT_TAG,lUsedObjectNames,lTranslatedObjectIds);
					tmpParametersWidget->setAllowedParameterValues(CONNECTION_TARGET_TAG "/" OBJECT_TAG,lUsedObjectNames,lTranslatedObjectIds);
					staticGraphicWidgetsHashTable.insert(sCollectionName,tmpParametersWidget);
				}
				else if(nSectionSelector == 1)
				{
					tmpParametersWidget->setAllowedParameterValues(OBJECT_TAG,lUsedObjectNames,lTranslatedObjectIds);
					tmpParametersWidget->setAllowedParameterValues(OBJECT_TAG,lUsedObjectNames,lTranslatedObjectIds);
					staticGraphicWidgetsHashTable.insert(sCollectionName,tmpParametersWidget);
				}
				else if(nSectionSelector == 2)
				{
					tmpParametersWidget->setAllowedParameterValues(OBJECT_TAG,lUsedObjectNames,lTranslatedObjectIds);
					tmpParametersWidget->setAllowedParameterValues(OBJECT_TAG,lUsedObjectNames,lTranslatedObjectIds);
					staticGraphicWidgetsHashTable.insert(sCollectionName,tmpParametersWidget);
				}
			}
		}
	}
}

void ExperimentGraphicEditor::createDockWindows()
{
	if(pCustomPropertiesDockWidget == NULL)
	{
		pCustomPropertiesDockWidget = new customDockWidget(tr(NAME_DOCKWIDGET_LIST_VIEW), Qt::DockWidgetArea::RightDockWidgetArea, this);
		pCustomPropertiesDockWidget->setAccessibleName(NAME_DOCKWIDGET_LIST_VIEW);
		pCustomPropertiesDockWidget->setObjectName(NAME_DOCKWIDGET_LIST_VIEW);
		pCustomPropertiesDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
		if(actionToggle_ListViewDockVisibility == NULL)
		{
			actionToggle_ListViewDockVisibility = new QAction(TOGGLE_DOCKWIDGET_LIST_VIEW_TEXT,this);
			connect(actionToggle_ListViewDockVisibility, SIGNAL(triggered()), pCustomPropertiesDockWidget, SLOT(toggleConfiguredVisibility()), Qt::ConnectionType(Qt::UniqueConnection));
			//menuView->addAction(actionToggle_ListViewDockVisibility);
		}
		bool bRetVal = false;
		QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), SLOT_REGISTERDOCKWIDGET_SIGNATURE, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(QWidget *, this), Q_ARG(MainWindowDockWidget*, pCustomPropertiesDockWidget));
		//bConnectRes = connect(pCustomPropertiesDockWidget, SIGNAL(CustomDockWidgetIsDestroying(QDockWidget *)), MainAppInfo::getMainWindow(), SLOT(unregisterDockWidget(QDockWidget *)), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
	}
	if(pCustomParamTableDockWidget == NULL)
	{
		pCustomParamTableDockWidget = new customDockWidget(tr(NAME_DOCKWIDGET_TABLE_VIEW), Qt::DockWidgetArea::BottomDockWidgetArea, this);
		pCustomParamTableDockWidget->setAccessibleName(NAME_DOCKWIDGET_TABLE_VIEW);
		pCustomParamTableDockWidget->setObjectName(NAME_DOCKWIDGET_TABLE_VIEW);
		pCustomParamTableDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
		if(actionToggle_TableViewDockVisibility == NULL)
		{
			actionToggle_TableViewDockVisibility = new QAction(TOGGLE_DOCKWIDGET_TABLE_VIEW_TEXT,this);
			connect(actionToggle_TableViewDockVisibility, SIGNAL(triggered()), pCustomParamTableDockWidget, SLOT(toggleConfiguredVisibility()), Qt::ConnectionType(Qt::UniqueConnection));
			//menuView->addAction(actionToggle_TableViewDockVisibility);
		}
		bool bRetVal = false;
		QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), SLOT_REGISTERDOCKWIDGET_SIGNATURE, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(QWidget *, this), Q_ARG(MainWindowDockWidget *, pCustomParamTableDockWidget));// +Qt::DockWidgetArea::LeftDockWidgetArea));
		//bConnectRes = connect(pCustomParamTableDockWidget, SIGNAL(CustomDockWidgetIsDestroying(QDockWidget *)), MainAppInfo::getMainWindow(), SLOT(unregisterDockWidget(QDockWidget *)), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
	}
	if (pCustomExperimentTreeDockWidget == NULL)
	{
		pCustomExperimentTreeDockWidget = new customDockWidget(tr(NAME_DOCKWIDGET_TREE_VIEW), Qt::DockWidgetArea::LeftDockWidgetArea, this);
		pCustomExperimentTreeDockWidget->setAccessibleName(NAME_DOCKWIDGET_TREE_VIEW);
		pCustomExperimentTreeDockWidget->setObjectName(NAME_DOCKWIDGET_TREE_VIEW);
		pCustomExperimentTreeDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
		if (actionToggle_TableViewDockVisibility == NULL)
		{
			actionToggle_TableViewDockVisibility = new QAction(TOGGLE_DOCKWIDGET_TREE_VIEW_TEXT, this);
			connect(actionToggle_TableViewDockVisibility, SIGNAL(triggered()), pCustomExperimentTreeDockWidget, SLOT(toggleConfiguredVisibility()), Qt::ConnectionType(Qt::UniqueConnection));
			//menuView->addAction(actionToggle_TableViewDockVisibility);
		}
		bool bRetVal = false;
		QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), SLOT_REGISTERDOCKWIDGET_SIGNATURE, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(QWidget *, this), Q_ARG(MainWindowDockWidget *, pCustomExperimentTreeDockWidget));//Qt::DockWidgetArea::BottomDockWidgetArea));
		//bConnectRes = connect(pCustomExperimentTreeDockWidget, SIGNAL(CustomDockWidgetIsDestroying(QDockWidget *)), MainAppInfo::getMainWindow(), SLOT(unregisterDockWidget(QDockWidget *)), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
		if (treeView)
		{
		//	treeView->setMinimumSize(treeView->width()/4, 100);
		//	treeView->setMaximumSize(treeView->width() * 2, treeView->height() * 2);
			pCustomExperimentTreeDockWidget->setWidget(treeView);
			pCustomExperimentTreeDockWidget->initialize();
		}
	}
	QList<QStringList> lTmpStringListList;
	lTmpStringListList.append(QStringList()<<"TAGS");
	bool bSelectResult = selectTreeItem(QStringList()<<BLOCKTRIALS_TAG, lTmpStringListList);
	toggleBlocksView();
	bSelectResult = selectTreeItem(QStringList()<<EXPERIMENT_TAG, lTmpStringListList);
}

void ExperimentGraphicEditor::setExperimentManager(ExperimentManager *pExpManager)
{
	expManager = pExpManager;
}

void ExperimentGraphicEditor::configureActions(bool bCreate)
{
	if(bCreate)
	{
		actionNew_File = new QAction("New Experiment File", this);
		action_Open_File = new QAction("Open Experiment File", this);
		//action_Open_File->setShortcut(QKeySequence::Open);
		action_Save = new QAction("Save Experiment File", this);
		action_Close_File = new QAction("Close Experiment File", this);
		actionFind = new QAction("Find", this);
		action_Quit = new QAction("Quit", this);
		actionAdd_Node = new QAction("Add Node", this);
		actionAdd_Subnode = new QAction("Add sub-Node", this);
		action_Remove_Node = new QAction("Remove Node", this);
		actionAdd_Attribute = new QAction("Add Node Attribute", this);
		actionToggleBlocksView = new QAction("Toggle Blocks View", this);
		//actionSwitchToDefaultView = new QAction("Switch to default View", this);
	}
	else
	{
		if(actionNew_File)
		{
			delete actionNew_File;
			actionNew_File = NULL;
		}
		if(action_Open_File)
		{
			delete action_Open_File;
			action_Open_File = NULL;
		}
		if(action_Save)
		{
			delete action_Save;
			action_Save = NULL;
		}
		if(action_Close_File)
		{
			delete action_Close_File;
			action_Close_File = NULL;
		}
		if(actionFind)
		{
			delete actionFind;
			actionFind = NULL;
		}
		if(action_Quit)
		{
			delete action_Quit;
			action_Quit = NULL;
		}
		if(actionAdd_Node)
		{
			delete actionAdd_Node;
			actionAdd_Node = NULL;
		}
		if(actionAdd_Subnode)
		{
			delete actionAdd_Subnode;
			actionAdd_Subnode = NULL;
		}
		if(action_Remove_Node)
		{
			delete action_Remove_Node;
			action_Remove_Node = NULL;
		}
		if(actionAdd_Attribute)
		{
			delete actionAdd_Attribute;
			actionAdd_Attribute = NULL;
		}
		if(actionToggleBlocksView)
		{
			delete actionToggleBlocksView;
			actionToggleBlocksView = NULL;
		}
		//if(actionSwitchToDefaultView)
		//{
		//	delete actionSwitchToDefaultView;
		//	actionSwitchToDefaultView = NULL;
		//}
	}
}

void ExperimentGraphicEditor::setupExperimentTreeView()
{
	treeView = new CustomChildDockTreeViewWidget();
	treeView->setGroupName(PLUGIN_EXMLDOC_EXTENSION);
	tblWidgetView = new QTableWidget();
	treeView->setHeaderHidden(true);

	tblWidgetView->setSelectionMode(QTableWidget::SingleSelection);
	tblWidgetView->setSelectionBehavior(QTableWidget::SelectRows);
	tblWidgetView->setEditTriggers(QTableWidget::NoEditTriggers);
	tblWidgetView->setVisible(false);
	tblWidgetView->setColumnHidden(4, true);
	tblWidgetView->horizontalHeader()->setHidden(true);
	tblWidgetView->verticalHeader()->setHidden(true);

	//connect(tblWidgetView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(selectTreeItem()));
	connect(treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(showTreeItemInfo(QModelIndex)), Qt::ConnectionType(Qt::UniqueConnection));
}

void ExperimentGraphicEditor::setupMenuAndActions()
{
	//toolBar = new QToolBar(this);
	//File menu///
	//buttonFile = new QToolButton(this);
	//buttonFile->setText("File ");
	//buttonFile->setPopupMode(QToolButton::InstantPopup);
	//menuFile = new QMenu();//FileButton
	//menuFile->addAction(actionNew_File);
	//menuFile->addAction(action_Open_File);
	//menuFile->addAction(action_Save);
	//menuFile->addAction(action_Close_File);
	//menuFile->addAction(action_Quit);
	//buttonFile->setMenu(menuFile);
	//toolBar->addWidget(buttonFile);
	//Edit menu///
	//buttonEdit=new QToolButton(this);
	//buttonEdit->setText("Edit ");
	//buttonEdit->setPopupMode(QToolButton::InstantPopup);
	//menuEdit = new QMenu();//EditButton
	//menuEdit->addAction(actionFind);
	//menuEdit->addAction(actionAdd_Node);
	//menuEdit->addAction(actionAdd_Subnode);
	//menuEdit->addAction(action_Remove_Node);
	//menuEdit->addAction(actionAdd_Attribute);
	//buttonEdit->setMenu(menuEdit);
	//toolBar->addWidget(buttonEdit);
	//View menu///
	//QComboBox *cmbViewSelection = new QComboBox(this);
	//cmbViewSelection->addItems(QStringList() << "Blocks" << "Objects");
	//QAction *tmpAction = toolBar->insertWidget(NULL, cmbViewSelection);
	//QLabel *labViewSelection = new QLabel("Show: ", this);
	//toolBar->insertWidget(tmpAction, labViewSelection);

	//buttonView = new QToolButton(this);
	//buttonView->setText("View ");
	//buttonView->setPopupMode(QToolButton::InstantPopup);
	//menuView = new QMenu();//ViewButton
	//menuView->addAction(actionSwitchToDefaultView);
	//menuView->addAction(actionToggleBlocksView);
	//buttonView->setMenu(menuView);
	//toolBar->addWidget(buttonView);
	
	//connect(actionNew_File, SIGNAL(triggered()), this, SLOT(newFile()));
	//connect(action_Open_File, SIGNAL(triggered()), this, SLOT(openFile()));
	//connect(action_Save, SIGNAL(triggered()), this, SLOT(saveFile()));
	//connect(action_Close_File, SIGNAL(triggered()), this, SLOT(closeFile()));
	//connect(action_Quit, SIGNAL(triggered()), this, SLOT(closeDocument()));
	//connect(actionFind, SIGNAL(triggered()), this, SLOT(showFindDialog()));
	//connect(actionAdd_Node, SIGNAL(triggered()), this, SLOT(insertNode()));
	//connect(actionAdd_Subnode, SIGNAL(triggered()), this, SLOT(insertSubnode()));
	//connect(action_Remove_Node, SIGNAL(triggered()), this, SLOT(removeNode()));
	//connect(actionAdd_Attribute, SIGNAL(triggered()), this, SLOT(addDefinition()));
	//connect(actionSwitchToDefaultView, SIGNAL(triggered()), this, SLOT(switchToDefaultView()));
	//connect(actionToggleBlocksView, SIGNAL(triggered()), this, SLOT(toggleBlocksView()));
}

void ExperimentGraphicEditor::setupLayout()
{
	mainLayout = new QVBoxLayout();
	if (toolBar)
		mainLayout->setMenuBar(toolBar);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);	
}

void ExperimentGraphicEditor::setupFilterModel()
{
	if(filterModel)
		delete filterModel;
	filterModel = new TreeFilterProxyModel();
	if(pCurrentExpTreeModel)
		filterModel->setSourceModel(pCurrentExpTreeModel);
	filterModel->setTreeFilterSettings(currentViewSettings);
	treeView->setModel(filterModel);
}

void ExperimentGraphicEditor::newFile()
{
	bool createNew = true;
	if (pCurrentExpTreeModel)
	{
		int confirm = QMessageBox::question(this, tr("Close current file"), tr("Are you sure to close the current file?"), QMessageBox::Ok | QMessageBox::Cancel);
		if (confirm == QMessageBox::Cancel)
			createNew = false;
	}
	if (createNew)
	{
		closeFile();
		action_Save->setEnabled(true);
		action_Close_File->setEnabled(true);
		actionAdd_Attribute->setEnabled(true);
		actionAdd_Node->setEnabled(true);
		actionAdd_Subnode->setEnabled(true);
		action_Remove_Node->setEnabled(true);
		actionFind->setEnabled(true);
		actionToggleBlocksView->setEnabled(true);
		//actionSwitchToDefaultView->setEnabled(true);
		pCurrentExpTreeModel = &loadedExpTreeModel; 
		connect(pCurrentExpTreeModel, SIGNAL(modelModified()), this, SLOT(setNewModel()), Qt::ConnectionType(Qt::UniqueConnection));
		//rootItem = new ExperimentTreeItem("[Root node]");
		//pExpTreeModel->setItem(0, rootItem);
		setupFilterModel();
	}
}

void ExperimentGraphicEditor::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Experiment Files (*.exml);;XML Files (*.xml)"));

	if (fileName != "")
	{
		QFile expFile(fileName);
		if (expFile.open(QFile::ReadOnly | QFile::Text)) 
		{
			closeFile();

			action_Save->setEnabled(true);
			action_Close_File->setEnabled(true);
			actionAdd_Attribute->setEnabled(true);
			actionAdd_Node->setEnabled(true);
			actionAdd_Subnode->setEnabled(true);
			action_Remove_Node->setEnabled(true);
			actionFind->setEnabled(true);	
			actionToggleBlocksView->setEnabled(true);
			//actionSwitchToDefaultView->setEnabled(true);
			
			QString tmpString = expFile.readAll();			
			expFile.close();

			loadedExpTreeModel.read(tmpString.toLatin1());
			pCurrentExpTreeModel = &loadedExpTreeModel;
			connect(pCurrentExpTreeModel, SIGNAL(modelModified()), this, SLOT(setNewModel()), Qt::ConnectionType(Qt::UniqueConnection));
			setupFilterModel();
		}
		else
		{
			qDebug() << __FUNCTION__ << "Could not open the file " << fileName;
		}
	}
}

void ExperimentGraphicEditor::closeDocument()
{
	QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowCloseActiveDocSlotName().toLatin1(), Qt::QueuedConnection);
}

void ExperimentGraphicEditor::closeFile()
{
	//Deleting model && cleaning QTreeView
	treeView->setModel(0);
	loadedExpTreeModel.reset();
	if (pCurrentExpTreeModel)
	{
		pCurrentExpTreeModel = NULL;
	}
	//Deleting scroll widget && cleaning QSrollArea
	if (dynamicGraphicWidget != NULL)
	{
		delete dynamicGraphicWidget;
		dynamicGraphicWidget = NULL;
	}
	//Hiding QTableWidget
	tblWidgetView->setHidden(true);
	action_Save->setDisabled(true);
	action_Close_File->setDisabled(true);
	actionAdd_Attribute->setDisabled(true);
	actionAdd_Node->setDisabled(true);
	actionAdd_Subnode->setDisabled(true);
	action_Remove_Node->setDisabled(true);
	actionFind->setDisabled(true);
	actionToggleBlocksView->setDisabled(true);
	//actionSwitchToDefaultView->setDisabled(true);
}

QString ExperimentGraphicEditor::saveFile(const QString &sFilePath)
{
	QString fileName;
	if ((sFilePath.isEmpty()) || (QFile(sFilePath).exists()==false))
		fileName = QFileDialog::getSaveFileName(this, tr("Save File"), sFilePath, tr("Experiment Files (*.exml);;XML Files (*.xml)"));
	else
		fileName = sFilePath;
	if (fileName.isEmpty() == false)
	{
		QFileInfo saveFileInfo(fileName);
		fileName = saveFileInfo.absolutePath() + "/" + saveFileInfo.fileName();
		pCurrentExpTreeModel->write(fileName);
		setNewFileName(fileName);
		emit ContentHasChanged(fileName,false);
		return fileName;
	}
	return "";
}

void ExperimentGraphicEditor::setNewModel()
{
	treeView->reset();
	if (pCurrentExpTreeModel)
	{
		treeView->activateWindow();
		treeView->setFocus(Qt::OtherFocusReason);
		if (filterModel == NULL)
			filterModel = new TreeFilterProxyModel();
		filterModel->setSourceModel(pCurrentExpTreeModel);
		filterModel->setTreeFilterSettings(currentViewSettings);
		treeView->setModel(filterModel);
		if (selectedIndex.isValid())
			treeView->selectionModel()->setCurrentIndex(selectedIndex, QItemSelectionModel::ClearAndSelect);
		else
			treeView->clearSelection();
		treeView->expandAll();
	}
	configureCustomParameterWidgets();
}

void ExperimentGraphicEditor::changeSelection(QList<ExperimentVisualizerGraphItemTypeEnum> lExpVisGraphItemTypes, QList<int> lItemIds)
{
	QStringList lTmpList;
	QList<QStringList> lTmpStringListList;
	foreach(ExperimentVisualizerGraphItemTypeEnum eTmpEnum, lExpVisGraphItemTypes)
	{
		if(eTmpEnum == EXPVIS_TYPE_BLOCK)
		{
			lTmpList.append(BLOCK_TAG);
			lTmpStringListList.append(QStringList()<<"TAGS");
		}
		else if(eTmpEnum == EXPVIS_TYPE_LOOP)
		{
			lTmpList.append(LOOP_TAG);
			lTmpStringListList.append(QStringList()<<"TAGS");
		}
		else if(eTmpEnum == EXPVIS_TYPE_OBJECT)
		{
			lTmpList.append(OBJECT_TAG);
			lTmpStringListList.append(QStringList()<<"TAGS");
		}
		else if(eTmpEnum == EXPVIS_TYPE_METHOD_CONNECTION)
		{
			lTmpList.append(CONNECTION_TAG);
			lTmpStringListList.append(QStringList()<<"TAGS");
		}
		else if (eTmpEnum == EXPVIS_TYPE_INIT)
		{
			lTmpList.append(INITIALIZATION_TAG);
			lTmpStringListList.append(QStringList() << "TAGS");
		}
		else if (eTmpEnum == EXPVIS_TYPE_FINIT)
		{
			lTmpList.append(FINALIZATION_TAG);
			lTmpStringListList.append(QStringList() << "TAGS");
		}
		else if (eTmpEnum == EXPVIS_TYPE_PARAMETERS)
		{
			lTmpList.append(PARAMETERS_TAG);
			lTmpStringListList.append(QStringList() << "TAGS");
		}
		else
		{
			lTmpList.append(EXPERIMENT_TAG);
			lTmpStringListList.append(QStringList()<<"TAGS");
		}
	}
	//Now select it
	if (selectTreeItem(lTmpList, lTmpStringListList, lItemIds) == false)
	{
		//Something went wrong....
		//Maybe we need to add some optional sections first, like an empty Parameters section...
		if (pCurrentExpTreeModel)
		{
			if ((lTmpList.count() == 3) && (lItemIds.count() == 2))
			{
				if ((lTmpList[0] == BLOCK_TAG) && (lTmpList[1] == OBJECT_TAG) && (lTmpList[2] == PARAMETERS_TAG))
				{
					if(pCurrentExpTreeModel->saveNewData(lItemIds[0], lItemIds[1], "", "", false))
						if (selectTreeItem(lTmpList, lTmpStringListList, lItemIds))
							return;
				}
			}
		}
		lTmpList.clear();
		lTmpList.append(EXPERIMENT_TAG);
		lTmpStringListList.clear();
		lTmpStringListList.append(QStringList() << "TAGS");
		lItemIds.clear();
		lItemIds.append(-1);
		selectTreeItem(lTmpList, lTmpStringListList, lItemIds);
	}
}

void ExperimentGraphicEditor::clearSelection()
{
	QStringList lTmpList;
	QList<QStringList> lTmpStringListList;
	lTmpList.append(EXPERIMENT_TAG);
	lTmpStringListList.append(QStringList() << "TAGS");
	selectTreeItem(lTmpList, lTmpStringListList, QList<int>());
}

void ExperimentGraphicEditor::showTreeItemInfo(const QModelIndex &index)
{
	if(treeView->model() == NULL)
		return;
	selectedIndex = index;	
	PropertySettingsWidget *tmpParametersWidget = NULL;
	QVariant tmpVarValue;
	QList<int> nIDList;
	QModelIndex originalIndex = dynamic_cast<TreeFilterProxyModel*>(treeView->model())->mapToSource(index);
	ExperimentTreeItem *item = pCurrentExpTreeModel->itemFromIndex(originalIndex);

	if (dynamicGraphicWidget)
	{
		delete dynamicGraphicWidget;
		dynamicGraphicWidget = NULL;
	}
	if(tmpExpObjectParamDefs)
		tmpExpObjectParamDefs = NULL;

	if (item->parent() == NULL)
		return;

	QString sSelectedItemLowerName = item->getName().toLower();
	QString sSelectedParentItemLowerName = item->parent()->getName().toLower();

	if (sSelectedParentItemLowerName == DEFINES_TAG)
	{
		if (sSelectedItemLowerName == EXPERIMENT_TAG)
		{
			QString sParamCollName = EXPERIMENT_TAG;
			if(staticGraphicWidgetsHashTable.contains(sParamCollName))
			{
				tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
				tmpParametersWidget->resetParameterModifiedFlagsAndValues();
			}
			else
			{
				PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
				tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
				if(tmpParametersWidget == NULL)
					qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
				else
					staticGraphicWidgetsHashTable.insert(sParamCollName,tmpParametersWidget);
			}
			//Set the values
			int nChildCount = item->childCount();
			QString sTmpString = "";
			QString sValue = "";
			for (int j=0;j<nChildCount;j++)
			{							
				if(item->child(j)->getName().toLower() == NAME_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(NAME_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == DEBUGMODE_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(DEBUGMODE_TAG,sTmpString,true,true);
				}
			}
		}
	}
	else if (sSelectedParentItemLowerName == DECLARATIONS_TAG)
	{
		if (sSelectedItemLowerName == OBJECT_TAG)
		{
			QString sParamCollName = DECLARATIONS_OBJECT_TAG;//OBJECT_TAG;
			QStringList lDefinedObjectClasses;
			if(staticGraphicWidgetsHashTable.contains(sParamCollName))
			{
				tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
				tmpParametersWidget->resetParameterModifiedFlagsAndValues();
			}
			else
			{
				PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
				tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
				if(tmpParametersWidget == NULL)
				{
					qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
				}
				else
				{
					staticGraphicWidgetsHashTable.insert(sParamCollName,tmpParametersWidget);
				}
			}

			//Set the values
			int nChildCount = item->childCount();
			QString sTmpString = "";
			QString sValue = "";
			for (int j=0;j<nChildCount;j++)
			{							
				if(item->child(j)->getName().toLower() == NAME_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(NAME_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == CLASS_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(CLASS_TAG,sTmpString,true,true);
				}
			}
		}
	}
	if (sSelectedItemLowerName == DEFINITION_TAG)
	{
		QString sParamCollName = OBJECT_DEFINITION_TAG;
		if(staticGraphicWidgetsHashTable.contains(sParamCollName))
		{
			tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
			tmpParametersWidget->resetParameterModifiedFlagsAndValues();
		}
		else
		{
			PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
			tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
			if(tmpParametersWidget == NULL)
				qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
			else
				staticGraphicWidgetsHashTable.insert(sParamCollName,tmpParametersWidget);
		}
		//Set the values
		int nChildCount = item->childCount();
		QString sTmpString = "";
		QString sValue = "";
		for (int j=0;j<nChildCount;j++)
		{							
			if(item->child(j)->getName().toLower() == FILE_TAG)
			{
				sTmpString = item->child(j)->getValue();
				if(sTmpString.isEmpty() == false)
					tmpParametersWidget->setParameter(FILE_TAG,sTmpString,true,true);
			}
		}
	}
	else if (sSelectedItemLowerName == CONNECTION_TAG)
	{
		if (item->childCount() == 2)//For Source and Target section, 3 needed for now...
		{
			QString sParamCollName = CONNECTION_OBJECT_TAG;
			if(staticGraphicWidgetsHashTable.contains(sParamCollName))
			{
				tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
				tmpParametersWidget->resetParameterModifiedFlagsAndValues();
			}
			else
			{
				PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
				tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
				if(tmpParametersWidget == NULL)
				{
					qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
				}
				else
				{
					staticGraphicWidgetsHashTable.insert(sParamCollName,tmpParametersWidget);
				}								
			}
			//Set the values
			QString sTmpString = "";
			QString sValue = "";
			for(int nSourceTargetSwitch=0;nSourceTargetSwitch<2;nSourceTargetSwitch++)
			{
				ExperimentTreeItem *tmpExpTreeItem = NULL;
				if(nSourceTargetSwitch == 0)//Source section...
					tmpExpTreeItem = item->firstChild(CONNECTION_SOURCE_TAG);
				else if(nSourceTargetSwitch == 1)//Target section...
					tmpExpTreeItem = item->firstChild(CONNECTION_TARGET_TAG);
				if(tmpExpTreeItem)
				{
					ExperimentTreeItem *tmpExpSubTreeItem = tmpExpTreeItem->firstChild(OBJECT_TAG);
					if(tmpExpSubTreeItem)
					{
						QString sObjectID = tmpExpSubTreeItem->getValue();
						if(sObjectID.isEmpty() == false)
						{
							ExperimentTreeItem *tmpExpSubTreeItem = tmpExpTreeItem->firstChild(CONNECTION_SIGNATURE_TAG);
							if(tmpExpSubTreeItem)
							{
								QString sSignature = tmpExpSubTreeItem->getValue();
								ExperimentTreeItem *tmpExpSubTreeItem = tmpExpTreeItem->firstChild(CONNECTION_TYPE_TAG);
								if(sSignature.isEmpty() == false)
								{
									if(tmpExpSubTreeItem)
									{
										QString sType = tmpExpSubTreeItem->getValue();
										if(sType.isEmpty() == false)
										{
											if(nSourceTargetSwitch == 0)//Source section...
											{
												tmpParametersWidget->setParameter(CONNECTION_SOURCE_TAG "/" OBJECT_TAG,sObjectID,true,true);
												tmpParametersWidget->setParameter(CONNECTION_SOURCE_TAG "/" CONNECTION_SIGNATURE_TAG,sSignature,true,true);
												tmpParametersWidget->setParameter(CONNECTION_SOURCE_TAG "/" CONNECTION_TYPE_TAG,sType,true,true);
											}
											else if(nSourceTargetSwitch == 1)//Target section...
											{
												tmpParametersWidget->setParameter(CONNECTION_TARGET_TAG "/" OBJECT_TAG,sObjectID,true,true);
												tmpParametersWidget->setParameter(CONNECTION_TARGET_TAG "/" CONNECTION_SIGNATURE_TAG,sSignature,true,true);
												tmpParametersWidget->setParameter(CONNECTION_TARGET_TAG "/" CONNECTION_TYPE_TAG,sType,true,true);
											}
										}
									}
								}
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	else if ((sSelectedItemLowerName == INITIALIZATION_TAG) || (sSelectedItemLowerName == FINALIZATION_TAG))
	{
		int nInitFinitOrderNumber = -1;
		ExperimentTreeItem *pTmpExpTreeItem = item->firstChild(INIT_FINIT_NUMBER_TAG);
		if (pTmpExpTreeItem)
		{
			bool bConversionResult = false;
			nInitFinitOrderNumber = pTmpExpTreeItem->getValue().toInt(&bConversionResult);
			if (bConversionResult == false)
				nInitFinitOrderNumber = -1;
		}
		if (nInitFinitOrderNumber>=0)
		{
			pTmpExpTreeItem = item->firstChild(OBJECT_TAG);
			if (pTmpExpTreeItem)
			{
				QString sParamCollName;
				if (sSelectedItemLowerName == INITIALIZATION_TAG)
					sParamCollName = INITIALIZATION_OBJECT_TAG;
				else if (sSelectedItemLowerName == FINALIZATION_TAG)
					sParamCollName = FINALIZATION_OBJECT_TAG;
				if (staticGraphicWidgetsHashTable.contains(sParamCollName))
				{
					tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
					tmpParametersWidget->resetParameterModifiedFlagsAndValues();
				}
				else
				{
					PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
					tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
					if (tmpParametersWidget == NULL)
					{
						qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
					}
					else
					{
						staticGraphicWidgetsHashTable.insert(sParamCollName, tmpParametersWidget);
					}
				}
				//Set the values
				QMap<QString, TreeItemDefinition> mItemDefs = pTmpExpTreeItem->getDefinitions();
				int nObjectId = -1;
				if (mItemDefs.contains(ID_TAG))
					nObjectId = mItemDefs[ID_TAG].value.toInt();
				if (nObjectId >= 0)
				{
					tmpParametersWidget->setParameter(INIT_FINIT_NUMBER_TAG, QString::number(nInitFinitOrderNumber), true, true);
					tmpParametersWidget->setParameter(OBJECT_TAG, QString::number(nObjectId), true, true);
					QString sTmpString = "";
					ExperimentTreeItem *pTmpExpTreeSubItem = pTmpExpTreeItem->firstChild(INIT_FINIT_SIGNATURE_TAG);
					if (pTmpExpTreeSubItem)
					{
						sTmpString = pTmpExpTreeSubItem->getValue();
						if (sTmpString.isEmpty() == false)
						{
							tmpParametersWidget->setParameter(INIT_FINIT_SIGNATURE_TAG, sTmpString, true, true);
							pTmpExpTreeSubItem = pTmpExpTreeItem->firstChild(INIT_FINIT_TYPE_TAG);
							if (pTmpExpTreeSubItem)
							{
								sTmpString = pTmpExpTreeSubItem->getValue();
								if (sTmpString.isEmpty() == false)
								{
									tmpParametersWidget->setParameter(INIT_FINIT_TYPE_TAG, sTmpString, true, true);

									//Remove already defined arguments 
									int nAddParamCntr = 3;
									int nParamIndexToRemove = nAddParamCntr;
									while (tmpParametersWidget->removeParameterProperty(QString::number(nParamIndexToRemove)))
										nParamIndexToRemove++;

									pTmpExpTreeSubItem = pTmpExpTreeItem->firstChild(PARAMETERS_TAG);
									if (pTmpExpTreeSubItem)
									{
										if (pTmpExpTreeSubItem->hasChildren())
										{
											for (int nChildCount = 0; nChildCount < pTmpExpTreeSubItem->childCount(); nChildCount++)
											{
												if (pTmpExpTreeSubItem->child(nChildCount)->getName().toLower() == PARAMETER_TAG)
												{
													if (pTmpExpTreeSubItem->child(nChildCount)->hasChildren())
													{
														QString sParamName = "";
														QString sParamValue = "";
														QString sParamType = "";
														ExperimentTreeItem *pTmpExpTreeSubParamItem = pTmpExpTreeSubItem->child(nChildCount)->firstChild(NAME_TAG);
														if (pTmpExpTreeSubParamItem)
														{
															sParamName = pTmpExpTreeSubParamItem->getValue();
															if (sParamName.isEmpty() == false)
															{
																pTmpExpTreeSubParamItem = pTmpExpTreeSubItem->child(nChildCount)->firstChild(VALUE_TAG);
																if (pTmpExpTreeSubParamItem)
																{
																	sParamValue = pTmpExpTreeSubParamItem->getValue();
																	if (sParamValue.isEmpty() == false)
																	{
																		pTmpExpTreeSubParamItem = pTmpExpTreeSubItem->child(nChildCount)->firstChild(MEMBER_TYPE_TAG);
																		if (pTmpExpTreeSubParamItem)
																		{
																			sParamType = pTmpExpTreeSubParamItem->getValue();
																			if (sParamType.isEmpty() == false)
																			{
																				//Add newly defined arguments
																				PropertySettingDefinitionStrc *tmpExpParamDefStrc = new PropertySettingDefinitionStrc();
																				tmpExpParamDefStrc->bCanBeScriptReference = false;
																				tmpExpParamDefStrc->bEnabled = true;
																				tmpExpParamDefStrc->bIsEditable = false;
																				tmpExpParamDefStrc->eType = PropertySetting_Type_Integer;
																				tmpExpParamDefStrc->nId = nAddParamCntr;
																				tmpExpParamDefStrc->sDefaultValue = "0";
																				tmpExpParamDefStrc->sName = sParamName;
																				tmpExpParamDefStrc->sDisplayName = sParamName + "(" + sParamType + ")";
																				tmpExpParamDefStrc->sGroupPath = "Arguments";//;Argument " + QString::number(nAddParamCntr-2);
																				tmpExpParamDefStrc->sInformation = "an Argument of Type " + sParamType;
																				tmpParametersWidget->addParameterProperty(tmpExpParamDefStrc, sParamValue);
																				nAddParamCntr++;
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
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else if (sSelectedItemLowerName == BLOCKTRIALS_TAG)
	{			
		if(item->hasChildren() || true)
		{						
			if(pCurrentExpTreeModel)
			{
				if(tmpExpStruct)
					delete tmpExpStruct;
				tmpExpStruct = new cExperimentStructure();
				QList<ExperimentTreeItem*> tmpExpTreeItemList;

				if(bShowGraphicalTreeView)
				{ 
					if (expStructVisualizer == NULL)
					{
						expStructVisualizer = new ExperimentStructureVisualizer();
						nAdditionalMenuHeight = expStructVisualizer->getAdditionalMenuHeight();
					}
					if(expStructVisualizer)
					{
						bool bResult = connect(expStructVisualizer, SIGNAL(destroyed(QWidget*)), this, SLOT(childWidgetDestroyed(QWidget*)), Qt::ConnectionType(Qt::UniqueConnection));
						bResult = expManager->createExperimentStructure(tmpExpTreeItemList, pCurrentExpTreeModel,tmpExpStruct);
						if(bResult)
						{
							bResult = expStructVisualizer->setExperimentTreeModel(pCurrentExpTreeModel);
							if(bResult)
							{
								mainLayout->addWidget(expStructVisualizer);
								expStructVisualizer->showMaximized();
								if (toolBar)
									expStructVisualizer->setVisualMargin(0, toolBar->height() * 2);
								bResult = connect(expStructVisualizer, SIGNAL(GraphItemSelectionChanged(QList<ExperimentVisualizerGraphItemTypeEnum>, QList<int>)), this, SLOT(changeSelection(QList<ExperimentVisualizerGraphItemTypeEnum>, QList<int>)), Qt::UniqueConnection);
								bResult = connect(this, SIGNAL(OnResized(const int &, const int &)), expStructVisualizer, SLOT(resizeStructureView(const int &, const int &)), Qt::ConnectionType(Qt::UniqueConnection));
								bResult = connect(expStructVisualizer, SIGNAL(GraphRedrawn()), this, SLOT(forceEmitResize()), Qt::ConnectionType(Qt::UniqueConnection));
								return;
							}
						}
					}							
				}		
				else
				{
					if(expBlockParamView == NULL)
					{
						expBlockParamView = new ExperimentBlockParameterView(NULL, this, pCurrentExpTreeModel);
					}
					if(expBlockParamView)
					{
						bool bConnectResult = connect(expBlockParamView, SIGNAL(destroyed(QWidget*)), this, SLOT(childWidgetDestroyed(QWidget*)), Qt::ConnectionType(Qt::UniqueConnection));
						bConnectResult = connect(expBlockParamView, SIGNAL(onFocusTable()), this, SLOT(clearSelection()), Qt::ConnectionType(Qt::UniqueConnection));
						if (pCustomParamTableDockWidget)
						{
							pCustomParamTableDockWidget->setWidget(expBlockParamView);
							pCustomParamTableDockWidget->initialize();
						}
						return;							
					}
				}
			}
		}				
	}
	else if (sSelectedItemLowerName == BLOCK_TAG)
	{
		if (sSelectedParentItemLowerName == BLOCKTRIALS_TAG)
		{
			QString sParamCollName = BLOCK_TAG;
			if(staticGraphicWidgetsHashTable.contains(sParamCollName))
			{
				tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
				tmpParametersWidget->resetParameterModifiedFlagsAndValues();
			}
			else
			{
				PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
				tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
				if(tmpParametersWidget == NULL)
				{
					qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
				}
				else
				{
					staticGraphicWidgetsHashTable.insert(sParamCollName,tmpParametersWidget);
				}								
			}
			//Set the values
			int nChildCount = item->childCount();
			QString sTmpString = "";
			QString sValue = "";
			for (int j=0;j<nChildCount;j++)
			{							
				if(item->child(j)->getName().toLower() == NAME_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(NAME_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == BLOCKNUMBER_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(BLOCKNUMBER_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == TRIALAMOUNT_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(TRIALAMOUNT_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == INTERNALTRIGGERAMOUNT_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(INTERNALTRIGGERAMOUNT_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == EXTERNALTRIGGERAMOUNT_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(EXTERNALTRIGGERAMOUNT_TAG,sTmpString,true,true);
				}
			}
		}
	}
	else if (sSelectedItemLowerName == PARAMETERS_TAG)
	{	
		ExperimentTreeItem *child = NULL;
		int nParameterRepeats = 1;
		if (item->hasChildren())
			nParameterRepeats = item->rowCount();
		for(int i=0;i<nParameterRepeats;i++)
		{
			if (item->hasChildren())
				child = item->child(i);
			if(pCurrentExpTreeModel)
			{
				if((item->parent() != NULL) && (item->parent()->parent() != NULL))
				{
					if(i==0)
					{
						if (sSelectedParentItemLowerName == OBJECT_TAG)
						{
							QList<ExperimentStructuresNameSpace::strcExperimentObject> tmpExperimentObjectList;
							tmpExperimentObjectList = pCurrentExpTreeModel->getDefinedExperimentObjectInfoList(item->parent());
							if(tmpExperimentObjectList.isEmpty() == false)
							{
								if(tmpExperimentObjectList.at(0).nID >= 0)
								{
									QString sParamCollName = "";
									QString sCloneParameterCollectionName = "";
									int nCurrentBlockId = -1;
									if(item->parent()->parent()->getName().toLower() == BLOCK_TAG)
									{
										sParamCollName = tmpExperimentObjectList.at(0).sClass;//Case sensitive!!;
										QMap<QString, TreeItemDefinition> mBlockItemDefs = item->parent()->parent()->getDefinitions();
										if (mBlockItemDefs.contains(ID_TAG))
											nCurrentBlockId = mBlockItemDefs[ID_TAG].value.toInt();
										if (nCurrentBlockId >= 0)
											sCloneParameterCollectionName = sParamCollName + "_" + BLOCK_TAG + QString::number(nCurrentBlockId);
									}
									else if(item->parent()->parent()->getName().toLower() == INITIALIZATION_TAG)
									{
										sParamCollName = ""; 
									}
									if (sCloneParameterCollectionName.isEmpty() == false)
									{
										if (staticGraphicWidgetsHashTable.contains(sCloneParameterCollectionName))
										{
											tmpParametersWidget = staticGraphicWidgetsHashTable.value(sCloneParameterCollectionName);
											tmpParametersWidget->resetParameterModifiedFlagsAndValues();
										}
										else
										{
											PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
											tmpParametersWidget = expParamWidgets->cloneExperimentParameterWidget(sParamCollName, sCloneParameterCollectionName);
											if(tmpParametersWidget == NULL)
												qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sCloneParameterCollectionName;
											else
												staticGraphicWidgetsHashTable.insert(sCloneParameterCollectionName, tmpParametersWidget);
										}
									}
								}
							}
						}
					}
					if(tmpParametersWidget)
					{
						if(child)
						{
							if((child->getName().toLower() == PARAMETER_TAG))
							{
								if(child->hasChildren())
								{
									//Set the values
									int nChildCount = child->childCount();
									QString sName = "";
									QString sValue = "";
									
									//tmpParametersWidget->resetParameterProperties();
									for (int j=0;j<nChildCount;j++)
									{							
										if(item->child(i)->child(j)->getName().toLower() == NAME_TAG)
										{
											sName = item->child(i)->child(j)->getValue();
										}
										else if(item->child(i)->child(j)->getName().toLower() == VALUE_TAG)
										{
											sValue = item->child(i)->child(j)->getValue();
										}
										if((sName.isEmpty() || sValue.isEmpty()) == false)
										{
											//bool bResult = 
											tmpParametersWidget->setParameter(sName,sValue,true,true);
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
	else if (sSelectedItemLowerName == CUSTOM_PARAMETERS_TAG)
	{	
		ExperimentTreeItem *child = NULL;
		int nParameterRepeats = 1;
		int nCustomParamsObjectID = -1;
		bool bParamCollectionPresent = false;
		QString sParamCollName = "";
		QString sCloneParameterCollectionName = "";
		int nCurrentBlockId = -1;
		QList<ExperimentStructuresNameSpace::strcExperimentObject> tmpExperimentObjectList;
		if (item->hasChildren())
			nParameterRepeats = item->rowCount();
		for(int i=0;i<nParameterRepeats;i++)
		{
			if (item->hasChildren())
				child = item->child(i);
			if(pCurrentExpTreeModel)
			{
				if((item->parent() != NULL) && (item->parent()->parent() != NULL))
				{
					if(i==0)
					{
						if (sSelectedParentItemLowerName == OBJECT_TAG)
						{
							tmpExperimentObjectList = pCurrentExpTreeModel->getDefinedExperimentObjectInfoList(item->parent());
							if(tmpExperimentObjectList.isEmpty() == false)
							{
								if(tmpExperimentObjectList.at(0).nID >= 0)
								{
									if(item->parent()->parent()->getName().toLower() == BLOCK_TAG)
									{
										sParamCollName = tmpExperimentObjectList.at(0).sClass;//Case sensitive!!;
										nCustomParamsObjectID = tmpExperimentObjectList.at(0).nID;
										QMap<QString, TreeItemDefinition> mBlockItemDefs = item->parent()->parent()->getDefinitions();
										if (mBlockItemDefs.contains(ID_TAG))
											nCurrentBlockId = mBlockItemDefs[ID_TAG].value.toInt();
										if (nCurrentBlockId >= 0)
											sCloneParameterCollectionName = sParamCollName + "_" + BLOCK_TAG + QString::number(nCurrentBlockId);
									}
									else if(item->parent()->parent()->getName().toLower() == INITIALIZATION_TAG)
									{
										sParamCollName = ""; 
									}
								}
							}
						}
					}
					if (sCloneParameterCollectionName.isEmpty() == false)
					{
						PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
						if(bParamCollectionPresent == false)
						{
							if (staticGraphicWidgetsHashTable.contains(sCloneParameterCollectionName + CUSTOMOBJECTPARAMS_POSTSTRING))
							{
								tmpParametersWidget = staticGraphicWidgetsHashTable.value(sCloneParameterCollectionName + CUSTOMOBJECTPARAMS_POSTSTRING);
								tmpParametersWidget->resetParameterModifiedFlagsAndValues();
								bParamCollectionPresent = true;
							}
							else
							{
								if(nCustomParamsObjectID>=0)
								{
									//ExperimentStructuresNameSpace::strcObjectDefinitions tmpObjectDefs;
									//tmpObjectDefs = pExpTreeModel->getExperimentObjectDefinitions(nCustomParamsObjectID);
									bool bLoadResult = false;
									if(tmpExperimentObjectList.isEmpty() == false)//(tmpObjectDefs.lObjectDefinitions.isEmpty() == false)
									{
										for(int nObjectDefCntr=0;nObjectDefCntr<tmpExperimentObjectList.at(0).lObjectDefinitions.count();nObjectDefCntr++)
										{
											QString sCurrentParamDefDirectory = "";
											if (getCurrentFileName().isEmpty() == false)
												sCurrentParamDefDirectory = QFileInfo(getCurrentFileName()).absoluteDir().canonicalPath();
											if (expParamWidgets->loadExperimentParameterDefinition(tmpExperimentObjectList.at(0).lObjectDefinitions.at(nObjectDefCntr).sDefinitionFile, sCloneParameterCollectionName + CUSTOMOBJECTPARAMS_POSTSTRING, true, sCurrentParamDefDirectory))
											{
												bLoadResult = true;
												break;
											}
										}
									}
									if(bLoadResult)
									{
										tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sCloneParameterCollectionName + CUSTOMOBJECTPARAMS_POSTSTRING);
										if(tmpParametersWidget == NULL)
										{
											qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sCloneParameterCollectionName + CUSTOMOBJECTPARAMS_POSTSTRING;
										}
										else
										{
											staticGraphicWidgetsHashTable.insert(sCloneParameterCollectionName + CUSTOMOBJECTPARAMS_POSTSTRING, tmpParametersWidget);
											bParamCollectionPresent = true;
										}								
									}
									if(bParamCollectionPresent == false)
									{
										bParamCollectionPresent = expParamWidgets->createCustomExperimentParameterWidgetCollection(sParamCollName + CUSTOMOBJECTPARAMS_POSTSTRING, true);
									}
								}
							}
						}
						if(child && bParamCollectionPresent)
						{
							QList<PropertySettingDefinitionStrc> *lExpParamsDefStrcs = new QList<PropertySettingDefinitionStrc>;
							PropertySettingDefinitionStrc tmpExperimentParameterDefinitionStrc;
							if((child->getName().toLower() == PARAMETER_TAG))
							{
								if(child->hasChildren())
								{
									//Set the values
									int nChildCount = child->childCount();
									QString sName = "";
									QString sValue = "";
									for (int j=0;j<nChildCount;j++)
									{							
										if(item->child(i)->child(j)->getName().toLower() == NAME_TAG)
										{
											sName = item->child(i)->child(j)->getValue();
										}
										else if(item->child(i)->child(j)->getName().toLower() == VALUE_TAG)
										{
											sValue = item->child(i)->child(j)->getValue();
										}
										if((sName.isEmpty() || sValue.isEmpty()) == false)
										{
											if(tmpParametersWidget)
											{
												if(tmpParametersWidget->checkIfParameterExists(sName)==false)
												{
													tmpExperimentParameterDefinitionStrc.bEnabled = true;
													tmpExperimentParameterDefinitionStrc.eType = PropertySetting_Type_String;
													tmpExperimentParameterDefinitionStrc.nId = i;
													tmpExperimentParameterDefinitionStrc.sName = sName;
													tmpExperimentParameterDefinitionStrc.sDisplayName = sName;
													tmpExperimentParameterDefinitionStrc.bCanBeScriptReference = true;
													tmpExperimentParameterDefinitionStrc.sInformation = EXPERIMENT_CUSTOMPARAM_INFOSTRING;
													
													lExpParamsDefStrcs->append(tmpExperimentParameterDefinitionStrc);

													tmpParametersWidget = expParamWidgets->insertCustomExperimentParameterWidget(sParamCollName + CUSTOMOBJECTPARAMS_POSTSTRING,lExpParamsDefStrcs);
													if(tmpParametersWidget)
													{
														tmpParametersWidget->setParameter(sName,sValue,true,true);
													}
												}
												else
												{
													tmpParametersWidget->setParameter(sName,sValue,true,true);
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
		}
	}
	else if (sSelectedItemLowerName == PARAMETER_TAG)
	{
		if((item->parent()) && (item->parent()->parent()) && (item->parent()->parent()->parent())) 
		{
			QString sParamCollName = "";
			if (item->parent()->parent()->parent()->getName() == INITIALIZATION_TAG)
			{
				sParamCollName = INITIALIZATIONS_PARAMETER_TAG;
			}
			else if (item->parent()->parent()->parent()->getName() == FINALIZATION_TAG)
			{
				sParamCollName = FINALIZATIONS_PARAMETER_TAG;
			}
			else if(item->parent()->parent()->parent()->getName() == BLOCK_TAG)
			{
				sParamCollName = BLOCK_PARAMETER_TAG;
			}
			if(sParamCollName.isEmpty() == false)
			{
				if(staticGraphicWidgetsHashTable.contains(sParamCollName))
				{
					tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
					tmpParametersWidget->resetParameterModifiedFlagsAndValues();
				}
				else
				{
					PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
					tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
					if(tmpParametersWidget == NULL)
					{
						qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
					}
					else
					{
						staticGraphicWidgetsHashTable.insert(sParamCollName,tmpParametersWidget);
					}								
				}
			}
			if(tmpParametersWidget)
			{
				if(item->hasChildren())
				{
					ExperimentTreeItem *child = NULL;
					for (int i = 0; i < item->rowCount(); i++)
					{
						child = item->child(i);
						tmpParametersWidget->setParameter(child->getName().toLower(),child->getValue(),true,true);
					}
				}
			}
		}
	}
	else if (sSelectedParentItemLowerName == LOOPS_TAG)
	{
		if (sSelectedItemLowerName == LOOP_TAG)
		{
			QString sParamCollName = LOOP_TAG;
			if(staticGraphicWidgetsHashTable.contains(sParamCollName))
			{
				tmpParametersWidget = staticGraphicWidgetsHashTable.value(sParamCollName);
				tmpParametersWidget->resetParameterModifiedFlagsAndValues();
			}
			else
			{
				PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
				tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(sParamCollName);
				if(tmpParametersWidget == NULL)
				{
					qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << sParamCollName;
				}
				else
				{
					staticGraphicWidgetsHashTable.insert(sParamCollName,tmpParametersWidget);
				}								
			}
			//Set the values
			int nChildCount = item->childCount();
			QString sTmpString = "";
			QString sValue = "";
			for (int j=0;j<nChildCount;j++)
			{							
				if(item->child(j)->getName().toLower() == NAME_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(NAME_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == LOOP_NUMBER_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(LOOP_NUMBER_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == LOOP_AMOUNT_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(LOOP_AMOUNT_TAG,sTmpString,true,true);
				}
				else if(item->child(j)->getName().toLower() == LOOP_TARGETBLOCKID_TAG)
				{
					sTmpString = item->child(j)->getValue();
					if(sTmpString.isEmpty() == false)
						tmpParametersWidget->setParameter(LOOP_TARGETBLOCKID_TAG,sTmpString,true,true);
				}
			}
		}
	}		
	else if(false)//unknown...
	{
		ExperimentTreeItem *child = NULL;
		if(item->hasChildren())
		{
			dynamicGraphicWidget = new QWidget();
			gridLayout = new QGridLayout();	
			for (int i = 0; i < item->rowCount(); i++)
			{
				child = item->child(i);
				gridLayout->addWidget(new QLabel(child->getName(), dynamicGraphicWidget),i,0);
				QString defValue;
				if (child->getType().toString().toLower() == "enum" || child->getType().toString().toLower() == "enum_text" || child->getType().toString().toLower() == "list")
				{
					QComboBox *cbList = new QComboBox(dynamicGraphicWidget);
					cbList->addItem("<Default>");
					cbList->addItems(child->getAvailableValuesList());
					QString value = "<Default>";
					if (child->getValue() != "")
						value = child->getValue();
					defValue = child->getDefaultValue().toString();
					cbList->setCurrentIndex(cbList->findText(value));
					cbList->setToolTip(child->getDescription().toString());
					gridLayout->addWidget(cbList,i,1);
					connect(cbList, SIGNAL(currentIndexChanged(int)), this, SLOT(saveNewData()), Qt::ConnectionType(Qt::UniqueConnection));
				}
				else if (child->getType().toString().toLower() == "bool" || child->getType().toString().toLower() == "boolean")
				{
					QComboBox *cbBool = new QComboBox(dynamicGraphicWidget);
					QStringList items;
					items.append("<Default>");
					items.append("TRUE");
					items.append("FALSE");
					cbBool->addItems(items);
					QString strValue = "<Default>";
					if (child->getValue() != "")
					{
						bool value = false;
						value = QVariant(child->getValue()).toBool();
						if (value)
							strValue = "TRUE";
						else
							strValue = "FALSE";
					}
					defValue = child->getDefaultValue().toString();
					if (defValue == "1")
						defValue = "TRUE";
					else
						defValue = "FALSE";
					cbBool->setCurrentIndex(cbBool->findText(strValue));
					cbBool->setToolTip(child->getDescription().toString());
					gridLayout->addWidget(cbBool,i,1);
					connect(cbBool, SIGNAL(currentIndexChanged(int)), this, SLOT(saveNewData()), Qt::ConnectionType(Qt::UniqueConnection));
				}
				else
				{
					QLineEdit *lineEdit = new QLineEdit("<Default>",dynamicGraphicWidget);
					lineEdit->setToolTip(child->getDescription().toString());
					QString value = child->getValue();
					if (value != "")
						lineEdit->setText(value);
					defValue = child->getDefaultValue().toString();
					connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(saveNewData()), Qt::ConnectionType(Qt::UniqueConnection));
					gridLayout->addWidget(lineEdit,i,1);
				}
				if (defValue.size() > 0)
				{
					defValue = "(Default=" + defValue + ")";
					QLabel *labelDefault = new QLabel(defValue);
					gridLayout->addWidget(labelDefault,i,2);
				}
				QLabel *labelID = new QLabel(child->getUID());
				labelID->setVisible(false);
				gridLayout->addWidget(labelID,i,3);
			}
		}
	}

	if (expStructVisualizer)
	{
		if (sSelectedItemLowerName == EXPERIMENT_TAG)//BLOCKTRIALS_TAG)
		{
			expStructVisualizer->clearSelection();
		}
	}

	if(tmpParametersWidget)
	{
		tmpParametersWidget->parseDependencies();
		tmpParametersWidget->setAutoDepencyParsing(true);
		connect(tmpParametersWidget, SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(saveNewData(const QString&, const QString&)),Qt::UniqueConnection);
		//tmpParametersWidget->setFocusPolicy(Qt::StrongFocus);
		//bool bConnectResult = connect(tmpParametersWidget, SIGNAL(outOfFocus()), this, SLOT(propertySelectionWidgetFocusChange()), Qt::UniqueConnection);
		if(pCustomPropertiesDockWidget)
		{
			if (pCustomParamListTabWidget == NULL)
			{
				pCustomParamListTabWidget = new CustomChildDockTabWidget(this);
				pCustomParamListTabWidget->setGroupName(PLUGIN_EXMLDOC_EXTENSION);
				connect(pCustomParamListTabWidget, SIGNAL(currentChanged(int)), this, SLOT(customParamListTabWidgetIndexChanged(int)), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
			}
			QString sTabName;
			if (sSelectedItemLowerName == EXPERIMENT_TAG)
				sTabName = "Experiment";
			else if (sSelectedItemLowerName == OBJECT_TAG)
				sTabName = TAB_SELECTION_NAME;//"Selected Object";
			else if (sSelectedItemLowerName == BLOCK_TAG)
				sTabName = TAB_SELECTION_NAME;//"Selected Block";
			else if (sSelectedItemLowerName == LOOP_TAG)
				sTabName = TAB_SELECTION_NAME;//"Selected Loop";
			else if (sSelectedItemLowerName == PARAMETERS_TAG)
				sTabName = TAB_SELECTION_NAME;//"Selected Parameters";
			else if (sSelectedItemLowerName == CUSTOM_PARAMETERS_TAG)
				sTabName = TAB_SELECTION_NAME;//"Selected Custom Parameters";
			else if (sSelectedItemLowerName == PARAMETER_TAG)
				sTabName = TAB_SELECTION_NAME;//"Selected Parameter";
			else if (sSelectedItemLowerName == CONNECTION_TAG)
				sTabName = TAB_SELECTION_NAME;//"Selected Connection";
			else
				sTabName = item->getName();
			int nRemovedTabIndex = -1;
			bCustomParamListTabWidgetConfigured = true;
			for(int i=0;i<pCustomParamListTabWidget->count();i++)
			{
				if (pCustomParamListTabWidget->tabText(i) == sTabName || pCustomParamListTabWidget->tabText(i) == TAB_SELECTION_NAME)
				{
					pCustomParamListTabWidget->removeTab(i);
					if (pCustomParamListTabWidget->tabText(i) == sTabName)
						nRemovedTabIndex = i;
					i--;
					//break;
				}
			}
			int nNewTabIndex = pCustomParamListTabWidget->insertTab(nRemovedTabIndex,tmpParametersWidget,sTabName);
			//pCustomParamListTabWidget->setTabEnabled(nNewTabIndex, false);
			pCustomParamListTabWidget->setCurrentIndex(nNewTabIndex);
			bCustomParamListTabWidgetConfigured = false;
			pCustomPropertiesDockWidget->setWidget(pCustomParamListTabWidget);
			pCustomPropertiesDockWidget->initialize();
		}
	}
	if(dynamicGraphicWidget)
		dynamicGraphicWidget->setLayout(gridLayout);		
}

//void ExperimentGraphicEditor::propertySelectionWidgetFocusChange()
//{
//	return;
//}

void ExperimentGraphicEditor::fillTableView(const QString &textToFind, const QStringList &filters)
{
	if (pCurrentExpTreeModel)
	{
		tblWidgetView->clear();
		tblWidgetView->setRowCount(0);
		tblWidgetView->setColumnCount(4);
		tblWidgetView->setColumnHidden(3, true);
		QList<ExperimentTreeItem*> list = pCurrentExpTreeModel->getFilteredItemList(textToFind, filters);
		for (int i = 0; i < list.size(); i++)
		{
			tblWidgetView->insertRow(i);
			tblWidgetView->setItem(i, 0, new QTableWidgetItem(list.at(i)->getName()));
			tblWidgetView->setItem(i, 1, new QTableWidgetItem(list.at(i)->getValue()));
			tblWidgetView->setItem(i, 2, new QTableWidgetItem(list.at(i)->parent()->getName()));
			tblWidgetView->setItem(i, 3, new QTableWidgetItem(list.at(i)->getUID()));
		}
		tblWidgetView->setVisible(true);
	}
}

bool ExperimentGraphicEditor::selectTreeItem(const QStringList &lTextToFind, const QList<QStringList> &lFilterLists, const QList<int> &lItemIds)
{
	if (pCurrentExpTreeModel)
	{
		ExperimentTreeItem *pTmpRecursiveExpTreeItem = NULL;
		QString sTextToFind;
		for(int nRecCounter=0;nRecCounter<lTextToFind.count();nRecCounter++)
		{
			sTextToFind = lTextToFind.at(nRecCounter);
			QStringList lTmpStringList;
			if(lFilterLists.count() > nRecCounter)
				lTmpStringList = lFilterLists.at(nRecCounter);
			QList<ExperimentTreeItem*> LExpTreeItems = pCurrentExpTreeModel->getFilteredItemList(sTextToFind, lTmpStringList,pTmpRecursiveExpTreeItem);
			int nItemCount = LExpTreeItems.count();
			if(nItemCount>0)//(nItemCount == 1) || (nItemId >= 0))
			{
				for (int i=0;i<nItemCount;i++)
				{
					ExperimentTreeItem *tmpTreeItem = LExpTreeItems.at(i);
					//QString selectedUID = tmpTreeItem->getUID();
					//QString sName = tmpTreeItem->getName();
					//QString sValue = tmpTreeItem->getValue();
					bool bCorrectItemFound = false;
					if(lItemIds.count() > nRecCounter)
					{
						if(lItemIds.at(nRecCounter) >= 0)
						{
							TreeItemDefinition tmpDef = tmpTreeItem->getDefinition(ID_TAG);
							if(tmpDef.value.isValid())
							{
								if(tmpDef.value.toInt() == lItemIds.at(nRecCounter))
								{
									bCorrectItemFound = true;
								}
							}
							if (bCorrectItemFound == false)
							{
								if ((i + 1) == nItemCount)
									return false;
								continue;
							}
						}
						else
						{
							bCorrectItemFound = true;
						}
					}
					else
					{
						bCorrectItemFound = true;
					}
					if(bCorrectItemFound == false)
					{
						continue;
					}
					else 
					{
						pTmpRecursiveExpTreeItem = tmpTreeItem;
						if(nRecCounter < lTextToFind.count()-1)
							break;
					}
					QModelIndex index = pCurrentExpTreeModel->indexFromItem(tmpTreeItem);
					QModelIndex modelIndex = filterModel->mapFromSource(index);
					QModelIndex currentIndex = treeView->selectionModel()->currentIndex();
					if (modelIndex.isValid())
					{
						//if (modelIndex.internalId() == currentIndex.internalId())
						if (modelIndex == currentIndex)
							return true;
						treeView->selectionModel()->setCurrentIndex(modelIndex, QItemSelectionModel::ClearAndSelect);
						showTreeItemInfo(modelIndex);
						return true;
						//Set the focus over the value editable widget
						/*
						for (int i = 0; i < gridLayout->rowCount(); i++)
						{
							QWidgetItem *widgetItem = dynamic_cast<QWidgetItem*>(gridLayout->itemAtPosition(i,1));
							QWidgetItem *widgetUid = dynamic_cast<QWidgetItem*>(gridLayout->itemAtPosition(i,3));
							QLabel *label = dynamic_cast<QLabel*>(widgetItem->widget());
							if (widgetUid && label && widgetItem)
							{
								if (label->text() == selectedUID)
								{
									dynamicGraphicWidget->activateWindow();
									widgetItem->widget()->setFocus(Qt::OtherFocusReason);
								}
							}
						}*/
					}
				}
			}
		}
	}
	return false;
}

void ExperimentGraphicEditor::showFindDialog()
{
	if (pCurrentExpTreeModel)
	{
		findDlg = new FindDialog();
		connect(findDlg, SIGNAL(findSignal(QString, QStringList)), this, SLOT(fillTableView(QString, QStringList)), Qt::ConnectionType(Qt::UniqueConnection));
		findDlg->show();
	}
}

void ExperimentGraphicEditor::insertNode()
{
	if (pCurrentExpTreeModel)
	{
		QModelIndex currentIndex = treeView->selectionModel()->currentIndex();
		QModelIndex originalIndex = dynamic_cast<TreeFilterProxyModel*>(treeView->model())->mapToSource(currentIndex);
		if (originalIndex.isValid())
		{
			ExperimentTreeItem *item = new ExperimentTreeItem("Node", "");
			ExperimentTreeItem *brother = pCurrentExpTreeModel->itemFromIndex(originalIndex);
			if (brother->parent() != NULL)
				brother->parent()->appendRow(item);
		}
	}
}

void ExperimentGraphicEditor::insertSubnode()
{
	if (pCurrentExpTreeModel)
	{
		QModelIndex currentIndex = treeView->selectionModel()->currentIndex();
		QModelIndex originalIndex = dynamic_cast<TreeFilterProxyModel*>(treeView->model())->mapToSource(currentIndex);
		if (originalIndex.isValid())
		{
			ExperimentTreeItem *item = new ExperimentTreeItem("Subnode", "");
			ExperimentTreeItem *parent = pCurrentExpTreeModel->itemFromIndex(originalIndex);
			parent->appendRow(item);
		}
	}
}

void ExperimentGraphicEditor::removeNode()
{
	if (pCurrentExpTreeModel)
	{
		QModelIndex currentIndex = treeView->selectionModel()->currentIndex();
		QModelIndex originalIndex = dynamic_cast<TreeFilterProxyModel*>(treeView->model())->mapToSource(currentIndex);
		if (originalIndex.isValid())
			pCurrentExpTreeModel->removeRow(originalIndex.row(), originalIndex.parent());
	}
}

void ExperimentGraphicEditor::toggleBlocksView()
{
	bShowGraphicalTreeView = !bShowGraphicalTreeView;
	showTreeItemInfo(treeView->currentIndex());
}

//void ExperimentGraphicEditor::switchToDefaultView()
//{
//	bool bNativeTextualView = true;
//	QString sFilePath = getCurrentFileName();
//	QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowReOpenSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, sFilePath), Q_ARG(bool, bNativeTextualView));
//}

void ExperimentGraphicEditor::addDefinition()
{
	if (pCurrentExpTreeModel)
	{
		QModelIndex currentIndex = treeView->selectionModel()->currentIndex();
		QModelIndex originalIndex = dynamic_cast<TreeFilterProxyModel*>(treeView->model())->mapToSource(currentIndex);
		if (originalIndex.isValid())
		{
			ExperimentTreeItem *item = pCurrentExpTreeModel->itemFromIndex(originalIndex);
			attWidget = new AttributeWidget(item);
			attWidget->show();
		}
	}
}

void ExperimentGraphicEditor::saveNewData()
{
	QModelIndex sourceIndex = filterModel->mapToSource(treeView->selectionModel()->currentIndex());
	pCurrentExpTreeModel->saveNewData(dynamicGraphicWidget, sourceIndex);
}

void ExperimentGraphicEditor::saveNewData(const QString &sName, const QString &sValue)
{
	QModelIndex sourceIndex = filterModel->mapToSource(treeView->selectionModel()->currentIndex());
	pCurrentExpTreeModel->saveNewData(sName, sValue, sourceIndex);	
}

void ExperimentGraphicEditor::setViewFilter(const TreeFilterSettings &newViewSettings)
{
	currentViewSettings = newViewSettings;
	if(filterModel)
		filterModel->setTreeFilterSettings(currentViewSettings);
}

void ExperimentGraphicEditor::showTableviewExperimentDialog()
{
	if (pCurrentExpTreeModel == NULL)
		return;
	if (expBlockParamView == NULL)
		expBlockParamView = new ExperimentBlockParameterView(NULL, this, pCurrentExpTreeModel);
	if (expBlockParamView)
	{
		//bool bConnectResult = 
		connect(expBlockParamView, SIGNAL(destroyed(QWidget*)), this, SLOT(childWidgetDestroyed(QWidget*)), Qt::UniqueConnection);
		expBlockParamView->resize(600, 400);
		expBlockParamView->show();
	}
}

void ExperimentGraphicEditor::showVisualExperimentDialog()
{
	if (pCurrentExpTreeModel == NULL)
		return;
	if (expStructVisualizer == NULL)
	{
		expStructVisualizer = new ExperimentStructureVisualizer();
		nAdditionalMenuHeight = expStructVisualizer->getAdditionalMenuHeight();
	}
	if (expStructVisualizer)
	{
		bool bResult = connect(expStructVisualizer, SIGNAL(destroyed(QWidget*)), this, SLOT(childWidgetDestroyed(QWidget*)),Qt::UniqueConnection);
		bResult = expStructVisualizer->setExperimentTreeModel(pCurrentExpTreeModel);
		if (bResult)
		{
			mainLayout->addWidget(expStructVisualizer);
			expStructVisualizer->showMaximized();
			if (toolBar)
				expStructVisualizer->setVisualMargin(0, toolBar->height() * 2);
			//bResult = connect(expStructVisualizer, SIGNAL(GraphItemSelectionChanged(QList<ExperimentVisualizerGraphItemTypeEnum>, QList<int>)), this, SLOT(changeSelection(QList<ExperimentVisualizerGraphItemTypeEnum>, QList<int>)), Qt::UniqueConnection);
			//bResult = connect(this, SIGNAL(OnResized(const int &, const int &)), expStructVisualizer, SLOT(resizeStructureView(const int &, const int &)));
			//bResult = connect(expStructVisualizer, SIGNAL(GraphRedrawn()), this, SLOT(forceEmitResize()));
			//expStructVisualizer->resizeStructureView(expStructVisualizer->width(), expStructVisualizer->height());
			this->show();
			this->resize(600, 600);
			expStructVisualizer->resizeStructureView(expStructVisualizer->width(), expStructVisualizer->height());
			return;
		}
	}
}

void ExperimentGraphicEditor::setNewFileName(const QString &sExpTreeModelCanonFilePath)
{ 
	sCurrentCanonFilePath = sExpTreeModelCanonFilePath; 
	if (expManager)
	{
		if(expManager->setExperimentFileName(sExpTreeModelCanonFilePath))
		{
			expManager->reparseExperiment();
		}
	}
}

QString ExperimentGraphicEditor::getCurrentFileName()
{
	return sCurrentCanonFilePath;
}

bool ExperimentGraphicEditor::setExperimentTreeModel(ExperimentTreeModel *expNewModel, const QString &sExpTreeModelCanonFilePath)
{
	if (pCurrentExpTreeModel != expNewModel)
	{
		if (expNewModel == NULL)//is the new model NULL?
			loadedExpTreeModel.reset(false);
		else
			loadedExpTreeModel.reset(true);
		if(pCurrentExpTreeModel)
			disconnect(pCurrentExpTreeModel, SIGNAL(modelModified()), this, SLOT(treeModelChanged()));
		if (expNewModel)
			connect(expNewModel, SIGNAL(modelModified()), this, SLOT(treeModelChanged()), Qt::ConnectionType(Qt::UniqueConnection));
		pCurrentExpTreeModel = expNewModel;
		selectedIndex = QModelIndex();
		setNewModel();
	}
	if (getCurrentFileName() != sExpTreeModelCanonFilePath)
		setNewFileName(sExpTreeModelCanonFilePath);
	return true;
}

void ExperimentGraphicEditor::treeModelChanged()
{
	emit ContentHasChanged(sCurrentCanonFilePath,true);
	configureCustomParameterWidgets();
}

void ExperimentGraphicEditor::childWidgetDestroyed(QWidget* pWidget)
{
	if(pWidget == expStructVisualizer)
	{
		expStructVisualizer = NULL;
	}	
	else if(pWidget == expBlockParamView)
	{
		expBlockParamView = NULL;
	}
}

//void ExperimentGraphicEditor::showEvent(QShowEvent *event)
//{
//	forceEmitResize();
//}

//void ExperimentGraphicEditor::focusInEvent(QFocusEvent *event)
//{
//	forceEmitResize();
//}

void ExperimentGraphicEditor::resizeEvent(QResizeEvent *event)
{
	int nWidth = event->size().width();
	int nHeight = event->size().height();
	if (nHeight > nAdditionalMenuHeight)
		emit OnResized(nWidth, nHeight - nAdditionalMenuHeight);
	else
		emit OnResized(nWidth, nHeight);
}

void ExperimentGraphicEditor::forceEmitResize()
{
	int nWidth = this->width();
	int nHeight = this->height();
	if (nHeight > nAdditionalMenuHeight)
		emit OnResized(nWidth, nHeight - nAdditionalMenuHeight);
	else
		emit OnResized(nWidth, nHeight);
}

void ExperimentGraphicEditor::customParamListTabWidgetIndexChanged(int nIndex)
{
	if ((nIndex != nLastCustomParamListTabWidgetIndex) && (nIndex >= 0) && (bCustomParamListTabWidgetConfigured==false))
	{
		nLastCustomParamListTabWidgetIndex = nIndex;
		QString sTabText = pCustomParamListTabWidget->tabText(nIndex);
		if (sTabText.toLower() == EXPERIMENT_TAG)
			clearSelection();
		return;
	}
	nLastCustomParamListTabWidgetIndex = nIndex;
}