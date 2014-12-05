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

#include "global.h"
#include "experimentmethodparamsconfiguration.h"
#include "PropertySettingsWidgetContainer.h"
#include "ExperimentManager.h"

ExperimentMethodParamsConfigurationDialog::ExperimentMethodParamsConfigurationDialog(QWidget *parent) : QDialog(parent), pCurrentExpTree(NULL), bIsInitialized(false), tmpParametersWidget(NULL), nInitFinitId(-1), bIsInit(true), nSelectedRow(-1)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);
	ui.leParamName->setEnabled(false);
	ui.leParamType->setEnabled(false);
	setWindowTitle("Configure Method Argument(s)");
	connect(ui.pbUpdate, SIGNAL(clicked()), this, SLOT(updateSelectedParameter()));
	connect(ui.pbClose, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.twParameters, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(tableItemSelectionChanged(QTableWidgetItem *, QTableWidgetItem *)));
	clearAllParsedStructures();
}

ExperimentMethodParamsConfigurationDialog::~ExperimentMethodParamsConfigurationDialog()
{
	clearAllParsedStructures();
	tmpParametersWidget = NULL;
	bIsInit = false;
}

bool ExperimentMethodParamsConfigurationDialog::initialize()
{
	if (bIsInitialized)
		return true;
	PropertySettingsWidgetContainer *expParamWidgets = PropertySettingsWidgetContainer::instance();
	tmpParametersWidget = expParamWidgets->getExperimentParameterWidget(INITIALIZATIONS_PARAMETER_TAG);
	if (tmpParametersWidget == NULL)
	{
		qDebug() << __FUNCTION__ << "Could not fetch parameter collection widget named " << INITIALIZATIONS_PARAMETER_TAG;
		return false;
	}
	tmpParametersWidget->configurePropertyEditSignaling(false);
	bIsInitialized = true;
	return true;
}

void ExperimentMethodParamsConfigurationDialog::setInitFinitParameter(const int &nMethodID, const bool &bIsInitMethod)
{
	nInitFinitId = nMethodID;
	bIsInit = bIsInitMethod;
	if (pCurrentExpTree)
		parseExperimentTreeModel(pCurrentExpTree);
}

bool ExperimentMethodParamsConfigurationDialog::parseExperimentTreeModel(ExperimentTreeModel *pExperimentTreeModel)
{
	clearAllParsedStructures();
	if (pExperimentTreeModel == NULL)
		return false;
	if (initialize() == false)
		return false;
	pCurrentExpTree = pExperimentTreeModel;
	//bool bConnectResult = 
	connect(pCurrentExpTree, SIGNAL(modelModified()), this, SLOT(checkReparseModel()), Qt::ConnectionType(Qt::UniqueConnection | Qt::DirectConnection));
	ExperimentTreeItem *tmpParentExpTreeItem = pCurrentExpTree->getExperimentObjectInitFinitTreeItem(nInitFinitId, bIsInit);
	if (tmpParentExpTreeItem)
	{
		if (tmpParentExpTreeItem->hasChildren())
		{
			tmpParentExpTreeItem = tmpParentExpTreeItem->firstChild(OBJECT_TAG);
			if (tmpParentExpTreeItem)
			{
				ExperimentTreeItem *tmpExpTreeItem = tmpParentExpTreeItem->firstChild(INIT_FINIT_SIGNATURE_TAG);
				if (tmpExpTreeItem)
				{
					QString sMethodSignature = tmpExpTreeItem->getValue();
					ExperimentTreeItem *tmpExpTreeItem = tmpParentExpTreeItem->firstChild(INIT_FINIT_TYPE_TAG);
					if (tmpExpTreeItem)
					{
						QString sMethodType = tmpExpTreeItem->getValue();
						QString sMethodDescription = sMethodType + ": " + sMethodSignature + "(";
						tmpParentExpTreeItem = tmpParentExpTreeItem->firstChild(PARAMETERS_TAG);
						if (tmpParentExpTreeItem)
						{
							if (tmpParentExpTreeItem->hasChildren())
							{
								QStringList lParamNames;
								QStringList lParamTypes;
								QStringList lParamValues;
								int nNumberOfParameters = 0;
								for (int nChildCount = 0; nChildCount < tmpParentExpTreeItem->childCount(); nChildCount++)
								{
									if (tmpParentExpTreeItem->child(nChildCount)->getName().toLower() == PARAMETER_TAG)
									{
										if (tmpParentExpTreeItem->child(nChildCount)->hasChildren())
										{
											ExperimentTreeItem *pTmpExpTreeSubParamItem = tmpParentExpTreeItem->child(nChildCount)->firstChild(NAME_TAG);
											if (pTmpExpTreeSubParamItem)
											{
												QString sParamName = pTmpExpTreeSubParamItem->getValue();
												pTmpExpTreeSubParamItem = tmpParentExpTreeItem->child(nChildCount)->firstChild(VALUE_TAG);
												if (pTmpExpTreeSubParamItem)
												{
													QString sParamValue = pTmpExpTreeSubParamItem->getValue();
													pTmpExpTreeSubParamItem = tmpParentExpTreeItem->child(nChildCount)->firstChild(MEMBER_TYPE_TAG);
													if (pTmpExpTreeSubParamItem)
													{
														QString sParamType = pTmpExpTreeSubParamItem->getValue();
														lParamNames.append(sParamName);
														lParamTypes.append(sParamType);
														lParamValues.append(sParamValue);
														nNumberOfParameters++;
													}
												}
											}
										}
									}
								}
								if (nNumberOfParameters > 0)
								{
									ui.twParameters->setRowCount(nNumberOfParameters);
									for (int i = 0; i < nNumberOfParameters; i++)
									{
										if (i>0)
											sMethodDescription = sMethodDescription + ", ";
										QTableWidgetItem *newItem = new QTableWidgetItem(lParamNames.at(i));
										ui.twParameters->setItem(i, 0, newItem);
										newItem = new QTableWidgetItem(lParamTypes.at(i));
										ui.twParameters->setItem(i, 1, newItem);
										newItem = new QTableWidgetItem(lParamValues.at(i));
										ui.twParameters->setItem(i, 2, newItem);
										sMethodDescription = sMethodDescription + lParamNames.at(i);
									}
									ui.twParameters->resizeColumnToContents(0);
									ui.twParameters->resizeColumnToContents(1);
									ui.twParameters->resizeColumnToContents(2);
									ui.twParameters->selectRow(0);
								}
							}
						}
						sMethodDescription = sMethodDescription + ")";
						ui.labMethodSignature->setText(sMethodDescription);
					}
				}
			}
		}
	}
	return true;
}

void ExperimentMethodParamsConfigurationDialog::clearAllParsedStructures()
{
	nSelectedRow = -1;
	pCurrentExpTree = NULL;
	ui.labMethodSignature->clear();
	ui.twParameters->clear();
	ui.twParameters->setHorizontalHeaderLabels(QStringList() << "Name" << "Type" << "Value");
	ui.leParamName->clear();
	ui.leParamType->clear();
	ui.gbParameter->setVisible(false);
}

void ExperimentMethodParamsConfigurationDialog::updateSelectedParameter()
{
	if (pCurrentExpTree)
	{
		ExperimentTreeModel::strcObjectInitFinitSpecifier sInitFinitSpec;
		sInitFinitSpec.nInitFinitID = nInitFinitId;
		sInitFinitSpec.sObjectDefinitionName = ui.leParamName->text();
		pCurrentExpTree->updateExperimentObjectInitFinitParameter(sInitFinitSpec, ui.leParamValue->text(), bIsInit);
	}
}

void ExperimentMethodParamsConfigurationDialog::tableItemSelectionChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
	if (current)
	{
		ui.gbParameter->setVisible(true);
		if (current == previous)
			return;
		nSelectedRow = current->row();
		ui.leParamName->setText(ui.twParameters->item(nSelectedRow, 0)->text());
		ui.leParamType->setText(ui.twParameters->item(nSelectedRow, 1)->text());
		ui.leParamValue->setText(ui.twParameters->item(nSelectedRow, 2)->text());
	}
}

void ExperimentMethodParamsConfigurationDialog::checkReparseModel()
{
	int nLastSelectedRow = nSelectedRow;
	if (parseExperimentTreeModel(pCurrentExpTree))
	{
		ui.twParameters->selectRow(nLastSelectedRow);
		return;
	}
	nSelectedRow = -1;
}