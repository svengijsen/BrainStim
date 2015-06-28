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
#include "installationmanagerbase.h"
#include "MainAppInfo.h"
#include "Archiver.h"
#include <QStandardItem>
#include <QFileDialog>
#include <QDesktopServices>

customStandardItemModel::customStandardItemModel(int nRows, int nColumns, QObject *parent) : QStandardItemModel(nRows, nColumns, parent){}
customStandardItemModel::~customStandardItemModel(){}

QVariant customStandardItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextColorRole)
	{
		if ((this->item(index.row(), index.column())) && (index.parent() == QModelIndex()))
		{
			if (this->item(index.row(), index.column())->text().endsWith(PLUGIN_STATIC_TEXT))
				return QColor(Qt::blue);
			else if (this->item(index.row(), index.column())->text().startsWith(PLUGIN_DISABLE_TEXT))
				return QColor(Qt::red);
		}
	}
	return QStandardItemModel::data(index, role);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////

PluginManagerDialog::PluginManagerDialog(installationManager *installMngr, QWidget *parent) : QDialog(parent), pInstallMngr(installMngr), pluginCollectionModel(NULL)
{
	ui.setupUi(this);
	this->setWindowTitle(QString("Plugin Manager"));
	connect(ui.buttonBox_2, SIGNAL(accepted()), this, SLOT(accept()));
	connect(ui.buttonBox_2, SIGNAL(rejected()), this, SLOT(reject()));
	connect(ui.treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onSelectTableRow(QModelIndex)));
	connect(ui.pbUninstall, SIGNAL(clicked()), this, SLOT(uninstallSelectedPlugin()));
	connect(ui.pbInstall, SIGNAL(clicked()), this, SLOT(installPlugin()));
	connect(ui.pbEnable, SIGNAL(clicked()), this, SLOT(enablePlugin()));
	connect(ui.pbDisable, SIGNAL(clicked()), this, SLOT(disablePlugin()));
	connect(ui.pbConfigure, SIGNAL(clicked()), this, SLOT(configurePlugin()));
	connect(ui.pbBackup, SIGNAL(clicked()), this, SLOT(backupAllPlugins()));
	connect(ui.pbBrowse, SIGNAL(clicked()), this, SLOT(browsePluginDirectory()));
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
	ui.pbConfigure->setEnabled(false);
	ui.pbUninstall->setEnabled(false);
	ui.pbEnable->setEnabled(false);
	ui.pbDisable->setEnabled(false);
	fillTable();
}

void PluginManagerDialog::fillTable()
{
	mapRegisteredPluginNamesToEnabledState = pInstallMngr->getRegisteredPluginNamesAndStates();
	if (pluginCollectionModel)
		pluginCollectionModel->clear();
	pluginCollectionModel = new customStandardItemModel(mapRegisteredPluginNamesToEnabledState.count(), 2, this);
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
	QStandardItem *childInternalNameItem;
	QStandardItem *childMinVersionItem;
	QList<QStandardItem *> lAdditionalChildColumns;
	QString sCurrentKey;
	bool bIsEnabled;

	for (int nRow = 0; nRow < mapRegisteredPluginNamesToEnabledState.count(); nRow++)
	{ 
		sCurrentKey = mapRegisteredPluginNamesToEnabledState.keys().at(nRow);
		sPluginInfo = pInstallMngr->getRegisteredPluginInterface(sCurrentKey)->GetPluginInformation();
		sMinMainAppVersion = pInstallMngr->getRegisteredPluginInterface(sCurrentKey)->GetMinimalMainProgramVersion();
		pluginType = pInstallMngr->getPluginType(sCurrentKey);
		sAbsFilePath = pInstallMngr->getPluginAbsFilePath(sCurrentKey);
		bIsEnabled = mapRegisteredPluginNamesToEnabledState.value(sCurrentKey);

		parentItem = new QStandardItem(sPluginInfo);
		if (pInstallMngr->isStaticPlugin(sCurrentKey))
			parentItem->setText(sPluginInfo + PLUGIN_STATIC_TEXT);
		if (bIsEnabled == false)
			parentItem->setText(PLUGIN_DISABLE_TEXT + sPluginInfo);

		pluginCollectionModel->setItem(nRow, parentItem);
		lAdditionalChildColumns.clear();
		
		childInternalNameItem = new QStandardItem("Internal Name");//Should stay the first child element!
		parentItem->appendRow(childInternalNameItem);
		lAdditionalChildColumns.append(new QStandardItem(sCurrentKey));

		childTypeItem = new QStandardItem("Type");//Should stay the second child element!
		parentItem->appendRow(childTypeItem);
		lAdditionalChildColumns.append(new QStandardItem(PluginInterface::pluginTypeToString(pluginType)));
		if (sAbsFilePath.isEmpty()==false)
		{
			childPathItem = new QStandardItem("Path");//Should stay the optional third child element!
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
	ui.treeView->selectionModel()->clearSelection();
}

int PluginManagerDialog::getSelectedRowIndex(const QModelIndex &index)
{
	//QString selectedText = index.data(Qt::DisplayRole).toString();
	if (index.parent() == QModelIndex())
	{
		//qDebug() << "row:" << index.row() << ", col:" << index.column();
		return index.row();
	}
	else
	{
		//qDebug() << "row:" << index.parent().row() << ", col:" << index.parent().column();
		return index.parent().row();
	}
	return -1;
}

void PluginManagerDialog::onSelectTableRow(QModelIndex index)
{
	ui.pbConfigure->setEnabled(false);
	ui.pbUninstall->setEnabled(false);
	ui.pbEnable->setEnabled(false);
	ui.pbDisable->setEnabled(false);

	if (!index.isValid())
		return;
	int nCurrentSelectedRow = getSelectedRowIndex(index);
	if (nCurrentSelectedRow >= 0)
	{
		ui.pbConfigure->setEnabled(true);
		ui.pbUninstall->setEnabled(true);
	}
	QString selectedText = index.data(Qt::DisplayRole).toString();
	if (selectedText.endsWith(PLUGIN_STATIC_TEXT))
	{
		ui.pbConfigure->setEnabled(false);
		ui.pbUninstall->setEnabled(false);
	}
	else if (selectedText.startsWith(PLUGIN_DISABLE_TEXT))
	{
		ui.pbEnable->setEnabled(true);
	}
	else
	{
		ui.pbDisable->setEnabled(true);
	}
}

void PluginManagerDialog::uninstallSelectedPlugin()
{
	if (pInstallMngr == NULL)
		return;
	this->setEnabled(false);
	QModelIndexList list = ui.treeView->selectionModel()->selectedIndexes();
	if (list.isEmpty() == false)
	{
		int nCurrentSelectedRow = getSelectedRowIndex(list.at(0));
		if (mapRegisteredPluginNamesToEnabledState.count() > nCurrentSelectedRow)
		{	
			QString sPluginInternalName = mapRegisteredPluginNamesToEnabledState.keys().at(nCurrentSelectedRow);
			if (pInstallMngr->isStaticPlugin(sPluginInternalName) == false)
			{
				int confirm = QMessageBox::question(this, tr("Uninstall selected plugin?"), tr("Are you sure to uninstall the selected plugin?\nThis action will delete all the installed plugin file(s)."), QMessageBox::Ok | QMessageBox::Cancel);
				if (confirm == QMessageBox::Ok)
				{
					//bool bResult = 
						pInstallMngr->unistallRegisteredPlugin(sPluginInternalName);
					fillTable();
				}
			}
		}
	}
	this->setEnabled(true);
}

void PluginManagerDialog::installPlugin()
{
	this->setEnabled(false);
	QString sInstallFilePath = QFileDialog::getOpenFileName(NULL, tr("Open Plugin installation file"), MainAppInfo::userPluginsDirPath(), tr("Compressed Install Package (*.zip);;Configuration Files (*.ini)"));
	if (sInstallFilePath.isEmpty() == false)
	{
		bool bRetval = false;
		if (pInstallMngr->installPlugin(sInstallFilePath))
		{
			QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_LOADDYNAMICPLUGINS_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetval));
			if (bRetval)
				fillTable();
		}
	}
	this->setEnabled(true);
}

void PluginManagerDialog::enablePlugin()
{
	enableDisablePlugin(true);
}

void PluginManagerDialog::disablePlugin()
{
	enableDisablePlugin(false);
}

void PluginManagerDialog::enableDisablePlugin(const bool &bEnable)
{
	if (pInstallMngr == NULL)
		return;
	this->setEnabled(false);
	QModelIndexList list = ui.treeView->selectionModel()->selectedIndexes();
	if (list.isEmpty() == false)
	{
		int nCurrentSelectedRow = getSelectedRowIndex(list.at(0));
		//bool bResult = 
			pInstallMngr->changeRegisteredPlugin(mapRegisteredPluginNamesToEnabledState.keys().at(nCurrentSelectedRow), bEnable);
	}
	fillTable();
	this->setEnabled(true);
}

void PluginManagerDialog::configurePlugin()
{
	if (pInstallMngr == NULL)
		return;
	this->setEnabled(false);
	QModelIndexList list = ui.treeView->selectionModel()->selectedIndexes();
	if (list.isEmpty() == false)
	{
		int nCurrentSelectedRow = getSelectedRowIndex(list.at(0));
		QString sPluginAbsFilePath = pInstallMngr->getPluginAbsFilePath(mapRegisteredPluginNamesToEnabledState.keys().at(nCurrentSelectedRow));
		QString sPluginSettingsFilePath = pInstallMngr->getPluginIniFilePath(sPluginAbsFilePath);
		if (sPluginSettingsFilePath.isEmpty() == false)
		{
			//bool bRetval = 
				QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_OPENFILES_SLOT_NAME, Qt::QueuedConnection, Q_ARG(QString, sPluginSettingsFilePath), Q_ARG(QStringList, QStringList()), Q_ARG(bool, false));
		}
		else
		{
			this->setEnabled(true);
			return;
		}
	}
	this->setEnabled(true);
	this->close();
}

void PluginManagerDialog::browsePluginDirectory()
{
	QDesktopServices::openUrl(QUrl("file:///" + MainAppInfo::userPluginsDirPath(), QUrl::TolerantMode));
}

void PluginManagerDialog::backupAllPlugins()
{
	if (pInstallMngr == NULL)
		return;
	int nAvailablePluginCount = pluginCollectionModel->rowCount();
	if (nAvailablePluginCount<=0)
		return;

	QString sPluginInternalName;
	QString sTempText;
	PluginInterface::PluginType tTempPluginType;
	bool bIsEnabled;
	QString sAbsPluginFilePath;
	QString sPluginFileBaseName;
	QTemporaryDir tempDir;
	QDir dIniFileDirectory;
	QString sIniFileDirectoryPath;
	bool bDidCreatePackageDirectory = false;
	for (int i = 0; i < nAvailablePluginCount; i++)
	{
		bDidCreatePackageDirectory = false;
		if (pluginCollectionModel->item(i)->text().endsWith(PLUGIN_STATIC_TEXT))
			continue;
		bIsEnabled = true;
		if (pluginCollectionModel->item(i)->text().startsWith(PLUGIN_DISABLE_TEXT))
			bIsEnabled = false;
		if (pluginCollectionModel->item(i)->hasChildren())
		{
			if (pluginCollectionModel->item(i)->child(0, 1))//InternalName
			{
				sPluginInternalName = pluginCollectionModel->item(i)->child(0, 1)->text();
				if (sPluginInternalName.isEmpty() == false)
				{
					if (pInstallMngr->isStaticPlugin(sPluginInternalName))
						continue;
					sAbsPluginFilePath = pInstallMngr->getPluginAbsFilePath(sPluginInternalName);
					sPluginFileBaseName = QFileInfo(sAbsPluginFilePath).completeBaseName();
					if (pInstallMngr->isRegisteredPlugin(sPluginFileBaseName))
					{
						if (pluginCollectionModel->item(i)->child(1, 1))//Type
						{
							sTempText = pluginCollectionModel->item(i)->child(1, 1)->text();
							tTempPluginType = PluginInterface::pluginStringToType(sTempText);
							if (tempDir.isValid() == false)
							{
								qDebug() << __FUNCTION__ << "Could not create a temporarily directory for the creation of the of the backup installer package, please perform this action with more credentials!";
								return;
							}
							QString sIniFileDirectoryPath = tempDir.path() + "/" + sPluginInternalName + "/" + sPluginFileBaseName;
							QString sIniFileName = sPluginFileBaseName + ".ini";
							QString sIniFilePath = sIniFileDirectoryPath + "/" + sIniFileName;
							if (dIniFileDirectory.mkpath(sIniFileDirectoryPath))
							{
								QStringList lInstallFiles = pInstallMngr->getPluginInstallFiles(sPluginInternalName);
								bDidCreatePackageDirectory = true;
								if (pInstallMngr->createPluginConfigurationSetting(sIniFilePath, sPluginInternalName, true, lInstallFiles) == false)
									bDidCreatePackageDirectory = false;
								for (int j = 0; j < lInstallFiles.count(); j++)
								{
									lInstallFiles[j] = installationManagerBase::resolveFileDirectoryPath(lInstallFiles[j], MainAppInfo::userPluginsDirPath(), MainAppInfo::appDirPath(), QFileInfo(sIniFileName).completeBaseName(), MainAppInfo::defaultPluginsDirPath(), MainAppInfo::appUserPath());
									QString sFileName = QFileInfo(lInstallFiles[j]).fileName();
									if (sFileName == sIniFileName)
										continue;
									QString sSourceFilePath = lInstallFiles[j]; //MainAppInfo::userPluginsDirPath() + "/" + lInstallFiles[j];
									QString sInstallationFilePath = sIniFileDirectoryPath + "/" + sFileName;
									QFile tmpFile;
									tmpFile.setFileName(sSourceFilePath);
									if (QFile(sInstallationFilePath).exists())
									{
										QFile(sInstallationFilePath).remove();
									}
									if (tmpFile.copy(sInstallationFilePath) == false)
									{
										bDidCreatePackageDirectory = false;
										qDebug() << __FUNCTION__ << "Could not copy a registered plugin installation file from " << sSourceFilePath << " to " << sInstallationFilePath << "Qt error: " << tmpFile.error();
										return;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	if (bDidCreatePackageDirectory == false)
	{
		qDebug() << __FUNCTION__ << "Could not create the backup installer package directory.";
	}
	else
	{
		QString sSaveFileName = QFileDialog::getSaveFileName(this, tr("Save File"), MainAppInfo::userPluginsDirPath(), tr("Compressed Install Package (*.zip)"));
		if (sSaveFileName.isEmpty() == false)
		{
			Archiver tmpArchiver;
			if (tmpArchiver.compressDir(sSaveFileName, tempDir.path(), true))
				QMessageBox::information(this,tr("Compressed Installer Backup Package Created"), "The Compressed Installer Backup Package was successfully created.");
		}
	}
	return;
}