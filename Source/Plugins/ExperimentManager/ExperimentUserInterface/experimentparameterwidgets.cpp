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

#include "experimentparameterwidgets.h"
#include "experimentparametervisualizer.h"
#include "global.h"

ExperimentParameterWidgets* ExperimentParameterWidgets::m_Instance = 0;

ExperimentParameterWidgets::ExperimentParameterWidgets()
{
	lExperimentParameterDefinitions = NULL;
	fetchExperimentParameterDefinitions();
	createExperimentParameterWidgets();
}

ExperimentParameterWidgets::~ExperimentParameterWidgets()
{
	if(lExperimentParameterDefinitions)
	{
		if(lExperimentParameterDefinitions->isEmpty() == false)
		{
			for (int i=0;i<lExperimentParameterDefinitions->count();i++)
			{
				if((*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition)
					delete (*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition;
				if((*lExperimentParameterDefinitions)[i].wWidget)
					delete (*lExperimentParameterDefinitions)[i].wWidget;
				(*lExperimentParameterDefinitions)[i].sCollectionName = "";
			}
			lExperimentParameterDefinitions->clear();
		}
	}
}

bool ExperimentParameterWidgets::loadExperimentParameterDefinition(const QString &sFilePath, const QString &sCollectionName, const bool &bOnlyIfNotPresent, const QString &sCurrentDirectory)
{
	if(lExperimentParameterDefinitions == NULL)
		lExperimentParameterDefinitions = new QList<ExperimentParameterDefinitionCollection>;

	if((sFilePath.isEmpty()) || (sCollectionName.isEmpty()))
		return false;

	QString sFixedPath = ""; 
	QDir tmpDir;
	tmpDir.setPath(sFilePath);
	if(sCurrentDirectory.isEmpty()==false)
	{
		QFileInfo tmpFileInfo(QDir(sCurrentDirectory), sFilePath);
		if (tmpFileInfo.exists())
		{
			sFixedPath = tmpFileInfo.canonicalFilePath();
		}
		else
		{
			return false;
		}
	}
	else if(tmpDir.exists() == false)
	{
		return false;
	}
	else
	{
		sFixedPath = sFilePath;
	}

	if(lExperimentParameterDefinitions->isEmpty()==false)
	{
		for(int i=0;i<lExperimentParameterDefinitions->count();i++)
		{
			if(lExperimentParameterDefinitions->at(i).sCollectionName == sCollectionName)//Already Present
			{
				if(bOnlyIfNotPresent)
					return true;
				else
				{
					if((*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition)
					{
						delete (*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition;
						(*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition = NULL;
					}
					if((*lExperimentParameterDefinitions)[i].wWidget)
					{
						delete (*lExperimentParameterDefinitions)[i].wWidget;
						(*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition = NULL;
					}
					lExperimentParameterDefinitions->removeAt(i);
				}
			}
		}
	}

	ExperimentControlDefinitionLocationInfo tmpExpContrDefLoc;
	tmpExpContrDefLoc.sName = sCollectionName;
	tmpExpContrDefLoc.sPathToDefFile = sFixedPath;
	ExperimentParameterDefinitionCollection *tmpParDefCollection = new ExperimentParameterDefinitionCollection();
	tmpParDefCollection->sCollectionName = tmpExpContrDefLoc.sName;
	tmpParDefCollection->cExperimentParameterDefinition = new ExperimentParameterDefinitionContainer();
	if(tmpParDefCollection->cExperimentParameterDefinition->loadFromFile(tmpExpContrDefLoc.sPathToDefFile))
	{
		lExperimentParameterDefinitions->append(*tmpParDefCollection);
		createExperimentParameterWidgets();
		return true;
	}
	return false;
}

void ExperimentParameterWidgets::fetchExperimentParameterDefinitions()
{
	if(lExperimentParameterDefinitions)
		return;
	else
		lExperimentParameterDefinitions = new QList<ExperimentParameterDefinitionCollection>;
	
	QList<ExperimentControlDefinitionLocationInfo> sExpContrDefLocLst;
	ExperimentControlDefinitionLocationInfo tmpExpContrDefLoc;

	tmpExpContrDefLoc.sName = RETINOTOPYMAPPER_NAME;
	tmpExpContrDefLoc.sPathToDefFile = RETINOTOPYMAPPER_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = EXPERIMENT_TAG;
	tmpExpContrDefLoc.sPathToDefFile = EXPERIMENT_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = DECLARATIONS_OBJECT_TAG;
	tmpExpContrDefLoc.sPathToDefFile = DECLARATIONS_OBJECT_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = INITIALIZATION_OBJECT_TAG;
	tmpExpContrDefLoc.sPathToDefFile = INITIALIZATION_OBJECT_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = FINALIZATION_OBJECT_TAG;
	tmpExpContrDefLoc.sPathToDefFile = FINALIZATION_OBJECT_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = CONNECTION_OBJECT_TAG;
	tmpExpContrDefLoc.sPathToDefFile = CONNECTION_OBJECT_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = BLOCK_TAG;
	tmpExpContrDefLoc.sPathToDefFile = BLOCK_OBJECT_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = LOOP_TAG;
	tmpExpContrDefLoc.sPathToDefFile = LOOP_OBJECT_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = INITIALIZATIONS_PARAMETER_TAG;
	tmpExpContrDefLoc.sPathToDefFile = INITIALIZATIONS_PARAMETER_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = FINALIZATIONS_PARAMETER_TAG;
	tmpExpContrDefLoc.sPathToDefFile = FINALIZATIONS_PARAMETER_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = BLOCK_PARAMETER_TAG;
	tmpExpContrDefLoc.sPathToDefFile = BLOCK_PARAMETER_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = QML2VIEWER_NAME;
	tmpExpContrDefLoc.sPathToDefFile = QML2VIEWER_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	tmpExpContrDefLoc.sName = OBJECT_DEFINITION_TAG;
	tmpExpContrDefLoc.sPathToDefFile = OBJECT_DEFINITION_PARAMDEF_PATH;
	sExpContrDefLocLst.append(tmpExpContrDefLoc);

	foreach(tmpExpContrDefLoc, sExpContrDefLocLst)
	{
		ExperimentParameterDefinitionCollection tmpParDefCollection;
		tmpParDefCollection.sCollectionName = tmpExpContrDefLoc.sName;
		tmpParDefCollection.cExperimentParameterDefinition = new ExperimentParameterDefinitionContainer();
		tmpParDefCollection.cExperimentParameterDefinition->loadFromFile(tmpExpContrDefLoc.sPathToDefFile);
		lExperimentParameterDefinitions->append(tmpParDefCollection);
	}
}

bool ExperimentParameterWidgets::createCustomExperimentParameterWidgetCollection(const QString &sCollectionName, const bool &bClearExisting)
{
	if(lExperimentParameterDefinitions == NULL)
		return false;

	//Remove the original if presence
	for(int i=0;i<lExperimentParameterDefinitions->count();i++)
	{
		if(lExperimentParameterDefinitions->at(i).sCollectionName == sCollectionName)
		{
			if(bClearExisting)
			{
				if((*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition)
				{
					delete (*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition;
					(*lExperimentParameterDefinitions)[i].cExperimentParameterDefinition = NULL;
				}
				if((*lExperimentParameterDefinitions)[i].wWidget)
				{
					delete (*lExperimentParameterDefinitions)[i].wWidget;
					(*lExperimentParameterDefinitions)[i].wWidget = NULL;
				}
			}
			return true;
		}
	}

	ExperimentParameterDefinitionCollection tmpParDefCollection;
	tmpParDefCollection.sCollectionName = sCollectionName;
	tmpParDefCollection.cExperimentParameterDefinition = NULL;//new ExperimentParameterDefinitionContainer();
	tmpParDefCollection.wWidget = NULL;			
	lExperimentParameterDefinitions->append(tmpParDefCollection);
	return true;
}

ExperimentParameterVisualizer *ExperimentParameterWidgets::insertCustomExperimentParameterWidget(const QString &sCollectionName, QList<ExperimentParameterDefinitionStrc> *lExpParDefs)
{
	if(lExperimentParameterDefinitions == NULL)
		return NULL;

	//Check if the collection exists
	for(int i=0;i<lExperimentParameterDefinitions->count();i++)
	{
		if(lExperimentParameterDefinitions->at(i).sCollectionName == sCollectionName)
		{
			if((*lExperimentParameterDefinitions)[i].wWidget == NULL)
				(*lExperimentParameterDefinitions)[i].wWidget = new ExperimentParameterVisualizer();			
			for (QList<ExperimentParameterDefinitionStrc>::const_iterator itParamDef=lExpParDefs->cbegin();itParamDef!=lExpParDefs->cend();++itParamDef)
			{
				if((*lExperimentParameterDefinitions)[i].wWidget->addParameterProperty(&(*itParamDef),itParamDef->sDefaultValue) == false)
				{
					QMessageLogger::qDebug() << __FUNCTION__ << "Could not add custom parameter property: " << itParamDef->sName;
				}
			}
			(*lExperimentParameterDefinitions)[i].wWidget->configurePropertyEditSignaling(true);
			return (*lExperimentParameterDefinitions)[i].wWidget;
		}
	}
	return NULL;
}

void ExperimentParameterWidgets::createExperimentParameterWidgets()
{
	if(lExperimentParameterDefinitions == NULL)
		return;

	for (QList<ExperimentParameterDefinitionCollection>::iterator itParamDefs=lExperimentParameterDefinitions->begin();itParamDefs!=lExperimentParameterDefinitions->end();++itParamDefs)
	{
		if(itParamDefs->wWidget == NULL)
		{
			itParamDefs->wWidget = new ExperimentParameterVisualizer();			
			itParamDefs->wWidget->addGroupProperties(itParamDefs->cExperimentParameterDefinition->getGroupDefinitions());
									
			QList<ExperimentParameterDefinitionStrc> *tmpExpParamDefList = itParamDefs->cExperimentParameterDefinition->getParameterDefinitions();
			if(tmpExpParamDefList)
			{
				for (QList<ExperimentParameterDefinitionStrc>::const_iterator itParamDef=tmpExpParamDefList->cbegin();itParamDef!=tmpExpParamDefList->cend();++itParamDef)
				{
					if(itParamDefs->wWidget->addParameterProperty(&(*itParamDef),itParamDef->sDefaultValue) == false)
					{
						QMessageLogger::qDebug() << __FUNCTION__ << "Could not add parameter property: " << itParamDef->sName;
					}
				}
			}

			itParamDefs->wWidget->configurePropertyEditSignaling(true);

			//bResult = connect(itParamDefs->wWidget->getVariantPropertyFactory(), SIGNAL(PropertyWidgetChanged(QWidget*, const QString&)), this, SIGNAL(ParameterWidgetChanged(QWidget*, const QString&)),Qt::UniqueConnection);
		}
	}
}

ExperimentParameterVisualizer *ExperimentParameterWidgets::getExperimentParameterWidget(const QString &sCollectionName)
{
	if((lExperimentParameterDefinitions == NULL) || (sCollectionName.isEmpty()))
		return NULL;

	for (QList<ExperimentParameterDefinitionCollection>::iterator itParamDefs=lExperimentParameterDefinitions->begin();itParamDefs!=lExperimentParameterDefinitions->end();++itParamDefs)
	{
		if(itParamDefs->sCollectionName.compare(sCollectionName,Qt::CaseInsensitive) == 0)
		{
			if(itParamDefs->wWidget)
			{
				return itParamDefs->wWidget;
			}
			return NULL;
		}		
	}
	return NULL;
}

bool ExperimentParameterWidgets::setWidgetParameter(const QString &sUniquePropertyIdentifier, const QString &sCollectionName, const QString &sParameterValue, const bool &bSetModified)
{
	ExperimentParameterVisualizer *tmpParamVis = getExperimentParameterWidget(sCollectionName);
	if(tmpParamVis)
	{
		return tmpParamVis->setWidgetParameter(sUniquePropertyIdentifier, sParameterValue, bSetModified);
	}
	return NULL;
}

ExperimentParameterDefinitionContainer *ExperimentParameterWidgets::getExperimentParameterDefinition(const QString &sCollectionName)
{
	if(lExperimentParameterDefinitions == NULL)
		return NULL;
	if(lExperimentParameterDefinitions->isEmpty())
		return NULL;
	for(int i=0;i<lExperimentParameterDefinitions->count();i++)
	{
		if(lExperimentParameterDefinitions->at(i).sCollectionName == sCollectionName)
			return lExperimentParameterDefinitions->at(i).cExperimentParameterDefinition;
	}	
	return NULL;
}

QString ExperimentParameterWidgets::getUniqueParameterIndentifier(const ParameterEditingType &cParamEditType, const int &nObjectID, const int &nParamDefinitionID, const QString &sParamName)
{
	return tr("%1%2%3%4%5%6%7").arg((int)cParamEditType).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(nObjectID).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(QString(QString(nParamDefinitionID).toLatin1().toHex())).arg(EXPPARAMWIDGETS_UNIQUEPARAM_SPLITTER).arg(sParamName);
}
