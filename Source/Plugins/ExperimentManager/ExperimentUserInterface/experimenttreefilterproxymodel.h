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

#ifndef TREEFILTERPROXYMODEL_H
#define TREEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "experimentgraphiceditor_global.h"

class TreeFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
		explicit TreeFilterProxyModel(QObject *parent = NULL);
		void setTreeFilterSettings(const TreeFilterSettings &settings);
        
    protected:
        bool filterAcceptsRow(int source_row, const QModelIndex &index) const;

	private:
		TreeFilterSettings m_ViewSettings;
};

#endif // TREEFILTERPROXYMODEL_H
