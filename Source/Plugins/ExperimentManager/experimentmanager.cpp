//ExperimentManagerplugin
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

#include <QFileDialog>
#include <QWaitCondition>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QGridLayout>
#include "experimentmanager.h"
#include "imageprocessor.h"
#include "prtformatmanager.h"
#include "triggertimer.h"
#include "retinotopymapper.h"
#include "qml2viewer.h"
#include "experimenttreeitem.h"
#include "ExperimentUserInterface/experimentgraphiceditor.h"
#include "propertysettingswidgetcontainer.h"
#include "experimentpropertysettingextensions.h"

QScriptValue ExperimentManager::ctor__experimentManager(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	//this function gets called first whenever a new object is constructed trough the script
	//	if (context->isCalledAsConstructor()) {
	//		// initialize the new object
	//		//context->thisObject().setProperty("bar", ...);
	//		// ...
	//		// return a non-object value to indicate that the
	//		// thisObject() should be the result of the "new Foo()" expression
	//		//return engine->undefinedValue();
	return engine->newQObject(new ExperimentManager(NULL,engine), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
} 

/*! \brief The ExperimentManager constructor.
*
*   You do not need to specify the parent object. 
*	The BrainStim script engine automatically retrieves the parent role
*/
ExperimentManager::ExperimentManager(QObject *parent, QScriptEngine* engine) : QObject(parent), parentObject(parent)
{	
	currentScriptEngine = engine;
	DefaultConstruct();
}

bool ExperimentManager::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

void ExperimentManager::DefaultConstruct()
{
	MainAppInfo::CreateHashTableFromEnumeration(typeid(ExperimentState).name(),experimentStateHash,this->staticMetaObject);
	m_RunFullScreen = true;
	m_ExpFileName = "";
	sExperimentOutputDataPostString = "";
	currentExperimentTree = NULL;
	cExperimentBlockTrialStructure = NULL;
	expDataLogger = NULL;
	ExpGraphicEditor = NULL;
	expParamWidgets = NULL;
	sConfiguredActiveScreen = NULL;
	RegisterMetaTypes();
	expParamWidgets = PropertySettingsWidgetContainer::instance();

	int nNewTypeIdentifier = expParamWidgets->registerCustomVariabeleType((QVariant::Type) rotationDirectionTypeId(), "rotationdirection");
	if (nNewTypeIdentifier >= 0)
	{
		RotationDirectionPropertyType *tmpRotationDirectionPropertyType = new RotationDirectionPropertyType();
		MainAppInfo::registerCustomPropertySettingObject((QObject*)tmpRotationDirectionPropertyType, nNewTypeIdentifier);
	}
	nNewTypeIdentifier = expParamWidgets->registerCustomVariabeleType((QVariant::Type) eccentricityDirectionTypeId(), "eccentricitydirection");
	if (nNewTypeIdentifier >= 0)
	{
		EccentricityDirectionPropertyType *tmpEccentricityDirectionPropertyType = new EccentricityDirectionPropertyType();
		MainAppInfo::registerCustomPropertySettingObject((QObject*)tmpEccentricityDirectionPropertyType, nNewTypeIdentifier);
	}
	nNewTypeIdentifier = expParamWidgets->registerCustomVariabeleType((QVariant::Type) movementDirectionTypeId(), "movementdirection");
	if (nNewTypeIdentifier >= 0)
	{
		MovementDirectionPropertyType *tmpMovementDirectionPropertyType = new MovementDirectionPropertyType();
		MainAppInfo::registerCustomPropertySettingObject((QObject*)tmpMovementDirectionPropertyType, nNewTypeIdentifier);
	}
	nNewTypeIdentifier = expParamWidgets->registerCustomVariabeleType((QVariant::Type) methodTypeTypeId(), "methodtype");
	if (nNewTypeIdentifier >= 0)
	{
		MethodTypePropertyType *tmpMethodTypePropertyType = new MethodTypePropertyType();
		MainAppInfo::registerCustomPropertySettingObject((QObject*)tmpMethodTypePropertyType, nNewTypeIdentifier);
	}

	bool bResult = expParamWidgets->loadExperimentParameterDefinition(BLOCK_OBJECT_PARAMDEF_PATH, BLOCK_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(RETINOTOPYMAPPER_PARAMDEF_PATH, RETINOTOPYMAPPER_NAME, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(EXPERIMENT_PARAMDEF_PATH, EXPERIMENT_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(DECLARATIONS_OBJECT_PARAMDEF_PATH, DECLARATIONS_OBJECT_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(INITIALIZATION_OBJECT_PARAMDEF_PATH, INITIALIZATION_OBJECT_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(FINALIZATION_OBJECT_PARAMDEF_PATH, FINALIZATION_OBJECT_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(CONNECTION_OBJECT_PARAMDEF_PATH, CONNECTION_OBJECT_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(BLOCK_OBJECT_PARAMDEF_PATH, BLOCK_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(LOOP_OBJECT_PARAMDEF_PATH, LOOP_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(INITIALIZATIONS_PARAMETER_PARAMDEF_PATH, INITIALIZATIONS_PARAMETER_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(FINALIZATIONS_PARAMETER_PARAMDEF_PATH, FINALIZATIONS_PARAMETER_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(BLOCK_PARAMETER_PARAMDEF_PATH, BLOCK_PARAMETER_TAG, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(QML2VIEWER_PARAMDEF_PATH, QML2VIEWER_NAME, true);
	bResult = bResult & expParamWidgets->loadExperimentParameterDefinition(OBJECT_DEFINITION_PARAMDEF_PATH, OBJECT_DEFINITION_TAG, true);
	changeCurrentExperimentState(ExperimentManager_Constructed);
}

/*! \brief The ExperimentManager destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
ExperimentManager::~ExperimentManager()
{
	cleanupExperiment();
	currentExperimentFile.clear();
	currentValidationFile.clear();
	if (ExpGraphicEditor)
	{
		delete ExpGraphicEditor;
		ExpGraphicEditor = NULL;
	}
	if (currentExperimentTree)
	{
		delete currentExperimentTree;
		currentExperimentTree = NULL;
	}
	if (cExperimentBlockTrialStructure)
	{
		delete cExperimentBlockTrialStructure;
		cExperimentBlockTrialStructure = NULL;
	}
}

QScriptValue ExperimentManager::ctor__experimentStateEnum(QScriptContext *context, QScriptEngine *engine)
{
	int nValue = context->argument(0).toInteger();	
	if (context->isCalledAsConstructor()) 
	{
		return engine->newVariant(0);	
	}
	else
	{
		return ExperimentManager::ExperimentState(nValue);
	}
}

#ifndef QT_NO_DEBUG

QString ExperimentManager::Test(const QString &sInput)
{
	return sInput;
}
#endif

void ExperimentManager::RegisterMetaTypes()
{//To register the Objects to the Meta, so they can be accessed trough an *.exml file
	MainAppInfo::registerMetaTypeClass<RetinotopyMapper>(RETINOTOPYMAPPER_NAME);
	MainAppInfo::registerMetaTypeClass<QML2Viewer>(QML2VIEWER_NAME);
	MainAppInfo::registerMetaTypeClass<TriggerTimer>(TRIGGERTIMER_NAME);
	MainAppInfo::registerMetaTypeClass<ExperimentTimer>(EXPERIMENTTIMER_NAME);
	MainAppInfo::registerMetaTypeClass<ImageProcessor>(IMAGEPROCESSOR_NAME);	
	MainAppInfo::registerMetaTypeClass<PrtFormatManager>(PRTFORMATMANAGER_NAME);
	MainAppInfo::registerMetaTypeClass<RandomGenerator>(RANDOMGENERATOR_NAME);
	MainAppInfo::registerMetaTypeClass<ExperimentTreeModel>(EXPERIMENTTREEMODEL_NAME);
	MainAppInfo::registerMetaTypeClass<ExperimentStructuresNameSpace::strcExperimentStructureState>(CEXPERIMENTSTRUCTURESTATE_NAME);
	MainAppInfo::registerMetaTypeClass<cExperimentStructure>(CEXPERIMENTSTRUCTURE_NAME);
	MainAppInfo::registerMetaTypeClass<cBlockStructure>(CBLOCKSTRUCTURE_NAME);
	MainAppInfo::registerMetaTypeClass<cLoopStructure>(CLOOPSTRUCTURE_NAME);
	MainAppInfo::registerMetaTypeClass<cBlockParameterStructure>(CBLOCKPARAMETERSTRUCTURE_NAME);
	MainAppInfo::registerMetaTypeClass<cObjectStructure>(COBJECTSTRUCTURE_NAME);
	MainAppInfo::registerMetaTypeClass<cMethodStructure>(CMETHODSTRUCTURE_NAME);
	MainAppInfo::registerMetaTypeClass<cMethodConnectionStructure>(CMETHODCONNECTIONSTRUCTURE_NAME);
}

void ExperimentManager::cleanupSingletons()
{
	if (expParamWidgets)
	{
		expParamWidgets->drop();
	}
}

void ExperimentManager::sendToMainAppLogOutput(const QString &strText2Write)
{
	emit WriteToLogOutput(strText2Write);
}

bool ExperimentManager::insertExperimentObjectBlockParameter(const int nObjectID,const QString sName,const QString sValue, bool bIsInitializing, bool bIsCustom)
{
	if (nObjectID >= 0) 
	{
		if (mExperimentObjectList.contains(nObjectID))
		{
			//Already set while initializing?
			if (bIsInitializing)
			{
				if (mExperimentObjectList.value(nObjectID).ExpBlockParams)
				{
					if (mExperimentObjectList.value(nObjectID).ExpBlockParams->contains(sName))
					{
						(*mExperimentObjectList[nObjectID].ExpBlockParams)[sName].bHasChanged = true;
						//(*mExperimentObjectList[nObjectID].ExpBlockParams)[sName].sValue = sValue;
						(*mExperimentObjectList[nObjectID].ExpBlockParams)[sName].bIsInitialized = bIsInitializing;
						(*mExperimentObjectList[nObjectID].ExpBlockParams)[sName].bIsCustom = bIsCustom;
						return true;
					}
				}
			}
			ParsedParameterDefinition tmpParDef;
			tmpParDef.bHasChanged = true;
			tmpParDef.sValue = sValue;
			tmpParDef.bIsInitialized = bIsInitializing;
			tmpParDef.bIsCustom = bIsCustom;
			mExperimentObjectList.value(nObjectID).ExpBlockParams->insert(sName, tmpParDef);
			return true;
		}
	}
	return false;
}

bool ExperimentManager::getExperimentObjectBlockParameter(const int nObjectID,const QString sName, ParsedParameterDefinition &strcParDef)
{
	if (nObjectID >= 0)
	{
		if (mExperimentObjectList.contains(nObjectID))
		{
			if ((mExperimentObjectList.value(nObjectID).ExpBlockParams == NULL) || (mExperimentObjectList.value(nObjectID).ExpBlockParams->isEmpty()))
				return false;
			if (mExperimentObjectList.value(nObjectID).ExpBlockParams->contains(sName))
			{
				strcParDef = mExperimentObjectList.value(nObjectID).ExpBlockParams->value(sName, strcParDef);
				if (strcParDef.bIsInitialized == true)
				{
					//Important, here we need to set the bFirstRequire option to false for future requests, but return the parameter with true
					ParsedParameterDefinition tmpPPD(strcParDef);
					tmpPPD.bIsInitialized = false;
					tmpPPD.bIsCustom = false;
					mExperimentObjectList.value(nObjectID).ExpBlockParams->insert(sName, tmpPPD);
				}
				return true;
			}
			return false;
		}
	}
	return false;
}

tParsedParameterList *ExperimentManager::constructOrRetrieveExperimentObjectBlockParameterStructure(const int nObjectID)
{
	if (nObjectID >= 0)
	{
		if (mExperimentObjectList.isEmpty() == false)
		{
			if (mExperimentObjectList.contains(nObjectID))
			{
				if (mExperimentObjectList[nObjectID].ExpBlockParams == NULL)
					mExperimentObjectList[nObjectID].ExpBlockParams = new tParsedParameterList();
				return mExperimentObjectList[nObjectID].ExpBlockParams;
			}
		}
	}
	return NULL;
}

bool ExperimentManager::getExperimentObjectScriptValue(const int &nObjectID,const QString &sKeyName,QScriptValue &sScriptValue)
{
	if (nObjectID >= 0)
	{
		if (mExperimentObjectList.contains(nObjectID))
		{
			if ((mExperimentObjectList.value(nObjectID).ExpBlockParams == NULL) || (mExperimentObjectList.value(nObjectID).ExpBlockParams->isEmpty()))
				return false;
			if (mExperimentObjectList.value(nObjectID).ExpBlockParams->contains(sKeyName.toLower()))
			{
				if (mExperimentObjectList.value(nObjectID).typedExpParamCntnr)
					return mExperimentObjectList.value(nObjectID).typedExpParamCntnr->getPropertySetting(sKeyName, sScriptValue, currentScriptEngine);
				else
					return false;
			}
		}
	}
	return false;
}

bool ExperimentManager::setExperimentObjectFromScriptValue(const int &nObjectID,const QString &sKeyName,const QScriptValue &sScriptValue)
{
	if (nObjectID >= 0)
	{
		if (mExperimentObjectList.contains(nObjectID))
		{
			if ((mExperimentObjectList.value(nObjectID).ExpBlockParams == NULL) || (mExperimentObjectList.value(nObjectID).ExpBlockParams->isEmpty()))
				return false;
			//int nCount = mExperimentObjectList.value(nObjectID).ExpBlockParams->count();
			if (mExperimentObjectList.value(nObjectID).ExpBlockParams->contains(sKeyName.toLower()))
			{
				if (mExperimentObjectList.value(nObjectID).typedExpParamCntnr)
				{
					QString tmpString = sScriptValue.toString();
					if (expandExperimentBlockParameterValue(tmpString))
						return mExperimentObjectList.value(nObjectID).typedExpParamCntnr->setPropertySetting(sKeyName, tmpString);
					return mExperimentObjectList.value(nObjectID).typedExpParamCntnr->setPropertySetting(sKeyName, sScriptValue);
				}
				return false;
			}
		}
	}
	return false;
}

bool ExperimentManager::logExperimentObjectData(const int &nObjectIndex, const int &nTimerIndex, const QString &strFunction, const QString &strTag, const QString &strMessage, const QString &strValue)
{
	if (expDataLogger)
	{
		expDataLogger->setLogVars(nObjectIndex, nTimerIndex, strFunction, strTag, strMessage, strValue);
		return true;
	}
	return false;
}

int ExperimentManager::createExperimentTimer()
{
	if (expDataLogger)
	{
		return expDataLogger->createTimer();
	}
	return -1;
}

bool ExperimentManager::startExperimentTimer(int nIndex)
{
	if (expDataLogger)
	{
		return expDataLogger->startTimer(nIndex);
	}
	return false;
}

double ExperimentManager::restartExperimentTimer(int nIndex)
{
	if ((expDataLogger) && (nIndex >= 0))
	{
		return expDataLogger->restartTimer(nIndex);
	}
	return -1;
}

double ExperimentManager::elapsedExperimentTimerTime(int nIndex)
{
	if ((expDataLogger) && (nIndex >= 0))
	{
		return expDataLogger->elapsedTimerTime(nIndex);
	}
	return -1;
}

QString ExperimentManager::getCurrentExperimentState()
{
	return experimentStateHash.key(getCurrExperimentState(),UNKNOWNENUMSTRING);
}

QString ExperimentManager::getCurrentDateTimeStamp(const QString &sFormat)
{
	return (QDateTime::currentDateTime().toString(sFormat));
}

QString ExperimentManager::getExperimentFileName()
{
	if(QFile::exists(m_ExpFileName))
	{
		return m_ExpFileName;
	}
	else
	{
		m_ExpFolder = "";
		return m_ExpFileName;
	}
}

bool ExperimentManager::setExperimentFileName(const QString sExpFileName)
{
	m_ExpFileName = sExpFileName;
	QFileInfo fInfo(m_ExpFileName);
	if (fInfo.exists())
	{
		m_ExpFolder = fInfo.absolutePath();
		return true;
	}
	return false;
}

bool ExperimentManager::saveExperiment(QString strFile)
{
	if (!currentExperimentTree)
		return false;
	
	QString fileName;
	if (strFile.isEmpty())
	{
		fileName = QFileDialog::getSaveFileName(NULL, tr("Save Experiment File"), QDir::currentPath(), tr("Experiment Files (*.exml *.xml);;Any file (*)"));
	}
	else
	{
		fileName = strFile;
	}
	if (fileName.isEmpty())
		return false;
	if(currentExperimentTree->write(fileName))
	{
		return true;
	}
	return false;
}

bool ExperimentManager::loadExperiment(QString strSource, bool bIsFile, const bool bSkipXMLValidation)
{
	cleanupExperiment();
	if (currentExperimentTree)
		delete currentExperimentTree;
	currentExperimentTree = new ExperimentTreeModel();
	QFile file;
	QString fileName = "";

	if (bIsFile)
	{
		fileName = getExperimentFileName();
		if (strSource.isEmpty())
		{
			if (fileName.isEmpty())
				fileName = QFileDialog::getOpenFileName(NULL, tr("Open Experiment File"), QDir::currentPath(), tr("Experiment Files (*.exml *.xml);;Any file (*)"));
		}
		else
		{
			fileName = strSource;
		}
		if (fileName.isEmpty())
			return false;
		file.setFileName(fileName);
		if (!file.open(QFile::ReadOnly | QFile::Text)) 
		{
			emit WriteToLogOutput("Could not open experiment file!");
			return false;
		}
		else
		{
			currentExperimentFile = file.readAll();
			//QString aa = MainAppInfo::appXsdFilePath() + QString(PLUGIN_EXMLDOC_VALIDATION_NAME);
			file.close();
		}
	}
	else
	{
		fileName = "";
		currentExperimentFile = strSource.toLatin1();
	}

	QFile validationFile(MainAppInfo::appXsdFilePath() + QString(PLUGIN_EXMLDOC_VALIDATION_NAME));//MainAppInfo::appXsdFilePath()
	if (validationFile.open(QFile::ReadOnly | QFile::Text)) 
		currentValidationFile = validationFile.readAll();
	validationFile.close();

	if (currentExperimentTree->read(currentExperimentFile))
	{
		setExperimentFileName(fileName);
		changeCurrentExperimentState(ExperimentManager_Loaded);
		return prePassiveParseExperiment(bSkipXMLValidation);
	}
	currentExperimentFile.clear();
	currentValidationFile.clear();
	delete currentExperimentTree;
	currentExperimentTree = NULL;
	return false;
}

void ExperimentManager::changeCurrentExperimentState(ExperimentState expCurrState)
{
	if (expCurrState != getCurrExperimentState())
	{
		experimentCurrentState = expCurrState;
		emit ExperimentStateHasChanged(expCurrState,getCurrentDateTimeStamp());
	}
	if(expCurrState == ExperimentManager_Stopped)
	{
		WriteAndCloseExperimentOutputData(sExperimentOutputDataPostString);
		cleanupExperimentObjects();
	}
}

bool ExperimentManager::validateExperiment(const bool bSkipXMLValidation)
{
	if (currentExperimentTree == NULL)
	{
		if ((loadExperiment("", true, bSkipXMLValidation) == false) || (currentExperimentTree == NULL))
		{
			qDebug() << __FUNCTION__ << "::No Experiment loaded!";
			return false;
		}
	}
	else if(getCurrExperimentState() == ExperimentManager_Constructed)
	{
		changeCurrentExperimentState(ExperimentManager_Loaded);
	}

	if (currentExperimentFile.isEmpty())
	{
		qDebug() << __FUNCTION__ << "::No Experiment in memory to validate!";
		return false;
	}
	if (currentValidationFile.isEmpty())
	{
		qDebug() << __FUNCTION__ << "::No Experiment validation in memory!";
		return false;
	}
	if((getCurrExperimentState() != ExperimentManager_Loaded) && (getCurrExperimentState() != ExperimentManager_PreParsed))
	{
		qDebug() << __FUNCTION__ << "::Wrong state, could not validate the experiment!";
		return false;
	}

	if (bSkipXMLValidation == false)
	{
		XmlMessageHandler messageHandler;
		QXmlSchema schema;
		schema.setMessageHandler(&messageHandler);
		schema.load(currentValidationFile);

		bool bDoValidate = true;
		bool bSettingFound = false;
		QString sMenuActionPath = QString(MAIN_PROGRAM_PLUGINS_REGISTRY_DIRNAME) + "/" + QString(PLUGIN_INTERNAL_NAME).toLower() + "/" + QString(SETTING_ENABLEAUTOEXMLVALIDATION);
		QString sTmpResult = "";
		if (QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_GETSTOREDSETTINGVALUE_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bSettingFound), Q_ARG(QString, sMenuActionPath), Q_ARG(QString*, &sTmpResult)))
		{
			if (bSettingFound)
			{
				if (sTmpResult.toLower() == "false")
				{
					bDoValidate = false;
				}
			}
		}
		if (bDoValidate)
		{
			bool errorOccurred = false;
			if (!schema.isValid())
			{
				errorOccurred = true;
			}
			else
			{
				QXmlSchemaValidator validator(schema);
				if (!validator.validate(currentExperimentFile))
					errorOccurred = true;
			}
			if (errorOccurred)
			{
				errorOccurred = errorOccurred;
				QString strMessage = messageHandler.statusMessage();
				int nColumn = messageHandler.column();
				int nLine = messageHandler.line();
				qDebug() << __FUNCTION__ << QString("Invalid schema, " + strMessage + "(line:" + QString::number(nLine) + ", col:" + QString::number(nColumn) + ")");
				return true;
			}
		}
	}
	return true;
}

bool ExperimentManager::runExperiment(const bool bSkipXMLValidation)
{
#ifdef Q_OS_WIN32 //Are we on Windows?
	//bool ret = 
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#else
	QThread::currentThread()->setPriority(QThread::HighPriority);  
	// QThread::TimeCriticalPriority);
#endif

	ExperimentState expCurrentState = getCurrExperimentState();
	if (expCurrentState == ExperimentManager_Stopped)
		changeCurrentExperimentState(ExperimentManager_Constructed);

	expCurrentState = getCurrExperimentState();
	if (expCurrentState < ExperimentManager_PreParsed)
	{
		if (!loadExperiment("","",bSkipXMLValidation))//(!prePassiveParseExperiment(bSkipXMLValidation))
		{
			changeCurrentExperimentState(ExperimentManager_Loaded);
			return false;
		}
	}
	
	if(!createExperimentObjects())
	{
		changeCurrentExperimentState(ExperimentManager_PreParsed);
		return false;
	}
	
	if(!connectExperimentObjects())
	{
		changeCurrentExperimentState(ExperimentManager_PreParsed);
		return false;
	}
	changeCurrentExperimentState(ExperimentManager_Configured);

	if(!initializeExperiment())
	{
		changeCurrentExperimentState(ExperimentManager_PreParsed);
		return false;
	}

	initializeDataLogger();//below should be after createExperimentObjects()

	if(!initExperimentObjects())
	{
		changeCurrentExperimentState(ExperimentManager_PreParsed);
		return false;
	}

	changeCurrentExperimentState(ExperimentManager_Initialized);
	changeCurrentExperimentState(ExperimentManager_IsStarting);

	if (expDataLogger)
		expDataLogger->startTimer(nExperimentTimerIndex);

	logExperimentObjectData(0,nExperimentTimerIndex,__FUNCTION__,"","FileName = ", getExperimentFileName());
	
	if(!startExperimentObjects(m_RunFullScreen))
	{
		changeCurrentExperimentState(ExperimentManager_PreParsed);
		return false;
	}

	//move the mouse cursor outside the most right screen
	QDesktopWidget *dt = QApplication::desktop();
	QCursor::setPos(dt->width(), dt->height()/2); // not at bottom because then mouse movement on Mac would show dock
	changeCurrentExperimentState(ExperimentManager_Started);
	return true;
}

void ExperimentManager::initializeDataLogger()
{
	if(expDataLogger == NULL)
	{
		expDataLogger = new ExperimentLogger(this);
		nExperimentTimerIndex = createExperimentTimer();
	}
}

void ExperimentManager::abortExperiment()
{
	if((getCurrExperimentState()==ExperimentManager_Started) || (getCurrExperimentState()==ExperimentManager_IsStarting))
	{
		changeCurrentExperimentState(ExperimentManager_IsStopping);
		if(!abortExperimentObjects())
			qDebug() << __FUNCTION__ << ": Could not abort the Experiment Objects";
		QThread::currentThread()->setPriority(QThread::NormalPriority);
	}
}

void ExperimentManager::stopExperiment()
{
	if(getCurrExperimentState()==ExperimentManager_Started)
	{
		changeCurrentExperimentState(ExperimentManager_IsStopping);
		if(!stopExperimentObjects())
			qDebug() << "ExperimentManager::stopExperiment Could not stop the Experiment Objects";
		QThread::currentThread()->setPriority(QThread::NormalPriority);
	}
}

bool ExperimentManager::WriteAndCloseExperimentOutputData(const QString &postFileName)
{
	if (expDataLogger)
	{
		QString strTemp; 
		if(postFileName == "")			
			strTemp = MainAppInfo::outputsDirPath() + "/" + QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()) + QString("_") + DEFAULT_OUTPUTFILE;
		else
			strTemp = MainAppInfo::outputsDirPath() + "/" + QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat()) + QString("_") + postFileName + ".txt";
		expDataLogger->WriteToOutput(strTemp);
		delete expDataLogger;
		expDataLogger = NULL;
		return true;
	}
	return false;
}

bool ExperimentManager::cleanupExperiment()
{
	if (ExpGraphicEditor)
		ExpGraphicEditor->setExperimentTreeModel(NULL, "");
	cleanupExperimentObjects();
	//ExperimentTreeBlockItemList.clear();
	ExperimentObjectTreeItemList.clear();
	//ExperimentTreeItemList.clear();
	currentExperimentFile.clear();
	currentValidationFile.clear();
	currentExperimentFile.clear();
	currentValidationFile.clear();
	if (currentExperimentTree)
	{
		delete currentExperimentTree;
		currentExperimentTree = NULL;
	}
	if (currentExperimentTree)
	{
		delete currentExperimentTree;
		currentExperimentTree = NULL;
	}
	if (cExperimentBlockTrialStructure)
	{
		delete cExperimentBlockTrialStructure;
		cExperimentBlockTrialStructure = NULL;
	}
	changeCurrentExperimentState(ExperimentManager_NoState);
	return true;
}

void ExperimentManager::cleanupExperimentObjects()
{
	QMap<int, objectElement>::iterator i;
	for (i = mExperimentObjectList.begin(); i != mExperimentObjectList.end(); ++i)
	{
		if ((i.value().pObject))
		{
			//if (!(lExperimentObjectList[i].pObject->metaObject()->indexOfSignal(QMetaObject::normalizedSignature("LogToOutputWindow(QString)")) == -1))//Is the signal present?
			//{
			//	//Disconnect the signal
			//	disconnect(lExperimentObjectList[i].pObject, SIGNAL(LogToOutputWindow(QString)), this, SIGNAL(WriteToLogOutput(QString)));//Qt::QueuedConnection --> makes it asynchronyous
			//}
			//The same for "LogExpObjData(int,int,QString)"??
			//logExperimentObjectData(const int nObjectID,const int nTimerID, const QString data2Log);
			QMetaType::destroy(i.value().nMetaID, i.value().pObject);
			i.value().pObject = NULL;
		}
		i.value().nMetaID = -1;
		i.value().nObjectID = -1;
		i.value().sObjectName = "";
		if (i.value().typedExpParamCntnr)
		{
			delete i.value().typedExpParamCntnr;
			i.value().typedExpParamCntnr = NULL;
		}
		if (i.value().ExpBlockParams)
		{
			delete i.value().ExpBlockParams;
			i.value().ExpBlockParams = NULL;
		}
	}
	mExperimentObjectList.clear();
}

bool ExperimentManager::changeExperimentObjectsSignalSlots(bool bDisconnect, int nObjectID)
{
	if (!currentExperimentTree)
	{
		qDebug() << __FUNCTION__ << ":No Experiment loaded!";
		return false;
	}
	if (mExperimentObjectList.isEmpty())
		return true;

	const objectElement *currentElement;
	//Check if nObjectID is specified, or -1 meaning all
	if (nObjectID >= 0)
	{
		if (mExperimentObjectList.contains(nObjectID) == false)
			return false;
	}

	QMap<int, objectElement>::iterator i;
	for (i = mExperimentObjectList.begin(); i != mExperimentObjectList.end(); ++i)
	{
		if (nObjectID >= 0)
			currentElement = &(mExperimentObjectList[nObjectID]);
		else
			currentElement = &i.value();
		const QMetaObject* metaObject = currentElement->pObject->metaObject();

		//QString sSlotFullName = "ObjectStateHasChanged";//FUNC_WIDGETSTATECHANGED_FULL;//FUNC_USERCLOSE_FULL;
		if (metaObject->indexOfSignal(metaObject->normalizedSignature(SIGNAL_USERCLOSE_FULL)) != -1)//Is the signal present?
		{
			if (bDisconnect)
			{
				disconnect(currentElement->pObject, SIGNAL(UserWantsToClose(void)), this, SLOT(abortExperiment(void)));
			}
			else
			{//For now the complete experiment including all object is aborted whenever the UserWantsToClose Signal gets activated
				connect(currentElement->pObject, SIGNAL(UserWantsToClose(void)), this, SLOT(abortExperiment(void)));
			}
		}

		if (metaObject->indexOfSignal(metaObject->normalizedSignature(SIGNAL_OBJECTSTOP_FULL)) != -1)//Is the signal present?
		{
			if (bDisconnect)
			{
				disconnect(currentElement->pObject, SIGNAL(ObjectShouldStop(void)), this, SLOT(stopExperiment(void)));
			}
			else
			{//For now the complete experiment including all object is aborted whenever the ObjectShouldStop Signal gets activated
				connect(currentElement->pObject, SIGNAL(ObjectShouldStop(void)), this, SLOT(stopExperiment(void)));
			}
		}

		if (metaObject->indexOfSignal(metaObject->normalizedSignature(FUNC_OBJECTSTATECHANGED_FULL)) != -1)//Is the signal present?
		{
			if (bDisconnect)
			{
				disconnect(currentElement->pObject, SIGNAL(ObjectStateHasChanged(ExperimentSubObjectState)), this, SLOT(changeExperimentSubObjectState(ExperimentSubObjectState)));
			} 
			else
			{
				connect(currentElement->pObject, SIGNAL(ObjectStateHasChanged(ExperimentSubObjectState)), this, SLOT(changeExperimentSubObjectState(ExperimentSubObjectState)));
			}
					
		}
		if (nObjectID >= 0)
			break;//Only one adjustment needed!
	}
	return true;
}

void ExperimentManager::changeExperimentSubObjectState(ExperimentSubObjectState nState)
{
	if (!currentExperimentTree)
	{
		qDebug() << __FUNCTION__ << ":No Experiment loaded!";
	}
	if (mExperimentObjectList.isEmpty())
		return;
	int nActiveExperimentObjects = 0;
	QMap<int, objectElement>::iterator i;
	for (i = mExperimentObjectList.begin(); i != mExperimentObjectList.end(); ++i)
	{ 
	//QString sName = i.value().sObjectName;
	if (QObject::sender() == i.value().pObject)
		{
			//We automatically close and delete the object after a "Abort" command...
			if(nState == Experiment_SubObject_Abort)
			{
				if (i.value().nCurrentState != Experiment_SubObject_IsAborting)//Multiple Abort events could occur, catch only first one
				{
					connectExperimentObjects(true, i.value().nObjectID);//disconnect!
					i.value().pObject->deleteLater();
					i.value().nCurrentState = Experiment_SubObject_IsAborting;
				}
			}
			else if(nState == Experiment_SubObject_Stop)
			{
				if (i.value().nCurrentState != Experiment_SubObject_IsStopping)//Multiple Stop events could occur, catch only first one
				{
					connectExperimentObjects(true, i.value().nObjectID);//disconnect!
					i.value().pObject->deleteLater();
					i.value().nCurrentState = Experiment_SubObject_IsStopping;
				}
			}
			else if((nState == Experiment_SubObject_Stopped) || (nState == Experiment_SubObject_Destructing))
			{
				changeExperimentObjectsSignalSlots(true,i.value().nObjectID);
				i.value().pObject = NULL;
				i.value().nCurrentState = nState;
			}
			else//All other cases
			{
				i.value().nCurrentState = nState;//Set the new object state
			}
			//i.value().sStateHistory.nState.append(i.value().nCurrentState);
			//i.value().sStateHistory.sDateTimeStamp.append(getCurrentDateTimeStamp());
		}
	if (!((i.value().nCurrentState == Experiment_SubObject_Initialized) || (i.value().nCurrentState == Experiment_SubObject_Stopped) ||
		(i.value().nCurrentState == Experiment_SubObject_Constructing) || (i.value().nCurrentState == Experiment_SubObject_Destructing)))
		{
			nActiveExperimentObjects++;
		}			
	}
	if(nActiveExperimentObjects == 0)//No more active object? Then the experiment ended
		changeCurrentExperimentState(ExperimentManager_Stopped);
}

bool ExperimentManager::invokeExperimentObjectsSlots(const QString &sSlotName)
{
	if (!currentExperimentTree)
	{
		qDebug() << __FUNCTION__ << ":No Experiment loaded!";
		return false;
	}
	if (mExperimentObjectList.isEmpty())
		return true;

	QMap<int, objectElement>::iterator iter;
	for (iter = mExperimentObjectList.begin(); iter != mExperimentObjectList.end(); ++iter)
	{
		//Get the meta object
		bool bRetVal;
		QString sSlotFullName = sSlotName + "()";
		QObject *tmpObject = iter.value().pObject;
		const QMetaObject* metaObject = tmpObject->metaObject();

		if (!(metaObject->indexOfMethod(sSlotFullName.toLatin1()) == -1))//Is the slot present?
		{
			//Invoke the start() slot
			if (!metaObject->invokeMethod(tmpObject, sSlotName.toLatin1(), Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal)))
			{
				qDebug() << __FUNCTION__ << "::Could not invoke the slot(" << sSlotFullName << ")!";		
				return false;
			}
		}
	}
	return true;
}

bool ExperimentManager::abortExperimentObjects()
{
	finalizeExperimentObjects();
	bool bRetVal = invokeExperimentObjectsSlots(QString(FUNC_OBJECT_ABORT));
	return bRetVal;
}

bool ExperimentManager::stopExperimentObjects()
{
	finalizeExperimentObjects();
	bool bRetVal = invokeExperimentObjectsSlots(QString(FUNC_OBJECT_STOP));
	return bRetVal;
}

bool ExperimentManager::initExperimentObjects()
{
	bool bRetVal = invokeExperimentObjectsSlots(QString(FUNC_OBJECT_INIT));
	return bRetVal;
}

bool ExperimentManager::startExperimentObjects(bool bRunFullScreen)
{
	Q_UNUSED(bRunFullScreen);
	changeExperimentObjectsSignalSlots(false);
	bool bRetVal = invokeExperimentObjectsSlots(QString(FUNC_OBJECT_START));
	return bRetVal;
}

bool ExperimentManager::prePassiveParseExperiment(const bool bSkipXMLValidation)
{
	if (!validateExperiment(bSkipXMLValidation))
		return false;

	if (!configureExperiment())
	{
		changeCurrentExperimentState(ExperimentManager_Loaded);
		return false;
	}
	QList<ExperimentTreeItem*> ExperimentTreeItemList;
	if (!createExperimentStructure(ExperimentTreeItemList, currentExperimentTree, cExperimentBlockTrialStructure, &mExperimentObjectList, this))
	{
		changeCurrentExperimentState(ExperimentManager_Loaded);
		return false;
	}
	changeCurrentExperimentState(ExperimentManager_PreParsed);
	return true;
}

bool ExperimentManager::createExperimentStructure(QList<ExperimentTreeItem*> &lExpTreeItems, ExperimentTreeModel *expTreeModel, cExperimentStructure* cExpStruct, QMap<int, objectElement> *mObjectElements, ExperimentManager *pCurrentExpManager)
{
	if ((expTreeModel == NULL) || (cExpStruct == NULL))
	{
		qDebug() << __FUNCTION__ << "::Could not create the Experiment Structure!";
		return false;
	}
	QStringList strList;
	strList.clear();
	strList.append(ROOT_TAG);//The EXML section

	if(ExperimentTreeModel::getStaticTreeElements(strList, lExpTreeItems,expTreeModel->getRootItem()) > 0)
	{
		if (convertExperimentDataStructure(&lExpTreeItems, cExpStruct, NULL, mObjectElements, pCurrentExpManager, EDS_TreeItemList_To_ExperimentStructure))
		{
			if (cExpStruct->prepareExperiment(true) == false)
			{
				qDebug() << __FUNCTION__ << "::Could not prepare Experiment Structure!";
				return false;
			}
			return true;
		}
		else
		{
			qDebug() << __FUNCTION__ << "::Could not parse Experiment Dom Node List!";
		}
	}
	return false;
}

void ExperimentManager::createNewExperimentStructure()
{
	if (cExperimentBlockTrialStructure)
	{
		delete cExperimentBlockTrialStructure;
		cExperimentBlockTrialStructure = NULL;
	}
	cExperimentBlockTrialStructure = new cExperimentStructure();
	//Default Settings
	cExperimentBlockTrialStructure->setExperimentID(0);
	cExperimentBlockTrialStructure->setExperimentName("");
	cExperimentBlockTrialStructure->setExperimentDebugMode(false);
}

bool ExperimentManager::configureExperiment()
{
	if (!currentExperimentTree)
	{
		qDebug() << __FUNCTION__ << ":No Experiment loaded!";
		return false;
	}
	createNewExperimentStructure();
	QStringList strList;
	strList.append(ROOT_TAG);
	strList.append(DEFINES_TAG);
	strList.append(EXPERIMENT_TAG);	
	
	QList<ExperimentTreeItem*> lExpTreeItems;
	if(currentExperimentTree->getTreeElements(strList, lExpTreeItems) > 0)
	{
		int nNrOfObjects = lExpTreeItems.count();
		if (nNrOfObjects>0)
		{
			ExperimentTreeItem* tmpTreeItem;
			ExperimentTreeItem* tmpTreeSubItem;
			QMap<QString, TreeItemDefinition> tTmpTreeItemDefs;
			QString tmpString = "";

			tmpTreeItem = lExpTreeItems.at(0);//We'll only use the first one by now and ignore the rest			
			if (tmpTreeItem->hasChildren()) 
			{
				tTmpTreeItemDefs = tmpTreeItem->getDefinitions();
				if(tTmpTreeItemDefs.contains(ID_TAG))
				{
					tmpString = tTmpTreeItemDefs[ID_TAG].value.toString();
					if (!tmpString.isEmpty())//Correct Experiment ID?
						cExperimentBlockTrialStructure->setExperimentID(tmpString.toInt());
				}

				tmpTreeSubItem = tmpTreeItem->firstChild(NAME_TAG);
				tmpString = tmpTreeSubItem->getValue();
				if (!tmpString.isEmpty())
					cExperimentBlockTrialStructure->setExperimentName(tmpString);

				tmpTreeSubItem = tmpTreeItem->firstChild(DEBUGMODE_TAG);
				tmpString = tmpTreeSubItem->getValue();				
				if (!tmpString.isEmpty())
				{
					expandExperimentBlockParameterValue(tmpString);
					if (tmpString == TYPE_BOOL_TRUE)
						cExperimentBlockTrialStructure->setExperimentDebugMode(true);
					else
						cExperimentBlockTrialStructure->setExperimentDebugMode(false);
				}
				else
				{
					cExperimentBlockTrialStructure->setExperimentDebugMode(false);
				}
			}
		}
		return true;
	}
	return false;
}

void ExperimentManager::setDebugMode(bool mode)
{
	cExperimentBlockTrialStructure->setExperimentDebugMode(mode);
}

bool ExperimentManager::getDebugMode()
{
	return cExperimentBlockTrialStructure->getExperimentDebugMode();
}

void ExperimentManager::setExperimentName(QString sName)
{
	cExperimentBlockTrialStructure->setExperimentName(sName);
}
QString ExperimentManager::getExperimentName()
{
	return cExperimentBlockTrialStructure->getExperimentName();
}

void ExperimentManager::setExperimentOutputFilePostString(const QString &sPostString) 
{
	sExperimentOutputDataPostString = sPostString;
}

QWidget *ExperimentManager::getVisualExperimentEditor()
{
	if(ExpGraphicEditor == NULL)
	{
		ExpGraphicEditor = new ExperimentGraphicEditor();
		connect(ExpGraphicEditor, SIGNAL(IsDestructing(ExperimentGraphicEditor*)), this, SLOT(visualExperimentEditorDestroyed(ExperimentGraphicEditor*))); 
		ExpGraphicEditor->setExperimentManager(this);		
	}
	if (currentExperimentTree == NULL)
	{
		currentExperimentTree = new ExperimentTreeModel();
		currentExperimentTree->resetExperiment();
	}
	ExpGraphicEditor->setExperimentTreeModel(currentExperimentTree, "");
	return ExpGraphicEditor;
}

void ExperimentManager::visualExperimentEditorDestroyed(ExperimentGraphicEditor *pExpGraphEditor)
{
	if(ExpGraphicEditor == pExpGraphEditor)
		ExpGraphicEditor = NULL;
}

QScreen* ExperimentManager::getActiveStimuliOutputScreen()
{
	return sConfiguredActiveScreen;
}

bool ExperimentManager::setActiveStimuliOutputScreen(int nScreenNumber)
{
	if(nScreenNumber >= 0)
	{
		if(nScreenNumber < QGuiApplication::screens().count())
		{
			sConfiguredActiveScreen = QGuiApplication::screens().at(nScreenNumber);
			return true;
		}
	}
	return false;
}

QString ExperimentManager::getAvailableScreensInformation()
{
	QString sGatheredInfo = "";
	sGatheredInfo = sGatheredInfo + "\n" + "Number of screens:" + QString::number(QGuiApplication::screens().size()) + "\n";
	sGatheredInfo = sGatheredInfo + "\n" + "Primary screen:" + QString(QGuiApplication::primaryScreen()->name());
	foreach (QScreen *screen, QGuiApplication::screens()) 
	{
		sGatheredInfo = sGatheredInfo + "\n" + "Information for screen:" + screen->name();
		sGatheredInfo = sGatheredInfo + "\n" + "  Available geometry: x=" + QString::number(screen->availableGeometry().x()) + " y=" + QString::number(screen->availableGeometry().y()) + ", " + QString::number(screen->availableGeometry().width()) + "x" + QString::number(screen->availableGeometry().height());
		sGatheredInfo = sGatheredInfo + "\n" + "  Available size:" + QString::number(screen->availableSize().width()) + "x" + QString::number(screen->availableSize().height());
		sGatheredInfo = sGatheredInfo + "\n" + "  Available virtual geometry: x=" + QString::number(screen->availableVirtualGeometry().x()) + " y=" + QString::number(screen->availableVirtualGeometry().y()) + ", " + QString::number(screen->availableVirtualGeometry().width()) + "x" + QString::number(screen->availableVirtualGeometry().height());
		sGatheredInfo = sGatheredInfo + "\n" + "  Available virtual size:" + QString::number(screen->availableVirtualSize().width()) + "x" + QString::number(screen->availableVirtualSize().height());
		sGatheredInfo = sGatheredInfo + "\n" + "  Depth:" + QString::number(screen->depth()) + "bits";
		sGatheredInfo = sGatheredInfo + "\n" + "  Geometry: x=" + QString::number(screen->geometry().x()) + " y=" + QString::number(screen->geometry().y()) + ", " + QString::number(screen->geometry().width()) + "x" + QString::number(screen->geometry().height());
		sGatheredInfo = sGatheredInfo + "\n" + "  Logical DPI:" + QString::number(screen->logicalDotsPerInch());
		sGatheredInfo = sGatheredInfo + "\n" + "  Logical DPI X:" + QString::number(screen->logicalDotsPerInchX());
		sGatheredInfo = sGatheredInfo + "\n" + "  Logical DPI Y:" + QString::number(screen->logicalDotsPerInchY());

		switch (screen->orientation()) 
		{
			case Qt::PrimaryOrientation           : sGatheredInfo = sGatheredInfo + "\n" + "  Orientation:" + "Primary";
			case Qt::LandscapeOrientation         : sGatheredInfo = sGatheredInfo + "\n" + "  Orientation:" + "Landscape";
			case Qt::PortraitOrientation          : sGatheredInfo = sGatheredInfo + "\n" + "  Orientation:" + "Portrait";
			case Qt::InvertedLandscapeOrientation : sGatheredInfo = sGatheredInfo + "\n" + "  Orientation:" + "Inverted landscape";
			case Qt::InvertedPortraitOrientation  : sGatheredInfo = sGatheredInfo + "\n" + "  Orientation:" + "Inverted portrait";
			default                               : sGatheredInfo = sGatheredInfo + "\n" + "  Orientation:" + "Unknown";
		}

		sGatheredInfo = sGatheredInfo + "\n" + "  Physical DPI:" + QString::number(screen->physicalDotsPerInch());
		sGatheredInfo = sGatheredInfo + "\n" + "  Physical DPI X:" + QString::number(screen->physicalDotsPerInchX());
		sGatheredInfo = sGatheredInfo + "\n" + "  Physical DPI Y:" + QString::number(screen->physicalDotsPerInchY());
		sGatheredInfo = sGatheredInfo + "\n" + "  Physical size:" + QString::number(screen->physicalSize().width()) + "x" + QString::number(screen->physicalSize().height()) + "mm";

		switch (screen->primaryOrientation()) 
		{
			case Qt::PrimaryOrientation           : sGatheredInfo = sGatheredInfo + "\n" + "  Primary orientation:" + "Primary";
			case Qt::LandscapeOrientation         : sGatheredInfo = sGatheredInfo + "\n" + "  Primary orientation:" + "Landscape";
			case Qt::PortraitOrientation          : sGatheredInfo = sGatheredInfo + "\n" + "  Primary orientation:" + "Portrait";
			case Qt::InvertedLandscapeOrientation : sGatheredInfo = sGatheredInfo + "\n" + "  Primary orientation:" + "Inverted landscape";
			case Qt::InvertedPortraitOrientation  : sGatheredInfo = sGatheredInfo + "\n" + "  Primary orientation:" + "Inverted portrait";
			default                               : sGatheredInfo = sGatheredInfo + "\n" + "  Primary orientation:" + "Unknown";
		}

		sGatheredInfo = sGatheredInfo + "\n" + "  Refresh rate:" + QString::number(screen->refreshRate()) + "Hz";
		sGatheredInfo = sGatheredInfo + "\n" + "  Size:" + QString::number(screen->size().width()) + "x" + QString::number(screen->size().height());
		sGatheredInfo = sGatheredInfo + "\n" + "  Virtual geometry: x=" + QString::number(screen->virtualGeometry().x()) + " y=" + QString::number(screen->virtualGeometry().y()) + ", " + QString::number(screen->virtualGeometry().width()) + "x" + QString::number(screen->virtualGeometry().height());
		sGatheredInfo = sGatheredInfo + "\n" + "  Virtual size:" + QString::number(screen->virtualSize().width()) + "x" + QString::number(screen->virtualSize().height()) + "\n";
	}
	return sGatheredInfo;
}

bool ExperimentManager::showTreeviewExperimentDialog(ExperimentTreeModel *expTreeModel, const QString &sExpTreeModelCanonFilePath)
{
	if (expTreeModel == NULL)
	{
		if (currentExperimentTree == NULL)
		{
			if (loadExperiment(sExpTreeModelCanonFilePath, true) == false)
				return false;
			if (!prePassiveParseExperiment())
				return false;
		}
		expTreeModel = currentExperimentTree;
	}
	if (ExpGraphicEditor == NULL)
	{
		ExpGraphicEditor = new ExperimentGraphicEditor();
		connect(ExpGraphicEditor, SIGNAL(IsDestructing(ExperimentGraphicEditor*)), this, SLOT(visualExperimentEditorDestroyed(ExperimentGraphicEditor*)));
		ExpGraphicEditor->setExperimentManager(this);
	}
	bool bParseResult = ExpGraphicEditor->setExperimentTreeModel(expTreeModel, sExpTreeModelCanonFilePath);
	if (bParseResult)
	{
		ExpGraphicEditor->showTableviewExperimentDialog();
	}
	return bParseResult;
}

bool ExperimentManager::showVisualExperimentDialog(ExperimentTreeModel *expTreeModel, const QString &sExpTreeModelCanonFilePath)
{
	if(expTreeModel == NULL)
	{
		if(currentExperimentTree == NULL)
		{
			if(loadExperiment(sExpTreeModelCanonFilePath,true) == false)
				return false;
			if (!prePassiveParseExperiment())
				return false;
		}
		expTreeModel = currentExperimentTree;
	}
	if(ExpGraphicEditor == NULL)
	{
		ExpGraphicEditor = new ExperimentGraphicEditor();
		connect(ExpGraphicEditor, SIGNAL(IsDestructing(ExperimentGraphicEditor*)), this, SLOT(visualExperimentEditorDestroyed(ExperimentGraphicEditor*))); 
		ExpGraphicEditor->setExperimentManager(this);
	}
	bool bParseResult = ExpGraphicEditor->setExperimentTreeModel(expTreeModel,sExpTreeModelCanonFilePath);
	if (bParseResult)
	{
		ExpGraphicEditor->showVisualExperimentDialog();
	}
	return bParseResult;
}

cExperimentStructure *ExperimentManager::getExperimentStructure(const bool bCreateIfUndefined)
{
	if (bCreateIfUndefined && (cExperimentBlockTrialStructure == NULL))
	{
		createNewExperimentStructure();
	}
	return cExperimentBlockTrialStructure;
}

bool ExperimentManager::parseCurrentExperimentStructure()
{
	if (cExperimentBlockTrialStructure == NULL)
		return false;
	if (currentExperimentTree == NULL)
		currentExperimentTree = new ExperimentTreeModel();
	currentExperimentTree->enableModifiedSignaling(false);
	if (currentExperimentTree->resetExperiment() == false)
	{
		currentExperimentTree->enableModifiedSignaling(true);
		return false;
	}
	bool bResult = convertExperimentDataStructure(NULL, cExperimentBlockTrialStructure, currentExperimentTree, NULL, NULL, EDS_ExperimentStructure_To_ExperimentTreeModel);
	if (bResult)
	{
		if(cExperimentBlockTrialStructure->prepareExperiment(true))
			changeCurrentExperimentState(ExperimentManager_PreParsed);
	}
	else
	{
		changeCurrentExperimentState(ExperimentManager_NoState);
	}
	if (bResult)
		currentExperimentTree->emitModifiedSignal(false);
	currentExperimentTree->enableModifiedSignaling(true);
	return bResult;
}

bool ExperimentManager::finalizeExperimentObjects()
{
	return initializeExperiment(true);
}

bool ExperimentManager::initializeExperiment(bool bFinalize)
{
	if (cExperimentBlockTrialStructure == NULL)
	{
		qDebug() << __FUNCTION__ << "(" << bFinalize << ")::No Experiment loaded!";
		return false;
	}
	QMap<QString, strcInvokeObjectDefs> mapHexNumberToObjectInitializationsInvokeDefs;
	QList<cObjectStructure*> lExpObjects = cExperimentBlockTrialStructure->getObjectList();
	foreach(cObjectStructure *currentObject, lExpObjects)
	{
		if (currentObject)
		{
			QString tmpString;
			QObject *pSourceObject = NULL;
			pSourceObject = getObjectElementById(currentObject->getObjectID());
			if (pSourceObject == NULL)
				continue;
			const QMetaObject* sourceMetaObject = pSourceObject->metaObject();
			if (sourceMetaObject == NULL)
				continue;
			QList<cMethodStructure*> *currentMethodList = NULL;
			if (bFinalize)
				currentMethodList = cExperimentBlockTrialStructure->getObjectFinalizationList(currentObject->getObjectID());
			else
				currentMethodList = cExperimentBlockTrialStructure->getObjectInitializationList(currentObject->getObjectID());
			if (currentMethodList)
			{
				foreach(cMethodStructure *currentMethod, *currentMethodList)
				{
					QString sInitMethodOrderHexNumber = "0000";
					QList<cMethodParameterStructure*> currentMethodParamList = currentMethod->getMethodParameterList();
					if (currentMethodParamList.isEmpty() == false)
					{
						QStringList lParsedMethodParamValues;
						QStringList lParsedMethodParamTypes;
						foreach(cMethodParameterStructure *currentMethodParam, currentMethodParamList)
						{
							tmpString = currentMethodParam->getMethodParameterValue();
							expandExperimentBlockParameterValue(tmpString);
							lParsedMethodParamValues.append(tmpString);
							lParsedMethodParamTypes.append(currentMethodParam->getMethodParameterType());
						}

						int nArgCount = currentMethodParamList.count();
						QByteArray normType;
						int typeId;
						QList<QGenericArgument> sArguments;
						for (int nArgCntr = 0; nArgCntr < MAX_INVOKE_ARG_COUNT; nArgCntr++)
							sArguments << QGenericArgument();

						for (int k = 0; k < MAX_INVOKE_ARG_COUNT; k++)//max arguments
						{
							if (k < nArgCount)
							{
								normType = QMetaObject::normalizedType(lParsedMethodParamTypes[k].toLatin1());
								typeId = QMetaType::type(normType);
								if (typeId == QMetaType::Bool)
								{
									sArguments[k] = Q_ARG(bool, lParsedMethodParamValues[k].toInt());
								}
								else if (typeId == QMetaType::Int)
								{
									sArguments[k] = Q_ARG(int, lParsedMethodParamValues[k].toInt());
								}
								else if (typeId == QMetaType::Short)
								{
									sArguments[k] = Q_ARG(short, lParsedMethodParamValues[k].toShort());
								}
								else if (typeId == QMetaType::Double)
								{
									sArguments[k] = Q_ARG(double, lParsedMethodParamValues[k].toDouble());
								}
								else if (typeId == QMetaType::Long)
								{
									sArguments[k] = Q_ARG(long, lParsedMethodParamValues[k].toLong());
								}
								else//In all other cases we marshal to QString and give that a try... 
								{
									qWarning() << "initializeExperimentObjects(" << bFinalize << ")::Undefined argument (typeId=" << typeId << "), switching to QString to create a generic argument...";
									// below is dirty, but array doesn't work, passes wrong value to invoked function! weird bug??
									sArguments[k] = Q_ARG(QString, lParsedMethodParamValues[k]);
								}
							}
							else
							{
								break;
							}
						}
						sInitMethodOrderHexNumber = "0000";
						MainAppInfo::getHexedOrderNumber(currentMethod->getMethodOrderNumber(), sInitMethodOrderHexNumber, 4);
						strcInvokeObjectDefs sObjInitInvokeDef;
						sObjInitInvokeDef.pMetaObject = sourceMetaObject;
						sObjInitInvokeDef.pObject = pSourceObject;
						sObjInitInvokeDef.nOrderNumber = currentMethod->getMethodOrderNumber();
						sObjInitInvokeDef.baSignature = currentMethod->getMethodSignature().toLatin1();
						sObjInitInvokeDef.lGenArgs = sArguments;//<< sArguments0 << sArguments1 << sArguments2 << sArguments3 << sArguments4 << sArguments5 << sArguments6 << sArguments7 << sArguments8 << sArguments9;
						mapHexNumberToObjectInitializationsInvokeDefs.insert(sInitMethodOrderHexNumber, sObjInitInvokeDef);
						continue;
					}
					else//No parameters?
					{
						strcInvokeObjectDefs sObjInitInvokeDef;
						sObjInitInvokeDef.pMetaObject = sourceMetaObject;
						sObjInitInvokeDef.pObject = pSourceObject;
						sObjInitInvokeDef.nOrderNumber = currentMethod->getMethodOrderNumber();
						sObjInitInvokeDef.baSignature = currentMethod->getMethodSignature().toLatin1();
						//sObjInitInvokeDef.lGenArgs;
						mapHexNumberToObjectInitializationsInvokeDefs.insert(sInitMethodOrderHexNumber, sObjInitInvokeDef);
						continue;
					}
				}
			}
		}
	}
	foreach(strcInvokeObjectDefs tmpObjInitInvokeDef, mapHexNumberToObjectInitializationsInvokeDefs)
	{
		if (!(tmpObjInitInvokeDef.pMetaObject->invokeMethod(tmpObjInitInvokeDef.pObject, tmpObjInitInvokeDef.baSignature, tmpObjInitInvokeDef.lGenArgs.value(0, QGenericArgument()), tmpObjInitInvokeDef.lGenArgs.value(1, QGenericArgument())
			, tmpObjInitInvokeDef.lGenArgs.value(2, QGenericArgument()), tmpObjInitInvokeDef.lGenArgs.value(3, QGenericArgument()), tmpObjInitInvokeDef.lGenArgs.value(4, QGenericArgument()), tmpObjInitInvokeDef.lGenArgs.value(5, QGenericArgument())
			, tmpObjInitInvokeDef.lGenArgs.value(6, QGenericArgument()), tmpObjInitInvokeDef.lGenArgs.value(7, QGenericArgument()), tmpObjInitInvokeDef.lGenArgs.value(8, QGenericArgument()), tmpObjInitInvokeDef.lGenArgs.value(9, QGenericArgument()))))
		{
			qDebug() << __FUNCTION__ << "(isFinalize:" << bFinalize << ")::Could not invoke the Method(" << tmpObjInitInvokeDef.baSignature << ")!";
			return false;
		}
	}
	return true;
}

bool ExperimentManager::expandExperimentBlockParameterValue(QString &sValue)
{
	if (!sValue.isEmpty())
	{		
		int nLastIndex = sValue.lastIndexOf("}");
		if(nLastIndex > 1)
		{
			int nFirstIndex = sValue.lastIndexOf("{");
			if((nFirstIndex >= 0) && (nFirstIndex < nLastIndex))
			{
				QVariant varResult = "";
				if(getScriptContextValue(sValue.mid(nFirstIndex+1,nLastIndex-nFirstIndex-1),varResult))
				{
					if(varResult.type() == QVariant::List)
					{
						//Let's assume a QStringList here...
						//QStringList stringList;
						QString sCombinedStringList;
						QListIterator<QVariant> it(varResult.toList());
						if (it.hasNext())
						{
							sCombinedStringList = it.next().toString();
							while (it.hasNext()) 
							{
								sCombinedStringList = sCombinedStringList + "," + it.next().toString();
									//stringList << it.next().toString();
							}
						}
						sValue.replace(nFirstIndex,nLastIndex-nFirstIndex+1,sCombinedStringList);//varResult.toString());
					}
					else
					{
						sValue.replace(nFirstIndex,nLastIndex-nFirstIndex+1,varResult.toString());
					}					
					return true;
				}
			}	
		}
	}
	return false;
}

bool ExperimentManager::connectExperimentObjects(bool bDisconnect, int nObjectID)
{//nObjectID only implemented when bDisconnect = true!
	if (cExperimentBlockTrialStructure)
	{
		QList<cObjectStructure*> lExpObjects = cExperimentBlockTrialStructure->getObjectList();
		foreach(cObjectStructure *currentObject, lExpObjects)
		{
			if (currentObject)
			{
				QList<cMethodConnectionStructure*> *lMethodConns = cExperimentBlockTrialStructure->getObjectMethodConnectionList(currentObject->getObjectID());
				{
					if (lMethodConns)
					{
						QObject *pSourceObject = NULL;
						QObject *pTargetObject = NULL;
						foreach(cMethodConnectionStructure *currentMethodConn, *lMethodConns)
						{
							pTargetObject = getObjectElementById(currentMethodConn->getTargetObjectID());
							if (pTargetObject == NULL)
								continue;
							pSourceObject = getObjectElementById(currentMethodConn->getSourceObjectID());
							if (pSourceObject == NULL)
								continue;

							const QMetaObject* targetMetaObject = pTargetObject->metaObject();
							const QMetaObject* sourceMetaObject = pSourceObject->metaObject();
							int nSourceID = -1;
							int nTargetID = -1;

							if (bDisconnect == false)
							{
								QMetaObject::Connection bMetaConnection;
								if (currentMethodConn->getTargetMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
								{
									if (currentMethodConn->getSourceMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
									{
										nSourceID = sourceMetaObject->indexOfSignal(QMetaObject::normalizedSignature(currentMethodConn->getSourceSignature().toLatin1()));
										if (!(nSourceID == -1))//Is the signal present?
										{
											nTargetID = targetMetaObject->indexOfSignal(QMetaObject::normalizedSignature(currentMethodConn->getTargetSignature().toLatin1()));
											if (!(nTargetID == -1))//Is the signal present?
											{
												bMetaConnection = QMetaObject::connect(pSourceObject, nSourceID, pTargetObject, nTargetID);
											}
										}
									}
								}
								else if (currentMethodConn->getTargetMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
								{
									if (currentMethodConn->getSourceMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
									{
										nSourceID = sourceMetaObject->indexOfSignal(QMetaObject::normalizedSignature(currentMethodConn->getSourceSignature().toLatin1()));
										if (!(nSourceID == -1))//Is the signal present?
										{
											nTargetID = targetMetaObject->indexOfMethod(QMetaObject::normalizedSignature(currentMethodConn->getTargetSignature().toLatin1()));
											if (!(nTargetID == -1))//Is the method present?
											{
												bMetaConnection = QMetaObject::connect(pSourceObject, nSourceID, pTargetObject, nTargetID);
												if (bMetaConnection == false)
													qDebug() << __FUNCTION__ << "Could not connect the objects (" << currentMethodConn->getSourceSignature() << " ," << currentMethodConn->getTargetSignature() << ")";
											}
										}
									}
								}
							}
							else
							{
								if ((nObjectID == -1) || ((currentMethodConn->getSourceObjectID() == nObjectID) || (currentMethodConn->getTargetObjectID() == nObjectID)))
								{
									if (currentMethodConn->getTargetMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
									{
										if (currentMethodConn->getSourceMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
										{
											nSourceID = sourceMetaObject->indexOfSignal(QMetaObject::normalizedSignature(currentMethodConn->getSourceSignature().toLatin1()));
											if (!(nSourceID == -1))//Is the signal present?
											{
												nTargetID = targetMetaObject->indexOfSignal(QMetaObject::normalizedSignature(currentMethodConn->getTargetSignature().toLatin1()));
												if (!(nTargetID == -1))//Is the signal present?
												{
													QMetaObject::disconnect(pSourceObject, nSourceID, pTargetObject, nTargetID);
												}
											}
										}
									}
									else if (currentMethodConn->getTargetMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
									{
										if (currentMethodConn->getSourceMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
										{
											nSourceID = sourceMetaObject->indexOfSignal(QMetaObject::normalizedSignature(currentMethodConn->getSourceSignature().toLatin1()));
											if (!(nSourceID == -1))//Is the signal present?
											{
												nTargetID = targetMetaObject->indexOfMethod(QMetaObject::normalizedSignature(currentMethodConn->getTargetSignature().toLatin1()));
												if (!(nTargetID == -1))//Is the method present?
												{
													QMetaObject::disconnect(pSourceObject, nSourceID, pTargetObject, nTargetID);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return true;
	}
	qDebug() << __FUNCTION__ << "::No Experiment loaded!";
	return false;
}

QObject *ExperimentManager::getObjectElementById(int nID)
{
	if (nID >= 0)
	{
		if (mExperimentObjectList.isEmpty())
			return NULL;
		if (mExperimentObjectList.contains(nID))
		{
			return mExperimentObjectList.value(nID).pObject;
		}
	}
	return NULL;
}

tParsedParameterList *ExperimentManager::getObjectBlockParamListById(int nID)
{
	if (nID >= 0)
	{
		if (mExperimentObjectList.isEmpty())
			return NULL;
		if (mExperimentObjectList.contains(nID))
		{
			return mExperimentObjectList.value(nID).ExpBlockParams;
		}
	}
	return NULL;
}

bool ExperimentManager::fetchExperimentBlockParameters(const int &nBlockNumber, const int &nObjectID)
{
	if (nObjectID >= 0)
	{
		if (mExperimentObjectList.isEmpty())
			return NULL;
		if (mExperimentObjectList.contains(nObjectID))
		{
			if(createExperimentBlockParamsFromExperimentStructure(nBlockNumber,nObjectID,mExperimentObjectList.value(nObjectID).ExpBlockParams) < 0)
			{
				qDebug() << __FUNCTION__ << "::Could not create a Block Parameter List!";
				return false;
			}

			QString sParamName;
			QHashIterator<QString, ParsedParameterDefinition> hashIter(*mExperimentObjectList.value(nObjectID).ExpBlockParams);
			while (hashIter.hasNext()) 
			{
				hashIter.next();
				if(hashIter.value().bIsCustom)
				{
					sParamName = hashIter.key();
					//bool bResult = 
						insertExperimentObjectVariabelePointer(nObjectID,sParamName,hashIter.value().sValue,true);							
				}
			}
			return true;
		}
	}
	qDebug() << __FUNCTION__ << "::Could not create a Block Parameter List, no current block or object is invalid!";
	return false;
}

int ExperimentManager::createExperimentBlockParamsFromExperimentStructure(const int &nBlockNumber, const int &nObjectID, tParsedParameterList *hParams)
{
	if (hParams == NULL)
		return -1;
	if(hParams->count() == 0)
		return -1;
	if(nObjectID < 0)
		return -1;
	if(nBlockNumber < 0)
		return -1;

	//Set all the parameter bHasChanged attributes too false again
	QString tmpString;
	QString tmpValue;
	QList<ParsedParameterDefinition> tmpStrValueList = hParams->values();//The order is guaranteed to be the same as that used by keys()!
	QList<QString> tmpStrKeyList = hParams->keys();//The order is guaranteed to be the same as that used by values()!
	for (int i = 0; i < tmpStrKeyList.count(); i++)
	{
		tmpStrValueList[i].bHasChanged = false;
		hParams->insert(tmpStrKeyList[i], tmpStrValueList[i]);
	}

	if (cExperimentBlockTrialStructure)
	{
		QList<cBlockStructure*> lBlockStruct = cExperimentBlockTrialStructure->getBlockList();
		if (lBlockStruct.count() > 0)
		{
			bool bIsCustom;
			bIsCustom = false;
			ParsedParameterDefinition tmpParDef;
			int nResult = 0;
			for (int nParamTypeCntr = 0; nParamTypeCntr < 2; nParamTypeCntr++)
			{
				if (nParamTypeCntr == 1)
					bIsCustom = true;
				for (int i = 0; i < lBlockStruct.count(); i++)
				{
					if (lBlockStruct[i]->getBlockNumber() == nBlockNumber)//Correct BlockNumber?
					{
						typeMapObjectsBlockParameterContainer *lObjectsParameterContainerStruct = lBlockStruct[i]->getParameterList(bIsCustom);
						if (lObjectsParameterContainerStruct)
						{
							if (lObjectsParameterContainerStruct->contains(nObjectID))//Do we have parameters for the Object(ID)?
							{
								//typeMapBlockParameterContainer *lParameterContainerStruct = &(*lObjectsParameterContainerStruct)[nObjectID];
								QMapIterator<QString, cBlockParameterStructure*> iterParamInner((*lObjectsParameterContainerStruct)[nObjectID]);//Iterate through all Parameters
								while (iterParamInner.hasNext())
								{
									iterParamInner.next();
									tmpString = iterParamInner.key().toLower();
									if (hParams->contains(tmpString))//Is the Parameter available in the predefined plug-in list?
									{
										if (iterParamInner.value())
										{
											tmpValue = iterParamInner.value()->getBlockParameterValue();//pSubTreeItem->getValue();
											expandExperimentBlockParameterValue(tmpValue);
											tmpParDef.sValue = tmpValue;
											tmpParDef.bHasChanged = true;
											hParams->insert(tmpString, tmpParDef);
											nResult++;
										}
									}
								}
							}
						}
					}
				}
			}
			return nResult;
		}
		else
		{
			return 0;
		}
	}
	return -1;
}

bool ExperimentManager::convertExperimentDataStructure(QList<ExperimentTreeItem*> *ExpRootTreeItems, cExperimentStructure *expStruct, ExperimentTreeModel *expTreeModel, QMap<int, objectElement> *mObjectElements, ExperimentManager *currentExpManager, const ExperimentManager::ExperimentDataStructureConversionType &conversionMethod)
{
	if (conversionMethod == EDS_TreeItemList_To_ExperimentStructure)
	{
		if (ExpRootTreeItems == NULL)
			return false;
		if (ExpRootTreeItems->count() < 1)
			return false;

		QStringList strSearchPathList;
		QList<ExperimentTreeItem*> lFoundExpTreeItems; 
		int nNrOfObjects;
		int nNrOfBlockTrials;
		int nNrOfMethodConnections;
		bool bExperimentDefinesParseResult = false;
		bool bBlockTrialParseResult = false;
		bool bObjectsParseResult = false;
		bool bMethodConnectionsResult = false;
		bool bFinitInitResult = false;
		ExperimentTreeItem *tmpTreeItem = NULL;
		QMap<QString, TreeItemDefinition> tTmpTreeItemDefs;

		expStruct->resetExperimentState();

		//Experiment Definitions//
		lFoundExpTreeItems.clear();
		strSearchPathList.clear();
		strSearchPathList.append(DEFINES_TAG);
		strSearchPathList.append(EXPERIMENT_TAG);
		if (ExperimentTreeModel::getStaticTreeElements(strSearchPathList, lFoundExpTreeItems, ExpRootTreeItems->at(0)) == 1)
		{
			if (lFoundExpTreeItems.at(0)->hasChildren())
			{
				tmpTreeItem = NULL;
				tTmpTreeItemDefs.clear();
				tmpTreeItem = lFoundExpTreeItems.at(0);
				tTmpTreeItemDefs = tmpTreeItem->getDefinitions();
				if (tTmpTreeItemDefs.contains(ID_TAG))
				{
					expStruct->setExperimentID(tTmpTreeItemDefs[ID_TAG].value.toInt());
					tmpTreeItem = lFoundExpTreeItems.at(0)->firstChild(NAME_TAG);
					if (tmpTreeItem)//Is there a experiment name defined?
					{
						expStruct->setExperimentName(tmpTreeItem->getValue());//Copy the Experiment Name
						tmpTreeItem = lFoundExpTreeItems.at(0)->firstChild(DEBUGMODE_TAG);
						if (tmpTreeItem)//Is there a Experiment DebugMode defined?
						{
							QString sTmpString = tmpTreeItem->getValue().toLower();
							if (sTmpString == TYPE_BOOL_FALSE)
								expStruct->setExperimentDebugMode(false);
							else
								expStruct->setExperimentDebugMode(true);
							bExperimentDefinesParseResult = true;
						}
					}
				}
			}
		}
		if (bExperimentDefinesParseResult == false)
		{
			expStruct->resetExperimentState();
			return false;
		}

		//Experiment Blocks, Parameters, Loops//
		lFoundExpTreeItems.clear();
		strSearchPathList.clear();
		strSearchPathList.append(ACTIONS_TAG);
		strSearchPathList.append(BLOCKTRIALS_TAG);
		if (ExperimentTreeModel::getStaticTreeElements(strSearchPathList, lFoundExpTreeItems, ExpRootTreeItems->at(0)) == 1)
		{
			strSearchPathList.append(BLOCK_TAG);
			lFoundExpTreeItems.clear();
			nNrOfBlockTrials = 0;
			if (ExperimentTreeModel::getStaticTreeElements(strSearchPathList, lFoundExpTreeItems, ExpRootTreeItems->at(0)) > 0)
			{
				nNrOfBlockTrials = lFoundExpTreeItems.count();
				cBlockStructure *tmpBlock = NULL;
				cLoopStructure *tmpLoop = NULL;
				tmpTreeItem = NULL;
				bool bUpdateSucceeded = false;
				tTmpTreeItemDefs.clear();
				int nTemp;
				QString sTemp;

				for (int i = 0; i < nNrOfBlockTrials; i++)
				{
					bUpdateSucceeded = false;
					if (lFoundExpTreeItems.at(i)->hasChildren())
					{
						tmpTreeItem = lFoundExpTreeItems.at(i);
						tTmpTreeItemDefs = tmpTreeItem->getDefinitions();
						if (tTmpTreeItemDefs.contains(ID_TAG))
						{
							tmpBlock = new cBlockStructure();
							tmpBlock->setBlockID(tTmpTreeItemDefs[ID_TAG].value.toInt());//Copy the BlockID
							tmpTreeItem = lFoundExpTreeItems.at(i)->firstChild(BLOCKNUMBER_TAG);
							if (tmpTreeItem)//Is there a block_number defined?
							{
								tmpBlock->setBlockNumber(tmpTreeItem->getValue().toInt());//Copy the BlockNumber
								if (tmpBlock->getBlockNumber() >= 0)
								{
									tmpTreeItem = lFoundExpTreeItems.at(i)->firstChild(TRIALAMOUNT_TAG);
									if (tmpTreeItem)//Is there a TrialAmount defined?
										tmpBlock->setNumberOfTrials(tmpTreeItem->getValue().toInt());//Copy the TrialAmount
									else
										tmpBlock->setNumberOfTrials(1);//Default if not defined
									tmpTreeItem = lFoundExpTreeItems.at(i)->firstChild(INTERNALTRIGGERAMOUNT_TAG);
									if (tmpTreeItem)//Is there a TriggerAmount defined?
									{
										sTemp = tmpTreeItem->getValue();
										if (!sTemp.isEmpty())
										{
											nTemp = sTemp.toInt();
											if (nTemp > 0)
												tmpBlock->setNumberOfInternalTriggers(nTemp);
										}
									}
									tmpTreeItem = lFoundExpTreeItems.at(i)->firstChild(EXTERNALTRIGGERAMOUNT_TAG);
									if (tmpTreeItem)//Is there a SubTriggerAmount defined?
									{
										sTemp = tmpTreeItem->getValue();
										if (!sTemp.isEmpty())
										{
											nTemp = sTemp.toInt();
											if (nTemp > 0)
												tmpBlock->setNumberOfExternalTriggers(nTemp);
										}
									}
									tmpTreeItem = lFoundExpTreeItems.at(i)->firstChild(NAME_TAG);
									if (tmpTreeItem)//Is there a Name defined?
										tmpBlock->setBlockName(tmpTreeItem->getValue());//Copy the Name


									if ((mObjectElements) && (currentExpManager))//only when provided we need to fill the object block parameters
									{
										//////////////////////////////////////////////////////////////////////////
										//Let's parse the custom and non-custom block parameters for each object
										//////////////////////////////////////////////////////////////////////////
										for (int nParameterTypeCntr = 0; nParameterTypeCntr < 2; nParameterTypeCntr++)
										{
											bool bIsCustomParameter = false;
											if (nParameterTypeCntr == 1)
												bIsCustomParameter = true;

											QList<ExperimentTreeItem*> tmpObjectBlockParameterTreeItemList;
											int nObjectListCount = ExperimentTreeModel::getStaticTreeElements(QStringList() << OBJECT_TAG, tmpObjectBlockParameterTreeItemList, lFoundExpTreeItems.at(i));
											if (nObjectListCount > 0)
											{
												QMap<QString, TreeItemDefinition> tTmpTreeObjectItemDefs;
												int nCurrentObjectID;
												for (int j = 0; j < nObjectListCount; j++)//For each object
												{
													nCurrentObjectID = -1;
													tTmpTreeObjectItemDefs = tmpObjectBlockParameterTreeItemList.at(j)->getDefinitions();
													if (tTmpTreeObjectItemDefs.contains(ID_TAG))
													{
														nCurrentObjectID = tTmpTreeObjectItemDefs[ID_TAG].value.toInt();//ObjectID
														if (bIsCustomParameter)
															tmpTreeItem = tmpObjectBlockParameterTreeItemList.at(j)->firstChild(CUSTOM_PARAMETERS_TAG);//j loops through available blocks
														else
															tmpTreeItem = tmpObjectBlockParameterTreeItemList.at(j)->firstChild(PARAMETERS_TAG);//j loops through available blocks
														if (tmpTreeItem)
														{
															QString tmpString;
															QStringList lParameterSearchPath;
															if (bIsCustomParameter)
																lParameterSearchPath << OBJECT_TAG << CUSTOM_PARAMETERS_TAG << PARAMETER_TAG;
															else
																lParameterSearchPath << OBJECT_TAG << PARAMETERS_TAG << PARAMETER_TAG;
															QList<ExperimentTreeItem*> tmpBlockParameterTreeItemList;
															int nParameterListCount = ExperimentTreeModel::getStaticTreeElements(lParameterSearchPath, tmpBlockParameterTreeItemList, tmpObjectBlockParameterTreeItemList.at(j));
															tParsedParameterList *hParams = NULL;
															if (nParameterListCount > 0)
															{
																int nCurrentParamID;
																for (int k = 0; k < nParameterListCount; k++)//For each parameter
																{
																	nCurrentParamID = -1;
																	tTmpTreeObjectItemDefs = tmpBlockParameterTreeItemList.at(k)->getDefinitions();
																	if (tTmpTreeObjectItemDefs.contains(ID_TAG))
																	{
																		nCurrentParamID = tTmpTreeObjectItemDefs[ID_TAG].value.toInt();//ParamID
																		tmpTreeItem = tmpBlockParameterTreeItemList.at(k)->firstChild(NAME_TAG);
																		if (tmpTreeItem)
																		{
																			tmpString = tmpTreeItem->getValue().toLower();
																			if (mObjectElements->contains(nCurrentObjectID) == false)
																			{
																				objectElement tmpObjElement;
																				tmpObjElement.nObjectID = nCurrentObjectID;
																				mObjectElements->insert(nCurrentObjectID, tmpObjElement);
																			}
																			if (mObjectElements->value(nCurrentObjectID).ExpBlockParams == NULL)
																				(*mObjectElements)[nCurrentObjectID].ExpBlockParams = new tParsedParameterList();
																			hParams = mObjectElements->value(nCurrentObjectID).ExpBlockParams;
																			tmpTreeItem = tmpBlockParameterTreeItemList.at(k)->firstChild(VALUE_TAG);
																			if (tmpTreeItem)
																			{
																				ParsedParameterDefinition tmpParDef;
																				QString tmpValue = tmpTreeItem->getValue();

																				cBlockParameterStructure *tmpBlockParameter = new cBlockParameterStructure();
																				tmpBlockParameter->setBlockParameterID(nCurrentParamID);
																				tmpBlockParameter->setBlockParameterName(tmpString);
																				tmpBlockParameter->setBlockParameterValue(tmpValue);
																				if (tmpBlock->insertObjectParameter(nCurrentObjectID, tmpBlockParameter, bIsCustomParameter))
																				{
																					currentExpManager->expandExperimentBlockParameterValue(tmpValue);
																					tmpParDef.sValue = tmpValue;
																					tmpParDef.bHasChanged = true;
																					tmpParDef.bIsCustom = bIsCustomParameter;
																					hParams->insert(tmpString, tmpParDef);
																				}
																			}
																		}
																	}
																}
															}
														}
													}
												}
											}
										}
									}
									
									//Are there any loops defined?
									ExperimentTreeItem *tmpLoopItem = lFoundExpTreeItems.at(i)->firstChild(LOOPS_TAG);
									if (tmpLoopItem)
									{
										if (tmpLoopItem->childCount() > 0)
										{
											int nTempLoopID = 0;
											QString sTempLoopName = "";
											int nTempLoopNumber = 0;
											int nTempLoopCount = 0;
											int nTempLoopTargetBlockID = 0;
											ExperimentTreeItem *tmpLoopTreeItemParams = NULL;
											bool bLoopParseResult = true;

											tmpTreeItem = tmpLoopItem->firstChild(LOOP_TAG);
											while (tmpTreeItem)
											{
												tTmpTreeItemDefs = tmpTreeItem->getDefinitions();
												if (tTmpTreeItemDefs.contains(ID_TAG))
												{
													if (tmpTreeItem->getName() == LOOP_TAG)
													{
														bLoopParseResult = false;
														nTempLoopID = tTmpTreeItemDefs[ID_TAG].value.toInt();//Copy the LoopID
														tmpLoopTreeItemParams = tmpTreeItem->firstChild(NAME_TAG);
														if (tmpLoopTreeItemParams)//Is it defined?
														{
															sTempLoopName = tmpLoopTreeItemParams->getValue();//Copy the LoopName
															tmpLoopTreeItemParams = tmpTreeItem->firstChild(LOOP_NUMBER_TAG);
															if (tmpLoopTreeItemParams)//Is it defined?
															{
																nTempLoopNumber = tmpLoopTreeItemParams->getValue().toInt();//Copy the LoopNumber
																tmpLoopTreeItemParams = tmpTreeItem->firstChild(LOOP_AMOUNT_TAG);
																if (tmpLoopTreeItemParams)//Is it defined?
																{
																	nTempLoopCount = tmpLoopTreeItemParams->getValue().toInt();//Copy the LoopNumber
																	tmpLoopTreeItemParams = tmpTreeItem->firstChild(LOOP_TARGETBLOCKID_TAG);
																	if (tmpLoopTreeItemParams)//Is it defined?
																	{
																		nTempLoopTargetBlockID = tmpLoopTreeItemParams->getValue().toInt();//Copy the LoopNumber
																		tmpLoop = new cLoopStructure(nTempLoopID, nTempLoopNumber, nTempLoopTargetBlockID, sTempLoopName, nTempLoopCount);
																		bLoopParseResult = tmpBlock->insertLoop(tmpLoop);////&cLoopStructure(nTempLoopID,nTempLoopNumber,nTempLoopTargetBlockID,sTempLoopName,nTempLoopCount));//tmpLoop);
																	}
																}
															}
														}
													}
													if (bLoopParseResult == false)
														qDebug() << __FUNCTION__ << "::Could not parse loop structure, false declared loop!";
													tmpTreeItem = tmpTreeItem->nextSiblingTreeItem();
												}
											}
										}
									}
									expStruct->insertBlock(tmpBlock);//Here we should make a copy to reserve and keep the memory space
									bUpdateSucceeded = true;
								}
							}
						}
					}
					if (!bUpdateSucceeded)
					{
						expStruct->resetExperimentState();
						return false;
					}
				}
			}
			bBlockTrialParseResult = true;
		}

		if (bBlockTrialParseResult)
		{
			lFoundExpTreeItems.clear();
			strSearchPathList.clear();
			strSearchPathList.append(DECLARATIONS_TAG);
			strSearchPathList.append(OBJECT_TAG);

			if (ExperimentTreeModel::getStaticTreeElements(strSearchPathList, lFoundExpTreeItems, ExpRootTreeItems->at(0)) > 0)
			{
				nNrOfObjects = lFoundExpTreeItems.count();
				cObjectStructure *tmpObject = NULL;
				tmpTreeItem = NULL;
				bool bUpdateSucceeded = false;
				tTmpTreeItemDefs.clear();

				for (int i = 0; i < nNrOfObjects; i++)
				{
					bUpdateSucceeded = false;
					if (lFoundExpTreeItems.at(i)->hasChildren())
					{
						tmpTreeItem = lFoundExpTreeItems.at(i);
						tTmpTreeItemDefs = tmpTreeItem->getDefinitions();
						if (tTmpTreeItemDefs.contains(ID_TAG))
						{
							tmpObject = new cObjectStructure();
							tmpObject->setObjectID(tTmpTreeItemDefs[ID_TAG].value.toInt());//Copy the ObjectID
							tmpTreeItem = lFoundExpTreeItems.at(i)->firstChild(NAME_TAG);
							if (tmpTreeItem)//Is there a name defined?
							{
								tmpObject->setObjectName(tmpTreeItem->getValue());//Copy the Object Name
								tmpTreeItem = lFoundExpTreeItems.at(i)->firstChild(CLASS_TAG);
								if (tmpTreeItem)//Is there a Class Name defined?
								{
									tmpObject->setObjectClass(tmpTreeItem->getValue());//Copy the Class Name
									expStruct->insertObject(tmpObject);//Here we should make a copy to reserve and keep the memory space
									bUpdateSucceeded = true;
								}
							}
						}
					}
					if (!bUpdateSucceeded)
					{
						expStruct->resetExperimentState();
						return false;
					}
				}
				bObjectsParseResult = true;
			}
			else
			{
				bObjectsParseResult = true;
			}
			if (bObjectsParseResult == false)
				return false;
		}

		//Connections//
		if (bObjectsParseResult)
		{
			lFoundExpTreeItems.clear();
			strSearchPathList.clear();
			strSearchPathList.append(CONNECTIONS_TAG);
			strSearchPathList.append(CONNECTION_TAG);

			if (ExperimentTreeModel::getStaticTreeElements(strSearchPathList, lFoundExpTreeItems, ExpRootTreeItems->at(0)) > 0)
			{
				nNrOfMethodConnections = lFoundExpTreeItems.count();
				cMethodConnectionStructure *tmpMethod = NULL;
				tmpTreeItem = NULL;
				bool bUpdateSucceeded = false;
				tTmpTreeItemDefs.clear();

				for (int i = 0; i < nNrOfMethodConnections; i++)
				{
					bUpdateSucceeded = false;
					if (lFoundExpTreeItems.at(i)->hasChildren())
					{
						tmpTreeItem = lFoundExpTreeItems.at(i);
						tTmpTreeItemDefs = tmpTreeItem->getDefinitions();
						if (tTmpTreeItemDefs.contains(ID_TAG))
						{
							tmpMethod = new cMethodConnectionStructure();
							tmpMethod->setMethodConnectionID(tTmpTreeItemDefs[ID_TAG].value.toInt());//Copy the Method Connection ID
							for (int nSourceTargetCounter = 0; nSourceTargetCounter < 2; nSourceTargetCounter++)
							{
								ExperimentTreeItem *pSelectedExperimentTreeItem = NULL;
								if (nSourceTargetCounter == 0)//Source section...
									pSelectedExperimentTreeItem = lFoundExpTreeItems.at(i)->firstChild(CONNECTION_SOURCE_TAG);
								else if (nSourceTargetCounter == 1)//Target section...
									pSelectedExperimentTreeItem = lFoundExpTreeItems.at(i)->firstChild(CONNECTION_TARGET_TAG);
								if (pSelectedExperimentTreeItem)
								{
									ExperimentTreeItem *tmpObjectIDTreeItem = pSelectedExperimentTreeItem->firstChild(OBJECT_TAG);
									if (tmpObjectIDTreeItem)
									{
										if (nSourceTargetCounter == 0)//Source section...
											tmpMethod->setSourceObjectID(tmpObjectIDTreeItem->getValue().toInt());//Copy the Source Object ID
										else if (nSourceTargetCounter == 1)//Target section...
											tmpMethod->setTargetObjectID(tmpObjectIDTreeItem->getValue().toInt());//Copy the Target Object ID

										tmpTreeItem = pSelectedExperimentTreeItem->firstChild(CONNECTION_TYPE_TAG);
										if (tmpTreeItem)//Is there a Object connection type defined?
										{
											int tmpIntVal = cMethodStructure::methodTypeStringToInteger(tmpTreeItem->getValue());
											if (tmpIntVal > 0)
											{
												if (nSourceTargetCounter == 0)//Source section...
													tmpMethod->setSourceMethodType(tmpIntVal);//Copy the Source Object Method Type
												else if (nSourceTargetCounter == 1)//Target section...
													tmpMethod->setTargetMethodType(tmpIntVal);//Copy the Target Object Method Type

												tmpTreeItem = pSelectedExperimentTreeItem->firstChild(CONNECTION_SIGNATURE_TAG);
												if (tmpTreeItem)//Is there a Target Signature defined?
												{
													if (nSourceTargetCounter == 0)//Source section...
														tmpMethod->setSourceSignature(tmpTreeItem->getValue());//Copy the Source Object Signature
													else if (nSourceTargetCounter == 1)//Target section...
														tmpMethod->setTargetSignature(tmpTreeItem->getValue());//Copy the Target Object Signature
													bUpdateSucceeded = expStruct->insertObjectMethodConnection(tmpMethod);//Here we should make a copy to reserve and keep the memory space
												}
											}
										}
									}
								}
							}
						}
					}
					if (!bUpdateSucceeded)
					{
						expStruct->resetExperimentState();
						return false;
					}
				}
				bMethodConnectionsResult = true;
			}
			else
			{
				bMethodConnectionsResult = true;
			}

			for (int nInitFinitSelector = 0; nInitFinitSelector < 2; nInitFinitSelector++)
			{
				//Initializations and Finalizations//
				lFoundExpTreeItems.clear();
				strSearchPathList.clear();
				if (nInitFinitSelector == 0)//Initializations
				{
					strSearchPathList.append(INITIALIZATIONS_TAG);
					strSearchPathList.append(INITIALIZATION_TAG);
				}
				else if (nInitFinitSelector == 1)//Finalizations
				{
					strSearchPathList.append(FINALIZATIONS_TAG);
					strSearchPathList.append(FINALIZATION_TAG);
				}

				if (ExperimentTreeModel::getStaticTreeElements(strSearchPathList, lFoundExpTreeItems, ExpRootTreeItems->at(0)) > 0)
				{
					int nNrOfItems = lFoundExpTreeItems.count();
					cMethodStructure *tmpMethod = NULL;
					tmpTreeItem = NULL;
					bool bUpdateSucceeded = false;
					tTmpTreeItemDefs.clear();

					for (int i = 0; i < nNrOfItems; i++)
					{
						bUpdateSucceeded = false;
						if (lFoundExpTreeItems.at(i)->hasChildren())
						{
							tmpTreeItem = lFoundExpTreeItems.at(i);
							tTmpTreeItemDefs = tmpTreeItem->getDefinitions();
							if (tTmpTreeItemDefs.contains(ID_TAG))
							{
								tmpMethod = new cMethodStructure();
								tmpMethod->setMethodID(tTmpTreeItemDefs[ID_TAG].value.toInt());//Copy the Method ID

								ExperimentTreeItem *pSelectedExperimentTreeItem = NULL;
								int nMethodOrderNumber = -1;
								pSelectedExperimentTreeItem = lFoundExpTreeItems.at(i)->firstChild(INIT_FINIT_NUMBER_TAG);
								if (pSelectedExperimentTreeItem)
								{
									bool bConversionResult = false;
									nMethodOrderNumber = pSelectedExperimentTreeItem->getValue().toInt(&bConversionResult);
									if (bConversionResult == false)
										nMethodOrderNumber = -1;
								}
								pSelectedExperimentTreeItem = lFoundExpTreeItems.at(i)->firstChild(OBJECT_TAG);
								if (pSelectedExperimentTreeItem && (nMethodOrderNumber >= 0))
								{
									tTmpTreeItemDefs = pSelectedExperimentTreeItem->getDefinitions();
									if (tTmpTreeItemDefs.contains(ID_TAG))
									{
										tmpMethod->setMethodObjectID(tTmpTreeItemDefs[ID_TAG].value.toInt());//Copy the Object ID
										tmpMethod->setMethodOrderNumber(nMethodOrderNumber);
										tmpTreeItem = pSelectedExperimentTreeItem->firstChild(INIT_FINIT_TYPE_TAG);
										if (tmpTreeItem)//Is there a Object method type defined?
										{
											int tmpIntVal = cMethodStructure::methodTypeStringToInteger(tmpTreeItem->getValue());
											if (tmpIntVal > 0)
											{
												tmpMethod->setMethodType(tmpIntVal);//Copy the Source Object Method Type
												tmpTreeItem = pSelectedExperimentTreeItem->firstChild(INIT_FINIT_SIGNATURE_TAG);
												if (tmpTreeItem)//Is there a Method Signature defined?
												{
													tmpMethod->setMethodSignature(tmpTreeItem->getValue());//Copy the Object Method Signature
													tmpTreeItem = pSelectedExperimentTreeItem->firstChild(PARAMETERS_TAG);
													if (tmpTreeItem)//Is there a Parameters Section defined?
													{
														if (tmpTreeItem->hasChildren())//Are there parameters defined?
														{
															for (int nChildCount = 0; nChildCount < tmpTreeItem->childCount(); nChildCount++)
															{
																if (tmpTreeItem->child(nChildCount)->getName().toLower() == PARAMETER_TAG)
																{
																	tTmpTreeItemDefs = tmpTreeItem->child(nChildCount)->getDefinitions();
																	if (tTmpTreeItemDefs.contains(ID_TAG))
																	{
																		int nParamID = tTmpTreeItemDefs[ID_TAG].value.toInt();//Copy the Parameter ID

																		if (tmpTreeItem->child(nChildCount)->hasChildren())
																		{
																			QString sParamName = "";
																			QString sParamValue = "";
																			QString sParamType = "";
																			ExperimentTreeItem *pTmpExpTreeSubParamItem = tmpTreeItem->child(nChildCount)->firstChild(NAME_TAG);
																			if (pTmpExpTreeSubParamItem)
																			{
																				sParamName = pTmpExpTreeSubParamItem->getValue();
																				if (sParamName.isEmpty() == false)
																				{
																					pTmpExpTreeSubParamItem = tmpTreeItem->child(nChildCount)->firstChild(VALUE_TAG);
																					if (pTmpExpTreeSubParamItem)
																					{
																						sParamValue = pTmpExpTreeSubParamItem->getValue();
																						//if(sParamValue.isEmpty() == false)
																						//{
																						pTmpExpTreeSubParamItem = tmpTreeItem->child(nChildCount)->firstChild(MEMBER_TYPE_TAG);
																						if (pTmpExpTreeSubParamItem)
																						{
																							sParamType = pTmpExpTreeSubParamItem->getValue();
																							if (sParamType.isEmpty() == false)
																							{
																								cMethodParameterStructure *pTmpMethParamStruct = new cMethodParameterStructure();
																								//Add newly defined arguments
																								pTmpMethParamStruct->setMethodParameterID(nParamID);
																								pTmpMethParamStruct->setMethodParameterName(sParamName);
																								pTmpMethParamStruct->setMethodParameterType(sParamType);
																								pTmpMethParamStruct->setMethodParameterValue(sParamValue);
																								tmpMethod->insertMethodParameter(pTmpMethParamStruct);
																							}
																						}
																						//}
																					}
																				}
																			}
																		}
																	}
																}
															}
														}
													}
													if (nInitFinitSelector == 0)//Initializations
														bUpdateSucceeded = expStruct->insertObjectInitialization(tmpMethod);//Here we should make a copy to reserve and keep the memory space
													else if (nInitFinitSelector == 1)//Finalizations
														bUpdateSucceeded = expStruct->insertObjectFinalization(tmpMethod);//Here we should make a copy to reserve and keep the memory space
												}
											}
										}
									}
								}
							}
						}
						if (!bUpdateSucceeded)
						{
							expStruct->resetExperimentState();
							return false;
						}
					}
				}
			}
			bFinitInitResult = true;
			return bFinitInitResult;
		}
	}
	else if (conversionMethod == EDS_ExperimentStructure_To_ExperimentTreeModel)
	{
		if (expStruct == NULL)
			return false;
		if (expTreeModel == NULL)
			return false;

		expTreeModel->enableModifiedSignaling(false);
		QList<ExperimentTreeItem*> tmpExpTreeItemList;
		QMap<QString, TreeItemDefinition> tTmpTreeItemDefs;
		TreeItemDefinition tmpExpTreeDef;
		QStringList tmpStringList;
		QString tmpString;
		ExperimentTreeItem *tmpExpTreeItem_0 = NULL;
		ExperimentTreeItem *tmpExpTreeItem_1 = NULL;
		int nLoopCounter_0 = 0;
		int nLoopCounter_1 = 0;
		int nLoopCounter_2 = 0;

		//Experiment Definitions//
		tmpStringList.clear();
		tmpStringList.append(ROOT_TAG);
		tmpStringList.append(DEFINES_TAG);
		tmpStringList.append(EXPERIMENT_TAG);
		if (expTreeModel->getTreeElements(tmpStringList, tmpExpTreeItemList) == 1)
		{
			tmpExpTreeItem_0 = tmpExpTreeItemList.at(0);
			if (tmpExpTreeItem_0)
			{
				//Experiment ID
				tTmpTreeItemDefs = tmpExpTreeItem_0->getDefinitions();
				tmpExpTreeDef.type = TreeItemType_Attribute;
				tmpExpTreeDef.value = expStruct->getExperimentID();
				tTmpTreeItemDefs.insert(ID_TAG, tmpExpTreeDef);
				tmpExpTreeItem_0->setDefinitions(tTmpTreeItemDefs);
				//Experiment Name
				tmpExpTreeItem_1 = tmpExpTreeItem_0->firstChild(NAME_TAG);
				if (tmpExpTreeItem_1)
					tmpExpTreeItem_1->setValue(expStruct->getExperimentName());
				//Experiment DebugMode
				tmpExpTreeItem_1 = tmpExpTreeItem_0->firstChild(DEBUGMODE_TAG);
				if (tmpExpTreeItem_1)
				{
					if(expStruct->getExperimentDebugMode())
						tmpExpTreeItem_1->setValue(TYPE_BOOL_TRUE);
					else
						tmpExpTreeItem_1->setValue(TYPE_BOOL_FALSE);
				}
			}
			else
			{
				expTreeModel->enableModifiedSignaling(true);
				return false;
			}
		}

		//Experiment Object Declarations, Initializations, Finalizations, Connections
		QList<cObjectStructure *> lObjects = expStruct->getObjectList();
		if (lObjects.isEmpty() == false)
		{
			for (nLoopCounter_0 = 0; nLoopCounter_0 < lObjects.count(); nLoopCounter_0++)
			{
				if (expTreeModel->addExperimentObject(lObjects.at(nLoopCounter_0)->getObjectName(), lObjects.at(nLoopCounter_0)->getObjectClass(), lObjects.at(nLoopCounter_0)->getObjectID()))
				{
					//Object Initializations
					ExperimentTreeModel::strcObjectInitFinitSpecifier sObjectInitFinitSpecifier;
					QList<cMethodStructure*> *lInitFinitMethods = expStruct->getObjectInitializationList(lObjects.at(nLoopCounter_0)->getObjectID());
					if (lInitFinitMethods)
					{
						for (nLoopCounter_1 = 0; nLoopCounter_1 < lInitFinitMethods->count(); nLoopCounter_1++)
						{
							sObjectInitFinitSpecifier.nInitFinitID = lInitFinitMethods->at(nLoopCounter_1)->getMethodID();
							sObjectInitFinitSpecifier.nObjectID = lInitFinitMethods->at(nLoopCounter_1)->getMethodObjectID();
							sObjectInitFinitSpecifier.nOrderNumber = lInitFinitMethods->at(nLoopCounter_1)->getMethodOrderNumber();
							//sObjectInitFinitSpecifier.sObjectDefinitionName = lInitMethods->at(nLoopCounter_1)->
							sObjectInitFinitSpecifier.sSignature = lInitFinitMethods->at(nLoopCounter_1)->getMethodSignature();
							if (lInitFinitMethods->at(nLoopCounter_1)->getMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
								sObjectInitFinitSpecifier.sType = METHOD_TYPE_SLOT_TAG;
							else
								sObjectInitFinitSpecifier.sType = METHOD_TYPE_SLOT_TAG;
							QList<cMethodParameterStructure*> lMethodParams = lInitFinitMethods->at(nLoopCounter_1)->getMethodParameterList();
							ExperimentTreeModel::strcObjectInitFinitParameterSpecifier objInitFinitParamSpec;
							for (nLoopCounter_2 = 0; nLoopCounter_2 < lMethodParams.count(); nLoopCounter_2++)
							{
								objInitFinitParamSpec.nParamID = lMethodParams.at(nLoopCounter_2)->getMethodParameterID();
								objInitFinitParamSpec.sName = lMethodParams.at(nLoopCounter_2)->getMethodParameterName();
								objInitFinitParamSpec.sType = lMethodParams.at(nLoopCounter_2)->getMethodParameterType();
								objInitFinitParamSpec.sValue = lMethodParams.at(nLoopCounter_2)->getMethodParameterValue();
								sObjectInitFinitSpecifier.lParameters.append(objInitFinitParamSpec);
							}
							if (expTreeModel->addExperimentObjectInitFinit(sObjectInitFinitSpecifier, true, sObjectInitFinitSpecifier.nInitFinitID) == false)
							{
								expTreeModel->enableModifiedSignaling(true);
								expTreeModel->resetExperiment();
								return false;
							}
						}
					}

					//Object Finalizations
					lInitFinitMethods = expStruct->getObjectFinalizationList(lObjects.at(nLoopCounter_0)->getObjectID());
					if (lInitFinitMethods)
					{
						for (nLoopCounter_1 = 0; nLoopCounter_1 < lInitFinitMethods->count(); nLoopCounter_1++)
						{
							sObjectInitFinitSpecifier.nInitFinitID = lInitFinitMethods->at(nLoopCounter_1)->getMethodID();
							sObjectInitFinitSpecifier.nObjectID = lInitFinitMethods->at(nLoopCounter_1)->getMethodObjectID();
							sObjectInitFinitSpecifier.nOrderNumber = lInitFinitMethods->at(nLoopCounter_1)->getMethodOrderNumber();
							//sObjectInitFinitSpecifier.sObjectDefinitionName = lInitMethods->at(nLoopCounter_1)->
							sObjectInitFinitSpecifier.sSignature = lInitFinitMethods->at(nLoopCounter_1)->getMethodSignature();
							if (lInitFinitMethods->at(nLoopCounter_1)->getMethodType() == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
								sObjectInitFinitSpecifier.sType = METHOD_TYPE_SLOT_TAG;
							else
								sObjectInitFinitSpecifier.sType = METHOD_TYPE_SLOT_TAG;
							QList<cMethodParameterStructure*> lMethodParams = lInitFinitMethods->at(nLoopCounter_1)->getMethodParameterList();
							ExperimentTreeModel::strcObjectInitFinitParameterSpecifier objInitFinitParamSpec;
							sObjectInitFinitSpecifier.lParameters.clear();
							for (nLoopCounter_2 = 0; nLoopCounter_2 < lMethodParams.count(); nLoopCounter_2++)
							{
								objInitFinitParamSpec.nParamID = lMethodParams.at(nLoopCounter_2)->getMethodParameterID();
								objInitFinitParamSpec.sName = lMethodParams.at(nLoopCounter_2)->getMethodParameterName();
								objInitFinitParamSpec.sType = lMethodParams.at(nLoopCounter_2)->getMethodParameterType();
								objInitFinitParamSpec.sValue = lMethodParams.at(nLoopCounter_2)->getMethodParameterValue();
								sObjectInitFinitSpecifier.lParameters.append(objInitFinitParamSpec);
							}
							if (expTreeModel->addExperimentObjectInitFinit(sObjectInitFinitSpecifier, false, sObjectInitFinitSpecifier.nInitFinitID)==false)
							{
								expTreeModel->enableModifiedSignaling(true);
								expTreeModel->resetExperiment();
								return false;
							}
						}
					}
				}
				else
				{
					expTreeModel->enableModifiedSignaling(true);
					expTreeModel->resetExperiment();
					return false;
				}
			}

			//Experiment Object Connections
			for (nLoopCounter_0 = 0; nLoopCounter_0 < lObjects.count(); nLoopCounter_0++)
			{
				QList<cMethodConnectionStructure*> *lMethodConns = expStruct->getObjectMethodConnectionList(lObjects.at(nLoopCounter_0)->getObjectID());
				if (lMethodConns)
				{
					if (lMethodConns->isEmpty() == false)
					{
						ExperimentTreeModel::strcObjectConnectionSpecifier sObjectConnSpec;
						for (nLoopCounter_1 = 0; nLoopCounter_1 < lMethodConns->count(); nLoopCounter_1++)
						{
							sObjectConnSpec.mSourceMethodType = (ExperimentStructuresNameSpace::MethodType)lMethodConns->at(nLoopCounter_1)->getSourceMethodType();
							sObjectConnSpec.mTargetMethodType = (ExperimentStructuresNameSpace::MethodType)lMethodConns->at(nLoopCounter_1)->getTargetMethodType();
							sObjectConnSpec.nConnectionID = lMethodConns->at(nLoopCounter_1)->getMethodConnectionID();
							sObjectConnSpec.nSourceObjectID = lMethodConns->at(nLoopCounter_1)->getSourceObjectID();
							sObjectConnSpec.nTargetObjectID = lMethodConns->at(nLoopCounter_1)->getTargetObjectID();
							//sObjectConnSpec.sConnectionDefinitionName = 
							sObjectConnSpec.sSourceSignature = lMethodConns->at(nLoopCounter_1)->getSourceSignature();
							sObjectConnSpec.sTargetSignature = lMethodConns->at(nLoopCounter_1)->getTargetSignature();
							if (expTreeModel->addObjectConnection(sObjectConnSpec, sObjectConnSpec.nConnectionID) == false)
							{
								expTreeModel->enableModifiedSignaling(true);
								expTreeModel->resetExperiment();
								return false;
							}
						}
					}
				}
			}
		}

		//Experiment Blocks//
		QList<cBlockStructure*> lDefinedBlocks = expStruct->getBlockList();
		int nExpStruct_BlockCount = lDefinedBlocks.count();
		if (nExpStruct_BlockCount > 0)
		{
			cBlockStructure *tmpBlockStructure = NULL;
			for (nLoopCounter_0 = 0; nLoopCounter_0 < nExpStruct_BlockCount; nLoopCounter_0++)
			{
				tmpExpTreeItem_0 = expTreeModel->addExperimentBlockTreeItems(1);//expTreeModel->getExperimentBlockTreeItem(nLoopCounter_0);//selected by ID!!      expTreeModel->addExperimentBlockTreeItems(1);
				if (tmpExpTreeItem_0)
				{
					tmpBlockStructure = lDefinedBlocks[nLoopCounter_0];// expStruct->getBlockPointerByID(nLoopCounter_0);//selected by ID!!    //getNextClosestBlockNumberByFromNumber(nNextBlockNumber);
					if (tmpBlockStructure)
					{
						//BlockID
						tmpExpTreeDef.type = TreeItemType_Attribute;
						tmpExpTreeDef.value = QString::number(tmpBlockStructure->getBlockID());
						tTmpTreeItemDefs = tmpExpTreeItem_0->getDefinitions();
						tTmpTreeItemDefs.insert(QString(ID_TAG), tmpExpTreeDef);
						tmpExpTreeItem_0->setDefinitions(tTmpTreeItemDefs);
						//BlockName
						tmpExpTreeItem_1 = tmpExpTreeItem_0->firstChild(NAME_TAG);
						if (tmpExpTreeItem_1)
							tmpExpTreeItem_1->setValue(tmpBlockStructure->getBlockName());
						//BlockNumber
						tmpExpTreeItem_1 = tmpExpTreeItem_0->firstChild(BLOCKNUMBER_TAG);
						if (tmpExpTreeItem_1)
							tmpExpTreeItem_1->setValue(QString::number(tmpBlockStructure->getBlockNumber()));
						//NumberOfTrials
						tmpExpTreeItem_1 = tmpExpTreeItem_0->firstChild(TRIALAMOUNT_TAG);
						if (tmpExpTreeItem_1)
							tmpExpTreeItem_1->setValue(QString::number(tmpBlockStructure->getNumberOfTrials()));
						//NumberOfInternalTriggers
						tmpExpTreeItem_1 = tmpExpTreeItem_0->firstChild(INTERNALTRIGGERAMOUNT_TAG);
						if (tmpExpTreeItem_1)
							tmpExpTreeItem_1->setValue(QString::number(tmpBlockStructure->getNumberOfInternalTriggers()));
						//NumberOfExternalTriggers
						tmpExpTreeItem_1 = tmpExpTreeItem_0->firstChild(EXTERNALTRIGGERAMOUNT_TAG);
						if (tmpExpTreeItem_1)
							tmpExpTreeItem_1->setValue(QString::number(tmpBlockStructure->getNumberOfExternalTriggers()));

						//Experiment Block Parameters (non-custom and custom)
						bool bIsCustom;
						for (int nParamTypeSwitcher = 0; nParamTypeSwitcher < 2; nParamTypeSwitcher++)
						{
							bIsCustom = (nParamTypeSwitcher == 1);
							typeMapObjectsBlockParameterContainer *lBlockParams = tmpBlockStructure->getParameterList(bIsCustom);
							if (lBlockParams)
							{
								ExperimentTreeModel::strcParameterSpecifier cParameterSpecifier;
								QMapIterator<int, typeMapBlockParameterContainer> iterParam(*lBlockParams);//Iterate through all ObjectID's
								while (iterParam.hasNext()) 
								{
									iterParam.next();
									QMapIterator<QString, cBlockParameterStructure*> iterParamInner(iterParam.value());//Iterate through all ParameterNames
									while (iterParamInner.hasNext()) 
									{
										iterParamInner.next();
										cParameterSpecifier.nBlockID = tmpBlockStructure->getBlockID();
										if (bIsCustom)
											cParameterSpecifier.cParamEditType = PSET_CUSTOM;
										else
											cParameterSpecifier.cParamEditType = PSET_DEFINED;
										cParameterSpecifier.nObjectID = iterParam.key();
										cParameterSpecifier.sParamHexID = "";
										cParameterSpecifier.sParamName = iterParamInner.value()->getBlockParameterName();
										if (expTreeModel->addUpdateExperimentBlockParameter(cParameterSpecifier, iterParamInner.value()->getBlockParameterValue(), (cParameterSpecifier.cParamEditType == PSET_CUSTOM))==false)
										{
											expTreeModel->enableModifiedSignaling(true);
											expTreeModel->resetExperiment();
											return false;
										}
									}
								}
							}
						}

						int nExpStruct_LoopCount = tmpBlockStructure->getLoopCount();
						if (nExpStruct_LoopCount > 0)
						{
							for (nLoopCounter_1 = 0; nLoopCounter_1 < nExpStruct_LoopCount; nLoopCounter_1++)
							{
								int nNextCurrentLoopNumber = 0;
								cLoopStructure *tmpLoopStructure = NULL;
								tmpLoopStructure = tmpBlockStructure->getNextClosestLoopIDByFromLoopNumber(nNextCurrentLoopNumber);
								if (tmpLoopStructure)
								{
									if (expTreeModel->addExperimentBlockLoop(tmpBlockStructure->getBlockID(), tmpLoopStructure->getTargetBlockID(), tmpLoopStructure->getNumberOfLoops(), tmpLoopStructure->getLoopName()) == false)
									{
										expTreeModel->enableModifiedSignaling(true);
										expTreeModel->resetExperiment();
										return false;
									}
								}
								else
								{
									expTreeModel->enableModifiedSignaling(true);
									expTreeModel->resetExperiment();
									return false;
								}
							}
						}
					}
					else
					{
						expTreeModel->enableModifiedSignaling(true);
						expTreeModel->resetExperiment();
						return false;
					}
				}
				else
				{
					expTreeModel->enableModifiedSignaling(true);
					expTreeModel->resetExperiment();
					return false;
				}
			}
		}
		expTreeModel->emitModifiedSignal(false);
		expTreeModel->enableModifiedSignaling(true);
		return true;
	}
	expTreeModel->emitModifiedSignal(false);
	expTreeModel->enableModifiedSignaling(true);
	return false;
}

bool ExperimentManager::createExperimentObjects()
{
	QStringList strList;
	//cleanupExperimentObjects();
	if (cExperimentBlockTrialStructure)
	{
		QList<cObjectStructure*> lExpObjects = cExperimentBlockTrialStructure->getObjectList();
		foreach(cObjectStructure *currentObject, lExpObjects)
		{
			if (currentObject)
			{
				int metaID = QMetaType::type(currentObject->getObjectClass().toLatin1());
				if (metaID > 0) 
				{
					bool bRetVal = true;
					QObject *tmpObjectPointer = static_cast< QObject* > (QMetaType::create(metaID));
					const QMetaObject* metaObject = tmpObjectPointer->metaObject();

					//QStringList properties;
					//for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
					//	properties << QString::fromLatin1(metaObject->method(i).signature());

					if (currentScriptEngine)
					{
						if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(FUNC_MAKETHISAVAILABLEINSCRIPT_FULL)) == -1))//Is the slot present?
						{
							//Invoke the slot
							bRetVal = true;
							if (!(metaObject->invokeMethod(tmpObjectPointer, FUNC_MAKETHISAVAILABLEINSCRIPT, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(QString, currentObject->getObjectName()), Q_ARG(QObject*, (QObject*)currentScriptEngine))))
							{
								qDebug() << "invokeExperimentObjectsSlots::Could not invoke the slot(" << FUNC_MAKETHISAVAILABLEINSCRIPT << "()" << ")!";		
								return false;
							}		
						}
					}

					if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(FUNC_SETMETAOBJECT_FULL)) == -1))//Is the slot present?
					{
						//Invoke the slot
						bRetVal = true;
						if (!(metaObject->invokeMethod(tmpObjectPointer, FUNC_SETMETAOBJECT, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal))))
						{
							qDebug() << __FUNCTION__ << "::Could not invoke the slot(" << FUNC_SETMETAOBJECT << "()" << ")!";		
							return false;
						}		
					}

					if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(FUNC_SETEXPERIMENTOBJECTID_FULL)) == -1))//Is the slot present?
					{
						//Invoke the slot
						bRetVal = true;
						if (!(metaObject->invokeMethod(tmpObjectPointer, FUNC_SETEXPERIMENTOBJECTID, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(int, currentObject->getObjectID()))))
						{
							qDebug() << __FUNCTION__ << "::Could not invoke the slot(" << FUNC_SETEXPERIMENTOBJECTID << "()" << ")!";		
							return false;
						}		
					}

					if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(FUNC_SETEXPERIMENTMANAGER_FULL)) == -1))//Is the slot present?
					{
						//Invoke the slot
						bRetVal = true;
						if (!(metaObject->invokeMethod(tmpObjectPointer, FUNC_SETEXPERIMENTMANAGER, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(ExperimentManager*, this))))
						{
							qDebug() << "invokeExperimentObjectsSlots::Could not invoke the slot(" << FUNC_SETEXPERIMENTMANAGER << "()" << ")!";		
							return false;
						}		
					}

					objectElement tmpElement;
					objectElement *tmpElementPointer = NULL;
					if (mExperimentObjectList.contains(currentObject->getObjectID()))
						tmpElementPointer = &(mExperimentObjectList[currentObject->getObjectID()]);
					else
						tmpElementPointer = &tmpElement;
									
					if (tmpElementPointer)
					{
						tmpElementPointer->nObjectID = currentObject->getObjectID();
						tmpElementPointer->nMetaID = metaID;
						tmpElementPointer->sObjectName = currentObject->getObjectName();
						tmpElementPointer->pObject = tmpObjectPointer;
						//tmpElementPointer->typedExpParamCntnr = NULL;
						//tmpElementPointer->ExpBlockParams = NULL;
						tmpElementPointer->nCurrentState = Experiment_SubObject_Initialized;//This is still an inactive state!
						//tmpElementPointer->sStateHistory.nState.append(tmpElementPointer->nCurrentState);
						//tmpElementPointer->sStateHistory.sDateTimeStamp.append(getCurrentDateTimeStamp());
						if (mExperimentObjectList.contains(currentObject->getObjectID()) == false)
							mExperimentObjectList.insert(tmpElementPointer->nObjectID, tmpElement);
					}
				}
			}
		}
		return true;
	}
	qDebug() << __FUNCTION__ << "::No Experiment loaded!";
	return false;
}

bool ExperimentManager::getScriptContextValue(const QString &sScriptContextStatement, QVariant &sScriptContextReturnValue)
{
	//if(sScriptContextStatement.left(3).toLower() == QString(QString(EXPERIMENTMANAGER_SCRIPTCONTEXT_NAME) + ".").toLower())
	//{//Here we have direct access to this (--> the current Experiment Manager used)
	//	QString sMethod = sScriptContextStatement.mid(3);
	//	QString sRetVal;
	//	QVariant vRetVal;
	//	bool a = QMetaObject::invokeMethod( this, sMethod.toLatin1(), Qt::DirectConnection, Q_RETURN_ARG(QVariant, (QVariant)vRetVal));//Q_RETURN_ARG(QString, sRetVal));//(QVariant, sScriptContextReturnValue));
	//	sScriptContextReturnValue = sRetVal;
	//	return a;
	//}

	if (!currentScriptEngine)
	{
		QString tmpString = "... Could not expand the script object (" + sScriptContextStatement + "), the script engine is not ready!";
		emit WriteToLogOutput(tmpString);
		//qDebug() << tmpString;
		return false;
	}
	else
	{
		QString adjScriptContextStatement = "BrainStim.executeScriptContent(\"" + sScriptContextStatement + "\");";
		//if (!currentScriptEngine->canEvaluate(adjScriptContextStatement))
		//{
		//	QString tmpString = "... Could not evaluate the script object (" + sScriptContextStatement + ")!";
		//	emit WriteToLogOutput(tmpString);
		//	qDebug() << tmpString;
		//	return false;
		//}
		//else
		//{
		//	//QScriptContext *tmpContext = currentScriptEngine->currentContext();
			QScriptValue tmpScriptValue = currentScriptEngine->evaluate(adjScriptContextStatement);
			//if(currentScriptEngine->hasUncaughtException())
			//{
			//	QString tmpString = "... Evaluate script::UncaughtException() (" + sScriptContextStatement + ")!";
			//	emit WriteToLogOutput(tmpString);
			//	qDebug() << tmpString;
			//}
			sScriptContextReturnValue = tmpScriptValue.toVariant();
			return true;
		//}
	}
	return false;
}

