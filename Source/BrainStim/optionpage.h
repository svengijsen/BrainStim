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


#ifndef OPTIONPAGE_H
#define OPTIONPAGE_H

#include <QDialog>
#include <QSettings>
#include <QMessageBox>
#include "ui_optionpage.h"
#include "globalapplicationinformation.h"
#include "propertysettingswidget.h"

class OptionPage : public QDialog
{
	Q_OBJECT

public:
	OptionPage(QWidget *parent, GlobalApplicationInformation *g_AppInfo);
	~OptionPage();

	bool setPluginTabControlWidgets(QMap<QString, PropertySettingsWidget *> &lPluginOptionWidgets);

private:
	Ui::OptionPageClass ui;
	GlobalApplicationInformation *glob_AppInfo;
	QMap<QString, PropertySettingsWidget *> mapPluginWidgets;

private slots:
	void on_rdb_3DRenderer_3_toggled(bool);
	void on_rdb_3DRenderer_2_toggled(bool);
	void on_rdb_3DRenderer_toggled(bool);
	void on_chkEnableNetworkServer_toggled(bool);
	void on_btnBrowseForUserDocumentsRootDirectory_pressed();
	void on_btnBrowseForCustomPluginsRootDirectory_pressed();
	void setUI3DRendererControls(int type);
	void validateAndApplySettings();
	void readSettings();
	void selectNewScriptIncludeDir();
	void currentScriptMoveUp();
	void currentScriptMoveDown();
	void currentScriptDelete();
	void pluginSettingEditFinished(const QString &sParamName, const QString &sNewValue);

private:
	void applySettings();
	bool checkSettings();
	bool validateIPAddressString(QString &input) const;

	QMap<QString, QString> mapSettingsApplyPending;
};

#endif // OPTIONPAGE_H
