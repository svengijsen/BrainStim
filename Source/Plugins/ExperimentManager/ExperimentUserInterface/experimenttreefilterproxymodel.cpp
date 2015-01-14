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

#include "experimenttreefilterproxymodel.h"
#include "experimenttreemodel.h"
#include "experimenttreeitem.h"

TreeFilterProxyModel::TreeFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) 
{
	m_ViewSettings.bSkipComments = false;
	m_ViewSettings.bSkipEmptyAttributes = false;
}

void TreeFilterProxyModel::setTreeFilterSettings(const TreeFilterSettings &settings)
{
	m_ViewSettings = settings;
}

bool TreeFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &index) const
{
    //return true;
	if (index.isValid())
	{
		ExperimentTreeModel *model = dynamic_cast<ExperimentTreeModel*>(sourceModel());
		if (model)
		{
			ExperimentTreeItem *item = model->itemFromIndex(index);
			if(item)
			{
				if((item->child(source_row)->getName() == DEFINITION_TAG_COMMENT) && m_ViewSettings.bSkipComments)
				{
					return false;
				}
				else if ((item->child(source_row)->hasChildren() == false) && m_ViewSettings.bSkipEmptyAttributes)
				{
					return false;
				}
			}
		}
	}
    return true;    
    //Q_UNUSED(source_row);
    //Q_UNUSED(index);
    //return true;
}
