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

#include "experimentstructures.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cBlockStructure_SharedData::cBlockStructure_SharedData()
{
	currentScriptEngine = NULL;
	nBlockID = 0;
	nBlockNumber = 0;
	sBlockName = "Block(ID=" + QString::number(nBlockID) + ",Number=" + QString::number(nBlockNumber) + ")";
	nNrOfTrials = ExperimentStructuresNameSpace::RA_ZERO;
	nNrOfInternalTriggers = ExperimentStructuresNameSpace::RA_ZERO;
	nNrOfExternalTriggers = ExperimentStructuresNameSpace::RA_ZERO;
}

cBlockStructure_SharedData::cBlockStructure_SharedData(const cBlockStructure_SharedData &other) : QSharedData(other)
{
	currentScriptEngine = other.currentScriptEngine;
	nBlockID = other.nBlockID;
	nBlockNumber = other.nBlockNumber;
	sBlockName = other.sBlockName;
	nNrOfTrials = other.nNrOfTrials;
	nNrOfInternalTriggers = other.nNrOfInternalTriggers;
	nNrOfExternalTriggers = other.nNrOfExternalTriggers;

	if(other.lLoops.isEmpty() == false)
	{
		for (int i=0;i<other.lLoops.count();i++)
		{
			lLoops.append(other.lLoops[i]);
		}
	}
}

cBlockStructure_SharedData::~cBlockStructure_SharedData()
{	
	if(lLoops.isEmpty() == false)
	{
		for (int i=0;i<lLoops.count();i++)
		{
			delete lLoops[i];//->deleteLater();
			lLoops[i] = NULL;
		}
		lLoops.clear();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The cBlockStructure constructor.
*
*   No parameter
*/
cBlockStructure::cBlockStructure()
{
	pSharedData = new cBlockStructure_SharedData();
}

cBlockStructure::cBlockStructure(const cBlockStructure& other) : pSharedData(other.pSharedData)
{

}

bool cBlockStructure::Initialize()
{
	pSharedData->nBlockID = 0;
	pSharedData->nBlockNumber = 0;
	pSharedData->sBlockName = "Block(ID=" + QString::number(pSharedData->nBlockID) + ",Number=" + QString::number(pSharedData->nBlockNumber) + ")";
	pSharedData->nNrOfTrials = ExperimentStructuresNameSpace::RA_ZERO;
	pSharedData->nNrOfInternalTriggers = ExperimentStructuresNameSpace::RA_ZERO;
	pSharedData->nNrOfExternalTriggers = ExperimentStructuresNameSpace::RA_ZERO;
	return true;
}

cBlockStructure::cBlockStructure(const int &BlockID,const int &BlockNumber,const QString &BlockName,const int &NumberOfTrials)
{
	pSharedData = new cBlockStructure_SharedData();
	pSharedData->nBlockID = BlockID;
	pSharedData->nBlockNumber = BlockNumber;
	pSharedData->sBlockName = BlockName;
	pSharedData->nNrOfTrials = NumberOfTrials;
	pSharedData->currentScriptEngine = NULL;
	if(pSharedData->sBlockName == "")
		pSharedData->sBlockName = "Block(ID=" + QString::number(pSharedData->nBlockID) + ",Number=" + QString::number(pSharedData->nBlockNumber) + ")";
};

/*! \brief The cBlockStructure destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
cBlockStructure::~cBlockStructure()
{	

}

QScriptValue cBlockStructure::ctor__cBlockStructure(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new cBlockStructure(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue cBlockStructure::blockStructureToScriptValue(QScriptEngine *engine, cBlockStructure* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void cBlockStructure::blockStructureFromScriptValue(const QScriptValue &obj, cBlockStructure* &s)
{
	s = qobject_cast<cBlockStructure*>(obj.toQObject());
}

bool cBlockStructure::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		pSharedData->currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		QScriptValue objectValue = pSharedData->currentScriptEngine->newQObject(this);
		pSharedData->currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

bool cBlockStructure::isUnusedLoopID(const int &nLoopID) const
{
	if(pSharedData->lLoops.isEmpty())
		return true;
	for (int i=0;i<pSharedData->lLoops.size();i++) 
	{
		if (pSharedData->lLoops.at(i)->getLoopID() == nLoopID)
			return false;
	}
	return true;
}

bool cBlockStructure::insertLoop(cLoopStructure *cLoop)
{
	if(isUnusedLoopID(cLoop->getLoopID()))
	{
		pSharedData->lLoops.append(cLoop);
		return true;
	}
	return false;
}

cLoopStructure *cBlockStructure::resetToFirstFreeLoopPointer()
{
	if(pSharedData->lLoops.isEmpty() == false)
	{
		int nCount;
		for(int i=0;i<pSharedData->lLoops.count();i++)
		{
			nCount = pSharedData->lLoops.at(i)->getLoopCounter();
			if(nCount >= 0)
			{
				return pSharedData->lLoops[i];
			}
			else if(nCount==ExperimentStructuresNameSpace::LCE_UNUSED)
			{
				if(pSharedData->lLoops.at(i)->getNumberOfLoops() > 0)
				{
					return pSharedData->lLoops[i];
				}
				else
				{
					pSharedData->lLoops.at(i)->finalizeCurrentLoopCounter();
				}
			}
		}
	}
	return NULL;
}

cLoopStructure *cBlockStructure::incrementToNextLoopPointer(cLoopStructure *pCurrentLoop)
{
	if(pSharedData->lLoops.isEmpty() == false)
	{
		if(pCurrentLoop == NULL)
			return resetToFirstFreeLoopPointer();//just return the first Loop
		if(pCurrentLoop->incrementCurrentLoopCounter() >= 0)
		{
			return pCurrentLoop;
		}
		bool bReturnNextPointer = false;
		for(int i=0;i<pSharedData->lLoops.count();i++)
		{
			if(bReturnNextPointer)
			{
				if(pSharedData->lLoops[i]->initializeCurrentLoopCounter() >= ExperimentStructuresNameSpace::LCE_FIRSTLOOP)   //initializeCurrentLoopCounter())//Initialization of the new Loop?
					return pSharedData->lLoops[i];
				return incrementToNextLoopPointer(pSharedData->lLoops[i]);//NULL
			}
			if(pSharedData->lLoops[i] == pCurrentLoop)
				bReturnNextPointer = true;
		}
	}
	return NULL;//This means that all Loops are processed and we can take the exit
}

void cBlockStructure::resetAllLoopCounters()
{
	if(pSharedData->lLoops.isEmpty() == false)
	{
		for(int i=0;i<pSharedData->lLoops.count();i++)
		{
			pSharedData->lLoops[i]->resetCurrentLoopCounter();
		}
	}
}

void cBlockStructure::resetAllInnerLoopCounters(const int &nCurrentLoopCounter)
{
	if(pSharedData->lLoops.isEmpty() == false)
	{
		for(int i=0;i<pSharedData->lLoops.count();i++)
		{
			if(pSharedData->lLoops[i]->getLoopNumber() < nCurrentLoopCounter)
			{
				pSharedData->lLoops[i]->resetCurrentLoopCounter();
			}
		}
	}
}

cLoopStructure* cBlockStructure::getLoopPointerByID(const int &nLoopID)
{
	if(pSharedData->lLoops.isEmpty())//Are there any Loops defined?
		return NULL;
	for (int i=0;i<pSharedData->lLoops.size();i++) 
	{
		if(pSharedData->lLoops[i]->getLoopID() == nLoopID)
			return pSharedData->lLoops[i];
	}
	return NULL;
}

cLoopStructure* cBlockStructure::getNextClosestLoopIDByFromID(const int &startLoopID)
{
	//result is an cLoopStructure* with ID=startLoopID or first closest higher value
	if(pSharedData->lLoops.isEmpty())//Are there any Loops defined?
		return NULL;
	//First try the expected location
	if(pSharedData->lLoops.size()>startLoopID)
		if (pSharedData->lLoops.at(startLoopID)->getLoopID() == startLoopID)
			return pSharedData->lLoops[startLoopID];
	//Now try the other items
	int closestIndex = -1;
	int closestLoopID = startLoopID;//This shouldn't matter
	int tmpLoopID;
	bool bAcceptAny = true;//Makes sure to accept any valid value from the start of the search
	for (int i=0;i<pSharedData->lLoops.size();i++) 
	{
		tmpLoopID = pSharedData->lLoops.at(i)->getLoopID();
		if(tmpLoopID == startLoopID)
		{//We found it although it was not at its expected location
			return pSharedData->lLoops[i];
		}
		else if(tmpLoopID > startLoopID)
		{//We found a larger block number...
			if(bAcceptAny || (tmpLoopID < closestLoopID))//First one or a more closer(thus smaller) number?
			{
				closestLoopID = tmpLoopID;
				closestIndex = i;
				bAcceptAny = false;
			}
		}
	}
	if(closestIndex>=0)//Do we have an closest result?
		return pSharedData->lLoops[closestIndex];
	else
		return NULL;
}

cLoopStructure* cBlockStructure::getNextClosestLoopIDByFromLoopNumber(const int &startLoopNumber)
{
	//result is an cLoopStructure* with ID=startLoopNumber or first closest higher value
	if(pSharedData->lLoops.isEmpty())//Are there any Loops defined?
		return NULL;
	//First try the expected location
	if(pSharedData->lLoops.size()>startLoopNumber)
		if (pSharedData->lLoops.at(startLoopNumber)->getLoopNumber() == startLoopNumber)
			return pSharedData->lLoops[startLoopNumber];
	//Now try the other items
	int closestIndex = -1;
	int closestLoopNumber = startLoopNumber;//This shouldn't matter
	int tmpLoopNumber;
	bool bAcceptAny = true;//Makes sure to accept any valid value from the start of the search
	for (int i=0;i<pSharedData->lLoops.size();i++) 
	{
		tmpLoopNumber = pSharedData->lLoops.at(i)->getLoopNumber();
		if(tmpLoopNumber == startLoopNumber)
		{//We found it although it was not at its expected location
			return pSharedData->lLoops[i];
		}
		else if(tmpLoopNumber > startLoopNumber)
		{//We found a larger block number...
			if(bAcceptAny || (tmpLoopNumber < closestLoopNumber))//First one or a more closer(thus smaller) number?
			{
				closestLoopNumber = tmpLoopNumber;
				closestIndex = i;
				bAcceptAny = false;
			}
		}
	}
	if(closestIndex>=0)//Do we have an closest result?
		return pSharedData->lLoops[closestIndex];
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cObjectStructure_SharedData::cObjectStructure_SharedData()
{
	currentScriptEngine = NULL;
	nObjectID = 0;
	sObjectName = "Object(ID=" + QString::number(nObjectID) + ")";
	sClassName = "";
}

cObjectStructure_SharedData::cObjectStructure_SharedData(const cObjectStructure_SharedData &other) : QSharedData(other)
{
	currentScriptEngine = other.currentScriptEngine;
	nObjectID = other.nObjectID;
	sObjectName = other.sObjectName;
	sClassName = other.sClassName;
}

cObjectStructure_SharedData::~cObjectStructure_SharedData()
{	

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The cObjectStructure constructor.
*
*   No parameter
*/
cObjectStructure::cObjectStructure()
{
	pSharedData = new cObjectStructure_SharedData();
}

cObjectStructure::cObjectStructure(const cObjectStructure& other) : pSharedData(other.pSharedData)
{

}

bool cObjectStructure::Initialize()
{
	pSharedData->nObjectID = 0;
	pSharedData->sObjectName = "Object(ID=" + QString::number(pSharedData->nObjectID) + ")";
	pSharedData->sClassName = "";
	return true;
}

cObjectStructure::cObjectStructure(const int &ObjectID, const QString &ObjectName, const QString &ObjectClassName)
{
	pSharedData = new cObjectStructure_SharedData();
	pSharedData->nObjectID = ObjectID;
	pSharedData->sObjectName = ObjectName;
	pSharedData->sClassName = ObjectClassName;
	if(pSharedData->sObjectName == "")
		pSharedData->sObjectName = "Object(ID=" + QString::number(pSharedData->nObjectID) + ")";
};

/*! \brief The cObjectStructure destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
cObjectStructure::~cObjectStructure()
{	

}

QScriptValue cObjectStructure::ctor__cObjectStructure(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new cObjectStructure(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue cObjectStructure::ObjectStructureToScriptValue(QScriptEngine *engine, cObjectStructure* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void cObjectStructure::ObjectStructureFromScriptValue(const QScriptValue &obj, cObjectStructure* &s)
{
	s = qobject_cast<cObjectStructure*>(obj.toQObject());
}

bool cObjectStructure::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		pSharedData->currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		QScriptValue objectValue = pSharedData->currentScriptEngine->newQObject(this);
		pSharedData->currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The cLoopStructure constructor.
*
*   No parameter
*/
cLoopStructure::cLoopStructure()
{
	currentScriptEngine = NULL;
	this->Initialize();
	sLoopName = "Loop(ID=" + QString::number(nLoopID) + ",Number=" + QString::number(nLoopNumber) + ")";
}

cLoopStructure::cLoopStructure(const cLoopStructure& other)
{
	currentScriptEngine = other.currentScriptEngine;
	nLoopID = other.nLoopID;
	nLoopNumber = other.nLoopNumber;
	sLoopName = other.sLoopName;
	nNrOfLoops = other.nNrOfLoops;
	nTargetBlockID = other.nTargetBlockID;
	nLoopCounter = other.nLoopCounter;
}

bool cLoopStructure::Initialize()
{
	nLoopID = 0;
	nLoopNumber = 0;
	nLoopCounter = ExperimentStructuresNameSpace::LCE_UNUSED;
	nNrOfLoops = ExperimentStructuresNameSpace::RA_ZERO;
	nTargetBlockID = ExperimentStructuresNameSpace::RA_UNDEFINED;
	return true;
}

cLoopStructure::cLoopStructure(const int &LoopID,const int &LoopNumber,const int &LoopTargetBlockID,const QString &LoopName,const int &NumberOfLoops) : nLoopID(LoopID), nLoopNumber(LoopNumber), nTargetBlockID(LoopTargetBlockID), sLoopName(LoopName), nNrOfLoops(NumberOfLoops)
{
	currentScriptEngine = NULL;
	nLoopCounter = ExperimentStructuresNameSpace::LCE_UNUSED;
	if(sLoopName == "")
		sLoopName = "Loop(ID=" + QString::number(nLoopID) + ",Number=" + QString::number(nLoopNumber) + ")";
};

/*! \brief The cLoopStructure destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
cLoopStructure::~cLoopStructure()
{	
}

QScriptValue cLoopStructure::ctor__cLoopStructure(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new cLoopStructure(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue cLoopStructure::loopStructureToScriptValue(QScriptEngine *engine, cLoopStructure* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void cLoopStructure::loopStructureFromScriptValue(const QScriptValue &obj, cLoopStructure* &s)
{
	s = qobject_cast<cLoopStructure*>(obj.toQObject());
}

bool cLoopStructure::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
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

int cLoopStructure::initializeCurrentLoopCounter()
{
	if(nLoopCounter==ExperimentStructuresNameSpace::LCE_FINISHED)
	{
		nLoopCounter = ExperimentStructuresNameSpace::LCE_FINISHED;
	}
	else if(nLoopCounter==ExperimentStructuresNameSpace::LCE_UNUSED)
	{
		nLoopCounter = ExperimentStructuresNameSpace::LCE_FIRSTLOOP;
	}
	else if(nLoopCounter==ExperimentStructuresNameSpace::LCE_FIRSTLOOP)//Here's the difference with function incrementCurrentLoopCounter()!
	{
		nLoopCounter++;
	}
	else if((nLoopCounter+1) < nNrOfLoops)
	{
		nLoopCounter++;
	}
	else
	{
		nLoopCounter = ExperimentStructuresNameSpace::LCE_FINISHED;
	}
	return nLoopCounter;
}

int cLoopStructure::incrementCurrentLoopCounter() 
{
	if(nLoopCounter==ExperimentStructuresNameSpace::LCE_FINISHED)
	{
		nLoopCounter = ExperimentStructuresNameSpace::LCE_FINISHED;
	}
	else if(nLoopCounter==ExperimentStructuresNameSpace::LCE_UNUSED)
	{
		nLoopCounter = ExperimentStructuresNameSpace::LCE_FIRSTLOOP;
	}
	else if((nLoopCounter+1) < nNrOfLoops)
	{
		nLoopCounter++;
	}
	else
	{
		nLoopCounter = ExperimentStructuresNameSpace::LCE_FINISHED;
	}
	return nLoopCounter;
}

void cLoopStructure::resetCurrentLoopCounter() 
{
	nLoopCounter = ExperimentStructuresNameSpace::LCE_UNUSED;
}

void cLoopStructure::finalizeCurrentLoopCounter() 
{
	nLoopCounter = ExperimentStructuresNameSpace::LCE_FINISHED;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The cMethodConnectionStructure constructor.
*
*   No parameter
*/
cMethodConnectionStructure::cMethodConnectionStructure()
{
	currentScriptEngine = NULL;
	this->Initialize();
}

cMethodConnectionStructure::cMethodConnectionStructure(const cMethodConnectionStructure& other)
{
	currentScriptEngine = other.currentScriptEngine;
	nMethodConnectionID = other.nMethodConnectionID;
	nSourceObjectID = other.nSourceObjectID;
	nTargetObjectID = other.nTargetObjectID;
	sSourceSignature = other.sSourceSignature;
	sTargetSignature = other.sTargetSignature;
	mSourceMethodType = other.mSourceMethodType;
	mTargetMethodType = other.mTargetMethodType;
}

bool cMethodConnectionStructure::Initialize()
{
	nMethodConnectionID = -1;
	nSourceObjectID = -1;
	nTargetObjectID = -1;
	sSourceSignature = "";
	sTargetSignature = "";
	mSourceMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_UNDEFINED;
	mTargetMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_UNDEFINED;
	return true;
}

cMethodConnectionStructure::cMethodConnectionStructure(const int &nMethodConnId, const int &nSrcObjectID, const int &nTargObjectID, const int &nSourceMethodType, const int &nTargetMethodType, const QString &sSrcSignature, const QString &sTargSignature)
{
	currentScriptEngine = NULL;
	nMethodConnectionID = nMethodConnId;
	nSourceObjectID = nSrcObjectID;
	nTargetObjectID = nTargObjectID;
	sSourceSignature = sSrcSignature;
	sTargetSignature = sTargSignature;
	mSourceMethodType = (ExperimentStructuresNameSpace::MethodType) nSourceMethodType;
	mTargetMethodType = (ExperimentStructuresNameSpace::MethodType) nTargetMethodType;
};

/*! \brief The cMethodConnectionStructure destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
cMethodConnectionStructure::~cMethodConnectionStructure()
{	
}

QScriptValue cMethodConnectionStructure::ctor__cMethodConnectionStructure(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new cMethodConnectionStructure(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue cMethodConnectionStructure::MethodConnectionStructureToScriptValue(QScriptEngine *engine, cMethodConnectionStructure* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void cMethodConnectionStructure::MethodConnectionStructureFromScriptValue(const QScriptValue &obj, cMethodConnectionStructure* &s)
{
	s = qobject_cast<cMethodConnectionStructure*>(obj.toQObject());
}

bool cMethodConnectionStructure::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The cMethodParameterStructure constructor.
*
*   No parameter
*/
cMethodParameterStructure::cMethodParameterStructure()
{
	currentScriptEngine = NULL;
	this->Initialize();
}

cMethodParameterStructure::cMethodParameterStructure(const cMethodParameterStructure& other)
{
	currentScriptEngine = other.currentScriptEngine;
	nMethodParameterID = other.nMethodParameterID;
	sMethodParameterValue = other.sMethodParameterValue;
	sMethodParameterType = other.sMethodParameterType;
	sMethodParameterName = other.sMethodParameterName;
}

bool cMethodParameterStructure::Initialize()
{
	nMethodParameterID = -1;
	sMethodParameterValue = "";
	sMethodParameterType = "";
	sMethodParameterName = "";
	return true;
}

cMethodParameterStructure::cMethodParameterStructure(const int &nMethParamId, const QString &sMethParamName, const QString &sMethParamType, const QString &sMethParamValue)
{
	currentScriptEngine = NULL;
	nMethodParameterID = nMethParamId;
	sMethodParameterName = sMethParamName;
	sMethodParameterType = sMethParamType;
	sMethodParameterValue = sMethParamValue;
};

/*! \brief The cMethodParameterStructure destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
cMethodParameterStructure::~cMethodParameterStructure()
{	
}

QScriptValue cMethodParameterStructure::ctor__cMethodParameterStructure(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new cMethodParameterStructure(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue cMethodParameterStructure::MethodParameterStructureToScriptValue(QScriptEngine *engine, cMethodParameterStructure* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void cMethodParameterStructure::MethodParameterStructureFromScriptValue(const QScriptValue &obj, cMethodParameterStructure* &s)
{
	s = qobject_cast<cMethodParameterStructure*>(obj.toQObject());
}

bool cMethodParameterStructure::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The cMethodStructure constructor.
*
*   No parameter
*/
cMethodStructure::cMethodStructure()
{
	currentScriptEngine = NULL;
	this->Initialize();
}

cMethodStructure::cMethodStructure(const cMethodStructure& other)
{
	currentScriptEngine = other.currentScriptEngine;
	nMethodID = other.nMethodID;
	nMethodObjectID = other.nMethodObjectID;
	nMethodOrderNumber = other.nMethodOrderNumber;
	sMethodSignature = other.sMethodSignature;
	mMethodType = other.mMethodType;
	mapParamIDtoParamStruct = other.mapParamIDtoParamStruct;
}

bool cMethodStructure::Initialize()
{
	nMethodID = -1;
	nMethodObjectID = -1;
	nMethodOrderNumber = -1;
	sMethodSignature = "";
	mMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_UNDEFINED;
	return true;
}

cMethodStructure::cMethodStructure(const int &nMethId, const int &nMethOrderNumber, const int &nMethObjID, const int &nMethType, const QString &sMethSignature)
{
	currentScriptEngine = NULL;
	nMethodID = nMethId;
	nMethodObjectID = nMethObjID;
	nMethodOrderNumber = nMethOrderNumber;
	sMethodSignature = sMethSignature;
	mMethodType = (ExperimentStructuresNameSpace::MethodType) nMethType;
};

/*! \brief The cMethodStructure destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
cMethodStructure::~cMethodStructure()
{	
	foreach(cMethodParameterStructure *pMethStrc, mapParamIDtoParamStruct)
	{
		delete pMethStrc;
		pMethStrc = NULL;
	}
	mapParamIDtoParamStruct.clear();
}

QScriptValue cMethodStructure::ctor__cMethodStructure(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new cMethodStructure(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue cMethodStructure::MethodStructureToScriptValue(QScriptEngine *engine, cMethodStructure* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void cMethodStructure::MethodStructureFromScriptValue(const QScriptValue &obj, cMethodStructure* &s)
{
	s = qobject_cast<cMethodStructure*>(obj.toQObject());
}

bool cMethodStructure::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
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

bool cMethodStructure::insertMethodParameter(cMethodParameterStructure *pMethodParamStrcValue) 
{
	if(pMethodParamStrcValue)
	{
		mapParamIDtoParamStruct.insert(pMethodParamStrcValue->getMethodParameterID(),pMethodParamStrcValue);
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cExperimentStructure_SharedData::cExperimentStructure_SharedData()
{
	currentScriptEngine = NULL;
	currentBlockPointer = NULL;
	currentLoopPointer = NULL;
	firstBlockPointer = NULL;
	nExperimentID = 0;
	sExperimentName = "Experiment " + QString::number(nExperimentID);
	bDebugMode = false;
	CurrentExperiment_RunState = ExperimentStructuresNameSpace::ES_IDLE;
	currentExperimentState.Experiment_ExternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	currentExperimentState.CurrentBlock_BlockID = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	currentExperimentState.CurrentBlock_ExternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	currentExperimentState.CurrentBlock_InternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	currentExperimentState.CurrentBlock_LoopID = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	currentExperimentState.CurrentBlock_TrialNumber = ExperimentStructuresNameSpace::RA_REINITIALIZE;
}

cExperimentStructure_SharedData::cExperimentStructure_SharedData(const cExperimentStructure_SharedData &other) : QSharedData(other)
{
	CurrentExperiment_RunState = other.CurrentExperiment_RunState;
	currentExperimentState = other.currentExperimentState;
	currentScriptEngine = other.currentScriptEngine;
	nExperimentID = other.nExperimentID;
	sExperimentName = other.sExperimentName;
	bDebugMode = other.bDebugMode;
	currentBlockPointer = NULL;
	firstBlockPointer = NULL;
	currentLoopPointer = other.currentLoopPointer;
	if(other.lBlocks.isEmpty() == false)
	{
		for (int i=0;i<other.lBlocks.count();i++)
		{
			lBlocks.append(other.lBlocks[i]);
			if(other.lBlocks[i] == other.currentBlockPointer)
			{
				currentBlockPointer = lBlocks[i];
			}
			if(other.lBlocks[i] == other.firstBlockPointer)
				firstBlockPointer = lBlocks[i];

		}
	}
	if(other.lObjects.isEmpty() == false)
	{
		for (int i=0;i<other.lObjects.count();i++)
		{
			lObjects.append(other.lObjects[i]);
		}
	}
	if(other.mObjectIdToMethodConnections.isEmpty() == false)
	{
		for (int i=0;i<other.mObjectIdToMethodConnections.keys().count();i++)
		{
			if(mObjectIdToMethodConnections.contains(other.mObjectIdToMethodConnections.keys().at(i))==false)
			{
				QList<cMethodConnectionStructure*> lMethodConnStrc;
				mObjectIdToMethodConnections.insert(other.mObjectIdToMethodConnections.keys().at(i),lMethodConnStrc);
			}
			for (int j=0;j<other.mObjectIdToMethodConnections.value(other.mObjectIdToMethodConnections.keys().at(i)).count();j++)
			{
				((QList<cMethodConnectionStructure*>)mObjectIdToMethodConnections.value(other.mObjectIdToMethodConnections.keys().at(i))).append(other.mObjectIdToMethodConnections.value(other.mObjectIdToMethodConnections.keys().at(i)).at(j));
			}
		}
	}
	if(other.mObjectIdToInitializations.isEmpty() == false)
	{
		for (int i=0;i<other.mObjectIdToInitializations.keys().count();i++)
		{
			if(mObjectIdToInitializations.contains(other.mObjectIdToInitializations.keys().at(i))==false)
			{
				QList<cMethodStructure*> lMethodConnStrc;
				mObjectIdToInitializations.insert(other.mObjectIdToInitializations.keys().at(i),lMethodConnStrc);
			}
			for (int j=0;j<other.mObjectIdToInitializations.value(other.mObjectIdToInitializations.keys().at(i)).count();j++)
			{
				((QList<cMethodStructure*>)mObjectIdToInitializations.value(other.mObjectIdToInitializations.keys().at(i))).append(other.mObjectIdToInitializations.value(other.mObjectIdToInitializations.keys().at(i)).at(j));
			}
		}
	}
	if(other.mObjectIdToFinalizations.isEmpty() == false)
	{
		for (int i=0;i<other.mObjectIdToFinalizations.keys().count();i++)
		{
			if(mObjectIdToFinalizations.contains(other.mObjectIdToFinalizations.keys().at(i))==false)
			{
				QList<cMethodStructure*> lMethodConnStrc;
				mObjectIdToFinalizations.insert(other.mObjectIdToFinalizations.keys().at(i),lMethodConnStrc);
			}
			for (int j=0;j<other.mObjectIdToFinalizations.value(other.mObjectIdToFinalizations.keys().at(i)).count();j++)
			{
				((QList<cMethodStructure*>)mObjectIdToFinalizations.value(other.mObjectIdToFinalizations.keys().at(i))).append(other.mObjectIdToFinalizations.value(other.mObjectIdToFinalizations.keys().at(i)).at(j));
			}
		}
	}
}

cExperimentStructure_SharedData::~cExperimentStructure_SharedData()
{	
	if(lBlocks.isEmpty() == false)
	{
		for (int i=0;i<lBlocks.count();i++)
		{
			delete lBlocks[i];
			lBlocks[i] = NULL;
		}
		lBlocks.clear();
	}
	if(lObjects.isEmpty() == false)
	{
		for (int i=0;i<lObjects.count();i++)
		{
			delete lObjects[i];
			lObjects[i] = NULL;
		}
		lObjects.clear();
	}
	if(mObjectIdToMethodConnections.isEmpty() == false)
	{
		for (int i=0;i<mObjectIdToMethodConnections.count();i++)
		{
			for (int j=0;j<mObjectIdToMethodConnections[i].count();j++)
			{
				delete mObjectIdToMethodConnections[i].at(j);
			}
			mObjectIdToMethodConnections[i].clear();
		}
		mObjectIdToMethodConnections.clear();
	}
	if(mObjectIdToInitializations.isEmpty() == false)
	{
		for (int i=0;i<mObjectIdToInitializations.count();i++)
		{
			for (int j=0;j<mObjectIdToInitializations[i].count();j++)
			{
				delete mObjectIdToInitializations[i].at(j);
			}
			mObjectIdToInitializations[i].clear();
		}
		mObjectIdToInitializations.clear();
	}
	if(mObjectIdToFinalizations.isEmpty() == false)
	{
		for (int i=0;i<mObjectIdToFinalizations.count();i++)
		{
			for (int j=0;j<mObjectIdToFinalizations[i].count();j++)
			{
				delete mObjectIdToFinalizations[i].at(j);
			}
			mObjectIdToFinalizations[i].clear();
		}
		mObjectIdToFinalizations.clear();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief The cExperimentStructure constructor.
*
*   No parameter
*/
cExperimentStructure::cExperimentStructure()
{
	pSharedData = new cExperimentStructure_SharedData();
}

cExperimentStructure::cExperimentStructure(const cExperimentStructure& other) : pSharedData(other.pSharedData)
{
	//TODO fill in copy constructor, should be declared for the Q_DECLARE_METATYPE macro
}

/*! \brief The cExperimentStructure destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
cExperimentStructure::~cExperimentStructure()
{
}

bool cExperimentStructure::Initialize()
{
	pSharedData->nExperimentID = 0;
	pSharedData->sExperimentName = "Experiment " + QString::number(pSharedData->nExperimentID);
	pSharedData->bDebugMode = false;
	pSharedData->CurrentExperiment_RunState = ExperimentStructuresNameSpace::ES_IDLE;
	resetExperiment();
	return true;
}

QScriptValue cExperimentStructure::ctor__cExperimentStructure(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new cExperimentStructure(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

QScriptValue cExperimentStructure::experimentStructureToScriptValue(QScriptEngine *engine, cExperimentStructure* const &s)
{
	QScriptValue obj = engine->newQObject(s);
	return obj;
}

void cExperimentStructure::experimentStructureFromScriptValue(const QScriptValue &obj, cExperimentStructure* &s)
{
	s = qobject_cast<cExperimentStructure*>(obj.toQObject());
}

void cExperimentStructure::resetExperimentStateStruct(ExperimentStructuresNameSpace::strcExperimentStructureState *strcExpState)
{
	if(strcExpState == NULL)
		return;
	strcExpState->Experiment_ExternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	strcExpState->CurrentBlock_BlockID = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	strcExpState->CurrentBlock_ExternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	strcExpState->CurrentBlock_InternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	strcExpState->CurrentBlock_LoopID = ExperimentStructuresNameSpace::RA_REINITIALIZE;
	strcExpState->CurrentBlock_TrialNumber = ExperimentStructuresNameSpace::RA_REINITIALIZE;
};

QScriptValue cExperimentStructure::createExperimentStructureStateFromScript(QScriptContext *, QScriptEngine *engine)
{
	ExperimentStructuresNameSpace::strcExperimentStructureState s;
	cExperimentStructure::resetExperimentStateStruct(&s);
	return engine->toScriptValue(s);
}

QScriptValue cExperimentStructure::experimentStructureStateToScriptValue(QScriptEngine *engine, const ExperimentStructuresNameSpace::strcExperimentStructureState &s)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("Experiment_ExternalTrigger", s.Experiment_ExternalTrigger);
	obj.setProperty("CurrentBlock_BlockID", s.CurrentBlock_BlockID);
	obj.setProperty("CurrentBlock_ExternalTrigger", s.CurrentBlock_ExternalTrigger);
	obj.setProperty("CurrentBlock_InternalTrigger", s.CurrentBlock_InternalTrigger);
	obj.setProperty("CurrentBlock_LoopID", s.CurrentBlock_LoopID);
	obj.setProperty("CurrentBlock_TrialNumber", s.CurrentBlock_TrialNumber);
	return obj;
}

void cExperimentStructure::experimentStructureStateFromScriptValue(const QScriptValue &obj, ExperimentStructuresNameSpace::strcExperimentStructureState &s)
{
	s.Experiment_ExternalTrigger = obj.property("Experiment_ExternalTrigger").toInt32();
	s.CurrentBlock_BlockID = obj.property("CurrentBlock_BlockID").toInt32();
	s.CurrentBlock_ExternalTrigger = obj.property("CurrentBlock_ExternalTrigger").toInt32();
	s.CurrentBlock_InternalTrigger = obj.property("CurrentBlock_InternalTrigger").toInt32();
	s.CurrentBlock_LoopID = obj.property("CurrentBlock_LoopID").toInt32();
	s.CurrentBlock_TrialNumber = obj.property("CurrentBlock_TrialNumber").toInt32();
}

bool cExperimentStructure::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		pSharedData->currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		QScriptValue objectValue = pSharedData->currentScriptEngine->newQObject(this);
		pSharedData->currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

void cExperimentStructure::ExperimentStop()
{
	if(pSharedData->CurrentExperiment_RunState == ExperimentStructuresNameSpace::ES_RUNNING)
	{
		pSharedData->CurrentExperiment_RunState = ExperimentStructuresNameSpace::ES_IDLE ;
		emit experimentStopped();
	}
}

void cExperimentStructure::ExperimentAbort()
{
	ExperimentStop();
}

void cExperimentStructure::ExperimentStart()
{
	if(pSharedData->CurrentExperiment_RunState == ExperimentStructuresNameSpace::ES_WAITING_FOR_TRIGGER)
	{
		pSharedData->CurrentExperiment_RunState = ExperimentStructuresNameSpace::ES_RUNNING;
		emit experimentStarted();
	}
}

bool cExperimentStructure::isUnusedBlockID(const int &nBlockID) const
{
	if(pSharedData->lBlocks.isEmpty())
		return true;
	for (int i = 0; i < pSharedData->lBlocks.size(); i++) 
	{
		if (pSharedData->lBlocks.at(i)->getBlockID() == nBlockID)
			return false;
	}
	return true;
}

bool cExperimentStructure::isUnusedObjectID(const int &nObjectID) const
{
	if(pSharedData->lObjects.isEmpty())
		return true;
	for (int i = 0; i < pSharedData->lObjects.size(); i++) 
	{
		if (pSharedData->lObjects.at(i)->getObjectID() == nObjectID)
			return false;
	}
	return true;
}

QList<cObjectStructure*> &cExperimentStructure::getObjectList()
{
	return pSharedData->lObjects;
}

cBlockStructure* cExperimentStructure::getNextClosestBlockNumberByFromNumber(const int &startBlockNumber)
{
	//result is an cBlockStructure* with ID=startID or first closest higher value
	if(pSharedData->lBlocks.isEmpty())//Are there any blocks defined?
		return NULL;
	//First try the expected location
	if(pSharedData->lBlocks.size()>startBlockNumber)
	{
		if (pSharedData->lBlocks.at(startBlockNumber)->getBlockNumber() == startBlockNumber)
		{
			return pSharedData->lBlocks[startBlockNumber];
		}
	}
	//Now try the other items
	int closestIndex = -1;
	int closestBlockNumber = startBlockNumber;//This shouldn't matter
	int tmpBlockNumber;
	bool bAcceptAny = true;//Makes sure to accept any valid value from the start of the search
	for (int i=0;i<pSharedData->lBlocks.size();i++) 
	{
		tmpBlockNumber = pSharedData->lBlocks.at(i)->getBlockNumber();
		if(tmpBlockNumber == startBlockNumber)
		{//We found it although it was not at its expected location
			return pSharedData->lBlocks[i];
		}
		else if(tmpBlockNumber > startBlockNumber)
		{//We found a larger block number...
			if(bAcceptAny || (tmpBlockNumber < closestBlockNumber))//First one or a more closer(thus smaller) number?
			{
				closestBlockNumber = tmpBlockNumber;
				closestIndex = i;
				bAcceptAny = false;
			}
		}
	}
	if(closestIndex>=0)//Do we have an closest result?
	{
		return pSharedData->lBlocks[closestIndex];
	}
	return NULL;
}

int cExperimentStructure::getCurrentBlockIndex() const
{
	if (pSharedData->currentBlockPointer == NULL)
		return ExperimentStructuresNameSpace::OI_UNDEFINED;
	if (pSharedData->lBlocks.isEmpty())
		return ExperimentStructuresNameSpace::OI_UNDEFINED;
	for (int i=0;i<pSharedData->lBlocks.count();i++)
	{
		if(pSharedData->lBlocks[i] == pSharedData->currentBlockPointer)
		{
			return i;
		}
	}
	return ExperimentStructuresNameSpace::OI_UNDEFINED;
}

cBlockStructure cExperimentStructure::getCurrentBlock(bool &bHasCurrBlock) const
{
	if(pSharedData->lBlocks.isEmpty() == false)
	{
		int nFoundIndex = getCurrentBlockIndex();
		if(nFoundIndex >= 0)
		{
			bHasCurrBlock = true;
			return *pSharedData->lBlocks[nFoundIndex];
		}
	}
	bHasCurrBlock = false;
	return cBlockStructure();
}

bool cExperimentStructure::prepareStartBlock()
{
	if(isValidBlockPointer(pSharedData->firstBlockPointer) == false)
	{
		if(pSharedData->lBlocks.isEmpty() == false)
		{
			pSharedData->firstBlockPointer = getNextClosestBlockNumberByFromNumber(0);//Here we just take the block with the lowest block number
			if(pSharedData->firstBlockPointer == NULL)
			{
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool cExperimentStructure::isValidBlockPointer(cBlockStructure *cBlock) const
{
	if(cBlock == NULL)
		return false;
	if(pSharedData->lBlocks.isEmpty())//Are there any blocks defined?
		return false;
	for (int i=0;i<pSharedData->lBlocks.size();i++) 
	{
		if(pSharedData->lBlocks[i] == cBlock)
			return true;
	}
	return false;
}

bool cExperimentStructure::insertBlock(cBlockStructure *cBlock)
{
	if(isUnusedBlockID(cBlock->getBlockID()))
	{
		pSharedData->lBlocks.append(cBlock);
		if(pSharedData->lBlocks.count()==1)//First block to append?
			pSharedData->firstBlockPointer = pSharedData->lBlocks[0];
		return true;
	}
	return false;
}

int cExperimentStructure::getBlockIndexByID(const int &nBlockID) const
{
	if(pSharedData->lBlocks.isEmpty())//Are there any blocks defined?
		return -1;
	for (int i=0;i<pSharedData->lBlocks.size();i++) 
	{
		if(pSharedData->lBlocks.at(i)->getBlockID() == nBlockID)
			return i;
	}
	return -1;
}

cBlockStructure* cExperimentStructure::getBlockPointerByID(const int &nBlockID)
{
	int nIndex = getBlockIndexByID(nBlockID);
	if(nIndex >= 0)
		return pSharedData->lBlocks[nIndex];
	return NULL;
}

int cExperimentStructure::getBlockCount() const 
{
	return pSharedData->lBlocks.count();
}

bool cExperimentStructure::isValidObjectPointer(cObjectStructure *cObject) const
{
	if(cObject == NULL)
		return false;
	if(pSharedData->lObjects.isEmpty())//Are there any Objects defined?
		return false;
	for (int i=0;i<pSharedData->lObjects.size();i++) 
	{
		if(pSharedData->lObjects[i] == cObject)
			return true;
	}
	return false;
}

bool cExperimentStructure::insertObject(cObjectStructure *cObject)
{
	if(isUnusedObjectID(cObject->getObjectID()))
	{
		pSharedData->lObjects.append(cObject);
		return true;
	}
	return false;
}

bool cExperimentStructure::insertObjectMethodConnection(cMethodConnectionStructure *pMethodConnection)
{
	if((isUnusedObjectID(pMethodConnection->getSourceObjectID()) == false) && (isUnusedObjectID(pMethodConnection->getTargetObjectID()) == false))
	{
		QList<cMethodConnectionStructure*> *pMethodConnectionStructureList = getObjectMethodConnectionList(pMethodConnection->getSourceObjectID());
		if(pMethodConnectionStructureList == NULL)
		{
			pMethodConnectionStructureList = new QList<cMethodConnectionStructure*>();
			pMethodConnectionStructureList->append(pMethodConnection);
			pSharedData->mObjectIdToMethodConnections.insert(pMethodConnection->getSourceObjectID(), *pMethodConnectionStructureList);
		}
		else
		{
			pMethodConnectionStructureList->append(pMethodConnection);
		}
		return true;
	}
	return false;
}

QList<cMethodConnectionStructure*> *cExperimentStructure::getObjectMethodConnectionList(const int &nObjectID)
{
	for (QMap<int, QList<cMethodConnectionStructure*>>::iterator itMethodList=pSharedData->mObjectIdToMethodConnections.begin();itMethodList!=pSharedData->mObjectIdToMethodConnections.end();++itMethodList)
	{
		if(itMethodList.key() == nObjectID)
			return &itMethodList.value();
	}
	return NULL;
}

bool cExperimentStructure::insertObjectInitialization(cMethodStructure *pObjectInitialization)
{
	if(isUnusedObjectID(pObjectInitialization->getMethodObjectID()) == false)
	{
		QList<cMethodStructure*> *pMethodStructureList = getObjectInitializationList(pObjectInitialization->getMethodObjectID());
		if(pMethodStructureList == NULL)
		{
			pMethodStructureList = new QList<cMethodStructure*>();
			pMethodStructureList->insert(0,pObjectInitialization);
			pSharedData->mObjectIdToInitializations.insert(pObjectInitialization->getMethodObjectID(), *pMethodStructureList);
		}
		else
		{
			int nNewIndex = 0;
			foreach(cMethodStructure *tmpMethodStruct, *pMethodStructureList)
			{
				if (tmpMethodStruct->getMethodOrderNumber() > pObjectInitialization->getMethodOrderNumber())
					break;
				nNewIndex++;
			}
			pMethodStructureList->insert(nNewIndex,pObjectInitialization);
		}
		return true;
	}
	return false;
}

QList<cMethodStructure*> *cExperimentStructure::getObjectInitializationList(const int &nObjectID)
{
	for (QMap<int, QList<cMethodStructure*>>::iterator itInitList=pSharedData->mObjectIdToInitializations.begin();itInitList!=pSharedData->mObjectIdToInitializations.end();++itInitList)
	{
		if (itInitList.key() == nObjectID)
			return &itInitList.value();
	}
	return NULL;
}

bool cExperimentStructure::insertObjectFinalization(cMethodStructure *pObjectFinalization)
{
	if(isUnusedObjectID(pObjectFinalization->getMethodObjectID()) == false)
	{
		QList<cMethodStructure*> *pMethodStructureList = getObjectFinalizationList(pObjectFinalization->getMethodObjectID());
		if(pMethodStructureList == NULL)
		{
			pMethodStructureList = new QList<cMethodStructure*>();
			pMethodStructureList->insert(0, pObjectFinalization);
			pSharedData->mObjectIdToFinalizations.insert(pObjectFinalization->getMethodObjectID(), *pMethodStructureList);
		}
		else
		{
			int nNewIndex = 0;
			foreach(cMethodStructure *tmpMethodStruct, *pMethodStructureList)
			{
				if (tmpMethodStruct->getMethodOrderNumber() > pObjectFinalization->getMethodOrderNumber())
					break;
				nNewIndex++;
			}
			pMethodStructureList->insert(nNewIndex, pObjectFinalization);
		}
		return true;
	}
	return false;
}

QList<cMethodStructure*> *cExperimentStructure::getObjectFinalizationList(const int &nObjectID)
{
	for (QMap<int, QList<cMethodStructure*>>::iterator itFinitList=pSharedData->mObjectIdToFinalizations.begin();itFinitList!=pSharedData->mObjectIdToFinalizations.end();++itFinitList)
	{
		if(itFinitList.key() == nObjectID)
			return &itFinitList.value();
	}
	return NULL;
}

int cExperimentStructure::getObjectIndexByID(const int &nObjectID) const
{
	if(pSharedData->lObjects.isEmpty())//Are there any Objects defined?
		return -1;
	for (int i=0;i<pSharedData->lObjects.size();i++) 
	{
		if(pSharedData->lObjects.at(i)->getObjectID() == nObjectID)
			return i;
	}
	return -1;
}

cObjectStructure* cExperimentStructure::getObjectPointerByID(const int &nObjectID)
{
	int nIndex = getObjectIndexByID(nObjectID);
	if(nIndex >= 0)
		return pSharedData->lObjects[nIndex];
	return NULL;
}

int cExperimentStructure::getObjectCount() const 
{
	return pSharedData->lObjects.count();
}

bool cExperimentStructure::prepareExperiment(const bool &bIgnoreNoDefinedBlocks)
{
	if (pSharedData->lBlocks.isEmpty())
		return bIgnoreNoDefinedBlocks;
	if(prepareStartBlock())
	{
		pSharedData->currentBlockPointer = pSharedData->firstBlockPointer;
		pSharedData->currentLoopPointer = pSharedData->currentBlockPointer->resetToFirstFreeLoopPointer();
		pSharedData->currentExperimentState.Experiment_ExternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
		pSharedData->currentExperimentState.CurrentBlock_BlockID = pSharedData->currentBlockPointer->getBlockID();
		pSharedData->currentExperimentState.CurrentBlock_ExternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
		pSharedData->currentExperimentState.CurrentBlock_InternalTrigger = ExperimentStructuresNameSpace::RA_REINITIALIZE;
		if(pSharedData->currentLoopPointer == NULL)
			pSharedData->currentExperimentState.CurrentBlock_LoopID = ExperimentStructuresNameSpace::RA_UNDEFINED;
		else
			pSharedData->currentExperimentState.CurrentBlock_LoopID = pSharedData->currentLoopPointer->getLoopID();
		pSharedData->currentExperimentState.CurrentBlock_TrialNumber = ExperimentStructuresNameSpace::RA_REINITIALIZE;
		pSharedData->CurrentExperiment_RunState = ExperimentStructuresNameSpace::ES_WAITING_FOR_TRIGGER;
		return true;
	}
	return false;
}

int cExperimentStructure::getExternalTriggerCount() const
{
	return pSharedData->currentExperimentState.Experiment_ExternalTrigger;
}

void cExperimentStructure::incrementExternalTrigger()
{
	if(pSharedData->CurrentExperiment_RunState == ExperimentStructuresNameSpace::ES_IDLE)
		return;
	if(pSharedData->CurrentExperiment_RunState == ExperimentStructuresNameSpace::ES_WAITING_FOR_TRIGGER)//currentExperimentState.Experiment_ExternalTrigger == RA_REINITIALIZE)//First external experiment trigger!
	{
		if(pSharedData->currentExperimentState.CurrentBlock_BlockID == ExperimentStructuresNameSpace::RA_REINITIALIZE)
		{
			if(prepareExperiment() == false)
				return;
			pSharedData->currentExperimentState.CurrentBlock_BlockID = pSharedData->currentBlockPointer->getBlockID();
		}
		if(pSharedData->currentExperimentState.CurrentBlock_LoopID == ExperimentStructuresNameSpace::RA_REINITIALIZE)
		{
			pSharedData->currentLoopPointer = pSharedData->currentBlockPointer->resetToFirstFreeLoopPointer();
			if(pSharedData->currentLoopPointer == NULL)
				pSharedData->currentExperimentState.CurrentBlock_LoopID = ExperimentStructuresNameSpace::RA_UNDEFINED;
			else
				pSharedData->currentExperimentState.CurrentBlock_LoopID = pSharedData->currentLoopPointer->getLoopID();
		}
		pSharedData->currentExperimentState.Experiment_ExternalTrigger = 0;
		pSharedData->currentExperimentState.CurrentBlock_ExternalTrigger = 0;
		pSharedData->currentExperimentState.CurrentBlock_InternalTrigger = 0;
		pSharedData->currentExperimentState.CurrentBlock_TrialNumber = 0;
		ExperimentStart();
	}
	else if(pSharedData->CurrentExperiment_RunState == ExperimentStructuresNameSpace::ES_RUNNING)
	{
		int BlockExtTriggerAmount = pSharedData->currentBlockPointer->getNumberOfExternalTriggers();
		int BlockIntTriggerAmount = pSharedData->currentBlockPointer->getNumberOfInternalTriggers();
		int BlockTrialAmount = pSharedData->currentBlockPointer->getNumberOfTrials();
		cLoopStructure *nextLoopBlock = NULL;
		if((pSharedData->currentExperimentState.CurrentBlock_ExternalTrigger + 1) >= BlockExtTriggerAmount)
		{
			pSharedData->currentExperimentState.CurrentBlock_ExternalTrigger = 0;//Reset to zero again!
			if((pSharedData->currentExperimentState.CurrentBlock_InternalTrigger + 1) >= BlockIntTriggerAmount)
			{
				pSharedData->currentExperimentState.CurrentBlock_InternalTrigger = 0;//Reset to zero again!
				if((pSharedData->currentExperimentState.CurrentBlock_TrialNumber + 1) >= BlockTrialAmount)
				{//Next Block?
					pSharedData->currentExperimentState.CurrentBlock_TrialNumber = 0;//Reset to zero again!
					nextLoopBlock = pSharedData->currentBlockPointer->incrementToNextLoopPointer(pSharedData->currentLoopPointer);//Do we have a Block defined by a Loop?
					if(nextLoopBlock)//Loop Block available?
					{
						pSharedData->currentBlockPointer->resetAllInnerLoopCounters(nextLoopBlock->getLoopNumber());
						int nTargetBlockID = nextLoopBlock->getTargetBlockID();
						pSharedData->currentBlockPointer = getBlockPointerByID(nTargetBlockID);
						if(pSharedData->currentBlockPointer == NULL)
						{
							ExperimentAbort();
							return;					
						}
						pSharedData->currentLoopPointer = pSharedData->currentBlockPointer->resetToFirstFreeLoopPointer();

						//if((nextLoopBlock->getLoopCounter() == 0) && ())
						//{
						//	nextLoopBlock.set
						//}

					}//No Loop Block available
					else
					{
						pSharedData->currentBlockPointer->resetAllLoopCounters();
						//int nFoundIndex = -1;
						pSharedData->currentBlockPointer = getNextClosestBlockNumberByFromNumber(pSharedData->currentBlockPointer->getBlockNumber()+1);
						if(pSharedData->currentBlockPointer == NULL)
						{
							ExperimentStop();
							return;					
						}
						pSharedData->currentLoopPointer = pSharedData->currentBlockPointer->incrementToNextLoopPointer(NULL);
					}
					pSharedData->currentExperimentState.CurrentBlock_BlockID = pSharedData->currentBlockPointer->getBlockID();
					if(pSharedData->currentLoopPointer)
						pSharedData->currentExperimentState.CurrentBlock_LoopID = pSharedData->currentLoopPointer->getLoopID();
					else
						pSharedData->currentExperimentState.CurrentBlock_LoopID = ExperimentStructuresNameSpace::RA_UNDEFINED;
				}
				else
				{
					pSharedData->currentExperimentState.CurrentBlock_TrialNumber++;//Increment with 1
				}
			}
			else
			{
				pSharedData->currentExperimentState.CurrentBlock_InternalTrigger++;//Increment with 1
			}
		}
		else
		{
			pSharedData->currentExperimentState.CurrentBlock_ExternalTrigger++;//Increment with 1
		}
		pSharedData->currentExperimentState.Experiment_ExternalTrigger++;
	}
	emit externalTriggerRecieved();
}