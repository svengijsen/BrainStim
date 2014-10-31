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

#ifndef EXPERIMENTMETHODPARAMSCONFIGURATION_H
#define EXPERIMENTMETHODPARAMSCONFIGURATION_H

#include <QDialog>
#include "ui_experimentmethodparamsconfiguration.h"
#include "ExperimentTreeModel.h"
#include "experimentparametervisualizer.h"

class ExperimentMethodParamsConfigurationDialog : public QDialog
{
	Q_OBJECT

public:
	ExperimentMethodParamsConfigurationDialog(QWidget *parent = NULL);
	~ExperimentMethodParamsConfigurationDialog();

	bool parseExperimentTreeModel(ExperimentTreeModel *pExperimentTreeModel);
	void setInitFinitParameter(const int &nMethodID, const bool &bIsInitMethod);

private slots:
	void updateSelectedParameter();
	void tableItemSelectionChanged(QTableWidgetItem * current, QTableWidgetItem * previous);
	void checkReparseModel();

private:
	bool initialize();
	void clearAllParsedStructures();

	Ui::ExperimentMethodParamsConfiguration ui;

	bool bIsInitialized;
	ExperimentTreeModel *pCurrentExpTree;
	ExperimentParameterVisualizer *tmpParametersWidget;
	int nInitFinitId;
	bool bIsInit;
	int nSelectedRow;
};

#endif // EXPERIMENTMETHODPARAMSCONFIGURATION_H
