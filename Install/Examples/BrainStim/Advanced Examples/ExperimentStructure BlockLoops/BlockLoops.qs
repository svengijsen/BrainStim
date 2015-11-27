//Includes
Include("QtScript/BasicQMLExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/
//Prepare Output Window
BrainStim.addOutputWindow("BlockLoops");
BrainStim.clearOutputWindow("BlockLoops");
//Configuration
BasicExperiment.sScriptRootPath = BrainStim.getActiveDocumentFileLocation();
BasicExperiment.sExmlFilePath = BasicExperiment.sScriptRootPath + "/" + "BlockLoops.exml";
BasicExperiment.sQmlFilePath = BasicExperiment.sScriptRootPath + "/" + "BlockLoops.qml";
BasicExperiment.nTestModeTriggerDuration = 200;

//Override Public Functions
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	
	textToShow = "Block " + _currentBlockNumber + ", Trial " + _currentTrialNumber;
	BrainStim.write2OutputWindow(textToShow, "BlockLoops");
}

BasicExperiment.__proto__.ExperimentStateChanged = function(currentState, sDateTimeStamp)
{
	//LogFunctionSignature("FingerTapping","ExperimentStateChanged", arguments, false);
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{
		BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Started(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "BlockLoops");	
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Stopped(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "BlockLoops");	
	}
}
//Run the Experiment
BasicExperiment.RunExperiment();
