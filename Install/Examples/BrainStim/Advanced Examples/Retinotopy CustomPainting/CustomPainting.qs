//Includes:
Include("QtScript/BasicRetinotopyExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/

//Included variables:
BasicExperiment.sExmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "CustomPainting.exml";
BasicExperiment.nTestModeTriggerDuration = 1000;

////////////////////////////////////////////////////SETTINGS BEGIN////////////////////////////////////////////////////
////////////////////////////////////////////////////BrainStim Script referenced Variabeles///////////////////////////////////
var colBackGround = "#E0E0E0";
var colFixation = "#FF0001"//Almost red, we shouldn't make this the same as a plaids color, because of the masking?
var nFixationSize = 12;
var bShowFixPoint = false;//We'll do this ourself
var nStimuliRefreshRate = 0; // 0; // 30 // I get smooth animation when setting this value very high (?)
var nStimuliWidth = 800;
var nStimuliHeight = 800;
//////////////////////////////////////////////////////////////Script Variabeles////////////////////////////////////////////
var colorStripe1 = new QColor("#505050"); //("#c0c0c0");
var colorStripe2 = new QColor("#b0b0b0");//new QColor("#808080");
var colorStripeUnion = new QColor("#9c9c9c"); // ("#808080");//new QColor(Qt.black);
var colorPlaidBackground = new QColor(128, 128, 128, 255);
var nStripeWidth = 35;
var nStripeDistance = 125;
var nStripeAngle = 65.0; // 57;
var nPlaidsViewportWidth = 500; // 600;
var nPlaidsViewportHeight = 500; // 400;
var nCycleTimeInMSec = 1200; // 2500;
var nStripeDirection = 1;
var bEnableStripe1 = true;
var bEnableStripe2 = true;
var bEnableUnion = true;
var bStaticPlaids = true; //Setting this to true makes the Stimuli Static

///////////////////////////////////////////////////////////Experiment Runtime Variabeles////////////////////////////////////////
// We set here the order of trials (or mini-blocks) by setting which of the prepared plaids to use and whether moving or static
// note that number of trials defined here must match number of trials in .exml file
// the plaids defined: 0 -> pattern-1 (white intersections), 1 -> component, 2 -> pattern-2 (black intersections), 3 -> grating right, 4 grating left
var usePlaidForTrialOrder1Array = [];
var isStaticPlaidOrder1Array = [];
var upOrDownDirOrder1Array = [];
usePlaidForTrialOrder1Array[0] = 1;  isStaticPlaidOrder1Array[0] = true;	  upOrDownDirOrder1Array[0] = 1;  // pattern static
usePlaidForTrialOrder1Array[1] = 1;  isStaticPlaidOrder1Array[1] = false;  upOrDownDirOrder1Array[1] = 1; // pattern
usePlaidForTrialOrder1Array[2] = 0;  isStaticPlaidOrder1Array[2] = false;  upOrDownDirOrder1Array[2] = 1; // component
usePlaidForTrialOrder1Array[3] = 1;  isStaticPlaidOrder1Array[3] = false;  upOrDownDirOrder1Array[3] = -1;
usePlaidForTrialOrder1Array[4] = 0;  isStaticPlaidOrder1Array[4] = false;  upOrDownDirOrder1Array[4] = -1;
usePlaidForTrialOrder1Array[5] = 1;  isStaticPlaidOrder1Array[5] = false;   upOrDownDirOrder1Array[5] = 1;
usePlaidForTrialOrder1Array[6] = 0;  isStaticPlaidOrder1Array[6] = false;  upOrDownDirOrder1Array[6] = 1;
usePlaidForTrialOrder1Array[7] = 1;  isStaticPlaidOrder1Array[7] = false;   upOrDownDirOrder1Array[7] = 1;
usePlaidForTrialOrder1Array[8] = 0;  isStaticPlaidOrder1Array[8] = false;  upOrDownDirOrder1Array[8] = 1;
usePlaidForTrialOrder1Array[9] = 1;  isStaticPlaidOrder1Array[9] = false;   upOrDownDirOrder1Array[9] = -1;
usePlaidForTrialOrder1Array[10] = 0;  isStaticPlaidOrder1Array[10] = false;  upOrDownDirOrder1Array[10] = -1;
usePlaidForTrialOrder1Array[11] = 1;  isStaticPlaidOrder1Array[11] = false;   upOrDownDirOrder1Array[11] = -1;
usePlaidForTrialOrder1Array[12] = 0;  isStaticPlaidOrder1Array[12] = false;  upOrDownDirOrder1Array[12] = -1;
usePlaidForTrialOrder1Array[13] = 1;  isStaticPlaidOrder1Array[13] = false;   upOrDownDirOrder1Array[13] = 1;
usePlaidForTrialOrder1Array[14] = 0;  isStaticPlaidOrder1Array[14] = false;  upOrDownDirOrder1Array[14] = 1;
usePlaidForTrialOrder1Array[15] = 1;  isStaticPlaidOrder1Array[15] = false;   upOrDownDirOrder1Array[15] = -1;
usePlaidForTrialOrder1Array[16] = 0;  isStaticPlaidOrder1Array[16] = false;  upOrDownDirOrder1Array[16] = -1;
usePlaidForTrialOrder1Array[17] = 0;  isStaticPlaidOrder1Array[17] = true;   upOrDownDirOrder1Array[17] = -1;

var usePlaidForTrialOrder2Array = [];
var isStaticPlaidOrder2Array = [];
var upOrDownDirOrder2Array = [];
usePlaidForTrialOrder2Array[0] = 1;  isStaticPlaidOrder2Array[0] = true;	  upOrDownDirOrder2Array[0] = -1;  // pattern static
usePlaidForTrialOrder2Array[1] = 1;  isStaticPlaidOrder2Array[1] = false;  upOrDownDirOrder2Array[1] = -1; // pattern
usePlaidForTrialOrder2Array[2] = 0;  isStaticPlaidOrder2Array[2] = false;  upOrDownDirOrder2Array[2] = -1; // component
usePlaidForTrialOrder2Array[3] = 1;  isStaticPlaidOrder2Array[3] = false;  upOrDownDirOrder2Array[3] = -1;
usePlaidForTrialOrder2Array[4] = 0;  isStaticPlaidOrder2Array[4] = false;  upOrDownDirOrder2Array[4] = -1;
usePlaidForTrialOrder2Array[5] = 1;  isStaticPlaidOrder2Array[5] = false;   upOrDownDirOrder2Array[5] = 1;
usePlaidForTrialOrder2Array[6] = 0;  isStaticPlaidOrder2Array[6] = false;  upOrDownDirOrder2Array[6] = 1;
usePlaidForTrialOrder2Array[7] = 1;  isStaticPlaidOrder2Array[7] = false;   upOrDownDirOrder2Array[7] = -1;
usePlaidForTrialOrder2Array[8] = 0;  isStaticPlaidOrder2Array[8] = false;  upOrDownDirOrder2Array[8] = -1;
usePlaidForTrialOrder2Array[9] = 1;  isStaticPlaidOrder2Array[9] = false;   upOrDownDirOrder2Array[9] = 1;
usePlaidForTrialOrder2Array[10] = 0;  isStaticPlaidOrder2Array[10] = false;  upOrDownDirOrder2Array[10] = 1;
usePlaidForTrialOrder2Array[11] = 1;  isStaticPlaidOrder2Array[11] = false;   upOrDownDirOrder2Array[11] = 1;
usePlaidForTrialOrder2Array[12] = 0;  isStaticPlaidOrder2Array[12] = false;  upOrDownDirOrder2Array[12] = 1;
usePlaidForTrialOrder2Array[13] = 1;  isStaticPlaidOrder2Array[13] = false;   upOrDownDirOrder2Array[13] = -1;
usePlaidForTrialOrder2Array[14] = 0;  isStaticPlaidOrder2Array[14] = false;  upOrDownDirOrder2Array[14] = -1;
usePlaidForTrialOrder2Array[15] = 1;  isStaticPlaidOrder2Array[15] = false;   upOrDownDirOrder2Array[15] = 1;
usePlaidForTrialOrder2Array[16] = 0;  isStaticPlaidOrder2Array[16] = false;  upOrDownDirOrder2Array[16] = 1;
usePlaidForTrialOrder2Array[17] = 0;  isStaticPlaidOrder2Array[17] = true;   upOrDownDirOrder2Array[17] = -1;

////////////////////////////////////////////////////Script Helper Variabeles//////////////////////////////////////////////
var painter = new QPainter();
var colorBackGround = new QColor(colBackGround);
var colorFixation = new QColor(colFixation);
var _currentBlockNumber = -1;
var _currentTrialNumber = -1;
var sCurrentScriptLocation = BrainStim.getActiveDocumentFileLocation();
var nScreenIndex = QApplication.desktop().screenNumber();
var nScreenSizeRect = QApplication.desktop().screenGeometry(nScreenIndex);
var nScreenWidth = nScreenSizeRect.width();
var nScreenHeight = nScreenSizeRect.height();
var nScreenMax = Math.max(nScreenWidth,nScreenHeight);
var nStimuliMax = Math.max(nStimuliWidth,nStimuliHeight);
var numberOfDifferentPlaides = 5;
var pixFinishedFinalPlaidsArray = new Array(0);
var nAdditionalAnimationPlaidsViewportHeight = nStripeDistance/(Math.sin((90.0-nStripeAngle)  * (Math.PI / 180.0)));
//Log("nAdditionalAnimationPlaidsViewportHeight: " + nAdditionalAnimationPlaidsViewportHeight);
var sizePlaidsMaskViewport = new QSize(nPlaidsViewportWidth, nPlaidsViewportHeight);
var sizePlaidsViewport = new QSize(nPlaidsViewportWidth, nPlaidsViewportHeight + nAdditionalAnimationPlaidsViewportHeight);
var imgPlaidsReferenceTotal = new QImage(sizePlaidsViewport, QImage.Format_ARGB32_Premultiplied);
var imgPlaidsReference1 = new QImage(sizePlaidsViewport, QImage.Format_ARGB32_Premultiplied);
var imgPlaidsReference2 = new QImage(sizePlaidsViewport, QImage.Format_ARGB32_Premultiplied);
var imgPlaidsReferenceUnion = new QImage(sizePlaidsViewport, QImage.Format_ARGB32_Premultiplied);
var imgViewPortMask = new QImage(sizePlaidsMaskViewport, QImage.Format_ARGB32_Premultiplied);
var lineStripeVector = new QLineF();
var lineStripeDistanceOffset = new QPointF();
var nStripeAngleMultiplier;
var bPlaidsImagesReady = false;
var nExtraWidthModifier;
var pointViewportOffset = new QPointF((-nPlaidsViewportWidth/2)+(nScreenWidth/2) , (-nPlaidsViewportHeight/2) + (nScreenHeight/2));
var rectViewportLocation = new QRectF(pointViewportOffset, new QSizeF(nPlaidsViewportWidth,nPlaidsViewportHeight));
var expTimer = new ExperimentTimer();
var startTime = 0;
var currentPastTime = 0;
var currentCycleTime = 0;
var bPaintResult;
var nYPixelTimeProgressTranslation = 0;
var bFirstCustomDraw = false;
var rectFCalculatedPresentationTargetArea = new QRectF();
var tmpBrush = new QBrush(colorPlaidBackground,Qt.SolidPattern);
var tmpPen = new QPen(Qt.NoPen);
var tmpMaskMode = new Qt.MaskMode(Qt.MaskOutColor);//Qt.MaskInColor
var ChoosenStimuliBuffer = 0; //1 or 2
////////////////////////////////////////////////////SETTINGS END///////////////////////////////////////////////////////////////

//Override Public Functions from the BasicRetinotopyExperiment.qs include (that includes BasicExperiment):
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	_currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	_currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	BrainStim.write2OutputWindow("*** Going to prepare a initialization of a new Block-trial:  BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber);	
	
	if((_currentBlockNumber==1)  || (_currentBlockNumber==5) || (_currentBlockNumber==9))
	{	
		ChoosenStimuliBuffer = usePlaidForTrialOrder1Array[_currentTrialNumber];
		bStaticPlaids = isStaticPlaidOrder1Array[_currentTrialNumber];
		nStripeDirection = upOrDownDirOrder1Array[_currentTrialNumber];
	}
	if((_currentBlockNumber==3) || (_currentBlockNumber==7) || (_currentBlockNumber==11)) 
	{	
		ChoosenStimuliBuffer = usePlaidForTrialOrder2Array[_currentTrialNumber];
		bStaticPlaids = isStaticPlaidOrder2Array[_currentTrialNumber];
		nStripeDirection = upOrDownDirOrder2Array[_currentTrialNumber];
	}
}

BasicExperiment.__proto__.ExperimentStateChanged = function(currentState, sDateTimeStamp)
{
	//This Funtion is called automatically at each time the Experiment State changes
	//See Help documentation (search for "ExperimentManager::ExperimentState") for a description of the different states
	
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Initialized at " + sDateTimeStamp);
		
		createPlaidsImagesArray();
		
		ChoosenStimuliBuffer = 0;
		bEnableStripe1 = true;
		bEnableStripe2 = true;
		bEnableUnion = true;
		colorStripeUnion = new QColor("#9c9c9c");
		bPlaidsImagesReady = createPlaidsImages();
		
		ChoosenStimuliBuffer = 1;
		bEnableStripe1 = true;
		bEnableStripe2 = true;
		bEnableUnion = true;
		colorStripeUnion = new QColor("#e0e0e0");
		bPlaidsImagesReady = bPlaidsImagesReady && createPlaidsImages();

		ChoosenStimuliBuffer = 2;
		bEnableStripe1 = true;
		bEnableStripe2 = true;
		bEnableUnion = true;
		colorStripeUnion = new QColor("#202020");
		bPlaidsImagesReady = bPlaidsImagesReady && createPlaidsImages();
		
		ChoosenStimuliBuffer = 3;
		bEnableStripe1 = true;
		bEnableStripe2 = false;
		bEnableUnion = false;
		colorStripeUnion = new QColor("#9c9c9c");
		bPlaidsImagesReady = bPlaidsImagesReady && createPlaidsImages();

		ChoosenStimuliBuffer = 4;
		bEnableStripe1 = false;
		bEnableStripe2 = true;
		bEnableUnion = false;
		colorStripeUnion = new QColor("#e0e0e0");
		bPlaidsImagesReady = bPlaidsImagesReady && createPlaidsImages();
		
		ChoosenStimuliBuffer = 0;
		
		if(BasicExperiment.RetinotopyMapperObj.installCustomScriptHandlerFunction("CustomScriptHandlerFunction"))
			Log("--- Custom Script Handler Function installed");
		else
			Log("--- Failed to install the Custom Script Handler Function");	
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Started at " + sDateTimeStamp);
	
		if(bPlaidsImagesReady == false)
		{
			Log("** An Error Occurred** PlaidsImages not ready, exiting!");
			BasicExperiment.ExperimentManagerObj.abortExperiment();	
		}	
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Stopped at " + sDateTimeStamp);
	}
}

//////////////////////////////////////////////////Paint Function definitions////////////////////////////////////////////////////////////////////////////////////////////
function CustomScriptHandlerFunction()//Our custom script handler funtion!!
{
	if((_currentBlockNumber % 2) == 1) // uneven blocks -> plaids, even -> fix
	{
		if(arguments[0] == "QPainter")
		{
			if(startTime == 0)//First call of this function?
			{
				bFirstCustomDraw = true;
				nYPixelTimeProgressTranslation = 0;
				startTime = expTimer.getElapsedTimeInMilliSec();
			}
			else
			{
				bFirstCustomDraw = false;
				if(bStaticPlaids)
				{
					nYPixelTimeProgressTranslation = 0;
				}
				else
				{					
					currentPastTime = expTimer.getElapsedTimeInMilliSec() - startTime;
					currentCycleTime = currentPastTime%nCycleTimeInMSec;
					nYPixelTimeProgressTranslation = (currentCycleTime/nCycleTimeInMSec) * nAdditionalAnimationPlaidsViewportHeight * nStripeDirection;
					if(nStripeDirection < 0)
						nYPixelTimeProgressTranslation += nAdditionalAnimationPlaidsViewportHeight; // RG: added to handle downward motion
					//Log(nYPixelTimeProgressTranslation);
				}
			}
			//if(bDoTimingTest)
			//	LogSpendTime("createPlaidsImages(after pixFinishedFinalPlaids_1) : ");	
			//arguments[0].setCompositionMode(QPainter.CompositionMode_DestinationOver);
			//rectFCalculatedPresentationSourceArea.setRect(0,nYPixelTimeProgressTranslation,rectViewportLocation.width(),rectViewportLocation.height());
			//	Setup for drawing the fixation point
			if(bFirstCustomDraw)
			{
				tmpPen.setColor(colorFixation);
				tmpPen.setStyle(Qt.SolidLine);
				tmpPen.setCapStyle(Qt.RoundCap);
				tmpPen.setWidth(nFixationSize);
				rectFCalculatedPresentationTargetArea.setRect(rectViewportLocation.x(), rectViewportLocation.y(), rectViewportLocation.width(),rectViewportLocation.height());
			}
			arguments[0].drawPixmap(rectViewportLocation.x(), rectViewportLocation.y(), rectViewportLocation.width(),rectViewportLocation.height(),pixFinishedFinalPlaidsArray[ChoosenStimuliBuffer],0,nYPixelTimeProgressTranslation,rectViewportLocation.width(),rectViewportLocation.height());
			
			//Set the Viewportmask
			arguments[0].drawImage(rectFCalculatedPresentationTargetArea,imgViewPortMask);
			arguments[0].setCompositionMode(QPainter.CompositionMode_SourceOver);
			arguments[0].setPen(tmpPen);
			arguments[0].drawPoint(nScreenWidth/2, nScreenHeight/2);			
		}
	}
}

function createPlaidsImagesArray()
{
	pixFinishedFinalPlaidsArray = null;
	pixFinishedFinalPlaidsArray = new Array(numberOfDifferentPlaides);
	for (var i=0;i<numberOfDifferentPlaides;i++)
	{
		pixFinishedFinalPlaidsArray[i] = new QPixmap();
	}
	Log("createPlaidsImagesArray() finished");
}

function createPlaidsImages()
{	
	//Begin painting the first stripe Image
	painter.begin(imgPlaidsReference1);
	bPaintResult = drawStripes();
	painter.end();
	
	if(bPaintResult)
	{	
		//The second one is mirrored
		imgPlaidsReference2 = imgPlaidsReference1.mirrored(true,false);
		if(bEnableStripe2)
		{		
			//Begin painting
			painter.begin(imgPlaidsReference2);
			painter.setCompositionMode(QPainter.CompositionMode_SourceAtop);
			//We need to change the color again
			//tmpPen.setColor(colorStripe2);
			tmpPen.setCapStyle(Qt.SquareCap);
			tmpPen.setStyle(Qt.NoPen);
			painter.setPen(tmpPen);
			tmpBrush.setColor(colorStripe2);
			painter.setBrush(tmpBrush);	
			//Fill the line with another color, first change composition mode
			painter.drawRect(imgPlaidsReference2.rect());
			painter.end();
		}	
		if(bEnableUnion)
		{			
			//The Union one is the combined one, lets start with the first
			imgPlaidsReferenceUnion = imgPlaidsReference1.copy();
			painter.begin(imgPlaidsReferenceUnion);
			painter.setCompositionMode(QPainter.CompositionMode_SourceIn);
			//We need to change the color again
			tmpPen.setStyle(Qt.NoPen);
			painter.setPen(tmpPen);
			tmpBrush.setColor(colorStripeUnion);
			painter.setBrush(tmpBrush);
			//Fill the line with another color, first change composition mode
			painter.drawImage(imgPlaidsReferenceUnion.rect(),imgPlaidsReference2,imgPlaidsReferenceUnion.rect());//targetRect,image,sourceRect
			painter.setCompositionMode(QPainter.CompositionMode_SourceAtop);//CompositionMode_SourceAtop
			painter.drawRect(imgPlaidsReferenceUnion.rect());	
			painter.end();
		}	
		//Now we need to combine the three drawn images
		if(bEnableUnion)
			imgPlaidsReferenceTotal = imgPlaidsReferenceUnion.copy();
		painter.begin(imgPlaidsReferenceTotal);
		if(bEnableUnion==false)
		{
			painter.setCompositionMode(QPainter.CompositionMode_Source);
			painter.fillRect(imgPlaidsReferenceTotal.rect(),Qt.transparent);
		}
		painter.setCompositionMode(QPainter.CompositionMode_DestinationOver);
		if(bEnableStripe1)
			painter.drawImage(imgPlaidsReferenceTotal.rect(),imgPlaidsReference1,imgPlaidsReferenceTotal.rect());//targetRect,image,sourceRect
		if(bEnableStripe2)
			painter.drawImage(imgPlaidsReferenceTotal.rect(),imgPlaidsReference2,imgPlaidsReferenceTotal.rect());//targetRect,image,sourceRect
				
		painter.fillRect(imgPlaidsReferenceTotal.rect(), colorPlaidBackground);
		painter.setCompositionMode(QPainter.CompositionMode_DestinationOver);			
		//End painting	
		painter.end();
		//Let's create the Viewport Image Mask;
		painter.begin(imgViewPortMask);
		//First make sure that the background in transparant
		painter.setCompositionMode(QPainter.CompositionMode_Source);
		painter.fillRect(imgViewPortMask.rect(), colorBackGround);
		painter.setCompositionMode(QPainter.CompositionMode_SourceOver);
		//We need to change the color again, black = active, white = inactive
		tmpPen.setStyle(Qt.NoPen);
		painter.setPen(tmpPen);
		tmpBrush.setColor(new QColor(0,0,0,255));//Color doesn't matter here, as long as the transparancy is 100%...
		painter.setBrush(tmpBrush);
		painter.setCompositionMode(QPainter.CompositionMode_Xor);
		painter.drawEllipse(imgViewPortMask.rect());
		//End painting
		painter.end();
		//imgViewPortMask.invertPixels(QImage.InvertRgba);
		
		//Create the resulting pixmap
		//if(ChoosenStimuliBuffer == 1)
			pixFinishedFinalPlaidsArray[ChoosenStimuliBuffer] = QPixmap.fromImage(imgPlaidsReferenceTotal);
		//if(ChoosenStimuliBuffer == 2)
		//	pixFinishedFinalPlaids_2 = QPixmap.fromImage(imgPlaidsReferenceTotal);
		Log("createPlaidsImages() finished");
		return true;
	}
	Log("*** Error in createPlaidsImages()");
	return false;
}

function drawStripes()
{
	//First make sure that the background in transparant
	painter.setCompositionMode(QPainter.CompositionMode_Source);
	painter.fillRect(imgPlaidsReference1.rect(), Qt.transparent);//colorPlaidBackground);//);	
	//Let's draw with a specific brush/color/pen
	tmpPen.setStyle(Qt.SolidLine);//Qt.NoPen);
	tmpPen.setWidth(nStripeWidth);
	tmpPen.setColor(colorStripe1);
	painter.setPen(tmpPen);
	painter.setCompositionMode(QPainter.CompositionMode_SourceOver);	
	if((nStripeAngle > 0.0) && (nStripeAngle < 90.0))//Important the nStripeAngle must be > 0.0 !
	{
		nStripeAngleMultiplier = 1.0/(Math.sin(nStripeAngle  * (Math.PI / 180.0)));
	}
	else
	{
		return false;
	}
	lineStripeVector.setLine(0,nStimuliHeight,nStimuliMax*nStripeAngleMultiplier,0);//1.5 means always enough space....
	lineStripeVector.setAngle(nStripeAngle);
	nExtraWidthModifier = Math.tan(nStripeAngle  * (Math.PI / 180.0));
	lineStripeVector.translate(-nStimuliHeight/nExtraWidthModifier,0);
	lineStripeDistanceOffset.setX(nStripeDistance * nStripeAngleMultiplier);
	lineStripeDistanceOffset.setY(0);
	while(lineStripeVector.x1() < nStimuliWidth)
	{
		painter.drawLine(lineStripeVector);
		lineStripeVector.translate(lineStripeDistanceOffset);
	}
	return true;
}	




//////////////////////////////////////////////Main Routine///////////////////////////////////////////
//Run the experiment:
BasicExperiment.RunExperiment();