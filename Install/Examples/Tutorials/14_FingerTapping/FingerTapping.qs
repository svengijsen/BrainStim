//Includes
Include("QtScript/BasicQMLExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/
//Prepare Output Window
BrainStim.addOutputWindow("FingerTapping");
BrainStim.clearOutputWindow("FingerTapping");
//Configuration
BasicExperiment.sScriptRootPath = BrainStim.getActiveDocumentFileLocation();
BasicExperiment.sExmlFilePath = BasicExperiment.sScriptRootPath + "/" + "FingerTapping.exml";
BasicExperiment.sQmlFilePath = BasicExperiment.sScriptRootPath + "/" + "FingerTapping.qml";
BasicExperiment.nTestModeTriggerDuration = 2000;
//Configuration - Keyboard capture
BasicExperiment.KeyBoardCapture_Enabled = true;
BasicExperiment.KeyBoardCapture_lAllowedKeyCodeList = Array[49 ,53, 54]; // keys 1, 5, 6 are used
	//Example buttonbox mapping definition (copied from MBIC)
	// key 1, Code 49 - R Index (blue) 
	// key 2, Code 50 - R Middle (yellow) 
	// key 3, Code 51 - R Ring (green) 
	// key 4, Code 52 - R Pinky (red)
	// key 5, Code 53 - * Scanner Trigger *
	// key 6, Code 54 - L Index (blue) 
	// key 7, Code 55 - L Middle (yellow) 
	// key 8, Code 56 - L Ring (green)
	// key 9, Code 57 - L Pinky (red)	
BasicExperiment.MainWindow_AutoDeactivate = true;//To prevent key presses to change the documents content while running the experiment

// Experiment -> TR = 2 sec -> 290 vols -> 580 sec -> 9 min, 40 sec
// Blocks:
// 0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28
// FL	L	FR	R	FL	L	FR	R	FR	R	FR	R	FR	R	FL	L	FR	R	FL	L	FR	R	FL	L	FL	L	FL	L	F

//Define some Local Variabeles for our own custom usage, name them starting with an underscore to prevent using already
var _sMainItemName = "functions";
var _sSetTextFunctionName = "setCenterText";
var _sSetImageFunctionName = "setImageFile";
var _sSetFingerFlickrAnimationFunctionName = "setFingerFlickrAnimation";
var _sLeftHandBlockImagePath = "./images/LeftHand_Block.png";
var _sRightHandBlockImagePath = "./images/RightHand_Block.png";
var _sLeftHandRestImagePath = "./images/LeftHand_Rest.png";
var _sRightHandRestImagePath = "./images/RightHand_Rest.png";

//Override Public Functions
BasicExperiment.__proto__.NewInitBlockTrial = function()
{
	BasicExperiment.LogFunctionSignature("FingerTapping","NewInitBlockTrial", arguments, false);
	//if(BasicExperiment.sChoosenExperimenMode == "Testing_Mode")
	//{
		//Beep();
	//}
	var BID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var BNr = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(BID).getBlockNumber();
	var TNr = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	
	if((BNr == 0) || (BNr == 5))
	{
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetFingerFlickrAnimationFunctionName, "");
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetImageFunctionName, "");
	} 
	if(BNr == 1)
	{
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetFingerFlickrAnimationFunctionName, "");
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetImageFunctionName, _sLeftHandRestImagePath);
	}
	else 	if(BNr == 2)
	{
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetFingerFlickrAnimationFunctionName, "left");
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetImageFunctionName, _sLeftHandBlockImagePath);
	}
	else 	if(BNr == 3)
	{
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetFingerFlickrAnimationFunctionName, "");
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetImageFunctionName, _sRightHandRestImagePath);
	}
	else 	if(BNr == 4)
	{
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetFingerFlickrAnimationFunctionName, "right");
		BasicExperiment.InvokeQMLFunction(_sMainItemName, _sSetImageFunctionName, _sRightHandBlockImagePath);
	}
}

BasicExperiment.__proto__.KeyBoardResponseRecieved = function(response)
{
	//BasicExperiment.LogFunctionSignature("FingerTapping","KeyBoardResponseRecieved", arguments, false);
	var nTimeStamp = BasicExperiment.GetCurrentDateTimeStamp();
	
	if(response==53)// = key '5', see above table!
	{
		BasicExperiment.QMLViewerObject.incrementExternalTrigger();
	}
	BrainStim.write2OutputWindow(nTimeStamp + ";" + "Keyboard pressed, code:" + ";" + response, "FingerTapping");		
}

BasicExperiment.__proto__.ExperimentStateChanged = function(currentState, sDateTimeStamp)
{
	//LogFunctionSignature("FingerTapping","ExperimentStateChanged", arguments, false);
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{
		BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Started(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "FingerTapping");	
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		var bOverwrite = true;
		BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Stopped(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "FingerTapping");	
		BrainStim.saveOutputWindow(BasicExperiment.sScriptRootPath + "/" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp() + "_FingerTapping.txt", bOverwrite , "FingerTapping");	
	}
}
//Log("XX: " + BasicExperiment.nStartDateTimeStamp.toLongDateString());//.toString("dd.MM.yyyy"));
//Run the Experiment
BasicExperiment.RunExperiment();