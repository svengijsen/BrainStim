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

#ifndef PLUGINMANAGERDIALOG_H
#define PLUGINMANAGERDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include "ui_pluginmanagerdialog.h"
#include "installationmanager.h"

class PluginManagerDialog : public QDialog
{
	Q_OBJECT

public:
	PluginManagerDialog(installationManager *installMngr, QWidget *parent = NULL);
	~PluginManagerDialog();

private slots:
	void onSelectTableRow(QModelIndex index);
	void uninstallSelectedPlugin();
	void installSelectedPlugin();

private:
	void initialize();
	void fillTable();
	int getSelectedRowIndex(const QModelIndex &index);

	Ui::PluginManagerDialog ui;
	installationManager *pInstallMngr;
	QStandardItemModel *pluginCollectionModel;
	QStringList lPluginNames;
};

#endif // PLUGINMANAGERDIALOG_H
