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


#include "optionpage.h"
#include "PropertySettingsWidgetContainer.h"
#include <QFileDialog>
#include <QListWidgetItem>
#include <QDesktopServices>

OptionPage::OptionPage(QWidget *parent, GlobalApplicationInformation *g_AppInfo) : QDialog(parent), glob_AppInfo(g_AppInfo)
{
	ui.setupUi(this);
	this->setWindowTitle(QString(MAIN_PROGRAM_INTERNAL_NAME) + " settings");
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(validateAndApplySettings()));	
	connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	
	ui.pbNewScriptIncludeDir->setIcon(QIcon(":/resources/browse.png"));
	ui.pbScriptIncludeMoveUp->setIcon(QIcon(":/resources/up.png"));
	ui.pbScriptIncludeMoveDown->setIcon(QIcon(":/resources/down.png"));
	ui.pbScriptIncludeDelete->setIcon(QIcon(":/resources/delete.png"));
	connect(ui.pbNewScriptIncludeDir, SIGNAL(clicked()), this, SLOT(selectNewScriptIncludeDir()));
	connect(ui.pbScriptIncludeMoveUp, SIGNAL(clicked()), this, SLOT(currentScriptMoveUp()));
	connect(ui.pbScriptIncludeMoveDown, SIGNAL(clicked()), this, SLOT(currentScriptMoveDown()));
	connect(ui.pbScriptIncludeDelete, SIGNAL(clicked()), this, SLOT(currentScriptDelete()));
	
	readSettings();	
}
OptionPage::~OptionPage()
{
	disconnect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(validateAndApplySettings()));	
	disconnect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	for (int i = 0; i < ui.twPlugins->count();i++)
	{
		if (ui.twPlugins->widget(i))
			ui.twPlugins->widget(i)->setParent(NULL);//To prevent a automatic destruction
	}
}

bool OptionPage::setPluginTabControlWidgets(QMap<QString, PropertySettingsWidget *> &lPluginOptionWidgets)
{
	mapPluginWidgets.clear();
	mapPluginWidgets = lPluginOptionWidgets;
	PropertySettingsWidgetContainer *tmpExpParWidgets = PropertySettingsWidgetContainer::instance();
	PropertySettingDefinition *tmpPropSettDef = NULL;
	QList<PropertySettingDefinitionStrc> *lTmpPropSettDefStrc = NULL;
	QMapIterator<QString, PropertySettingsWidget *> iter(mapPluginWidgets);
	QVariant tmpVariant;
	QString sTmpRegistryValue;
	QString sTmpRegPath;
	QString sPluginLowerName;

	while (iter.hasNext()) 
	{
		iter.next();
		PropertySettingsWidget *tmpPropSettWidget = iter.value();
		if (tmpPropSettWidget)
		{
			sPluginLowerName = iter.key().toLower();
			tmpPropSettDef = tmpExpParWidgets->getExperimentParameterDefinition(sPluginLowerName);
			if (tmpPropSettDef)
			{
				lTmpPropSettDefStrc = tmpPropSettDef->getParameterDefinitions();
				if (lTmpPropSettDefStrc)
				{
					foreach(PropertySettingDefinitionStrc tmpPropSettDefStrc, *lTmpPropSettDefStrc)
					{
						sTmpRegistryValue = tmpPropSettDefStrc.sDefaultValue;
						sTmpRegPath = QString(MAIN_PROGRAM_PLUGINS_REGISTRY_DIRNAME) + "/" + sPluginLowerName + "/" + tmpPropSettDefStrc.sName;
						if (glob_AppInfo->getSettingsInformation(sTmpRegPath, tmpVariant))
						{
							sTmpRegistryValue = tmpVariant.toString();
							if (tmpPropSettWidget->checkIfParameterExists(tmpPropSettDefStrc.sName))
							{
								tmpPropSettWidget->setParameter(tmpPropSettDefStrc.sName, sTmpRegistryValue, false, true);
							}
						}
						else
						{
							glob_AppInfo->setSettingsInformation(sTmpRegPath, sTmpRegistryValue, "string");
						}

					}
					connect((QObject*)tmpPropSettWidget, SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(pluginSettingEditFinished(const QString&, const QString&)), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
					ui.twPlugins->addTab(tmpPropSettWidget, iter.key());
				}
			}
		}
	}
	return true;
}

void OptionPage::pluginSettingEditFinished(const QString &sParamName, const QString &sNewValue)
{
	QMapIterator<QString, PropertySettingsWidget *> iter(mapPluginWidgets);
	while (iter.hasNext())
	{
		iter.next();
		PropertySettingsWidget *tmpPropSettWidget = iter.value();
		if (tmpPropSettWidget)
		{
			if (tmpPropSettWidget->checkIfParameterExists(sParamName))
			{
				QString sTmpRegPath = QString(MAIN_PROGRAM_PLUGINS_REGISTRY_DIRNAME) + "/" + iter.key().toLower() + "/" + sParamName;
				mapSettingsApplyPending.insert(sTmpRegPath, sNewValue);
			}
		}
	}

}

void OptionPage::validateAndApplySettings()
{
	if(checkSettings())
	{
		applySettings();
		accept();
	}
}

bool OptionPage::checkSettings()
{
	if(validateIPAddressString(ui.edtNetworkServerAddress->text()) == false)
	{
		QMessageBox msgBox(QMessageBox::Warning,"Input Validation Failed","Could not validate the Network Server's IP Address, please fix before apply!",QMessageBox::Ok,this);
		msgBox.exec();
		return false;
	}
	QDir tmpDir(ui.lneMainApplicationUserDirectory->text());
	if((tmpDir.exists() == false) || (ui.lneMainApplicationUserDirectory->text().isEmpty()))
	{
		QMessageBox msgBox(QMessageBox::Warning,"Directory not found","Could not find the User Documents Root Directory, please enter a valid directory!",QMessageBox::Ok,this);
		msgBox.exec();
		return false;
	}
	return true;
}

bool OptionPage::validateIPAddressString(QString &input) const 
{
	if(input.isEmpty()) 
		return true;
	QStringList slist = input.split(".",QString::SkipEmptyParts);
	int s = slist.size();
	if(s!=4)
		return false;
	//bool emptyGroup = false;
	bool bCanConvert;
	int nVal;
	for(int i=0;i<s;i++)
	{
		//if(slist[i].isEmpty())
		//{
		//	emptyGroup = true;
		//	continue;
		//}
		nVal = slist[i].toInt(&bCanConvert);
		if(!bCanConvert || nVal<0 || nVal>255) 
			return false;
	}
	//if(s<4 || emptyGroup) 
	//	return Intermediate;
	return true;
}

void OptionPage::applySettings()
{
	glob_AppInfo->setSettingsInformation(REGISTRY_DONOTLOADSCRIPTEXTENSION,(bool)(ui.chkDoNotLoadQTBindings->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setSettingsInformation(REGISTRY_OPENINEXTERNALDEBUGGER,(bool)(ui.chk_OpenDebOnError->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setSettingsInformation(REGISTRY_ALLOWMULTIPLEINHERITANCE,(bool)(ui.chkAllowMultipleInstances->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setSettingsInformation(REGISTRY_ENABLECUSTOMUSERLOGINS, (bool)(ui.chkAllowCustomUserLogins->checkState() && Qt::Checked), "bool");
	glob_AppInfo->setSettingsInformation(REGISTRY_USERPLUGINSROOTDIRECTORY, (QString)(ui.lneCustomPluginRootDirectory->text()), "string");
	MainAppInfo::setPluginsDirPath(ui.lneCustomPluginRootDirectory->text());
	glob_AppInfo->setSettingsInformation(REGISTRY_ENABLENETWORKSERVER,(bool)(ui.chkEnableNetworkServer->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setSettingsInformation(REGISTRY_SERVERHOSTADDRESS,(QString)(ui.edtNetworkServerAddress->text()),"string");
	glob_AppInfo->setSettingsInformation(REGISTRY_SERVERHOSTPORT,ui.edtNetworkServerPort->text().toInt(),"int");
	glob_AppInfo->setSettingsInformation(REGISTRY_MAINAPPUSERDIRECTORY,(QString)(ui.lneMainApplicationUserDirectory->text()),"string");

	if (ui.rdb_3DRenderer->isChecked())	
	{		
		glob_AppInfo->setSettingsInformation(REGISTRY_RENDERTYPE, 0, "int");//SvgView::Native);	
		glob_AppInfo->setSettingsInformation(REGISTRY_HQANTIALIAS, false, "bool");
	}
	else if (ui.rdb_3DRenderer_2->isChecked()) 
	{
		glob_AppInfo->setSettingsInformation(REGISTRY_RENDERTYPE, 1, "int");//SvgView::OpenGL);	
		glob_AppInfo->setSettingsInformation(REGISTRY_HQANTIALIAS, (bool)(ui.chk_HighAntiAFilter->checkState() && Qt::Checked), "bool");
	}
	else if (ui.rdb_3DRenderer_3->isChecked()) 
	{
		glob_AppInfo->setSettingsInformation(REGISTRY_RENDERTYPE, 2, "int");//SvgView::Image);	
		glob_AppInfo->setSettingsInformation(REGISTRY_HQANTIALIAS, false, "bool");
	}
	QStringList lNewIncludePaths;
	for(int i=0;i<ui.lwScriptIncludeDirs->count();i++)
	{
		lNewIncludePaths.append(ui.lwScriptIncludeDirs->item(i)->text());
	}
	glob_AppInfo->setSettingsInformation(REGISTRY_SCRIPTING_INCLUDEPATHS,lNewIncludePaths,"stringlist");

	//Pending Plugin changes settings...
	QMapIterator<QString, QString> iter(mapSettingsApplyPending);
	while (iter.hasNext())
	{
		iter.next();
		glob_AppInfo->setSettingsInformation(iter.key(), iter.value(), "string");
	}
	mapSettingsApplyPending.clear();
}

void OptionPage::readSettings()
{
	bool bTemp;
	QString sTemp;
	QStringList lTemp;
	int nTemp;

	QVariant tmpVariant;
	if (glob_AppInfo->getSettingsInformation(REGISTRY_MAINAPPUSERDIRECTORY, tmpVariant))
	{
		sTemp = tmpVariant.toString();
		ui.lneMainApplicationUserDirectory->setText(sTemp);
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_DONOTLOADSCRIPTEXTENSION, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkDoNotLoadQTBindings->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_OPENINEXTERNALDEBUGGER, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chk_OpenDebOnError->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_ENABLENETWORKSERVER, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkEnableNetworkServer->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_SERVERHOSTADDRESS, tmpVariant))
	{
		sTemp = tmpVariant.toString();
		ui.edtNetworkServerAddress->setText(sTemp);
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_SERVERHOSTPORT, tmpVariant))
	{
		nTemp = tmpVariant.toInt();
		ui.edtNetworkServerPort->setValue(nTemp);
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_RENDERTYPE, tmpVariant))
	{
		int nValue = tmpVariant.toInt();//(SvgView::RendererType)
		switch (nValue)
		{
		case 0://SvgView::Native://Native
			{
				ui.chk_HighAntiAFilter->setChecked(false);
				ui.rdb_3DRenderer->setChecked(true);
				break;
			}
		case 1://SvgView::OpenGL://OpenGL
			{
			if (glob_AppInfo->getSettingsInformation(REGISTRY_HQANTIALIAS, tmpVariant))
				{
					bTemp = tmpVariant.toBool();
					ui.chk_HighAntiAFilter->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
				}
				else
				{
					ui.chk_HighAntiAFilter->setChecked(false);
				}
				ui.rdb_3DRenderer_2->setChecked(true);
				break;
			}
		case 2://SvgView::Image://Image
			{
				ui.chk_HighAntiAFilter->setChecked(false);
				ui.rdb_3DRenderer_3->setChecked(true);
				break;
			}
		}
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_ALLOWMULTIPLEINHERITANCE, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkAllowMultipleInstances->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_ENABLECUSTOMUSERLOGINS, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkAllowCustomUserLogins->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_USERPLUGINSROOTDIRECTORY, tmpVariant))
	{
		sTemp = tmpVariant.toString();
		ui.lneCustomPluginRootDirectory->setText(sTemp);
	}
	if (glob_AppInfo->getSettingsInformation(REGISTRY_SCRIPTING_INCLUDEPATHS, tmpVariant))
	{
		lTemp = tmpVariant.toStringList();
		foreach(QString sTmpString, lTemp)
		{
			ui.lwScriptIncludeDirs->addItem(sTmpString);
			QListWidgetItem *tmpWidgetItem = ui.lwScriptIncludeDirs->item(ui.lwScriptIncludeDirs->count()-1);
			tmpWidgetItem->setFlags(tmpWidgetItem->flags() | Qt::ItemIsEditable);
		}
	}
}

void OptionPage::selectNewScriptIncludeDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
		QCoreApplication::applicationDirPath(),
		QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);
	if(dir.isEmpty() == false)
	{
		ui.lwScriptIncludeDirs->addItem(dir);
		QListWidgetItem *tmpWidgetItem = ui.lwScriptIncludeDirs->item(ui.lwScriptIncludeDirs->count()-1);
		tmpWidgetItem->setFlags(tmpWidgetItem->flags() | Qt::ItemIsEditable);
	}
}

void OptionPage::currentScriptMoveUp()
{
	if(ui.lwScriptIncludeDirs->selectedItems().isEmpty() == false)
	{
		int nTmpIndex = -1;
		foreach(QListWidgetItem *tmpWidgetItem,ui.lwScriptIncludeDirs->selectedItems())
		{
			nTmpIndex = ui.lwScriptIncludeDirs->row(tmpWidgetItem);
			if(nTmpIndex>0)
			{
				ui.lwScriptIncludeDirs->insertItem(nTmpIndex-1,ui.lwScriptIncludeDirs->takeItem(nTmpIndex));
			}
		}
		if(nTmpIndex > 0)
			ui.lwScriptIncludeDirs->item(nTmpIndex-1)->setSelected(true);
	}
}

void OptionPage::currentScriptMoveDown()
{
	if(ui.lwScriptIncludeDirs->selectedItems().isEmpty() == false)
	{
		int nTmpIndex = -1;
		foreach(QListWidgetItem *tmpWidgetItem,ui.lwScriptIncludeDirs->selectedItems())
		{
			nTmpIndex = ui.lwScriptIncludeDirs->row(tmpWidgetItem);
			if(nTmpIndex<ui.lwScriptIncludeDirs->count()-1)
			{
				ui.lwScriptIncludeDirs->insertItem(nTmpIndex+1,ui.lwScriptIncludeDirs->takeItem(nTmpIndex));
			}
		}
		if(nTmpIndex<ui.lwScriptIncludeDirs->count()-1)
			ui.lwScriptIncludeDirs->item(nTmpIndex+1)->setSelected(true);
	}
}

void OptionPage::currentScriptDelete()
{
	if(ui.lwScriptIncludeDirs->selectedItems().isEmpty() == false)
	{
		int nTmpIndex;
		foreach(QListWidgetItem *tmpWidgetItem,ui.lwScriptIncludeDirs->selectedItems())
		{
			nTmpIndex = ui.lwScriptIncludeDirs->row(tmpWidgetItem);
			ui.lwScriptIncludeDirs->takeItem(nTmpIndex);
		}
	}
}

void OptionPage::on_btnBrowseForMainApplicationUserDirectory_pressed()
{
	QFileDialog::Options options = QFlag(QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
	QFileDialog tmpFileDialog;
	QString sDirectory;
	sDirectory = tmpFileDialog.getExistingDirectory(this,
		"Select the directory",
		ui.lneMainApplicationUserDirectory->text(),
		options);
	if (!sDirectory.isEmpty())
		ui.lneMainApplicationUserDirectory->setText(sDirectory);
}

void OptionPage::on_btnBrowseForCustomPluginRootDirectory_pressed()
{
	QFileDialog::Options options = QFlag(QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
	QFileDialog tmpFileDialog;
	QString sDirectory;
	sDirectory = tmpFileDialog.getExistingDirectory(this,
		"Select the directory",
		ui.lneCustomPluginRootDirectory->text(),
		options);
	if (!sDirectory.isEmpty())
		ui.lneCustomPluginRootDirectory->setText(sDirectory);
}

void OptionPage::on_btnViewMainApplicationUserDirectory_pressed()
{
	QDesktopServices::openUrl(QUrl("file:///" + ui.lneMainApplicationUserDirectory->text(), QUrl::TolerantMode));
}

void OptionPage::on_btnViewCustomPluginRootDirectory_pressed()
{
	QDesktopServices::openUrl(QUrl("file:///" + ui.lneCustomPluginRootDirectory->text(), QUrl::TolerantMode));
}

void OptionPage::on_chkEnableNetworkServer_toggled(bool)
{
	ui.gbNetworkServerSettings->setEnabled(ui.chkEnableNetworkServer->isChecked());
}

void OptionPage::on_rdb_3DRenderer_toggled(bool)//native
{
	if (ui.rdb_3DRenderer->isChecked()) setUI3DRendererControls(1);
}

void OptionPage::on_rdb_3DRenderer_2_toggled(bool)//opengl
{
	if (ui.rdb_3DRenderer_2->isChecked())  setUI3DRendererControls(2);
}

void OptionPage::on_rdb_3DRenderer_3_toggled(bool)//image
{
	if (ui.rdb_3DRenderer_3->isChecked())  setUI3DRendererControls(3);
}

void OptionPage::setUI3DRendererControls(int type)
{
	#ifndef QT_NO_OPENGL
		ui.chk_HighAntiAFilter->setEnabled(false);
	#endif
		//if (type == 1)
			//SVGPreviewer->setRenderer(SvgView::Native);
			//type = type;
	#ifndef QT_NO_OPENGL
		//else if (type == 2) {
			if (type == 2) {
			ui.chk_HighAntiAFilter->setEnabled(true);
			//SVGPreviewer->setRenderer(SvgView::OpenGL);
		}
	#endif
		//else if (type == 3) {
			//SVGPreviewer->setRenderer(SvgView::Image);
		//}
}
