var cExperimentStructure_Object = new cExperimentStructure();
var ExperimentManagerObj = new ExperimentManager(); 	
var cBlockStructure_Object0;
var cBlockStructure_Object1;
var cBlockStructure_Object2;
var cBlockStructure_Object3;
var cBlockStructure_Object4;
var cLoopStructure_Object0;
var cLoopStructure_Object1;
var cLoopStructure_Object2;
var cLoopStructure_Object3;
var cLoopStructure_Object4;
var cLoopStructure_Object = new cLoopStructure();
var cExperimentStructureState_Object = new cExperimentStructureState();
var sTimer = new TriggerTimer();
var KeyBoardCaptureObj = new KeyBoardCapture(); //Construct a BrainStim Plugin KeyBoard Object
var ExperimentManagerObj = new ExperimentManager(); 				//Here we create the Experiment Manager object that can run experiments.
var sCurrentScriptLocation = BrainStim.getSelectedScriptFileLocation();		//Here we store the directory-path from this script file for further usage.
var sExperimentFilePath = sCurrentScriptLocation + "/StructureLoading.exml";
var ExampleMode=0; // 0: Visual, 1: Counting, 2:LoadFromXML(Counting), 3:LoadFromXML(Visual)

function KeyCaptureDetectFunction(keyCode)
{
	Log("A key press was detected: " + keyCode); 
	if(keyCode == 27)//Escape
	{
		CleanupScript();
	}
	else if(keyCode == 82)//r = refresh
	{
		Log("* Refresh the Experiment Structure");
		if(!ExperimentManagerObj.showVisualExperimentEditor(cExperimentStructure_Object)) //Try to run the experiment
		{
			Log("Failed to show the Experiment Structure");
			CleanupScript(); //If the experiment fails to run exit the script nicely
		}
	}
	else if(keyCode == 67)//c = change custom
	{
		cBlockStructure_Object0.BlockName = "Custom Block Name";
		cLoopStructure_Object2 = new cLoopStructure();
		CreateLoop(cLoopStructure_Object2,2,1,cBlockStructure_Object0.getBlockID());//(LoopObject,ID,NumberOfLoops,TargetBlockID)
		Log("Add a defined Loop result: " + cBlockStructure_Object0.insertLoop(cLoopStructure_Object2));		
	}	
}

function CleanupScript()
{
	KeyBoardCaptureObj.StopCaptureThread();
	ConnectDisconnectScriptFunctions(false);
	ConnectDisconnectScriptFunctions = null;
	sTimer.stopTimer();
	cLoopStructure_Object0 = null;
	cLoopStructure_Object1 = null;
	cLoopStructure_Object2 = null;
	cLoopStructure_Object3 = null;
	cLoopStructure_Object4 = null;	
	cBlockStructure_Object0 = null;
	cBlockStructure_Object1 = null;
	cBlockStructure_Object2 = null;
	cBlockStructure_Object3 = null;
	cBlockStructure_Object4 = null;
	cExperimentStructure_Object = null;	
	cExperimentStructureState_Object = null;
	ExperimentManagerObj = null;
	CleanupScript = null;
	LogState = null;
	sTimer = null;
	ExperimentStarted = null;
	ExperimentStopped = null;
	ExperimentStateChanged = null;
	KeyCaptureDetectFunction = null;
	CreateBlock = null;
	CreateLoop = null;
	KeyBoardCaptureObj = null;
	EM_ExperimentStateChanged = null;
	EM_ExternalTriggerIncremented = null;
	ExperimentManagerObj = null;
	Log("\nFinished script cleanup, ready for garbage collection!");
	Beep();
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
			if(ExampleMode == 1)
				sTimer.timeout.connect(cExperimentStructure_Object, cExperimentStructure_Object.incrementExternalTrigger);
			cExperimentStructure_Object.experimentStarted.connect(this, ExperimentStarted);
			cExperimentStructure_Object.experimentStopped.connect(this, ExperimentStopped);
			cExperimentStructure_Object.externalTriggerRecieved.connect(this, ExperimentStateChanged);
			KeyBoardCaptureObj.CaptureThreadKeyPressed.connect(this, this.KeyCaptureDetectFunction);
			ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.EM_ExperimentStateChanged);  
			ExperimentManagerObj.WriteToLogOutput.connect(this,this.Log);
			
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
			if(ExampleMode == 1)
				sTimer.timeout.disconnect(cExperimentStructure_Object, cExperimentStructure_Object.incrementExternalTrigger);
			cExperimentStructure_Object.experimentStarted.disconnect(this, ExperimentStarted);
			cExperimentStructure_Object.experimentStopped.disconnect(this, ExperimentStopped);
			cExperimentStructure_Object.externalTriggerRecieved.disconnect(this, ExperimentStateChanged);
			KeyBoardCaptureObj.CaptureThreadKeyPressed.disconnect(this, this.KeyCaptureDetectFunction);
			ExperimentManagerObj.ExperimentStateHasChanged.disconnect(this, this.EM_ExperimentStateChanged);  
			ExperimentManagerObj.WriteToLogOutput.disconnect(this,this.Log);
	
			cExperimentStructure_Object.externalTriggerRecieved.disconnect(this, EM_ExternalTriggerIncremented);
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}	
}

function EM_ExternalTriggerIncremented() 
{
	Log("*** EM_ExternalTriggerIncremented()");
	LogState(cExperimentStructure_Object.getCurrentExperimentState());
}

function EM_ExperimentStateChanged(currentState,timestamp)
{
//The first parameter of the signal where this slot is connected to holds the current Experiment Manager State.
	
	Log("EM_ExperimentStateChanged() to:" + currentState + " @" + timestamp);
	currentEMState = currentState;
	if(currentEMState == ExperimentManager.ExperimentState.ExperimentManager_IsStarting)
	{
		cExperimentStructure_Object = ExperimentManagerObj.getExperimentStructure();
		//re-connect...
		cExperimentStructure_Object.experimentStarted.connect(this, ExperimentStarted);
		cExperimentStructure_Object.experimentStopped.connect(this, ExperimentStopped);
		cExperimentStructure_Object.externalTriggerRecieved.connect(this, EM_ExternalTriggerIncremented);
	}
	else if(currentEMState == ExperimentManager.ExperimentState.ExperimentManager_IsStopping)
	{
		//ExperimentManagerObj.showVisualExperimentEditor(cExperimentStructure_Object);
	}
	else if(currentEMState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		CleanupScript();
		//Whenever the experiment is stopped (or aborted) we make sure here that we'll end the script nicely
	}	
}

function LogState(currentExperimentState)
{
	Log("-----");
	var tmpLoopID = currentExperimentState.CurrentBlock_LoopID;
	if (tmpLoopID == -4)
		Log("CurrentBlock_LoopID: RA_UNDEFINED");
	else
		Log("CurrentBlock_LoopID:" + currentExperimentState.CurrentBlock_LoopID);
	if(cLoopStructure_Object0)
		Log("CurrentBlock_LoopCounter0:" + cLoopStructure_Object0.getLoopCounter());
	if(cLoopStructure_Object1)
		Log("CurrentBlock_LoopCounter1:" + cLoopStructure_Object1.getLoopCounter());
	if(cLoopStructure_Object2)
		Log("CurrentBlock_LoopCounter2:" + cLoopStructure_Object2.getLoopCounter());
	Log("CurrentBlock_BlockID:" + currentExperimentState.CurrentBlock_BlockID);
	Log("CurrentBlock_TrialNumber:" + currentExperimentState.CurrentBlock_TrialNumber);
	Log("CurrentBlock_InternalTrigger:" + currentExperimentState.CurrentBlock_InternalTrigger);
	Log("CurrentBlock_ExternalTrigger:" + currentExperimentState.CurrentBlock_ExternalTrigger);
	Log("Experiment_ExternalTrigger:" + currentExperimentState.Experiment_ExternalTrigger);
	Log("-----");
}

function ExperimentStateChanged()
{
	Log("-O- Experiment State Changed -O-");
	//cExperimentStructureState_Object = cExperimentStructure_Object.getCurrentExperimentState();
	LogState(cExperimentStructure_Object.getCurrentExperimentState());
}

function ExperimentStarted()
{
	Log("-*- Experiment Started");
}

function ExperimentStopped()
{
	Log("-*- Experiment Stopped");
	CleanupScript();
}

function CreateBlock(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
{
	//BlockObject = new cBlockStructure();	
	BlockObject.BlockName = "Block " + ID;
	Log("Block Name: " + BlockObject.BlockName);
	BlockObject.BlockID = ID;
	Log("Block ID: " + BlockObject.BlockID);
	BlockObject.BlockNumber = ID;
	Log("Block Number: " + BlockObject.BlockNumber);
	BlockObject.NumberOfTrials = Trials;
	Log("Block Number Of Trials: " + BlockObject.NumberOfTrials);
	BlockObject.NumberOfInternalTriggers = IntTriggers;
	Log("Block Number Of Internal Triggers: " + BlockObject.NumberOfInternalTriggers);
	BlockObject.NumberOfExternalTriggers = ExtTriggers;
	Log("Block Number Of External Triggers: " + BlockObject.NumberOfExternalTriggers);
	//return BlockObject;
}

function CreateLoop(LoopObject,ID,NumberOfLoops,TargetBlockID)
{
	//LoopObject = new cLoopStructure();	
	LoopObject.LoopName = "Loop " + ID;
	Log("Loop Name: " + LoopObject.LoopName);
	LoopObject.LoopID = ID;
	Log("Loop ID: " + LoopObject.LoopID);
	LoopObject.LoopNumber = ID;
	Log("Loop Number: " + LoopObject.LoopNumber);
	LoopObject.NumberOfLoops = NumberOfLoops;
	Log("Loop Number Of Loops: " + LoopObject.NumberOfLoops);
	LoopObject.TargetBlockID = TargetBlockID;
	Log("Loop Target Block ID: " + LoopObject.TargetBlockID);
	//return LoopObject;
}

Log();

if(ExampleMode != 2)
{
	cExperimentStructure_Object.ExperimentName = "Test Experiment";
	Log("Experiment Name: " + cExperimentStructure_Object.ExperimentName);
	cExperimentStructure_Object.ExperimentID = 99;
	Log("Experiment ID: " + cExperimentStructure_Object.ExperimentID);
	cExperimentStructure_Object.ExperimentDebugMode = true;
	Log("Experiment Debug Mode: " + cExperimentStructure_Object.ExperimentDebugMode);

	cBlockStructure_Object0 = new cBlockStructure();
	CreateBlock(cBlockStructure_Object0,0,2,5,2);//(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
	Log("Add a defined Block result: " + cExperimentStructure_Object.insertBlock(cBlockStructure_Object0));

	cBlockStructure_Object1 = new cBlockStructure();
	CreateBlock(cBlockStructure_Object1,1,1,1,1);//(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
	Log("Add a defined Block result: " + cExperimentStructure_Object.insertBlock(cBlockStructure_Object1));
	cBlockStructure_Object2 = new cBlockStructure();
	CreateBlock(cBlockStructure_Object2,2,1,1,1);//(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
	Log("Add a defined Block result: " + cExperimentStructure_Object.insertBlock(cBlockStructure_Object2));
	
	cLoopStructure_Object0 = new cLoopStructure();
	CreateLoop(cLoopStructure_Object0,0,2,cBlockStructure_Object0.getBlockID());//(LoopObject,ID,NumberOfLoops,TargetBlockID)
	Log("Add a defined Loop result: " + cBlockStructure_Object1.insertLoop(cLoopStructure_Object0));
	Log("\tLoop Count: " + cBlockStructure_Object1.getLoopCount());
	
	//cLoopStructure_Object1 = new cLoopStructure();
	//CreateLoop(cLoopStructure_Object1,1,3,cBlockStructure_Object0.getBlockID());//(LoopObject,ID,NumberOfLoops,TargetBlockID)
	//Log("Add a defined Loop result: " + cBlockStructure_Object1.insertLoop(cLoopStructure_Object1));
	//Log("\tLoop Count: " + cBlockStructure_Object1.getLoopCount());
	
	//cLoopStructure_Object2 = new cLoopStructure();
	//CreateLoop(cLoopStructure_Object2,2,1,cBlockStructure_Object0.getBlockID());//(LoopObject,ID,NumberOfLoops,TargetBlockID)
	//Log("Add a defined Loop result: " + cBlockStructure_Object0.insertLoop(cLoopStructure_Object2));

	
	
	
	
	//cBlockStructure_Object.BlockName = "Block 0";
	//Log("Block Name: " + cBlockStructure_Object.BlockName);
	//cBlockStructure_Object.BlockID = 0;
	//Log("Block ID: " + cBlockStructure_Object.BlockID);
	//cBlockStructure_Object.BlockNumber = 0;
	//Log("Block Number: " + cBlockStructure_Object.BlockNumber);
	//cBlockStructure_Object.NumberOfTrials = 2;
	//Log("Block Number Of Trials: " + cBlockStructure_Object.NumberOfTrials);
	//cBlockStructure_Object.NumberOfInternalTriggers = 2;
	//Log("Block Number Of Internal Triggers: " + cBlockStructure_Object.NumberOfInternalTriggers);
	//cBlockStructure_Object.NumberOfExternalTriggers = 2;
	//Log("Block Number Of External Triggers: " + cBlockStructure_Object.NumberOfExternalTriggers);

	//cLoopStructure_Object.LoopName = "Test Loop";
	//Log("Loop Name: " + cLoopStructure_Object.LoopName);
	//cLoopStructure_Object.LoopID = 4;
	//Log("Loop ID: " + cLoopStructure_Object.LoopID);
	//cLoopStructure_Object.LoopNumber = 0;
	//Log("Loop Number: " + cLoopStructure_Object.LoopNumber);
	//cLoopStructure_Object.NumberOfLoops = 2;
	//Log("Loop Number Of Loops: " + cLoopStructure_Object.NumberOfLoops);

	//Log("Add a defined Block result: " + cExperimentStructure_Object.insertBlock(cBlockStructure_Object));
	//cBlockStructure_Object1 = cBlockStructure_Object;//Do not do this! Original is overwritten!
	//cBlockStructure_Object1.BlockID = 77;
	//cBlockStructure_Object1.BlockNumber = 2;
	//cBlockStructure_Object1.NumberOfInternalTriggers = 2;
	//cBlockStructure_Object1.NumberOfExternalTriggers = 1;
	//Log("Block Name: " + cBlockStructure_Object1.BlockName);//This returns the default name if not provided
	//Log("Add a defined Block result: " + cExperimentStructure_Object.insertBlock(cBlockStructure_Object1));
	//Log();

	//LogState(cExperimentStructureState_Object);
	//LogState(cExperimentStructure_Object.getCurrentExperimentState());
	
//	cExperimentStructure_Object.resetExperiment();
//	if(cExperimentStructure_Object.prepareExperiment())
//	{
//		Log("Experiment Prepared");
		//cExperimentStructureState_Object = cExperimentStructure_Object.getCurrentExperimentState();
		//LogState(cExperimentStructureState_Object);
//	}
//	else
//	{
//		Log("Prepare Experiment failed!");
//	}	
}

//BrainStim.clearOutputWindow();
ConnectDisconnectScriptFunctions(true);


if(ExampleMode == 0)
{
	Log("Ëxample Mode = " + ExampleMode);
	if(!ExperimentManagerObj.showVisualExperimentEditor(cExperimentStructure_Object)) //Try to run the experiment
	{
		Log("Failed to show the Experiment Structure");
		CleanupScript(); //If the experiment fails to run exit the script nicely
	}	
	KeyBoardCaptureObj.StartCaptureThread(0, true);
}
else if(ExampleMode == 1)
{
	Log("Ëxample Mode = " + ExampleMode);
	sTimer.startTimer(200);
	KeyBoardCaptureObj.StartCaptureThread(0, true);
}
else if(ExampleMode == 2)
{
	Log("Ëxample Mode = " + ExampleMode);
	ExperimentManagerObj.setExperimentFileName(sExperimentFilePath); //Set the experiment file
	if(!ExperimentManagerObj.runExperiment()) //Try to run the experiment
	{
		Log("Failed to execute the Experiment");
		CleanupScript(); //If the experiment fails to run exit the script nicely
	}
}
else if(ExampleMode == 3)
{
	Log("Ëxample Mode = " + ExampleMode);
	ExperimentManagerObj.setExperimentFileName(sExperimentFilePath); //Set the experiment file
	if(!ExperimentManagerObj.showVisualExperimentEditor()) //Try to run the experiment
	{
		Log("Failed to show the Experiment Structure");
		CleanupScript(); //If the experiment fails to run exit the script nicely
	}
	//if(!ExperimentManagerObj.runExperiment()) //Try to run the experiment
	//{
	//	Log("Failed to execute the Experiment");
	//	CleanupScript(); //If the experiment fails to run exit the script nicely
	//}	
}


