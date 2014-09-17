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

#include "ExperimentTreeItem.h"

ExperimentTreeItem::ExperimentTreeItem(const QString &name, const QString &value) : QStandardItem(name)
{
    m_value = value;
    uid = QUuid::createUuid();
}

ExperimentTreeItem::ExperimentTreeItem(const ExperimentTreeItem &item) : QStandardItem(item.getName())
{
    m_value = item.m_value;
    m_storeValue = item.m_storeValue;
    m_definitions = item.m_definitions;
    uid = QUuid::createUuid();
}

ExperimentTreeItem::ExperimentTreeItem(ExperimentTreeItem *item) : QStandardItem(item->getName())
{
    m_value = item->m_value;
    m_storeValue = item->m_storeValue;
    m_definitions = item->m_definitions;
    uid = QUuid::createUuid();
}

QVariant ExperimentTreeItem::getType() const
{
	if (m_definitions.contains("TYPE"))
		return m_definitions.value("TYPE").value;
	else if (m_definitions.contains("Type"))
		return m_definitions.value("Type").value;
	else if (m_definitions.contains("type"))
		return m_definitions.value("type").value;
	else
        return QVariant();
}

QVariant ExperimentTreeItem::getDescription() const
{
    if (m_definitions.contains("Description"))
        return m_definitions.value("Description").value;
    else if (m_definitions.contains("DESCRIPTION"))
        return m_definitions.value("DESCRIPTION").value;
    else if (m_definitions.contains("description"))
        return m_definitions.value("description").value;
    else
        return QVariant();
}

QVariant ExperimentTreeItem::getDefaultValue() const
{
    if (m_definitions.contains("Default"))
        return m_definitions.value("Default").value;
    else if (m_definitions.contains("DEFAULT"))
        return m_definitions.value("DEFAULT").value;
    else if (m_definitions.contains("default"))
        return m_definitions.value("default").value;
    else
        return QVariant();
}

QStringList ExperimentTreeItem::getAvailableValuesList() const
{
    if (m_definitions.contains("LIST"))
        return m_definitions.value("LIST").value.toString().split(";");
    else if (m_definitions.contains("List"))
        return m_definitions.value("List").value.toString().split(";");
    else if (m_definitions.contains("list"))
        return m_definitions.value("list").value.toString().split(";");
    else
        return QStringList();
}

void ExperimentTreeItem::addDefinition(const QString &defName, const QVariant &defValue, TreeItemType defType) 
{
	TreeItemDefinition tmpTreeItemDef;
	tmpTreeItemDef.type = defType;
	tmpTreeItemDef.value = defValue;
	m_definitions.insert(defName,tmpTreeItemDef); 
}

void ExperimentTreeItem::setDefinitions(const QMap<QString,TreeItemDefinition> &definitions) 
{ 
	m_definitions = definitions; 
}

TreeItemDefinition ExperimentTreeItem::getDefinition(const QString &defName) const 
{ 
	return m_definitions.value(defName); 
}

QMap<QString,TreeItemDefinition> ExperimentTreeItem::getDefinitions() const 
{ 
	return m_definitions; 
}

int ExperimentTreeItem::getDefinitionsCount() 
{ 
	return m_definitions.size(); 
}

ExperimentTreeItem *ExperimentTreeItem::firstChild(const QString &sName)
{
	if(rowCount() <= 0)
		return NULL;
	int nRowCount = QStandardItem::rowCount();
	for(int i=0;i<nRowCount;i++)
	{
		if(QStandardItem::child(i,0)->text().compare(sName,Qt::CaseInsensitive) == 0)
		{
			return dynamic_cast<ExperimentTreeItem*>(QStandardItem::child(i,0));
		}
	}
	return NULL;
}

ExperimentTreeItem *ExperimentTreeItem::nextSiblingTreeItem()
{
	int nRequestedRow = QStandardItem::row() + 1;
	if(QStandardItem::parent()->rowCount() > nRequestedRow)
	{
		return dynamic_cast<ExperimentTreeItem*>(QStandardItem::parent()->child(nRequestedRow,0));
	}
	return NULL;
}
