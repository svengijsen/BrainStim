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


#include "retinotopymapperwindow.h"
#include <QPainter>

RetinotopyMapperWindow::RetinotopyMapperWindow(RetinotopyMapper *parent) : parentRetinotopyMapper(parent)
{
	CDatOutputStream = NULL;
	outputFile = NULL;
	bCDATFileReadyToWrite = false;
	bFistRenderCall = true;
	nLastOutputTriggerFrameNumber = ExperimentStructuresNameSpace::RA_REINITIALIZE;
}

RetinotopyMapperWindow::~RetinotopyMapperWindow()
{
	if(CDatOutputStream)
	{
		if(bCDATFileReadyToWrite && outputFile)
		{
			CDatOutputStream->device()->seek(1*sizeof(quint32)); //This line will move the internal QBuffer to the position at the number of items
			*CDatOutputStream << OutputImageItemCount;
		}
		delete CDatOutputStream;
		CDatOutputStream = NULL;
	}	
	if(outputFile)
	{
		if(outputFile->isOpen())
			outputFile->close();
		delete outputFile;
		outputFile = NULL;
	}
}

QScreen *RetinotopyMapperWindow::grabScreenUnderMouseCursor()
{
	QPoint pCurrMouseCursorPos = this->mapFromGlobal(QCursor::pos());
	int nScreenNumber = QApplication::desktop()->screenNumber(pCurrMouseCursorPos);
	if(nScreenNumber>=0)
	{
		QList<QScreen*> lAvailableScreens = QGuiApplication::screens();
		if(lAvailableScreens.count() > nScreenNumber)
			return lAvailableScreens.at(nScreenNumber);
	}
	return QGuiApplication::primaryScreen();
}

void RetinotopyMapperWindow::initialize()
{

}

//void RetinotopyMapperWindow::render()
//{
//	++m_frame;
//}

void RetinotopyMapperWindow::postSwapBuffers()
{
	if (bFistRenderCall)
		bFistRenderCall = false;
	
	double dCurrentTime;
	double dFramePeriodTime;
	//bool bObjectIsLocked = isLocked(); //buffer it..

	//bool bExperimentStructureChanged = false;
	bool bHasCurrentBlock = false;
	cExperimentStructure tmpExpStr;
	cBlockStructure tmpExpBlockStr;
	ExperimentStructuresNameSpace::strcExperimentStructureState tmpExpStrState;
	if(parentRetinotopyMapper->experimentManager)
	{
		if(parentRetinotopyMapper->isDebugMode())
			parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","Screen Buffer Swapped, locked=",QString::number(parentRetinotopyMapper->isLocked()));

		cExperimentStructure tmpExpStr = cExperimentStructure(*parentRetinotopyMapper->experimentManager->getExperimentStructure());
		cBlockStructure tmpExpBlockStr = tmpExpStr.getCurrentBlock(bHasCurrentBlock);
		tmpExpStrState = tmpExpStr.getCurrentExperimentState();
	}
	if (parentRetinotopyMapper->nRefreshRate > 0)
	{
		dFramePeriodTime = 1000.0f/parentRetinotopyMapper->nRefreshRate; //DisplayRefreshRate
		dCurrentTime = parentRetinotopyMapper->experimentManager->restartExperimentTimer(parentRetinotopyMapper->getFrameTimerIndex());
	}
	if (parentRetinotopyMapper->isLocked()==false)
	{
		parentRetinotopyMapper->incrementCurrentBlockTrialFrame();// = CF_UNINITIALIZED;
		if(parentRetinotopyMapper->nRefreshRate > 0)
		{
			if(parentRetinotopyMapper->experimentManager)
			{
				if (dCurrentTime > (dFramePeriodTime*1.5f))
					parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),-1,__FUNCTION__,"","Paint routine took too long(" + QString::number(dCurrentTime) + " mSecs),(BlockID=" + QString::number(tmpExpStrState.CurrentBlock_BlockID) + ", TrialNumber=" + QString::number(tmpExpStrState.CurrentBlock_TrialNumber) +", Trigger(Int)=" + QString::number(tmpExpStrState.CurrentBlock_InternalTrigger) + ", Frame=" + QString::number(parentRetinotopyMapper->getCurrentBlockTrialFrame()) + ")",QString::number(dCurrentTime));
				else if(parentRetinotopyMapper->isDebugMode())
					parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),-1,__FUNCTION__,"","Paint routine took(" + QString::number(dCurrentTime) + " mSecs),(BlockID=" + QString::number(tmpExpStrState.CurrentBlock_BlockID) + ", TrialNumber=" + QString::number(tmpExpStrState.CurrentBlock_TrialNumber) +", Trigger(Int)=" + QString::number(tmpExpStrState.CurrentBlock_InternalTrigger) + ", Frame=" + QString::number(parentRetinotopyMapper->getCurrentBlockTrialFrame()) + ")",QString::number(dCurrentTime));
			}
		}
	}
}

void RetinotopyMapperWindow::render(QPainter *stimuliPainter)
{
	if(parentRetinotopyMapper->experimentManager == NULL)
		return;	
	if(parentRetinotopyMapper->isDebugMode())
		parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","Starting to paint the object");

	bool bHasABlock = false;
	cExperimentStructure tmpExpStr = cExperimentStructure(*parentRetinotopyMapper->experimentManager->getExperimentStructure());
	int elapsedTrialTime = (int)parentRetinotopyMapper->getElapsedTrialTime();
	int elapsedTrialTimeCopy = elapsedTrialTime;
	int nExpBlockTrialFrame = parentRetinotopyMapper->getCurrentBlockTrialFrame();
	ExperimentStructuresNameSpace::strcExperimentStructureState tmpExpStrState = tmpExpStr.getCurrentExperimentState();
	cBlockStructure tmpBlockStrc = tmpExpStr.getCurrentBlock(bHasABlock);
	if(bHasABlock == false)
	{
		qDebug() << __FUNCTION__ << "::No (more) Block defined to process, exiting...!";
		return;
	}

	bRenderStimuli = true;
	QString tmpStr = "";
	int i,j;
	parentRetinotopyMapper->tmpParamValue = "";
	parentRetinotopyMapper->dStimulusDiameter = 0.0;
	parentRetinotopyMapper->fTrialTimeProgress = 0.0f;
	if(parentRetinotopyMapper->isLocked())
	{
		parentRetinotopyMapper->bCreateActivationMap = false;
		//parentRetinotopyMapper->currentExpType = RETINOMAPPER_PATTERN_FIXATION;
	}
	else
	{
		parentRetinotopyMapper->bCreateActivationMap = (parentRetinotopyMapper->outputTriggerFrame) && (parentRetinotopyMapper->retinoOutputType==RETINOMAPPER_OUTPUTTYPE_MASK);
	}

	if((elapsedTrialTime >= ((parentRetinotopyMapper->triggerDurationMsec * (tmpExpStrState.CurrentBlock_InternalTrigger+1)))) &&(parentRetinotopyMapper->currentExpType != RETINOMAPPER_PATTERN_FIXATION))//If the TrialTime exceeds the maximum time for the current Block-Trial Time
		elapsedTrialTime = (parentRetinotopyMapper->triggerDurationMsec * (tmpExpStrState.CurrentBlock_InternalTrigger+1))-1;
	else if((elapsedTrialTime <= ((parentRetinotopyMapper->triggerDurationMsec * (tmpExpStrState.CurrentBlock_InternalTrigger)))) &&(parentRetinotopyMapper->currentExpType != RETINOMAPPER_PATTERN_FIXATION))//If the TrialTime is less than the minimum time for the current Block-Trial Time
		elapsedTrialTime = (parentRetinotopyMapper->triggerDurationMsec * (tmpExpStrState.CurrentBlock_InternalTrigger))+1;
	if (parentRetinotopyMapper->currExpBlockTrialCycle < (int)(tmpExpStrState.CurrentBlock_InternalTrigger/parentRetinotopyMapper->cycleTriggerAmount))
	{
		parentRetinotopyMapper->currExpBlockTrialCycle++;
		parentRetinotopyMapper->nextNewCycleEntered = true;
	}
	if (nExpBlockTrialFrame == 0)
	{
		parentRetinotopyMapper->nextNewCycleEntered = true;
		parentRetinotopyMapper->nextNewBlockEntered = true;
		parentRetinotopyMapper->randStimStateGenerator->clear();
	}
	if (parentRetinotopyMapper->currentExpType != RETINOMAPPER_PATTERN_FIXATION)
	{
		if (parentRetinotopyMapper->randEmptyStimGenerator->isEmpty() && (parentRetinotopyMapper->currExpBlockTrialCycle == 0) && (parentRetinotopyMapper->bAllTrialEmptyProcessed == false))//Initialize the "Empty" random list
		{
			if ((parentRetinotopyMapper->emptyTriggerSteps > 0) && (parentRetinotopyMapper->cycleTriggerAmount>=parentRetinotopyMapper->emptyTriggerSteps))//We have to make sure that the Empty item occur in a block next to each other
			{
				if(parentRetinotopyMapper->emptyTriggerStepsArray.isEmpty() == false)
				{
					for (j=0;j<(parentRetinotopyMapper->emptyTriggerStepsArray.count());j++)
					{
						parentRetinotopyMapper->randEmptyStimGenerator->append(parentRetinotopyMapper->emptyTriggerStepsArray.at(j));
					}
				}
				else
				{
					RandomGenerator tmpGenerator;
					for (j=0;j<(parentRetinotopyMapper->cycleTriggerAmount/parentRetinotopyMapper->emptyTriggerSteps);j++)
					{
						tmpGenerator.append(QString::number(j));
					}
					tmpGenerator.randomizeList();
					for (j=0;j<tmpGenerator.count();j++)//Create random Empty trigger steps within the Cycle
					{
						for (i=0;i<parentRetinotopyMapper->emptyTriggerSteps;i++)
						{
							parentRetinotopyMapper->randEmptyStimGenerator->append(QString::number((tmpGenerator.at(j).toInt()*parentRetinotopyMapper->emptyTriggerSteps)+i));
						}				
					}
					if (parentRetinotopyMapper->cycleTriggerAmount%parentRetinotopyMapper->emptyTriggerSteps>0)//Do we need some additional steps?
					{
						for (i=0;i<(parentRetinotopyMapper->cycleTriggerAmount%parentRetinotopyMapper->emptyTriggerSteps);i++)
						{
							parentRetinotopyMapper->randEmptyStimGenerator->append(QString::number((parentRetinotopyMapper->emptyTriggerSteps*((int)parentRetinotopyMapper->cycleTriggerAmount/parentRetinotopyMapper->emptyTriggerSteps))+i));
						}
					}
				}
			}
			else//Just fill the list
			{
				for (j=0;j<parentRetinotopyMapper->cycleTriggerAmount;j++)//Create random Empty trigger steps within the Cycle
				{
					parentRetinotopyMapper->randEmptyStimGenerator->append(QString::number(j));
				}
				parentRetinotopyMapper->randEmptyStimGenerator->randomizeList();
			}

			if ((parentRetinotopyMapper->emptyTriggerSteps > 0))
			{
				parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","EmptyRandomList Filled(BlockNumber:" + QString::number(tmpBlockStrc.getBlockNumber()) + ", TrialNumber:" + QString::number(tmpExpStrState.CurrentBlock_TrialNumber) + ")");
				tmpStr = "";
				for (j=0;j<parentRetinotopyMapper->cycleTriggerAmount;j++)
				{
					if (j==0)
						tmpStr = parentRetinotopyMapper->randEmptyStimGenerator->at(j);
					else
						tmpStr = tmpStr + QString(",") + parentRetinotopyMapper->randEmptyStimGenerator->at(j);
				}
				parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","EmptyRandomList(steps) = ",tmpStr);
				//if(isDebugMode())
					//emit LogToOutputWindow("EmptyRandomList(steps) = " + tmpStr);
			}
			parentRetinotopyMapper->emptyTriggerStepCount = parentRetinotopyMapper->emptyTriggerSteps;
		}

		if (parentRetinotopyMapper->randomizeTriggerSteps && parentRetinotopyMapper->nextNewCycleEntered)
		{
			if(parentRetinotopyMapper->randomizeTriggerStepsArray.count() == parentRetinotopyMapper->cycleTriggerAmount)
			{
				//if(parentRetinotopyMapper->randStimStateGenerator->count() != parentRetinotopyMapper->cycleTriggerAmount)
				//{
					parentRetinotopyMapper->randStimStateGenerator->clear();
					for (j=0;j<parentRetinotopyMapper->cycleTriggerAmount;j++)//Create random Stimuli trigger steps within a cycle
					{
						parentRetinotopyMapper->randStimStateGenerator->append(parentRetinotopyMapper->randomizeTriggerStepsArray.at(j));
					}
				//}
				//User now needs to take care of randomization and balancing etc.. --> parentRetinotopyMapper->randStimStateGenerator->randomizeList(RandomGenerator_NoRandomizePreservedIndexes,parentRetinotopyMapper->previousRandEmptyStimGenerator);
			}
			else
			{
				if(parentRetinotopyMapper->randStimStateGenerator->count() != parentRetinotopyMapper->cycleTriggerAmount)
				{
					parentRetinotopyMapper->randStimStateGenerator->clear();
					for (j=0;j<parentRetinotopyMapper->cycleTriggerAmount;j++)//Create random Stimuli trigger steps within a cycle
					{
						parentRetinotopyMapper->randStimStateGenerator->append(QString::number(j));
					}
				}
				parentRetinotopyMapper->randStimStateGenerator->randomizeList(RandomGenerator::RandomGenerator_RandomizePreservedIndexes,parentRetinotopyMapper->previousRandEmptyStimGenerator);
			}


			//experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"","StimRandomList Filled(Block:" + QString::number(expSnapshot.currExpBlock) + ", Trial:" + QString::number(expSnapshot.currExpTrial) + ")");
			tmpStr = "";
			for (j=0;j<parentRetinotopyMapper->cycleTriggerAmount;j++)
			{
				if (j==0)
					tmpStr = parentRetinotopyMapper->randStimStateGenerator->at(j);
				else
					tmpStr = tmpStr + QString(",") + parentRetinotopyMapper->randStimStateGenerator->at(j);
			}
			parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","StimRandomList = ",tmpStr);
			//if(isDebugMode())
				//emit LogToOutputWindow("StimRandomList = " + tmpStr);
		}	

		if (parentRetinotopyMapper->discreteTriggerSteps)
		{	
			int numberOfSteps = 1;
			if (parentRetinotopyMapper->cycleTriggerAmount > 1)
			{
				if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_POLARANGLE) 
				{
					//0-->1 travels from 0 to 360(which is also 0!) degrees
					//To use a full scale spread we'll use the following:
					numberOfSteps = parentRetinotopyMapper->cycleTriggerAmount;
				}
				else
				{
					//0-->1 travels from 0 to 100(which is not 0!) procent
					//To use a full scale spread we'll use the following:
					numberOfSteps = parentRetinotopyMapper->cycleTriggerAmount-1;
				}
				int nTrialIndex;
				if (parentRetinotopyMapper->randomizeTriggerSteps)
					nTrialIndex = parentRetinotopyMapper->randStimStateGenerator->at(tmpExpStrState.CurrentBlock_InternalTrigger%parentRetinotopyMapper->cycleTriggerAmount).toInt()%parentRetinotopyMapper->cycleTriggerAmount;
				else
					nTrialIndex = tmpExpStrState.CurrentBlock_InternalTrigger%parentRetinotopyMapper->cycleTriggerAmount;
				parentRetinotopyMapper->fTrialTimeProgress = (float)(nTrialIndex) / numberOfSteps;
			}
			else
			{
				parentRetinotopyMapper->fTrialTimeProgress = (float)tmpExpStrState.CurrentBlock_InternalTrigger / parentRetinotopyMapper->cycleTriggerAmount;
			}
			//qDebug() << fTrialTimeProgress;
		} 
		else
		{
			if (parentRetinotopyMapper->randomizeTriggerSteps)
			{
				float subElapsedTrialTime = elapsedTrialTime%(parentRetinotopyMapper->triggerDurationMsec);
				float newElapsedTrialTime = (float)subElapsedTrialTime + (parentRetinotopyMapper->randStimStateGenerator->at(tmpExpStrState.CurrentBlock_InternalTrigger%parentRetinotopyMapper->cycleTriggerAmount).toFloat() * parentRetinotopyMapper->triggerDurationMsec);//(randStimStateGenerator->at(currExpBlockTrialTrigger).toFloat() * triggerDurationMsec);
				parentRetinotopyMapper->fTrialTimeProgress = (float)newElapsedTrialTime / (parentRetinotopyMapper->cycleTriggerAmount * parentRetinotopyMapper->triggerDurationMsec);
			}
			else
			{
				parentRetinotopyMapper->fTrialTimeProgress = (float)elapsedTrialTime / (parentRetinotopyMapper->cycleTriggerAmount * parentRetinotopyMapper->triggerDurationMsec);
			}
		}
		if (parentRetinotopyMapper->fTrialTimeProgress>1.0f)//Maximum for this cycle overridden?
			parentRetinotopyMapper->fTrialTimeProgress = parentRetinotopyMapper->fTrialTimeProgress - ((int)parentRetinotopyMapper->fTrialTimeProgress);//Revert cycle to beginning (subtract the full cycles so that 0<=fTrialTimeProgress>=1 counts)
	
		if (parentRetinotopyMapper->nextNewCycleEntered)
			parentRetinotopyMapper->emptyTriggerStepCount = parentRetinotopyMapper->emptyTriggerSteps;

		if   (  (  (parentRetinotopyMapper->emptyTriggerSteps >0)  &&  ((parentRetinotopyMapper->emptyTriggerStepCount>0)||(parentRetinotopyMapper->emptyTriggerLastIndex==tmpExpStrState.CurrentBlock_InternalTrigger)       )  )  )
		{
			if (parentRetinotopyMapper->emptyTriggerLastIndex != tmpExpStrState.CurrentBlock_InternalTrigger) 
			{
				if(parentRetinotopyMapper->emptyTriggerStepCount>0)
				{
					for (int k=0;k<parentRetinotopyMapper->emptyTriggerStepCount;k++)//Check if one of the empty steps is equal to the current trigger
					{
						if (parentRetinotopyMapper->randEmptyStimGenerator->count() > k)
						{
							int nSelectedBlockTrialStep;
							nSelectedBlockTrialStep = tmpExpStrState.CurrentBlock_InternalTrigger%parentRetinotopyMapper->cycleTriggerAmount;
							if (parentRetinotopyMapper->randEmptyStimGenerator->at(k).toInt()==nSelectedBlockTrialStep)
							{
								bRenderStimuli = false;
								parentRetinotopyMapper->emptyTriggerLastIndex = tmpExpStrState.CurrentBlock_InternalTrigger;
								parentRetinotopyMapper->emptyTriggerStepCount--;
								tmpStr = parentRetinotopyMapper->randEmptyStimGenerator->takeAt(k);
								parentRetinotopyMapper->previousRandEmptyStimGenerator->append(tmpStr);
								parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"",QString("Removed a empty at (step=")+QString::number(nSelectedBlockTrialStep)+QString(", size=")+QString::number(parentRetinotopyMapper->randEmptyStimGenerator->count())+QString(")"),parentRetinotopyMapper->previousRandEmptyStimGenerator->last());
								if (parentRetinotopyMapper->previousRandEmptyStimGenerator->count() == parentRetinotopyMapper->cycleTriggerAmount)
								{
									parentRetinotopyMapper->previousRandEmptyStimGenerator->clear();
								}
								if(parentRetinotopyMapper->randEmptyStimGenerator->isEmpty())
									parentRetinotopyMapper->bAllTrialEmptyProcessed = true;
								break;
							}
						}
					}
				}
			}
			else//This one should not be shown, the trigger is still equal to the last one where it was empty
			{
				bRenderStimuli = false;
			}
		}
		else if(parentRetinotopyMapper->emptyTriggerLastIndex==tmpExpStrState.CurrentBlock_InternalTrigger)//This one should not be shown, the trigger is still equal to the last one where it was empty
		{
			bRenderStimuli = false;
		}
	}
	if (bFistRenderCall)
	{
		if(parentRetinotopyMapper->experimentManager)
			parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","First rendering of experiment: ",tmpExpStr.getExperimentName());
		stimuliPainter->fillRect(parentRetinotopyMapper->rectScreenRes,QColor(Qt::black));
	}
	parentRetinotopyMapper->StimulusResultImageFrame.fill(parentRetinotopyMapper->colorBackground);
	imgPainter.begin(&parentRetinotopyMapper->StimulusResultImageFrame);
	if (parentRetinotopyMapper->bCreateActivationMap)
	{
		parentRetinotopyMapper->StimulusActivationMap.fill(parentRetinotopyMapper->blackColor);//The activation map should be first filled with 0's
		activationPainter.begin(&parentRetinotopyMapper->StimulusActivationMap);//For the parentRetinotopyMapper->StimulusActivationMap
	}
	if (parentRetinotopyMapper->antiAliasing)
		imgPainter.setRenderHint(QPainter::Antialiasing);
	imgPainter.setPen(parentRetinotopyMapper->textPen);
	imgPainter.setFont(parentRetinotopyMapper->textFont);

	if(elapsedTrialTimeCopy >= parentRetinotopyMapper->flickrThreshold)//Can we already switch the flickr state?
	{
		if(parentRetinotopyMapper->flickrSwitch == 0)
			parentRetinotopyMapper->flickrSwitch = 1;    
		else
			parentRetinotopyMapper->flickrSwitch = 0;		
		parentRetinotopyMapper->flickrThreshold += (1000 / parentRetinotopyMapper->flickrSpeedFreq);//Calculate the next flickr threshold moment in time
	}

	nStimFrameHeight = parentRetinotopyMapper->StimulusResultImageFrame.height();
	nStimFrameWidth = parentRetinotopyMapper->StimulusResultImageFrame.width();
	if (parentRetinotopyMapper->isLocked())
	{
		QFont textFont("arial", 22, QFont::Bold, false);
		QString strText;
		QString strTextStart = "Experiment ready, ";
		if (parentRetinotopyMapper->isReadyToUnlock())
			strText = strTextStart + "waiting for a trigger to start...";
		else
			strText = strTextStart + "press 'Alt' to proceed or CTRL + 'a' to abort the experiment";
		//if(isDebugMode() && pExperimentManager)
		//	pExperimentManager->logExperimentObjectData(nObjectID,0,__FUNCTION__,"","Painting the initial widget","Step 2");
		const int nBorder = 25;
		QPainterPath textPath;
		textPath.addText(0,0,textFont,strText);
		const QRectF textPathRect = textPath.boundingRect();
		imgPainter.setRenderHint(QPainter::Antialiasing);
		//imgPainter.fillRect(windowRect,QColor(87,87,87));
		//if(isDebugMode() && pExperimentManager)
		//	pExperimentManager->logExperimentObjectData(nObjectID,0,__FUNCTION__,"","Painting the initial widget","Step 3");
		imgPainter.setPen(Qt::NoPen);
		imgPainter.setBrush(Qt::white);
		//QRect rViewPort = imgPainter.viewport();
		imgPainter.setWindow ( textPathRect.x() - nBorder , textPathRect.y() - (parentRetinotopyMapper->rStimuliScreenArea.height()/2) , textPathRect.width() + (nBorder*2) , parentRetinotopyMapper->rStimuliScreenArea.height());//translate text rect to rect window button
		imgPainter.drawPath(textPath);
		//if(isDebugMode() && pExperimentManager)
		//	pExperimentManager->logExperimentObjectData(nObjectID,0,__FUNCTION__,"","Painting the initial widget","Step 4");
		//imgPainter.end();
	}
	else if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_POLARANGLE)
	{
		drawPolar();
	}
	else if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_ECCENTRICITY)
	{
		drawEccentricity();
	}
	else if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_MOVINGBAR)
	{
		if (parentRetinotopyMapper->nextNewCycleEntered)
		{
			if(parentRetinotopyMapper->experimentManager)
				parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","New Bar cycle entered, drawing Bar Angle:",QString::number(parentRetinotopyMapper->movingBarAngle));
		}
		drawMovingBar();
	}
	else if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_MOVINGDOTS)
	{
		drawMovingDots();
	}
	else if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_FIXATION)
	{
		drawFixation();
	}
	else if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_CUSTOM)
	{
		drawCustom();
	}
	if ((parentRetinotopyMapper->bCreateActivationMap) && (activationPainter.isActive()))
		activationPainter.end();

	if(imgPainter.isActive())
		imgPainter.end();

	if (parentRetinotopyMapper->outputTriggerFrame)
	{
		if(parentRetinotopyMapper->retinoOutputType == RETINOMAPPER_OUTPUTTYPE_FRAME)
			doOutputTriggerFrame(tmpExpStrState,tmpExpStr,tmpBlockStrc,parentRetinotopyMapper->StimulusResultImageFrame); 
		else if(parentRetinotopyMapper->retinoOutputType == RETINOMAPPER_OUTPUTTYPE_MASK)
			doOutputTriggerFrame(tmpExpStrState,tmpExpStr,tmpBlockStrc,parentRetinotopyMapper->StimulusActivationMap); 
	}

	stimuliPainter->fillRect(parentRetinotopyMapper->rStimuliScreenArea, parentRetinotopyMapper->brushBackground);
	stimuliPainter->setPen(parentRetinotopyMapper->textPen);
	stimuliPainter->setFont(parentRetinotopyMapper->textFont);	
	stimuliPainter->drawPixmap(parentRetinotopyMapper->rStimuliScreenArea.x(),parentRetinotopyMapper->rStimuliScreenArea.y(),parentRetinotopyMapper->StimulusResultImageFrame);//*parentRetinotopyMapper->StimulusResultImageFrame);

	if (parentRetinotopyMapper->customScriptHandlerFunction && parentRetinotopyMapper->getScriptEngine())
	{
		parentRetinotopyMapper->scriptVal1 = parentRetinotopyMapper->getScriptEngine()->toScriptValue<QPainter *>(stimuliPainter);
		if (parentRetinotopyMapper->currentExpType == RETINOMAPPER_PATTERN_FIXATION)
		{
			parentRetinotopyMapper->customScriptHandlerFunction->call(QScriptValue(), QScriptValueList() << parentRetinotopyMapper->scriptVal1);
		}
		else
		{
			parentRetinotopyMapper->scriptVal2 = parentRetinotopyMapper->getScriptEngine()->toScriptValue<float>(parentRetinotopyMapper->fTrialTimeProgress);
			parentRetinotopyMapper->customScriptHandlerFunction->call(QScriptValue(), QScriptValueList() << parentRetinotopyMapper->scriptVal1 << parentRetinotopyMapper->scriptVal2);
		}
	}

	//if (customScriptHandlerFunction && currentScriptEngine)
	//{
	//	scriptVal = currentScriptEngine->toScriptValue<QPixmap *>(parentRetinotopyMapper->StimulusResultImageFrame);
	//	funcReturnValue = customScriptHandlerFunction->call(QScriptValue(), QScriptValueList() << scriptVal);
	//	tmpPixmap = NULL;
	//	tmpPixmap = currentScriptEngine->fromScriptValue<QPixmap *>(funcReturnValue);
	//	if(tmpPixmap)
	//		*parentRetinotopyMapper->StimulusResultImageFrame = tmpPixmap->copy();	
	//}		
	//stimuliPainter->drawPixmap(0,0,*parentRetinotopyMapper->StimulusResultImageFrame);

	//if(isDebugMode())
	//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString::number(5));

	if(parentRetinotopyMapper->isDebugMode())
	{
		//if (debugUsedTestSamples == -1)//This is the first experiment sample and is ignored for the speed measurement
		//{
		//	debugTotalElapsedTime = 0;
		//	debugUsedTestSamples++;
		//}
		//else if (debugUsedTestSamples<debugTestSamples)//During these samples we collect the information for the speed measurement
		//{
		//	debugTotalElapsedTime = debugTotalElapsedTime + debugElapsedTime;
		//	debugUsedTestSamples++;
		//	debugString = debugString + ", " + QString::number(debugElapsedTime);
		//}
		//else if (debugUsedTestSamples==debugTestSamples)//During this sample all the debugging functionality is processed and also ignored for the speed measurement
		//{
		//float fAverageElapsed = debugTotalElapsedTime/debugUsedTestSamples;
		//textContent = QString("Average elapsed time: %1").arg(fAverageElapsed); //+ ": " + debugString; //"Retinotopic Mapping";
		//painter.drawText(QRect(10, 0, rectScreenRes.width(), 100), Qt::AlignLeft, textContent);//Takes about 5ms additional total drawing time!
		parentRetinotopyMapper->textContent = "ExternalTriggerNr:" + QString::number(tmpExpStrState.CurrentBlock_ExternalTrigger) + ", InternalTriggerNr:" + QString::number(tmpExpStrState.CurrentBlock_InternalTrigger) + ", TrialNr:" + QString::number(tmpExpStrState.CurrentBlock_TrialNumber) + ", BlockNr:" + QString::number(tmpBlockStrc.getBlockNumber());
		//stimuliPainter->drawText(QRect(10, 50, parentRetinotopyMapper->rectScreenRes.width(), 100), Qt::AlignLeft, parentRetinotopyMapper->textContent);
		stimuliPainter->drawText(parentRetinotopyMapper->rStimuliScreenArea, Qt::AlignLeft, parentRetinotopyMapper->textContent);
		//debugUsedTestSamples++;
		//}
		//else if (debugUsedTestSamples==(debugTestSamples+1))//During this sample we reset all the information for the speed measurement again
		//{
		//	debugTotalElapsedTime = 0;
		//	debugUsedTestSamples = 0;
		//}
	}
	//if(isDebugMode())
	//{
	//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"","painter.end()");
	//	//emit LogExpObjData(nRetinoID,0,"paintEvent():painter.end()2");//0 is the default experiment timer
	//}
	//if(isDebugMode())
	//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString::number(6));
	parentRetinotopyMapper->nextNewCycleEntered = false;
	parentRetinotopyMapper->nextNewBlockEntered = false;
	if(parentRetinotopyMapper->isDebugMode())
		parentRetinotopyMapper->experimentManager->logExperimentObjectData(parentRetinotopyMapper->getObjectID(),0,__FUNCTION__,"","Finished painting the object");
}

bool RetinotopyMapperWindow::doOutputTriggerFrame(const ExperimentStructuresNameSpace::ExperimentStructureState &tmpExpStrState, const cExperimentStructure &tmpExpStr, const cBlockStructure &tmpBlockStrc, const QPixmap &pix2Output)
{
	if (tmpExpStrState.CurrentBlock_InternalTrigger < nLastOutputTriggerFrameNumber)
	{
		nLastOutputTriggerFrameNumber = ExperimentStructuresNameSpace::RA_REINITIALIZE;//tmpExpStrState.CurrentBlock_InternalTrigger;
	}
	if (tmpExpStrState.CurrentBlock_InternalTrigger > nLastOutputTriggerFrameNumber)
	{
		QString outputDir = MainAppInfo::outputsDirPath();
		outputDir = outputDir + RETINOMAPPER_OUTPUT_SUBFOLDER;
		if(QDir(outputDir).exists()==false)
		{
			QDir().mkdir(outputDir);
		}
		outputDir = outputDir + parentRetinotopyMapper->getLastLoggedObjectStateTime(Experiment_SubObject_Started) + "_" + tmpExpStr.getExperimentName() + "/";
		if(QDir(outputDir).exists()==false)
		{
			QDir().mkdir(outputDir);
		}
		QString fileName = outputDir + QString::number(parentRetinotopyMapper->getObjectID()) + "_" + QString::number(tmpBlockStrc.getBlockNumber()) + QString("_") + QString::number(tmpExpStrState.CurrentBlock_TrialNumber) + QString("_") + QString::number(tmpExpStrState.CurrentBlock_InternalTrigger);

		if (parentRetinotopyMapper->retinoOutputFormat == RETINOMAPPER_OUTPUTFORMAT_PNG)
		{
			fileName = fileName + QString(".png");
			if(outputFile)
			{
				if(outputFile->isOpen())
					outputFile->close();
				delete outputFile;
			}
			outputFile = new QFile(fileName);
			if (outputFile->open(QIODevice::WriteOnly))
			{
				pix2Output.save(outputFile, "PNG");
				outputFile->close();
			}
		} 
		else if (parentRetinotopyMapper->retinoOutputFormat == RETINOMAPPER_OUTPUTFORMAT_DAT)
		{
			fileName = fileName + QString(".dat");
			if(outputFile)
			{
				if(outputFile->isOpen())
					outputFile->close();
				delete outputFile;
			}
			outputFile = new QFile(fileName);					
			if (outputFile->open(QIODevice::WriteOnly))
			{
				QImage imgWrite = pix2Output.toImage().convertToFormat(QImage::Format_ARGB32);
				QDataStream output(outputFile);
				quint32 magic = ARGB_FORMAT_HEADER;
				quint32 width = imgWrite.width();
				quint32 height = imgWrite.height();
				output << magic << width << height;
				for (quint32 y = 0; y < height; ++y) 
				{
					QRgb *scanLine = (QRgb *)imgWrite.scanLine(y);
					for (quint32 x = 0; x < width; ++x)
					{
						output << scanLine[x];
					}
				}
				//bool bResult = 
				//output.status() == QDataStream::Ok;
				outputFile->close();
			}
		}
		else if (parentRetinotopyMapper->retinoOutputFormat == RETINOMAPPER_OUTPUTFORMAT_CDAT)
		{
			QImage imgWrite;
			bool bFirstTime = ((bCDATFileReadyToWrite == false) && (outputFile == NULL));
			if (bFirstTime)
			{
				fileName = outputDir + QString::number(parentRetinotopyMapper->getObjectID()) + "_" + parentRetinotopyMapper->retinoOutputType + QString(".cdat");//tmpExpStr.getExperimentName()
				if(outputFile)
				{
					if(outputFile->isOpen())
						outputFile->close();
					delete outputFile;
				}
				outputFile = new QFile(fileName);
				bCDATFileReadyToWrite = outputFile->open(QIODevice::WriteOnly);
				if(bCDATFileReadyToWrite)
				{
					CDatOutputStream = new QDataStream(outputFile);
					imgWrite = pix2Output.toImage().convertToFormat(QImage::Format_ARGB32);
					OutputImageWidth = imgWrite.width();
					OutputImageHeight = imgWrite.height();
					OutputImageItemCount = 1;
					*CDatOutputStream << ARGB_FORMAT_HEADER_CONC << OutputImageItemCount << OutputImageWidth << OutputImageHeight;
				}
			}					
			if (bCDATFileReadyToWrite && outputFile)
			{
				if(outputFile->isOpen())
				{
					if(bFirstTime == false)
					{
						imgWrite = pix2Output.toImage().convertToFormat(QImage::Format_ARGB32);
						OutputImageItemCount++;
					}
					for (quint32 y = 0; y < OutputImageHeight; ++y) 
					{
						QRgb *scanLine = (QRgb *)imgWrite.scanLine(y);
						for (quint32 x = 0; x < OutputImageWidth; ++x)
						{
							*CDatOutputStream << scanLine[x];
						}
					}
				}
			}
		}
		nLastOutputTriggerFrameNumber = tmpExpStrState.CurrentBlock_InternalTrigger;
		return true;
	}
	return true;
}

bool RetinotopyMapperWindow::drawPolar()
{
	if (bRenderStimuli)
	{
		float startAngle;
		parentRetinotopyMapper->wedgeSpanAngle = parentRetinotopyMapper->polarWedgeSpan / parentRetinotopyMapper->polarWedgeNrChecks * 16.0f;
		if((parentRetinotopyMapper->disableCortMagFac==false) && (parentRetinotopyMapper->cortMagFactor > 0))
			parentRetinotopyMapper->currentWedgeDiameter = ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter)) / 2.0f * parentRetinotopyMapper->cortMagFactor;
		else
			parentRetinotopyMapper->currentWedgeDiameter = ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter)) / 2.0f / parentRetinotopyMapper->polarWedgeNrRings;		
		parentRetinotopyMapper->dCurrentSize = nStimFrameHeight - parentRetinotopyMapper->currentWedgeDiameter;
		parentRetinotopyMapper->currentXPoint = (nStimFrameWidth - parentRetinotopyMapper->dCurrentSize) / 2.0f;
		parentRetinotopyMapper->currentYPoint = (nStimFrameHeight - parentRetinotopyMapper->dCurrentSize) / 2.0f;
		if(parentRetinotopyMapper->polarRotationDirection == 1)//Clockwise
		{
			startAngle = (-360.0f * parentRetinotopyMapper->fTrialTimeProgress) - (parentRetinotopyMapper->polarWedgeSpan);
		}
		else//Counterclockwise 
		{
			startAngle = (360.0f * parentRetinotopyMapper->fTrialTimeProgress);
		}
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor,(nStimFrameHeight - parentRetinotopyMapper->gapDiameter) / 2.0f, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
			float fHalfWedgeLenght = (nStimFrameHeight - parentRetinotopyMapper->gapDiameter) / 2.0f;
			activationPainter.drawArc(fHalfWedgeLenght/2, fHalfWedgeLenght/2, (nStimFrameHeight - (fHalfWedgeLenght)), (nStimFrameHeight - (fHalfWedgeLenght)), (startAngle * 16.0f), (parentRetinotopyMapper->polarWedgeSpan * 16.0f));
		}
		for(int i=1; i<(parentRetinotopyMapper->polarWedgeNrRings+1);i++)
		{
			parentRetinotopyMapper->currentStartAngle = startAngle * 16.0f;
			for(int k=0; k<parentRetinotopyMapper->polarWedgeNrChecks;k++)
			{
				if(parentRetinotopyMapper->flickrSwitch==1)
				{
					if((k+i)%2==0)
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor1, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
					else
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor2, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
				}
				else
				{
					if((k+i)%2!=0)
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor1, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
					else
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor2, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
				}	
				if(k==0)//draw a full wide wedge
				{
					imgPainter.drawArc(parentRetinotopyMapper->currentXPoint, parentRetinotopyMapper->currentYPoint, parentRetinotopyMapper->dCurrentSize, parentRetinotopyMapper->dCurrentSize, parentRetinotopyMapper->currentStartAngle, parentRetinotopyMapper->wedgeSpanAngle*parentRetinotopyMapper->polarWedgeNrChecks);//Same as drawing an partial ellipse
				}					
				else if (k%2!=0)
				{
					imgPainter.drawArc(parentRetinotopyMapper->currentXPoint, parentRetinotopyMapper->currentYPoint, parentRetinotopyMapper->dCurrentSize, parentRetinotopyMapper->dCurrentSize, parentRetinotopyMapper->currentStartAngle, parentRetinotopyMapper->wedgeSpanAngle);//Same as drawing an partial ellipse
				}
				parentRetinotopyMapper->currentStartAngle = parentRetinotopyMapper->currentStartAngle + parentRetinotopyMapper->wedgeSpanAngle;
			}
			parentRetinotopyMapper->dCurrentSize = parentRetinotopyMapper->dCurrentSize - parentRetinotopyMapper->currentWedgeDiameter;//First subtract the first wedge diameter
			if((parentRetinotopyMapper->disableCortMagFac==false) && (parentRetinotopyMapper->cortMagFactor > 0))
			{
				if(i==(parentRetinotopyMapper->polarWedgeNrRings-1))//Pre-last loop to prepare last wedge?
					parentRetinotopyMapper->currentWedgeDiameter = (parentRetinotopyMapper->dCurrentSize-parentRetinotopyMapper->gapDiameter) / 2;
				else
					parentRetinotopyMapper->currentWedgeDiameter = ((parentRetinotopyMapper->dCurrentSize - parentRetinotopyMapper->currentWedgeDiameter) - parentRetinotopyMapper->gapDiameter) / 2 * parentRetinotopyMapper->cortMagFactor;
			}
			parentRetinotopyMapper->dCurrentSize = parentRetinotopyMapper->dCurrentSize - parentRetinotopyMapper->currentWedgeDiameter;//First subtract the second wedge diameter
			parentRetinotopyMapper->currentXPoint = (nStimFrameWidth - parentRetinotopyMapper->dCurrentSize) / 2.0f;
			parentRetinotopyMapper->currentYPoint = (nStimFrameHeight - parentRetinotopyMapper->dCurrentSize) / 2.0f;
		}
	}
	if(parentRetinotopyMapper->showFixationPoint) // show fix cross
	{
		imgPainter.setPen(QPen(parentRetinotopyMapper->fixationColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
		imgPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
			activationPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		}
	}
	return true;
}

bool RetinotopyMapperWindow::drawEccentricity()
{
	if (bRenderStimuli)
	{		
		QPen oldPen;
		parentRetinotopyMapper->wedgeSpanAngle = 360.0f / parentRetinotopyMapper->eccentricityNrChecks * 16.0f;		
		if((parentRetinotopyMapper->enableCortMagTiming)&&(parentRetinotopyMapper->cortMagFactor > 0.0f)&&(parentRetinotopyMapper->cortMagFactor < 1.0f))
		{
			//Matlab
			//CortMagFac = 0.2
			//CycleTriggerAmount = 8
			//CortMagFacTimingFactor = (100-(1/(((log(-CortMagFac+1)/(10/(CycleTriggerAmount-1)))-1)/-100)))/(CortMagFac*100)
			//time=[0:0.01:15];
			//Decrease
			//y=  ((-100+(CortMagFac*CortMagFacTimingFactor*100))*exp(((CortMagFac*CortMagFacTimingFactor*100)/(-100+(CortMagFac*CortMagFacTimingFactor*100)))*  time)    +100-(CortMagFac*CortMagFacTimingFactor*100))/(100-(CortMagFac*CortMagFacTimingFactor*100));
			//Increase
			//y=1-((-100+(CortMagFac*CortMagFacTimingFactor*100))*exp(((CortMagFac*CortMagFacTimingFactor*100)/(-100+(CortMagFac*CortMagFacTimingFactor*100)))*(-time+10))+100-(CortMagFac*CortMagFacTimingFactor*100))/(100-(CortMagFac*CortMagFacTimingFactor*100));
			//plot(time,y)
			//additional --> s = solve('CycleTriggerAmount=20','CortMagFac=0.3','t=(10/(CycleTriggerAmount-1))','((-100+(CortMagFac*answer*100))*exp(((CortMagFac*answer*100)/(-100+(CortMagFac*answer*100)))*t)+100-(CortMagFac*answer*100))/(100-(CortMagFac*answer*100))=CortMagFac')
			if(parentRetinotopyMapper->eccentricityDirection == -1)//Decrease ring diameter
			{			
				parentRetinotopyMapper->fCalculatedCortMagFacTimingConst =(100-(1/(((qLn(-parentRetinotopyMapper->cortMagFactor+1)/(10/(float)(parentRetinotopyMapper->cycleTriggerAmount-1)))-1)/-100)))/(parentRetinotopyMapper->cortMagFactor*100);
				float fTau = (float)(parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100)/(-100 + (parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100));
				parentRetinotopyMapper->fCortMagTrialTimeProgress = ((-100 + (parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100))*qExp(fTau *(parentRetinotopyMapper->fTrialTimeProgress*10))+100-(parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100))/(100-(parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100));
			}
			else
			{
				parentRetinotopyMapper->fCalculatedCortMagFacTimingConst =(100-(1/(((qLn(-parentRetinotopyMapper->cortMagFactor+1)/(10/(float)(parentRetinotopyMapper->cycleTriggerAmount-1)))-1)/-100)))/(parentRetinotopyMapper->cortMagFactor*100);
				float fTau = (float)(parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100)/(-100 + (parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100));
				parentRetinotopyMapper->fCortMagTrialTimeProgress = 1-((-100 + (parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100))*qExp(fTau *( (parentRetinotopyMapper->fTrialTimeProgress*-10)+10 ))+100-(parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100))/(100-(parentRetinotopyMapper->cortMagFactor * parentRetinotopyMapper->fCalculatedCortMagFacTimingConst*100));
			}				
		}
		else
		{
			parentRetinotopyMapper->fCortMagTrialTimeProgress = parentRetinotopyMapper->fTrialTimeProgress;
		}
		if (parentRetinotopyMapper->disableCortMagFac)
		{
			parentRetinotopyMapper->currentCompleteWedgeDiameter = ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter) * parentRetinotopyMapper->cortMagFactor) / 2.0f;
			//currentCompleteWedgeDiameter = ((nStimFrameHeight - gapDiameter)) / 2.0f;
			parentRetinotopyMapper->currentWedgeDiameter = parentRetinotopyMapper->currentCompleteWedgeDiameter / parentRetinotopyMapper->eccentricityNrRings;
			if(parentRetinotopyMapper->eccentricityDirection == -1)//Decrease ring diameter
				parentRetinotopyMapper->currentOuterCompleteRingDiameter = nStimFrameHeight - ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter - (2*parentRetinotopyMapper->currentCompleteWedgeDiameter)) * parentRetinotopyMapper->fCortMagTrialTimeProgress);
			else//Increase ring diameter
				parentRetinotopyMapper->currentOuterCompleteRingDiameter = (parentRetinotopyMapper->gapDiameter + (2*parentRetinotopyMapper->currentCompleteWedgeDiameter)) + ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter - (2*parentRetinotopyMapper->currentCompleteWedgeDiameter)) * parentRetinotopyMapper->fCortMagTrialTimeProgress);
		} 
		else
		{
			if(parentRetinotopyMapper->eccentricityDirection == -1)//Decrease ring diameter
			{
				parentRetinotopyMapper->currentCompleteWedgeDiameter = (((nStimFrameHeight - parentRetinotopyMapper->gapDiameter) * parentRetinotopyMapper->cortMagFactor) - ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter) * parentRetinotopyMapper->fCortMagTrialTimeProgress * parentRetinotopyMapper->cortMagFactor)) / 2.0f;
				parentRetinotopyMapper->currentWedgeDiameter = parentRetinotopyMapper->currentCompleteWedgeDiameter / parentRetinotopyMapper->eccentricityNrRings;
				parentRetinotopyMapper->currentOuterCompleteRingDiameter = nStimFrameHeight - ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter) * parentRetinotopyMapper->fCortMagTrialTimeProgress);
				//if (fCortMagTrialTimeProgress > 0.0)
				//{
				//	int aa = 7;
				//}
			}
			else//Increase ring diameter
			{
				parentRetinotopyMapper->currentCompleteWedgeDiameter = (((nStimFrameHeight - parentRetinotopyMapper->gapDiameter) / 2.0f) * parentRetinotopyMapper->fCortMagTrialTimeProgress * parentRetinotopyMapper->cortMagFactor) ;
				parentRetinotopyMapper->currentWedgeDiameter = parentRetinotopyMapper->currentCompleteWedgeDiameter / parentRetinotopyMapper->eccentricityNrRings;
				parentRetinotopyMapper->currentOuterCompleteRingDiameter = parentRetinotopyMapper->gapDiameter + ((nStimFrameHeight - parentRetinotopyMapper->gapDiameter) * parentRetinotopyMapper->fCortMagTrialTimeProgress);
			}
		}
		parentRetinotopyMapper->currentXPoint = (nStimFrameWidth - parentRetinotopyMapper->currentOuterCompleteRingDiameter + parentRetinotopyMapper->currentWedgeDiameter) / 2.0f;
		parentRetinotopyMapper->currentYPoint = (nStimFrameHeight - parentRetinotopyMapper->currentOuterCompleteRingDiameter + parentRetinotopyMapper->currentWedgeDiameter) / 2.0f;
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			float fTemp = parentRetinotopyMapper->currentOuterCompleteRingDiameter - (parentRetinotopyMapper->currentWedgeDiameter*parentRetinotopyMapper->eccentricityNrRings);
			float fTemp2 = fTemp + (2*parentRetinotopyMapper->currentCompleteWedgeDiameter);
			
			//First activate everything
			activationPainter.fillRect(QRect(0,0,nStimFrameWidth,nStimFrameHeight),parentRetinotopyMapper->whiteColor);
			//Change the pen to inactive(black)
			activationPainter.setPen(QPen(QColor(Qt::black), parentRetinotopyMapper->currentCompleteWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
			//The outer path rectangle
			QPainterPath OuterRectPath;
			OuterRectPath.addRect(0,0,nStimFrameWidth,nStimFrameHeight);
			//The outer path ellipse
			QPainterPath OuterEllipsePath;
			float fTemp2Adjusted = fTemp2 - parentRetinotopyMapper->currentCompleteWedgeDiameter;
			OuterEllipsePath.addEllipse((nStimFrameWidth - fTemp2Adjusted) / 2.0f, (nStimFrameHeight - fTemp2Adjusted) / 2.0f, fTemp2Adjusted, fTemp2Adjusted);
			//The inner path ellipse
			QPainterPath InnerEllipsePath;
			float fTempAdjusted = fTemp - parentRetinotopyMapper->currentCompleteWedgeDiameter;
			InnerEllipsePath.addEllipse((nStimFrameWidth - fTempAdjusted) / 2.0f, (nStimFrameHeight - fTempAdjusted) / 2.0f, fTempAdjusted, fTempAdjusted);
			//Bring it all together
			OuterEllipsePath = OuterEllipsePath.subtracted(InnerEllipsePath);
			QPainterPath FillInActivePath = OuterRectPath.subtracted(OuterEllipsePath);
			//Paint the result
			activationPainter.fillPath(FillInActivePath, Qt::black);

			//what should be filled originally?
			//activationPainter.setPen(QPen(QColor(Qt::red), parentRetinotopyMapper->currentCompleteWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
			//activationPainter.drawEllipse((nStimFrameWidth - fTemp) / 2.0f, (nStimFrameHeight - fTemp) / 2.0f, fTemp, fTemp);
		}
		for(int i=1;i<parentRetinotopyMapper->eccentricityNrRings+1;i++)
		{
			float startAngle = 0.0f * 16.0f;
			for(int k=0; k<parentRetinotopyMapper->eccentricityNrChecks; k++)
			{
				if(parentRetinotopyMapper->flickrSwitch ==1)
				{
					if((k+i)%2==0)
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor1, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
					else
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor2, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
				}
				else
				{
					if((k+i)%2!=0)
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor1, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
					else
					{
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor2, parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
					}
				}
				if(k==0)//first checker? 
				{
					if(i==1)//first ring?
					{//draw a full complete ring
						oldPen = imgPainter.pen();
						imgPainter.setPen(QPen(parentRetinotopyMapper->cCheckerColor1, parentRetinotopyMapper->currentCompleteWedgeDiameter, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
						float fTemp = parentRetinotopyMapper->currentOuterCompleteRingDiameter - (parentRetinotopyMapper->currentWedgeDiameter*parentRetinotopyMapper->eccentricityNrRings);
						//imgPainter.drawArc((nStimFrameWidth - fTemp) / 2.0f, (nStimFrameHeight - fTemp) / 2.0f, fTemp, fTemp, 0.0f, 5760.0f);
						imgPainter.drawEllipse((nStimFrameWidth - fTemp) / 2.0f, (nStimFrameHeight - fTemp) / 2.0f, fTemp, fTemp);
						imgPainter.setPen(oldPen);
					}
					imgPainter.drawEllipse(parentRetinotopyMapper->currentXPoint, parentRetinotopyMapper->currentYPoint, parentRetinotopyMapper->currentOuterCompleteRingDiameter - parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->currentOuterCompleteRingDiameter - parentRetinotopyMapper->currentWedgeDiameter);
				}					
				else if (k%2!=0)
				{
					imgPainter.drawArc(parentRetinotopyMapper->currentXPoint, parentRetinotopyMapper->currentYPoint, parentRetinotopyMapper->currentOuterCompleteRingDiameter - parentRetinotopyMapper->currentWedgeDiameter, parentRetinotopyMapper->currentOuterCompleteRingDiameter - parentRetinotopyMapper->currentWedgeDiameter, startAngle, parentRetinotopyMapper->wedgeSpanAngle);
				}
				startAngle = startAngle + parentRetinotopyMapper->wedgeSpanAngle;
			}
			//if(isDebugMode() && (debugUsedTestSamples==debugTestSamples))
			//{
			//	imgPainter.setPen(QPen(QColor(255,0,0), 1, style, cap));
			//	imgPainter.drawEllipse(currentXPoint, currentYPoint, currentSize, currentSize);//To draw this it takes about 2ms additional time!!
			//}
			if(parentRetinotopyMapper->eccentricityDirection == -1)//Decrease ring diameter
			{
				parentRetinotopyMapper->currentOuterCompleteRingDiameter = parentRetinotopyMapper->currentOuterCompleteRingDiameter - (2*parentRetinotopyMapper->currentWedgeDiameter);
			}
			else//Increase ring diameter
			{
				parentRetinotopyMapper->currentOuterCompleteRingDiameter = parentRetinotopyMapper->currentOuterCompleteRingDiameter - (2*parentRetinotopyMapper->currentWedgeDiameter);
			}
			parentRetinotopyMapper->currentXPoint = (nStimFrameWidth - parentRetinotopyMapper->currentOuterCompleteRingDiameter + parentRetinotopyMapper->currentWedgeDiameter) / 2.0f;
			parentRetinotopyMapper->currentYPoint = (nStimFrameHeight - parentRetinotopyMapper->currentOuterCompleteRingDiameter + parentRetinotopyMapper->currentWedgeDiameter) / 2.0f;
		}		
	}
	//if(isDebugMode())
	//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString::number(3));
	if(parentRetinotopyMapper->showFixationPoint) // show fix cross
	{
		imgPainter.setPen(QPen(parentRetinotopyMapper->fixationColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
		imgPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
			activationPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		}
	}
	return true;
}

bool RetinotopyMapperWindow::drawMovingDots()
{
	if (bRenderStimuli)
	{
		imgPainter.setPen(QPen(parentRetinotopyMapper->movingDotsColor, parentRetinotopyMapper->movingDotsDotSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
		//fTrialTimeProgress = fTrialTimeProgress;//(float)elapsedTrialTime / (cycleTriggerAmount * triggerDurationMsec);
		//float fDiffTrialTimeProgress;
		//float fMovCorrFactor;
		//int nStimRefreshRate = getCurrentStimuliRefreshRate();
		//int nTrialDuration = (cycleTriggerAmount * triggerDurationMsec);

		//if ((fLastTrialTimeProgress < fTrialTimeProgress) && ((fLastTrialTimeProgress != 0.0)))
		//{
		//	fDiffTrialTimeProgress = fTrialTimeProgress - fLastTrialTimeProgress;
		//	//fDiffTrialTimeProgress = (1000.0/nStimRefreshRate)/nTrialDuration;//assume that it was fine...
		//} 
		//else
		//{				
		//	fDiffTrialTimeProgress = (1000.0/nStimRefreshRate)/nTrialDuration;//assume that it was fine...				
		//}	
		//fMovCorrFactor = (fDiffTrialTimeProgress * (nTrialDuration/1000.0)) * nStimRefreshRate;
		//fLastTrialTimeProgress = fTrialTimeProgress;

		for(int i=0; i<parentRetinotopyMapper->movingDotsNrOfDots; i++)
		{
			//nCurrentThreadNum = omp_get_thread_num();
			//nCurrentThreadAmount = omp_get_num_threads(); 
			//if ((nCurrentThreadNum != nDefaultThreadNum) )// || (nCurrentThreadAmount != nDefaultThreadAmount))
			//{
			//	nDefaultThreadNum = nCurrentThreadNum;
			//	nDefaultThreadAmount = nCurrentThreadAmount; 
			//}
			//if(isDebugMode())
				
			if(parentRetinotopyMapper->movingDotsIsStationary)
			{
				parentRetinotopyMapper->movingDots.Pos[i] = parentRetinotopyMapper->movingDots.OldPos[i];
			}
			else
			{
				//move the dots
				//#pragma omp atomic
				{
					//#pragma omp critical
					parentRetinotopyMapper->movingDots.Pos[i] = parentRetinotopyMapper->movingDots.OldPos.at(i) + (parentRetinotopyMapper->movingDots.Mov[i].p2());// * fMovCorrFactor);
					// check whether dot moves out of display field - x direction					
					if(parentRetinotopyMapper->movingDots.Pos[i].x() >= parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().x())
					{
						//#pragma omp atomic
						parentRetinotopyMapper->movingDots.Pos[i].setX(parentRetinotopyMapper->movingDots.Pos[i].x() - parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dx());
					}
					if(parentRetinotopyMapper->movingDots.Pos[i].x() <= parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().x())//
					{
						//#pragma omp atomic
						parentRetinotopyMapper->movingDots.Pos[i].setX(parentRetinotopyMapper->movingDots.Pos[i].x() + parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dx());
					}
					// check whether dot moves out of display field - y direction
					if(parentRetinotopyMapper->movingDots.Pos[i].y() >= parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().y())
					{
						//#pragma omp atomic
						parentRetinotopyMapper->movingDots.Pos[i].setY(parentRetinotopyMapper->movingDots.Pos[i].y() - parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dy());
					}
					if(parentRetinotopyMapper->movingDots.Pos[i].y() <= parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y())//movingDotsYStartRel)
					{
						//#pragma omp atomic
						parentRetinotopyMapper->movingDots.Pos[i].setY(parentRetinotopyMapper->movingDots.Pos[i].y() + parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dy());
					}
				}
			}
		}
		//parentRetinotopyMapper->StimulusResultImageFrame.fill(Qt::transparent);
		//#pragma omp parallel for --> doesn't work in the drawing routines!
		for(int i=0; i<parentRetinotopyMapper->movingDotsNrOfDots; i++)
		{
			if(parentRetinotopyMapper->isDebugMode())
			{
				if (i==0)
				{
					imgPainter.save();
					imgPainter.setPen(QPen(QColor(255,0,0), parentRetinotopyMapper->movingDotsDotSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
					imgPainter.drawPoint(parentRetinotopyMapper->movingDots.Pos.at(i));
					imgPainter.restore();
				} 
				else
				{
					imgPainter.drawPoint(parentRetinotopyMapper->movingDots.Pos.at(i));
				}

			}
			else
			{
				imgPainter.drawPoint(parentRetinotopyMapper->movingDots.Pos.at(i));
			}

								
			//Draw the remaining parts of a dot (mirrored!)
			if (parentRetinotopyMapper->movingDots.Pos.at(i).x()>=parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().x()-parentRetinotopyMapper->movingDotsDotSize)
				imgPainter.drawPoint(parentRetinotopyMapper->movingDots.Pos.at(i).x() - parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dx(),parentRetinotopyMapper->movingDots.Pos.at(i).y());
			if (parentRetinotopyMapper->movingDots.Pos.at(i).x()<=parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().x()+parentRetinotopyMapper->movingDotsDotSize)
				imgPainter.drawPoint(parentRetinotopyMapper->movingDots.Pos.at(i).x() + parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dx(),parentRetinotopyMapper->movingDots.Pos.at(i).y());
			if (parentRetinotopyMapper->movingDots.Pos.at(i).y()>=parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().y()-parentRetinotopyMapper->movingDotsDotSize)
				imgPainter.drawPoint(parentRetinotopyMapper->movingDots.Pos.at(i).x(),parentRetinotopyMapper->movingDots.Pos.at(i).y() - parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dy());
			if (parentRetinotopyMapper->movingDots.Pos.at(i).y()<=parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y()+parentRetinotopyMapper->movingDotsDotSize)
				imgPainter.drawPoint(parentRetinotopyMapper->movingDots.Pos.at(i).x(),parentRetinotopyMapper->movingDots.Pos.at(i).y() + parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dy());
			if (i==parentRetinotopyMapper->movingDotsNrOfDots-1)
			{
				//Remove the dot-parts that are outside the first hemisphere
				imgPainter.save();
				imgPainter.setPen(QPen(parentRetinotopyMapper->colorBackground, parentRetinotopyMapper->movingDotsDotSize*2, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
				imgPainter.drawLine(parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().x()+parentRetinotopyMapper->movingDotsDotSize,parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y()-(2*parentRetinotopyMapper->movingDotsDotSize),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().x()+parentRetinotopyMapper->movingDotsDotSize,parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().y()+(2*parentRetinotopyMapper->movingDotsDotSize));//Right border
				imgPainter.drawLine(parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().x()-(2*parentRetinotopyMapper->movingDotsDotSize),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y()-parentRetinotopyMapper->movingDotsDotSize,parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().x()+(2*parentRetinotopyMapper->movingDotsDotSize),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y()-parentRetinotopyMapper->movingDotsDotSize);//Top border				
				imgPainter.drawLine(parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().x()-parentRetinotopyMapper->movingDotsDotSize,parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y()-(2*parentRetinotopyMapper->movingDotsDotSize),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().x()-parentRetinotopyMapper->movingDotsDotSize,parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().y()+(2*parentRetinotopyMapper->movingDotsDotSize));//Left border				
				imgPainter.drawLine(parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().x()-(2*parentRetinotopyMapper->movingDotsDotSize),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().y()+parentRetinotopyMapper->movingDotsDotSize,parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().x()+(2*parentRetinotopyMapper->movingDotsDotSize),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p2().y()+parentRetinotopyMapper->movingDotsDotSize);//Bottom border				
					
				imgPainter.restore();
			}
			if(parentRetinotopyMapper->movingDotsHemifieldPos == RETINOMAPPER_POS_BOTH)
			{
				if (i==parentRetinotopyMapper->movingDotsNrOfDots-1)
				{
					mirrorImage = parentRetinotopyMapper->StimulusResultImageFrame.copy(parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().x(),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y(),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dx(),parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.dy()).toImage();
					//mirrorImage = mirrorImage.mirrored(true,false);
					//QPixmap mirror( QPixmap::fromImage( mirrorImage ) );						
					imgPainter.drawImage((parentRetinotopyMapper->stimWidthPixelAmount/2)+parentRetinotopyMapper->movingDotsPixelFromCenter,parentRetinotopyMapper->movingDotsFirstHemiVisibleArea.p1().y(),mirrorImage);//    10+movingDotsPixelFromCenter,0,mirrorImage);//movingDotsFirstHemiVisibleArea.p2().x()+
					//imgPainter.save();
					//imgPainter.setPen(Qt::NoPen);
					//imgPainter.setBrush(tmpPXMirrorMap);
					////imgPainter.setOpacity( 0.5 );
					////imgPainter.translate((stimWidthPixelAmount/2)+movingDotsPixelFromCenter, 0 );
					//imgPainter.translate(300,300);
					//int nCurrCompMode = imgPainter.compositionMode();
					//imgPainter.setCompositionMode(QPainter::CompositionMode_Plus);
					//nCurrCompMode = imgPainter.compositionMode();
					//imgPainter.drawRect(0,0,movingDotsFirstHemiVisibleArea.dx(),movingDotsFirstHemiVisibleArea.dy()); //mirrorImage->boundingRect() );
					//imgPainter.restore();
					//nCurrCompMode = imgPainter.compositionMode();	
				}
			}
			parentRetinotopyMapper->movingDots.OldPos[i] = parentRetinotopyMapper->movingDots.Pos.at(i);
		}
		//static bool firstUpdate = true;
		//if (firstUpdate)
		//{ 
		//	firstUpdate = false;
		//}
		//if(movingDotsRetPosition>0)
		//{
		//	QPainterPath ret_apeture;
		//	switch(movingDotsRetPosition)
		//	{
		//	case 1:
		//		ret_apeture.moveTo(300,0);
		//		ret_apeture.lineTo(300,rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width(),rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width(),0);
		//		ret_apeture.lineTo(rectScreenRes.width()/2,rectScreenRes.height()/2);
		//		ret_apeture.lineTo(rectScreenRes.width()/2,0);
		//		ret_apeture.lineTo(300,0);
		//		break;
		//	case 2:
		//		ret_apeture.moveTo(300,0);
		//		ret_apeture.lineTo(300,rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width(),rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width(),rectScreenRes.height()/2);
		//		ret_apeture.lineTo(rectScreenRes.width()/2,rectScreenRes.height()/2);
		//		ret_apeture.lineTo(rectScreenRes.width(),0);
		//		ret_apeture.lineTo(300,0);
		//		break;
		//	case 3:
		//		ret_apeture.moveTo(300,0);
		//		ret_apeture.lineTo(300,rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width(),rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width()/2,rectScreenRes.height()/2);
		//		ret_apeture.lineTo(rectScreenRes.width(),rectScreenRes.height()/2);
		//		ret_apeture.lineTo(rectScreenRes.width(),0);
		//		ret_apeture.lineTo(300,0);
		//		break;
		//	case 4:
		//		ret_apeture.moveTo(300,0);
		//		ret_apeture.lineTo(300,rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width()/2,rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width()/2,rectScreenRes.height()/2);
		//		ret_apeture.lineTo(rectScreenRes.width(),rectScreenRes.height());
		//		ret_apeture.lineTo(rectScreenRes.width(),0);
		//		ret_apeture.lineTo(300,0);
		//		break;
		//	}
		//	ret_apeture.closeSubpath();
		//	imgPainter.setBrush(brushBackground);
		//	imgPainter.setPen(colorBackground);//(87, 87, 87));
		//	imgPainter.drawPath(ret_apeture);
		//}
	}
	//if(isDebugMode())
	//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString::number(3));
	if(parentRetinotopyMapper->showFixationPoint) // show fix cross
	{
		imgPainter.setPen(QPen(parentRetinotopyMapper->fixationColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
		imgPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
			activationPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		}
	}
	return true;
}

bool RetinotopyMapperWindow::drawMovingBar()
{
	qreal qrYOffset = 0.0;
	//QPointF qOffset(0.0,0.0);



	if (bRenderStimuli)
	{
		//parentRetinotopyMapper->fStimulusDiameter = qSqrt(qPow(nStimFrameWidth,2) + qPow(nStimFrameHeight,2));//qSqrt(qPow(nStimFrameWidth,2) + qPow(nStimFrameHeight,2));
		parentRetinotopyMapper->dStimulusDiameter = qSqrt(qPow(nStimFrameWidth,2) + qPow(nStimFrameHeight,2));
		imgPainter.translate(nStimFrameWidth/2, nStimFrameHeight/2);
		imgPainter.rotate(parentRetinotopyMapper->movingBarAngle);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.translate(nStimFrameWidth/2, nStimFrameHeight/2);
			activationPainter.rotate(parentRetinotopyMapper->movingBarAngle);
		}
		double movingBarAreaLength = parentRetinotopyMapper->movingBarCoverage * parentRetinotopyMapper->dStimulusDiameter;
		//parentRetinotopyMapper->dCurrentSize = (movingBarAreaLength)/(parentRetinotopyMapper->movingBarHeight*parentRetinotopyMapper->movingBarHeightCheckAmount);
		parentRetinotopyMapper->dCurrentSize = (movingBarAreaLength)/(parentRetinotopyMapper->movingBarHeight*parentRetinotopyMapper->movingBarHeightCheckAmount);
		if(parentRetinotopyMapper->movingBarDirection == -1)//Down->Up (When 0 <= movingBarAngle >= 180 degrees)
		{				
			//if (movingBarIncludeOppositeDirection)
			//{
			//	if (fTrialTimeProgress < 0.5f)
			//		fYOffset = (((0.5 * movingBarCoverage * fStimulusDiameter) - (0.5 * currentSize * movingBarHeightCheckAmount) + (0.5 * currentSize)) - (((movingBarCoverage * fStimulusDiameter) - (currentSize * movingBarHeightCheckAmount) ) * (fTrialTimeProgress*2)));
			//	else
			//		fYOffset = (((-0.5 * movingBarCoverage * fStimulusDiameter) + (0.5 * currentSize * movingBarHeightCheckAmount) + (0.5 * currentSize)) + (((movingBarCoverage * fStimulusDiameter) - (currentSize * movingBarHeightCheckAmount) ) * ((fTrialTimeProgress-0.5f)*2)));
			//} 
			//else
			//qrYOffset
			//qOffset.setY((((0.5 * movingBarAreaLength) - (0.5 * parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) + (0.5 * parentRetinotopyMapper->dCurrentSize)) - (((movingBarAreaLength) - (parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) ) * parentRetinotopyMapper->fTrialTimeProgress)));
			qrYOffset = (((0.5 * movingBarAreaLength) - (0.5 * parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) + (0.5 * parentRetinotopyMapper->dCurrentSize)) - (((movingBarAreaLength) - (parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) ) * parentRetinotopyMapper->fTrialTimeProgress));
		}
		else//Up->Down (When 0 <= movingBarAngle >= 180 degrees)
		{
			//if (movingBarIncludeOppositeDirection)
			//{
			//	if (fTrialTimeProgress < 0.5f)
			//		fYOffset = (((-0.5 * movingBarCoverage * fStimulusDiameter) + (0.5 * currentSize * movingBarHeightCheckAmount) + (0.5 * currentSize)) + (((movingBarCoverage * fStimulusDiameter) - (currentSize * movingBarHeightCheckAmount) ) * (fTrialTimeProgress*2)));
			//	else
			//		fYOffset = (((0.5 * movingBarCoverage * fStimulusDiameter) - (0.5 * currentSize * movingBarHeightCheckAmount) + (0.5 * currentSize)) - (((movingBarCoverage * fStimulusDiameter) - (currentSize * movingBarHeightCheckAmount) ) * ((fTrialTimeProgress-0.5f)*2)));
			//} 
			//else
			//qrYOffset
			//double temp = (((-0.5 * movingBarAreaLength) + (0.5 * parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) + (0.5 * parentRetinotopyMapper->dCurrentSize)) + (((movingBarAreaLength) - (parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) )));
			//qOffset.setY((((-0.5 * movingBarAreaLength) + (0.5 * parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) + (0.5 * parentRetinotopyMapper->dCurrentSize)) + (((movingBarAreaLength) - (parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) ) * parentRetinotopyMapper->fTrialTimeProgress)));
			qrYOffset = (((-0.5 * movingBarAreaLength) + (0.5 * parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) + (0.5 * parentRetinotopyMapper->dCurrentSize)) + (((movingBarAreaLength) - (parentRetinotopyMapper->dCurrentSize * parentRetinotopyMapper->movingBarHeightCheckAmount) ) * parentRetinotopyMapper->fTrialTimeProgress));
		}

		//if(isDebugMode())
		//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString("3a"));
		//qreal a
		//qrYOffset = (int)(qrYOffset - 0.5);//DBL_MIN;    //-467.0;//ok: -469.0;//ok: -470.0;//no: -465.0;//no: -468.0;//no:-466.65;
		imgPainter.translate(0.0,qrYOffset);//qOffset);//qrYOffset);
		//if(isDebugMode())
		//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString("3b"));
		if(parentRetinotopyMapper->bCreateActivationMap)
			activationPainter.translate(0.0,qrYOffset);//qOffset);//0,qrYOffset);
		parentRetinotopyMapper->currentYPoint = (-1 * parentRetinotopyMapper->movingBarHeightCheckAmount * parentRetinotopyMapper->dCurrentSize) / 2.0;
		parentRetinotopyMapper->movingBarWidthCheckAmount = ((int)(parentRetinotopyMapper->dStimulusDiameter/parentRetinotopyMapper->dCurrentSize)+1);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor, parentRetinotopyMapper->dCurrentSize*parentRetinotopyMapper->movingBarHeightCheckAmount, parentRetinotopyMapper->style, parentRetinotopyMapper->flatCap));
			activationPainter.drawLine(-parentRetinotopyMapper->dStimulusDiameter/2, -0.5 * parentRetinotopyMapper->dCurrentSize, -parentRetinotopyMapper->dStimulusDiameter/2+(parentRetinotopyMapper->dCurrentSize*parentRetinotopyMapper->movingBarWidthCheckAmount), -0.5 * parentRetinotopyMapper->dCurrentSize);
		}
		//if(isDebugMode())
		//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString("3c"));
		imgPainter.drawImage(-parentRetinotopyMapper->dStimulusDiameter/2,parentRetinotopyMapper->currentYPoint-(0.5 * parentRetinotopyMapper->dCurrentSize),fractalFillCheckeredImage(parentRetinotopyMapper->dCurrentSize*parentRetinotopyMapper->movingBarWidthCheckAmount,parentRetinotopyMapper->movingBarHeightCheckAmount * parentRetinotopyMapper->dCurrentSize,parentRetinotopyMapper->dCurrentSize,parentRetinotopyMapper->flickrSwitch));
	}
	//if(isDebugMode())
	//	experimentManager->logExperimentObjectData(nRetinoID,0,__FUNCTION__,"",QString("SubPainting the object"),QString("3d"));
	if(parentRetinotopyMapper->showFixationPoint) // show fix cross
	{
		imgPainter.setPen(QPen(parentRetinotopyMapper->fixationColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
		if (bRenderStimuli)
			imgPainter.drawPoint(0.0,-qrYOffset);//-qOffset);//0.0f,-qrYOffset);
		else
			imgPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{				
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
			if (bRenderStimuli)
				activationPainter.drawPoint(0.0,-qrYOffset);//(-qOffset);//0.0f,-qrYOffset);
			else
				activationPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		}
	}
	return true;
}

bool RetinotopyMapperWindow::drawFixation()
{
	if(parentRetinotopyMapper->showFixationPoint) // show fix cross
	{
		imgPainter.setPen(QPen(parentRetinotopyMapper->fixationColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
		imgPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));			
			activationPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		}
	}
	return true;
}

bool RetinotopyMapperWindow::drawCustom()
{
	if(parentRetinotopyMapper->showFixationPoint) // show fix cross
	{
		imgPainter.setPen(QPen(parentRetinotopyMapper->fixationColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));
		imgPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		if(parentRetinotopyMapper->bCreateActivationMap)
		{
			activationPainter.setPen(QPen(parentRetinotopyMapper->whiteColor, parentRetinotopyMapper->fixationSize, parentRetinotopyMapper->style, parentRetinotopyMapper->roundCap));			
			activationPainter.drawPoint(nStimFrameWidth/2, nStimFrameHeight/2);
		}
	}
	return true;
}

QImage RetinotopyMapperWindow::fractalFillCheckeredImage(float fWidth, float fHeigth, float fSize, int nflickr)
{
	int nWidth = (int)(fWidth + 0.5f);
	int nHeigth = (int)(fHeigth + 0.5f);
	int nSize = (int)(fSize + 1.0f);
	QImage image(nWidth, nHeigth, QImage::Format_RGB16);
	QImage base((int)((fSize*2)+0.5f), (int)((fSize*2)+0.5f), QImage::Format_RGB16);
	QPainter p1(&base); 
	if (parentRetinotopyMapper->antiAliasing)
		p1.setRenderHint(QPainter::Antialiasing);
	p1.setCompositionMode(QPainter::CompositionMode_Source);
	if (nflickr == 1)
	{
		p1.fillRect(0, 0, nSize, nSize, parentRetinotopyMapper->cCheckerColor1);// Qt::gray);
		p1.fillRect(nSize, 0, nSize, nSize, parentRetinotopyMapper->cCheckerColor2);// Qt::white);
		p1.fillRect(0, nSize, nSize, nSize, parentRetinotopyMapper->cCheckerColor2);//Qt::white);
		p1.fillRect(nSize, nSize, nSize, nSize, parentRetinotopyMapper->cCheckerColor1);//Qt::gray);
	}
	else
	{
		p1.fillRect(0, 0, nSize, nSize, parentRetinotopyMapper->cCheckerColor2);// Qt::gray);
		p1.fillRect(nSize, 0, nSize, nSize, parentRetinotopyMapper->cCheckerColor1);// Qt::white);
		p1.fillRect(0, nSize, nSize, nSize, parentRetinotopyMapper->cCheckerColor1);//Qt::white);
		p1.fillRect(nSize, nSize, nSize, nSize, parentRetinotopyMapper->cCheckerColor2);//Qt::gray);
	}
	QPainter p(&image);
	if (parentRetinotopyMapper->antiAliasing)
		p.setRenderHint(QPainter::Antialiasing);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.drawImage(0, 0, base);
	const int imageW = image.width();
	const int imageH = image.height();
	int w = base.width();
	int h = base.height();
	while (w < imageW || h < imageH) {
		if (w < imageW) {
			p.drawImage(QRect(w, 0, w, h), image, QRect(0, 0, w, h));
			w *= 2;
		}
		if (h < imageH) {
			p.drawImage(QRect(0, h, w, h), image, QRect(0, 0, w, h));
			h *= 2;
		}
	}
	return image;
}