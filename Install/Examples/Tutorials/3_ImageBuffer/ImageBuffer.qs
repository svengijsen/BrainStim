var scriptRootPath = StimulGL.getActiveDocumentFileLocation();
var qsFilePath = scriptRootPath + "/" + "ImageBuffer_ImageArrays.qs";
var exmlFilePath = scriptRootPath + "/" + "ImageBuffer.exml";
var sSeperator = "?";
var ExperimentManagerObj = new ExperimentManager();
var tmpPixMap;
var tmpString;
var retVal;
var functionName;
var MainItem;

Include(qsFilePath);

function CleanupScript()
{
	ConnectDisconnectScriptFunctions(false);
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	NewInitBlockTrial = null;
	ExternalTriggerIncremented = null;
	ExperimentStructureChanged = null;
	ConnectDisconnectScriptFunctions = null;
	CleanupScript = null;
	preCacheImages = null;
	preProcessImages = null;
	NewQMLSourceLoaded = null;
	tmpPixMap = null;
	tmpString = null;
	StimulGL.cleanupScript();
}

function NewInitBlockTrial()
{
	Log("--- Starting to initialize a new Block Trial (" + arguments.length + " argument(s))");
}

function ExternalTriggerIncremented()
{
	Log("--- The External Trigger is incremented (" + arguments.length + " argument(s): " + arguments[0] +")");	
}

function preCacheImages()
{
	var nCounter = 0;
	tmpPixMap = new QPixmap();
	functionName = "setImageFunction";
	MainItem = "functions";
	
	for(var i=0;i<ImagePathArray.length;i++)
	{
		tmpString = ImagePathArray[i];
		if(tmpString != "fixation")
		{
			try
			{
				tmpString = tmpString.split(sSeperator)[0];
				if(tmpPixMap.load(tmpString))
				{					
					tmpString = qmlWidget_Object_1.addPixmapToImageBuffer(tmpPixMap);
					if(qmlWidget_Object_1.updatePixmapIDFromImageBuffer(tmpString,ImagePathArray[i]))
					{
						nCounter++;
					}
					else
					{
						Log("updatePixmapIDFromImageBuffer() failed");
					}
					//Log("- pre-Cached :" + ImagePathArray[i]);
				}
				else
				{
					Log(".*. Something went wrong loading the Image: " + Param1);
				}
			} 
			catch(e)
			{
				Log(".*. Something went wrong pre-Caching the Image:" + e);
			}
			//Log("<<< " + functionName + "(" + Param1 + ") returned: " + retVal + " >>>");	
		}
	}
	Log("... pre-Caching images done(" + nCounter + " item(s) cached)!");
}

function preProcessImages() 
{
	Log("... pre-Processing images");
	var currLenght = ImagePathArray.length;
	var nCounter = 0;
	for(var i=0;i<currLenght;i++)
	{
		if(ImagePathArray[i] != "fixation")
		{		
			if(qmlWidget_Object_1.getPixmapFromImageBuffer(tmpPixMap, ImagePathArray[i]))
			{
				Log("... getPixmapFromImageBuffer(" + ImagePathArray[i] + ") successfully done!");

				//tmpPixMap.fill(new QColor ("#ff0000"));
				tmpPixMap = tmpPixMap.scaledToWidth(300);

				//tmpString = qmlWidget_Object_1.addPixmapToImageBuffer(tmpPixMap);
				//Log("... addPixmapToImageBuffer returned: " + tmpString);
				//Log("... removePixmapFromImageBuffer returned: " + qmlWidget_Object_1.removePixmapFromImageBuffer(tmpString));
				Log("... updatePixmapFromImageBuffer(" + tmpString + ") returned: " + qmlWidget_Object_1.updatePixmapFromImageBuffer(tmpPixMap,ImagePathArray[i]));
			}		
			else
			{
				Log("preProcessImages --> Could not getPixmapFromImageBuffer()");
			}
		}
	}
	Log("... pre-Processing images done(" + ImagePathArray.length + " item(s) processed)!");	
}

function ExperimentStructureChanged(nCurrentBlock,nCurrentTrial,nCurrentTrigger)
{
	var textToShow = "Block " + nCurrentBlock + ", Trial " + nCurrentTrial + ", Trigger " + nCurrentTrigger;
	Log("--- The Experiment Structure Changed to : " + textToShow);

	try 
	{		
		if((nCurrentBlock == 0))
		{
			functionName = "setImageFunction";
			MainItem = "functions";
			Param1 = ImagePathArray[nCurrentTrigger];
			retVal = qmlWidget_Object_1.invokeQml2Method(MainItem,functionName,Param1);
			Log("    <<< " + functionName + "(" + Param1 + ") returned: " + retVal + " >>>");			

			MainItem = "DebugMode_Functions";
			functionName = "updateExperimentState";
			retVal = qmlWidget_Object_1.invokeQml2Method(MainItem,functionName,nCurrentBlock,nCurrentTrial,nCurrentTrigger);
			Log("    <<< " + functionName + " returned: " + retVal + " >>>");
		}
		else if(nCurrentTrial == 1)
		{
			ExperimentManagerObj.abortExperiment();
		}
	} 
	catch (e) 
	{
		Log(".*. Something went wrong invoking the slot:" + e);
	}
}

function ExperimentStateChanged(state)
{
	var textToShow = "--- The Experiment State Changed to: ";
	
	if(state == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{
		Log(textToShow + "ExperimentManager_Initialized");
		//Now all defined objects in the experiment file are constructed and therefore available in this script, so now we can make the connections between constructed the objects.
		ConnectDisconnectScriptFunctions(true);	
		preCacheImages();	
		preProcessImages();	
	}	
	else if(state == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		Log(textToShow + "ExperimentManager_Stopped");
		CleanupScript();
	}
}

function NewQMLSourceLoaded()
{
	Log("NewQMLSourceLoaded() called.");
	ExperimentStructureChanged(0,0,0);//Initialization, this structure state is not detected in the ExperimentStructureChanged() slot
}

function ConnectDisconnectScriptFunctions(Connect)
{
	if(Connect)
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			qmlWidget_Object_1.NewInitBlockTrial.connect(this, this.NewInitBlockTrial);
			qmlWidget_Object_1.ExternalTriggerIncremented.connect(this, this.ExternalTriggerIncremented);
			qmlWidget_Object_1.ExperimentStructureChanged.connect(this, this.ExperimentStructureChanged);
			qmlWidget_Object_1.NewSourceLoaded.connect(this, this.NewQMLSourceLoaded);			
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
			qmlWidget_Object_1.NewInitBlockTrial.disconnect(this, this.NewInitBlockTrial);
			qmlWidget_Object_1.ExternalTriggerIncremented.disconnect(this, this.ExternalTriggerIncremented);
			qmlWidget_Object_1.ExperimentStructureChanged.disconnect(this, this.ExperimentStructureChanged);
			qmlWidget_Object_1.NewSourceLoaded.connect(this, this.NewQMLSourceLoaded);
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}

ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);
if(ExperimentManagerObj.setExperimentFileName(exmlFilePath))
{
	ExperimentManagerObj.runExperiment();
}
else
{
	Log("*** Error: Could not locate the experiment file!");
}
