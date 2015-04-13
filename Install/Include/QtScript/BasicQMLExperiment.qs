///////////
//Includes//
///////////
Include("QtScript/BasicExperiment.qs");
////////////////////////
//Prepare Output Window//
////////////////////////
BrainStim.addOutputWindow("BasicQMLExperiment");
BrainStim.clearOutputWindow("BasicQMLExperiment");

//////////////////////////
//Public Property Variabeles//
//////////////////////////
//QML Experiment Directories
BasicExperiment.sQmlFilePath = BasicExperiment.sScriptRootPath + "/" + "FileName.qml";
//QML Experiment Objects
BasicExperiment.QMLViewerObject = null;
var tmpQMLViewerObj = null;
	
///////////////////////////
//Private Property Variabeles//
///////////////////////////

/////////////////////////
//Public Property Functions//
/////////////////////////
BasicExperiment.__proto__.InvokeQMLFunction = function(sMainItemName, sFunctionName, vParam1, vParam2, vParam3, vParam4, vParam5, vParam6, vParam7, vParam8, vParam9)
{
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","InvokeQMLFunction", arguments, true);
	var retVal = BasicExperiment.QMLViewerObject.invokeQml2Method(sMainItemName,sFunctionName,vParam1, vParam2, vParam3, vParam4, vParam5, vParam6, vParam7, vParam8, vParam9);
	if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	{
		var sArguments = "";
		if(arguments.length > 2)
		{
			for(var nArgCounter=2;nArgCounter<arguments.length;++nArgCounter)
			{
				if(nArgCounter==2)
					sArguments = arguments[nArgCounter];
				else
					sArguments = sArguments + "," + arguments[nArgCounter];
			}
		}
		BrainStim.write2OutputWindow("\tReturned value by " + sFunctionName + "(" + sArguments + ") is: " + retVal + " >>>","BasicQMLExperiment");	
	}
}

BasicExperiment.__proto__.preNewQMLSourceLoaded = function(sSource)
{
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","preNewQMLSourceLoaded", arguments, true);
	if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	{
		BasicExperiment.InvokeQMLFunction("DebugMode_Functions", "enableDebugMode");
	}
	BasicExperiment.NewQMLSourceLoaded(sSource);
}

BasicExperiment.__proto__.NewQMLSourceLoaded = function(sSource)
{
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","NewQMLSourceLoaded", arguments, true);
}

//*!This wrapper function uses the wrap method because the original function has more than one parameter
BasicExperiment.__proto__.preExperimentStateChanged = _.wrap(BasicExperiment.__proto__.preExperimentStateChanged, function(OriginalFunction,currentState,sDateTimeStamp) 
{	
	//*!Remove the first element and store the arguments in a seperate args array
	var args = [].slice.call(arguments);
	[].shift.apply(args);
	
	//ExperimentManager_NoState				= 0, /*!< this state is only internally used by the object while it is constructing, thus not yet fully available and therefore cannot be detected/used in the script environment. */
	//ExperimentManager_Constructed			= 1, /*!< this state is used to determine whenever the object has been constructed, this state also cannot be detected/used in the script environment. */
	//ExperimentManager_Loaded				= 2, /*!< after the Experiment file (*.exml) is successfully loaded (and validated) the ExperimentManager state changes to this state.*/
	//ExperimentManager_PreParsed			= 3, /*!< when the Experiment could be successfully preparsed (after loading) then the ExperimentManager state changes to this state.*/
	//ExperimentManager_Configured			= 4, /*!< after the function runExperiment()�is called the ExperimentManager (validates if it has not been done before) and configures the experiment, it then changes to this state if the configuration was done.*/
	//ExperimentManager_Initialized			= 5, /*!< after the Experiment configuration the ExperimentManager tries to initialized and changes the state to this state if this step was successful.*/
	//ExperimentManager_IsStarting			= 6, /*!< after the Experiment initialization the ExperimentManager is ready to actually start the Experiment but first it changes the state to this state.*/
	//ExperimentManager_Started				= 7, /*!< after the Experiment is successfully started the ExperimentManager changes to this state.*/
	//ExperimentManager_IsStopping			= 8, /*!< before the ExperimentManager tries to stop the Experiment it first changes to this state.*/
	//ExperimentManager_Stopped				= 9  /*!< after the ExperimentManager could finalize and stop the Experiment it changes to this state.*/
	
	//*!Call the below BasicExperiment.LogFunctionSignature method with the args variabele
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","preExperimentStateChanged", args, true);
	if((currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized) || (currentState == ExperimentManager.ExperimentState.ExperimentManager_PreParsed))
	{
		if(BasicExperiment.QMLViewerObj==null)
		{
			if(BasicExperiment.cExperimentStructure_Object == null)
				BasicExperiment.cExperimentStructure_Object = BasicExperiment.ExperimentManagerObj.getExperimentStructure();
			if(BasicExperiment.cExperimentStructure_Object != null)
			{
				var _bQMLViewerObjFound = false;
				var _arrObjIDList = [];
				var _nTmpObjectID;
				var _nTmpNewObjectID = 0;
				var _sQML2ViewerObjectName;
				BrainStim.write2OutputWindow("Searching for a declared available QML2Viewer Object..." ,"BasicQMLExperiment");
				var _nObjectCount = BasicExperiment.cExperimentStructure_Object.getObjectCount();
				BrainStim.write2OutputWindow("\t" + _nObjectCount + " Object(s) declared." ,"BasicQMLExperiment");
				if(_nObjectCount>0)
				{
					_arrObjIDList = BasicExperiment.cExperimentStructure_Object.getObjectIDList();
					for(var k=0;k<_nObjectCount;k++)
					{
						if(typeof _arrObjIDList[k] !== 'undefined')
						{
							if(_arrObjIDList[k] >= _nTmpNewObjectID)
								_nTmpNewObjectID = _arrObjIDList[k]+1;
							var _tmpObject = BasicExperiment.cExperimentStructure_Object.getObjectPointerByID(_arrObjIDList[k]);
							if(_tmpObject)
							{
								if(_tmpObject.getObjectClass() === 'QML2Viewer')
								{
									_bQMLViewerObjFound = true;
									_sQML2ViewerObjectName = _tmpObject.getObjectName();
									break;
								}
							}
						}
					}
				}
				
				if(_bQMLViewerObjFound)
				{
					if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
					{
						BrainStim.write2OutputWindow("\tFound and using QML2Viewer Object[" + k + "]: " + _sQML2ViewerObjectName ,"BasicQMLExperiment");
						BasicExperiment.QMLViewerObject = eval(_sQML2ViewerObjectName);
						BrainStim.write2OutputWindow(BasicExperiment.QMLViewerObj ,"BasicQMLExperiment");
					}
				}
				else
				{
					if(currentState == ExperimentManager.ExperimentState.ExperimentManager_PreParsed)
					{
						_sQML2ViewerObjectName = "objScriptCreatedQML2Viewer";
						BrainStim.write2OutputWindow("\tCreating a QML2Viewer Object: " + _sQML2ViewerObjectName,"BasicQMLExperiment");
						tmpQMLViewerObj = new cObjectStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
						tmpQMLViewerObj.setObjectID(_nTmpNewObjectID);
						tmpQMLViewerObj.setObjectName(_sQML2ViewerObjectName);
						tmpQMLViewerObj.setObjectClass("QML2Viewer");
						if(BasicExperiment.cExperimentStructure_Object.insertObject(tmpQMLViewerObj))
						{
							if(BasicExperiment.ExperimentManagerObj.parseCurrentExperimentStructure())
								BrainStim.write2OutputWindow("\tSuccessfully created and inserted a QML2Viewer Object: " + _sQML2ViewerObjectName,"BasicQMLExperiment");
						}
					}
				}
			}
		}
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		BasicExperiment.QMLViewerObject = null;
		tmpQMLViewerObj = null;
	}
	
	//*!Call the original function (wrap method, with more that one arguments) and return
	var vResult = OriginalFunction.apply(this,args);
	return vResult;
})

//*!This wrapper function can use the compose method because the original function has one or no parameters
BasicExperiment.__proto__.LogExperimentState = _.compose(BasicExperiment.LogExperimentState, function()
{
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","LogExperimentState", arguments, true);
	if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	{	
		var retVal = BasicExperiment.QMLViewerObject.invokeQml2Method("DebugMode_Functions","updateExperimentState",BasicExperiment.cCurrentExperimentBlockStructure.getBlockNumber(),BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber,BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_InternalTrigger);
		BrainStim.write2OutputWindow("* " + "DebugMode_Functions:updateExperimentState" + " returned: " + retVal + " >>>", "BasicQMLExperiment");	
	}
	//*!Return the first argument if there is one defined
	return;
})

BasicExperiment.__proto__.StartExperimentObjects = _.compose(BasicExperiment.StartExperimentObjects, function()
{
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","StartExperimentObjects", arguments, true);
	if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	{
		BasicExperiment.TriggerTimerObj.timeout.connect(BasicExperiment.QMLViewerObject, BasicExperiment.QMLViewerObject.incrementExternalTrigger);
	}
	if(BasicExperiment.ParallelPortCapture_Enabled)
	{				
		BasicExperiment.ParallelPortObj.CaptureThreadTriggered.connect(BasicExperiment.QMLViewerObject, BasicExperiment.QMLViewerObject.incrementExternalTrigger);
	}
	return;
})	

BasicExperiment.__proto__.StopExperimentObjects = _.compose(BasicExperiment.StopExperimentObjects, function()
{
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","StopExperimentObjects", arguments, true);
	if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	{	
		BasicExperiment.TriggerTimerObj.timeout.disconnect(BasicExperiment.QMLViewerObject, BasicExperiment.QMLViewerObject.incrementExternalTrigger);
	}
	if(BasicExperiment.ParallelPortCapture_Enabled)
	{
		BasicExperiment.ParallelPortObj.CaptureThreadTriggered.disconnect(BasicExperiment.QMLViewerObject, incrementExternalTrigger);
	}
	return;
})

BasicExperiment.__proto__.ConnectDisconnectScriptFunctions = _.compose(BasicExperiment.ConnectDisconnectScriptFunctions, function(Connect)
{
	BasicExperiment.LogFunctionSignature("BasicQMLExperiment","ConnectDisconnectScriptFunctions", arguments, true);
	if(Connect)
	{
		try 
		{	
			if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
			{
				if(BasicExperiment.QMLViewerObject)
				{
					BasicExperiment.QMLViewerObject.PrepareNewInitBlockTrial.connect(BasicExperiment,BasicExperiment.prePrepareNewInitBlockTrial);
					BasicExperiment.QMLViewerObject.NewInitBlockTrial.connect(BasicExperiment,BasicExperiment.preNewInitBlockTrial);
					BasicExperiment.QMLViewerObject.NewSourceLoaded.connect(BasicExperiment,BasicExperiment.preNewQMLSourceLoaded);
				}
			}
		} 
		catch (e) 
		{
			BrainStim.write2OutputWindow(".*. Something went wrong connecting the Signal/Slots:" + e,"BasicQMLExperiment");
		}		
	}
	else
	{
		try 
		{	
			if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
			{
				if(BasicExperiment.QMLViewerObject)
				{
					BasicExperiment.QMLViewerObject.PrepareNewInitBlockTrial.disconnect(BasicExperiment,BasicExperiment.prePrepareNewInitBlockTrial);
					BasicExperiment.QMLViewerObject.NewInitBlockTrial.disconnect(BasicExperiment,BasicExperiment.preNewInitBlockTrial);
					BasicExperiment.QMLViewerObject.NewSourceLoaded.disconnect(BasicExperiment,BasicExperiment.preNewQMLSourceLoaded);
				}
			}
		} 
		catch (e) 
		{
			BrainStim.write2OutputWindow(".*. Something went wrong disconnecting the Signal/Slots:" + e,"BasicQMLExperiment");
		}		
	}
	return Connect;
})