///////////
//Includes//
///////////
Include("JavaScript/underscore.js");
////////////////////////
//Prepare Output Window//
////////////////////////
BrainStim.addOutputWindow("BasicExperiment");
BrainStim.clearOutputWindow("BasicExperiment");

(function( BasicExperiment, undefined )
{
//////////////////////////
//Public Property Variabeles//
//////////////////////////
	//Main Window Settings
	BasicExperiment.MainWindow_AutoDeactivate = false;
	//Experiment Directories
	BasicExperiment.sScriptRootPath = BrainStim.getActiveDocumentFileLocation();
	BasicExperiment.sExmlFilePath = BasicExperiment.sScriptRootPath + "/" + "FileName.exml";
	//Experiment Structure, State and Control Settings
	BasicExperiment.nCurrentExperimentState = ExperimentManager.ExperimentState.ExperimentManager_NoState;
	BasicExperiment.cExperimentStructure_Object = null;	
	BasicExperiment.nCurrentExperimentStructureState = null;
	BasicExperiment.cCurrentExperimentBlockStructure = null;
	BasicExperiment.nStartDateTimeStamp = 0;
	BasicExperiment.bExperimentFirstTriggerReceived = false;
	BasicExperiment.nTestModeTriggerDuration = 2000;
	BasicExperiment.arrExperimentModes = new Array("Testing_Mode", "Experiment_Mode");
	BasicExperiment.sChoosenExperimenMode;
	//Default Experiment Objects
	BasicExperiment.KeyBoardCaptureObj = null; 
	BasicExperiment.TriggerTimerObj = null;
	BasicExperiment.ParallelPortObj = null;	
	BasicExperiment.ExperimentManagerObj = null;
	//Keyboard Capture Settings
	BasicExperiment.KeyBoardCapture_Enabled = false;
	BasicExperiment.KeyBoardCapture_lAllowedKeyCodeList;
	//Parallel Port Capture Settings
	BasicExperiment.ParallelPortCapture_Enabled = false;
	BasicExperiment.ParallelPortCapture_Address = 8209;
	BasicExperiment.ParallelPortCapture_Mask = 32;
	BasicExperiment.ParallelPortCapture_Method = 2;
	BasicExperiment.ParallelPortCapture_PostLHDelay = 100;
	BasicExperiment.ParallelPortCapture_PostHLDelay = 100;
	//Additional Settings
	//BasicExperiment.StimulusHeightSpan = 1050;
	//BasicExperiment.StimulusWidthSpan = 1050;
	
///////////////////////////
//Private Property Variabeles//
//////////////////////////	
	//Local variabeles, name starts with '___' to prevent further usuage outside this scope
	var ___nLoopCounter;
	var ___cLoopStructure_Object0;
	var ___nLogFunctionSignatureCounter = 0;

/////////////////////////
//Public Property Functions//
/////////////////////////
	BasicExperiment.__proto__.RunExperiment = function(bSkipXMLValidation)
	{		
		bSkipXMLValidation = typeof bSkipXMLValidation !== 'undefined' ? bSkipXMLValidation : false;
		
		BasicExperiment.nStartDateTimeStamp = 0;
		//BasicExperiment.LogFunctionSignature(arguments.callee.name, arguments, true);
		BasicExperiment.LogFunctionSignature("BasicExperiment","RunExperiment", arguments, true);
		
		if(BasicExperiment.checkRequiredScriptPlugins()==false)
		{
			BasicExperiment.CleanupScript();
			return;
		}
		BasicExperiment.sChoosenExperimenMode = BasicExperiment.arrExperimentModes[1];
		BasicExperiment.sChoosenExperimenMode = BasicExperiment.DialogGetComboBoxItem("Choose the Experiment mode","Experiment mode:",BasicExperiment.arrExperimentModes,0);
		if(BasicExperiment.sChoosenExperimenMode === null)
		{
			BasicExperiment.CleanupScript();
		}
		else
		{			
			BrainStim.write2OutputWindow("Choosen experiment mode = " + BasicExperiment.sChoosenExperimenMode, "BasicExperiment");	
			BasicExperiment.ExperimentManagerObj = new ExperimentManager();
			BasicExperiment.ExperimentManagerObj.ExperimentStateHasChanged.connect(BasicExperiment, BasicExperiment.preExperimentStateChanged);
			if(BasicExperiment.ExperimentManagerObj.setExperimentFileName(BasicExperiment.sExmlFilePath))
			{
				BasicExperiment.ExperimentManagerObj.runExperiment(bSkipXMLValidation);
			}
			else
			{
				BrainStim.write2OutputWindow("*** Error: Could not locate the experiment file!", "BasicExperiment");
				BasicExperiment.CleanupScript();
			}
		}
	}
	
	BasicExperiment.__proto__.CreateArray = function(length)
	{
		var arr = new Array(length || 0),
		i = length;
		if (arguments.length > 1) 
		{
			var args = Array.prototype.slice.call(arguments, 1);
			while(i--) 
				arr[length-1 - i] = BasicExperiment.CreateArray.apply(this, args);
		}
		return arr;
	}	
	
	BasicExperiment.__proto__.GetCurrentDateTimeStamp = function()
	{
		var __nCurrentDateTimeStamp = _.now();
		if(BasicExperiment.nStartDateTimeStamp == 0)
			BasicExperiment.nStartDateTimeStamp = __nCurrentDateTimeStamp;
		return (__nCurrentDateTimeStamp - BasicExperiment.nStartDateTimeStamp);
	}	
	
	BasicExperiment.__proto__.KeyBoardResponseRecieved = function(response)
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","KeyBoardResponseRecieved", arguments, true);
		BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Keyboard code: " + response, "BasicExperiment");		
	}	
	
	BasicExperiment.__proto__.ExternalTriggerRecieved = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","ExternalTriggerRecieved", arguments, true);
	}

	BasicExperiment.__proto__.PrepareNewInitBlockTrial = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","PrepareNewInitBlockTrial", arguments, true);		
	}
	
	BasicExperiment.__proto__.NewInitBlockTrial = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","NewInitBlockTrial", arguments, true);		
	}
	
	BasicExperiment.__proto__.ExperimentStateChanged = function(currentState,sDateTimeStamp)
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","ExperimentStateChanged", arguments, true);
		
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
		
		var textToShow = "--- The Experiment State Changed to: ";
		
		if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Loaded)
		{
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_Loaded", "BasicExperiment");
		}
		else 	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_PreParsed)
		{
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_PreParsed", "BasicExperiment");
		}
		else 	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Configured)
		{
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_Configured", "BasicExperiment");
		}
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
		{
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_Initialized", "BasicExperiment");
			//Now all defined objects in the experiment file are constructed and therefore available in this script, so now we can make the connections between constructed the objects.
		}	
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_IsStarting)
		{	
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_IsStarting", "BasicExperiment");
		}
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
		{	
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_Started", "BasicExperiment");
		}
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_IsStopping)
		{	
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_IsStopping", "BasicExperiment");
		}	
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
		{
			BrainStim.write2OutputWindow(textToShow + "ExperimentManager_Stopped", "BasicExperiment");			
		}
	}	

//////////////////////////
//Private Property Functions//
//////////////////////////
	BasicExperiment.__proto__.CleanupScript = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","CleanupScript(begin)", arguments, true);
		//Disconnect Signal/Slots
		BasicExperiment.ConnectDisconnectScriptFunctions(false);
		//Destruct function definitions
		BasicExperiment.__proto__.RunExperiment = null;
		BasicExperiment.__proto__.CreateArray = null;
		BasicExperiment.__proto__.GetCurrentDateTimeStamp = null;
		BasicExperiment.__proto__.KeyBoardResponseRecieved = null;
		BasicExperiment.__proto__.ExternalTriggerRecieved = null;
		BasicExperiment.__proto__.PrepareNewInitBlockTrial = null;
		BasicExperiment.__proto__.NewInitBlockTrial = null;
		BasicExperiment.__proto__.ExperimentStateChanged = null;
		BasicExperiment.__proto__.CleanupScript = null;
		BasicExperiment.__proto__.preExperimentStateChanged = null;
		BasicExperiment.__proto__.LogFunctionSignature = null;
		BasicExperiment.__proto__.preExternalTriggerRecieved = null;
		BasicExperiment.__proto__.prePrepareNewInitBlockTrial = null;
		BasicExperiment.__proto__.preNewInitBlockTrial = null;
		BasicExperiment.__proto__.LogExperimentState = null;
		BasicExperiment.__proto__.InitExperimentObjects = null;
		BasicExperiment.__proto__.StartExperimentObjects = null;
		BasicExperiment.__proto__.StopExperimentObjects = null;
		BasicExperiment.__proto__.ConnectDisconnectScriptFunctions = null;
		BasicExperiment.__proto__.DialogGetComboBoxItem = null;
		//Destruct Allocated Objects
		BasicExperiment.nCurrentExperimentState = null;
		BasicExperiment.cExperimentStructure_Object = null;	
		BasicExperiment.nCurrentExperimentStructureState = null;
		BasicExperiment.cCurrentExperimentBlockStructure = null;
		BasicExperiment.arrExperimentModes = null;
		BasicExperiment.sChoosenExperimenMode = null;
		BasicExperiment.KeyBoardCaptureObj = null; 
		BasicExperiment.TriggerTimerObj = null;
		BasicExperiment.ParallelPortObj = null;	
		BasicExperiment.ExperimentManagerObj = null;
		BasicExperiment.KeyBoardCapture_lAllowedKeyCodeList = null;
		___cLoopStructure_Object0 = null;
		//Delete underscore library
		_ = null;
		//Finally call BrainStim's cleanupScript() function
		BrainStim.cleanupScript();
		BrainStim.write2OutputWindow("CleanupScript(finalized)", "BasicExperiment");
	}
	
	BasicExperiment.__proto__.checkRequiredScriptPlugins = function()	
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","checkRequiredScriptPlugins", arguments, true);
		//if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
		if(BasicExperiment.KeyBoardCapture_Enabled)
		{
			if (typeof KeyBoardCapture === 'undefined')
			{
				BasicExperiment.KeyBoardCapture_Enabled = false;
				BrainStim.write2OutputWindow("***ERROR***: The script plugin class named 'KeyBoardCapture' is not available, probably due to a missing plugin?", "BasicExperiment");
				return false;
			}
		}
		if(BasicExperiment.ParallelPortCapture_Enabled)
		{
			if (typeof ParallelPortDevice === 'undefined')
			{
				BasicExperiment.ParallelPortCapture_Enabled = false;
				BrainStim.write2OutputWindow("***ERROR***: The script plugin class named 'ParallelPortDevice' is not available, probably due to a missing plugin?", "BasicExperiment");
				return false;
			}
		}
		return true;
	}
	
	BasicExperiment.__proto__.preExperimentStateChanged = function(currentState,sDateTimeStamp)
	{			
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
		
		BasicExperiment.LogFunctionSignature("BasicExperiment","preExperimentStateChanged", arguments, true);
		BasicExperiment.nCurrentExperimentState = currentState;		
		if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Loaded)
		{

		}
		else 	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_PreParsed)
		{

		}
		else 	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Configured)
		{

		}
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
		{
			//Now all defined objects in the experiment file are constructed and therefore available in this script, so now we can make the connections between constructed the objects.
			
			if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
				BasicExperiment.ExperimentManagerObj.setDebugMode(true);
			else
				BasicExperiment.ExperimentManagerObj.setDebugMode(false);
			BasicExperiment.bExperimentFirstTriggerReceived = false;
			BasicExperiment.cExperimentStructure_Object = new cExperimentStructure();
			BasicExperiment.cExperimentStructure_Object = BasicExperiment.ExperimentManagerObj.getExperimentStructure();
			BasicExperiment.nCurrentExperimentStructureState = BasicExperiment.cExperimentStructure_Object.getCurrentExperimentState();
			BasicExperiment.ConnectDisconnectScriptFunctions(true);
		}	
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_IsStarting)
		{	
			if(BasicExperiment.InitExperimentObjects())
				BasicExperiment.StartExperimentObjects();
			else
				return;
		}
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
		{	

		}
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_IsStopping)
		{	
			BasicExperiment.StopExperimentObjects();
		}	
		else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
		{
			BasicExperiment.ExperimentManagerObj.ExperimentStateHasChanged.disconnect(BasicExperiment, BasicExperiment.preExperimentStateChanged);
			BasicExperiment.ExperimentStateChanged(currentState,sDateTimeStamp);
			BasicExperiment.CleanupScript();
		}
		BasicExperiment.ExperimentStateChanged(currentState,sDateTimeStamp);
	}	
	
	BasicExperiment.__proto__.LogFunctionSignature = function(sOutputTabName, sFunctionName, lArguments, ignoreMode)	
	{
		if((BasicExperiment.sChoosenExperimenMode == "Testing_Mode") || (ignoreMode))
		{
			var sArguments = "";
			if(lArguments.length > 0)
			{
				for(var nArgCounter=0;nArgCounter<lArguments.length;++nArgCounter)
				{
					if(nArgCounter==0)
						sArguments = lArguments[nArgCounter];
					else
						sArguments = sArguments + "," + lArguments[nArgCounter];
				}
			}
			BrainStim.write2OutputWindow("- Function " + sOutputTabName + ":" + sFunctionName + "(" + sArguments + ") called(" + ___nLogFunctionSignatureCounter + ").", sOutputTabName);
			___nLogFunctionSignatureCounter++;
		}
	}
	
	BasicExperiment.__proto__.preExternalTriggerRecieved = function()	
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","preExternalTriggerRecieved", arguments, true);
		if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
		{
			if(BasicExperiment.bExperimentFirstTriggerReceived == false)
			{
				BasicExperiment.TriggerTimerObj.startTimer(BasicExperiment.nTestModeTriggerDuration);
			}
		}
		
		BasicExperiment.nCurrentExperimentStructureState = BasicExperiment.cExperimentStructure_Object.getCurrentExperimentState();
		if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID>=0)
		{
			BasicExperiment.cCurrentExperimentBlockStructure = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID);
			if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_LoopID>=0)
			{
				___cLoopStructure_Object0 = BasicExperiment.cCurrentExperimentBlockStructure.getLoopPointerByID(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_LoopID);
			}
		}
		var __nCurrentDateTimeStamp = BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp();
		BasicExperiment.ExternalTriggerRecieved();
		if(BasicExperiment.bExperimentFirstTriggerReceived == false)
		{
			BasicExperiment.bExperimentFirstTriggerReceived = true;
			BasicExperiment.NewInitBlockTrial();
		}	
		if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
			BasicExperiment.LogExperimentState();
		
		if(BasicExperiment.nStartDateTimeStamp == 0)
		{
			BasicExperiment.nStartDateTimeStamp = __nCurrentDateTimeStamp;
			BrainStim.write2OutputWindow("* " + BasicExperiment.nStartDateTimeStamp + ";" + "Experiment Started(First External Trigger)", "BasicExperiment");
		}
		BrainStim.write2OutputWindow("* " + (__nCurrentDateTimeStamp - BasicExperiment.nStartDateTimeStamp) + ";" + "External Trigger Received", "BasicExperiment");
	}
	
	BasicExperiment.__proto__.prePrepareNewInitBlockTrial = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","prePrepareNewInitBlockTrial", arguments, true);
		if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
		{
			BasicExperiment.PrepareNewInitBlockTrial();
			//BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "PrepareNewInitBlockTrial() executed", "BasicExperiment");
		}
	}
	
	BasicExperiment.__proto__.preNewInitBlockTrial = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","preNewInitBlockTrial", arguments, true);
		if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
		{
			BasicExperiment.NewInitBlockTrial();
			BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "New Block/Trial Initialization", "BasicExperiment");
		}
	}

	BasicExperiment.__proto__.LogExperimentState = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","LogExperimentState", arguments, true);
		
		___nLoopCounter = -1;
		BrainStim.write2OutputWindow("-----", "BasicExperiment");
		if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID >= 0)
		{
			BrainStim.write2OutputWindow("CurrentBlock_BlockID:" + BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID, "BasicExperiment");
			BrainStim.write2OutputWindow("CurrentBlock_BlockNumber:" + BasicExperiment.cCurrentExperimentBlockStructure.getBlockNumber(), "BasicExperiment");
			BrainStim.write2OutputWindow("CurrentBlock_TrialNumber:" + BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber, "BasicExperiment");
			BrainStim.write2OutputWindow("CurrentBlock_InternalTrigger:" + BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_InternalTrigger, "BasicExperiment");
			BrainStim.write2OutputWindow("CurrentBlock_ExternalTrigger:" + BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_ExternalTrigger, "BasicExperiment");
			BrainStim.write2OutputWindow("Experiment_ExternalTrigger:" + BasicExperiment.nCurrentExperimentStructureState.Experiment_ExternalTrigger, "BasicExperiment");		
			if (BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_LoopID == -4)
			{
				BrainStim.write2OutputWindow("CurrentBlock_LoopID: RA_UNDEFINED", "BasicExperiment");
			}
			else if (BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_LoopID >= 0)
			{
				BrainStim.write2OutputWindow("CurrentBlock_LoopID:" + BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_LoopID, "BasicExperiment");
				___nLoopCounter = ___cLoopStructure_Object0.getLoopCounter();
				BrainStim.write2OutputWindow("CurrentBlock_LoopCounter0:" + ___nLoopCounter, "BasicExperiment");	
			}
			BrainStim.write2OutputWindow("-----", "BasicExperiment");
		}
	}

	BasicExperiment.__proto__.InitExperimentObjects = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","InitExperimentObjects", arguments, true);
		if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
		{	
			BasicExperiment.TriggerTimerObj = new TriggerTimer();
		}
		if(BasicExperiment.checkRequiredScriptPlugins()==false)
			return false;
		if(BasicExperiment.KeyBoardCapture_Enabled)
		{
			BasicExperiment.KeyBoardCaptureObj = new KeyBoardCapture();
		}
		if(BasicExperiment.ParallelPortCapture_Enabled)
		{
			BasicExperiment.ParallelPortObj = new ParallelPortDevice();
		}
		return true;
	}	

	BasicExperiment.__proto__.StartExperimentObjects = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","StartExperimentObjects", arguments, true);
		if(BasicExperiment.KeyBoardCapture_Enabled)
		{
			if (BasicExperiment.KeyBoardCaptureObj)
			{
				BasicExperiment.KeyBoardCaptureObj.StartCaptureThread(0, true, BasicExperiment.KeyBoardCapture_lAllowedKeyCodeList);
				//StartCaptureThread(const short method, bool keyForwarding, const QList<int> &lKeyList)
				//0: KeyBoardNameSpace::KeyPressed;
				//1: KeyBoardNameSpace::KeyReleased;
				//2: KeyBoardNameSpace::KeyPressedReleased;	
				try 
				{	
					BasicExperiment.KeyBoardCaptureObj.CaptureThreadKeyPressed.connect(BasicExperiment, BasicExperiment.KeyBoardResponseRecieved);
				} 
				catch (e) 
				{
					BrainStim.write2OutputWindow(".*. Something went wrong connecting the Keyboard Capture Signal/Slots:" + e,"Default");
				}
			}
		}
		if(BasicExperiment.ParallelPortCapture_Enabled)
		{
			if (BasicExperiment.ParallelPortObj)
				BasicExperiment.ParallelPortObj.StartCaptureThread(BasicExperiment.ParallelPortCapture_Address,BasicExperiment.ParallelPortCapture_Mask,BasicExperiment.ParallelPortCapture_Method,BasicExperiment.ParallelPortCapture_PostLHDelay,BasicExperiment.ParallelPortCapture_PostHLDelay);
		}
		if(BasicExperiment.MainWindow_AutoDeactivate)
			BrainStim.enableActiveDocument(false);
	}	
	
	BasicExperiment.__proto__.StopExperimentObjects = function()
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","StopExperimentObjects", arguments, true);
		if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
		{	
			BasicExperiment.TriggerTimerObj.stopTimer();
		}
		if(BasicExperiment.KeyBoardCapture_Enabled)
		{
			if(BasicExperiment.KeyBoardCaptureObj)
			{
				BasicExperiment.KeyBoardCaptureObj.StopCaptureThread();
				BasicExperiment.KeyBoardCaptureObj.CaptureThreadKeyPressed.disconnect(BasicExperiment, BasicExperiment.KeyBoardResponseRecieved);
			}
		}		
		if(BasicExperiment.ParallelPortCapture_Enabled)
		{
			if(BasicExperiment.ParallelPortObj)
				BasicExperiment.ParallelPortObj.StopCaptureThread();
		}
		if(BasicExperiment.MainWindow_AutoDeactivate)
			BrainStim.enableActiveDocument(true);		
	}

	BasicExperiment.__proto__.ConnectDisconnectScriptFunctions = function(Connect)
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","ConnectDisconnectScriptFunctions", arguments, true);
		if(Connect)
		{
			BrainStim.write2OutputWindow("::,... Connecting Signal/Slots","BasicExperiment");
			try 
			{	
				if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
				{
					BasicExperiment.cExperimentStructure_Object.externalTriggerRecieved.connect(BasicExperiment,BasicExperiment.preExternalTriggerRecieved);
				}
			} 
			catch (e) 
			{
				BrainStim.write2OutputWindow(".*. Something went wrong connecting the Signal/Slots:" + e,"BasicExperiment");
			}		
		}
		else
		{
			BrainStim.write2OutputWindow("... Disconnecting Signal/Slots","BasicExperiment");
			try 
			{	
				if(BasicExperiment.nCurrentExperimentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
				{
					BasicExperiment.cExperimentStructure_Object.externalTriggerRecieved.disconnect(BasicExperiment, BasicExperiment.preExternalTriggerRecieved);
				}
			} 
			catch (e) 
			{
				BrainStim.write2OutputWindow(".*. Something went wrong disconnecting the Signal/Slots:" + e,"BasicExperiment");
			}		
		}	
	}

	BasicExperiment.__proto__.DialogGetComboBoxItem = function(title,text,items,current)
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","DialogGetComboBoxItem", arguments, true);
		var tstDialog = new QInputDialog();
		tstDialog.setWindowTitle(title);
		tstDialog.setComboBoxItems (items);
		tstDialog.setTextValue(items[current]);
		tstDialog.setLabelText(text);
		var bResult = tstDialog.exec();
		var sResult = tstDialog.textValue();
		tstDialog = null;
		if(bResult == QDialog.Accepted)
		{
			return sResult;
		}
		return null;
	}
	
	BasicExperiment.__proto__.DialogGetStringItem = function(title,text,current)
	{
		BasicExperiment.LogFunctionSignature("BasicExperiment","DialogGetStringItem", arguments, true);
		var tstDialog = new QInputDialog();
		tstDialog.setWindowTitle(title);
		tstDialog.setTextValue(current);
		tstDialog.setLabelText(text);
		var bResult = tstDialog.exec();
		var sResult = tstDialog.textValue();
		tstDialog = null;
		if(bResult == QDialog.Accepted)
		{
			return sResult;
		}
		return null;
	}
	
}( this.BasicExperiment = this.BasicExperiment || {} ));

//////////////////////////////////////////////////////////

//BasicExperiment.RunExperiment();
//BasicExperiment.CleanupScript();
