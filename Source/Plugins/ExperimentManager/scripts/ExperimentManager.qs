var ExperimentManagerObj = new ExperimentManager();

function CleanupScript()
{
	ConnectDisconnectScriptFunctions(false);
	ConnectDisconnectScriptFunctions = null;
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	ExperimentStructureChanged = null;
	CleanupScript = null;
	BrainStim.cleanupScript();
	Log("CleanupScript() exited successfully")
}

function ConnectDisconnectScriptFunctions(Connect)
{
	if(Connect)
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);
			ExperimentManagerObj.WriteToLogOutput.connect(this,this.Log);//For more extensive debugging information
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
			ExperimentManagerObj.WriteToLogOutput.disconnect(this,this.Log);//For more extensive debugging information
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}

function ExperimentStructureChanged()
{
//	Log("--- ExperimentStructureChanged arguments count: " + arguments.length);
//	for (var i = 0; i < arguments.length; ++i)
//		Log("\t argument(" + i + "):" + arguments[i]);
	
	var nCurrentBlock = arguments[0];
	var nCurrentTrial = arguments[1];
	var nCurrentTrigger = arguments[2];
	var textToShow = "->-> Block " + nCurrentBlock + ", Trial " + nCurrentTrial + ", Trigger " + nCurrentTrigger;		
	Log(textToShow);
}

function ExperimentStateChanged(currState)
{
	if(currState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{	
		Log("--- ExperimentManager_Initialized(" + arg + ")");
	}	
	else if(currState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		Log("--- ExperimentManager_Stopped(" + arg + ")");
		CleanupScript();
	}
}

ConnectDisconnectScriptFunctions(true);
ExperimentManagerObj.setExperimentFileName(BrainStim.getApplicationRootDirPath() + "/examples/experiments/Polar1.exml");
if(!ExperimentManagerObj.runExperiment())
	CleanupScript();

//Polar1.exml
//Polar2.exml
//Eccentricity1.exml
//Eccentricity2.exml
//MovingBar1.exml
//MovingBar2.exml
//MovingDots1.exml
