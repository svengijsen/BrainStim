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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QUuid>
#include <QTime>
#include <QCryptographicHash>
#include <QStandardItem>
#include "ExperimentGraphicEditor_Global.h"

class ExperimentTreeItem : public QStandardItem
{
	public:
        ExperimentTreeItem(const QString &name, const QString &value = "");
		ExperimentTreeItem(const ExperimentTreeItem &item);
		ExperimentTreeItem(ExperimentTreeItem *item);

        QString getUID() const { return uid.toString(); }
        QString getName() const { return QStandardItem::text(); }
        void setValue(const QString &value) { m_value = value; }
        QString getValue() const { return m_value; }
        void setStoreValue(bool store) { m_storeValue = store; }
        bool canStoreValue() const { return m_storeValue; }

        void addDefinition(const QString &defName, const QVariant &defValue, TreeItemType defType);
        void setDefinitions(const QMap<QString,TreeItemDefinition> &definitions);
        TreeItemDefinition getDefinition(const QString &defName) const;
        QMap<QString,TreeItemDefinition> getDefinitions() const;
        int getDefinitionsCount();
        //bool existDefinition(const QString &defName) const;

		QStringList getAvailableValuesList() const;
        QVariant getType() const;
        QVariant getDescription() const;
        QVariant getDefaultValue() const;
		ExperimentTreeItem *child(int row) const { return dynamic_cast<ExperimentTreeItem*>(QStandardItem::child(row,0)); }
		ExperimentTreeItem *firstChild(const QString &sName);
        int childCount() const { return rowCount(); }
        ExperimentTreeItem *parent() const { return dynamic_cast<ExperimentTreeItem*>(QStandardItem::parent()); }
		ExperimentTreeItem *nextSiblingTreeItem();
        int row() const { return QStandardItem::row(); }

    private:
        QUuid uid;
        QString m_value;
        bool m_storeValue;
        QMap<QString,TreeItemDefinition> m_definitions;
};

#endif // TREEITEM_H
