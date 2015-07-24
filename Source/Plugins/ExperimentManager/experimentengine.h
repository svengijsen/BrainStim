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

#ifndef EXPERIMENTENGINE_H
#define EXPERIMENTENGINE_H

#include <QObject>
#include <QKeyEvent>
#include "experimentmanager.h"
#include "experimenttimer.h"

using namespace ExperimentManagerNameSpace;

//!  The ExperimentEngine class. 
/*!
  The ExperimentEngine class can be derived from to create custom classes that can again be used in combination with the ExperimentManager for presenting visual or other kind of stimuli.
  This class is optimized for various painting operations supported by OpenGL.
*/
class ExperimentEngine : public QObject
{
	friend class QML2Viewer;

	Q_OBJECT

signals:
	//! The UserWantsToClose Signal.
	/*!
		You can use this Signal to detect whenever the user tries to abort the experiment.
		No Parameter.
	*/
	void UserWantsToClose(void);
	//! The ObjectShouldStop Signal.
	/*!
		You can use this Signal to detect whenever this object tries to abort/stop the experiment.
		No Parameter.
	*/
	void ObjectShouldStop(void);
	//! The ObjectStateHasChanged Signal.
	/*!
		You can use this Signal to detect whenever the Experiment State for this object changes.
		Parameter expSubObjectState is type of the enum ExperimentManagerNameSpace::ExperimentSubObjectState.
	*/
	void ObjectStateHasChanged(ExperimentSubObjectState expSubObjectState);
	//! The PrepareNewInitBlockTrial Signal.
	/*!
		You can use this Signal to keep track of whenever the Experiment Manager tries to Initialize a new BlockTrial for this object, this is executed before the parameters are fetched.
		No Parameter.
	*/
	void PrepareNewInitBlockTrial(void);
	//! The NewInitBlockTrial Signal.
	/*!
		You can use this Signal to keep track of whenever the Experiment Manager tries to Initialize a new BlockTrial for this object, this is executed after the parameters are fetched.
		No Parameter.
	*/
	void NewInitBlockTrial(void);
	//! The ExternalTriggerIncremented Signal.
	/*!
		You can use this Signal to keep track of whenever the External Trigger gets incremented, see ExperimentEngine::incrementExternalTrigger
		Please bear in mind that this is already emitted before a new BlockTrial is initialized.
		Parameter nTrigger holds the number of received external triggers after the start of the experiment.
	*/
	void ExternalTriggerIncremented(int nTrigger);
	//! The ExperimentStructureChanged Signal.
	/*!
		You can use this Signal to keep track Experiment structure changes.
		Parameter nBlock represents the current Block number.
		Parameter nTrial represents the current Trial number.
		Parameter nInternalTrigger represents the current Internal Trigger number.
	*/
	void ExperimentStructureChanged(int nBlock,int nTrial,int nInternalTrigger);

public:
	/*! \brief The ExperimentEngine constructor.
	*
	*   You do not need to specify the parent object. 
	*	The BrainStim script engine automatically retrieves the parent role
	*/
	ExperimentEngine(QObject *parent = NULL);
	/*! \brief The ExperimentEngine destructor.
	*
	*   You do not need call the destructor. 
	*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
	*/
	~ExperimentEngine();

	template< typename T1 > bool insertExpObjectParameter(const int &nObjectID,const QString &sKeyName,T1 &tVariabele,bool bIsInitializing = true)
	{
		bool bRetVal = false;
		QString tVarName = typeid(tVariabele).name();
		QString sValue = templateVariabeleToString(tVariabele);
		if (insertExpObjectBlockParameter(nObjectID, sKeyName, sValue, bIsInitializing))
		{
			if (bIsInitializing)
			{
				QString sNewValue = getExpObjectBlockParameter(nObjectID, sKeyName, sValue).sValue;//used for retrieving already set initialized values
				if (sNewValue != sValue)
				{
					sValue = sNewValue;
					tVariabele = stringToTemplateVariabele<T1>(sNewValue).value<T1>();
				}
			}
			bRetVal = pExperimentManager->insertExperimentObjectVariabelePointer(nObjectID, sKeyName, tVariabele);
		}
		return bRetVal;
	}
	QString templateVariabeleToString(const QString &Var) { return Var; }
	QString templateVariabeleToString(const QStringList &Var) { return Var.join(","); }
	QString templateVariabeleToString(const QColor &Var) { return Var.name(); }
	QString templateVariabeleToString(const int &Var) { return QString::number(Var); }
	QString templateVariabeleToString(const float &Var) { return QString::number(Var); }
	QString templateVariabeleToString(const double &Var) { return QString::number(Var); }
	QString templateVariabeleToString(const bool &Var)
	{
		if (Var)
			return TYPE_BOOL_TRUE;
		else
			return TYPE_BOOL_FALSE;
	}
	template< typename T1 > QVariant stringToTemplateVariabele(const QString &sVar)
	{ 
		bool bResult = false;
		if (typeid(T1) == typeid(QString))
		{
			return sVar;
		}
		else if (typeid(T1) == typeid(QStringList))
		{
			return sVar.split(",");
		}
		else if (typeid(T1) == typeid(QColor))
		{
			return QColor(sVar);
		}
		else if (typeid(T1) == typeid(int))
		{
			int tmpInt = sVar.toInt(&bResult);
			if (bResult)
				return tmpInt;
		}
		else if (typeid(T1) == typeid(float))
		{
			float tmpFloat = sVar.toFloat(&bResult);
			if (bResult)
				return tmpFloat;
		}
		else if (typeid(T1) == typeid(double))
		{
			double tmpDouble = sVar.toDouble(&bResult);
			if (bResult)
				return tmpDouble;
		}
		else if (typeid(T1) == typeid(bool))
		{
			if (TYPE_BOOL_TRUE)
				{return true;}
			else if (TYPE_BOOL_FALSE)
				{return true;}
		}
		qDebug() << __FUNCTION__ << "Could not parse the template variable(" << sVar << ")";
		return T1();
	}
	template< typename T2 > T2* getExpObjectVariabelePointer(const int &nObjectID,const QString &sKeyName)
	{
		return pExpConf->pExperimentManager->getExperimentObjectVariabelePointer<T2>(nObjectID,sKeyName);
	}
	bool setScriptEngine(QScriptEngine *currScriptEngine) {currentScriptEngine = currScriptEngine; return true;};
	QScriptEngine *getScriptEngine() {return currentScriptEngine;};
	ParsedParameterDefinition getExpObjectBlockParameter(const int nObjectID,const QString sName, QString sDefValue);

public slots:
	bool initExperimentObject();
	bool startExperimentObject();
	bool stopExperimentObject();
	bool abortExperimentObject();
	void ExperimentShouldFinish();					//Slot for a signal to connect to
	bool setExperimentObjectID(int nObjID);			//Necessary to set the ID!
	bool setExperimentMetaObject();					//Necessary to set the MetaObject!
	const QMetaObject *getExperimentMetaObject();
	virtual bool setExperimentManager(ExperimentManager *expManager);
	virtual bool setExperimentObjectReadyToUnlock();
	
	/*! \brief Increments the External Trigger.
	*
	*  Invoke this slot to increment the External Trigger.
	*/
	void incrementExternalTrigger();
	/*! \brief Returns this Object ID.
	 *
	 *  This function returns its unique Object ID as registered in the ExperimentManager.
	 */
	int getObjectID();
	/*! \brief retrieves the current value of an Experiment Parameter variable.
	 *
	 *  This function can be used to retrieve the current value of an Experiment Parameter variable for a specific object.
	 *  This parameter variable value is currently in use.
	 */
	QScriptValue getExperimentObjectParameter(const int &nObjectID, const QString &strName);
	/*! \brief Sets the current value of an Experiment Parameter variable.
	*
	*   This function can be used to immediately set a value of an Experiment Parameter variable for a specific object.
	*	This parameter variable value is immediately set and used.
	*/
	bool setExperimentObjectParameter(const int &nObjectID, const QString &strName, const QScriptValue &sScriptVal);
	/*! \brief Update or insert an Experiment Block Parameter that can be parsed.
	 *
	 *  This function can be used to insert or update an Experiment Block Parameter to be parsed for a specific object.
	 *  This Parameter can be parsed at the next initialization of a new BlockTrial if not yet initialized.
	 *  Setting the parameter bIsInitializing to true forces the new parameter to be re-parsed at the next initialization of a new BlockTrial.
	 *  The parameter bIsCustom (default = false) defines whether this parameter should be treated as a custom parameter.
	 */
	bool insertExpObjectBlockParameter(const int nObjectID,const QString sName,const QString sValue,bool bIsInitializing = true, bool bIsCustom = false);
	/*! \brief Returns a QScreen object representing the active Stimuli Output Screen.
	*
	*  This function returns a pointer to an QScreen object representing the active configured Stimuli Output Screen.
	*  @return a pointer to an QScreen object.
	*/
	virtual QScreen* getActiveStimuliOutputScreen() = NULL;
	/*! \brief Sets a Screen referred by it's Screen Number as the current active Stimuli Output Screen.
	*
	*  This function sets a Screen referred by it's Screen Number as the current active Stimuli Output Screen.
	*  @param nScreenNumber a integer value representing the Screen Number that should be set as the current active Stimuli Output Screen.
	*  @return a boolean value determining whether this function executed successfully.
	*/
	virtual bool setActiveStimuliOutputScreen(int nScreenNumber) = NULL;

protected:
	int checkForNextBlockTrial();
	bool isDebugMode();
	bool isLocked() {return bCurrentSubObjectIsLocked;};
	bool isReadyToUnlock() {return bCurrentSubObjectReadyToUnlock;};
	bool experimentShouldStop() {return bExperimentShouldStop;};
	double getElapsedTrialTime() {return expTrialTimer.getElapsedTimeInMilliSec();};
	int getCurrentBlockTrialFrame() {return nCurrExpBlockTrialFrame;};
	int incrementCurrentBlockTrialFrame() {return nCurrExpBlockTrialFrame++;};
	int getFrameTimerIndex() {return nFrameTimerIndex;};
	QString getLastLoggedObjectStateTime(ExperimentSubObjectState state);
	void customEvent(QEvent *event);
	void initBlockTrial();
	ExperimentSubObjectState getSubObjectState() {return currentSubObjectState;};
	bool changeSubObjectState(ExperimentSubObjectState newSubObjectState);

	QScreen *sConfiguredActiveScreen;

protected slots:
	void animate(bool bOnlyCheckBlockTrials = false);

private:
	bool expandExperimentBlockParameterValue(QString &sValue);
	bool unlockExperimentObject();
	void init();
	void fetchCurrentExperimentStructures();

	bool bCurrentSubObjectReadyToUnlock;				//The user first has to press the 'Alt' key before the experiment can be unlocked by the next trigger.
	bool bFirstTriggerAfterUnlock;						//To detect the exact start of the experiment detected by the checkForNextBlockTrial() function.
	bool bCurrentSubObjectIsLocked;						//After the above key is pressed this variable is set to false at the first trigger and the experiment starts.
	QMutex mutProcEvents;								//Another implementation, due to qApp->processEvents() RecursiveRepaint can occur...
	int nLastProcessedExternalTriggers;
	int nCurrExpBlockTrialFrame;
	//double dElapsedTrialTime;
	bool bExperimentShouldStop;
	int nObjectID;
	QTimer tStimTimer;
	QEvent::Type tEventObjectStopped;
	ExperimentSubObjectState currentSubObjectState;
	ExperimentSubObjectStateHistory subObjectStateHistory;
	int nFrameTimerIndex;
	int nTrialTimerIndex;
	tParsedParameterList *ExpBlockParams;
	ExperimentManager *pExperimentManager; 
	ExperimentTimer expTrialTimer;
	const QMetaObject* thisMetaObject;
	QScriptEngine* currentScriptEngine;
};

#endif // EXPERIMENTENGINE_H
