//Includes:
Include("QtScript/BasicQMLExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/

//Included variables:
BasicExperiment.sExmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "Ogre3D_Example1.exml";
BasicExperiment.nTestModeTriggerDuration = 1000;

//Custom script references (variables):
var QML2Viewer_Global_QmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "Ogre3D_Example1.qml";

//Override Public Functions from the BasicQMLExperiment.qs include (that includes BasicExperiment):
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	BrainStim.write2OutputWindow("*** Going to initialize a new Block-trial: BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber);

	if(_currentBlockNumber == 1)
	{
		var retVal = BasicExperiment.QMLViewerObject.invokeQml2Method("functions", "showOgre3DItem", true);
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