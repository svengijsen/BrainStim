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

#include <QModelIndex>
#include "attributewidget.h"
#include "ui_AttributeWidget.h"
#include "experimenttreeitem.h"

AttributeWidget::AttributeWidget(ExperimentTreeItem *item) : QWidget(), ui(new Ui::AttributeWidget)
{
    itemEditing = item;
    m_definitions = item->getDefinitions();

    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);
    ui->twAttributes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(addAttributes()));
    connect(ui->pbRemove, SIGNAL(clicked()), this, SLOT(removeAttributes()));
    connect(ui->pbSave, SIGNAL(clicked()), this, SLOT(saveData()));
    connect(ui->twAttributes, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(modifyTable(QModelIndex)));

    setWindowTitle("Attributes of node: " + itemEditing->getName());
    addAction(ui->actionClose);

    if (m_definitions.size() > 0)
        fillAttTable();
}

AttributeWidget::~AttributeWidget()
{
    delete ui;
}

void AttributeWidget::fillAttTable()
{
    QList<QString> keyList = m_definitions.keys();

    for (int i = 0; i < m_definitions.size(); i++)
    {
        ui->twAttributes->insertRow(ui->twAttributes->rowCount());
        ui->twAttributes->setItem(ui->twAttributes->rowCount()-1, 0, new QTableWidgetItem(keyList.at(i)));
        ui->twAttributes->setItem(ui->twAttributes->rowCount()-1, 1, new QTableWidgetItem(m_definitions.value(keyList.at(i)).value.toString()));
    }
}

void AttributeWidget::addAttributes()
{
    if (ui->leName->text().size() != 0)
    {
		TreeItemDefinition tmpTreeItemDefinition;
		tmpTreeItemDefinition.type = TreeItemType_Attribute;
		tmpTreeItemDefinition.value = ui->leValue->text();
        m_definitions.insert(ui->leName->text(), tmpTreeItemDefinition);//ui->leValue->text());
        ui->twAttributes->insertRow(ui->twAttributes->rowCount());
        ui->twAttributes->setItem(ui->twAttributes->rowCount()-1, 0, new QTableWidgetItem(ui->leName->text()));
        ui->twAttributes->setItem(ui->twAttributes->rowCount()-1, 1, new QTableWidgetItem(ui->leValue->text()));

        ui->leName->setText("");
        ui->leValue->setText("");
    }
}

void AttributeWidget::removeAttributes()
{
    int row = -1;
    QList<QTableWidgetItem*> itemList = ui->twAttributes->selectedItems();

    if (itemList.size() > 0)
    {
        row = itemList.at(0)->row();

        if (row >= 0)
        {
            QTableWidgetItem *item = ui->twAttributes->item(row,0);

            if (item != NULL)
            {
                m_definitions.remove(item->text());
                ui->twAttributes->removeRow(row);
            }
        }
    }
}

void AttributeWidget::saveData()
{
    itemEditing->setDefinitions(m_definitions);
    close();
}

void AttributeWidget::modifyTable(const QModelIndex &index)
{
    if (!index.isValid())
    {
        ui->twAttributes->insertRow(ui->twAttributes->rowCount());
        ui->twAttributes->setItem(ui->twAttributes->rowCount()-1, 0, new QTableWidgetItem("[No Data]"));
        ui->twAttributes->setItem(ui->twAttributes->rowCount()-1, 1, new QTableWidgetItem("[No Data]"));
    }
}
