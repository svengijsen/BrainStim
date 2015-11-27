//Includes:
Include("QtScript/BasicQMLExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/

//Included variables:
BasicExperiment.sExmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "Ogre3D_Example2.exml";
BasicExperiment.nTestModeTriggerDuration = 1000;

//Custom script references (variables):
var QML2Viewer_Global_QmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "Ogre3D_Example2.qml";

//Local variables
var _meshSourceLocation = QDir.cleanPath(BrainStim.getActiveDocumentFileLocation() + "/../../Common/Meshes");
Log(_meshSourceLocation);
var _meshSourceLocationType = "FileSystem";
var _arr3DObjects = new Array( 
					"face_05n.mesh",
					"teapot_02n.mesh");	
var _arrViewpoints = new Array( 
					-60,
					-45,
					-30,
					-15,
					 0,
					 15,
					 30,
					 45,
					 60);
var _nTrials = 18;				
var _runArray = [];	
var _currentRunArrayCounter = 0;

//Local Functions
function _createArray(length) 
{
	var arr = new Array(length || 0),
	i = length;

	if (arguments.length > 1) 
	{
		var args = Array.prototype.slice.call(arguments, 1);
		while(i--) 
			arr[length-1 - i] = _createArray.apply(this, args);
	}
	return arr;
}

function _changeStimuli(_nodeName, _entityName, _startFaceAngle, _animationAngleSpan, _animationDuration)
{
	Log("-----------------------------------------");
	Log("-- _changeStimuli called -> " + _nodeName + "," + _entityName + "," + _startFaceAngle + "," + _animationAngleSpan + "," + _animationDuration);
	Log("-----------------------------------------");

	if(_nodeName == "initialize")
	{
		var i = 0;
		var _retVal;
		var _tmpName = "";
		
		_retVal = BasicExperiment.QMLViewerObject.invokeQml2Method("functions","addOgreResourceLocation",_meshSourceLocation,_meshSourceLocationType);
		Log("addOgreResourceLocation() returned: " + _retVal);
		
		for(i=0; i<_arr3DObjects.length;i++)
		{
			_tmpName =  _arr3DObjects[i].split(".")[0];
			_retVal = BasicExperiment.QMLViewerObject.invokeQml2Method("functions", "addOgreMesh3DModel", "SceneNode_" + _tmpName, "Entity_" + _tmpName, _arr3DObjects[i]);
			Log("addOgreMesh3DModel() returned: " + _retVal);
		}		
		_retVal = BasicExperiment.QMLViewerObject.invokeQml2Method("functions", "set3DMeshAnimation", _nodeName, _entityName, 0, 0, 0);	//To let the component initialize	
	}
	else
	{
		_retVal = BasicExperiment.QMLViewerObject.invokeQml2Method("functions", "set3DMeshAnimation", _nodeName, _entityName, _startFaceAngle, _animationAngleSpan, _animationDuration);
		Log("set3DMeshAnimation() returned: " + _retVal);
	}
}

//Override Public Functions from the BasicQMLExperiment.qs include (that includes BasicExperiment):
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	BrainStim.write2OutputWindow("*** Going to initialize a new Block-trial: BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber);

	if(_currentBlockNumber == 0)
	{
		_changeStimuli("initialize","",0,0,0);
		//var retVal = BasicExperiment.QMLViewerObject .invokeQml2Method("functions", "showOgre3DItem", true);
	}
	else if(_currentBlockNumber == 1)
	{
		var _tmpName =  _runArray[_currentRunArrayCounter][0].split(".")[0];
		_changeStimuli("SceneNode_" + _tmpName, "Entity_" + _tmpName,_runArray[_currentRunArrayCounter][1], 10, 2 * BasicExperiment.nTestModeTriggerDuration);
		_currentRunArrayCounter++;
	}	
}

BasicExperiment.__proto__.ExperimentStateChanged = function(currentState, sDateTimeStamp)
{
	//This Funtion is called automatically at each time the Experiment State changes
	//See Help documentation (search for "ExperimentManager::ExperimentState") for a description of the different states
	
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Initialized at " + sDateTimeStamp);
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Started at " + sDateTimeStamp);
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		BrainStim.write2OutputWindow("*** ExperimentStateChanged to: Stopped at " + sDateTimeStamp);
	}
}

//Create the experiment arrays
_runArray = _createArray(_nTrials,2);
for(var x=0;x<_nTrials;x++)
{
	_runArray[x][0] = _arr3DObjects[x%2];
	_runArray[x][1] = _arrViewpoints[x%(_nTrials/2)];
}
//Run the experiment:
BasicExperiment.RunExperiment();






















/*

var ExperimentManagerObj = new ExperimentManager(); 					//Here we create the Experiment Manager object that can run experiments.
var currentScriptLocation = BrainStim.getActiveDocumentFileLocation();		//Here we store the directory-path from this script file for further usage.
var scriptRootPath = BrainStim.getActiveDocumentFileLocation();
var exmlFilePath = scriptRootPath + "/" + "Ogre3D_Example2.exml";

var cExperimentStructure_Object = new cExperimentStructure();

var timerTriggerInterval = 1000;
var currentExperimentState;
var cBlockStructure_Object0;


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
			BasicExperiment.QMLViewerObject.NewInitBlockTrial.connect(this, this.NewInitBlockTrial);
			BasicExperiment.QMLViewerObject.NewSourceLoaded.connect(this,this.NewQMLSourceLoaded);
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
			BasicExperiment.QMLViewerObject.NewInitBlockTrial.disconnect(this, this.NewInitBlockTrial);
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
	Log("Going to call BrainStim.cleanupScript()...");
	BrainStim.cleanupScript();
	//BrainStim.quit();
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
*/
