//BrainStim
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


#include "optionpage.h"
#include <QFileDialog>
#include <QListWidgetItem>

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
	QMapIterator<QString, PropertySettingsWidget *> iter(mapPluginWidgets);
	while (iter.hasNext()) 
	{
		iter.next();
		if (iter.value())
		{

			//if (glob_AppInfo->getRegistryInformation(REGISTRY_ENABLENETWORKSERVER, tmpVariant))

			QString sParamName = "block_number";
			QString sParamValue = "99";
			if (iter.value()->checkIfParameterExists(sParamName))
			{
				iter.value()->setParameter(sParamName, sParamValue, false, true);
			}

			connect((QObject*)iter.value(), SIGNAL(rootItemEditFinished(const QString&, const QString&)), this, SLOT(pluginSettingEditFinished(const QString&, const QString&)), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
			ui.twPlugins->addTab(iter.value(), iter.key());
		}
	}
	return true;
}

void OptionPage::pluginSettingEditFinished(const QString &sParamName, const QString &sNewValue)
{
	int p = 9;
	//const QString &sParamName, const QString &sNewValue
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
	QDir tmpDir(ui.lneUserDocumentsRootDirectory->text());
	if((tmpDir.exists() == false) || (ui.lneUserDocumentsRootDirectory->text().isEmpty()))
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
	glob_AppInfo->setRegistryInformation(REGISTRY_DONOTLOADSCRIPTEXTENSION,(bool)(ui.chkDoNotLoadQTBindings->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setRegistryInformation(REGISTRY_OPENINEXTERNALDEBUGGER,(bool)(ui.chk_OpenDebOnError->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setRegistryInformation(REGISTRY_ALLOWMULTIPLEINHERITANCE,(bool)(ui.chkAllowMultipleInstances->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setRegistryInformation(REGISTRY_ENABLECUSTOMUSERLOGINS, (bool)(ui.chkAllowCustomUserLogins->checkState() && Qt::Checked), "bool");
	glob_AppInfo->setRegistryInformation(REGISTRY_ENABLENETWORKSERVER,(bool)(ui.chkEnableNetworkServer->checkState() && Qt::Checked),"bool");
	glob_AppInfo->setRegistryInformation(REGISTRY_SERVERHOSTADDRESS,(QString)(ui.edtNetworkServerAddress->text()),"string");
	glob_AppInfo->setRegistryInformation(REGISTRY_SERVERHOSTPORT,ui.edtNetworkServerPort->text().toInt(),"int");
	glob_AppInfo->setRegistryInformation(REGISTRY_USERDOCUMENTSROOTDIRECTORY,(QString)(ui.lneUserDocumentsRootDirectory->text()),"string");

	if (ui.rdb_3DRenderer->isChecked())	
	{		
		glob_AppInfo->setRegistryInformation(REGISTRY_RENDERTYPE, 0, "int");//SvgView::Native);	
		glob_AppInfo->setRegistryInformation(REGISTRY_HQANTIALIAS, false, "bool");
	}
	else if (ui.rdb_3DRenderer_2->isChecked()) 
	{
		glob_AppInfo->setRegistryInformation(REGISTRY_RENDERTYPE, 1, "int");//SvgView::OpenGL);	
		glob_AppInfo->setRegistryInformation(REGISTRY_HQANTIALIAS, (bool)(ui.chk_HighAntiAFilter->checkState() && Qt::Checked), "bool");
	}
	else if (ui.rdb_3DRenderer_3->isChecked()) 
	{
		glob_AppInfo->setRegistryInformation(REGISTRY_RENDERTYPE, 2, "int");//SvgView::Image);	
		glob_AppInfo->setRegistryInformation(REGISTRY_HQANTIALIAS, false, "bool");
	}
	QStringList lNewIncludePaths;
	for(int i=0;i<ui.lwScriptIncludeDirs->count();i++)
	{
		lNewIncludePaths.append(ui.lwScriptIncludeDirs->item(i)->text());
	}
	glob_AppInfo->setRegistryInformation(REGISTRY_SCRIPTING_INCLUDEPATHS,lNewIncludePaths,"stringlist");
}

void OptionPage::readSettings()
{
	bool bTemp;
	QString sTemp;
	QStringList lTemp;
	int nTemp;

	QVariant tmpVariant;
	if (glob_AppInfo->getRegistryInformation(REGISTRY_USERDOCUMENTSROOTDIRECTORY, tmpVariant))
	{
		sTemp = tmpVariant.toString();
		ui.lneUserDocumentsRootDirectory->setText(sTemp);
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_DONOTLOADSCRIPTEXTENSION, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkDoNotLoadQTBindings->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_OPENINEXTERNALDEBUGGER, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chk_OpenDebOnError->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_ENABLENETWORKSERVER, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkEnableNetworkServer->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_SERVERHOSTADDRESS, tmpVariant))
	{
		sTemp = tmpVariant.toString();
		ui.edtNetworkServerAddress->setText(sTemp);
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_SERVERHOSTPORT, tmpVariant))
	{
		nTemp = tmpVariant.toInt();
		ui.edtNetworkServerPort->setValue(nTemp);
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_RENDERTYPE, tmpVariant))
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
			if (glob_AppInfo->getRegistryInformation(REGISTRY_HQANTIALIAS, tmpVariant))
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
	if (glob_AppInfo->getRegistryInformation(REGISTRY_ALLOWMULTIPLEINHERITANCE, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkAllowMultipleInstances->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_ENABLECUSTOMUSERLOGINS, tmpVariant))
	{
		bTemp = tmpVariant.toBool();
		ui.chkAllowCustomUserLogins->setCheckState((Qt::CheckState)(bTemp*Qt::Checked));
	}
	if (glob_AppInfo->getRegistryInformation(REGISTRY_SCRIPTING_INCLUDEPATHS, tmpVariant))
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


void OptionPage::on_btnBrowseForUserDocumentsRootDirectory_pressed()
{
	QFileDialog::Options options = QFlag(QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly);
	QFileDialog tmpFileDialog;
	QString sDirectory;
	sDirectory = tmpFileDialog.getExistingDirectory(this,
		"Select the directory",
		ui.lneUserDocumentsRootDirectory->text(),
		options);
	if (!sDirectory.isEmpty())
		ui.lneUserDocumentsRootDirectory->setText(sDirectory);
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
