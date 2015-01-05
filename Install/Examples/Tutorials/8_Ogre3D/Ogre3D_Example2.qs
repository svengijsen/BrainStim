var ExperimentManagerObj = new ExperimentManager(); 					//Here we create the Experiment Manager object that can run experiments.
var currentScriptLocation = StimulGL.getActiveDocumentFileLocation();		//Here we store the directory-path from this script file for further usage.
var scriptRootPath = StimulGL.getActiveDocumentFileLocation();
var exmlFilePath = scriptRootPath + "/" + "Ogre3D_Example2.exml";
var meshSourceLocation = QDir.cleanPath(scriptRootPath + "/../Common/Meshes");
Log(meshSourceLocation);
var meshSourceLocationType = "FileSystem";
var cExperimentStructure_Object = new cExperimentStructure();
var currentRunArrayCounter = 0;
var timerTriggerInterval = 1000;
var currentExperimentState;
var cBlockStructure_Object0;
var arr3DObjects = Array[ 
					"face_05n.mesh",
					"teapot_02n.mesh"];	
var arrViewpoints = Array[ 
					-60,
					-45,
					-30,
					-15,
					 0,
					 15,
					 30,
					 45,
					 60];
var nTrials = 18;				
var runArray = Array[];	

function preConfigureExperimentParameters()
{
	ExperimentManagerObj.setDebugMode(true);
}

function startExperimentObjects()
{
	Timer_Object_1.startTimer(timerTriggerInterval);
}

function stopExperimentObjects()
{
	Timer_Object_1.stopTimer();
}

function changeStimuli(nodeName,entityName,startFaceAngle,animationAngleSpan,animationDuration)
{
	Log("-----------------------------------------");
	Log("-- changeStimuli called -> " + nodeName + "," + entityName + "," + startFaceAngle + "," + animationAngleSpan + "," + animationDuration);
	Log("-----------------------------------------");

	MainItem = "functions";
	if(nodeName == "initialize")
	{
		var i = 0;
		functionName = "addOgreResourceLocation";
		retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,meshSourceLocation,meshSourceLocationType);
		Log(functionName + "() returned: " + retVal);
		functionName = "addOgreMesh3DModel";	
		var tmpName = "";
		for(i=0; i<arr3DObjects.length;i++)
		{
			tmpName =  arr3DObjects[i].split(".")[0];
			retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,"SceneNode_" + tmpName,"Entity_" + tmpName,arr3DObjects[i]);
			Log(functionName + "() returned: " + retVal);
		}		
		functionName = "set3DMeshAnimation";
		retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,nodeName,entityName,0,0,0);	//To let the component initialize	
	}
	else
	{
		functionName = "set3DMeshAnimation";
		retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,nodeName,entityName,startFaceAngle,animationAngleSpan,animationDuration);
		Log(functionName + " returned: " + retVal);
	}
}

function createArray(length) 
{
	var arr = new Array(length || 0),
	i = length;

	if (arguments.length > 1) 
	{
		var args = Array.prototype.slice.call(arguments, 1);
		while(i--) 
			arr[length-1 - i] = createArray.apply(this, args);
	}
	return arr;
}

function GenerateExperiment()
{
	Log("\n");
	
	runArray = createArray(nTrials,2);
	for(var x=0;x<nTrials;x++)
	{
		runArray[x][0] = arr3DObjects[x%2];
		runArray[x][1] = arrViewpoints[x%(nTrials/2)];
	}
	Log("\n");
}

function externalTriggerRecieved()
{
	Log("-----------------------------------------");
	Log("-- externalTriggerRecieved called");
	Log("-----------------------------------------");
	currentExperimentState = cExperimentStructure_Object.getCurrentExperimentState();	
	if(currentExperimentState.CurrentBlock_BlockID==1)
	{
		if(currentExperimentState.CurrentBlock_InternalTrigger%2 == 0)
		{
			tmpName =  runArray[currentRunArrayCounter][0].split(".")[0];
			changeStimuli("SceneNode_" + tmpName,"Entity_" + tmpName,runArray[currentRunArrayCounter][1],10,2 * timerTriggerInterval);
			currentRunArrayCounter++;
		}
	}
	if(currentExperimentState.CurrentBlock_BlockID>=0)
	{
		cBlockStructure_Object0 = cExperimentStructure_Object.getBlockPointerByID(currentExperimentState.CurrentBlock_BlockID);
	}	
	LogExperimentState();	
}

function LogExperimentState()
{
	MainItem = "DebugMode_Functions";
	functionName = "updateExperimentState";
	retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,currentExperimentState.CurrentBlock_BlockID,currentExperimentState.CurrentBlock_TrialNumber,currentExperimentState.CurrentBlock_InternalTrigger);
}

function NewInitBlockTrial()
{
	Log("-----------------------------------------");
	Log("--- Starting to initialize a new Block Trial (" + currentExperimentState.CurrentBlock_TrialNumber + ")");
	Log("-----------------------------------------");
}

function NewQMLSourceLoaded(sSource)
{
	changeStimuli("initialize","",0,0,0);
	Log("-----------------------------------------");
	Log("NewQMLSourceLoaded: " + sSource);
	Log("-----------------------------------------");
	MainItem = "DebugMode_Functions";
	functionName = "setVisibility";
	retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,true);
	Log("    <<< " + functionName + " returned: " + retVal + " >>>");		
}

function ExperimentStateChanged(currentState)
{
	var textToShow = "--- The Experiment State Changed to: ";
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{
		Log(textToShow + "ExperimentManager_Initialized");
		//Now all defined objects in the experiment file are constructed and therefore available in this script, so now we can make the connections between constructed the objects.
		cExperimentStructure_Object = ExperimentManagerObj.getExperimentStructure();
		currentExperimentState = cExperimentStructure_Object.getCurrentExperimentState();
		preConfigureExperimentParameters();
		ConnectDisconnectScriptFunctions(true);	
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_IsStarting)
	{	
		Log(textToShow + "ExperimentManager_IsStarting");
		//initExperimentObjects();
		startExperimentObjects();
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_IsStopping)
	{	
		Log(textToShow + "ExperimentManager_IsStopping");
		stopExperimentObjects();
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		Log(textToShow + "ExperimentManager_Stopped");
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
			QML2Viewer_Object_1.NewInitBlockTrial.connect(this, this.NewInitBlockTrial);
			QML2Viewer_Object_1.NewSourceLoaded.connect(this,this.NewQMLSourceLoaded);
			cExperimentStructure_Object.externalTriggerRecieved.connect(this, externalTriggerRecieved);
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
			QML2Viewer_Object_1.NewInitBlockTrial.disconnect(this, this.NewInitBlockTrial);
			cExperimentStructure_Object.externalTriggerRecieved.disconnect(this, externalTriggerRecieved);
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}

function CleanupScript()
{
	//Functions
	ConnectDisconnectScriptFunctions(false);
	ConnectDisconnectScriptFunctions = null;
	preConfigureExperimentParameters = null;
	GenerateExperiment = null;
	ExperimentStateChanged = null;
	NewInitBlockTrial = null;
	NewQMLSourceLoaded = null;
	externalTriggerRecieved = null;
	LogExperimentState = null;
	startExperimentObjects = null;
	stopExperimentObjects  = null;
	changeStimuli = null;
	createArray = null;
	CleanupScript = null;
	//Objects
	arrViewpoints = null;
	arr3DObjects = null;
	runArray = null;
	cExperimentStructure_Object = null;
	cBlockStructure_Object0 = null;
	//Finalize
	Log("Going to call StimulGL.cleanupScript()...");
	StimulGL.cleanupScript();
	//StimulGL.quit();
}

GenerateExperiment();
Log(runArray);
ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);
ExperimentManagerObj.setExperimentFileName(exmlFilePath); //Set the experiment file to use
if(ExperimentManagerObj.runExperiment()==false) //Try to run the experiment
{
	Log("Something went wrong running the experiment");
	CleanupScript();
}
else
{
	Log("Started the experiment");
}