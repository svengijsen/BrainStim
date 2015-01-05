// This example shows how you can make use of the Experiment Manager.

var ExperimentManagerObj = new ExperimentManager(); 				//Here we create the Experiment Manager object that can run experiments.
var currentScriptLocation = StimulGL.getActiveDocumentFileLocation();	//Here we store the directory-path from this script file for further usage.
var functionName;
var MainItem;
var Param1 = "";
var retVal;
var textToShow;

function CleanupScript() //Cleanup the script
{
	//Disconnect the signal/slots
	ConnectDisconnectScriptFunctions(false);
	//Set all functions and constructed objects to null
	ConnectDisconnectScriptFunctions = null;
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	ExperimentStructureChanged = null;
	//NewInitBlockTrial = null;
	//ExternalTriggerIncremented = null;
	//NewSourceLoaded = null;
	CleanupScript = null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script cleanup, ready for garbage collection!");
	StimulGL.cleanupScript();
}

function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			ExperimentManagerObj.WriteToLogOutput.connect(this,this.Log);
			//This signal emits usefull and extensive debugging information, we'll connect it to the Log function to make sure that we can see it in the Log output Pane			
			qmlWidget_Object_1.ExperimentStructureChanged.connect(this, this.ExperimentStructureChanged);
			//This signal is emitted whenever the internal Experiment Structure as defined in the EXML file changes.
			//qmlWidget_Object_1.NewInitBlockTrial.connect(this, this.NewInitBlockTrial);
			//This signal is emitted whenever the internal Experiment Structure changes to new Block or Trial.
			//qmlWidget_Object_1.ExternalTriggerIncremented.connect(this, this.ExternalTriggerIncremented);
			//This signal is emitted whenever the 'External Trigger' from the internal Experiment Structure changes.			
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
			ExperimentManagerObj.WriteToLogOutput.disconnect(this,this.Log);
			qmlWidget_Object_1.ExperimentStructureChanged.disconnect(this, this.ExperimentStructureChanged);
			//qmlWidget_Object_1.NewInitBlockTrial.disconnect(this, this.NewInitBlockTrial);
			//qmlWidget_Object_1.ExternalTriggerIncremented.disconnect(this, this.ExternalTriggerIncremented);	
			//qmlWidget_Object_1.NewSourceLoaded.disconnect(this, this.NewSourceLoaded);		
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}	
}

function ExperimentStateChanged(currentState)
{
//The signal where this slot is connected to contains 1 parameters (that holds the current Experiment Manager State).
	
	Log(ExperimentManagerObj.getCurrentExperimentState());	
	if (currentState == ExperimentManager.ExperimentState.ExperimentManager_Configured)
	{
		//because the used qmlWidget_Object_1 object (used in the below ConnectDisconnectScriptFunctions() function) is declared in the EXML file and is created by the Experiment Manager DURING the configuration we make sure to call the connect function AFTER the configuration (ExperimentManager_Configured)!
		ConnectDisconnectScriptFunctions(true); //Connect the signal/slots
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{	
		//qmlWidget_Object_1.NewSourceLoaded.connect(this, this.NewSourceLoaded);
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		CleanupScript();
		//Whenever the experiment is stopped (or aborted) we make sure here that we'll end the script nicely
	}	
}

//function NewInitBlockTrial()
//{
	//Log("--- Starting to initialize a new Block Trial (" + arguments.length + " argument(s))");
//}

//function ExternalTriggerIncremented()
//{
	//Log("--- The External Trigger is incremented (" + arguments.length + " argument(s): " + arguments[0] +")");	
//}

//function NewSourceLoaded(sFileURL)
//{
	//Log("--- A new QML source was loaded (" + sFileURL + ")");	
//}

function ExperimentStructureChanged(nCurrentBlock,nCurrentTrial,nCurrentTrigger)
{
	textToShow = "Block " + nCurrentBlock + ", Trial " + nCurrentTrial + ", Trigger " + nCurrentTrigger;
	Log("--- The Experiment Structure Changed to: " + textToShow);

	if(nCurrentTrigger == 0)
	{
		MainItem = "functions";
		functionName = "setTextFunction";
		Param1 = Param1 + "\n" + nCurrentBlock + ", " + nCurrentTrial;
		retVal = qmlWidget_Object_1.invokeQml2Method(MainItem,functionName,Param1);
		Log("    <<< " + functionName + "(" + Param1 + ") returned: " + retVal + " >>>");	
	}
	
	MainItem = "DebugMode_Functions";
	functionName = "updateExperimentState";
	retVal = qmlWidget_Object_1.invokeQml2Method(MainItem,functionName,nCurrentBlock,nCurrentTrial,nCurrentTrigger);
	Log("    <<< " + functionName + " returned: " + retVal + " >>>");
	
}

ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);  
//This signal is emitted whenever the EM changes its state, we'll use it to detect changes of the current Block/Trial/Trigger
ExperimentManagerObj.setExperimentFileName(currentScriptLocation + "/Example1.exml"); //Set the experiment file
if(!ExperimentManagerObj.runExperiment()) //Try to run the experiment
	CleanupScript(); //If the experiment fails to run exit the script nicely