var cExperimentStructure_Object = new cExperimentStructure();
var ExperimentManagerObj = new ExperimentManager(); 				//Here we create the Experiment Manager object that can run experiments
var currentScriptLocation = BrainStim.getActiveDocumentFileLocation();	//Here we store the directory-path from this script file for further usage
var ExperimentStructureState_Object;							//We'll use this object to hold and retrieve the Experiment Structure state
var cBlockStructure_Object = new cBlockStructure();				//We'll use this object to hold and retrieve specific Block information
var sCurrentExperimentPresentation;								//This value holds the current Experiment Presentation Type for the QML2Viewer object; eg. which fruit to show
var sExperimentPresentationArray = new Array();					//This value holds a list with the available Experiment Presentation item to show, they can also be randomized
var RandomGeneratorObj;										//This object is used for the Experiment Presentation item randomization

function CleanupScript() //Cleanup the script
{
	//Disconnect the signal/slots
	ConnectDisconnectScriptFunctions(false);
	//Set all functions and constructed objects to null
	ConnectDisconnectScriptFunctions = null;
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	ExperimentStarted = null;
	ExperimentStopped = null;	
	EM_ExternalTriggerIncremented = null;
	VisualizeExperimentStructure = null;
	UpdateExperiment = null;
	cExperimentStructure_Object = null;
	ExperimentStructureState_Object = null;
	cBlockStructure_Object = null;
	RandomGeneratorObj = null;
	CleanupScript = null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script cleanup, ready for garbage collection!");
	BrainStim.cleanupScript();
}

function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);  
			//This signal is emitted whenever the EM changes its state, we'll use it to detect changes of the current Block/Trial/Trigger
		} 
		catch (e) 
		{
			Log(".*. Something went wrong connecting the Signal/Slots:" + e); //If a connection fails warn the user!
		}		
	}
	else
	{
		Log("... Disconnecting Signal/Slots");
		try 
		{	
			ExperimentManagerObj.ExperimentStateHasChanged.disconnect(this, this.ExperimentStateChanged);
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}	
}

function ExperimentStarted()
{
	Log("-*- Experiment Started");
}

function ExperimentStopped()
{
	Log("-*- Experiment Stopped");
}

function EM_ExternalTriggerIncremented() 
{
	Log("*** EM_ExternalTriggerIncremented()");
	ExperimentStructureState_Object = cExperimentStructure_Object.getCurrentExperimentState();
	UpdateExperiment(ExperimentStructureState_Object);
	VisualizeExperimentStructure(ExperimentStructureState_Object);
}

function UpdateExperiment(currentExperimentState)
{
	cBlockStructure_Object = cExperimentStructure_Object.getBlockPointerByID(currentExperimentState.CurrentBlock_BlockID);
	if(cBlockStructure_Object.BlockNumber == 1) //Only active for the second block
	{
		sCurrentExperimentPresentation = RandomGeneratorObj.at(currentExperimentState.CurrentBlock_TrialNumber);
	}
	else
	{
		sCurrentExperimentPresentation = "fixation";
	}
	
	if(currentExperimentState.CurrentBlock_InternalTrigger == 0)
	{
		//The below is for updating the Experiment by invoking a QML2Viewer method defined in the QML file which takes care of updating the experiment for us
		var MainItem = "Fruits_Functions";
		var functionName = "updateExperiment";
		var retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,sCurrentExperimentPresentation);
		Log("    <<< " + functionName + "(" + sCurrentExperimentPresentation + ") returned: " + retVal + " >>>");
	}
}

function VisualizeExperimentStructure(currentExperimentState)
{
	//The below is for logging the current Experiment Structure to the Log Output Pane
	Log("-----");
//	var tmpLoopID = currentExperimentState.CurrentBlock_LoopID;
//	if (tmpLoopID == -4)
//		Log("CurrentBlock_LoopID: RA_UNDEFINED");
//	else
//		Log("CurrentBlock_LoopID:" + currentExperimentState.CurrentBlock_LoopID);
	Log("CurrentBlock_BlockID:" + currentExperimentState.CurrentBlock_BlockID);
	Log("CurrentBlock_TrialNumber:" + currentExperimentState.CurrentBlock_TrialNumber);
	Log("CurrentBlock_InternalTrigger:" + currentExperimentState.CurrentBlock_InternalTrigger);
	Log("CurrentBlock_ExternalTrigger:" + currentExperimentState.CurrentBlock_ExternalTrigger);
	Log("Experiment_ExternalTrigger:" + currentExperimentState.Experiment_ExternalTrigger);
	//Log(cExperimentStructure_Object.getBlockCount());
	//Log(cExperimentStructure_Object.ExperimentDebugMode);
	//Log(cExperimentStructure_Object.getExperimentName());	
	cBlockStructure_Object = cExperimentStructure_Object.getBlockPointerByID(currentExperimentState.CurrentBlock_BlockID);
	Log("> The current Block (BlockNumber: " + cBlockStructure_Object.BlockNumber + ", BlockName: " + cBlockStructure_Object.BlockName + ")");	
	Log("-----");
	
	//The below is for logging the current Experiment Structure to the QML2Viewer Window
	var MainItem = "DebugMode_Functions";
	var functionName = "updateExperimentState";
	var retVal = QML2Viewer_Object_1.invokeQml2Method(MainItem,functionName,cBlockStructure_Object.BlockNumber,currentExperimentState.CurrentBlock_TrialNumber,currentExperimentState.CurrentBlock_InternalTrigger);
	Log("    <<< " + functionName + " returned: " + retVal + " >>>");	
}

function ExperimentStateChanged(currentState)
{
//The signal where this slot is connected to contains 1 parameters (that holds the current Experiment Manager State).
	
	Log(ExperimentManagerObj.getCurrentExperimentState());	
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_IsStarting)//ExperimentManager_IsStarting
	{
		cExperimentStructure_Object = ExperimentManagerObj.getExperimentStructure();
		cExperimentStructure_Object.experimentStarted.connect(this, ExperimentStarted);
		cExperimentStructure_Object.experimentStopped.connect(this, ExperimentStopped);
		cExperimentStructure_Object.externalTriggerRecieved.connect(this, EM_ExternalTriggerIncremented);		
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)//ExperimentManager_Stopped
	{
		cExperimentStructure_Object.experimentStarted.disconnect(this, ExperimentStarted);
		cExperimentStructure_Object.experimentStopped.disconnect(this, ExperimentStopped);
		cExperimentStructure_Object.externalTriggerRecieved.disconnect(this, EM_ExternalTriggerIncremented);
		CleanupScript();
		//Whenever the experiment is stopped (or aborted) we make sure here that we'll end the script nicely
	}	
}


//Let's fill and reandomize the Experiment Presentation Array
for (var i=0;i<10;i++)//in the used EXML file there are 10 trials (<nr_of_trials>10</nr_of_trials>) defined in the Fruits block (<block_number>1</block_number>) so we'll make sure to fill the array with 10 items.
{	
	if(i<5)
		sExperimentPresentationArray[i] = "banana";
	else
		sExperimentPresentationArray[i] = "acai";
}
RandomGeneratorObj = new RandomGenerator(sExperimentPresentationArray);
RandomGeneratorObj.randomize(0);
Log(RandomGeneratorObj.toStringList());


ConnectDisconnectScriptFunctions(true); //Connect the signal/slots
ExperimentManagerObj.setExperimentFileName(currentScriptLocation + "/BlockDesign2.exml"); //Set the experiment file
if(!ExperimentManagerObj.runExperiment()) //Try to run the experiment
	CleanupScript(); //If the experiment fails to run exit the script nicely
