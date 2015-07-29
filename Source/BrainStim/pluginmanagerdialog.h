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

#define PLUGIN_DISABLE_TEXT		"Disabled: "
#define PLUGIN_STATIC_TEXT		" (Static)"

class customStandardItemModel : public QStandardItemModel
{
	Q_OBJECT

public:
	customStandardItemModel(int nRows, int nColumns, QObject *parent = NULL);
	~customStandardItemModel();

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

class PluginManagerDialog : public QDialog
{
	Q_OBJECT

public:
	PluginManagerDialog(installationManager *installMngr, QWidget *parent = NULL);
	~PluginManagerDialog();

private slots:
	void onSelectTableRow(QModelIndex index);
	void uninstallSelectedPlugin();
	void installPlugin();
	void enablePlugin();
	void disablePlugin();
	void configurePlugin();
	void backupAllPlugins();
	void browsePluginDirectory();
	void reloadAllPlugins();

private:
	void initialize();
	void fillTable();
	void enableDisablePlugin(const bool &bEnable);
	int getSelectedRowIndex(const QModelIndex &index);

	Ui::PluginManagerDialog ui;
	installationManager *pInstallMngr;
	QStandardItemModel *pluginCollectionModel;
	QMap<QString, bool> mapRegisteredPluginNamesToEnabledState;
};

#endif // PLUGINMANAGERDIALOG_H
