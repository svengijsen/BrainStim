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

#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QMap>
#include "ExperimentGraphicEditor_Global.h"

class QModelIndex;
class ExperimentTreeItem;

namespace Ui
{
    class AttributeWidget;
}

class AttributeWidget : public QWidget
{
    Q_OBJECT
        
    signals:
        void newAttributes(QMap<QString,QVariant> attributes);

    public:
        explicit AttributeWidget(ExperimentTreeItem *item);
        ~AttributeWidget();
        
    private:
        Ui::AttributeWidget *ui;
        QMap<QString,TreeItemDefinition> m_definitions;
        ExperimentTreeItem *itemEditing;

        void fillAttTable();

    private slots:
        void addAttributes();
        void removeAttributes();
        void saveData();
        void modifyTable(const QModelIndex &index);
};

#endif // ATTRIBUTEWIDGET_H
