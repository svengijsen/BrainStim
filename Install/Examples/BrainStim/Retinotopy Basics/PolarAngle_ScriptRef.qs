//Includes:
Include("QtScript/BasicRetinotopyExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/

//Included variables:
BasicExperiment.sExmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "PolarAngle_ScriptRef.exml";
BasicExperiment.nTestModeTriggerDuration = 1000;

//Custom script references (variables):
var RetinoMapper_Global_Pattern = "Fixation";
var RetinoMapper_Global_Width = 800.0;
var RetinoMapper_Global_Height = RetinoMapper_Global_Width;
var RetinoMapper_Global_FixColor = "#00FF00"; //Green color
var RetinoMapper_Polar_NrOfCheckers = 6;
var RetinoMapper_Polar_Direction = -1 //CounterClockWise
var RetinoMapper_Polar_TriggerDuration = 1000.0;

//Override Public Functions from the BasicRetinotopyExperiment.qs include (that includes BasicExperiment):
BasicExperiment.__proto__.PrepareNewInitBlockTrial = function()
{
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	BrainStim.write2OutputWindow("*** Going to prepare a initialization of a new Block-trial:  BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber);	
	
	if(_currentBlockNumber==1)
	{
		RetinoMapper_Global_Pattern = "PolarAngle";
		RetinoMapper_Global_FixColor = "#FF0000"; //Red color
	}
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