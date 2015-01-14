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


#ifndef EXPERIMENTSTRUCTURES_H
#define EXPERIMENTSTRUCTURES_H

#include <QtScript/QtScript>
#include <QList>
#include <QSharedData>
#include "maindefines.h"

/*! The namespace for the Experiment Structures */
namespace ExperimentStructuresNameSpace
{
	//The enum RepeatAmount can store the remaining amount of trials or actions to take
	enum RepeatAmount
	{
		RA_ZERO							=  0,	//  0: Zero Repeats
		RA_FINISHED						= -1,	// -1: Repeats have all been processed/finished
		RA_REINITIALIZE					= -2,	// -2: Repeats should be re-initialized first
		RA_INFINITE						= -3,	// -3: Infinite Repeats
		RA_UNDEFINED					= -4	// -4: None defined (i.e. no loops defined)
	};

	enum LoopCounterException
	{
		LCE_FIRSTLOOP					=  0,	//   0: This is the first loop
		LCE_UNUSED						= -1,	//  -1: This loop is not yet used
		LCE_FINISHED					= -2	//  -2: This loop is fully processed
	};

	enum ExperimentRunState
	{
		ES_IDLE							=  0,	//  0: IDLE STATE
		ES_WAITING_FOR_TRIGGER			=  1,	//  1: WAITING FOR TRIGGER
		ES_RUNNING						=  2	//  2: RUNNING
	};

	enum ObjectIndex
	{
		OI_UNDEFINED					= -1
	};

	enum CounterFlag
	{
		CF_UNINITIALIZED				= -1
	};

	enum MethodType //See also methodTypeStringToInteger() and methodTypeToString() slot!
	{
		METHOD_TYPE_UNDEFINED			= 0,
		METHOD_TYPE_SIGNAL				= 1,
		METHOD_TYPE_SLOT				= 2
	};

	/*! \struct strcExperimentStructureState
	 *  \brief The strcExperimentStructureState structure.
	 *
	 * This can store a state of a Experiment Structure, that holds a number of public accessible variables.
	 */
	typedef struct strcExperimentStructureState
	{
		int Experiment_ExternalTrigger;			//!<  The received ExternalTriggers since the Experiment started.
		int CurrentBlock_BlockID;				//!<  The current Block ID.
		int CurrentBlock_LoopID;				//!<  The current Loop ID from the current Block (with ID=strcExperimentStructureState::CurrentBlock_BlockID).
		int CurrentBlock_TrialNumber;			//!<  The current Trial Number from the current Block (with ID=strcExperimentStructureState::CurrentBlock_BlockID).
		int CurrentBlock_InternalTrigger;		//!<  The current Internal Trigger Number from the current Block (with ID=strcExperimentStructureState::CurrentBlock_BlockID).
		int CurrentBlock_ExternalTrigger;		//!<  The current External Trigger Number from the current Block (with ID=strcExperimentStructureState::CurrentBlock_BlockID).
	} ExperimentStructureState;

	struct strcObjectDefinition
	{
		QString sDefinitionFile;
		strcObjectDefinition()
		{
			sDefinitionFile = "";
		}
	};

	struct strcExperimentObject
	{
		int nID;
		QString sName;
		QString sClass;
		QList<ExperimentStructuresNameSpace::strcObjectDefinition> lObjectDefinitions;
		strcExperimentObject()
		{
			nID = -1;
			sName = "";
			sClass = "";
		}
	};
}

class cBlockParameterStructure;
typedef QMap<QString, cBlockParameterStructure*> typeMapBlockParameterContainer;			//key is Parameter Name
typedef QMap<int, typeMapBlockParameterContainer> typeMapObjectsBlockParameterContainer;	//key is objectID

//!  The cLoopStructure class. 
/*!
  The cLoopStructure class is a container to store a Loop structure from a block structure, see cBlockStructure.
*/
class cLoopStructure : public QObject
{
	Q_OBJECT

	//! \brief LoopID property.
	/*!  Use this property to set or retrieve the LoopID.
    */
	Q_PROPERTY(int LoopID WRITE setLoopID READ getLoopID)
	//! \brief LoopNumber property.
	/*!  Use this property to set or retrieve the LoopNumber.
    */
	Q_PROPERTY(int LoopNumber WRITE setLoopNumber READ getLoopNumber)
	//! \brief LoopName property.
	/*!  Use this property to set or retrieve the LoopName.
    */
	Q_PROPERTY(QString LoopName WRITE setLoopName READ getLoopName)
	//! \brief NumberOfLoops property.
	/*!  Use this property to set or retrieve the NumberOfLoops.
    */
	Q_PROPERTY(int NumberOfLoops WRITE setNumberOfLoops READ getNumberOfLoops)
	//! \brief TargetBlockID property.
	/*!  Use this property to set or retrieve the TargetBlockID.
    */
	Q_PROPERTY(int TargetBlockID WRITE setTargetBlockID READ getTargetBlockID)
	//! \brief LoopCounter property.
	/*!  Use this property to set or retrieve the LoopCounter.
    */
	Q_PROPERTY(int LoopCounter READ getTargetBlockID)

public:
	cLoopStructure();
	cLoopStructure(const int &LoopID,const int &LoopNumber,const int &LoopTargetBlockID,const QString &LoopName = "",const int &NumberOfLoops = ExperimentStructuresNameSpace::RA_ZERO);
	cLoopStructure(const cLoopStructure& other);
	~cLoopStructure();

	static QScriptValue ctor__cLoopStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue loopStructureToScriptValue(QScriptEngine *engine, cLoopStructure* const &s);
	static void loopStructureFromScriptValue(const QScriptValue &obj, cLoopStructure* &s);

	int initializeCurrentLoopCounter();
	int incrementCurrentLoopCounter();
	void resetCurrentLoopCounter();
	void finalizeCurrentLoopCounter();

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	//! \brief setLoopID slot.
	/*!  This function sets the loop ID to the new provided value.
	 * @param nValue a integer value (>0) holding the new loop ID.
	 */	
	void setLoopID(const int &nValue) {nLoopID = nValue;};
	//! \brief getLoopID slot.
	/*!  This function returns the loop ID.
	 * @return a integer value containing the requested loop ID.
	 */	
	int getLoopID() const {return nLoopID;};
	//! \brief setLoopNumber slot.
	/*!  This function sets the loop number to the new provided value.
	 * @param nValue a integer value (>0) holding the new loop number.
	 */	
	void setLoopNumber(const int &nValue) {nLoopNumber = nValue;};
	//! \brief getLoopNumber slot.
	/*!  This function returns the loop number.
	 * @return a integer value containing the requested loop number.
	 */	
	int getLoopNumber() const {return nLoopNumber;};
	//! \brief setLoopName slot.
	/*!  This function sets the loop name to the new provided value.
	 * @param sValue a string value holding the new loop name.
	 */	
	void setLoopName(const QString &sValue) {sLoopName = sValue;};
	//! \brief getLoopName slot.
	/*!  This function returns the loop name.
	 * @return a string value containing the requested loop name.
	 */	
	QString getLoopName() const {return sLoopName;};
	//! \brief setNumberOfLoops slot.
	/*!  This function set the number of loops to the new provided value.
	 * @param nValue a integer value holding the number of loops.
	 */	
	void setNumberOfLoops(const int &nValue) {nNrOfLoops = nValue;};
	//! \brief getNumberOfLoops slot.
	/*!  This function returns the number of defined loops.
	 * @return a integer value containing number of defined loops.
	 */	
	int getNumberOfLoops() const {return nNrOfLoops;};
	//! \brief setTargetBlockID slot.
	/*!  This function sets the target block ID for the current loop to the new provided value.
	 * @param nValue a integer value (>0) holding the new target block ID.
	 */	
	void setTargetBlockID(const int &nValue) {nTargetBlockID = nValue;};
	//! \brief getTargetBlockID slot.
	/*!  This function returns the current loop target block ID.
	 * @return a integer value containing the current loop target block ID.
	 */	
	int getTargetBlockID() const {return nTargetBlockID;};
	//! \brief getLoopCounter slot.
	/*!  This function returns the loop counter value for the current loop.
	 * @return a integer value containing the loop counter value for the current loop.
	 */	
	int getLoopCounter() const {return nLoopCounter;};

private:
	bool Initialize();

	QScriptEngine* currentScriptEngine;
	int nLoopID;
	int nLoopNumber;
	QString sLoopName;
	int nNrOfLoops;
	int nTargetBlockID;
	int nLoopCounter;
};

//!  The cBlockParameterStructure class. 
/*!
The cBlockParameterStructure class is a container to store a Block Parameter structure that can be used by a Block structure definition for a specific Object Structure, see cBlockStructure, cObjectStructure.
*/
class cBlockParameterStructure : public QObject
{
	Q_OBJECT

		//! \brief BlockParameterID property.
		/*!  Use this property to set or retrieve the Block Parameter ID.
		*/
		Q_PROPERTY(int BlockParameterID WRITE setBlockParameterID READ getBlockParameterID)
		//! \brief BlockParameterName property.
		/*!  Use this property to set or retrieve the Block Parameter Name.
		*/
		Q_PROPERTY(QString BlockParameterName WRITE setBlockParameterName READ getBlockParameterName)
		//! \brief BlockParameterValue property.
		/*!  Use this property to set or retrieve the Block Parameter Value.
		*/
		Q_PROPERTY(QString BlockParameterValue WRITE setBlockParameterValue READ getBlockParameterValue)

public:
	cBlockParameterStructure();
	cBlockParameterStructure(const int &nBlockParamId, const QString &sBlockParamName, const QString &sBlockParamValue);
	cBlockParameterStructure(const cBlockParameterStructure& other);
	~cBlockParameterStructure();

	static QScriptValue ctor__cBlockParameterStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue BlockParameterStructureToScriptValue(QScriptEngine *engine, cBlockParameterStructure* const &s);
	static void BlockParameterStructureFromScriptValue(const QScriptValue &obj, cBlockParameterStructure* &s);

	public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	//! \brief setBlockParameterID slot.
	/*!  This function sets the Block Parameter ID to the new provided value.
	* @param nValue a integer value (>0) holding the new Block Parameter ID.
	*/
	void setBlockParameterID(const int &nValue) { nBlockParameterID = nValue; };
	//! \brief getBlockParameterID slot.
	/*!  This function returns the Block Parameter ID.
	* @return a integer value containing the requested Block Parameter ID.
	*/
	int getBlockParameterID() const { return nBlockParameterID; };
	//! \brief setBlockParameterName slot.
	/*!  This function sets the Block Parameter Name to the new provided value.
	* @param sValue a string value holding the new Block Parameter Name.
	*/
	void setBlockParameterName(const QString &sValue) { sBlockParameterName = sValue; };
	//! \brief getBlockParameterName slot.
	/*!  This function returns the Block Parameter Name.
	* @return a string value containing the requested Block Parameter Name.
	*/
	QString getBlockParameterName() const { return sBlockParameterName; };
	//! \brief setBlockParameterValue slot.
	/*!  This function sets the Block Parameter Value to the new provided value.
	* @param sValue a string value holding the new Block Parameter Value.
	*/
	void setBlockParameterValue(const QString &sValue) { sBlockParameterValue = sValue; };
	//! \brief getBlockParameterValue slot.
	/*!  This function returns the Block Parameter Value.
	* @return a string value containing the requested Block Parameter Value.
	*/
	QString getBlockParameterValue() const { return sBlockParameterValue; };

private:
	bool Initialize();

	QScriptEngine* currentScriptEngine;
	int nBlockParameterID;
	QString sBlockParameterValue;
	QString sBlockParameterName;
};

class cBlockStructure_SharedData : public QSharedData
{
public:
	cBlockStructure_SharedData();
	cBlockStructure_SharedData(const cBlockStructure_SharedData &other);
	~cBlockStructure_SharedData();

	QScriptEngine* currentScriptEngine;
	int nBlockID;
	int nBlockNumber;
	QString sBlockName;
	int nNrOfTrials;
	int nNrOfInternalTriggers;
	int nNrOfExternalTriggers;
	QList<cLoopStructure*> lLoops;
	typeMapObjectsBlockParameterContainer lDefinedBlockParams;	//key is objectID, second inner key is Parameter Name
	typeMapObjectsBlockParameterContainer lCustomBlockParams;		//key is objectID, second inner key is Parameter Name
};

//!  The cBlockStructure class. 
/*!
  The cBlockStructure class is a container to store a Block structure from a experiment structure, see cExperimentStructure.
*/
class cBlockStructure : public QObject
{
	Q_OBJECT

	//! \brief BlockID property.
	/*!  Use this property to set or retrieve the BlockID.
    */
	Q_PROPERTY(int BlockID WRITE setBlockID READ getBlockID)
	//! \brief BlockNumber property.
	/*!  Use this property to set or retrieve the BlockNumber.
    */
	Q_PROPERTY(int BlockNumber WRITE setBlockNumber READ getBlockNumber)
	//! \brief BlockName property.
	/*!  Use this property to set or retrieve the BlockName.
    */
	Q_PROPERTY(QString BlockName WRITE setBlockName READ getBlockName)
	//! \brief NumberOfTrials property.
	/*!  Use this property to set or retrieve the NumberOfTrials.
    */
	Q_PROPERTY(int NumberOfTrials WRITE setNumberOfTrials READ getNumberOfTrials)
	//! \brief NumberOfInternalTriggers property.
	/*!  Use this property to set or retrieve the NumberOfInternalTriggers.
    */
	Q_PROPERTY(int NumberOfInternalTriggers WRITE setNumberOfInternalTriggers READ getNumberOfInternalTriggers)
	//! \brief NumberOfExternalTriggers property.
	/*!  Use this property to set or retrieve the NumberOfExternalTriggers.
    */
	Q_PROPERTY(int NumberOfExternalTriggers WRITE setNumberOfExternalTriggers READ getNumberOfExternalTriggers)

public:
	cBlockStructure();
	cBlockStructure(const int &BlockID,const int &BlockNumber,const QString &BlockName = "",const int &NumberOfTrials = ExperimentStructuresNameSpace::RA_ZERO);
	cBlockStructure(const cBlockStructure& other);
	~cBlockStructure();

	static QScriptValue ctor__cBlockStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue blockStructureToScriptValue(QScriptEngine *engine, cBlockStructure* const &s);
	static void blockStructureFromScriptValue(const QScriptValue &obj, cBlockStructure* &s);
	
public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script
	
	//! \brief setBlockID slot.
	/*!  This function sets the block ID to the new provided value.
	 * @param nValue a integer value (>0) containing the new block ID.
	 */		
	void setBlockID(const int &nValue) {pSharedData->nBlockID = nValue;};
	//! \brief getBlockID slot.
	/*!  This function returns the block ID.
	 * @return a integer containing the block ID.
	 */		
	int getBlockID() const {return pSharedData->nBlockID;};
	//! \brief setBlockNumber slot.
	/*!  This function sets the block number to the new provided value.
	 * @param nValue a integer value (>0) containing the new block number.
	 */		
	void setBlockNumber(const int &nValue) {pSharedData->nBlockNumber = nValue;};
	//! \brief getBlockNumber slot.
	/*!  This function returns the block number.
	 * @return a integer containing the block number.
	 */		
	int getBlockNumber() const {return pSharedData->nBlockNumber;};
	//! \brief setBlockName slot.
	/*!  This function sets the block name to the new provided value.
	 * @param sValue a string value containing the new block name.
	 */		
	void setBlockName(const QString &sValue) {pSharedData->sBlockName = sValue;};
	//! \brief getBlockName slot.
	/*!  This function returns the block name.
	 * @return a string containing the block name.
	 */		
	QString getBlockName() const {return pSharedData->sBlockName;};
	//! \brief setNumberOfTrials slot.
	/*!  This function sets the number of trials for the block to the new provided value.
	 * @param nValue a integer value containing the new number of trials.
	 */		
	void setNumberOfTrials(const int &nValue) {pSharedData->nNrOfTrials = nValue;};
	//! \brief getNumberOfTrials slot.
	/*!  This function returns the number of trials for the block.
	 * @return a integer value containing the number of trials from the block.
	 */		
	int getNumberOfTrials() const {return pSharedData->nNrOfTrials;};
	//! \brief setNumberOfInternalTriggers slot.
	/*!  This function sets the number of internal triggers for the block to the new provided value.
	 * @param nValue a integer value containing the new number of internal triggers.
	 */		
	void setNumberOfInternalTriggers(const int &nValue) {pSharedData->nNrOfInternalTriggers = nValue;};
	//! \brief getNumberOfInternalTriggers slot.
	/*!  This function returns the number of internal triggers for the block.
	 * @return a integer value containing the number of internal triggers from the block.
	 */		
	int getNumberOfInternalTriggers() const {return pSharedData->nNrOfInternalTriggers;};
	//! \brief setNumberOfExternalTriggers slot.
	/*!  This function sets the number of external triggers for the block to the new provided value.
	 * @param nValue a integer value containing the new number of external triggers.
	 */		
	void setNumberOfExternalTriggers(const int &nValue) {pSharedData->nNrOfExternalTriggers = nValue;};
	//! \brief getNumberOfExternalTriggers slot.
	/*!  This function returns the number of external triggers for the block.
	 * @return a integer value containing the number of external triggers from the block.
	 */		
	int getNumberOfExternalTriggers() const {return pSharedData->nNrOfExternalTriggers;};
	//! \brief incrementToNextLoopPointer slot.
	/*!  This function increments and returns the provided cLoopStructure pointer to the next available cLoopStructure pointer.
	 *   If the provided cLoopStructure is NULL, then the internal cLoopStructure is reset and returns the first available cLoopStructure for this block.
	 *   If there's no next available cLoopStructure the this function returns NULL.
	 * @param pCurrentLoop a pointer to a cLoopStructure that should be incremented.
	 * @return a pointer to a cLoopStructure or a NULL value.
	 */		
	cLoopStructure *incrementToNextLoopPointer(cLoopStructure *pCurrentLoop = NULL);
	//! \brief resetToFirstFreeLoopPointer slot.
	/*!  This function resets the provided cLoopStructure to the first free (unprocessed) cLoopStructure and return a pointer to this.
	 * @return a pointer to a cLoopStructure, if there's no available free cLoopStructure the NULL is returned.
	 */		
	cLoopStructure *resetToFirstFreeLoopPointer();
	//! \brief resetAllLoopCounters slot.
	/*!  This function resets all cLoopStructure structures from the block.
	 */		
	void resetAllLoopCounters();
	//! \brief resetAllInnerLoopCounters slot.
	/*!  This function resets all cLoopStructure structures from the block which have loop number(s) smaller than the provided loop number.
	 * @param nCurrentLoopCounter a integer value that holds the first outer loop number that should not be reset by this function.
	 */		
	void resetAllInnerLoopCounters(const int &nCurrentLoopCounter);
	//! \brief insertLoop slot.
	/*!  This function inserts a new cLoopStructure in the block.
	 * @param cLoop a pointer to a cLoopStructure that should be inserted.
	 * @return a boolean value determining whether the new cLoopStructure structure could be inserted.
	 */	
	bool insertLoop(cLoopStructure *cLoop);
	//! \brief insertObjectParameter slot.
	/*!  This function inserts a new cBlockParameterStructure in the block for a specified object by referring to it's object id.
	* @param nObjectID a integer value holding the Object ID for the cBlockParameterStructure that should be inserted.
	* @param cBlockParameter a pointer to a cBlockParameterStructure that should be inserted.
	* @param bIsCustom a boolean value determining whether the parameter is a custom parameter (default=false).
	* @return a boolean value determining whether the new cBlockParameterStructure structure could be inserted.
	*/
	bool insertObjectParameter(int nObjectID, cBlockParameterStructure *cBlockParameter, bool bIsCustom = false);
	//! \brief getNextClosestLoopIDByFromID slot.
	/*!  This function returns a pointer to the next first cLoopStructureloop where the loop ID value is bigger then the provided loop ID.
	 * @param startLoopID a integer value determining the value for the requested cLoopStructureloop where the loop ID value is bigger then this value.
	 * @return a pointer to the next first cLoopStructure structure, if no cLoopStructure is available then this function returns NULL.
	 */		
	cLoopStructure* getNextClosestLoopIDByFromID(const int &startLoopID);
	//! \brief getNextClosestLoopIDByFromLoopNumber slot.
	/*!  This function returns a pointer to the next first cLoopStructureloop where the Loop Number value is bigger or the same as the provided Loop Number.
	 * @param startLoopNumber a integer value determining the value for the requested cLoopStructureloop where the Loop Number value is bigger or the same as this value.
	 * @return a pointer to the next first cLoopStructure structure, if no cLoopStructure is available then this function returns NULL.
	 */		
	cLoopStructure* getNextClosestLoopIDByFromLoopNumber(const int &startLoopNumber);
	//! \brief *  slot.
	/*!  This function returns a pointer to a cLoopStructure specified by the provided loop ID.
	 * @param nLoopID a integer value holding the value for the specified loop ID.
	 * @return a pointer to the cLoopStructure structure specified by the provided loop ID, if it's unavailable then NULL is returned.
	 */		
	cLoopStructure* getLoopPointerByID(const int &nLoopID);
	//! \brief getLoopCount slot.
	/*!  This function return the amount of defined cLoopStructure structures in the block.
	 * @return a integer value holding the amount of defined cLoopStructure structures.
	 */		
	int getLoopCount() const {return pSharedData->lLoops.count();};

public:
	typeMapObjectsBlockParameterContainer *getParameterList(const bool &bIsCustom = false);

private:
	bool Initialize();
	bool isUnusedLoopID(const int &nLoopID) const;

	QSharedDataPointer<cBlockStructure_SharedData> pSharedData;
};

//!  The cMethodConnectionStructure class. 
/*!
  The cMethodConnectionStructure class is a container to store a Method Connection structure from a Object structure, see cObjectStructure.
*/
class cMethodConnectionStructure : public QObject
{
	Q_OBJECT

	//! \brief MethodConnectionID property.
	/*!  Use this property to set or retrieve the Method Connection ID.
    */
	Q_PROPERTY(int MethodConnectionID WRITE setMethodConnectionID READ getMethodConnectionID)
	//! \brief SourceObjectID property.
	/*!  Use this property to set or retrieve the Source Object ID.
    */
	Q_PROPERTY(int SourceObjectID WRITE setSourceObjectID READ getSourceObjectID)
	//! \brief TargetObjectID property.
	/*!  Use this property to set or retrieve the Target Object ID.
    */
	Q_PROPERTY(int TargetObjectID WRITE setTargetObjectID READ getTargetObjectID)
	//! \brief SourceMethodType property.
	/*!  Use this property to set or retrieve the Source Object Method Type.
    */
	Q_PROPERTY(int SourceMethodType WRITE setSourceMethodType READ getSourceMethodType)
	//! \brief TargetMethodType property.
	/*!  Use this property to set or retrieve the Target Object Method Type.
    */
	Q_PROPERTY(int TargetMethodType WRITE setTargetMethodType READ getTargetMethodType)

	//! \brief SourceSignature property.
	/*!  Use this property to set or retrieve the Source Object Signature.
    */
	Q_PROPERTY(QString SourceSignature WRITE setSourceSignature READ getSourceSignature)
	//! \brief TargetSignature property.
	/*!  Use this property to set or retrieve the Target Object Signature.
    */
	Q_PROPERTY(QString TargetSignature WRITE setTargetSignature READ getTargetSignature)

public:
	cMethodConnectionStructure();
	cMethodConnectionStructure(const int &nMethodConnId, const int &nSrcObjectID, const int &nTargObjectID, const int &nSourceMethodType, const int &nTargetMethodType, const QString &sSrcSignature, const QString &sTargSignature);
	cMethodConnectionStructure(const cMethodConnectionStructure& other);
	~cMethodConnectionStructure();

	static QScriptValue ctor__cMethodConnectionStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue MethodConnectionStructureToScriptValue(QScriptEngine *engine, cMethodConnectionStructure* const &s);
	static void MethodConnectionStructureFromScriptValue(const QScriptValue &obj, cMethodConnectionStructure* &s);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	//! \brief setMethodConnectionID slot.
	/*!  This function sets the Method Connection ID to the new provided value.
	 * @param nValue a integer value (>0) holding the new Method Connection ID.
	 */	
	void setMethodConnectionID(const int &nValue) {nMethodConnectionID = nValue;};
	//! \brief getMethodConnectionID slot.
	/*!  This function returns the Method Connection ID.
	 * @return a integer value containing the requested Method Connection ID.
	 */	
	int getMethodConnectionID() const {return nMethodConnectionID;};
	//! \brief setSourceObjectID slot.
	/*!  This function sets the Source Object ID to the new provided value.
	 * @param nValue a integer value (>0) holding the new Source Object ID.
	 */	
	void setSourceObjectID(const int &nValue) {nSourceObjectID = nValue;};
	//! \brief getSourceObjectID slot.
	/*!  This function returns the Source Object ID.
	 * @return a integer value containing the requested Source Object ID.
	 */	
	int getSourceObjectID() const {return nSourceObjectID;};
	//! \brief setSourceMethodType slot.
	/*!  This function sets the Source Object Method Type to the new provided value, see ExperimentStructuresNameSpace::MethodType.
	 * @param nValue a integer value (see ExperimentStructuresNameSpace::MethodType) holding the new Source Object Method Type.
	 */	
	void setSourceMethodType(const int &nValue) {mSourceMethodType = (ExperimentStructuresNameSpace::MethodType)nValue;};
	//! \brief getSourceMethodType slot.
	/*!  This function returns the Source Object Method Type, see ExperimentStructuresNameSpace::MethodType.
	 * @return a integer value (see ExperimentStructuresNameSpace::MethodType) containing the requested Source Object Method Type.
	 */	
	int getSourceMethodType() const {return (int)mSourceMethodType;};
	//! \brief setSourceSignature slot.
	/*!  This function sets the Source Object Method Signature to the new provided value.
	 * @param sValue a string value holding the new Source Object Method Signature.
	 */	
	void setSourceSignature(const QString &sValue) {sSourceSignature = sValue;};
	//! \brief getSourceSignature slot.
	/*!  This function returns the Source Object Method Signature.
	 * @return a string value containing the requested Source Object Method Signature.
	 */	
	QString getSourceSignature() const {return sSourceSignature;};
	//! \brief setTargetObjectID slot.
	/*!  This function sets the Target Object ID to the new provided value.
	 * @param nValue a integer value (>0) holding the new Target Object ID.
	 */	
	void setTargetObjectID(const int &nValue) {nTargetObjectID = nValue;};
	//! \brief getTargetObjectID slot.
	/*!  This function returns the Target Object ID.
	 * @return a integer value containing the requested Target Object ID.
	 */	
	int getTargetObjectID() const {return nTargetObjectID;};
	//! \brief setTargetMethodType slot.
	/*!  This function sets the Target Object Method Type to the new provided value, see ExperimentStructuresNameSpace::MethodType.
	 * @param nValue a integer value (see ExperimentStructuresNameSpace::MethodType) holding the new Target Object Method Type.
	 */	
	void setTargetMethodType(const int &nValue) {mTargetMethodType = (ExperimentStructuresNameSpace::MethodType)nValue;};
	//! \brief getTargetMethodType slot.
	/*!  This function returns the Target Object Method Type, see ExperimentStructuresNameSpace::MethodType.
	 * @return a integer value (see ExperimentStructuresNameSpace::MethodType) containing the requested Target Object Method Type.
	 */	
	int getTargetMethodType() const {return (int)mTargetMethodType;};
	//! \brief setTargetSignature slot.
	/*!  This function sets the Target Object Method Signature to the new provided value.
	 * @param sValue a string value holding the new Target Object Method Signature.
	 */	
	void setTargetSignature(const QString &sValue) {sTargetSignature = sValue;};
	//! \brief getTargetSignature slot.
	/*!  This function returns the Target Object Method Signature.
	 * @return a string value containing the requested Target Object Method Signature.
	 */	
	QString getTargetSignature() const {return sTargetSignature;};

private:
	bool Initialize();

	QScriptEngine* currentScriptEngine;
	int nMethodConnectionID;
	int nSourceObjectID;
	int nTargetObjectID;
	QString sSourceSignature;
	QString sTargetSignature;
	ExperimentStructuresNameSpace::MethodType mSourceMethodType;
	ExperimentStructuresNameSpace::MethodType mTargetMethodType;
};

//!  The cMethodParameterStructure class. 
/*!
  The cMethodParameterStructure class is a container to store a Method Parameter structure that can be used by a Method structure definition, see cMethodStructure.
*/
class cMethodParameterStructure : public QObject
{
	Q_OBJECT

	//! \brief MethodParameterID property.
	/*!  Use this property to set or retrieve the Method Parameter ID.
    */
	Q_PROPERTY(int MethodParameterID WRITE setMethodParameterID READ getMethodParameterID)
	//! \brief MethodParameterName property.
	/*!  Use this property to set or retrieve the Method Parameter Name.
    */
	Q_PROPERTY(QString MethodParameterName WRITE setMethodParameterName READ getMethodParameterName)
	//! \brief MethodParameterType property.
	/*!  Use this property to set or retrieve the Method Parameter Type.
    */
	Q_PROPERTY(QString MethodParameterType WRITE setMethodParameterType READ getMethodParameterType)
	//! \brief MethodParameterValue property.
	/*!  Use this property to set or retrieve the Method Parameter Value.
    */
	Q_PROPERTY(QString MethodParameterValue WRITE setMethodParameterValue READ getMethodParameterValue)

public:
	cMethodParameterStructure();
	cMethodParameterStructure(const int &nMethParamId, const QString &sMethParamName, const QString &sMethParamType, const QString &sMethParamValue);
	cMethodParameterStructure(const cMethodParameterStructure& other);
	~cMethodParameterStructure();

	static QScriptValue ctor__cMethodParameterStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue MethodParameterStructureToScriptValue(QScriptEngine *engine, cMethodParameterStructure* const &s);
	static void MethodParameterStructureFromScriptValue(const QScriptValue &obj, cMethodParameterStructure* &s);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	//! \brief setMethodParameterID slot.
	/*!  This function sets the Method Parameter ID to the new provided value.
	 * @param nValue a integer value (>0) holding the new Method Parameter ID.
	 */	
	void setMethodParameterID(const int &nValue) {nMethodParameterID = nValue;};
	//! \brief getMethodParameterID slot.
	/*!  This function returns the Method Parameter ID.
	 * @return a integer value containing the requested Method Parameter ID.
	 */	
	int getMethodParameterID() const {return nMethodParameterID;};
	//! \brief setMethodParameterName slot.
	/*!  This function sets the Method Parameter Name to the new provided value.
	 * @param sValue a string value holding the new Method Parameter Name.
	 */	
	void setMethodParameterName(const QString &sValue) {sMethodParameterName = sValue;};
	//! \brief getMethodParameterName slot.
	/*!  This function returns the Method Parameter Name.
	 * @return a string value containing the requested Method Parameter Name.
	 */	
	QString getMethodParameterName() const {return sMethodParameterName;};
	//! \brief setMethodParameterType slot.
	/*!  This function sets the MethodParameter Type to the new provided value.
	 * @param sValue a string value holding the new Method Parameter Type.
	 */	
	void setMethodParameterType(const QString &sValue) {sMethodParameterType = sValue;};
	//! \brief getMethodParameterType slot.
	/*!  This function returns the Method Parameter Type.
	 * @return a string value containing the requested Method Parameter Type.
	 */	
	QString getMethodParameterType() const {return sMethodParameterType;};
	//! \brief setMethodParameterValue slot.
	/*!  This function sets the Method Parameter Value to the new provided value.
	 * @param sValue a string value holding the new Method Parameter Value.
	 */	
	void setMethodParameterValue(const QString &sValue) {sMethodParameterValue = sValue;};
	//! \brief getMethodParameterValue slot.
	/*!  This function returns the Method Parameter Value.
	 * @return a string value containing the requested Method Parameter Value.
	 */	
	QString getMethodParameterValue() const {return sMethodParameterValue;};

private:
	bool Initialize();

	QScriptEngine* currentScriptEngine;
	int nMethodParameterID;
	QString sMethodParameterValue;
	QString sMethodParameterType;
	QString sMethodParameterName;
};

//!  The cMethodStructure class. 
/*!
  The cMethodStructure class is a container to store a Method structure from a specific Object structure, see cObjectStructure.
*/
class cMethodStructure : public QObject
{
	Q_OBJECT

	//! \brief MethodID property.
	/*!  Use this property to set or retrieve the Method ID.
    */
	Q_PROPERTY(int MethodID WRITE setMethodID READ getMethodID)
	//! \brief MethodType property.
	/*!  Use this property to set or retrieve the Method Type.
    */
	Q_PROPERTY(int MethodType WRITE setMethodType READ getMethodType)
	//! \brief MethodSignature property.
	/*!  Use this property to set or retrieve the Method Signature.
    */
	Q_PROPERTY(QString MethodSignature WRITE setMethodSignature READ getMethodSignature)
	//! \brief SourceMethodID property.
	/*!  Use this property to set or retrieve the Method Object ID.
    */
	Q_PROPERTY(int SourceObjectID WRITE setMethodObjectID READ getMethodObjectID)
	//! \brief OrderNumber property.
	/*!  Use this property to set or retrieve the Method Order Number.
	*/
	Q_PROPERTY(int OrderNumber WRITE setMethodOrderNumber READ getMethodOrderNumber)


public:
	cMethodStructure();
	cMethodStructure(const int &nMethId, const int &nMethOrderNumber, const int &nMethObjID, const int &nMethType, const QString &sMethSignature);
	cMethodStructure(const cMethodStructure& other);
	~cMethodStructure();

	static QScriptValue ctor__cMethodStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue MethodStructureToScriptValue(QScriptEngine *engine, cMethodStructure* const &s);
	static void MethodStructureFromScriptValue(const QScriptValue &obj, cMethodStructure* &s);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	//! \brief setMethodID slot.
	/*!  This function sets the Method ID to the new provided value.
	 * @param nValue a integer value (>0) holding the new Method  ID.
	 */	
	void setMethodID(const int &nValue) {nMethodID = nValue;};
	//! \brief getMethodID slot.
	/*!  This function returns the Method ID.
	 * @return a integer value containing the requested Method ID.
	 */	
	int getMethodID() const {return nMethodID;};
	//! \brief setMethodeObjectID slot.
	/*!  This function sets the Method Object ID to the new provided value.
	 * @param nValue a integer value (>0) holding the new Method Object ID.
	 */	
	void setMethodObjectID(const int &nValue) {nMethodObjectID = nValue;};
	//! \brief getMethodObjectID slot.
	/*!  This function returns the Method Object ID.
	 * @return a integer value containing the requested Method Object ID.
	 */	
	int getMethodObjectID() const {return nMethodObjectID;};
	//! \brief setMethodOrderNumber slot.
	/*!  This function sets the Method Order Number to the new provided value.
	* @param nValue a integer value (>0) holding the new Method Order Number.
	*/
	void setMethodOrderNumber(const int &nValue) { nMethodOrderNumber = nValue; };
	//! \brief getMethodObjectID slot.
	/*!  This function returns the Method Order Number.
	* @return a integer value containing the requested Method Order Number.
	*/
	int getMethodOrderNumber() const { return nMethodOrderNumber; };
	//! \brief setMethodType slot.
	/*!  This function sets the Method Type to the new provided value, see ExperimentStructuresNameSpace::MethodType.
	 * @param nValue a integer value (see ExperimentStructuresNameSpace::MethodType) holding the new Method Type.
	 */	
	void setMethodType(const int &nValue) {mMethodType = (ExperimentStructuresNameSpace::MethodType)nValue;};
	//! \brief getMethodType slot.
	/*!  This function returns the Method Type, see ExperimentStructuresNameSpace::MethodType.
	 * @return a integer value (see ExperimentStructuresNameSpace::MethodType) containing the requested Method Type.
	 */	
	int getMethodType() const {return (int)mMethodType;};
	//! \brief setMethodSignature slot.
	/*!  This function sets the Method Signature to the new provided value.
	 * @param sValue a string value holding the new Method Signature.
	 */	
	void setMethodSignature(const QString &sValue) {sMethodSignature = sValue;};
	//! \brief getMethodSignature slot.
	/*!  This function returns the Method Signature.
	 * @return a string value containing the requested Method Signature.
	 */	
	QString getMethodSignature() const {return sMethodSignature;};
	//! \brief insertMethodParameter slot.
	/*!  This function inserts a Method Parameter (see ExperimentStructuresNameSpace::cMethodParameterStructure) from the provided data structure.
	 * @param pMethodParamStrcValue a pointer to an ExperimentStructuresNameSpace::cMethodParameterStructure structure holding the Method Parameter data to insert.
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool insertMethodParameter(cMethodParameterStructure *pMethodParamStrcValue);
	//! \brief getMethodParameterList slot.
	/*!  This function returns the inserted Method Parameters List (see cMethodStructure::insertMethodParameter).
	 * @return a list containing the the inserted Method Parameters (see ExperimentStructuresNameSpace::cMethodParameterStructure).
	 */	
	QList<cMethodParameterStructure*> getMethodParameterList() const {return mapParamIDtoParamStruct.values();};
	//! \brief methodTypeStringToInteger slot.
	/*!  This function returns from the provided Method Type String the corresponding Method Type Integer value, see ExperimentStructuresNameSpace::MethodType.
	 * @param sValue a string value holding the new Method Type.
	 * @return a integer value containing the requested Method Type.
	 */	
	static int methodTypeStringToInteger(const QString &sValue)
	{
		if(sValue.toLower() == METHOD_TYPE_SIGNAL_TAG)
			return ExperimentStructuresNameSpace::MethodType::METHOD_TYPE_SIGNAL;
		else if(sValue.toLower() == METHOD_TYPE_SLOT_TAG)
			return ExperimentStructuresNameSpace::MethodType::METHOD_TYPE_SLOT;
		else
			return ExperimentStructuresNameSpace::MethodType::METHOD_TYPE_UNDEFINED;
	};

	//! \brief methodTypeToString slot.
	/*!  This function returns from the provided Method Type the corresponding Method Type String value, see ExperimentStructuresNameSpace::MethodType.
	 * @param sValue a integer value holding the Method Type.
	 * @return a string value containing the requested corresponding Method Type value as string.
	 */	
	static QString methodTypeToString(const int &nValue)
	{
		if(nValue == ExperimentStructuresNameSpace::MethodType::METHOD_TYPE_SIGNAL)
			return METHOD_TYPE_SIGNAL_TAG;
		else if(nValue == ExperimentStructuresNameSpace::MethodType::METHOD_TYPE_SLOT)
			return METHOD_TYPE_SLOT_TAG;
		else
			return "Undefined";
	};

private:
	bool Initialize();

	QScriptEngine* currentScriptEngine;
	int nMethodID;
	int nMethodObjectID;
	int nMethodOrderNumber;
	QString sMethodSignature;
	ExperimentStructuresNameSpace::MethodType mMethodType;
	QMap<int,cMethodParameterStructure*> mapParamIDtoParamStruct;
};


class cObjectStructure_SharedData : public QSharedData
{
public:
	cObjectStructure_SharedData();
	cObjectStructure_SharedData(const cObjectStructure_SharedData &other);
	~cObjectStructure_SharedData();

	QScriptEngine* currentScriptEngine;
	int nObjectID;
	QString sObjectName;
	QString sClassName;
};

//!  The cObjectStructure class. 
/*!
  The cObjectStructure class is a container to store a Object structure from a experiment structure, see cExperimentStructure.
*/
class cObjectStructure : public QObject
{
	Q_OBJECT

	//! \brief ObjectID property.
	/*!  Use this property to set or retrieve the ObjectID.
    */
	Q_PROPERTY(int ObjectID WRITE setObjectID READ getObjectID)
	//! \brief ObjectName property.
	/*!  Use this property to set or retrieve the ObjectName.
    */
	Q_PROPERTY(QString ObjectName WRITE setObjectName READ getObjectName)
	//! \brief ObjectClass property.
	/*!  Use this property to set or retrieve the ObjectClass.
    */
	Q_PROPERTY(QString ObjectClass WRITE setObjectClass READ getObjectClass)
	
public:
	cObjectStructure();
	cObjectStructure(const int &ObjectID,const QString &ObjectName = "",const QString &ObjectClassName = "");
	cObjectStructure(const cObjectStructure& other);
	~cObjectStructure();

	static QScriptValue ctor__cObjectStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue ObjectStructureToScriptValue(QScriptEngine *engine, cObjectStructure* const &s);
	static void ObjectStructureFromScriptValue(const QScriptValue &obj, cObjectStructure* &s);
	
public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script
	
	//! \brief setObjectID slot.
	/*!  This function sets the Object ID to the new provided value.
	 * @param nValue a integer value (>0) containing the new Object ID.
	 */		
	void setObjectID(const int &nValue) {pSharedData->nObjectID = nValue;};
	//! \brief getObjectID slot.
	/*!  This function returns the Object ID.
	 * @return a integer containing the Object ID.
	 */		
	int getObjectID() const {return pSharedData->nObjectID;};
	//! \brief setObjectName slot.
	/*!  This function sets the Object name to the new provided value.
	 * @param sValue a string value containing the new Object name.
	 */		
	void setObjectName(const QString &sValue) {pSharedData->sObjectName = sValue;};
	//! \brief getObjectName slot.
	/*!  This function returns the Object name.
	 * @return a string containing the Object name.
	 */		
	QString getObjectName() const {return pSharedData->sObjectName;};
	//! \brief setObjectClass slot.
	/*!  This function sets the Object Class name to the new provided value.
	 * @param sValue a string value containing the new Object Class name.
	 */		
	void setObjectClass(const QString &sValue) {pSharedData->sClassName = sValue;};
	//! \brief getObjectClass slot.
	/*!  This function returns the Object Class name.
	 * @return a string containing the Object Class name.
	 */		
	QString getObjectClass() const {return pSharedData->sClassName;};

private:
	bool Initialize();
	QSharedDataPointer<cObjectStructure_SharedData> pSharedData;
};


class cExperimentStructure_SharedData : public QSharedData
{
public:
	cExperimentStructure_SharedData();
	cExperimentStructure_SharedData(const cExperimentStructure_SharedData &other);
	~cExperimentStructure_SharedData();

	void clearInternalDataStructures();

	ExperimentStructuresNameSpace::ExperimentRunState CurrentExperiment_RunState;			//Only for internal usage
	ExperimentStructuresNameSpace::strcExperimentStructureState currentExperimentState;
	QScriptEngine* currentScriptEngine;
	int nExperimentID;
	QString sExperimentName;
	bool bDebugMode;
	QList<cBlockStructure*> lBlocks;
	QList<cObjectStructure*> lObjects;
	QMap<int, QList<cMethodConnectionStructure*>> mObjectIdToMethodConnections;		//key=ObjectId
	QMap<int, QList<cMethodStructure*>> mObjectIdToInitializations;					//key=ObjectId
	QMap<int, QList<cMethodStructure*>> mObjectIdToFinalizations;					//key=ObjectId
	cBlockStructure *currentBlockPointer;											//Only for internal usage
	cLoopStructure *currentLoopPointer;												//Only for internal usage
	cBlockStructure *firstBlockPointer;												//Only for internal usage
};


//!  The cExperimentStructure class. 
/*!
  The cExperimentStructure class is a container to store the experiment structure, it can be created/changed dynamically or loaded from a EXML file and is internally used by the ExperimentManager to store the Experiment Structure in memory.
*/
class cExperimentStructure : public QObject, protected QScriptable
{
	Q_OBJECT
	//Q_CLASSINFO("ScriptAPIClassName", "cExperimentStructure");//Can't use defines here!, moc doesn't handle defines, not needed here
	
	//! \brief ExperimentName property.
	/*!  Use this property to set or retrieve the Experiment Name.
    */	
	Q_PROPERTY(QString ExperimentName WRITE setExperimentName READ getExperimentName)
	//! \brief ExperimentID property.
	/*!  Use this property to set or retrieve the Experiment ID.
    */	
	Q_PROPERTY(int ExperimentID WRITE setExperimentID READ getExperimentID)
	//! \brief ExperimentDebugMode property.
	/*!  Use this property to set or retrieve the Experiment Debug Mode.
    */	
	Q_PROPERTY(bool ExperimentDebugMode WRITE setExperimentDebugMode READ getExperimentDebugMode)

signals:
	//! \brief experimentStarted() signal.
	/*!  This signal is emitted when the Experiment is started.
	 */
	void experimentStarted();
	//! \brief externalTriggerRecieved() signal.
	/*!  This signal is emitted when the Experiment Manager/Engine receives an external trigger.
	 */
	void externalTriggerRecieved();
	//! \brief experimentStopped() signal.
	/*!  This signal is emitted when the Experiment is stopped.
	 */
	void experimentStopped();

public:
	cExperimentStructure();
	cExperimentStructure(const cExperimentStructure& other);
	~cExperimentStructure();

	static QScriptValue ctor__cExperimentStructure(QScriptContext* context, QScriptEngine* engine);
	static QScriptValue experimentStructureToScriptValue(QScriptEngine *engine, cExperimentStructure* const &s);
	static void experimentStructureFromScriptValue(const QScriptValue &obj, cExperimentStructure* &s);
	static QScriptValue createExperimentStructureStateFromScript(QScriptContext *, QScriptEngine *engine);
	static QScriptValue experimentStructureStateToScriptValue(QScriptEngine *engine, const ExperimentStructuresNameSpace::strcExperimentStructureState &s);
	static void resetExperimentStateStruct(ExperimentStructuresNameSpace::strcExperimentStructureState *strcExpState);
	static void experimentStructureStateFromScriptValue(const QScriptValue &obj, ExperimentStructuresNameSpace::strcExperimentStructureState &s);
	cBlockStructure* getNextClosestBlockNumberByFromNumber(const int &startBlockNumber); 
	QList<cObjectStructure*> &getObjectList();
	QList<cBlockStructure*> &getBlockList();
	cBlockStructure getCurrentBlock(bool &bHasCurrBlock) const;//This doesn't work within the script, see reference...

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	//! \brief resetExperiment slot.
	/*!  This function resets the current Experiment Structure state, see ExperimentStructuresNameSpace::strcExperimentStructureState.
	 */
	void resetExperimentState() {resetExperimentStateStruct(&pSharedData->currentExperimentState);};
	//! \brief clearExperiment slot.
	/*!  This function removes all settings in the current Experiment Structure.
	*/
	void clearExperiment();
	//! \brief getCurrentExperimentState slot.
	/*!  This function returns a copy of the current Experiment Structure state, see ExperimentStructuresNameSpace::strcExperimentStructureState.
	 * @return a strcExperimentStructureState structure .
	 */	
	ExperimentStructuresNameSpace::strcExperimentStructureState getCurrentExperimentState() const {return pSharedData->currentExperimentState;};
	//! \brief setExperimentName slot.
	/*!  This function sets the Experiment Name.
	 * @param sValue a string value containing the new Experiment File Name.
	 */	
	void setExperimentName(const QString &sValue) {pSharedData->sExperimentName = sValue;};
	//! \brief getExperimentName slot.
	/*!  This function returns the current Experiment Name.
	 * @return a string value containing the Experiment Name.
	 */	
	QString getExperimentName() const {return pSharedData->sExperimentName;};
	//! \brief setExperimentID slot.
	/*!  This function sets the Experiment ID.
	 * @param nValue a integer value containing the new Experiment ID.
	 */	
	void setExperimentID(const int &nValue) {pSharedData->nExperimentID = nValue;};
	//! \brief getExperimentID slot.
	/*!  This function returns the current Experiment ID.
	 * @return a integer value containing the Experiment ID.
	 */	
	int getExperimentID() const {return pSharedData->nExperimentID;};
	//! \brief setExperimentDebugMode slot.
	/*!  This function configures the Experiment Debug mode.
	 * @param bValue a boolean value determining whether the debug mode should be enabled.
	 */	
	void setExperimentDebugMode(const bool &bValue) {pSharedData->bDebugMode = bValue;};
	//! \brief getExperimentDebugMode slot.
	/*!  This function returns the current Experiment Debug mode.
	 * @return a boolean value determining whether the current Experiment Debug mode is enabled.
	 */	
	bool getExperimentDebugMode() const {return pSharedData->bDebugMode;};
	//! \brief incrementExternalTrigger slot.
	/*!  This function increments the current Experiment External trigger with 1. 
	 *   It is used to control the Experiment Structure.
	 */	
	void incrementExternalTrigger();
	//! \brief prepareExperiment slot.
	/*!  This function prepares the current Experiment so it can be started.
	 * @param bIgnoreNoDefinedBlocks a boolean value (default = false), if set to true than the function returns true even when there are no blocks yet defined.
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool prepareExperiment(const bool &bIgnoreNoDefinedBlocks = false);
	//! \brief startExperiment slot.
	/*!  This function starts the current Experiment and will then wait for an external trigger, see cExperimentStructure::incrementExternalTrigger().
	 */	
	void startExperiment() {ExperimentStart();};
	//! \brief insertBlock slot.
	/*!  This function inserts a new cBlockStructure structure in the current Experiment Structure.
	 * @param cBlock a pointer to a cBlockStructure that needs to be inserted.
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool insertBlock(cBlockStructure *cBlock);
	//! \brief insertObject slot.
	/*!  This function inserts a new cObjectStructure structure in the current Experiment Structure.
	 * @param cObject a pointer to a cObjectStructure that needs to be inserted.
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool insertObject(cObjectStructure *cObject);
	//! \brief insertObjectMethodConnection slot.
	/*!  This function inserts a new cMethodConnection structure in the current Experiment Structure.
	 * @param cMethodConnection a pointer to a cMethodConnectionStructure that needs to be inserted.
	 * @return a boolean value determining whether the function executed successfully.
	 */	
	bool insertObjectMethodConnection(cMethodConnectionStructure *pMethodConnection);
	//! \brief getObjectMethodConnectionList slot.
	/*!  This function returns a pointer to a cMethodConnectionStructure List identified by a object ID.
	 * @param nObjectID a integer value defining the Object ID from the requested Object Method connection structure list pointer.
	 * @return a pointer to a cMethodConnectionStructure Structure List.
	 */	
	QList<cMethodConnectionStructure*> *getObjectMethodConnectionList(const int &nObjectID);
	//! \brief insertObjectInitialization slot.
	/*!  This function inserts a new cMethodStructure structure initialization method in the current Experiment Structure.
	* @param pObjectInitialization a pointer to a cMethodStructure that needs to be inserted.
	* @return a boolean value determining whether the function executed successfully.
	*/
	bool insertObjectInitialization(cMethodStructure *pObjectInitialization);
	//! \brief getObjectInitializationList slot.
	/*!  This function returns a pointer to a cMethodStructure List containing all initialization methods for a specified object ID.
	* @return a pointer to a cMethodStructure Structure List.
	*/
	QList<cMethodStructure*> *getObjectInitializationList(const int &nObjectID);
	//! \brief insertObjectFinalization slot.
	/*!  This function inserts a new cMethodStructure structure finalization method in the current Experiment Structure.
	* @param pObjectFinalization a pointer to a cMethodStructure that needs to be inserted.
	* @return a boolean value determining whether the function executed successfully.
	*/
	bool insertObjectFinalization(cMethodStructure *pObjectFinalization);
	//! \brief getObjectFinalizationList slot.
	/*!  This function returns a pointer to a cMethodStructure List containing all finalization methods for a specified object ID.
	* @return a pointer to a cMethodStructure Structure List.
	*/
	QList<cMethodStructure*> *getObjectFinalizationList(const int &nObjectID);
	//! \brief getBlockPointerByID slot.
	/*!  This function returns a pointer to a cBlockStructure identified by a block ID.
	 * @param nBlockID a integer value defining the block ID from the requested Block pointer.
	 * @return a pointer to a cBlockStructure structure .
	 */	
	cBlockStructure* getBlockPointerByID(const int &nBlockID);
	//! \brief getObjectPointerByID slot.
	/*!  This function returns a pointer to a cObjectStructure identified by a Object ID.
	 * @param nObjectID a integer value defining the Object ID from the requested Object pointer.
	 * @return a pointer to a cObjectStructure structure .
	 */	
	cObjectStructure* getObjectPointerByID(const int &nObjectID);
	//! \brief getBlockCount slot.
	/*!  This function return the number of defined blocks in the current experiment.
	 * @return a integer value holding the experiment block count.
	 */	
	int getBlockCount() const;
	//! \brief getObjectCount slot.
	/*!  This function return the number of defined Objects in the current experiment.
	 * @return a integer value holding the experiment Object count.
	 */	
	int getObjectCount() const;
	//! \brief getExternalTriggerCount slot.
	/*!  This function returns the total number of received external triggers from the experiment since it was started.
	 * @return a integer value containing the received experiment external triggers.
	 */	
	int getExternalTriggerCount() const;

private:
	void ExperimentAbort();
	void ExperimentStop();
	void ExperimentStart();
	bool Initialize();
	bool isUnusedBlockID(const int &nBlockID) const;
	bool isUnusedObjectID(const int &nObjectID) const;
	bool isValidBlockPointer(cBlockStructure *cBlock) const;
	bool isValidObjectPointer(cObjectStructure *cObject) const;
	bool prepareStartBlock();
	int getBlockIndexByID(const int &nBlockID) const;
	int getObjectIndexByID(const int &nObjectID) const;
	int getCurrentBlockIndex() const;
	
	QSharedDataPointer<cExperimentStructure_SharedData> pSharedData;
};

Q_DECLARE_METATYPE(ExperimentStructuresNameSpace::strcExperimentStructureState)
Q_DECLARE_METATYPE(cExperimentStructure*)
Q_DECLARE_METATYPE(cBlockStructure*)
Q_DECLARE_METATYPE(cObjectStructure*)
Q_DECLARE_METATYPE(cLoopStructure*)
Q_DECLARE_METATYPE(cMethodConnectionStructure*)
Q_DECLARE_METATYPE(cMethodParameterStructure*)
Q_DECLARE_METATYPE(cBlockParameterStructure*)
Q_DECLARE_METATYPE(cMethodStructure*)

#endif // EXPERIMENTSTRUCTURES_H
