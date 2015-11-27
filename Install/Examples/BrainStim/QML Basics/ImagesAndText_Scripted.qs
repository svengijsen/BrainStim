//Includes:
Include("QtScript/BasicQMLExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/

//Included variables:
BasicExperiment.sExmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "ImagesAndText_Scripted.exml";
BasicExperiment.nTestModeTriggerDuration = 1000;

//Custom script references (variables):
var QML2Viewer_Global_QmlFilePath = BrainStim.getActiveDocumentFileLocation() + "/" + "ImagesAndText_Scripted.qml";

//Override Public Functions from the BasicQMLExperiment.qs include (that includes BasicExperiment):
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
   var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
   var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
   var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
   BrainStim.write2OutputWindow("*** Going to initialize a new Block-trial: BlockNr: BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber);
   if(_currentBlockNumber==0)
   {
      BasicExperiment.InvokeQMLFunction("functions", "setTextFunction", "Block 0");
      BasicExperiment.InvokeQMLFunction("functions", "setImageFunction", "images/World.png");
   }
   else if(_currentBlockNumber==1)
   {
      if(_currentTrialNumber==0)
         BasicExperiment.InvokeQMLFunction("functions", "setTextFunction", "Block 1, Trial 0");    
      else
         BasicExperiment.InvokeQMLFunction("functions", "setTextFunction", "Block 1, Trial 1");
      BasicExperiment.InvokeQMLFunction("functions", "setImageFunction", "images/BrainStim.png");
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