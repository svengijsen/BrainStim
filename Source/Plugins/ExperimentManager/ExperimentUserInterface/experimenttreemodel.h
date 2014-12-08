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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QStandardItemModel>
#include <QDomDocument>
#include "experimentgraphiceditor_global.h"
#include "experimentstructures.h"
#include "experimenttreeitem.h"
#include "propertysetting.h"

#define EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID		-1
#define EXPERIMENTTREEMODEL_FILTER_TAGS				"TAGS"
#define EXPERIMENTTREEMODEL_FILTER_VALUES			"VALUES"
#define EXPERIMENTTREEMODEL_FILTER_ATTRIBUTES		"ATTRIBUTES"
#define EXPERIMENTTREEMODEL_FILTER_CASE_SENSITIVE	"CASE_SENSITIVE"

class QXmlStreamWriter;
class ExperimentTreeItem;

class ExperimentTreeModel : public QStandardItemModel
{
    Q_OBJECT

    signals:
        void modelModified();

    public:
        explicit ExperimentTreeModel(QObject *parent = 0);
		ExperimentTreeModel(const ExperimentTreeModel& other);
		~ExperimentTreeModel();

		static QScriptValue ctor__pExperimentTreeModel(QScriptContext* context, QScriptEngine* engine);
		static QScriptValue experimentTreeModelToScriptValue(QScriptEngine *engine, ExperimentTreeModel* const &s);
		static void experimentTreeModelFromScriptValue(const QScriptValue &obj, ExperimentTreeModel* &s);

		struct  strcParameterSpecifier
		{
			int nBlockID;
			int nObjectID;
			QString sParamHexID;
			QString sParamName;
			PropertySettingEditingType cParamEditType;
			strcParameterSpecifier()
			{
				nBlockID = -1;
				nObjectID = -1;
				sParamName = "";
				sParamHexID = "";
				cParamEditType = PSET_DEFINED;
			}
		};

		struct  strcBlockLoopSpecifier
		{
			int nBlockID;
			int nLoopID;
			QString sLoopDefinitionName;
			strcBlockLoopSpecifier()
			{
				sLoopDefinitionName = "";
				nBlockID = -1;
				nLoopID = -1;
			}
		};

		struct strcObjectSpecifier
		{
			int nObjectID;
			QString sObjectName;
			QString sObjectClass;
			QString sObjectDefinitionName;
			strcObjectSpecifier()
			{
				nObjectID = -1;
				sObjectName = "";
				sObjectClass = "";
				sObjectDefinitionName = "";
			}
		};

		struct strcObjectInitFinitParameterSpecifier
		{
			int nParamID;
			QString sType;
			QString sName;
			QString sValue;
			strcObjectInitFinitParameterSpecifier()
			{
				nParamID = -1;
				sType = "";
				sName = "";
				sValue = "";
			}
		};

		struct strcObjectInitFinitSpecifier
		{
			int nInitFinitID;
			int nOrderNumber;
			int nObjectID;
			QString sType;
			QString sSignature;
			QString sObjectDefinitionName;
			QList<strcObjectInitFinitParameterSpecifier> lParameters;
			strcObjectInitFinitSpecifier()
			{
				nInitFinitID = -1;
				nOrderNumber = -1;
				nObjectID = -1;
				sType = METHOD_TYPE_SLOT_TAG;
				sSignature = "";
				sObjectDefinitionName = "";
			}
		};

		struct strcObjectConnectionSpecifier
		{
			int nConnectionID;
			int nSourceObjectID;
			int nTargetObjectID;
			QString sSourceSignature;
			QString sTargetSignature;
			ExperimentStructuresNameSpace::MethodType mSourceMethodType;
			ExperimentStructuresNameSpace::MethodType mTargetMethodType;
			QString sConnectionDefinitionName;
			strcObjectConnectionSpecifier()
			{
				nConnectionID = -1;
				nSourceObjectID = -1;
				nTargetObjectID = -1;
				sSourceSignature = "";
				sTargetSignature = "";
				mSourceMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_UNDEFINED;
				mTargetMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_UNDEFINED;
				sConnectionDefinitionName = "";
			}
		};

		//QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
        ExperimentTreeItem *getRootItem() const { return rootItem; }
        bool write(const QString &fileName);
        QList<ExperimentTreeItem*> getFilteredItemList(const QString &textToFind, const QStringList &filters, ExperimentTreeItem *expTreeItem = NULL);
        QModelIndex getIndexByUID(const QString &uuid);
        ExperimentTreeItem *itemFromIndex(const QModelIndex &index) const;

		bool read(QByteArray &byteArrayContent);
		bool reset();
		//int getDocumentElements(const QStringList &sElementTagName,QDomNodeList &ResultDomNodeList);
		int getTreeElements(const QStringList &sElementTagName, QList<ExperimentTreeItem *> &lFoundTreeItems, ExperimentTreeItem *pSearchRootItem = NULL);
		static int getStaticTreeElements(const QStringList &sElementTagName, QList<ExperimentTreeItem *> &lFoundTreeItems, ExperimentTreeItem *pSearchRootItem);
		
		QList<ExperimentStructuresNameSpace::strcExperimentObject> getDefinedExperimentObjectInfoList(ExperimentTreeItem *objItem);

		bool removeExperimentBlocks(const QList<int> &lBlockIDs);
		bool moveExperimentBlocks(const QList<int> &lBlockIDsToMove, const int &nBlockIDToSwitch, const int &nBlockNumberChangeDirection);
		bool moveExperimentObjectInitFinit(const QList<int> &lInitFinitsToMove, const int &nInitFinitIDToSwitch, const int &nMoveDirection, const bool &bIsInit);
	
	public slots:
		bool initializeExperiment();
		bool resetExperiment();
		bool addExperimentBlocks(const int &nAmount = 1);
	public:
		bool removeExperimentParameters(const QList<ExperimentTreeModel::strcParameterSpecifier> lstParameterSpecifiers);
		bool renameExperimentParameter(const ExperimentTreeModel::strcParameterSpecifier &cParameterSpecifier, const QString &sNewName);
		bool addUpdateExperimentBlockParameter(const ExperimentTreeModel::strcParameterSpecifier &cParameterSpecifier, const QString &sNewValue, const bool &bIsCustom);
		bool updateExperimentBlockLoop(const ExperimentTreeModel::strcBlockLoopSpecifier &cBlockLoopSpecifier, const QString &sNewValue);
		bool addExperimentBlockLoop(const int &nSourceBlockId, const int &nTargetBlockId, const int &nLoopAmount, const QString &sLoopName);
		bool removeExperimentBlockLoop(const ExperimentTreeModel::strcBlockLoopSpecifier &cBlockLoopSpecifier);
		bool changeExperimentBlockLoopNumber(const ExperimentTreeModel::strcBlockLoopSpecifier &cBlockLoopSpecifier, const int &nLoopNumberCorrection);
		bool addExperimentObject(const QString &sObjectName, const QString &sClassName);
		bool removeExperimentObject(const int &nObjectId);
		bool updateExperimentObject(const ExperimentTreeModel::strcObjectSpecifier &cObjectSpecifier, const QString &sNewValue);
		bool addObjectConnection(const ExperimentTreeModel::strcObjectConnectionSpecifier &cConnectionSpecifier);
		bool removeObjectConnection(const int &nConnectionId);
		bool updateObjectConnection(const ExperimentTreeModel::strcObjectConnectionSpecifier &cConnectionSpecifier, const QString &sNewValue);
		bool updateExperimentObjectInitFinitParameter(const ExperimentTreeModel::strcObjectInitFinitSpecifier &cObjectInitFinitSpecifier, const QString &sNewValue, const bool bIsInit);
		bool removeExperimentObjectInitFinit(const int &nInitFinitId, const bool bIsInit);
		bool addExperimentObjectInitFinit(const ExperimentTreeModel::strcObjectInitFinitSpecifier &cObjectInitFinitSpecifier, const bool bIsInit);

		ExperimentTreeItem* getExperimentObjectTreeItem(const int &nObjectID);
		ExperimentTreeItem* getExperimentBlockTreeItem(const int &nBlockID);
		ExperimentTreeItem* getExperimentParameterTreeItem(const int &nBlockID, const int &nObjectID, const QString &sParamName);
		ExperimentTreeItem* getExperimentBlockLoopTreeItem(const int &nBlockID, const int &nLoopID, const QString &sLoopItemName = "");
		ExperimentTreeItem* getExperimentObjectConnectionTreeItem(const int &nConnectionID);
		ExperimentTreeItem* addExperimentBlockTreeItems(const int &nAmount = 1);
		ExperimentTreeItem* getExperimentObjectInitFinitTreeItem(const int &nInitFinitID, const bool &bIsInit);

    public slots:
        void saveNewData(QWidget *widgetContainer, const QModelIndex &parentIndex);
		void saveNewData(const QString &sName, const QString &sValue, const QModelIndex &parentIndex, ExperimentTreeItem *pParametersSection = NULL);
		bool saveNewData(const int &nBlockID, const int &nObjectID, const QString &sParamName, const QString &sParamValue, const bool &bIsCustomParameter = false);

    private:
		void recursiveRead(QDomNode dNode, ExperimentTreeItem *item);
        void recursiveWrite(QXmlStreamWriter &xml, ExperimentTreeItem *item);
        static void recursiveSearch(const QString &textToFind, const QStringList &filters, ExperimentTreeItem *item, QList<ExperimentTreeItem *> &list);
        static void recursiveMultiSearch(const QString &textToFind, const QStringList &filters, QList<ExperimentTreeItem *> items, QList<ExperimentTreeItem*> &list);
		void recursiveUidSearch(const QString &uuid, ExperimentTreeItem *item, bool found, QModelIndex &index);
		bool fillModel();

		int cleanupObjectDependencies(const int &nObjectId);
		int cleanupBlockDependencies(const int &nBlockId);

		QDomDocument *doc;
		QDomElement *root;
        ExperimentTreeItem *rootItem;
		ExperimentManagerNameSpace::EXML_DocVersion nXMLCurrentClassVersion;
		ExperimentManagerNameSpace::EXML_DocVersion nXMLDocumentVersion;
};

Q_DECLARE_METATYPE(ExperimentTreeModel*)

#endif // TREEMODEL_H
