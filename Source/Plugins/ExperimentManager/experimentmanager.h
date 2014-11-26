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

//This file defines the script binding interface, all below function are scriptable except for the destructor

#ifndef ExperimentManager_H
#define ExperimentManager_H
#include <QObject>
#include <QString>
#include <QtScript/QtScript>
#include <QtScript/QScriptable>
#include <QGraphicsView>
#include <QGLWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QScreen>
#include "global.h"
#include "defines.h"
#include "./../../BrainStim/mainappinfo.h"
#include "experimentlogger.h"
#include "xmlmessagehandler.h"
#include "experimentparameter.h"
#include "experimentstructures.h"

#ifdef Q_OS_WIN32 //Are we on Windows?
#include <windows.h>
#endif

#define EXPERIMENTMANAGER_SCRIPTCONTEXT_NAME	"EM"

using namespace ExperimentManagerNameSpace;

class cExperimentStructure;
class ExperimentTreeItem;
class ExperimentGraphicEditor;
class ExperimentTreeModel;
class ExperimentParameterWidgets;

//!  The ExperimentManager class. 
/*!
  The Experiment Manager can be used configure, open and execute experiment (*.exml) files.
*/
class ExperimentManager : public QObject, protected QScriptable
{
	Q_OBJECT
	//Q_CLASSINFO("ScriptAPIClassName", "ExperimentManager");//Can't use defines here!, moc doesn't handle defines, not needed here

signals:
	//! The ExperimentStateHasChanged Signal.
	/*!
		You can use this Signal to keep track of the internal Experiment state changes.
		Parameter nExperimentMainState is type of the enum #ExperimentState.
	*/
	void ExperimentStateHasChanged(int nExperimentMainState, QString timeTextStamp);
	//! The WriteToLogOutput Signal.
	/*!
		You can use this signal to let the ExperimentManager write something to the BrainStim log window.
	*/
	void WriteToLogOutput(const QString &strText2Write);

public:
	//ExperimentManager(QObject *parent = NULL);
	ExperimentManager(QObject *parent = NULL, QScriptEngine* engine = NULL);
	~ExperimentManager();

	/*! The enum ExperimentState represents the main state of the ExperimentManager object, access in script e.g. : ExperimentManager.ExperimentState.ExperimentManager_Configured */
	enum ExperimentState 
	{
		ExperimentManager_NoState		= 0, /*!< this state is only internally used by the object while it is constructing, thus not yet fully available and therefore cannot be detected/used in the script environment. */
		ExperimentManager_Constructed	= 1, /*!< this state is used to determine whenever the object has been constructed, this state also cannot be detected/used in the script environment. */
		ExperimentManager_Loaded		= 2, /*!< after the Experiment file (*.exml) is successfully loaded (and validated) the ExperimentManager state changes to this state.*/
		ExperimentManager_PreParsed		= 3, /*!< when the Experiment could be successfully preparsed (after loading) then the ExperimentManager state changes to this state.*/
		ExperimentManager_Configured	= 4, /*!< after the function runExperiment() is called the ExperimentManager (validates if it has not been done before) and configures the experiment, it then changes to this state if the configuration was done.*/
		ExperimentManager_Initialized	= 5, /*!< after the Experiment configuration the ExperimentManager tries to initialized and changes the state to this state if this step was successful.*/
		ExperimentManager_IsStarting	= 6, /*!< after the Experiment initialization the ExperimentManager is ready to actually start the Experiment but first it changes the state to this state.*/
		ExperimentManager_Started		= 7, /*!< after the Experiment is successfully started the ExperimentManager changes to this state.*/
		ExperimentManager_IsStopping	= 8, /*!< before the ExperimentManager tries to stop the Experiment it first changes to this state.*/
		ExperimentManager_Stopped		= 9  /*!< after the ExperimentManager could finalize and stop the Experiment it changes to this state.*/
	};
	Q_ENUMS(ExperimentState)

	typedef struct
	{
		int nObjectID;
		int nMetaID;
		QObject *pObject;
		QString sObjectName;
		ExperimentSubObjectState nCurrentState;
		tParsedParameterList *ExpBlockParams;
		TypedExperimentParameterContainer *typedExpParamCntnr;
	} objectElement;

	struct strcInvokeObjectDefs
	{
		const QMetaObject* pMetaObject;
		QObject *pObject;
		QByteArray baSignature;
		int nOrderNumber;
		QList<QGenericArgument> lGenArgs;
		strcInvokeObjectDefs()
		{
			nOrderNumber = 0;
			pMetaObject = NULL;
			pObject = NULL;
		}
	};

	static QScriptValue ctor__experimentManager(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue ctor__experimentStateEnum(QScriptContext *context, QScriptEngine *engine);
	//static QScriptValue toExperimentStateEnumScriptValue(QScriptEngine *engine, const ExperimentState &s);
	//static void fromExperimentStateEnumScriptValue(const QScriptValue &obj, ExperimentState &s);
	//static ExperimentParameterDefinitionContainer *getExperimentParameterDefinition(const QString &sCollectionName);

	ExperimentState getCurrExperimentState() { return experimentCurrentState; }
	void cleanupSingletons();
	bool cleanupExperiment();
	bool fetchExperimentBlockParamsFromTreeItemList(const int &nBlockNumber, const int &nObjectID);
	tParsedParameterList *getObjectBlockParamListById(int nID);
	bool setExperimentObjectBlockParameterStructure(const int nObjectID, tParsedParameterList *expBlockTrialStruct);
	bool getScriptContextValue(const QString &sScriptContextStatement, QVariant &sScriptContextReturnValue);
	bool expandExperimentBlockParameterValue(QString &sValue);
	bool getExperimentObjectScriptValue(const int &nObjectID,const QString &sKeyName,QScriptValue &sScriptValue);
	bool setExperimentObjectFromScriptValue(const int &nObjectID,const QString &sKeyName,const QScriptValue &sScriptValue);
	QWidget *getVisualExperimentEditor();

	static bool createExperimentStructure(QList<ExperimentTreeItem*> &lExpTreeItems, ExperimentTreeModel *expTreeModel = NULL, cExperimentStructure* cExpStruct = NULL);

	template< typename T > T* getExperimentObjectVariabelePointer(const int &nObjectID,const QString &sKeyName)
	{
		if (nObjectID >= 0)
		{
			if (!lExperimentObjectList.isEmpty())
			{
				int nObjectCount = lExperimentObjectList.count();
				if (nObjectCount>0)
				{
					for (int i=0;i<nObjectCount;i++)
					{
						if (lExperimentObjectList[i].nObjectID == nObjectID)
						{
							if ((lExperimentObjectList[i].ExpBlockParams == NULL) || (lExperimentObjectList[i].ExpBlockParams->isEmpty()))
								return NULL;
							if (lExperimentObjectList[i].ExpBlockParams->contains(sKeyName.toLower()))
							{
								if(lExperimentObjectList[i].typedExpParamCntnr)
									return lExperimentObjectList[i].typedExpParamCntnr->getExperimentParameter<T>(sKeyName);
								else
									return NULL;
							}
						}
					}
				}
			}
		}
		return NULL;
	}

	template< typename T > bool insertExperimentObjectVariabelePointer(const int &nObjectID,const QString &sKeyName,T &tVariabele, bool bCreateVariabeleInMemory = false)
	{
		if (nObjectID >= 0) 
		{
			if (!lExperimentObjectList.isEmpty())
			{
				int nObjectCount = lExperimentObjectList.count();
				if (nObjectCount>0)
				{
					for (int i=0;i<nObjectCount;i++)
					{
						if (lExperimentObjectList[i].nObjectID == nObjectID)
						{
							if ((lExperimentObjectList[i].ExpBlockParams == NULL) || (lExperimentObjectList[i].ExpBlockParams->isEmpty()))
								return false;
							if(lExperimentObjectList[i].typedExpParamCntnr == NULL)
								lExperimentObjectList[i].typedExpParamCntnr = new TypedExperimentParameterContainer();
							bool bRetVal;
							if(bCreateVariabeleInMemory)
							{
								bRetVal = lExperimentObjectList[i].typedExpParamCntnr->createExperimentParameter<T>(sKeyName,tVariabele);
							}
							else
							{
								bRetVal = lExperimentObjectList[i].typedExpParamCntnr->insertExperimentParameter<T>(sKeyName,&tVariabele);
							}
							return bRetVal;
						}
					}
				}
			}
		}
		return false;
	}

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	/*! \brief Sends a text string to the BrainStim Log output pane.
	 *
	 *  This function sends a text string to the BrainStim Log output pane.
	 *  @param strText2Write a string containing the text to send.
	 */
	void sendToMainAppLogOutput(const QString &strText2Write);
	/*! \brief Sets the path for the experiment file.
	 *
	 *  This function sets the internal path to the experiment file.
	 *  @param sExpFileName a string holding the path to the experiment file (*.EXML), see also ExperimentManager.getExperimentFileName().
	 *  @return a boolean value determining whether the function executed successfully.
	 */		
	bool setExperimentFileName(const QString sExpFileName);
	/*! \brief Returns the path to the current set experiment file. 
	 *
	 *  This function returns the path to the current set experiment file, see also ExperimentManager.setExperimentFileName()
	 *  @return a string containing the path to the current set experiment file.
	 */	
	QString getExperimentFileName();
	/*! \brief Loads the Experiment from a file path into the memory.
	 *
	 *  The Experiment file (*.EXML) is loaded into memory.
	 *  @param strSource a string containing the file path to the experiment file (*.EXML), if this value is "" then the last configured experiment file path is automatically loaded in memory, see ExperimentManager.setExperimentFileName(). A second configurable option (see @param bIsFile) is that this variabele can also hold the script content to load.
	 *  @param bIsFile a boolean determining whether the @param strSource parameter should be threated as a string containing a path to a experiment file. If false then the parameter is threated threated as a string containing the script content itself.
	 *  @return a boolean value determining whether the function executed successfully.
	 */	
	bool loadExperiment(QString strSource = "", bool bIsFile = true);
	/*! \brief Saves the current experiment file from memory to a file.
	 *
	 *  The current experiment file in memory is saved to a file.
	 *  @param strFile a string containing the path to the to be saved experiment file (*.EXML). If this parameter  is empty ("") then the experiment is automatically saved to the last configured experiment file path, see ExperimentManager.setExperimentFileName().
	 *  @return a boolean value determining whether the function executed successfully.
	 */	
	bool saveExperiment(QString strFile = "");
	/*! \brief Validates the current experiment available in memory.
	 *
	 *  The current experiment available in memory is validated.
	 *  @return a boolean value determining whether the current experiment could be validated successfully.
	 */	
	bool validateExperiment();
	/*! \brief Runs the current Experiment available in memory.
	 *
	 *  The current available experiment in memory is started if/after it has been successfully loaded and validated.
	 *  @return a boolean value determining whether the function executed successfully.
	 */	
	bool runExperiment();
	/*! \brief Aborts the current Experiment that is running.
	 *
	 *  Tries to abort the current Experiment that is running, see ExperimentManager.runExperiment().
	 */	
	void abortExperiment();
	/*! \brief Stops the current Experiment that is running.
	 *
	 *  Tries to stop the current Experiment that is running, see ExperimentManager.runExperiment().
	 */	
	void stopExperiment();	
	/*! \brief Use this function to retrieve the current date/time.
	 *
	 *  This function returns the current date/time string that can be custom formatted.
	 *  @param sFormat a string value containing the custom Date/Time format, if left empty the value "yyyyMMddHHmmsszzz" is used.
	 *  @return a string value containing the current date/time string.
	 */	
	static QString getCurrentDateTimeStamp(const QString &sFormat = MainAppInfo::stdDateTimeFormat());
	/*! \brief Inserts a specified object parameter for the current experiment block.
	 *
	 *  Inserts a specified object parameter with an object ID, a parameter name and a parameter value in the current experiment block definitions.
	 *  @param nObjectID a integer value that holds the object ID.
	 *  @param sName a string value that contains the parameter name.
	 *  @param sValue a string value that contains the parameter value.
	 *  @param bIsInitializing a boolean value, if true then then the new parameter is marked as Initializing, see #ParsedParameterDefinition.bIsInitialized. This property marks the parameter as unused which can be helpful for further setting the default value.
	 *  @param bIsCustom a boolean value that determines whether the parameter should be treated as a custom parameter (default = false).
	 *  @return a boolean value determining whether the function executed successfully.
	 */	
	bool insertExperimentObjectBlockParameter(const int nObjectID, const QString sName, const QString sValue, bool bIsInitializing = true, bool bIsCustom = false);
	/*! \brief Returns a boolean value that determines if a specified named object parameter from the current experiment block could be retrieved and set.
	 *
	 *  @param nObjectID a integer value that holds the object ID.
	 *  @param sName a string value that contains the parameter name.
	 *  @param strcParDef a ParsedParameterDefinition structure that hold the requested named object parameter after a successful function call.
	 *  @return a boolean value determining whether this function executed successfully.
	 */	
	bool getExperimentObjectBlockParameter(const int nObjectID,const QString sName, ParsedParameterDefinition &strcParDef);
	/*! \brief Use this function to append a experiment data log to the default BrainStim log file.
	 *
	 *  This function appends a experiment data log entry to the default BrainStim log file by forwarding this information to the internal Experiment Data Logger.
	 *  @param nObjectIndex a integer value that holds the object ID to which the line to log correlates.
	 *  @param nTimerIndex a integer value that holds the Timer ID to which the line to log correlates.
	 *  @param strFunction a string value that defines the Function string of text of the experiment data log.
	 *  @param strTag a string value that defines the Tag string of text of the experiment data log.
	 *  @param strMessage a string value that defines the Message string of text of the experiment data log.
	 *  @param strValue a string value that defines the Value string of text of the experiment data log.
	 *  @return a boolean value determining whether this function executed successfully.
	 */	
	bool logExperimentObjectData(const int &nObjectIndex, const int &nTimerIndex, const QString &strFunction = "", const QString &strTag = "", const QString &strMessage = "", const QString &strValue = "");
	/*! \brief Use this function to create a Experiment Timer object.
	 *
	 *  This function creates a Experiment Timer object and returns the Timer identifier of the newly constructed Experiment Timer object.
	 *  @return a integer value determining whether this function executed successfully, if it failed it returns -1.
	 */	
	int createExperimentTimer();
	/*! \brief Use this function to start a Experiment Timer object.
	 *
	 *  This function tries to starts a experiment timer object specified with a Timer identifier.
	 *  @param nIndex a integer value that holds the Timer identifier.
	 *  @return a boolean value determining whether this function executed successfully.
	 */	
	bool startExperimentTimer(int nIndex);
	/*! \brief Use this function to retrieve the current elapsed Experiment Timer time and restart a Experiment Timer object.
	 *
	 *  This function tries to retrieve the current elapsed Experiment Timer time and restart a Experiment Timer object.
	 *  @param nIndex a integer value that holds the Timer identifier.
	 *  @return a double value boolean value holding the elapsed time or -1 in case the function executed unsuccessfully.
	 */	
	double restartExperimentTimer(int nIndex);
	/*! \brief Use this function to retrieve the current elapsed Experiment Timer time.
	 *
	 *  This function tries to retrieve the current elapsed Experiment Timer time
	 *  @param nIndex a integer value that holds the Timer identifier.
	 *  @return a double value boolean value holding the elapsed time or -1 in case the function executed unsuccessfully.
	 */
	double elapsedExperimentTimerTime(int nIndex);
	/*! \brief Use this function to retrieve the current Experiment State.
	 *
	 *  @return a string containing the current Experiment State.
	 */	
	QString getCurrentExperimentState();
	/*! \brief Sets the debug mode for the experiment.
	 *
	 *  This function sets the debug mode for the experiment.
	 *  @param mode a boolean value holding the debug mode for the current experiment.
	 */	
	void setDebugMode(bool mode);
	/*! \brief Get the debug mode for the experiment.
	 *
	 *  Returns the configured debug mode for the current experiment.
	 *  @return a boolean value determining whether the experiments debug mode is enabled.
	 */	
	bool getDebugMode();
	/*! \brief Sets the experiment name for the current experiment.
	 *
	 *  This function sets the experiment name for the current experiment.
	 *  @param sName a string containing the new experiment name.
	 */	
	void setExperimentName(QString sName);
	/*! \brief Returns the current experiment name.
	 *
	 *  This function returns the configured experiment name for the current experiment.
	 *  @return a string containing the configured experiment name.
	 */	
	QString getExperimentName();
	/*! \brief Sets the configurable part of the automatic generated experiment Output filename.
	 *
	 *  This function sets the configurable part of the automatic generated experiment Output filename. This string is then integrated in the output filename (format = YearMonthDayHourMinuteSecond_<sPostString>.txt ).
	 *  @param sPostString a string containing the configurable post file name string.
	 */	
	void setExperimentOutputFilePostString(const QString &sPostString);
	/*! \brief Returns the current in-memory Experiment Structure
	 *
	 *  This function returns a pointer to the current in-memory Experiment Structure.
	 *  @return a pointer to a cExperimentStructure structure, if the function fails it returns NULL.
	 */	
	cExperimentStructure *getExperimentStructure();
	/*! \brief Sets the in-memory Experiment Structure
	 *
	 *  This function sets the current in-memory Experiment Structure.
	 *  @param expStruct a cExperimentStructure that should be set as the new in-memory Experiment Structure.
	 *  @return a boolean value determining whether this function executed successfully.
	 */	
	bool setExperimentStructure(cExperimentStructure *expStruct);
	/*! \brief Shows the Experiment in a Visual Experiment Editor UI.
	 *
	 *  This function shows the Experiment in a Visual Experiment Editor UI, if the provided ExperimentTreeModel is NULL than the current in-memory Experiment is parsed by the editor.
	 *  @param expTreeModel a ExperimentTreeModel to be edited by the Visual Experiment Editor UI, make it NULL to automatically use the current in-memory Experiment.
	 *  @param sExpTreeModelCanonFilePath a string holding a canonical path referring to a file that should be used by default for saving changes to. 
	 *  @return a boolean value determining whether this function executed successfully.
	 */	
	bool showVisualExperimentEditor(ExperimentTreeModel *expTreeModel = NULL, const QString &sExpTreeModelCanonFilePath = "");
	/*! \brief Returns a QScreen object representing the active Stimuli Output Screen.
	 *
	 *  This function returns a pointer to an QScreen object representing the active configured Stimuli Output Screen.
	 *  @return a pointer to an QScreen object.
	 */		
	QScreen* getActiveStimuliOutputScreen();
	/*! \brief Sets a Screen referred by it's Screen Number as the current active Stimuli Output Screen.
	 *
	 *  This function sets a Screen referred by it's Screen Number as the current active Stimuli Output Screen.
	 *  @param nScreenNumber a integer value representing the Screen Number that should be set as the current active Stimuli Output Screen.
	 *  @return a boolean value determining whether this function executed successfully.
	 */	
	bool setActiveStimuliOutputScreen(int nScreenNumber);
	/*! \brief Returns various Screen information from all available screens.
	 *
	 *  This function returns various Screen information from all available screens.
	 *  @return a string containing the various Screen information from all the available screens.
	 */	
	static QString getAvailableScreensInformation();



#ifndef QT_NO_DEBUG
	QString Test(const QString &sInput = "");
#endif

private slots:
	void changeExperimentSubObjectState(ExperimentSubObjectState nState);
	void visualExperimentEditorDestroyed(ExperimentGraphicEditor *pExpGraphEditor);

private:
	void DefaultConstruct();
	bool WriteAndCloseExperimentOutputData(const QString &postFileName = "");
	void initializeDataLogger();
	void RegisterMetaTypes();
	bool invokeExperimentObjectsSlots(const QString &sSlotName);
	bool prePassiveParseExperiment();
	bool configureExperiment();
	bool createExperimentObjects();
	//bool createExperimentStructureFromDomNodeList(const QDomNodeList &ExpBlockTrialsDomNodeLst, cExperimentStructure *expStruct);
	static bool createExperimentStructureFromTreeItemList(const QList<ExperimentTreeItem*> &ExpRootTreeItems, cExperimentStructure *expStruct);
	//int createExperimentBlockParamsFromDomNodeList(const int &nBlockNumber, const int &nObjectID, QDomNodeList *pExpBlockTrialsDomNodeLst = NULL, tParsedParameterList *hParams = NULL);
	int createExperimentBlockParamsFromTreeItemList(const int &nBlockNumber, const int &nObjectID, QList<ExperimentTreeItem*> *pExpBlockTrialsTreeItems = NULL, tParsedParameterList *hParams = NULL);
	bool connectExperimentObjects(bool bDisconnect = false, int nObjectID = -1);
	bool initializeExperiment(bool bFinalize = false);
	bool finalizeExperimentObjects();
	bool startExperimentObjects(bool bRunFullScreen = true);
	bool initExperimentObjects();
	bool stopExperimentObjects();
	bool abortExperimentObjects();
	bool changeExperimentObjectsSignalSlots(bool bDisconnect = false, int nSpecificIndex = -1);
	void cleanupExperimentObjects();
	void changeCurrentExperimentState(ExperimentState expCurrState);
	QObject *getObjectElementById(int nID);
		
	ExperimentParameterWidgets *expParamWidgets;
	QList<ExperimentTreeItem*> ExperimentTreeBlockItemList;
	QList<ExperimentTreeItem*> ExperimentObjectTreeItemList;
	QList<ExperimentTreeItem*> ExperimentTreeItemList;
	cExperimentStructure *cExperimentBlockTrialStructure;
	QList<objectElement> lExperimentObjectList;
	ExperimentTreeModel *currentExperimentTree;
	ExperimentGraphicEditor *ExpGraphicEditor;
	ExperimentState experimentCurrentState;
	QHash<QString, int> experimentStateHash;
	
	QByteArray currentExperimentFile;
	QByteArray currentValidationFile;

	QObject *parentObject;
	QScreen *sConfiguredActiveScreen;
	QScriptEngine *currentScriptEngine;
	bool m_RunFullScreen;
	QString m_ExpFileName;
	QString m_ExpFolder;
	ExperimentLogger *expDataLogger;
	int nExperimentTimerIndex;
	QString sExperimentOutputDataPostString;
};

#endif // ExperimentManager_H
