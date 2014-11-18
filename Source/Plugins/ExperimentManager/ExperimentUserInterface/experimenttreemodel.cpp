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

#include <QFile>
#include <QXmlStreamWriter>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include "experimenttreemodel.h"
#include "experimentparameterdefinition.h"

ExperimentTreeModel::ExperimentTreeModel(QObject *parent) : QStandardItemModel(parent)
{
	doc = NULL;
	root = NULL;
    rootItem = NULL;

	nXMLDocumentVersion.major = 1;
	nXMLDocumentVersion.minor = 0;
	nXMLDocumentVersion.version = 0;
	nXMLDocumentVersion.build = 0;
	nXMLCurrentClassVersion.major = PLUGIN_EXMLDOC_MINIMAL_VERSION_MAJOR;
	nXMLCurrentClassVersion.minor = PLUGIN_EXMLDOC_MINIMAL_VERSION_MINOR;
	nXMLCurrentClassVersion.version = PLUGIN_EXMLDOC_MINIMAL_VERSION_REVISION;
	nXMLCurrentClassVersion.build = PLUGIN_EXMLDOC_MINIMAL_VERSION_BUILD;
}

ExperimentTreeModel::~ExperimentTreeModel()
{
	reset();
}

ExperimentTreeModel::ExperimentTreeModel(const ExperimentTreeModel& other)
{
	Q_UNUSED(other);
	//TODO fill in copy constructor, should be declared for the Q_DECLARE_METATYPE macro
}

QScriptValue ExperimentTreeModel::ctor__pExperimentTreeModel(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new ExperimentTreeModel(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue ExperimentTreeModel::experimentTreeModelToScriptValue(QScriptEngine *engine, ExperimentTreeModel* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void ExperimentTreeModel::experimentTreeModelFromScriptValue(const QScriptValue &obj, ExperimentTreeModel* &s)
{
	s = qobject_cast<ExperimentTreeModel*>(obj.toQObject());
}


//QVariant ExperimentTreeModel::data(const QModelIndex &index, int role) const
//{
//	if (role == Qt::TextAlignmentRole) 
//		return Qt::AlignRight;
//	return QStandardItemModel::data(index, role);
//}

bool ExperimentTreeModel::fillModel()
{
	if(doc->isNull() || root->isNull() || rootItem == NULL)
		return false;

	for (int i = 0; i < root->attributes().size(); i++)//version --> <EXML version="2.1.0.0">
	{
		//tmpString = root.attributes().item(i).nodeName();
		//tmpString = root.attributes().item(i).nodeValue();
		rootItem->addDefinition(root->attributes().item(i).nodeName(), root->attributes().item(i).nodeValue(), TreeItemType_Attribute);
	}

	QDomNode dNode = root->firstChild();//<defines>
    recursiveRead(dNode, rootItem);
	setItem(0, rootItem);
	return true;
}

void ExperimentTreeModel::recursiveRead(QDomNode dNode, ExperimentTreeItem *item)
{
    do
    {
        int totalOfChilds = dNode.childNodes().size();
        if (dNode.nodeType() != QDomNode::CommentNode)
        {
            if (totalOfChilds == 0)
            {
                if (dNode.nodeType() == QDomNode::TextNode)
				{
					//tmpString = dNode.nodeValue();
                    item->setValue(dNode.nodeValue());
				}
                else
                {
					//tmpString = dNode.nodeName();
                    ExperimentTreeItem *subItem = new ExperimentTreeItem(dNode.nodeName());
					for (int i = 0; i < dNode.attributes().size(); i++)
					{
						//tmpString = dNode.attributes().item(i).nodeName();
                        subItem->addDefinition(dNode.attributes().item(i).nodeName(), dNode.attributes().item(i).nodeValue(), TreeItemType_Attribute);
					}
                    item->appendRow(subItem);
                }
            }
            else
            {
				//tmpString = dNode.nodeName();
                ExperimentTreeItem *item2 = new ExperimentTreeItem(dNode.nodeName());
                for (int i = 0; i < dNode.attributes().size(); i++)
				{
					//tmpString = dNode.attributes().item(i).nodeName();
					//tmpString = dNode.attributes().item(i).nodeValue();
					item2->addDefinition(dNode.attributes().item(i).nodeName(), dNode.attributes().item(i).nodeValue(), TreeItemType_Attribute);
				}
                for (int i = 0; i < totalOfChilds; i++)
				{
                    if (dNode.childNodes().size() > 0 && i == 0)
					{
                        recursiveRead(dNode.childNodes().at(i), item2);
					}
				}
                item->appendRow(item2);
            }
        }
		else//This is an comment node...
		{
			//tmpString = dNode.nodeValue();
			//tmpString = dNode.nodeName();
			ExperimentTreeItem *item3 = new ExperimentTreeItem(DEFINITION_TAG_COMMENT);
			item3->addDefinition(DEFINITION_NAME_VALUE, dNode.nodeValue(), TreeItemType_Comment);
			item->appendRow(item3);			
		}
        dNode = dNode.nextSibling();
    }
    while (!dNode.isNull());
}

bool ExperimentTreeModel::saveNewData(const int &nBlockID, const int &nObjectID, const QString &sParamName, const QString &sParamValue, const bool &bIsCustomParameter)
{
	QStringList sFilterList;
	TreeItemDefinition tmpTreeItemDef;
	int nTempObjectID;
	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;

	if(nObjectID >= 0)
	{
		ExperimentTreeItem* tmpItem3 = getExperimentBlockTreeItem(nBlockID);
		if(tmpItem3)
		{
			QList<ExperimentTreeItem*> list4 = getFilteredItemList(OBJECT_TAG, sFilterList, tmpItem3);
			bool bBlockObjectDefPresent = false;
			foreach (ExperimentTreeItem* tmpItem4 ,list4)
			{
				if(tmpItem4->getDefinitions().contains(ID_TAG))
				{
					nTempObjectID = tmpItem4->getDefinition(ID_TAG).value.toInt();
					if(nTempObjectID == nObjectID)//Correct Object ID?
					{
						bBlockObjectDefPresent = true;
						QList<ExperimentTreeItem*> list5;
						if(bIsCustomParameter)
							list5 = getFilteredItemList(CUSTOM_PARAMETERS_TAG, sFilterList, tmpItem4);
						else
							list5 = getFilteredItemList(PARAMETERS_TAG, sFilterList, tmpItem4);
						if(list5.isEmpty())//Still no parameters or custom_parameters tag?
						{
							if(bIsCustomParameter)
								list5.append(new ExperimentTreeItem(CUSTOM_PARAMETERS_TAG, ""));
							else
								list5.append(new ExperimentTreeItem(PARAMETERS_TAG, ""));
							tmpItem4->appendRow(list5.first());			
						}
						foreach (ExperimentTreeItem* tmpItem5 ,list5)
						{
							this->saveNewData(sParamName,sParamValue,QModelIndex(),tmpItem5);
							return true;
						}
						return false;
					}
				}
			}
			if(bBlockObjectDefPresent == false)//Do we need to add the Block/Object/Params def struct because it was not yet available?
			{
					ExperimentTreeItem* tmpNewObjectItem = new ExperimentTreeItem(OBJECT_TAG);
					tmpNewObjectItem->addDefinition(ID_TAG,QString::number(nObjectID),TreeItemType_Attribute);
					ExperimentTreeItem* tmpNewParametersSectionItem;
					if(bIsCustomParameter)
						tmpNewParametersSectionItem = new ExperimentTreeItem(CUSTOM_PARAMETERS_TAG,"");
					else
						tmpNewParametersSectionItem = new ExperimentTreeItem(PARAMETERS_TAG,"");
					tmpNewObjectItem->appendRow(tmpNewParametersSectionItem);
					tmpItem3->appendRow(tmpNewObjectItem);
					bool bRetryResult = saveNewData(nBlockID, nObjectID, sParamName, sParamValue, bIsCustomParameter);
					return bRetryResult;
			}
			return false;
		}
	}
	else if(nObjectID == EXPERIMENTTREEMODEL_BLOCKOBJECT_INDEXID)
	{
		ExperimentTreeItem* tmpItem1 = getExperimentBlockTreeItem(nBlockID);
		if(tmpItem1)
		{
			if (tmpItem1->hasChildren())
			{
				for (int i=0;i<tmpItem1->rowCount(); i++)
				{
					if(tmpItem1->child(i)->getName().toLower() == sParamName)
					{
						tmpItem1->child(i)->setValue(sParamValue);
						emit modelModified();
						return true;
					}
				}
			}
		}
	}	
	return false;
}

bool ExperimentTreeModel::addExperimentObject(const QString &sObjectName, const QString &sClassName)
{
	if((sObjectName.isEmpty()) || (sClassName.isEmpty()))
		return false;
	ExperimentTreeItem *pTmpExpTreeItem = NULL;
	QStringList sFilterList;
	int nTempObjectID;
	QString sTempObjectName;
	QString sTempObjectClass;
	QList<ExperimentTreeItem*> lstDeclarations;
	QList<ExperimentTreeItem*> lstObjects;
	QList<int> lstUsedObjectIds;
	ExperimentTreeItem* pTempObjectTreeItem = NULL;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	lstDeclarations = getFilteredItemList(DECLARATIONS_TAG, sFilterList);
	if(lstDeclarations.count()!=1)
		return false;
	foreach (ExperimentTreeItem* tmpItem1 ,lstDeclarations)
	{
		lstObjects = getFilteredItemList(OBJECT_TAG, sFilterList, tmpItem1);
		foreach (pTempObjectTreeItem ,lstObjects)
		{
			if(pTempObjectTreeItem->getDefinitions().contains(ID_TAG))
			{
				nTempObjectID = pTempObjectTreeItem->getDefinition(ID_TAG).value.toInt();
				if(nTempObjectID >= 0)
				{
					if (pTempObjectTreeItem->hasChildren())
					{
						pTmpExpTreeItem = pTempObjectTreeItem->firstChild(NAME_TAG);
						if(pTmpExpTreeItem)
						{
							sTempObjectName = pTmpExpTreeItem->getValue();
							if(sTempObjectName == sObjectName)//Name already in use?
								return false;
						}
					}
					lstUsedObjectIds.append(nTempObjectID);
				}
			}
		}
	}
	ExperimentTreeItem* tmpNewObjectItem = new ExperimentTreeItem(OBJECT_TAG);
	int nNewID = 0;
	while(lstUsedObjectIds.contains(nNewID))
		nNewID++;
	tmpNewObjectItem->addDefinition(ID_TAG,QString::number(nNewID),TreeItemType_Attribute);
	tmpNewObjectItem->appendRow(new ExperimentTreeItem(NAME_TAG,sObjectName));
	tmpNewObjectItem->appendRow(new ExperimentTreeItem(CLASS_TAG,sClassName));
	lstDeclarations[0]->insertRow(lstDeclarations[0]->rowCount(),tmpNewObjectItem);
	emit modelModified();
	return true;
}

bool ExperimentTreeModel::addExperimentObjectInitFinit(const ExperimentTreeModel::strcObjectInitFinitSpecifier &cObjectInitFinitSpecifier, const bool bIsInit)
{
	QStringList sFilterList;
	int nTempInitFinitID;
	QList<int> lFoundInitFinitIds;
	QString sMainSectionTag;
	QString sSubSectionTag;
	bool bAppendReady = false;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	if(bIsInit)
	{
		sMainSectionTag = INITIALIZATIONS_TAG;
		sSubSectionTag = INITIALIZATION_TAG;
	}
	else
	{
		sMainSectionTag = FINALIZATIONS_TAG;
		sSubSectionTag = FINALIZATION_TAG;
	}
	QList<ExperimentTreeItem*> list1 = getFilteredItemList(sMainSectionTag, sFilterList);
	if(list1.count() != 1)
		return false;
	else
		bAppendReady = true;
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(sSubSectionTag, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{
			if(tmpItem2->getDefinitions().contains(ID_TAG))
			{
				nTempInitFinitID = tmpItem2->getDefinition(ID_TAG).value.toInt();
				if(nTempInitFinitID >= 0)
				{
					lFoundInitFinitIds.append(nTempInitFinitID); 
				}
			}
		}
	}
	if(bAppendReady)
	{
		ExperimentTreeItem* tmpNewConnectionItem = new ExperimentTreeItem(sSubSectionTag);
		int nNewId = cObjectInitFinitSpecifier.nInitFinitID;
		while(lFoundInitFinitIds.contains(nNewId))
			nNewId++;
		tmpNewConnectionItem->addDefinition(ID_TAG,QString::number(nNewId),TreeItemType_Attribute);
		ExperimentTreeItem* pObjectItem = new ExperimentTreeItem(OBJECT_TAG);
		pObjectItem->addDefinition(ID_TAG,QString::number(cObjectInitFinitSpecifier.nObjectID),TreeItemType_Attribute);
		pObjectItem->appendRow(new ExperimentTreeItem(INIT_FINIT_TYPE_TAG,cObjectInitFinitSpecifier.sType));
		pObjectItem->appendRow(new ExperimentTreeItem(INIT_FINIT_SIGNATURE_TAG,cObjectInitFinitSpecifier.sSignature));
		ExperimentTreeItem* pParametersItem = new ExperimentTreeItem(PARAMETERS_TAG);
		for(int i=0;i<cObjectInitFinitSpecifier.lParameters.count();i++)
		{
			ExperimentTreeItem* pParameterItem = new ExperimentTreeItem(PARAMETER_TAG);
			pParameterItem->addDefinition(ID_TAG,QString::number(cObjectInitFinitSpecifier.lParameters.at(i).nParamID),TreeItemType_Attribute);
			pParameterItem->appendRow(new ExperimentTreeItem(NAME_TAG,cObjectInitFinitSpecifier.lParameters.at(i).sName));
			pParameterItem->appendRow(new ExperimentTreeItem(MEMBER_TYPE_TAG,cObjectInitFinitSpecifier.lParameters.at(i).sType));
			pParameterItem->appendRow(new ExperimentTreeItem(VALUE_TAG,cObjectInitFinitSpecifier.lParameters.at(i).sValue));
			pParametersItem->appendRow(pParameterItem);
		}
		pObjectItem->appendRow(pParametersItem);
		tmpNewConnectionItem->appendRow(pObjectItem);
		list1.at(0)->insertRow(list1.at(0)->rowCount(),tmpNewConnectionItem);

		emit modelModified();
		return true;
	}
	return false;
}

bool ExperimentTreeModel::addObjectConnection(const ExperimentTreeModel::strcObjectConnectionSpecifier &cConnectionSpecifier)
{
	QStringList sFilterList;
	int nTempConnectionID;
	QList<int> lFoundConnectionIds;
	bool bAppendReady = false;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	QList<ExperimentTreeItem*> list1 = getFilteredItemList(CONNECTIONS_TAG, sFilterList);
	if(list1.count() != 1)
	{
		return false;
	}
	else
	{
		bAppendReady = true;
	}
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(CONNECTION_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{
			if(tmpItem2->getDefinitions().contains(ID_TAG))
			{
				nTempConnectionID = tmpItem2->getDefinition(ID_TAG).value.toInt();
				if(nTempConnectionID >= 0)
				{
					lFoundConnectionIds.append(nTempConnectionID);
					//if (tmpItem2->hasChildren())
					//{
					//	for (int i=0;i<tmpItem2->rowCount(); i++)
					//	{
					//		if(tmpItem2->child(i)->getName().toLower() == 
				}
			}
		}
	}
	if(bAppendReady)
	{
		ExperimentTreeItem* tmpNewConnectionItem = new ExperimentTreeItem(CONNECTION_TAG);
		int nNewId = 0;
		while(lFoundConnectionIds.contains(nNewId))
			nNewId++;
		tmpNewConnectionItem->addDefinition(ID_TAG,QString::number(nNewId),TreeItemType_Attribute);
		ExperimentTreeItem* pSourceConnItem = new ExperimentTreeItem(CONNECTION_SOURCE_TAG);
		pSourceConnItem->appendRow(new ExperimentTreeItem(OBJECT_TAG,QString::number(cConnectionSpecifier.nSourceObjectID)));
		pSourceConnItem->appendRow(new ExperimentTreeItem(CONNECTION_SIGNATURE_TAG,cConnectionSpecifier.sSourceSignature));
		pSourceConnItem->appendRow(new ExperimentTreeItem(CONNECTION_TYPE_TAG,cMethodStructure::methodTypeToString(cConnectionSpecifier.mSourceMethodType)));
		ExperimentTreeItem* pTargetConnItem = new ExperimentTreeItem(CONNECTION_TARGET_TAG);
		pTargetConnItem->appendRow(new ExperimentTreeItem(OBJECT_TAG,QString::number(cConnectionSpecifier.nTargetObjectID)));
		pTargetConnItem->appendRow(new ExperimentTreeItem(CONNECTION_SIGNATURE_TAG,cConnectionSpecifier.sTargetSignature));
		pTargetConnItem->appendRow(new ExperimentTreeItem(CONNECTION_TYPE_TAG,cMethodStructure::methodTypeToString(cConnectionSpecifier.mTargetMethodType)));
		tmpNewConnectionItem->appendRow(pSourceConnItem);
		tmpNewConnectionItem->appendRow(pTargetConnItem);
		list1.at(0)->insertRow(list1.at(0)->rowCount(),tmpNewConnectionItem);

		emit modelModified();
		return true;
	}
	return false;
}

bool ExperimentTreeModel::removeExperimentObjectInitFinit(const int &nInitFinitId, const bool bIsInit)
{
	ExperimentTreeItem* tmpExpTreeItem;
	QModelIndex tmpModelIndex;
	bool bResult = false;
	if(nInitFinitId >= 0)
	{
		tmpExpTreeItem = getExperimentObjectInitFinitTreeItem(nInitFinitId,bIsInit);
		if(tmpExpTreeItem)
		{
			tmpModelIndex = indexFromItem(tmpExpTreeItem);
			if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
				bResult = true;
			else
				return false;
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::removeObjectConnection(const int &nConnectionId)
{
	ExperimentTreeItem* tmpExpTreeItem;
	QModelIndex tmpModelIndex;
	bool bResult = false;
	if(nConnectionId >= 0)
	{
		tmpExpTreeItem = getExperimentObjectConnectionTreeItem(nConnectionId);
		if(tmpExpTreeItem)
		{
			tmpModelIndex = indexFromItem(tmpExpTreeItem);
			if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
				bResult = true;
			else
				return false;
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::updateObjectConnection(const ExperimentTreeModel::strcObjectConnectionSpecifier &cConnectionSpecifier, const QString &sNewValue)
{
	ExperimentTreeItem* tmpParentExpTreeItem;
	bool bResult = false;
	if(cConnectionSpecifier.nConnectionID >= 0)
	{
		tmpParentExpTreeItem = getExperimentObjectConnectionTreeItem(cConnectionSpecifier.nConnectionID);
		if(tmpParentExpTreeItem)
		{
			if(tmpParentExpTreeItem->hasChildren())
			{
				QStringList lSplittedSpecifier = cConnectionSpecifier.sConnectionDefinitionName.split("/");
				foreach(QString tmpStringSpec, lSplittedSpecifier)
				{
					tmpParentExpTreeItem = tmpParentExpTreeItem->firstChild(tmpStringSpec);
				}
				if(tmpParentExpTreeItem)
				{
					tmpParentExpTreeItem->setValue(sNewValue);
					bResult = true;
				}
			}
		}
		else//We'll add a new parameter value
		{
			saveNewData(cConnectionSpecifier.sConnectionDefinitionName,sNewValue,QModelIndex(),tmpParentExpTreeItem);
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::removeExperimentObject(const int &nObjectId)
{
	ExperimentTreeItem* tmpExpTreeItem;
	QModelIndex tmpModelIndex;
	bool bResult = false;
	if(nObjectId >= 0)
	{
		tmpExpTreeItem = getExperimentObjectTreeItem(nObjectId);
		if(tmpExpTreeItem)
		{
			tmpModelIndex = indexFromItem(tmpExpTreeItem);
			if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
				bResult = true;
			else
				return false;
		}
	}
	if(bResult)
	{
		//int nNumberOfDepsRemoved = 
			cleanupObjectDependencies(nObjectId);
		emit modelModified();
	}
	return bResult;
}

int ExperimentTreeModel::cleanupObjectDependencies(const int &nObjectId)
{
	QStringList lObjectSearchPath;
	QList<ExperimentTreeItem*> lExpTreeObjectItems;
	QMap<QString,TreeItemDefinition> tTmpTreeItemDefs;
	QModelIndex tmpModelIndex;
	int nTmpObjectID;
	int nNrOfObjects;
	ExperimentTreeItem *tmpExpTreeItem;
	int nRemovedDependencies = 0;

	for(int nTreeItemSection = 0;nTreeItemSection<5;nTreeItemSection++)
	{
		if((nTreeItemSection == 0) || (nTreeItemSection == 1))
			lObjectSearchPath = QStringList() << CONNECTIONS_TAG << CONNECTION_TAG;
		else if(nTreeItemSection == 2)
			lObjectSearchPath = QStringList() << INITIALIZATIONS_TAG << INITIALIZATION_TAG << OBJECT_TAG;
		else if(nTreeItemSection == 3)
			lObjectSearchPath = QStringList() << FINALIZATIONS_TAG << FINALIZATION_TAG << OBJECT_TAG;
		else if(nTreeItemSection == 4)
			lObjectSearchPath = QStringList() << BLOCKTRIALS_TAG << BLOCK_TAG << OBJECT_TAG;

		lExpTreeObjectItems.clear();
		nNrOfObjects = ExperimentTreeModel::getStaticTreeElements(lObjectSearchPath, lExpTreeObjectItems, rootItem);
		if(nNrOfObjects > 0)
		{
			for (int j=0;j<nNrOfObjects;j++)//For each Object
			{
				tmpExpTreeItem = NULL;
				if(nTreeItemSection == 0)
				{
					if(lExpTreeObjectItems.at(j)->hasChildren())
						tmpExpTreeItem = lExpTreeObjectItems.at(j)->firstChild(CONNECTION_SOURCE_TAG);
				}
				else if(nTreeItemSection == 1)
				{
					if(lExpTreeObjectItems.at(j)->hasChildren())
						tmpExpTreeItem = lExpTreeObjectItems.at(j)->firstChild(CONNECTION_TARGET_TAG);
				}
				else if((nTreeItemSection == 2) || (nTreeItemSection == 3) || (nTreeItemSection == 4))
				{
					tmpExpTreeItem = lExpTreeObjectItems.at(j);
				}

				if(tmpExpTreeItem)
				{
					tTmpTreeItemDefs = tmpExpTreeItem->getDefinitions();
					if(tTmpTreeItemDefs.contains(ID_TAG))
					{
						nTmpObjectID = tTmpTreeItemDefs[ID_TAG].value.toInt();
						if(nTmpObjectID == nObjectId)
						{
							if(tmpExpTreeItem->parent())
							{
								if(nTreeItemSection == 4)
								{
									tmpModelIndex = indexFromItem(tmpExpTreeItem);
								}
								else
								{
									tmpModelIndex = indexFromItem(tmpExpTreeItem->parent());
								}
								if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
								{
									nRemovedDependencies++;
								}
								else
								{
									qDebug() << __FUNCTION__ << "Something went wrong while deleting the object dependencies (ID =" << nObjectId << ")";
									return -1;
								}
							}
						}
					}
					else if(tmpExpTreeItem->hasChildren() && ((nTreeItemSection == 0)||(nTreeItemSection == 1)))
					{
						ExperimentTreeItem *tmpExpObjectTreeItem = tmpExpTreeItem->firstChild(OBJECT_TAG);
						if(tmpExpObjectTreeItem)
						{
							nTmpObjectID = tmpExpObjectTreeItem->getValue().toInt();
							if(nTmpObjectID == nObjectId)
							{
								if(tmpExpTreeItem->parent())
								{
									tmpModelIndex = indexFromItem(tmpExpTreeItem->parent());
									if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
									{
										nRemovedDependencies++;
									}
									else
									{
										qDebug() << __FUNCTION__ << "Something went wrong while deleting the object dependencies (ID =" << nObjectId << ")";
										return -1;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//emit modelModified(); --> should be performed in calling parent function!
	return nRemovedDependencies;
}

int ExperimentTreeModel::cleanupBlockDependencies(const int &nBlockId)
{
	QStringList lBlockSearchPath;
	QList<ExperimentTreeItem*> lExpTreeBlockItems;
	QMap<QString,TreeItemDefinition> tTmpTreeItemDefs;
	QModelIndex tmpModelIndex;
	int nNrOfBlocks;
	ExperimentTreeItem *tmpExpTreeItem;
	int nRemovedDependencies = 0;
	bool bContinue;
	bool bIsInteger;
	int nTargetBlockID;

	for(int nTreeItemSection = 0;nTreeItemSection<1;nTreeItemSection++)
	{
		if(nTreeItemSection == 0)
			lBlockSearchPath = QStringList() << BLOCKTRIALS_TAG << BLOCK_TAG << LOOPS_TAG << LOOP_TAG;

		lExpTreeBlockItems.clear();
		nNrOfBlocks = ExperimentTreeModel::getStaticTreeElements(lBlockSearchPath, lExpTreeBlockItems, rootItem);
		if(nNrOfBlocks > 0)
		{
			for (int j=0;j<nNrOfBlocks;j++)//For each Object
			{
				tmpExpTreeItem = NULL;
				if(nTreeItemSection == 0)
				{
					if(lExpTreeBlockItems.at(j)->hasChildren())
						tmpExpTreeItem = lExpTreeBlockItems.at(j)->firstChild(LOOP_TARGETBLOCKID_TAG);
				}

				if(tmpExpTreeItem)
				{
					bContinue = false;
					if(nTreeItemSection == 0)
					{
						bIsInteger = false;
						nTargetBlockID = tmpExpTreeItem->getValue().toInt(&bIsInteger);
						if((bIsInteger) && (nTargetBlockID>=0))
						{
							bContinue = true;
						}
					}
					//else
					//{
					//	tTmpTreeItemDefs = tmpExpTreeItem->getDefinitions();
					//	if(tTmpTreeItemDefs.contains(ID_TAG))
					//	{
					//		nTmpBlockID = tTmpTreeItemDefs[ID_TAG].value.toInt();
					//		if(nTmpBlockID == nBlockId)
					//		{
					if(bContinue)
					{
						if(tmpExpTreeItem->parent())
						{
							tmpModelIndex = indexFromItem(tmpExpTreeItem->parent());
							if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
							{
								nRemovedDependencies++;
							}
							else
							{
								qDebug() << __FUNCTION__ << "Something went wrong while deleting the block dependencies (ID =" << nBlockId << ")";
								return -1;
							}
						}
					}
				}
			}
		}
	}
	//emit modelModified(); --> should be performed in calling parent function!
	return nRemovedDependencies;
}

bool ExperimentTreeModel::initializeExperiment()
{
	if(root)
		return true;
	return resetExperiment();
}

bool ExperimentTreeModel::resetExperiment()
{
	QFile tmpFile(":/resources/emptyexperiment.exml");
	if (!tmpFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	QByteArray tmpByteArray = tmpFile.readAll();
	bool bReadResult = read(tmpByteArray);
	tmpFile.close();
	return bReadResult;
}

bool ExperimentTreeModel::addExperimentBlocks(const int &nAmount)
{
	ExperimentTreeItem* tmpExpTreeItem;
	bool bResult = false;
	tmpExpTreeItem = addExperimentBlockTreeItems(nAmount);
	if(tmpExpTreeItem)
		bResult = true;
	return bResult;
}

bool ExperimentTreeModel::changeExperimentBlockLoopNumber(const ExperimentTreeModel::strcBlockLoopSpecifier &cBlockLoopSpecifier, const int &nLoopNumberCorrection)
{
	if((cBlockLoopSpecifier.nBlockID >= 0) && (cBlockLoopSpecifier.nLoopID >= 0))
	{
		QStringList sFilterList;
		int nTempBlockID;
		int nTempLoopID;
		int nTempLoopNumber;
		int nNewLoopNumber = -1;

		sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
		QList<ExperimentTreeItem*> list1 = getFilteredItemList(ACTIONS_TAG, sFilterList);
		foreach (ExperimentTreeItem* tmpItem1 ,list1)
		{
			QList<ExperimentTreeItem*> list2 = getFilteredItemList(BLOCKTRIALS_TAG, sFilterList, tmpItem1);
			foreach (ExperimentTreeItem* tmpItem2 ,list2)
			{
				QList<ExperimentTreeItem*> list3 = getFilteredItemList(BLOCK_TAG, sFilterList, tmpItem2);
				foreach (ExperimentTreeItem* tmpItem3 ,list3)
				{
					if(tmpItem3->getDefinitions().contains(ID_TAG))
					{
						nTempBlockID = tmpItem3->getDefinition(ID_TAG).value.toInt();
						if(nTempBlockID == cBlockLoopSpecifier.nBlockID)//Correct Block ID?
						{	
							QList<ExperimentTreeItem*> list4 = getFilteredItemList(LOOP_TAG, sFilterList, tmpItem3);
							QMap<int, int> mapLoopNumberToLoopID;
							QMap<int, ExperimentTreeItem*> mapLoopNumberToLoopExpTreeItem;
							int nCurrentLoopNumber = -1;
							int nCurrentLoopID = -1;
							int nSuccesfullyParsedLoops = 0;
							foreach (ExperimentTreeItem* tmpItem4 ,list4)
							{
								if(tmpItem4->getDefinitions().contains(ID_TAG))
								{
									nTempLoopID = tmpItem4->getDefinition(ID_TAG).value.toInt();
									ExperimentTreeItem *pTmpChildItem = tmpItem4->firstChild(LOOP_NUMBER_TAG);
									if(pTmpChildItem)
									{
										nTempLoopNumber = pTmpChildItem->getValue().toInt();
										mapLoopNumberToLoopID.insert(nTempLoopNumber,nTempLoopID);
										mapLoopNumberToLoopExpTreeItem.insert(nTempLoopNumber,tmpItem4);
										if(nTempLoopID == cBlockLoopSpecifier.nLoopID)//Correct Object ID?
										{
											nCurrentLoopNumber = nTempLoopNumber;
											nCurrentLoopID = nTempLoopID;
											nNewLoopNumber = nCurrentLoopNumber+nLoopNumberCorrection;
											if((nNewLoopNumber) < 0)
												return false;
										}
										nSuccesfullyParsedLoops++;
									}
								}
							}
							if((nSuccesfullyParsedLoops == 1) && (nCurrentLoopID == cBlockLoopSpecifier.nLoopID))//Only the current loop defined
							{
								if(nCurrentLoopNumber > 0)
								{
									//we need to update/reset the loop counter to zero here...
									if (mapLoopNumberToLoopExpTreeItem.value(nCurrentLoopNumber)->hasChildren())
									{
										for (int i=0;i<mapLoopNumberToLoopExpTreeItem.value(nCurrentLoopNumber)->rowCount(); i++)
										{
											if(mapLoopNumberToLoopExpTreeItem.value(nCurrentLoopNumber)->child(i)->getName().toLower() == LOOP_NUMBER_TAG)
											{
												mapLoopNumberToLoopExpTreeItem.value(nCurrentLoopNumber)->child(i)->setValue("0");
												emit modelModified();
												return true;
											}
										}
									}
								}
								return false;
							}
							else if(nSuccesfullyParsedLoops > 1)
							{
								if(mapLoopNumberToLoopID.contains(nNewLoopNumber))//Do we need to perform a switch?
								{
									ExperimentTreeItem* pLoopOldNumberItem = NULL;
									ExperimentTreeItem* pLoopNewNumberItem = NULL;
									pLoopNewNumberItem = mapLoopNumberToLoopExpTreeItem.value(nCurrentLoopNumber)->firstChild(LOOP_NUMBER_TAG);
									if(pLoopNewNumberItem)
									{
										pLoopOldNumberItem = mapLoopNumberToLoopExpTreeItem.value(nNewLoopNumber)->firstChild(LOOP_NUMBER_TAG);
										if(pLoopOldNumberItem == NULL)
											return false;
										pLoopNewNumberItem->setValue(QString::number(nNewLoopNumber));
										pLoopOldNumberItem->setValue(QString::number(nCurrentLoopNumber));
										emit modelModified();
										return true;
									}
									return false;
								}
							}
							break;
						}
					}
				}
			}
		}
	}
	return false;
}

bool ExperimentTreeModel::removeExperimentBlockLoop(const ExperimentTreeModel::strcBlockLoopSpecifier &cBlockLoopSpecifier)
{
	ExperimentTreeItem* tmpExpTreeItem;
	QModelIndex tmpModelIndex;
	bool bResult = false;
	if((cBlockLoopSpecifier.nBlockID >= 0) && (cBlockLoopSpecifier.nLoopID >= 0))
	{
		tmpExpTreeItem = getExperimentBlockLoopTreeItem(cBlockLoopSpecifier.nBlockID,cBlockLoopSpecifier.nLoopID);
		if(tmpExpTreeItem)
		{
			tmpModelIndex = indexFromItem(tmpExpTreeItem);
			if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
				bResult = true;
			else
				return false;
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::removeExperimentParameters(const QList<ExperimentTreeModel::strcParameterSpecifier> lstParameterSpecifiers)
{
	ExperimentTreeItem* tmpExpTreeItem;
	QModelIndex tmpModelIndex;
	bool bResult = false;
	foreach(ExperimentTreeModel::strcParameterSpecifier sTempParamSpec, lstParameterSpecifiers)
	{
		if(sTempParamSpec.nBlockID >= 0)
		{
			tmpExpTreeItem = getExperimentParameterTreeItem(sTempParamSpec.nBlockID,sTempParamSpec.nObjectID,sTempParamSpec.sParamName.toLower());
			if(tmpExpTreeItem)
			{
				tmpModelIndex = indexFromItem(tmpExpTreeItem);
				if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
					bResult = true;
				else
					return false;
			}
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::renameExperimentParameter(const ExperimentTreeModel::strcParameterSpecifier &cParameterSpecifier, const QString &sNewName)
{
	if(cParameterSpecifier.sParamName.toLower() == sNewName.toLower())
		return false;

	ExperimentTreeItem* tmpExpTreeItem;
	bool bResult = false;
	if(cParameterSpecifier.nBlockID >= 0)
	{
		tmpExpTreeItem = getExperimentParameterTreeItem(cParameterSpecifier.nBlockID,cParameterSpecifier.nObjectID,cParameterSpecifier.sParamName.toLower());
		if(tmpExpTreeItem)
		{
			if (tmpExpTreeItem->hasChildren())
			{
				for (int i=0;i<tmpExpTreeItem->rowCount(); i++)
				{
					if(tmpExpTreeItem->child(i)->getName().toLower() == EXPERIMENT_NAME_TAG)
					{
						tmpExpTreeItem->child(i)->setValue(sNewName);
						bResult = true;
						break;
					}
				}
			}
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::removeExperimentBlocks(const QList<int> &lBlockIDs)
{
	ExperimentTreeItem* tmpExpTreeItem;
	QModelIndex tmpModelIndex;
	bool bResult = false;
	foreach(int nBlockId, lBlockIDs)
	{
		if(nBlockId >= 0)
		{
			tmpExpTreeItem = getExperimentBlockTreeItem(nBlockId);
			if(tmpExpTreeItem)
			{
				tmpModelIndex = indexFromItem(tmpExpTreeItem);
				if(removeRow(tmpModelIndex.row(),tmpModelIndex.parent()))
				{
					cleanupBlockDependencies(nBlockId);
					bResult = true;
				}
				else
				{
					bResult = false;
					break;
				}
			}
		}		
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

ExperimentTreeItem* ExperimentTreeModel::getExperimentBlockLoopTreeItem(const int &nBlockID, const int &nLoopID, const QString &sLoopItemName)
{
	QStringList sFilterList;
	ExperimentTreeItem *tempExpTreeItem;
	int nTempBlockID;
	int nTempLoopID;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	QList<ExperimentTreeItem*> list1 = getFilteredItemList(ACTIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(BLOCKTRIALS_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{
			QList<ExperimentTreeItem*> list3 = getFilteredItemList(BLOCK_TAG, sFilterList, tmpItem2);
			foreach (ExperimentTreeItem* tmpItem3 ,list3)
			{
				if(tmpItem3->getDefinitions().contains(ID_TAG))
				{
					nTempBlockID = tmpItem3->getDefinition(ID_TAG).value.toInt();
					if(nTempBlockID == nBlockID)//Correct Block ID?
					{	
						QList<ExperimentTreeItem*> list4 = getFilteredItemList(LOOP_TAG, sFilterList, tmpItem3);
						foreach (ExperimentTreeItem* tmpItem4 ,list4)
						{
							if(tmpItem4->getDefinitions().contains(ID_TAG))
							{
								nTempLoopID = tmpItem4->getDefinition(ID_TAG).value.toInt();
								if(nTempLoopID == nLoopID)//Correct Object ID?
								{
									if(sLoopItemName.isEmpty())
										return tmpItem4;
									if(tmpItem4->hasChildren())
									{
										tempExpTreeItem = tmpItem4->firstChild(sLoopItemName);
										if(tempExpTreeItem)
											return tempExpTreeItem;
									}
								}
							}
						}
						break;
					}
				}
			}
		}
	}
	return NULL;
}

ExperimentTreeItem* ExperimentTreeModel::getExperimentParameterTreeItem(const int &nBlockID, const int &nObjectID, const QString &sParamName)
{
	QStringList sFilterList;
	ExperimentTreeItem *tempExpTreeItem;
	int nTempBlockID;
	int nTempObjectID;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	QList<ExperimentTreeItem*> list1 = getFilteredItemList(ACTIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(BLOCKTRIALS_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{
			QList<ExperimentTreeItem*> list3 = getFilteredItemList(BLOCK_TAG, sFilterList, tmpItem2);
			foreach (ExperimentTreeItem* tmpItem3 ,list3)
			{
				if(tmpItem3->getDefinitions().contains(ID_TAG))
				{
					nTempBlockID = tmpItem3->getDefinition(ID_TAG).value.toInt();
					if(nTempBlockID == nBlockID)//Correct Block ID?
					{	
						QList<ExperimentTreeItem*> list4 = getFilteredItemList(OBJECT_TAG, sFilterList, tmpItem3);
						foreach (ExperimentTreeItem* tmpItem4 ,list4)
						{
							if(tmpItem4->getDefinitions().contains(ID_TAG))
							{
								nTempObjectID = tmpItem4->getDefinition(ID_TAG).value.toInt();
								if(nTempObjectID == nObjectID)//Correct Object ID?
								{
									QList<ExperimentTreeItem*> list5 = getFilteredItemList(PARAMETER_TAG, sFilterList, tmpItem4);
									foreach (ExperimentTreeItem* tmpItem5 ,list5)
									{
										if(tmpItem5->hasChildren())
										{
											tempExpTreeItem = tmpItem5->firstChild(NAME_TAG);
											if(tempExpTreeItem)
											{
												if(tempExpTreeItem->getValue().toLower() == sParamName)
												{
													return tempExpTreeItem->parent();
												}
											}
										}
									}
									break;
								}
							}
						}
						break;
					}
				}
			}
		}
	}
	return NULL;
}

ExperimentTreeItem* ExperimentTreeModel::getExperimentObjectTreeItem(const int &nObjectID)
{
	QStringList sFilterList;
	int nTempBlockID;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	QList<ExperimentTreeItem*> list1 = getFilteredItemList(DECLARATIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(OBJECT_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{
			if(tmpItem2->getDefinitions().contains(ID_TAG))
			{
				nTempBlockID = tmpItem2->getDefinition(ID_TAG).value.toInt();
				if(nTempBlockID == nObjectID)//Correct Object ID?
				{						
					return tmpItem2;
				}
			}
		}
	}
	return NULL;
}

ExperimentTreeItem* ExperimentTreeModel::getExperimentObjectInitFinitTreeItem(const int &nInitFinitID, const bool &bIsInit)
{
	QStringList sFilterList;
	int nTempInitFinitID;
	QString sTempTag;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	if(bIsInit)
		sTempTag = INITIALIZATIONS_TAG;
	else
		sTempTag = FINALIZATIONS_TAG;

	QList<ExperimentTreeItem*> list1 = getFilteredItemList(sTempTag, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		if(bIsInit)
			sTempTag = INITIALIZATION_TAG;
		else
			sTempTag = FINALIZATION_TAG;
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(sTempTag, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{
			if(tmpItem2->getDefinitions().contains(ID_TAG))
			{
				nTempInitFinitID = tmpItem2->getDefinition(ID_TAG).value.toInt();
				if(nTempInitFinitID == nInitFinitID)//Correct Initialization/Finalization ID?
				{						
					return tmpItem2;
				}
			}
		}
	}
	return NULL;
}

ExperimentTreeItem* ExperimentTreeModel::getExperimentBlockTreeItem(const int &nBlockID)
{
	QStringList sFilterList;
	int nTempBlockID;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	QList<ExperimentTreeItem*> list1 = getFilteredItemList(ACTIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(BLOCKTRIALS_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{
			QList<ExperimentTreeItem*> list3 = getFilteredItemList(BLOCK_TAG, sFilterList, tmpItem2);
			foreach (ExperimentTreeItem* tmpItem3 ,list3)
			{
				if(tmpItem3->getDefinitions().contains(ID_TAG))
				{
					nTempBlockID = tmpItem3->getDefinition(ID_TAG).value.toInt();
					if(nTempBlockID == nBlockID)//Correct Block ID?
					{						
						return tmpItem3;
					}
				}
			}
		}
	}
	return NULL;
}

bool ExperimentTreeModel::moveExperimentBlocks(const QList<int> &lBlockIDsToMove, const int &nBlockIDToSwitch, const int &nBlockNumberChangeDirection)
{
	QStringList sFilterList;
	int nTempBlockID;
	int nBlockNumberToSwitch = -1;
	QList<ExperimentTreeItem*> lstActions;
	QList<ExperimentTreeItem*> lstBlockTrials;
	QList<ExperimentTreeItem*> lstBlocks;
	QMap<int,int> mapNewBlockNumbers;//BlockNumber, BlockID	//QMap items are ordered by key -> BlockNumber!
	QList<int> lstNewOrderedBlockIDList;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	lstActions = getFilteredItemList(ACTIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,lstActions)
	{
		lstBlockTrials = getFilteredItemList(BLOCKTRIALS_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,lstBlockTrials)
		{
			lstBlocks = getFilteredItemList(BLOCK_TAG, sFilterList, tmpItem2);
			foreach (ExperimentTreeItem* tmpItem3 ,lstBlocks)
			{
				if(tmpItem3->getDefinitions().contains(ID_TAG))
				{
					nTempBlockID = tmpItem3->getDefinition(ID_TAG).value.toInt();
					if(nTempBlockID >= 0)
					{
						if (tmpItem3->hasChildren())
						{
							for (int i=0;i<tmpItem3->rowCount();i++)
							{
								if(tmpItem3->child(i)->getName().toLower() == BLOCKNUMBER_TAG)
								{
									int nTempBlockNumber = tmpItem3->child(i)->getValue().toInt();
									mapNewBlockNumbers.insert(tmpItem3->child(i)->getValue().toInt(),nTempBlockID);
									if(nTempBlockID == nBlockIDToSwitch)
										nBlockNumberToSwitch = nTempBlockNumber;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if(mapNewBlockNumbers.isEmpty())
		return false;
	bool bBlockIDsInjected = false;
	foreach(nTempBlockID, mapNewBlockNumbers)
	{
		if(nTempBlockID == nBlockIDToSwitch)
		{
			if(bBlockIDsInjected == false)
			{
				if(nBlockNumberChangeDirection == -1)
				{
					lstNewOrderedBlockIDList.append(lBlockIDsToMove);
					lstNewOrderedBlockIDList.append(nBlockIDToSwitch);
				}
				else if(nBlockNumberChangeDirection == 1)
				{
					lstNewOrderedBlockIDList.append(nBlockIDToSwitch);
					lstNewOrderedBlockIDList.append(lBlockIDsToMove);
				}
				bBlockIDsInjected = true;
			}
		}
		else if(lstNewOrderedBlockIDList.contains(nTempBlockID) == false)
		{
			lstNewOrderedBlockIDList.append(nTempBlockID);
		}
	}
	if(bBlockIDsInjected == false)
		return false;

	bool bRetVal = false;
	int nNewBlockNumberCounter = 0;
	ExperimentTreeItem* tmpExpItem;
	foreach(nTempBlockID, lstNewOrderedBlockIDList)
	{
		tmpExpItem = getExperimentBlockTreeItem(nTempBlockID);
		if (tmpExpItem->hasChildren())
		{
			for (int i=0;i<tmpExpItem->rowCount();i++)
			{
				if(tmpExpItem->child(i)->getName().toLower() == BLOCKNUMBER_TAG)
				{
					tmpExpItem->child(i)->setValue(QString::number(nNewBlockNumberCounter));
					nNewBlockNumberCounter++;
					bRetVal = true;
					break;
				}
			}
		}
	}
	if(bRetVal)
		emit modelModified();
	return bRetVal;
}

bool ExperimentTreeModel::addExperimentBlockLoop(const int &nSourceBlockId, const int &nTargetBlockId, const int &nLoopAmount, const QString &sLoopName)
{
	if((nSourceBlockId < 0) || (nTargetBlockId < 0) || (nLoopAmount < 0))
		return false;

	QStringList sFilterList;
	int nTempBlockID;
	int nHighestFoundLoopNumber = -1;
	int nFoundLoopNumber = -1;
	QList<ExperimentTreeItem*> lstActions;
	QList<ExperimentTreeItem*> lstBlockTrials;
	QList<ExperimentTreeItem*> lstBlocks;
	QList<ExperimentTreeItem*> lstLoopCollections;
	QList<ExperimentTreeItem*> lstLoops;
	QList<int> nReservedLoopIDs;
	bool bTreeReadyForInsert = false;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	lstActions = getFilteredItemList(ACTIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,lstActions)
	{
		lstBlockTrials = getFilteredItemList(BLOCKTRIALS_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,lstBlockTrials)
		{
			lstBlocks = getFilteredItemList(BLOCK_TAG, sFilterList, tmpItem2);
			foreach (ExperimentTreeItem* tmpItem3 ,lstBlocks)
			{
				if(tmpItem3->getDefinitions().contains(ID_TAG))
				{
					nTempBlockID = tmpItem3->getDefinition(ID_TAG).value.toInt();
					if(nTempBlockID == nSourceBlockId)
					{
						lstLoopCollections = getFilteredItemList(LOOPS_TAG, sFilterList, tmpItem3);
						if(lstLoopCollections.isEmpty())
						{
							ExperimentTreeItem* tmpNewItemLoopsCollection = new ExperimentTreeItem(LOOPS_TAG);
							tmpItem3->insertRow(tmpItem3->rowCount(),tmpNewItemLoopsCollection);
							lstLoopCollections = getFilteredItemList(LOOPS_TAG, sFilterList, tmpItem3);
						}
						foreach (ExperimentTreeItem* tmpItem4 ,lstLoopCollections)
						{
							bTreeReadyForInsert = true;
							lstLoops = getFilteredItemList(LOOP_TAG, sFilterList, tmpItem4);
							foreach (ExperimentTreeItem* tmpItem5 ,lstLoops)
							{
								if(tmpItem5->getDefinitions().contains(ID_TAG))
									nReservedLoopIDs.append(tmpItem5->getDefinition(ID_TAG).value.toInt());
								if (tmpItem5->hasChildren())
								{
									for (int i=0;i<tmpItem5->rowCount(); i++)
									{
										if(tmpItem5->child(i)->getName().toLower() == LOOP_NUMBER_TAG)
										{
											nFoundLoopNumber = tmpItem5->child(i)->getValue().toInt();
											if(nFoundLoopNumber > nHighestFoundLoopNumber)//Highest Block Number?
											{
												nHighestFoundLoopNumber = nFoundLoopNumber;
											}
											break;
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
	if(bTreeReadyForInsert==false)
	{
		qDebug() << __FUNCTION__ << "Experiment tree not ready for new loop insertion.";
		return false;
	}
	ExperimentTreeItem* tmpItemLoopCollection = lstLoopCollections.at(0);
	ExperimentTreeItem* tmpNewLoopItem = NULL;
	tmpNewLoopItem = new ExperimentTreeItem(LOOP_TAG);
	int nNewLoopId = 0;
	while(nReservedLoopIDs.contains(nNewLoopId))
		nNewLoopId++;
	tmpNewLoopItem->addDefinition(ID_TAG,QString::number(nNewLoopId),TreeItemType_Attribute);
	tmpNewLoopItem->appendRow(new ExperimentTreeItem(NAME_TAG,sLoopName));
	tmpNewLoopItem->appendRow(new ExperimentTreeItem(LOOP_NUMBER_TAG,QString::number(nHighestFoundLoopNumber+1)));
	tmpNewLoopItem->appendRow(new ExperimentTreeItem(LOOP_AMOUNT_TAG,QString::number(nLoopAmount)));
	tmpNewLoopItem->appendRow(new ExperimentTreeItem(LOOP_TARGETBLOCKID_TAG,QString::number(nTargetBlockId)));
	tmpItemLoopCollection->insertRow(tmpItemLoopCollection->rowCount(),tmpNewLoopItem);
	emit modelModified();
	return true;
}

bool ExperimentTreeModel::updateExperimentObjectInitFinitParameter(const ExperimentTreeModel::strcObjectInitFinitSpecifier &cObjectInitFinitSpecifier, const QString &sNewValue, const bool bIsInit)
{
	ExperimentTreeItem* tmpParentExpTreeItem;
	bool bResult = false;
	if(cObjectInitFinitSpecifier.nInitFinitID >= 0)
	{
		tmpParentExpTreeItem = getExperimentObjectInitFinitTreeItem(cObjectInitFinitSpecifier.nInitFinitID,bIsInit);
		if(tmpParentExpTreeItem)
		{
			if(tmpParentExpTreeItem->hasChildren())
			{
				tmpParentExpTreeItem = tmpParentExpTreeItem->firstChild(OBJECT_TAG);
				if(tmpParentExpTreeItem)
				{
					tmpParentExpTreeItem = tmpParentExpTreeItem->firstChild(PARAMETERS_TAG);
					if(tmpParentExpTreeItem)
					{
						if(tmpParentExpTreeItem->hasChildren())
						{
							for(int nChildCount=0;nChildCount<tmpParentExpTreeItem->childCount();nChildCount++)
							{
								if(tmpParentExpTreeItem->child(nChildCount)->getName().toLower() == PARAMETER_TAG)
								{
									if(tmpParentExpTreeItem->child(nChildCount)->hasChildren())
									{
										ExperimentTreeItem *pTmpExpTreeSubParamItem = tmpParentExpTreeItem->child(nChildCount)->firstChild(NAME_TAG);
										if(pTmpExpTreeSubParamItem)
										{
											if(pTmpExpTreeSubParamItem->getValue() == cObjectInitFinitSpecifier.sObjectDefinitionName)
											{
												pTmpExpTreeSubParamItem = tmpParentExpTreeItem->child(nChildCount)->firstChild(VALUE_TAG);
												if(pTmpExpTreeSubParamItem)
												{
													pTmpExpTreeSubParamItem->setValue(sNewValue);
													bResult = true;
													break;
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
		//else//We'll add a new parameter value
		//{
		//	saveNewData(cObjectInitFinitSpecifier.sObjectDefinitionName,sNewValue,QModelIndex(),tmpParentExpTreeItem);
		//}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::updateExperimentObject(const ExperimentTreeModel::strcObjectSpecifier &cObjectSpecifier, const QString &sNewValue)
{
	ExperimentTreeItem* tmpExpTreeItem;
	ExperimentTreeItem* tmpParentExpTreeItem;
	bool bResult = false;
	if(cObjectSpecifier.nObjectID >= 0)
	{
		tmpParentExpTreeItem = getExperimentObjectTreeItem(cObjectSpecifier.nObjectID);
		if(tmpParentExpTreeItem)
		{
			if(tmpParentExpTreeItem->hasChildren())
			{
				tmpExpTreeItem = tmpParentExpTreeItem->firstChild(cObjectSpecifier.sObjectDefinitionName);
				if(tmpExpTreeItem)
				{
					tmpExpTreeItem->setValue(sNewValue);
					bResult = true;
				}
			}
		}
		else//We'll add a new parameter value
		{
			saveNewData(cObjectSpecifier.sObjectDefinitionName,sNewValue,QModelIndex(),tmpParentExpTreeItem);
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::updateExperimentBlockLoop(const ExperimentTreeModel::strcBlockLoopSpecifier &cBlockLoopSpecifier, const QString &sNewValue)
{
	ExperimentTreeItem* tmpExpTreeItem;
	ExperimentTreeItem* tmpParentExpTreeItem;
	bool bResult = false;
	if(cBlockLoopSpecifier.nBlockID >= 0)
	{
		tmpParentExpTreeItem = getExperimentBlockLoopTreeItem(cBlockLoopSpecifier.nBlockID,cBlockLoopSpecifier.nLoopID);
		if(tmpParentExpTreeItem)
		{
			tmpExpTreeItem = getExperimentBlockLoopTreeItem(cBlockLoopSpecifier.nBlockID,cBlockLoopSpecifier.nLoopID,cBlockLoopSpecifier.sLoopDefinitionName);
			if(tmpExpTreeItem)//We need to do a update
			{
				tmpExpTreeItem->setValue(sNewValue);
				bResult = true;
			}
			else//We'll add a new parameter value
			{
				saveNewData(cBlockLoopSpecifier.sLoopDefinitionName,sNewValue,QModelIndex(),tmpParentExpTreeItem);
			}
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

bool ExperimentTreeModel::addUpdateExperimentBlockParameter(const ExperimentTreeModel::strcParameterSpecifier &cParameterSpecifier, const QString &sNewValue, const bool &bIsCustom)
{
	ExperimentTreeItem* tmpExpTreeItem;
	//QModelIndex tmpModelIndex;
	bool bResult = false;
	if(cParameterSpecifier.nBlockID >= 0)
	{
		tmpExpTreeItem = getExperimentParameterTreeItem(cParameterSpecifier.nBlockID,cParameterSpecifier.nObjectID,cParameterSpecifier.sParamName.toLower());
		if(tmpExpTreeItem)//We need to do a update
		{
			//tmpModelIndex = indexFromItem(tmpExpTreeItem);
			if (tmpExpTreeItem->hasChildren())
			{
				for (int i=0;i<tmpExpTreeItem->rowCount(); i++)
				{
					if(tmpExpTreeItem->child(i)->getName().toLower() == EXPERIMENT_VALUE_TAG)
					{
						tmpExpTreeItem->child(i)->setValue(sNewValue);
						bResult = true;
						break;
					}
				}
			}
		}
		else//We'll add a new parameter value
		{
			if(saveNewData(cParameterSpecifier.nBlockID,cParameterSpecifier.nObjectID,cParameterSpecifier.sParamName, sNewValue, bIsCustom))
				bResult = true;
		}
	}
	if(bResult)
		emit modelModified();
	return bResult;
}

ExperimentTreeItem* ExperimentTreeModel::getExperimentObjectConnectionTreeItem(const int &nConnectionID)
{
	QStringList sFilterList;
	int nTempConnectionID;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	QList<ExperimentTreeItem*> list1 = getFilteredItemList(CONNECTIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,list1)
	{
		QList<ExperimentTreeItem*> list2 = getFilteredItemList(CONNECTION_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,list2)
		{ 
			if(tmpItem2->getDefinitions().contains(ID_TAG))
			{
				nTempConnectionID = tmpItem2->getDefinition(ID_TAG).value.toInt();
				if(nTempConnectionID == nConnectionID)//Correct Connection ID?
				{						
					return tmpItem2;
				}
			}
		}
	}
	return NULL;
}

ExperimentTreeItem* ExperimentTreeModel::addExperimentBlockTreeItems(const int &nAmount)
{
	if(nAmount < 1)
		return NULL;

	QStringList sFilterList;
	int nTempBlockID;
	int nLatestFoundBlockID = -1;
	int nHighestFoundBlockNumber = -1;
	int nFoundBlockNumber = -1;
	QList<ExperimentTreeItem*> lstActions;
	QList<ExperimentTreeItem*> lstBlockTrials;
	QList<ExperimentTreeItem*> lstBlocks;

	sFilterList << EXPERIMENTTREEMODEL_FILTER_TAGS;
	lstActions = getFilteredItemList(ACTIONS_TAG, sFilterList);
	foreach (ExperimentTreeItem* tmpItem1 ,lstActions)
	{
		lstBlockTrials = getFilteredItemList(BLOCKTRIALS_TAG, sFilterList, tmpItem1);
		foreach (ExperimentTreeItem* tmpItem2 ,lstBlockTrials)
		{
			lstBlocks = getFilteredItemList(BLOCK_TAG, sFilterList, tmpItem2);
			foreach (ExperimentTreeItem* tmpItem3 ,lstBlocks)
			{
				if(tmpItem3->getDefinitions().contains(ID_TAG))
				{
					nTempBlockID = tmpItem3->getDefinition(ID_TAG).value.toInt();
					if(nTempBlockID >= 0)
					{
						if (tmpItem3->hasChildren())
						{
							for (int i=0;i<tmpItem3->rowCount(); i++)
							{
								if(tmpItem3->child(i)->getName().toLower() == BLOCKNUMBER_TAG)
								{
									nFoundBlockNumber = tmpItem3->child(i)->getValue().toInt();
									if(nFoundBlockNumber > nHighestFoundBlockNumber)//Highest Block Number?
									{
										nHighestFoundBlockNumber = nFoundBlockNumber;
										nLatestFoundBlockID = nTempBlockID;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	if(lstBlockTrials.count() != 1)
		return NULL;
	ExperimentTreeItem* tmpItemBlockTrials = lstBlockTrials.at(0);
	ExperimentTreeItem* tmpNewBlockItem = NULL;

	for (int nNewBlockCounter=0;nNewBlockCounter<nAmount;nNewBlockCounter++)
	{
		tmpNewBlockItem = new ExperimentTreeItem(BLOCK_TAG);
		tmpNewBlockItem->addDefinition(ID_TAG,QString::number(nLatestFoundBlockID+1+nNewBlockCounter),TreeItemType_Attribute);
		tmpNewBlockItem->appendRow(new ExperimentTreeItem(NAME_TAG,"_noname_"));
		tmpNewBlockItem->appendRow(new ExperimentTreeItem(BLOCKNUMBER_TAG,QString::number(nHighestFoundBlockNumber+1+nNewBlockCounter)));
		tmpNewBlockItem->appendRow(new ExperimentTreeItem(TRIALAMOUNT_TAG,"1"));
		tmpNewBlockItem->appendRow(new ExperimentTreeItem(INTERNALTRIGGERAMOUNT_TAG,"1"));
		tmpNewBlockItem->appendRow(new ExperimentTreeItem(EXTERNALTRIGGERAMOUNT_TAG,"1"));
		tmpItemBlockTrials->insertRow(tmpItemBlockTrials->rowCount(),tmpNewBlockItem);
	}
	emit modelModified();
	return tmpNewBlockItem;
}

void ExperimentTreeModel::saveNewData(const QString &sName, const QString &sValue, const QModelIndex &parentIndex, ExperimentTreeItem *pParametersSection)
{
	ExperimentTreeItem *m_parent;
	if(pParametersSection == NULL)
		m_parent = itemFromIndex(parentIndex);
	else
		m_parent = pParametersSection;

	bool bModelChanged = false;

	if ((m_parent != NULL) && (sName.isEmpty() == false))
	{
		int totalChilds;
		QString sTempString1;
		QString sTempString2;
		int nValueIndex;
		int nHighestIDNumber;
		bool bParamFound;
		bool bDoBreak = false;
		TreeItemDefinition tmpTreeItemDef;
		QMap<QString,TreeItemDefinition> m_definitions;
		QStringList sRelativeNames = sName.split("/");

		while(bDoBreak == false)
		{
			totalChilds = m_parent->childCount();
			nHighestIDNumber = -1;
			bParamFound = false;
			bDoBreak = false;
			if(totalChilds == 0)
				bDoBreak = true;
			for (int j=0;j<totalChilds;j++)
			{
				if((m_parent->getName() == EXPERIMENT_PARAMETERS_TAG) || (m_parent->getName() == EXPERIMENT_CUSTOMPARAMETERS_TAG))
				{
					if(m_parent->child(j)->getName() == EXPERIMENT_PARAMETER_TAG)
					{
						if(m_parent->child(j)->hasChildren())
						{
							m_definitions = m_parent->child(j)->getDefinitions();
							QMapIterator<QString, TreeItemDefinition> it(m_definitions);
							while (it.hasNext()) 
							{
								it.next();
								if(it.key().toLower() == EXPERIMENT_ID_TAG)
								{
									if(it.value().value.toInt() > nHighestIDNumber)
									{
										nHighestIDNumber = it.value().value.toInt();
									}
									break;
								}
							}
							sTempString1 = ""; 
							sTempString2 = "";
							nValueIndex = -1;
							for(int i=0;i<m_parent->child(j)->childCount();i++)
							{
								if(m_parent->child(j)->child(i)->getName() == EXPERIMENT_NAME_TAG)
								{
									if (sRelativeNames[0].compare(m_parent->child(j)->child(i)->getValue(), Qt::CaseInsensitive) == 0)
									{
										sTempString1 = m_parent->child(j)->child(i)->getValue();
									}
									else
									{
										continue;	
									}							
								}
								else if(m_parent->child(j)->child(i)->getName() == EXPERIMENT_VALUE_TAG)
								{
									sTempString2 = m_parent->child(j)->child(i)->getValue();
									nValueIndex = i;
								}
								if((nValueIndex != -1) && (sTempString1.isEmpty() == false))
								{
									if (sRelativeNames[0].compare(sTempString1, Qt::CaseInsensitive) == 0)
									{
										m_parent->child(j)->child(nValueIndex)->setValue(sValue);
										bModelChanged = true;
										//ExperimentTreeItem *item = new ExperimentTreeItem(m_parent->child(j)->child(nValueIndex));
										//m_parent->child(j)->removeRow(nValueIndex);
										//m_parent->child(j)->insertRow(nValueIndex, item);								
										sTempString1 = ""; 
										sTempString2 = "";
										nValueIndex = -1;
										bParamFound = true;
										//We break because then only the first parameter would be changed. 
										//In a non-unique situation this would not be compatible, so you need to comment the below 2 lines.
										bDoBreak = true;
										break;
									}
								}
							}					
						}
					}
					else
					{
						if(j == (totalChilds-1))//last one?
						{
							bDoBreak = true;
						}
						continue;
					}
					if(bDoBreak)
						break;
				}
				else
				{
					if (sRelativeNames[0].compare(m_parent->child(j)->getName(), Qt::CaseInsensitive) == 0)
					{
						if(sRelativeNames.count() > 1)
						{
							m_parent = m_parent->child(j);
							sRelativeNames.removeFirst();
							bDoBreak = false;
							break;							
						}
						else
						{
							m_parent->child(j)->setValue(sValue);
							bModelChanged = true;
							bParamFound = true;
							bDoBreak = true;
							break;
						}
					}
					else
					{
						if(j == (totalChilds-1))//last one?
						{
							bDoBreak = true;
						}
						continue;	
					}
					if(bDoBreak)
						break;
				}
				if(j == (totalChilds-1))//last one?
				{
					bDoBreak = true;
				}
			}
		}		
		if(bParamFound == false)
		{
			if((m_parent->getName() == EXPERIMENT_PARAMETERS_TAG) || (m_parent->getName() == EXPERIMENT_CUSTOMPARAMETERS_TAG))
			{
				//we need to add this new data to the tree model
				ExperimentTreeItem *item = new ExperimentTreeItem(EXPERIMENT_PARAMETER_TAG, "");
				m_parent->appendRow(item);			
				TreeItemDefinition tmpTreeItemDefinition;
				tmpTreeItemDefinition.type = TreeItemType_Attribute;
				tmpTreeItemDefinition.value = QString::number(nHighestIDNumber + 1);
				QMap<QString,TreeItemDefinition> m_definitions = item->getDefinitions();
				m_definitions.insert(QString(EXPERIMENT_ID_TAG).toUpper(), tmpTreeItemDefinition);
				item->setDefinitions(m_definitions);
				//Append the name
				ExperimentTreeItem *subNameItem = new ExperimentTreeItem(EXPERIMENT_NAME_TAG, sRelativeNames[0]);
				item->appendRow(subNameItem);			
				//Append the value
				subNameItem = new ExperimentTreeItem(EXPERIMENT_VALUE_TAG, sValue);
				item->appendRow(subNameItem);
				bModelChanged = true;
			}
		}
	}
	if(bModelChanged)
		emit modelModified();
}

void ExperimentTreeModel::saveNewData(QWidget *widgetContainer, const QModelIndex &parentIndex)
{
    ExperimentTreeItem *m_parent = itemFromIndex(parentIndex);
	//int nRow = parentIndex.row();
	//int nCol = parentIndex.column();
	if (m_parent != NULL)
	{
		int totalChilds = m_parent->childCount();
		QGridLayout *gridLayout = dynamic_cast<QGridLayout*>(widgetContainer->layout());
		bool bModelChanged = false;

		if(gridLayout == NULL)
		{
			//ExperimentParameterVisualizer *paramVisualizer = dynamic_cast<ExperimentParameterVisualizer*>(widgetContainer->layout());
		}
		else
		{
			for (int i = 0; i < gridLayout->rowCount(); i++)
			{
				QWidgetItem *widgetItem = dynamic_cast<QWidgetItem*>(gridLayout->itemAtPosition(i,3));
				if (widgetItem)
				{
					QLabel *label = dynamic_cast<QLabel*>(widgetItem->widget());
					QWidgetItem *widgetItem2 = dynamic_cast<QWidgetItem*>(gridLayout->itemAtPosition(i,1));

					if (label && widgetItem2)
					{
						for (int j = 0; j < totalChilds; j++)
						{
							if (label->text() == m_parent->child(j)->getUID())
							{
								QLineEdit *lineEdit = qobject_cast<QLineEdit*>(widgetItem2->widget());
								QComboBox *comboBox = qobject_cast<QComboBox*>(widgetItem2->widget());
								if(lineEdit)
								{
									m_parent->child(j)->setValue(lineEdit->text());
								}
								else if(comboBox)
								{
									QString valAux = "";
									if (comboBox->currentText() == "TRUE")
										valAux = "1";
									else if (comboBox->currentText() == "FALSE")
										valAux = "0";
									m_parent->child(j)->setValue(valAux);
								}
								else
								{
									ExperimentTreeItem *item = new ExperimentTreeItem(m_parent->child(j));
									m_parent->removeRow(j);
									m_parent->insertRow(j, item);
								}
								bModelChanged = true;
							}
						}
					}
				}
			}
		}
		if (bModelChanged)
		{
			emit modelModified();
		}
	}
}

bool ExperimentTreeModel::write(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
		xmlWriter.writeDTD(DEFAULT_DOCUMENTTYPEDEFINITION);
		xmlWriter.writeComment("\nThis file was automatically generated by BrainStim.\n");

        recursiveWrite(xmlWriter, rootItem);

        xmlWriter.writeEndDocument();
        file.close();
		return true;
    }
	return false;
}

void ExperimentTreeModel::recursiveWrite(QXmlStreamWriter &xml, ExperimentTreeItem *item)
{
    if (item->hasChildren())
    {		
        xml.writeStartElement(item->getName());
		//tmpString = item->getName();
        QList<QString> keyDefinitions = item->getDefinitions().keys();
        for (int i = 0; i < keyDefinitions.count(); i++)
		{
			//tmpString = keyDefinitions.at(i);
			if(item->getDefinition(keyDefinitions.at(i)).type == TreeItemType_Attribute)
				xml.writeAttribute(keyDefinitions.at(i), item->getDefinition(keyDefinitions.at(i)).value.toString());
			else if(item->getDefinition(keyDefinitions.at(i)).type == TreeItemType_Comment)
				xml.writeComment(item->getDefinition(keyDefinitions.at(i)).value.toString());
		}
        for (int i = 0; i < item->childCount(); i++)
		{
            recursiveWrite(xml, item->child(i));
		}
		xml.writeEndElement();
    }
    else
    {
        if (item->getValue() == "")
        {
			//tmpString = item->getName();
			if(item->getName() != DEFINITION_TAG_COMMENT)
				xml.writeEmptyElement(item->getName());
            QList<QString> keyDefinitions = item->getDefinitions().keys();
            for (int i = 0; i < keyDefinitions.count(); i++)
			{
				//tmpString = keyDefinitions.at(i);
				if(item->getDefinition(keyDefinitions.at(i)).type == TreeItemType_Attribute)
					xml.writeAttribute(keyDefinitions.at(i), item->getDefinition(keyDefinitions.at(i)).value.toString());
				else if(item->getDefinition(keyDefinitions.at(i)).type == TreeItemType_Comment)
					xml.writeComment(item->getDefinition(keyDefinitions.at(i)).value.toString());
			}
        }
        else
		{
			//tmpString = item->getName();
            xml.writeTextElement(item->getName(), item->getValue());
		}
    }
}

QList<ExperimentTreeItem*> ExperimentTreeModel::getFilteredItemList(const QString &textToFind, const QStringList &filters, ExperimentTreeItem *expTreeItem)
{
    QList<ExperimentTreeItem*> list;
	if(expTreeItem == NULL)
		expTreeItem = rootItem;
	if (expTreeItem)
	{
		recursiveSearch(textToFind, filters, expTreeItem, list);
		return list;
	}
	return QList<ExperimentTreeItem*>();
}

void ExperimentTreeModel::recursiveSearch(const QString &textToFind, const QStringList &filters, ExperimentTreeItem *item, QList<ExperimentTreeItem*> &list)
{
    QString itName = item->getName();
    QString itValue = item->getValue();
    QString text = textToFind;

    if (!filters.contains(EXPERIMENTTREEMODEL_FILTER_CASE_SENSITIVE))
    {
        text = textToFind.toLower();
        itName = itName.toLower();
        itValue = itValue.toLower();
    }

    if ((itName == text && filters.contains(EXPERIMENTTREEMODEL_FILTER_TAGS)) ||
        (itValue == text && filters.contains(EXPERIMENTTREEMODEL_FILTER_VALUES)) ||
        (item->getDefinition(textToFind).value.toString().size() > 0 && filters.contains(EXPERIMENTTREEMODEL_FILTER_ATTRIBUTES)))
    {
        list.append(item);
    }
    if (item->hasChildren())
        for (int i = 0; i < item->rowCount(); i++)
            recursiveSearch(textToFind, filters, item->child(i), list);
}

QModelIndex ExperimentTreeModel::getIndexByUID(const QString &uuid)
{
    QModelIndex index;
    bool found = false;
    recursiveUidSearch(uuid, rootItem, found, index);
    return index;
}

void ExperimentTreeModel::recursiveUidSearch(const QString &uuid, ExperimentTreeItem *item, bool found, QModelIndex &index)
{
    if (item->getUID() == uuid)
    {
        index = indexFromItem(item->parent());
        found = true;
    }
    else if (item->hasChildren())
        for (int i = 0; i < item->rowCount() && !found; i++)
            recursiveUidSearch(uuid, item->child(i), found, index);
}

ExperimentTreeItem *ExperimentTreeModel::itemFromIndex(const QModelIndex &index) const
{
    return dynamic_cast<ExperimentTreeItem*>(QStandardItemModel::itemFromIndex(index));
}

int ExperimentTreeModel::getTreeElements(const QStringList &sElementTagName, QList<ExperimentTreeItem *> &lFoundTreeItems, ExperimentTreeItem *pSearchRootItem)
{
	if(doc == NULL)
		return -1;

	if(pSearchRootItem)
		return getStaticTreeElements(sElementTagName,lFoundTreeItems,pSearchRootItem);
	else
		return getStaticTreeElements(sElementTagName,lFoundTreeItems,rootItem);
}

int ExperimentTreeModel::getStaticTreeElements(const QStringList &sElementTagName, QList<ExperimentTreeItem *> &lFoundTreeItems, ExperimentTreeItem *pSearchRootItem)
{
	if(pSearchRootItem == NULL)
	{
		return -1;
	}
	int nDepth = sElementTagName.count();
	if(nDepth<=0)
		return 0;

	QStringList sTreeFilter;
	sTreeFilter << EXPERIMENTTREEMODEL_FILTER_TAGS;
	QList<ExperimentTreeItem *> currentStartSearchItems;
	currentStartSearchItems << pSearchRootItem;

	for (int i=0;i<nDepth;i++)
	{
		recursiveMultiSearch(sElementTagName.at(i), sTreeFilter, currentStartSearchItems, lFoundTreeItems);
		if(i==(nDepth-1))
			return lFoundTreeItems.count();

		if(lFoundTreeItems.isEmpty() == false)
		{
			currentStartSearchItems = lFoundTreeItems;
			lFoundTreeItems.clear();
		}
		else
		{
			return 0;
		}
	}	
	return 0;
}

QList<ExperimentStructuresNameSpace::strcExperimentObject> ExperimentTreeModel::getDefinedExperimentObjectInfoList(ExperimentTreeItem *objItem)
{
	//If objItem is NULL then all the objects are returned!
	QList<ExperimentStructuresNameSpace::strcExperimentObject> tmpExperimentObjectList;
	int nSearchObjectID = -1;
	if(objItem)
	{
		QMap<QString, TreeItemDefinition> mapTreeItemDefinition = objItem->getDefinitions();
		if(mapTreeItemDefinition.contains(ID_TAG))
		{
			nSearchObjectID = mapTreeItemDefinition.value(ID_TAG).value.toInt();
		}
		else
		{
			return tmpExperimentObjectList;
		}
	}
	ExperimentStructuresNameSpace::strcExperimentObject tmpExperimentObject;
	QList<ExperimentTreeItem*> tmpExpTreeItemList;
	QStringList strList;
	strList.clear();
	strList.append(ROOT_TAG);
	strList.append(DECLARATIONS_TAG); 
	strList.append(OBJECT_TAG);
	if (this->getTreeElements(strList,tmpExpTreeItemList) >= 0)
	{
		int nNrOfObjects = tmpExpTreeItemList.count();
		if (nNrOfObjects>0)
		{
			ExperimentTreeItem *pExpTreeItem;
			QMap<QString, TreeItemDefinition> tTmpTreeItemDefs;
			QString tmpString;
			int nObjectID = -1;
			for(int i=0;i<nNrOfObjects;i++)
			{
				pExpTreeItem = tmpExpTreeItemList.at(i);
				if (pExpTreeItem) 
				{
					tTmpTreeItemDefs = pExpTreeItem->getDefinitions();
					if(!tTmpTreeItemDefs.contains(ID_TAG))
						break;
					tmpString = tTmpTreeItemDefs[ID_TAG].value.toString();//Correct ObjectID?
					if (tmpString.isEmpty() == false)
					{
						nObjectID = tmpString.toInt();
						if((nObjectID == nSearchObjectID) || (objItem == NULL))
						{
							tmpExperimentObject.nID = nObjectID;
							pExpTreeItem = tmpExpTreeItemList.at(i)->firstChild(NAME_TAG);
							//if(tmpElement.tagName() == NAME_TAG)
							if(pExpTreeItem)
								tmpExperimentObject.sName = pExpTreeItem->getValue();
							pExpTreeItem = tmpExpTreeItemList.at(i)->firstChild(CLASS_TAG);
							//if(tmpElement.tagName() == CLASS_TAG)
							if(pExpTreeItem)
								tmpExperimentObject.sClass =pExpTreeItem->getValue();

							pExpTreeItem = tmpExpTreeItemList.at(i)->firstChild(DEFINITIONS_TAG);
							if(pExpTreeItem)
							{
								QList<ExperimentTreeItem*> lTmpDefsTreeItemList;
								int nNumberOfDefs = getTreeElements(QStringList() << DEFINITION_TAG, lTmpDefsTreeItemList,pExpTreeItem);
								if(nNumberOfDefs>0)
								{
									for (int nNumberOfDefsCntr=0;nNumberOfDefsCntr<nNumberOfDefs;nNumberOfDefsCntr++)
									{
										pExpTreeItem = lTmpDefsTreeItemList.at(i)->firstChild(FILE_TAG);
										if(pExpTreeItem)
										{
											ExperimentStructuresNameSpace::strcObjectDefinition tmpObjectDef;
											tmpObjectDef.sDefinitionFile = pExpTreeItem->getValue();
											tmpExperimentObject.lObjectDefinitions.append(tmpObjectDef);
										}
									}									
								}
							}
							tmpExperimentObjectList.append(tmpExperimentObject);
						}
					}
				}
			}
		}
	}
	return tmpExperimentObjectList;
}

void ExperimentTreeModel::recursiveMultiSearch(const QString &textToFind, const QStringList &filters, QList<ExperimentTreeItem *> items, QList<ExperimentTreeItem*> &list)
{
	if(items.isEmpty())
		return;
	QList<ExperimentTreeItem *> resultingItemList;
	for (int i = 0;i<items.count(); i++)
	{
		recursiveSearch(textToFind,filters,items.at(i),list);
	}
}

bool ExperimentTreeModel::reset()
{
	if(rootItem)
	{
		delete rootItem;
		rootItem = NULL;
	}
	if(root)
	{
		delete root;
		root = NULL;
	}
	if(doc)
	{
		delete doc;
		doc = NULL;
	}
	return true;
}

bool ExperimentTreeModel::read(QByteArray &byteArrayContent)
{
	QString errorStr;
	int errorLine;
	int errorColumn;

	if(root)
	{
		delete root;
		root = NULL;
	}
	if(doc)
	{
		delete doc;
		doc = NULL;
	}
	doc = new QDomDocument();
	//Perform a global XML standard parse test
	if (!doc->setContent(byteArrayContent, true, &errorStr, &errorLine, &errorColumn))
	{
		//QMessageBox::information(window(), tr(MODULE_NAME),
		//	tr("Parse error at line %1, column %2:\n%3")
		//	.arg(errorLine)
		//	.arg(errorColumn)
		//	.arg(errorStr));
		qDebug() << tr("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr);
		return false;
	}
	//First we search for the Root Tag
	root = new QDomElement(doc->documentElement());
	rootItem = new ExperimentTreeItem(root->tagName());//root.tagName() = EXML
	if (root->tagName() != ROOT_TAG) 
	{
		//QMessageBox::information(window(), tr(MODULE_NAME), tr("The file is not an EXML file."));
		qDebug() << tr("The file is not an EXML file.");
		return false;
	} 
	else//Found!
	{
		if (root->hasAttribute(VERSION_TAG))//Check whether we have a version tag defined
		{	
			QString strCurrentVersion = root->attribute(VERSION_TAG);
			QString strMinimalVersion = QString::number(nXMLCurrentClassVersion.major) + "." + QString::number(nXMLCurrentClassVersion.minor) + "." + QString::number(nXMLCurrentClassVersion.version) + "." + QString::number(nXMLCurrentClassVersion.build);
			QStringList lVersion = strCurrentVersion.split(".");
			nXMLDocumentVersion.major = lVersion[0].toInt();
			nXMLDocumentVersion.minor = lVersion[1].toInt();
			nXMLDocumentVersion.version = lVersion[2].toInt();
			nXMLDocumentVersion.build = lVersion[3].toInt();
						
			//Is the documents version compatible with this class?
			if(!MainAppInfo::isCompatibleVersion(strMinimalVersion,strCurrentVersion))
			{
				//QMessageBox::information(window(), tr(MODULE_NAME), tr("The defined EXML file is not compatible from EXML version %1.%2.%3.%4")
				//	.arg(nXMLCurrentClassVersion.major)
				//	.arg(nXMLCurrentClassVersion.minor)
				//	.arg(nXMLCurrentClassVersion.version)
				//	.arg(nXMLCurrentClassVersion.build));
				qDebug() << tr("The defined EXML file is not compatible from EXML version %1.%2.%3.%4").arg(nXMLCurrentClassVersion.major).arg(nXMLCurrentClassVersion.minor).arg(nXMLCurrentClassVersion.version).arg(nXMLCurrentClassVersion.build);
				return false;
			}
		}
		else
		{
			//QMessageBox::information(window(), tr(MODULE_NAME), tr("The EXML file version could not be determined."));
			qDebug() << tr("The EXML file version could not be determined.");
			return false;
		}
	}
	return fillModel();
}
