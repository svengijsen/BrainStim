//Includes:
Include("QtScript/BasicQMLExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/

//Included variables:
BasicExperiment.sExmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "CustomParameters.exml";
BasicExperiment.nTestModeTriggerDuration = 1000;

//Custom script references (variables):
var QML2Viewer_Global_QmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "CustomParameters.qml";
var _CustomParam2_ScriptRef = "_CustomParam2 value!"
var _ScriptRef_1 = "Script refered value 1";

//Override Public Functions from the BasicQMLExperiment.qs include (that includes BasicExperiment):
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;

	Log("\n");
	BrainStim.write2OutputWindow("*** Going to initialize a new Block-trial: BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber);
	if(_currentBlockNumber == 0)
	{
		if(_currentTrialNumber == 1)
		{
			//The following change of an existing custom parameter value (using the setExperimentObjectParameter() method) is
			//not buffered and thus only valid for this BlockTrial (see last argument)! If the last parameter is set to true then it stays
			//buffered untill the parameter value is changed again by the ExperimentStructure. This is the case for the second trial of this block!
			Log(">>>>> setExperimentObjectParameter(_CustomParam1): " + BasicExperiment.QMLViewerObject.setExperimentObjectParameter(0,"_CustomParam1","New Value 1",false));
			//The following inserts a new non-existing custom parameter value, this value stays buffered untill the parameter value is changed again by our custom script,
			//the value is immediately parsed for the current Block Trial (see last argument) .
			Log(">>>>> insertExpObjectBlockParameter(_NewParam1): " + BasicExperiment.QMLViewerObject.insertExpObjectBlockParameter(0,"_NewParam1","Inserted value",true,true,true));
		}
		else if(_currentTrialNumber == 2)
		{
			//Although we change  the script referenced value  below we must wait for the next Block Trial for the result to be parsed
			_CustomParam2_ScriptRef = "_CustomParam2 value changed!"
		}		
	}
	else if(_currentBlockNumber == 1)
	{
		if(_currentTrialNumber == 0)
		{			
			Log(">>>>> setExperimentObjectParameter(_NewParam1): " + BasicExperiment.QMLViewerObject.setExperimentObjectParameter(0,"_NewParam1","Inserted value changed",true));
		}
		else if(_currentTrialNumber == 1)
		{
			Log(">>>>> setExperimentObjectParameter(_CustomParam2): " + BasicExperiment.QMLViewerObject.setExperimentObjectParameter(0,"_CustomParam2","_CustomParam2 value changed again!",true));
		}
	}
	else if(_currentBlockNumber == 2)
	{
		if(_currentTrialNumber == 0)
		{
			//The following change of an existing custom parameter value (using the setExperimentObjectParameter() method) is
			//buffered (see last argument) untill the parameter value is changed again by the ExperimentStructure.			
			Log(">>>>> setExperimentObjectParameter(_CustomParam1): " + BasicExperiment.QMLViewerObject.setExperimentObjectParameter(0,"_CustomParam1","{_ScriptRef_1}",true));
		}
		else if(_currentTrialNumber == 1)
		{
			//The following script ref is still buffered and used  by the ExperimentStructure so we can change it here.
			_ScriptRef_1 = "Script refered value 1 changed";	
		}
		else if(_currentTrialNumber == 2)
		{
			Log(">>>>> setExperimentObjectParameter(_CustomParam1): " + BasicExperiment.QMLViewerObject.setExperimentObjectParameter(0,"_CustomParam1","{3+4}",false));
		}		
	}
	Log("\n");
	
	var tmpValueString1 = BasicExperiment.QMLViewerObject.getExperimentObjectParameter(0, "_CustomParam1");
	var tmpValueString2 = BasicExperiment.QMLViewerObject.getExperimentObjectParameter(0, "_NewParam1");
	var tmpValueString3 = BasicExperiment.QMLViewerObject.getExperimentObjectParameter(0, "_CustomParam2");	
	retVal = BasicExperiment.InvokeQMLFunction("functions", "setTextFunction", "_CustomParam1: " + tmpValueString1,"_NewParam1 is: " + tmpValueString2,"_CustomParam2 is: " + tmpValueString3);
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

//Run the experiment:
BasicExperiment.RunExperiment();

















// This example shows how you can make use of the Experiment Manager.

//var ExperimentManagerObj = new ExperimentManager(); 				//Here we create the Experiment Manager object that can run experiments.
//var currentScriptLocation = BrainStim.getActiveDocumentFileLocation();	//Here we store the directory-path from this script file for further usage.
//var functionName;
//var MainItem;
//var Param1;
//var Param2;
//var Param3;
//var retVal;
//var textToShow;

//_CustomParameters
//Following are referred to by a default parameter enclosed with the {} in the EXML file
//var qmlFilePath = "./_CustomParameters.qml"; //referred to by a parameter enclosed with {} in EXML file
//Following are referred to by a custom parameter enclosed with the {} in the EXML file


//function CleanupScript() //Cleanup the script
//{
	//Disconnect the signal/slots
//	ConnectDisconnectScriptFunctions(false);
	//Set all functions and constructed objects to null
//	ConnectDisconnectScriptFunctions = null;
//	ExperimentManagerObj = null;
//	ExperimentStateChanged = null;
//	ExperimentStructureChanged = null;
//	NewInitBlockTrial = null;
//	ExternalTriggerIncremented = null;	
//	CleanupScript = null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
//	Log("Finished script cleanup, ready for garbage collection!");
//	BrainStim.cleanupScript();
//}
//
//function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
//{
//	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
//	{
//		Log("... Connecting Signal/Slots");
//		try 
//		{	
//			ExperimentManagerObj.WriteToLogOutput.connect(this,this.Log);
			//This signal emits usefull and extensive debugging information, we'll connect it to the Log function to make sure that we can see it in the Log output Pane			
//			qmlWidget_Object_1.ExperimentStructureChanged.connect(this, this.ExperimentStructureChanged);
			//This signal is emitted whenever the internal Experiment Structure as defined in the EXML file changes.
//			qmlWidget_Object_1.NewInitBlockTrial.connect(this, this.NewInitBlockTrial);
			//This signal is emitted whenever the internal Experiment Structure changes to new Block or Trial.
//			qmlWidget_Object_1.ExternalTriggerIncremented.connect(this, this.ExternalTriggerIncremented);
			//This signal is emitted whenever the 'External Trigger' from the internal Experiment Structure changes.			
//		} 
//		catch (e) 
//		{
//			Log(".*. Something went wrong connecting the Signal/Slots:" + e); //If a connection fails warn the user!
//		}		
//	}
//	else
//	{
//		Log("... Disconnecting Signal/Slots");
//		try 
//		{	
//			ExperimentManagerObj.ExperimentStateHasChanged.disconnect(this, this.ExperimentStateChanged);
//			ExperimentManagerObj.WriteToLogOutput.disconnect(this,this.Log);
//			qmlWidget_Object_1.ExperimentStructureChanged.disconnect(this, this.ExperimentStructureChanged);
//			qmlWidget_Object_1.NewInitBlockTrial.disconnect(this, this.NewInitBlockTrial);
//			qmlWidget_Object_1.ExternalTriggerIncremented.disconnect(this, this.ExternalTriggerIncremented);			
//		} 
//		catch (e) 
//		{
//			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
//		}		
//	}	
//}



