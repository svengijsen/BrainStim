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


#include "RetinotopyMapper.h"

RetinotopyMapper::RetinotopyMapper(QObject *parent)	: ExperimentEngine(parent)
{
	retinoMapperWindow = NULL;
	customScriptHandlerFunction = NULL;
	initialize();	
}

bool RetinotopyMapper::eventFilter(QObject *target, QEvent *event)
{
	if (target == retinoMapperWindow)
	{
		QEvent::Type tmpEvent = event->type();
		if (tmpEvent == QEvent::KeyPress) 
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

			switch (keyEvent->key())
			{
			case Qt::Key_A://Key_Escape:	//To abort the experiment
				if((keyEvent->modifiers() & Qt::ControlModifier))// && (keyEvent->modifiers() & Qt::AltModifier))
					if(experimentManager)
						experimentManager->abortExperiment();
				break;
			case Qt::Key_T:
				if((keyEvent->modifiers() & Qt::ControlModifier))// && (keyEvent->modifiers() & Qt::AltModifier))
				{
					ExperimentEngine::incrementExternalTrigger();
				}	
				break;
			case Qt::Key_Alt:	//To start the experiment
				setExperimentObjectReadyToUnlock();
				break;
			}
		}
		else if(tmpEvent == QEvent::Close)
		{
			if(experimentManager)
				experimentManager->abortExperiment();
		}
	}
	return false;
}

bool RetinotopyMapper::setExperimentObjectReadyToUnlock()
{
	bool bResult = ExperimentEngine::setExperimentObjectReadyToUnlock();
	return bResult;
}

QScriptValue RetinotopyMapper::ctor__RetinotopyMapper(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	return engine->newQObject(new RetinotopyMapper(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
}

RetinotopyMapper::~RetinotopyMapper()
{
	if (customScriptHandlerFunction)
	{
		delete customScriptHandlerFunction;
		customScriptHandlerFunction = NULL;
	}
	if (randStimStateGenerator)
	{
		delete randStimStateGenerator;
		randStimStateGenerator = NULL;
	}
	if (randEmptyStimGenerator)
	{
		delete randEmptyStimGenerator;
		randEmptyStimGenerator = NULL;
	}
	if (previousRandEmptyStimGenerator)
	{
		delete previousRandEmptyStimGenerator;
		previousRandEmptyStimGenerator = NULL;
	}
	if(retinoMapperWindow)
	{
		delete retinoMapperWindow;
		retinoMapperWindow = NULL;
	}
}

bool RetinotopyMapper::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		QScriptEngine *currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		setScriptEngine(currentScriptEngine);
		return true;
	}
	return false;
}

void RetinotopyMapper::initialize()
{
	//Place your custom initialization code here:
	experimentManager = NULL;
	bNoChangesSinceLastFrame = false;
	//getObjectID() = -1;
	sActiveStimScreen = NULL;
	rectScreenRes = QGuiApplication::primaryScreen()->geometry();	
	firstBlockTrialPaintFrame = false;	
	//lastTriggerNumber = -1;
	flickrSwitch = 0;
	textContent = "";
	textContent = "";
	randStimStateGenerator = NULL;
	randEmptyStimGenerator = NULL;
	previousRandEmptyStimGenerator = NULL;
	textPen = QPen(Qt::white);
	textFont.setPixelSize(20);
	whiteColor = QColor(255,255,255);//For the activity maps
	blackColor = QColor(0,0,0);//For the activity maps
	style = Qt::SolidLine;
	flatCap = Qt::FlatCap;
	roundCap = Qt::RoundCap;
	nextNewBlockEntered = false;
	nextNewCycleEntered = false;
	fCalculatedCortMagFacTimingConst = 1.0f;
}

void RetinotopyMapper::parseExperimentObjectBlockParameters(bool bInit)
{	
	int tmpInteger = -1;
	if (bInit)
	{	
		QString tmpString = "";
		nRefreshRate = 0;
		insertExpObjectBlockParameter(getObjectID(),RETINOMAPPER_STIMULI_REFRESHRATE,QString::number(nRefreshRate));
		polarWedgeSpan = 22.5;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_POLAR_WEDGE_SPAN,polarWedgeSpan);
		//insertExpObjectBlockParameter(getObjectID(),RETINOMAPPER_POLAR_WEDGE_SPAN,QString::number(polarWedgeSpan));
		//float *tmp = getExpObjectVariabelePointer<float>(getObjectID(),RETINOMAPPER_POLAR_WEDGE_SPAN);
		//*tmp = 11.7;
		polarWedgeNrChecks = 4;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_POLAR_CHECK_AMOUNT,polarWedgeNrChecks);
		polarWedgeNrRings = 20;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_POLAR_RING_AMOUNT,polarWedgeNrRings);
		polarRotationDirection = 1;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_POLAR_ROTATION_DIRECTION,polarRotationDirection);
		eccentricityNrChecks = 20;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_CHECK_AMOUNT,eccentricityNrChecks);
		eccentricityNrRings = 4;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_RING_AMOUNT,eccentricityNrRings);
		disableCortMagFac = false;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_DISABLECORTMAGFAC,disableCortMagFac);
		enableCortMagTiming = false;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_ENABLE_CORTMAG_TIMING,enableCortMagTiming);
		eccentricityDirection = 1;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_DIRECTION,eccentricityDirection);
		movingBarAngle = 45.0;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_ANGLE,movingBarAngle);
		movingBarCoverage = 0.5;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_COVERAGE,movingBarCoverage);
		movingBarHeight = 10.0;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_HEIGHT,movingBarHeight);
		//movingBarIncludeOppositeDirection = false;
		//insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_INCOPPDIR,movingBarIncludeOppositeDirection);
		movingBarHeightCheckAmount = 4;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_HEIGTH_CHECK_AMOUNT,movingBarHeightCheckAmount);
		movingBarDirection = 1;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_DIRECTION,movingBarDirection);
		movingDotsColor = QColor(255,255,255);
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_COLOR,movingDotsColor);
		currentExpType = RETINOMAPPER_PATTERN_POLARANGLE;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_PATTERN,currentExpType);
		colorBackground = QColor(QColor(87,87,87));//gives "#575757";
		brushBackground = QBrush(colorBackground);
		insertExpObjectParameter(getObjectID(),GLWIDGET_BACKGROUNDCOLOR,colorBackground);
		fixationColor = QColor(255, 0, 0);
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_FIXATIONCOLOR,fixationColor);
		cCheckerColor1 = QColor(255, 255, 255);
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_CHECKERCOLOR1,cCheckerColor1);
		cCheckerColor2 = QColor(0, 0, 0);
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_CHECKERCOLOR2,cCheckerColor2);
		showFixationPoint = true;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_SHOWFIXPOINT,showFixationPoint);
		fixationSize = 8;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_FIXSIZE,fixationSize);
		stimHeightPixelAmount = 480;
		insertExpObjectParameter(getObjectID(),GLWIDGET_HEIGHT_PIXEL_AMOUNT,stimHeightPixelAmount);
		stimWidthPixelAmount = stimHeightPixelAmount;
		insertExpObjectParameter(getObjectID(),GLWIDGET_WIDTH_PIXEL_AMOUNT,stimWidthPixelAmount);
		adjustStimScreenArea();
		cycleTriggerAmount = 1;
		if(experimentManager)
		{
			//cExperimentStructure tmpExpStructImplicitCopy = cExperimentStructure(*experimentManager->getExperimentStructure());//Creates an implicit shared safe copy
			bool bHasCurrentBlock = false;
			cBlockStructure tmpBlock = experimentManager->getExperimentStructure()->getCurrentBlock(bHasCurrentBlock);
			//cBlockStructure tmpBlock = tmpExpStructImplicitCopy.getCurrentBlock(bHasCurrentBlock);
			//tmpExpStructImplicitCopy.setExperimentName("Safe Copy");
			//QString tmpString = tmpExpStructImplicitCopy.getExperimentName();
			//tmpString = tmpBlock.getBlockName();
			//tmpBlock.setBlockName("ff");
			//tmpString = tmpBlock.getBlockName();
			if(bHasCurrentBlock)
			{
				tmpInteger = tmpBlock.getNumberOfInternalTriggers();
				if(tmpInteger > 0)
					cycleTriggerAmount = tmpInteger;
			}
		}
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_CYCLE_TRIGGER_AMOUNT,cycleTriggerAmount);
		flickrSpeedFreq = 7.5;//5 times per second
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_FLICKRSPEED_HZ,flickrSpeedFreq);
		flickrThreshold = 1000 / flickrSpeedFreq;//First at 200 mSecs
		cortMagFactor = 0.2f;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_CORTMAG_FACTOR,cortMagFactor);
		triggerDurationMsec = 2000;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_TRIGGERDURATION_MSEC,triggerDurationMsec);
		emptyTriggerSteps = 0;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_EMPTYTRIGGERSTEPS,emptyTriggerSteps);
		emptyTriggerStepsArray.clear();
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_EMPTYTRIGGERSTEPS_ARRAY,emptyTriggerStepsArray);
		randomizeTriggerStepsArray.clear();
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_RANDOMIZETRIGGERSTEPS_ARRAY,randomizeTriggerStepsArray);
		gapDiameter = 20.0;//aperture in the middle left blank
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_GAP_DIAMETER,gapDiameter);
		outputTriggerFrame = false;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_OUTPUTTRIGGERFRAME,outputTriggerFrame);
		discreteTriggerSteps = false;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_DISCRETETRIGGERSTEPS,discreteTriggerSteps);
		randomizeTriggerSteps = false;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_RANDOMIZETRIGGERSTEPS,randomizeTriggerSteps);
		antiAliasing = true;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_ANTIALIASING,antiAliasing);
		retinoOutputType = RETINOMAPPER_OUTPUTTYPE_FRAME;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_OUTPUTFRAMETYPE,retinoOutputType);
		retinoOutputFormat = RETINOMAPPER_OUTPUTFORMAT_DAT;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_OUTPUTFRAMEFORMAT,retinoOutputFormat);
		movingDotsMaxMoveSpeed = 4;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MAX_MOVESPEED,movingDotsMaxMoveSpeed);
		movingDotsMinMoveSpeed = 4;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MIN_MOVESPEED,movingDotsMinMoveSpeed);
		movingDotsMinMoveAngle = 0;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MIN_MOVEANGLE,movingDotsMinMoveAngle);
		movingDotsMaxMoveAngle = 359;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MAX_MOVEANGLE,movingDotsMaxMoveAngle);
		movingDotsHemifieldPos = RETINOMAPPER_POS_BOTH;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELD,movingDotsHemifieldPos);
		movingDotsPixelFromCenter = 100;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_PIXELFROMCENTER,movingDotsPixelFromCenter);
		movingDotsIsStationary = false;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_STATIONAIRY,movingDotsIsStationary);
		movingDotsDotSize = 12;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_DOTSIZE,movingDotsDotSize);
		movingDotsNrOfDots = 1000;
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_NROFDOTS,movingDotsNrOfDots);
		////movingDotsRetPosition = 0;		
		////insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_RETINALPOSITION,movingDotsRetPosition);
		movingDotsHemiFieldWidth = stimWidthPixelAmount/2;		
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELDWIDTH,movingDotsHemiFieldWidth);
		movingDotsHemiFieldHeight = stimHeightPixelAmount;		
		insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELDHEIGHT,movingDotsHemiFieldHeight);
	} 
	else
	{
		ParsedParameterDefinition pParDef;

		if (randStimStateGenerator == NULL)
			randStimStateGenerator = new RandomGenerator();
		if (randEmptyStimGenerator == NULL)
			randEmptyStimGenerator = new RandomGenerator();
		if (previousRandEmptyStimGenerator == NULL)
			previousRandEmptyStimGenerator = new RandomGenerator();	

		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_STIMULI_REFRESHRATE,QString::number(nRefreshRate));
		nRefreshRate = pParDef.sValue.toInt();
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_PATTERN,currentExpType);
		currentExpType = pParDef.sValue.toLower();		
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_SHOWFIXPOINT,booleanToParamString(showFixationPoint));
		showFixationPoint = paramStringToBool(pParDef.sValue);
		stimHeightPixelAmount = getExpObjectBlockParameter(getObjectID(),GLWIDGET_HEIGHT_PIXEL_AMOUNT,QString::number(stimHeightPixelAmount)).sValue.toFloat();
		stimWidthPixelAmount = getExpObjectBlockParameter(getObjectID(),GLWIDGET_WIDTH_PIXEL_AMOUNT,QString::number(stimWidthPixelAmount)).sValue.toFloat();
		adjustStimScreenArea();
		fixationSize = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_FIXSIZE,QString::number(fixationSize)).sValue.toInt();
		
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_CYCLE_TRIGGER_AMOUNT,QString::number(cycleTriggerAmount));
		if (pParDef.bHasChanged)
		{
			cycleTriggerAmount = pParDef.sValue.toInt();
		} 
		else
		{
			if(experimentManager)//->getCurrExperimentState()
			{
				bool bHasCurrentBlock = false;
				cBlockStructure tmpBlock = experimentManager->getExperimentStructure()->getCurrentBlock(bHasCurrentBlock);
				if(bHasCurrentBlock)
					cycleTriggerAmount = tmpBlock.getNumberOfInternalTriggers();
			}
		}

		flickrSpeedFreq = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_FLICKRSPEED_HZ,QString::number(flickrSpeedFreq)).sValue.toFloat();
		flickrThreshold = 1000 / flickrSpeedFreq;
		cortMagFactor = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_CORTMAG_FACTOR,QString::number(cortMagFactor)).sValue.toFloat();
		triggerDurationMsec = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_TRIGGERDURATION_MSEC,QString::number(triggerDurationMsec)).sValue.toInt();
		emptyTriggerSteps = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_EMPTYTRIGGERSTEPS,QString::number(emptyTriggerSteps)).sValue.toInt();		
		emptyTriggerStepsArray = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_EMPTYTRIGGERSTEPS_ARRAY,emptyTriggerStepsArray.join(",")).sValue.split(",",QString::SkipEmptyParts);
		randomizeTriggerStepsArray = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_RANDOMIZETRIGGERSTEPS_ARRAY,randomizeTriggerStepsArray.join(",")).sValue.split(",",QString::SkipEmptyParts);
		gapDiameter = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_GAP_DIAMETER,QString::number(gapDiameter)).sValue.toInt();
		colorBackground = QColor(getExpObjectBlockParameter(getObjectID(),GLWIDGET_BACKGROUNDCOLOR,colorBackground.name()).sValue.toLower());
		brushBackground = QBrush(colorBackground);
		fixationColor = QColor(getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_FIXATIONCOLOR,fixationColor.name()).sValue.toLower());
		cCheckerColor1 = QColor(getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_CHECKERCOLOR1,cCheckerColor1.name()).sValue.toLower());
		cCheckerColor2 = QColor(getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_CHECKERCOLOR2,cCheckerColor2.name()).sValue.toLower());		
				
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_OUTPUTTRIGGERFRAME,booleanToParamString(outputTriggerFrame));
		outputTriggerFrame = paramStringToBool(pParDef.sValue);
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_DISCRETETRIGGERSTEPS,booleanToParamString(discreteTriggerSteps));
		discreteTriggerSteps = paramStringToBool(pParDef.sValue);
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_RANDOMIZETRIGGERSTEPS,booleanToParamString(randomizeTriggerSteps));
		randomizeTriggerSteps = paramStringToBool(pParDef.sValue);
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_ANTIALIASING,booleanToParamString(antiAliasing));
		antiAliasing = paramStringToBool(pParDef.sValue);
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_OUTPUTFRAMETYPE,retinoOutputType);
		retinoOutputType = pParDef.sValue.toLower();
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_OUTPUTFRAMEFORMAT,retinoOutputFormat);
		retinoOutputFormat = pParDef.sValue.toLower();
		pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_DISABLECORTMAGFAC,booleanToParamString(disableCortMagFac));
		disableCortMagFac = paramStringToBool(pParDef.sValue);
		
		//Specific ExperimentType Parameters
		if(currentExpType == RETINOMAPPER_PATTERN_POLARANGLE)
		{
			polarWedgeSpan = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_POLAR_WEDGE_SPAN,QString::number(polarWedgeSpan)).sValue.toFloat();
			polarWedgeNrChecks = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_POLAR_CHECK_AMOUNT,QString::number(polarWedgeNrChecks)).sValue.toInt();
			polarWedgeNrRings = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_POLAR_RING_AMOUNT,QString::number(polarWedgeNrRings)).sValue.toInt();
			polarRotationDirection = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_POLAR_ROTATION_DIRECTION,QString::number(polarRotationDirection)).sValue.toInt();
		}
		else if(currentExpType == RETINOMAPPER_PATTERN_ECCENTRICITY)
		{
			eccentricityNrChecks = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_CHECK_AMOUNT,QString::number(eccentricityNrChecks)).sValue.toInt();
			eccentricityNrRings = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_RING_AMOUNT,QString::number(eccentricityNrRings)).sValue.toInt();
			eccentricityDirection = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_DIRECTION,QString::number(eccentricityDirection)).sValue.toInt();
			pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_ECCENTRICITY_ENABLE_CORTMAG_TIMING,booleanToParamString(enableCortMagTiming));
			enableCortMagTiming = paramStringToBool(pParDef.sValue);
		}
		else if(currentExpType == RETINOMAPPER_PATTERN_MOVINGBAR)
		{
			movingBarAngle = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_ANGLE,QString::number(movingBarAngle)).sValue.toFloat();
			movingBarCoverage = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_COVERAGE,QString::number(movingBarCoverage)).sValue.toFloat();
			movingBarHeight = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_HEIGHT,QString::number(movingBarHeight)).sValue.toFloat();
			movingBarHeightCheckAmount = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_HEIGTH_CHECK_AMOUNT,QString::number(movingBarHeightCheckAmount)).sValue.toInt();
			movingBarDirection = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGBAR_DIRECTION,QString::number(movingBarDirection)).sValue.toInt();
		}
		else if(currentExpType == RETINOMAPPER_PATTERN_MOVINGDOTS)
		{
			movingDotsColor = QColor(getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_COLOR,movingDotsColor.name()).sValue.toLower());
			movingDotsMinMoveAngle = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MIN_MOVEANGLE,QString::number(movingDotsMinMoveAngle)).sValue.toFloat();
			movingDotsMaxMoveAngle = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MAX_MOVEANGLE,QString::number(movingDotsMaxMoveAngle)).sValue.toFloat();
			movingDotsMaxMoveSpeed = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MAX_MOVESPEED,QString::number(movingDotsMaxMoveSpeed)).sValue.toFloat();
			movingDotsMinMoveSpeed = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_MIN_MOVESPEED,QString::number(movingDotsMinMoveSpeed)).sValue.toFloat();
			pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELD,RETINOMAPPER_POS_BOTH);
			movingDotsHemifieldPos = pParDef.sValue.toLower();
			movingDotsPixelFromCenter = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_PIXELFROMCENTER,QString::number(movingDotsPixelFromCenter)).sValue.toInt();
			pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_STATIONAIRY,booleanToParamString(movingDotsIsStationary));
			movingDotsIsStationary = paramStringToBool(pParDef.sValue);
			movingDotsDotSize = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_DOTSIZE,QString::number(movingDotsDotSize)).sValue.toInt();
			movingDotsNrOfDots = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_NROFDOTS,QString::number(movingDotsNrOfDots)).sValue.toInt();
			//movingDotsRetPosition = getExperimentObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_RETINALPOSITION,QString::number(movingDotsRetPosition)).toInt();
			
			//We have initialized the parameter(movingDotsHemiFieldWidth) the first time(before the experiment actually started), but because the default value is dependent of another 
			//parameter(stimWidthPixelAmount) that might have changed again we might have to reinitialize this parameter again. But this applies only when we fetch the 
			//parameter for the first time and that the parameter is also not re-defined by the user. When we reinitialize the parameter here its important to set the bIsInitializing
			//from the function insertExpObjectParameter() to false!
			pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELDWIDTH,QString::number(stimWidthPixelAmount/2));
			if ((pParDef.bIsInitialized) && (pParDef.bHasChanged == false))
			{
				int tmpBuffer = movingDotsHemiFieldWidth;
				movingDotsHemiFieldWidth = QString::number(stimWidthPixelAmount/2).toInt();
				if(insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELDWIDTH,movingDotsHemiFieldWidth,false))
					movingDotsHemiFieldWidth = QString::number(stimWidthPixelAmount/2).toInt();
				movingDotsHemiFieldWidth = tmpBuffer;
			}
			else
			{
				movingDotsHemiFieldWidth = pParDef.sValue.toInt();
			}
			//Same counts for the next parameter
			pParDef = getExpObjectBlockParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELDHEIGHT,QString::number(stimHeightPixelAmount));
			if ((pParDef.bIsInitialized) && (pParDef.bHasChanged == false))
			{
				int tmpBuffer = movingDotsHemiFieldHeight;
				movingDotsHemiFieldHeight = QString::number(stimHeightPixelAmount).toInt();
				if(insertExpObjectParameter(getObjectID(),RETINOMAPPER_MOVINGDOTS_HEMIFIELDHEIGHT,movingDotsHemiFieldHeight,false))
					movingDotsHemiFieldHeight = QString::number(stimHeightPixelAmount).toInt();
				else
					movingDotsHemiFieldHeight = tmpBuffer;
			}
			else
			{
				movingDotsHemiFieldHeight = pParDef.sValue.toInt();
			}
			initializeMovingDotsStructures();
		}
		else if(currentExpType == RETINOMAPPER_PATTERN_FIXATION)
		{

		}
		else if(currentExpType == RETINOMAPPER_PATTERN_CUSTOM)
		{

		}
		else
		{
			qDebug() << __FUNCTION__ << "::Unknown RetinoMapping type!";
		}
	}
	bNoChangesSinceLastFrame = false;
}

void RetinotopyMapper::adjustStimScreenArea()
{
	rStimuliScreenArea = QRect((int)(rectScreenRes.width()-stimWidthPixelAmount)/2,(int)(rectScreenRes.height()-stimHeightPixelAmount)/2,stimWidthPixelAmount,stimHeightPixelAmount);
}

bool RetinotopyMapper::startObject()
{
	//lastTriggerNumber = -1;
	//emit LogToOutputWindow("Started");	
	if(retinoMapperWindow == NULL)
	{
		retinoMapperWindow = new RetinotopyMapperWindow(this);
		retinoMapperWindow->installEventFilter(this);//re-route all retinoMapperWindow events to this->bool RetinotopyMapper::eventFilter(QObject *target, QEvent *event)
	}
	QSurfaceFormat format;
	//int a = format.samples();
	//format.setSamples(4);
	//format.setRenderableType(QSurface::OpenGLSurface);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	//format.setAlphaBufferSize(8);
	//qDebug() << format.hasAlpha();
	retinoMapperWindow->setFormat(format);
	retinoMapperWindow->setSurfaceType(QSurface::OpenGLSurface);	
	retinoMapperWindow->setAnimating(true);	
	if(sActiveStimScreen == NULL)
	{
		sActiveStimScreen = retinoMapperWindow->grabScreenUnderMouseCursor();
		if(sActiveStimScreen)
		{
			rectScreenRes = sActiveStimScreen->geometry();
			adjustStimScreenArea();
			retinoMapperWindow->setScreen(sActiveStimScreen);
		}
	}
	else
	{
		retinoMapperWindow->setScreen(sActiveStimScreen);
	}	
	if((stimWidthPixelAmount > sActiveStimScreen->size().width()) || (stimHeightPixelAmount > sActiveStimScreen->size().height()))
	{
		qWarning() << __FUNCTION__ << "Choosen Stimuli size exceeds active Screen resolution!"; 
		emit ExperimentEngine::UserWantsToClose();
	}

	retinoMapperWindow->showFullScreen();
	QRect tmpRect = retinoMapperWindow->geometry();
	tmpRect.setHeight(tmpRect.height()+1);//To prevent flickering, (bug, on some systems), doesn't make a change since drawing starts upper left corner.
	retinoMapperWindow->setGeometry(tmpRect);
	//retinoMapperWindow->setFlags(Qt::WindowStaysOnTopHint);//This gives problems...
	return true;
}

bool RetinotopyMapper::initObject()
{
	//1234 experimentManager->logExperimentObjectData(getObjectID(),0,__FUNCTION__,"","swapInterval() = ", QString::number(this->format().swapInterval()));
	dAdditionalRefreshDelayTime = 0.0;
	parseExperimentObjectBlockParameters(true);
	return true;
}

bool RetinotopyMapper::initObjectBlockTrial()
{
	parseExperimentObjectBlockParameters(false);
	//Some variable initializations
	flickrSwitch = 0;
	StimulusResultImageFrame = QPixmap(stimWidthPixelAmount,stimHeightPixelAmount);
	StimulusResultImageFrame.fill();
	StimulusActivationMap = StimulusResultImageFrame;

	firstBlockTrialPaintFrame = true;	
	currExpBlockTrialCycle = 0;
	emptyTriggerStepCount = 0;
	emptyTriggerLastIndex = -1;
	bAllTrialEmptyProcessed = false;
	return true;
}

QString RetinotopyMapper::booleanToParamString(bool bValue)
{ 
	if(bValue) 
		return TYPE_BOOL_TRUE;
	else
		return TYPE_BOOL_FALSE;
}

bool RetinotopyMapper::paramStringToBool(QString sValue)
{
	if(sValue.toLower() == TYPE_BOOL_TRUE) 
		return true;
	else if(sValue.toLower() == TYPE_BOOL_FALSE)
		return false;
	qDebug() << __FUNCTION__ << "::Could not parse param String to boolean(" << sValue << ")!";
	return false;
}

void RetinotopyMapper::initializeMovingDotsStructures()
{
	if (movingDotsHemifieldPos == RETINOMAPPER_POS_RIGHT)
	{
		movingDotsFirstHemiVisibleArea.setP1(QPointF((stimWidthPixelAmount/2)+movingDotsPixelFromCenter, (stimHeightPixelAmount-movingDotsHemiFieldHeight)/2));
		movingDotsFirstHemiVisibleArea.setP2(QPointF(stimWidthPixelAmount - ((stimWidthPixelAmount/2)-movingDotsHemiFieldWidth), stimHeightPixelAmount-(movingDotsPixelFromCenter, (stimHeightPixelAmount-movingDotsHemiFieldHeight)/2)));
	}
	else//(RETINOMAPPER_POS_LEFT || RETINOMAPPER_POS_BOTH)
	{
		movingDotsFirstHemiVisibleArea.setP1(QPointF(((stimWidthPixelAmount/2)-movingDotsHemiFieldWidth), (stimHeightPixelAmount-movingDotsHemiFieldHeight)/2));
		movingDotsFirstHemiVisibleArea.setP2(QPointF((stimWidthPixelAmount/2)-movingDotsPixelFromCenter, stimHeightPixelAmount-movingDotsFirstHemiVisibleArea.p1().y()));
	}
	QPointF tmpPoint;
	QLineF tmpLine;
	movingDots.Pos.clear();
	movingDots.Mov.clear();
	//movingDots.MirrorXPos.clear();
	movingDots.OldPos.clear();
	for (int i=0; i<movingDotsNrOfDots; i++)
	{
		tmpPoint.setX(randStimStateGenerator->randomizeInt(movingDotsFirstHemiVisibleArea.p1().x(),movingDotsFirstHemiVisibleArea.p2().x()));
		tmpPoint.setY(randStimStateGenerator->randomizeInt(movingDotsFirstHemiVisibleArea.p1().y(),movingDotsFirstHemiVisibleArea.p2().y()));
		movingDots.OldPos.append(tmpPoint);//The initial position will be copied from here
		movingDots.Pos.append(QPointF(0,0));
		tmpLine.setP1(QPointF(0,0));//Point1 of Movement Vector
		tmpLine.setP2(QPointF(1,0));//The base speed
		tmpLine.setAngle(randStimStateGenerator->randomizeInt(movingDotsMinMoveAngle,movingDotsMaxMoveAngle));//The randomized movement direction
		tmpLine.setLength(randStimStateGenerator->randomizeDouble(movingDotsMinMoveSpeed,movingDotsMaxMoveSpeed));//The randomized speed of the movement  
		movingDots.Mov.append(tmpLine);
		//movingDots.MirrorXPos.append(0.0f);//The Mirrored X Position
	}
}

bool RetinotopyMapper::installCustomScriptHandlerFunction(QString FuncName)
{
/*! \brief Install a custom function that is defined inside the script as an alternative painter.
 *
 *  This slot can be used for custom painting in the script.
 *	The String parameter(FuncName) is the name of the script function which is automatically called when a repaint is requested.
 *  The custom paint function then automatically receives an QPainter pointer that can be further used for custom painting purpose.
 */
	if (getScriptEngine())
	{
		customScriptHandlerFunction = new QScriptValue(getScriptEngine()->globalObject().property(FuncName));
		if(customScriptHandlerFunction->isFunction())
		{
			return true;
		}
		else
		{
			if(customScriptHandlerFunction)
			{
				delete customScriptHandlerFunction;
				customScriptHandlerFunction = NULL;
			}
		}
	}
	return false;
}

bool RetinotopyMapper::setExperimentManager(ExperimentManager *expManager)
{
	if(experimentManager!=expManager)
	{
		experimentManager = expManager;
		if(expManager)
		{
			sActiveStimScreen = expManager->getActiveStimuliOutputScreen();
			if(sActiveStimScreen)
				rectScreenRes = sActiveStimScreen->geometry();
		}
		ExperimentEngine::setExperimentManager(expManager);//Important!
	}
	return true;
}

