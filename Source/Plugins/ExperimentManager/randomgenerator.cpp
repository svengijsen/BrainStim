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


#include "randomgenerator.h"
#include <QDateTime>
#include "randomc/mersenne.cpp"
#include "randomc/mother.cpp"

Q_DECLARE_METATYPE(QStringList)

/*! \brief The RandomGenerator constructor.
*
*   You do not need to specify the parent object. 
*	The BrainStim script engine automatically retrieves the parent role
*/
RandomGenerator::RandomGenerator(QObject *parent):QObject(parent)
{
	currentScriptEngine = NULL;
	tCombinedRandGen = NULL;
	nSeed = QDateTime::currentDateTime().toString(RANDOMIZE_DATETIME_FORMAT).toInt();
	tCombinedRandGen = new TRandomCombined<CRandomMersenne,CRandomMother>(nSeed);
}

QScriptValue RandomGenerator::ctor__randomGenerator(QScriptContext* context, QScriptEngine* engine)
{
	//QString sArgs;
	//int nArgCount = context->argumentCount();
	//bool bIsArray;
	//for(int i=0;i<nArgCount;i++)
	//{
	//	bIsArray = context->argument(i).isArray();
	//	sArgs = sArgs + context->argument(i).toString();
	//	if((i+1)<nArgCount)
	//		sArgs = sArgs + " , ";
	//	
	//}
	RandomGenerator *RandomGeneratorObj = new RandomGenerator();
	if(context->argumentCount()>0)
	{
		if(context->argument(0).isArray())
		{
			RandomGeneratorObj->appendStringList(qscriptvalue_cast<QStringList> (context->argument(0)));
		}
	}
	RandomGeneratorObj->setScriptEngine(engine);
	return engine->newQObject(RandomGeneratorObj, QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

void RandomGenerator::setScriptEngine(QScriptEngine* engine)
{
	currentScriptEngine = engine;
	qScriptRegisterSequenceMetaType<QStringList>(currentScriptEngine);
}

bool RandomGenerator::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
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

/*! \brief The RandomGenerator destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
RandomGenerator::~RandomGenerator()
{
	if(tCombinedRandGen != NULL)
	{
		delete tCombinedRandGen;
		tCombinedRandGen = NULL;
	}
}

int RandomGenerator::randomizeInt(int nMin, int nMax) 
{
	return tCombinedRandGen->IRandom(nMin,nMax);
}

double RandomGenerator::randomizeDouble(double dMin, double dMax) 
{
	return tCombinedRandGen->DRandom(dMin,dMax);
}

const QString RandomGenerator::at(int i) const 
{
	return QStringList::at(i);
}

int RandomGenerator::count() const 
{
	return QStringList::count();
}

int RandomGenerator::count(const QString &sValue) const 
{
	return QStringList::count(sValue);
}

bool RandomGenerator::isEmpty() const 
{
	return QStringList::isEmpty();
}

void RandomGenerator::append(const QString &sValue) 
{
	QStringList::append(sValue);
}

QString RandomGenerator::takeFirst() 
{
	return QStringList::takeFirst();
}

QString RandomGenerator::takeLast() 
{
	return QStringList::takeLast();
}

QString RandomGenerator::takeAt(int i) 
{
	return QStringList::takeAt(i);
}

void RandomGenerator::clear() 
{
	QStringList::clear();
}

int RandomGenerator::removeAll(const QString &sValue) 
{
	return QStringList::removeAll(sValue);
}

void RandomGenerator::removeAt(int i) 
{
	return QStringList::removeAt(i);
}

int RandomGenerator::indexOf(const QString &sValue, int nFrom) const 
{
	return QStringList::indexOf(sValue,nFrom);
}

void RandomGenerator::swap(int i, int j) 
{
	QStringList::swap(i,j);
}

void RandomGenerator::insert(int i, const QString &sValue) 
{
	QStringList::insert(i,sValue);
}

QStringList RandomGenerator::randomize(int rMethod, QStringList sList)
{
	RandomGenerator_RandomizeMethod rndMethod = (RandomGenerator_RandomizeMethod) rMethod;
	if(sList.isEmpty())
	{
		if(randomizeList(rndMethod))
		{
			return (QStringList)*this;
		}
	}
	else
	{
		if(randomizeList(rndMethod,&sList))
		{
			return (QStringList)*this;
		}
	}
	return QStringList();
}

QStringList RandomGenerator::toStringList()
{
	return (QStringList)*this;
}

int RandomGenerator::appendStringList(const QStringList &sList)
{
	QStringList::append(sList);
	return QStringList::count();
}

bool RandomGenerator::randomizeList(RandomGenerator_RandomizeMethod rMethod, QStringList *sList)
{
	//int a = randomizeInt(0,700);
	//InitRandomizer();
	bool bRetVal = false;
	int nListCount = QList::count();
	int i,j;
	int nIndexFound;
	int nRandom;

	switch (rMethod)
	{
	case RandomGenerator_RandomizeStandard:
		////just randomize
		for(i=(nListCount-1);i>0;--i)
		{
			nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
			QList::swap(i,nRandom);
		}
		bRetVal = true;
		break;
	case RandomGenerator_RandomizePreservedValues:
		//sList[] contains the values of the indexes that should be preserved(should not be randomized)!
		for(i=(nListCount-1);i>0;--i)
		{//first start with an randomize
			nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
			QList::swap(i,nRandom);
		}
		if (sList == NULL)
		{
			bRetVal = true;
			break;
		}
		if (sList->count() < 1)
		{
			bRetVal = true;
			break;
		}
		for(i=0;i<sList->count();i++)
		{
			nIndexFound = QList::indexOf(sList->at(i));
			if (nIndexFound == -1)
			{
				QList::replace(i,sList->at(i));
			} 
			else if(i!=nIndexFound)
			{
				QList::swap(i,nIndexFound);
			}
		}
		bRetVal = true;
		break;
	case RandomGenerator_RandomizePreservedIndexes:
		//sList[] contains the indexes that should be preserved(should not be randomized)!
		if (sList == NULL)
		{//just randomize
			for(i=(nListCount-1);i>0;--i)
			{
				nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
				QList::swap(i,nRandom);
			}
			bRetVal = true;
			break;
		}
		else if (sList->count() < 1)
		{//just randomize
			for(i=(nListCount-1);i>0;--i)
			{
				nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
				QList::swap(i,nRandom);
			}
			bRetVal = true;
			break;
		}
		else if(sList->count() >= (QList::count()-1))
		{//nothing to do
			bRetVal = true;
			break;
		}
		else
		{
			QStringList tmpCopyLst;
			tmpCopyLst = QStringList::mid(0);//copy the current list
			//first start with an randomize
			for(i=(nListCount-1);i>0;--i)
			{
				nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
				QList::swap(i,nRandom);
			}

			int nRecoverCount = sList->count();
			if (nRecoverCount > 0)
			{
				//recover the preserved items
				for(j=0;j<nRecoverCount;j++)
				{
					nIndexFound = QList::indexOf(tmpCopyLst.at(sList->at(j).toInt()));
					if(nIndexFound >= 0)//is the preserved item still in the current list?
						QList::swap(sList->at(j).toInt(),nIndexFound);				
				}
				if (nRecoverCount > 1)
				{
					//randomize the recovered values
					for(i=(nRecoverCount-1);i>0;--i)
					{
						QList::swap(sList->at(i).toInt(),sList->at(tCombinedRandGen->IRandom(0,nRecoverCount-1)).toInt());
					}
				}
			}
			bRetVal = true;
			break;
		}
	case RandomGenerator_RandomizePreservedNonRandomizedIndexes:
		//sList[] contains the indexes that should be preserved(should not be randomized)!
		if (sList == NULL)
		{//just randomize
			for(i=(nListCount-1);i>0;--i)
			{
				nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
				QList::swap(i,nRandom);
			}
			bRetVal = true;
			break;
		}
		else if (sList->count() < 1)
		{//just randomize
			for(i=(nListCount-1);i>0;--i)
			{
				nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
				QList::swap(i,nRandom);
			}
			bRetVal = true;
			break;
		}
		else if(sList->count() >= (QList::count()-1))
		{//nothing to do
			bRetVal = true;
			break;
		}
		else
		{
			QStringList tmpCopyLst;
			tmpCopyLst = QStringList::mid(0);//copy the current list
			//first start with an randomize
			for(i=(nListCount-1);i>0;--i)
			{
				nRandom = tCombinedRandGen->IRandom(0,nListCount-1);
				QList::swap(i,nRandom);
			}
			int nRecoverCount = sList->count();
			if (nRecoverCount > 0)
			{
				//recover the preserved items
				for(j=0;j<nRecoverCount;j++)
				{
					nIndexFound = QList::indexOf(tmpCopyLst.at(sList->at(j).toInt()));
					if(nIndexFound >= 0)//is the preserved item still in the current list?
						QList::swap(sList->at(j).toInt(),nIndexFound);				
				}
			}
			bRetVal = true;
			break;
		}
	}
	return bRetVal;
}
