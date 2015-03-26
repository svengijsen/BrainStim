//BrainStim
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

#include "pluginmanagerdialog.h"
#include <QStandardItem>

PluginManagerDialog::PluginManagerDialog(installationManager *installMngr, QWidget *parent) : QDialog(parent), pInstallMngr(installMngr), pluginCollectionModel(NULL)
{
	ui.setupUi(this);
	this->setWindowTitle(QString("Plugin Manager"));
	connect(ui.buttonBox_2, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui.buttonBox_2, SIGNAL(rejected()), this, SLOT(reject()));
	connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onSelectTableRow(QModelIndex)));
	connect(ui.pbUninstall, SIGNAL(clicked()), this, SLOT(uninstallSelectedPlugin()));
	connect(ui.pbInstall, SIGNAL(clicked()), this, SLOT(installSelectedPlugin()));
	initialize();
}

PluginManagerDialog::~PluginManagerDialog()
{
	disconnect(ui.buttonBox_2, SIGNAL(accepted()), this, SLOT(accept()));
	disconnect(ui.buttonBox_2, SIGNAL(rejected()), this, SLOT(reject()));
	disconnect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onSelectTableRow(QModelIndex)));
}

void PluginManagerDialog::initialize()
{
	fillTable();
}

void PluginManagerDialog::fillTable()
{
	lPluginNames = pInstallMngr->getRegisteredPluginNames();

	pluginCollectionModel = new QStandardItemModel(lPluginNames.count(), 2, this);
	pluginCollectionModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Name")));
	pluginCollectionModel->setHorizontalHeaderItem(1, new QStandardItem(QString("Value")));

	ui.treeView->setUniformRowHeights(true);
	ui.treeView->setModel(pluginCollectionModel);
	ui.treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QString sPluginInfo;
	QString sMinMainAppVersion;
	PluginInterface::PluginType pluginType;
	QString sAbsFilePath;
	QStandardItem *parentItem;
	QStandardItem *childPathItem;
	QStandardItem *childTypeItem;
	QStandardItem *childMinVersionItem;
	QList<QStandardItem *> lAdditionalChildColumns;

	for (int nRow = 0; nRow < lPluginNames.count(); nRow++)
	{ 
		//pInstallMngr->getRegisteredPluginInterface(lPluginNames.at(nRow))->
		sPluginInfo = pInstallMngr->getRegisteredPluginInterface(lPluginNames.at(nRow))->GetPluginInformation();
		sMinMainAppVersion = pInstallMngr->getRegisteredPluginInterface(lPluginNames.at(nRow))->GetMinimalMainProgramVersion();
		pluginType = pInstallMngr->getPluginType(lPluginNames.at(nRow));
		sAbsFilePath = pInstallMngr->getPluginAbsFilePath(lPluginNames.at(nRow));

		parentItem = new QStandardItem(sPluginInfo);
		pluginCollectionModel->setItem(nRow, parentItem);
		lAdditionalChildColumns.clear();
		
		childTypeItem = new QStandardItem("Type");
		parentItem->appendRow(childTypeItem);
		lAdditionalChildColumns.append(new QStandardItem(PluginInterface::pluginTypeToString(pluginType)));
		if (sAbsFilePath.isEmpty()==false)
		{
			childPathItem = new QStandardItem("Path");
			parentItem->appendRow(childPathItem);
			lAdditionalChildColumns.append(new QStandardItem(sAbsFilePath));
		}
		childMinVersionItem = new QStandardItem("Minimal BrainStim Version");
		parentItem->appendRow(childMinVersionItem);
		lAdditionalChildColumns.append(new QStandardItem(sMinMainAppVersion));

		parentItem->appendColumn(lAdditionalChildColumns);
		for (int i = 0; i < 2; i++)
			ui.treeView->resizeColumnToContents(i);
	}
}

int PluginManagerDialog::getSelectedRowIndex(const QModelIndex &index)
{
	//QString selectedText = index.data(Qt::DisplayRole).toString();
	if (index.parent() == QModelIndex())
	{
		qDebug() << "row:" << index.row() << ", col:" << index.column();
		return index.row();
	}
	else
	{
		qDebug() << "row:" << index.parent().row() << ", col:" << index.parent().column();
		return index.parent().row();
	}
	return -1;
}

void PluginManagerDialog::onSelectTableRow(QModelIndex index)
{
	int nCurrentSelectedRow = getSelectedRowIndex(index);
}

void PluginManagerDialog::uninstallSelectedPlugin()
{
	if (pInstallMngr == NULL)
		return;
	QModelIndexList list = ui.treeView->selectionModel()->selectedIndexes();
	if (list.isEmpty() == false)
	{
		int nCurrentSelectedRow = getSelectedRowIndex(list.at(0));
		if (lPluginNames.count() > nCurrentSelectedRow)
		{
			pInstallMngr->unistallRegisteredPlugin(lPluginNames.at(nCurrentSelectedRow));
		}
		//QString selectedText = index.data(Qt::DisplayRole).toString();

	}
	//bool unistallRegisteredPlugin(const QString &sRegisteredPluginName);
	//bool installRegisteredPlugin(const QString &sRegisteredPluginName);
}

void PluginManagerDialog::installSelectedPlugin()
{
	QModelIndexList list = ui.treeView->selectionModel()->selectedIndexes();
	if (list.isEmpty() == false)
	{
		int nCurrentSelectedRow = getSelectedRowIndex(list.at(0));
		//QString selectedText = index.data(Qt::DisplayRole).toString();

	}
}