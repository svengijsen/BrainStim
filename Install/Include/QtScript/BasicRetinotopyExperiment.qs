///////////
//Includes//
///////////
Include("QtScript/BasicExperiment.qs");
////////////////////////
//Prepare Output Window//
////////////////////////
BrainStim.addOutputWindow("BasicRetinotopyExperiment");
BrainStim.clearOutputWindow("BasicRetinotopyExperiment");

//////////////////////////
//Public Property Variabeles//
//////////////////////////
//Retinotopy Experiment Objects
BasicExperiment.RetinotopyMapperObj = null;
	
///////////////////////////
//Private Property Variabeles//
///////////////////////////

/////////////////////////
//Public Property Functions//
/////////////////////////

//*!This wrapper function uses the wrap method because the original function has more than one parameter
BasicExperiment.__proto__.preExperimentStateChanged = _.wrap(BasicExperiment.__proto__.preExperimentStateChanged, function(OriginalFunction,currentState,sDateTimeStamp) 
{	
	//*!Remove the first element and store the arguments in a seperate args array
	var args = [].slice.call(arguments);
	[].shift.apply(args);
	
	//ExperimentManager_NoState			= 0, /*!< this state is only internally used by the object while it is constructing, thus not yet fully available and therefore cannot be detected/used in the script environment. */
	//ExperimentManager_Constructed		= 1, /*!< this state is used to determine whenever the object has been constructed, this state also cannot be detected/used in the script environment. */
	//ExperimentManager_Loaded			= 2, /*!< after the Experiment file (*.exml) is successfully loaded (and validated) the ExperimentManager state changes to this state.*/
	//ExperimentManager_PreParsed		= 3, /*!< when the Experiment could be successfully preparsed (after loading) then the ExperimentManager state changes to this state.*/
	//ExperimentManager_Configured		= 4, /*!< after the function runExperiment() is called the ExperimentManager (validates if it has not been done before) and configures the experiment, it then changes to this state if the configuration was done.*/
	//ExperimentManager_Initialized		= 5, /*!< after the Experiment configuration the ExperimentManager tries to initialized and changes the state to this state if this step was successful.*/
	//ExperimentManager_IsStarting		= 6, /*!< after the Experiment initialization the ExperimentManager is ready to actually start the Experiment but first it changes the state to this state.*/
	//ExperimentManager_Started			= 7, /*!< after the Experiment is successfully started the ExperimentManager changes to this state.*/
	//ExperimentManager_IsStopping		= 8, /*!< before the ExperimentManager tries to stop the Experiment it first changes to this state.*/
	//ExperimentManager_Stopped			= 9  /*!< after the ExperimentManager could finalize and stop the Experiment it changes to this state.*/
	
	//*!Call the below BasicExperiment.LogFunctionSignature method with the args variabele
	BasicExperiment.LogFunctionSignature("BasicRetinotopyExperiment","preExperimentStateChanged", args, true);
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{
		//Now all defined objects in the experiment file are constructed and therefore available in this script, so now we can make the connections between constructed the objects.
		BasicExperiment.RetinotopyMapperObj = RetinoMap_Object_1;
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		BasicExperiment.RetinotopyMapperObj = null;
	}
	
	//*!Call the original function (wrap method, with more that one arguments) and return
	var bResult = OriginalFunction.apply(this,args);	
	return bResult;
})

BasicExperiment.__proto__.StartExperimentObjects = _.compose(BasicExperiment.StartExperimentObjects, function()
{
	BasicExperiment.LogFunctionSignature("BasicRetinotopyExperiment","StartExperimentObjects", arguments, true);
	if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	{
		BasicExperiment.TriggerTimerObj.timeout.connect(BasicExperiment.RetinotopyMapperObj, BasicExperiment.RetinotopyMapperObj.incrementExternalTrigger);
	}
	if(BasicExperiment.ParallelPortCapture_Enabled)
	{
		BasicExperiment.ParallelPortObj.CaptureThreadTriggered.connect(BasicExperiment.RetinotopyMapperObj, BasicExperiment.RetinotopyMapperObj.incrementExternalTrigger);
	}
	return;
})

BasicExperiment.__proto__.StopExperimentObjects = _.compose(BasicExperiment.StopExperimentObjects, function()
{
	BasicExperiment.LogFunctionSignature("BasicRetinotopyExperiment","StopExperimentObjects", arguments, true);
	if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	{	
		BasicExperiment.TriggerTimerObj.timeout.disconnect(BasicExperiment.RetinotopyMapperObj, BasicExperiment.RetinotopyMapperObj.incrementExternalTrigger);
	}
	if(BasicExperiment.ParallelPortCapture_Enabled)
	{
		BasicExperiment.ParallelPortObj.CaptureThreadTriggered.disconnect(BasicExperiment.RetinotopyMapperObj, incrementExternalTrigger);
	}
	return;
})

BasicExperiment.__proto__.ConnectDisconnectScriptFunctions = _.compose(BasicExperiment.ConnectDisconnectScriptFunctions, function(Connect)
{
	BasicExperiment.LogFunctionSignature("BasicRetinotopyExperiment","ConnectDisconnectScriptFunctions", arguments, true);
	if(Connect)
	{
		try 
		{	
			if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
			{
				if(BasicExperiment.RetinotopyMapperObj)
				{
					BasicExperiment.RetinotopyMapperObj.NewInitBlockTrial.connect(BasicExperiment,BasicExperiment.preNewInitBlockTrial);
				}
			}
		} 
		catch (e) 
		{
			BrainStim.write2OutputWindow(".*. Something went wrong connecting the Signal/Slots:" + e,"BasicRetinotopyExperiment");
		}		
	}
	else
	{
		try 
		{	
			if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
			{
				if(BasicExperiment.RetinotopyMapperObj)
				{
					BasicExperiment.RetinotopyMapperObj.NewInitBlockTrial.disconnect(BasicExperiment,BasicExperiment.preNewInitBlockTrial);
				}
			}
		} 
		catch (e) 
		{
			BrainStim.write2OutputWindow(".*. Something went wrong disconnecting the Signal/Slots:" + e,"BasicRetinotopyExperiment");
		}		
	}
	return Connect;
})