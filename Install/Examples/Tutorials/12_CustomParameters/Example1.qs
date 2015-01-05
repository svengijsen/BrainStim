// This example shows how you can make use of the Experiment Manager.

var ExperimentManagerObj = new ExperimentManager(); 				//Here we create the Experiment Manager object that can run experiments.
var currentScriptLocation = StimulGL.getActiveDocumentFileLocation();	//Here we store the directory-path from this script file for further usage.
var functionName;
var MainItem;
var Param1;
var Param2;
var Param3;
var retVal;
var textToShow;

//CustomParameters
//Following are referred to by a default parameter enclosed with the {} in the EXML file
var qmlFilePath = "./Example1.qml"; //referred to by a parameter enclosed with {} in EXML file
var StimulusWidthSpan = 768;
var StimulusHeightSpan = StimulusWidthSpan;
//Following are referred to by a custom parameter enclosed with the {} in the EXML file
var CustomParam2 = "CustomParam2 value."

function CleanupScript() //Cleanup the script
{
	//Disconnect the signal/slots
	ConnectDisconnectScriptFunctions(false);
	//Set all functions and constructed objects to null
	ConnectDisconnectScriptFunctions = null;
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	ExperimentStructureChanged = null;
	NewInitBlockTrial = null;
	ExternalTriggerIncremented = null;	
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
			qmlWidget_Object_1.NewInitBlockTrial.connect(this, this.NewInitBlockTrial);
			//This signal is emitted whenever the internal Experiment Structure changes to new Block or Trial.
			qmlWidget_Object_1.ExternalTriggerIncremented.connect(this, this.ExternalTriggerIncremented);
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
			qmlWidget_Object_1.NewInitBlockTrial.disconnect(this, this.NewInitBlockTrial);
			qmlWidget_Object_1.ExternalTriggerIncremented.disconnect(this, this.ExternalTriggerIncremented);			
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
		qmlWidget_Object_1.NewSourceLoaded.connect(this, this.NewSourceLoaded);
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		CleanupScript();
		//Whenever the experiment is stopped (or aborted) we make sure here that we'll end the script nicely
	}	
}

function NewInitBlockTrial()
{
	Log("--- Starting to initialize a new Block Trial (" + arguments.length + " argument(s))");
}

function ExternalTriggerIncremented()
{
	Log("--- The External Trigger is incremented (" + arguments.length + " argument(s): " + arguments[0] +")");	
}

function NewSourceLoaded(sFileURL)
{
	Log("--- A new QML source was loaded (" + sFileURL + ")");	
}

function ExperimentStructureChanged(nCurrentBlock,nCurrentTrial,nCurrentTrigger)
{
	textToShow = "Block " + nCurrentBlock + ", Trial " + nCurrentTrial + ", Trigger " + nCurrentTrigger;
	Log("--- The Experiment Structure Changed to: " + textToShow);


	Log("\n>>>>>");
		//Log(">>>>> registerCustomExperimentObjectParameter: " + RetinoMap_Object_1.registerCustomExperimentObjectParameter(1,"customparam1","value1",0,true));
	Log(">>>>> getExperimentObjectParameter(CustomParam1): " + qmlWidget_Object_1.getExperimentObjectParameter(0, "CustomParam1"));
	Log(">>>>> getExperimentObjectParameter(CustomParam2): " + qmlWidget_Object_1.getExperimentObjectParameter(0, "CustomParam2"));
	if((nCurrentBlock == 0) && (nCurrentTrial == 0) && (nCurrentTrigger == 0))
	{
		Log(">>>>>insertExpObjectBlockParameter(CustomParam3 = '{CustomParam2}') returned: " + qmlWidget_Object_1.insertExpObjectBlockParameter(0,"CustomParam3","{CustomParam2}",true,true));//This custom parameter is applied at the next Block or Trial and stays buffered
		Log(">>>>> setExperimentObjectParameter(CustomParam2 = 'xxx'): " + qmlWidget_Object_1.setExperimentObjectParameter(0,"CustomParam2", "xxx"));//This is applied Immediately, but is not Buffered for the next BlockTrial...!
		Log(">>>>> insertExpObjectBlockParameter(CustomParam2 = 'yyy'): " + qmlWidget_Object_1.insertExpObjectBlockParameter(0,"CustomParam2", "yyy",true,true));//This custom parameter is applied at the next Block or Trial and stays buffered
	}
		//bool insertExpObjectBlockParameter(const int nObjectID,const QString sName,const QString sValue,bool bIsInitializing = true, bool bIsCustom = false);
	//if(nCurrentBlock == 1)
	Log(">>>>> getExperimentObjectParameter(CustomParam3): " + qmlWidget_Object_1.getExperimentObjectParameter(0, "CustomParam3"));
	CustomParam2 = CustomParam2 + "*";
	Log(">>>>>\n");
	
	
	MainItem = "functions";
	functionName = "setTextFunction";
	Param1 = qmlWidget_Object_1.getExperimentObjectParameter(0, "CustomParam1");
	Param2 = qmlWidget_Object_1.getExperimentObjectParameter(0, "CustomParam2");
	Param3 = qmlWidget_Object_1.getExperimentObjectParameter(0, "CustomParam3");
	if(Param3 == null)
		Param3 = "Undefined";
	retVal = qmlWidget_Object_1.invokeQml2Method(MainItem,functionName,"CustomParam1 is: " + Param1,"CustomParam2 is: " + Param2,"CustomParam3 is: " + Param3);
	Log("    <<< " + functionName + "(" + Param1 + ") returned: " + retVal + " >>>");	
	
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