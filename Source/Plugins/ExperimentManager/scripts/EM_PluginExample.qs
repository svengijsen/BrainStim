var ExperimentManagerObj = new ExperimentManager();

function CleanupScript()
{
	ConnectDisconnectScriptFunctions(false);
	ExperimentManagerObj = null;
	ExperimentStateChanged = null;
	SignalCaptureFunction = null;
	ExperimentStructureChanged = null;
	ConnectDisconnectScriptFunctions = null;
	CleanupScript = null;
	BrainStim.cleanupScript();
}

function SignalCaptureFunction()
{
	Log("--- SignalCaptureFunction arguments count: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
		Log("\t argument(" + i + "):" + arguments[i]); 	
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
	
	try 
	{		
		if((nCurrentBlock == 0))
		{
//			var retVal;
//			var retVal2 = new QColor;
//			var functionName = "myQmlFunction";
//			var MainItem = "functions";
//			var Param1 = "input Param1";//new QString("input Param1");
//			retVal2 = qmlWidget_Object_1.invokeQmlMethod(MainItem,functionName,Param1);
//			Log("<<< " + functionName + " returned: " + retVal2.name() + " >>>");
//			
//			functionName = "myQmlTextFunction";
//			retVal = qmlWidget_Object_1.invokeQmlMethod(MainItem,functionName,Param1);
//			Log("<<< " + functionName + " returned: " + retVal + " >>>");
			
			MainItem = "DebugMode_Functions";
			functionName = "updateExperimentState";
			retVal = qmlWidget_Object_1.invokeQmlMethod(MainItem,functionName,nCurrentBlock,nCurrentTrial,nCurrentTrigger);
			//Log("<<< " + functionName + " returned: " + retVal + " >>>");
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

function ExperimentStateChanged(currState)
{
	if(currState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{	
		Log("--- ExperimentManager_Initialized(" + arg + ")");
		//Now all defined objects in the experiment file are also available in the script,
		//make sure to use the right name!
		ConnectDisconnectScriptFunctions(true);	
		RetinoMap_RenderWidgetGL.installCustomScriptHandlerFunction("");	
	}	
	else if(currState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		Log("--- ExperimentManager_Stopped(" + arg + ")");
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
			qmlWidget_Object_1.NewInitBlockTrial.connect(this, this.SignalCaptureFunction);
			qmlWidget_Object_1.ExternalTriggerIncremented.connect(this, this.SignalCaptureFunction);
			qmlWidget_Object_1.ExperimentStructureChanged.connect(this, this.ExperimentStructureChanged);
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
			qmlWidget_Object_1.NewInitBlockTrial.disconnect(this, this.SignalCaptureFunction);
			qmlWidget_Object_1.ExternalTriggerIncremented.disconnect(this, this.SignalCaptureFunction);
			qmlWidget_Object_1.ExperimentStructureChanged.disconnect(this, this.ExperimentStructureChanged);
			//RetinoMap_RenderWidgetGL.NewInitBlockTrial.disconnect(this, this.SignalCaptureFunction);
			//RetinoMap_RenderWidgetGL.ExternalTriggerIncremented.disconnect(this, this.SignalCaptureFunction);
			//RetinoMap_RenderWidgetGL.ExperimentStructureChanged.disconnect(this, this.ExperimentStructureChanged);
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}



ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.ExperimentStateChanged);
ExperimentManagerObj.setExperimentFileName(BrainStim.getApplicationRootDirPath() + "/examples/MediaPlayer/PluginExample.exml");
ExperimentManagerObj.runExperiment();
