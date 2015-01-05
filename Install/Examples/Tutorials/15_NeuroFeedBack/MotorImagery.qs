//Includes
Include("QtScript/RealTimeFeedback.qs"); //Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/
//Prepare Output Window
StimulGL.addOutputWindow("MotorImagery");
StimulGL.clearOutputWindow("MotorImagery");
//Configuration
//Override existing variables to customize the experiment
BasicExperiment.sScriptRootPath = StimulGL.getActiveDocumentFileLocation();
BasicExperiment.sExmlFilePath = BasicExperiment.sScriptRootPath + "/" + "MotorImagery.exml";
BasicExperiment.sQmlFilePath = BasicExperiment.sScriptRootPath + "/" + "MotorImagery.qml";
BasicExperiment.nTestModeTriggerDuration = 2000;

//Parallel Port Settings
//BasicExperiment.ParallelPortCapture_Enabled = true;
//BasicExperiment.ParallelPortCapture_Address = 8209;
//BasicExperiment.ParallelPortCapture_Mask = 32;
//BasicExperiment.ParallelPortCapture_Method = 2;
//BasicExperiment.ParallelPortCapture_PostLHDelay = 100;
//BasicExperiment.ParallelPortCapture_PostHLDelay = 100;

//Private Property Variabeles
var _baselineItemAmount = 0; 
var _baselineValues = Array[3]; 
var _fMeanROI = 0;
var _bTBVConnectResult = false;
var _sMainItemName = "functions";

//Override Public Functions
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	BasicExperiment.LogFunctionSignature("MotorImagery","NewInitBlockTrial", arguments, false);
	if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber==0)
	{
		if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID==0)
		{
			BasicExperiment.InvokeQMLFunction(_sMainItemName, "setTextAndBarLevelFunction", "Rest", 0);	
		}
		else if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID==1)
		{
			BasicExperiment.InvokeQMLFunction(_sMainItemName, "setTextAndBarLevelFunction", "Feedback", _fMeanROI);		
		}
	}
}

BasicExperiment.__proto__.ExperimentStateChanged = function(currentState, sDateTimeStamp)
{
	BasicExperiment.LogFunctionSignature("MotorImagery","ExperimentStateChanged", arguments, false);
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{
		StimulGL.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Started(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "MotorImagery");	
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		var bOverwrite = true;
		StimulGL.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Stopped(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "MotorImagery");	
		StimulGL.saveOutputWindow(BasicExperiment.sScriptRootPath + "/" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp() + "_MotorImagery.txt", bOverwrite , "MotorImagery");	
	}	
}

RealTimeFeedback.__proto__.executePostStep = function executePostStep(nTimePoint)
{
	Log("custom RealTimeFeedback.executePostStep");
	StimulGL.write2OutputWindow("nTimePoint" + ";" + nTimePoint, "MotorImagery");	
	if(CalculateAndStoreNewFeedBack(RealTimeFeedback.TBVExchangerObject.tGetMeanOfROI(0)))
	{
		var nTimeStamp = BasicExperiment.GetCurrentDateTimeStamp();
		StimulGL.write2OutputWindow(nTimeStamp + ";" + "CalculateAndStoreNewFeedBack()" + ";" + _fMeanROI, "MotorImagery");	
	}
}

function CalculateAndStoreNewFeedBack(fRawValue)
{
	StimulGL.write2OutputWindow("CalculateAndStoreNewFeedBack(fRawValue) called" + ";" + fRawValue, "MotorImagery");
	if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID==0)
	{
		if (_baselineItemAmount == 2) 
		{
			_baselineItemAmount = 0;
		} 
		else 
		{
			_baselineItemAmount++;
		}
		_baselineValues[_baselineItemAmount] = fRawValue;
		return true;
	}
	else if(BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID==1)
	{
		var bl = (_baselineValues[0]+_baselineValues[1]+_baselineValues[2])/3
		var fb = (fRawValue - bl) / bl * 100;
		fb = fb / 3;
		fb = Math.round(fb*10)/10;
		if(fb>0.99)
			fb = 0.99;
		else if(fb < 0.01)
			fb = 0.01;
		_fMeanROI = fb;
		BasicExperiment.InvokeQMLFunction("functions", "setTextAndBarLevelFunction", "Update", _fMeanROI);	
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Start the experiment....
//var _bTBVConnectResult = RealTimeFeedback.TBVExchangerObject.connectToServer("127.0.0.1",55555);
var _bTBVConnectResult = RealTimeFeedback.TBVExchangerObject.activateAutoConnection();
//_bTBVConnectResult = true;
if(_bTBVConnectResult)
{
	BasicExperiment.RunExperiment();
}
else
{
	StimulGL.cleanupScript();
	StimulGL.write2OutputWindow("TBV connectToServer() returned false, aborting!", "MotorImagery");	
}

