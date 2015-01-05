var ExperimentManagerObj = new ExperimentManager();
var ExperimentTimerObj = new ExperimentTimer();
var currentScriptLocation = StimulGL.getActiveDocumentFileLocation();
var nCurrentBlock = -1;
var nCurrentTrial = -1;
var nCurrentTrigger = -1;
var nFrameCounter = 0;

function CustomScriptHandlerFunction(QPaintObject)
{
	if(nCurrentBlock == 1)
	{
		if(QPaintObject == "QPainter")
		{
			//Let's draw an lineair sinusoidal grating...	
			if(false)
			{
				QPaintObject.save();													//Save the painter iits current state
				var nWidth = 400; 													//The Width of the grating in pixels
				var nHeight = 300;													//The Height of the grating in pixels
				var nPixelsPerCycle = 100;												//The pixels per sinusoidal cycle in the grating
				var nAngle = 45;														//The angle of the grating
				var nPhaseOffset = 0.25 * ExperimentTimerObj.getElapsedTimeInSec();				//The phase offset (0.0 --> 1.0), here we make it dynamic (change each trigger)
				var myStimWindowRect = QPaintObject.window();								//Retrieve the size stimulus area
				myStimWindowRect.setHeight(myStimWindowRect.height() -1);						//We need to fix this, because StimulGL adds an additional pixel...
				QPaintObject.translate(myStimWindowRect.width()/2,myStimWindowRect.height()/2);		//Translate the painters object
				QPaintObject.rotate(nAngle);												//Rotate the painters object
				QPaintObject.translate(myStimWindowRect.width()/-2,myStimWindowRect.height()/-2);		//Translate the painters object (back to original position)				
				var mySingleCycleRect = new QRect(0,0,nPixelsPerCycle,1);						
				var image = new QImage(nPixelsPerCycle,1,QImage.Format_RGB32);
				var color = new QColor();
				var pixelValue;
				var degreeStep = (Math.PI * 2) / nPixelsPerCycle;
				for(var i=0;i<nPixelsPerCycle;i++)
				{
					pixelValue = (255/2) * (Math.sin( (degreeStep/2) + (degreeStep*i) + ((Math.PI * 2) * nPhaseOffset) ) + 1);
					color.setRgb(pixelValue,pixelValue,pixelValue);
					image.setPixel(i, 0, color.rgb());
				}		
				var brush = new QBrush(image);
				QPaintObject.setPen(new QPen(Qt.NoPen));
				QPaintObject.setBrush(brush);
				QPaintObject.drawEllipse((myStimWindowRect.width()-nWidth)/2,(myStimWindowRect.height()-nHeight)/2,nWidth,nHeight);
				QPaintObject.restore();
			}
			else
			{
				var myStimWindowRect = QPaintObject.window();
				myStimWindowRect.setHeight(myStimWindowRect.height() -1);
				//var image = new QImage(nPixelsPerCycle,1,QImage.Format_RGB32);
				var color = new QColor("ffaaaa");
				var pixmap = new QPixmap(200,200);
				//for(var i=0;i<250;i++)//nPixelsPerCycle;i++)
				//{
					//pixelValue = (255/2) * (Math.sin( (degreeStep/2) + (degreeStep*i) + ((Math.PI * 2) * nPhaseOffset) ) + 1);
					//color.setRgb(pixelValue,pixelValue,pixelValue);
					//image.setPixel(i, 0, color.rgb());
				//}
				var brush = new QBrush();//(image);
				brush.setColor(color);
				//QPaintObject.setPen(new QPen(Qt.NoPen));
				QPaintObject.setBrush(brush);
				for(var qq=0;qq<250;qq++)
				{
					QPaintObject.drawEllipse((myStimWindowRect.width())/2,(myStimWindowRect.height())/2,200+(2*qq),200+nFrameCounter);
				}
				nFrameCounter++;
			}
		}
	}	
}

function CleanupScript()
{
	ExperimentTimerObj.stop();
	ConnectDisconnectScriptFunctions(false);
	CustomScriptHandlerFunction = null;
	ExperimentTimerObj = null;
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	ExperimentStructureChanged = null;
	ConnectDisconnectScriptFunctions = null;	
	CleanupScript = null;
	StimulGL.cleanupScript();
}

function ExperimentStructureChanged()
{
//	Log("--- ExperimentStructureChanged arguments count: " + arguments.length);
//	for (var i = 0; i < arguments.length; ++i)
//		Log("\t argument(" + i + "):" + arguments[i]);	
	nCurrentBlock = arguments[0];
	nCurrentTrial = arguments[1];
	nCurrentTrigger = arguments[2];
	if(nCurrentBlock==1) 
	{
		if((nCurrentTrial==0) && (nCurrentTrigger==0))
		{
			ExperimentTimerObj.start();
		}
		//Log("- Elapsed Time: " + ExperimentTimerObj.getElapsedTimeInSec());
	}
	var textToShow = "->-> Block " + nCurrentBlock + ", Trial " + nCurrentTrial + ", Trigger " + nCurrentTrigger;		
	Log(textToShow);
}

function ExperimentStateChanged(currState)
{
	if(currState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{	
		Log("--- ExperimentManager_Initialized");
		ConnectDisconnectScriptFunctions(true);	
		if(RetinoMap_Object_1.installCustomScriptHandlerFunction("CustomScriptHandlerFunction"))
			Log("--- Custom Script Handler Function installed");
		else
			Log("--- Failed to install the Custom Script Handler Function");
	}	
	if(currState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
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
			RetinoMap_Object_1.ExperimentStructureChanged.connect(this, this.ExperimentStructureChanged);
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
			RetinoMap_Object_1.ExperimentStructureChanged.disconnect(this, this.ExperimentStructureChanged);
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}
ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);
ExperimentManagerObj.setExperimentFileName(currentScriptLocation + "/CustomPainting.exml");
ExperimentManagerObj.runExperiment();