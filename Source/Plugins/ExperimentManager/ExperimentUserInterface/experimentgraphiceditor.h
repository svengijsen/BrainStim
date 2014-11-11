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

#ifndef ExperimentGraphicEditor_H
#define ExperimentGraphicEditor_H

#define APP_NAME							"Experiment Graphic Editor"
#define VERSION								"v1.0"
#define TABLEVIEWINDEX						1
#define CUSTOMOBJECTPARAMS_POSTSTRING		"_Custom"
#define SLOT_REGISTERDOCKWIDGET_SIGNATURE	"registerDockWidget"
#define NAME_DOCKWIDGET_LIST_VIEW			"Parameter List View"
#define TOGGLE_DOCKWIDGET_LIST_VIEW_TEXT	"Show/Hide Parameter List View"
#define NAME_DOCKWIDGET_TABLE_VIEW			"Parameters Table View"
#define NAME_DOCKWIDGET_TREE_VIEW			"Experiment Tree View"
#define TOGGLE_DOCKWIDGET_TABLE_VIEW_TEXT	"Show/Hide Parameter Table View"
#define TOGGLE_DOCKWIDGET_TREE_VIEW_TEXT	"Show/Hide Experiment Tree View"
#define TAB_SELECTION_NAME					"Selection"

#include <QWidget>
#include <QModelIndex>
#include <QVBoxLayout>
#include "customdockwidget.h"
#include "experimentstructures.h"
#include "experimenttreemodel.h"
#include "experimentstructurevisualizer.h"
#include "experimentgraphiceditor_global.h"

class QTabWidget;
class QMenu;
class QToolButton;
class QVBoxLayout;
class QTreeView;
class QScrollArea;
class QTableWidget;
class QGridLayout;
class QToolBar;
class QtProperty;
class QSplitter;
class FindDialog;
class AttributeWidget;
class ExperimentTreeModel;
class ExperimentTreeItem;
class TreeFilterProxyModel;
class ExperimentManager;
class cExperimentStructure;
class ExperimentParameterDefinitionContainer;
class ExperimentParameterVisualizer;
class ExperimentBlockParameterView;

class ExperimentGraphicEditor : public QWidget
{
    Q_OBJECT

signals:
	void OnResized(const int &nNewWidth, const int &nNewHeight);
	void copyAvailable(bool bCopyAvailable);//this signature is and should stay fixed, do not rename!
	void IsDestructing(ExperimentGraphicEditor *pExpGraphEditor);
	void IsClosing(QString sCanoFilePath, bool bShouldAutoSaveChanges);
	void ContentHasChanged(QString sCanoFilePath, bool bHasChanged);
        
public:
    explicit ExperimentGraphicEditor(QWidget *parent = 0);
    ~ExperimentGraphicEditor();

	void setNewFileName(const QString &sExpTreeModelCanonFilePath = "") {sCurrentCanonFilePath = sExpTreeModelCanonFilePath;};
        
public slots:
	void setExperimentManager(ExperimentManager *pExpManager);
	bool setExperimentTreeModel(ExperimentTreeModel *expModel = NULL, const QString &sExpTreeModelCanonFilePath = "");
	QString saveFile(const QString &sFilePath = "");
	void createDockWindows();
	bool selectTreeItem(const QStringList &lTextToFind, const QList<QStringList> &lFilterLists, const QList<int> &lItemIds = QList<int>());
	void changeSelection(QList<ExperimentVisualizerGraphItemTypeEnum> lExpVisGraphItemTypes, QList<int> lItemIds);
	void clearSelection();

protected slots:
	void resizeEvent(QResizeEvent *event);

private slots:	
    void newFile();
	void openFile();
	void closeFile();
	void closeDocument();
	void showTreeItemInfo(const QModelIndex &index);
	void fillTableView(const QString &textToFind, const QStringList &filters);
    void showFindDialog();
    void setNewModel();
    void insertNode();
    void insertSubnode();
    void removeNode();
    void addDefinition();
	void toggleBlocksView();
	void switchToDefaultView();
    void saveNewData();
	void saveNewData(const QString &sName, const QString &sValue);
	void setViewFilter(const TreeFilterSettings &newViewSettings);
	//void updateWindowTitle(const QString sSuffix = QString(""));
	void treeModelChanged();
	void childWidgetDestroyed(QWidget* pWidget);
	void forceEmitResize();
	//void propertySelectionWidgetFocusChange();

private:
	void configureActions(bool bCreate);
	void setupExperimentTreeView();
	void setupMenuAndActions();
	void setupLayout();
	void setupFilterModel();
	void configureCustomParameterWidgets();

	bool bShowGraphicalTreeView;

	CustomChildDockTreeViewWidget *treeView;
	QToolBar *toolBar;
	QWidget *dynamicGraphicWidget;
	QGridLayout *gridLayout;

	QTableWidget *tblWidgetView;
	TreeFilterProxyModel *filterModel;
	ExperimentTreeModel *pExpTreeModel;		//used for setExperimentTreeModel() -> Pointer
	ExperimentTreeModel loadedExpTreeModel; //used for openFile();
	FindDialog *findDlg;
	AttributeWidget *attWidget;	

	QVBoxLayout *mainLayout;
	QAction *actionNew_File;
	QAction *action_Open_File;
	QAction *action_Save;
	QAction *action_Close_File;
	QAction *actionFind;
	QAction *action_Quit;
	QAction *actionAdd_Node;
	QAction *actionAdd_Subnode;
	QAction *action_Remove_Node;
	QAction *actionAdd_Attribute;
	QAction *actionToggleBlocksView;
	QAction *actionSwitchToDefaultView;
	QAction *actionToggle_ListViewDockVisibility;
	QAction *actionToggle_TableViewDockVisibility;
	QMenu *menuFile;
	QMenu *menuEdit;
	QMenu *menuView;
	QToolButton *buttonFile;
	QToolButton *buttonEdit;
	QToolButton *buttonView;

	CustomChildDockTabWidget *pCustomParamListTabWidget;
	customDockWidget *pCustomPropertiesDockWidget;
	customDockWidget *pCustomParamTableDockWidget;
	customDockWidget *pCustomExperimentTreeDockWidget;
	TreeFilterSettings currentViewSettings;
	QModelIndex selectedIndex;

	ExperimentManager *expManager;
	QString sCurrentCanonFilePath;
	ExperimentStructureVisualizer *expStructVisualizer;
	ExperimentBlockParameterView *expBlockParamView;
	cExperimentStructure *tmpExpStruct;					//Only temporarily use!
	ExperimentParameterDefinitionContainer *tmpExpObjectParamDefs;
	QHash<QString, ExperimentParameterVisualizer *> staticGraphicWidgetsHashTable;
};

#endif // ExperimentGraphicEditor_H
