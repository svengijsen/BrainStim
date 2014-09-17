var ExperimentManagerObj = new ExperimentManager();
var painter = new QPainter();
var mousePixmap = new QPixmap(384,384);//40,70);
var flickrTimer = new QTimer();//TriggerTimer();
var tmpColor = new QColor();
var nCurrentBlock = -1;
var nCurrentTrial = -1;
var nCurrentTrigger = -1;
var xPos = 20;
var yPos = 25;
var _color = new QColor(Qt.darkRed);//or 255,255,255
var _brush = new QBrush(_color,Qt.SolidPattern);
var _pen = new QPen(Qt.NoPen);
var _tail = new QPainterPath(new QPointF(xPos, yPos+20));
_tail.cubicTo(xPos-5, yPos+22, xPos-5, yPos+22, xPos+0, yPos+25);
_tail.cubicTo(xPos+5, yPos+27, xPos+5, yPos+32, xPos+0, yPos+30);
_tail.cubicTo(xPos-5, yPos+32, xPos-5, yPos+42, xPos+0,yPos+35);
var classifierWidth = 50;
var classifierHeight = classifierWidth;
var screenWidth = 768;
var stimuliWidth = screenWidth/2;
var stimuliHeight = stimuliWidth;
var classifierPixmap_1 = new QPixmap(classifierWidth,classifierHeight);
var classifierPixmap_2 = new QPixmap(classifierWidth,classifierHeight);
var classifierImage = new QImage(classifierWidth, classifierHeight, QImage.Format_ARGB32_Premultiplied);
var classifierImages = new QImage(stimuliWidth, stimuliHeight, QImage.Format_ARGB32_Premultiplied);

var stimuliStepAmount = 12;
var stimuliIndex = 0;
var stimuliSpan = 360/stimuliStepAmount;
var stimuliDegrees = (stimuliIndex*stimuliSpan) + (stimuliSpan/2);
var stimuliStart = 0.25;
var PI = 3.141592653589793238462643383279502884197;
var fCalculatedSize = (stimuliStepAmount/(stimuliStepAmount-1)) * PI * (stimuliStart * ( (2/12) * PI) );
Log(stimuliWidth*fCalculatedSize);

var innerEccPoint             = new QPointF(stimuliStart*stimuliWidth,0);
var outerEccPoint            = new QPointF((stimuliStart*stimuliWidth)+stimuliWidth*fCalculatedSize,0);
var polarPoint                  = new QPointF((stimuliStart*stimuliWidth)+stimuliWidth*fCalculatedSize/2,0);
var currentFlickr = 0;
var testAngle = 0;


function CustomScriptHandlerFunction()
{
	if(nCurrentBlock == 1)
	{
		if(arguments[0] == "QPainter")
		{
			arguments[0].save();
			arguments[0].translate(stimuliWidth,stimuliHeight);
			arguments[0].rotate(testAngle);
			arguments[0].translate(0,(stimuliHeight/-2));
			if(currentFlickr)
			{
				arguments[0].drawPixmap(0,0, classifierPixmap_1);
			}
			else
			{
				arguments[0].drawPixmap(0,0, classifierPixmap_2);
			}
			arguments[0].restore();
		}
	}	
}

function drawClassifiers()
{
	painter.setCompositionMode(QPainter.CompositionMode_Source);
	painter.fillRect(classifierImages.rect(), Qt.transparent);//Qt.darkRed);//Qt.white);//transparent);
	painter.translate(0,(stimuliHeight/2));
	painter.save();

	painter.translate(0,-(classifierHeight/2));
	painter.translate(innerEccPoint);
	drawClassifier();
	painter.restore();
	painter.save();
	
	painter.translate(0,-(classifierHeight/2));	
	painter.translate(outerEccPoint);
	drawClassifier();
	painter.restore();
	painter.save();
	
	painter.rotate(stimuliDegrees);
	painter.translate(0,-(classifierHeight/2));	
	painter.translate(polarPoint);
	drawClassifier();
	painter.restore();
	painter.save();

	painter.rotate(-stimuliDegrees);
	painter.translate(0,-(classifierHeight/2));
	painter.translate(polarPoint);
	drawClassifier();
	painter.restore();
	painter.save();
		
	
//	_brush.setColor(Qt.darkYellow);
//	painter.setBrush(_brush);
//	painter.drawLine(,outerEccPoint);	
//	painter.restore();
//	painter.save();
}

function cropImage()
{
//	Log(classifierImages.width());
	//classifierImages = classifierImages.createHeuristicMask(true);
//	tmpColor = Qt.darkRed;
//	classifierImages = classifierImages.createMaskFromColor(tmpColor);
//	Log(classifierImages.width());
	classifierPixmap_1 = QPixmap.fromImage(classifierImages);
	classifierPixmap_2 = QPixmap.fromImage(classifierImages.mirrored());
}

function drawClassifier()
{
	//arguments[0].rotate(60);
	
	_brush.setColor(Qt.white);
	painter.setBrush(_brush);
	_pen.setStyle(Qt.NoPen);
	painter.setPen(_pen);
		
	painter.setCompositionMode(QPainter.CompositionMode_Source);
	painter.fillRect(classifierImage.rect(), Qt.transparent);
	
	painter.setCompositionMode(QPainter.CompositionMode_SourceOver);	
	_brush.setColor(Qt.white);
	painter.setBrush(_brush);
	painter.drawEllipse(0, 0, classifierWidth, classifierWidth);	
	
	painter.setCompositionMode(QPainter.CompositionMode_SourceIn);//CompositionMode_SourceIn);//CompositionMode_SourceAtop);
	_brush.setColor(Qt.black);
	painter.setBrush(_brush);
	painter.drawRect(0, 0, classifierWidth/2, classifierWidth/2);
	painter.drawRect(classifierWidth/2, classifierWidth/2, classifierWidth/2, classifierWidth/2);
	
	//painter.setCompositionMode(QPainter.CompositionMode_DestinationOver);
	//tmpColor.setRgb(0,0,255);
	//painter.fillRect(classifierImage.rect(), tmpColor);	
}

function changeFlickr()
{
	if(currentFlickr)
		currentFlickr = 0;
	else
		currentFlickr = 1;	
}

function CleanupScript()
{
	ConnectDisconnectScriptFunctions(false);
	CustomScriptHandlerFunction = null;
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	ExperimentStructureChanged = null;
	ConnectDisconnectScriptFunctions = null;
	drawMouse = null;
	drawClassifier = null;
	drawClassifiers = null;
	cropImage = null;
	painter = null;
	_tail = null;
	_brush = null;
	_color = null;
	_pen = null;
	innerEccPoint = null;
	outerEccPoint = null;
	polarPoint = null;
	lastClockWisePoint = null;
	orgPolygon = null;
	polyBoundingRect = null;
	transform = null;
	createMousePixmap = null;
	createClassifierImage = null;
	createClassifierImages = null;
	classifierPixmap_1 = null;
	classifierPixmap_2 = null;
	classifierImage = null;
	classifierImages = null;
	mousePixmap = null;
	CleanupScript = null;
	changeFlickr = null;
	flickrTimer = null;
	BrainStim.cleanupScript();
}

function createMousePixmap()
{
	painter.begin(mousePixmap);
	drawMouse();
	painter.end();
}

function createClassifierImage()
{
	//Log("createClassifierImage");
	//classifierImage.fill(backgroundColor);
	painter.begin(classifierImage);
	drawClassifier();
	painter.end();
	classifierPixmap_1 = QPixmap.fromImage(classifierImage);
	classifierPixmap_2 = QPixmap.fromImage(classifierImage.mirrored());
	//tmpColor.setRgb(0,255,0);
	//Log("1: " + classifierPixmap.hasAlpha());	
	//classifierPixmap.setMask(classifierPixmap.createMaskFromColor(tmpColor));//,Qt.MaskOutColor));//backgroundColor, Qt.MaskOutColor));
	//Log("2: " + classifierPixmap.hasAlpha());	
	//Log("createClassifierImage done...");
}

function createClassifierImages()
{
	painter.begin(classifierImages);
	drawClassifiers();
	painter.end();
	cropImage();
}

function drawMouse()
{
	//Body
	_brush.setColor(Qt.darkRed);
	painter.setBrush(_brush);
	painter.drawEllipse(xPos-10, yPos-20, 20, 40);
	//Eyes
	_brush.setColor(Qt.white);
	painter.setBrush(_brush);//this._brush);
	painter.drawEllipse(xPos-10, yPos-17, 8, 8);
	painter.drawEllipse(xPos+2, yPos-17, 8, 8);
	//Nose
	_brush.setColor(Qt.black);
	painter.setBrush(_brush);
	painter.drawEllipse(xPos-2, yPos-22, 4, 4);
	//Pupils
	painter.drawEllipse(xPos-7, yPos-14, 3, 3);
	painter.drawEllipse(xPos+5, yPos-14, 3, 3);
	//Ears
	_brush.setColor(Qt.darkYellow);
	painter.setBrush(_brush);
	painter.drawEllipse(xPos-17, yPos-12, 16, 16);
	painter.drawEllipse(xPos+1, yPos-12, 16, 16);
//	Tail
	painter.setBrush(Qt.NoBrush);
	painter.drawPath(_tail);
}

function ExperimentStructureChanged()
{
//	Log("--- ExperimentStructureChanged arguments count: " + arguments.length);
//	for (var i = 0; i < arguments.length; ++i)
//		Log("\t argument(" + i + "):" + arguments[i]);
	
	nCurrentBlock = arguments[0];
	nCurrentTrial = arguments[1];
	nCurrentTrigger = arguments[2];
	var textToShow = "->-> Block " + nCurrentBlock + ", Trial " + nCurrentTrial + ", Trigger " + nCurrentTrigger;		
	Log(textToShow);
	
//	try 
//	{		
//		if((nCurrentBlock == 0))
//		{
//			var retVal;
//			var retVal2 = new QColor;
//			var functionName = "myQmlFunction";
//			var MainItem = "functions";
//			var Param1 = "input Param1";//new QString("input Param1");
//			retVal2 = qmlWidget_Object_1.invokeQmlMethod(MainItem,functionName,Param1);
//			Log("<<< " + functionName + " returned: " + retVal2.name() + " >>>");
//			
//			functionName = "myQmlTextFunction";
//			retVal = qmlWidget_Object_1.invokeQmlMethod(MainItem,functionName,Param1);
//			Log("<<< " + functionName + " returned: " + retVal + " >>>");
//			
//			MainItem = "DebugMode_Functions";
//			functionName = "updateExperimentState";
//			retVal = qmlWidget_Object_1.invokeQmlMethod(MainItem,functionName,nCurrentBlock,nCurrentTrial,nCurrentTrigger);
			//Log("<<< " + functionName + " returned: " + retVal + " >>>");
//		}
//		else if(nCurrentTrial == 1)
//		{
//			ExperimentManagerObj.abortExperiment();
//		}
//	} 
//	catch (e) 
//	{
//		Log(".*. Something went wrong invoking the slot:" + e);
//	}	
}

function ExperimentStateChanged(currState)
{
	if(currState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{	
		Log("--- ExperimentManager_Initialized");
		//Now all defined objects in the experiment file are also available in the script,
		//make sure to use the right name!
		ConnectDisconnectScriptFunctions(true);	
		createClassifierImages();
		flickrTimer.start(100);
		if(RetinoMap_RenderWidgetGL.installCustomScriptHandlerFunction("CustomScriptHandlerFunction"))
			Log("--- Custom Script Handler Function installed");
		else
			Log("--- Failed to install the Custom Script Handler Function");
	}	
	else if(currState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		Log("--- ExperimentManager_Stopped");
		CleanupScript();
	}
}

function ConnectDisconnectScriptFunctions(Connect)
{
	if(Connect)
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			//RetinoMap_RenderWidgetGL.NewInitBlockTrial.connect(this, this.SignalCaptureFunction);
			//RetinoMap_RenderWidgetGL.ExternalTriggerIncremented.connect(this, this.SignalCaptureFunction);
			RetinoMap_RenderWidgetGL.ExperimentStructureChanged.connect(this, this.ExperimentStructureChanged);
			flickrTimer.timeout.connect(this, this.changeFlickr);			
		} 
		catch (e) 
		{
			Log(".*. Something went wrong connecting the Signal/Slots:" + e);
		}		
	}
	else
	{
		Log("... Disconnecting Signal/Slots");
		try 
		{	
			ExperimentManagerObj.ExperimentStateHasChanged.disconnect(this, this.ExperimentStateChanged);
			//RetinoMap_RenderWidgetGL.NewInitBlockTrial.disconnect(this, this.SignalCaptureFunction);
			//RetinoMap_RenderWidgetGL.ExternalTriggerIncremented.disconnect(this, this.SignalCaptureFunction);
			RetinoMap_RenderWidgetGL.ExperimentStructureChanged.disconnect(this, this.ExperimentStructureChanged);
			flickrTimer.timeout.disconnect(this, this.changeFlickr);	
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}
ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);
ExperimentManagerObj.setExperimentFileName(BrainStim.getApplicationRootDirPath() + "/Experiments/CustomScriptPainting.exml");
ExperimentManagerObj.runExperiment();

//Polar1.exml
//Polar2.exml
//Eccentricity1.exml
//Eccentricity2.exml
//MovingBar1.exml
//MovingBar2.exml
//MovingDots1.exml