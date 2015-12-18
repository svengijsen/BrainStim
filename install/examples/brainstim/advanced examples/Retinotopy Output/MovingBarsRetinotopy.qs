//Includes
Include("QtScript/BasicRetinotopyExperiment.qs");//Do not use a backslash (\) in a string, use forward slashes for a directory definition!//../../include/
Include("CreateMeanImage.qs");
Include("FileRenaming.qs");
//Prepare Output Window
BrainStim.addOutputWindow("MovingBarsRetinotopy");
BrainStim.clearOutputWindow("MovingBarsRetinotopy");
//Configuration
BasicExperiment.sScriptRootPath = BrainStim.getActiveDocumentFileLocation();
BasicExperiment.sExmlFilePath = BasicExperiment.sScriptRootPath + "/" + "MovingBarsRetinotopy.exml";
BasicExperiment.nTestModeTriggerDuration = 2000;
var _generateModeTriggerDuration = 200;//The time your computer has to generate a single png file, if set to low then the png generation can fail, depends on your local hardware.
//Parallel port settings
BasicExperiment.ParallelPortCapture_Enabled = false;
BasicExperiment.ParallelPortCapture_Address = 8209;
BasicExperiment.ParallelPortCapture_Mask = 32;
BasicExperiment.ParallelPortCapture_Method = 2;
BasicExperiment.ParallelPortCapture_PostLHDelay = 100;
BasicExperiment.ParallelPortCapture_PostHLDelay = 100;
//Configuration - Keyboard capture
BasicExperiment.KeyBoardCapture_Enabled = true;
BasicExperiment.KeyBoardCapture_lAllowedKeyCodeList = Array[53]; // keys 5 is used
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
//BasicExperiment.MainWindow_AutoDeactivate = true;//To prevent key presses to change the documents content while running the experiment

//Add some new Parameter configurations, to be used from the exml file
BasicExperiment.bShowFixPoint = true;
BasicExperiment.bDiscreteTriggerSteps = true;
BasicExperiment.nFixationSize = 8;
BasicExperiment.nStimuliRefreshRate = 30;
BasicExperiment.colBackGround = "#575757";
BasicExperiment.colFixation = "#FF0000";
BasicExperiment.nMovingBarCycleAmount = 12;
BasicExperiment.bOutputTriggerFrame = false;
BasicExperiment.nMovingBarDirection = -1;
BasicExperiment.fMovingBarCoverage = 0.70710678;//remaining part... 118654752440084436210485;
BasicExperiment.fFlickrFrequency = 7.5;
BasicExperiment.nMovingBarHeightCheckAmount = 3;
BasicExperiment.fMovingBarHeight = 12.0;
BasicExperiment.sOutputFrameType = "Mask";
BasicExperiment.sOutputFrameFormat = "PNG";
BasicExperiment.nStimWidth = 1200;//see below scalefactor (_nPngGenerationDownScaleFactor)!
BasicExperiment.nStimHeight = 1200;//see below scalefactor (_nPngGenerationDownScaleFactor)!
BasicExperiment.nMovingBarAngle = 0;
BasicExperiment.arrEmptyTriggerSteps = [];
BasicExperiment.nEmptyTriggerSteps = BasicExperiment.arrEmptyTriggerSteps.length;
BasicExperiment.bMovingBarRandomisation = true;
BasicExperiment.arrRandomizeTriggerSteps = [];
//////////////////Local Variabeles///////////////////////////////
var _sPrtExperimentFileName = "MovingBars"; 
var _nPngGenerationDownScaleFactor = 8//This means that the generated PNG files are 1200/8 = 150 pixels, see above two lines!
var _objPRTFormatManager = new PRTFormatManager();//See also http://www.brainvoyager.de/BV2000OnlineHelp/BrainVoyagerWebHelp/mergedProjects/FileFormats/The_format_of_PRT_files.htm
var _objRandomGenerator = new RandomGenerator();
var _arrBlockAngles = [];
var _arrPrtFileConditions = [];
var _sLogFileDirectory = BasicExperiment.sScriptRootPath + "/logfiles/";
var _sPrtFileDirectory = BasicExperiment.sScriptRootPath + "/prtfiles/";
var _sInputFileDirectory = BasicExperiment.sScriptRootPath + "/inputfiles/";
var _sOutputFileDirectory = BasicExperiment.sScriptRootPath + "/outputfiles/";
var _dirLogFilePath = new QDir(_sLogFileDirectory);
var _dirPrtFilePath = new QDir(_sPrtFileDirectory);
var _dirInputFilePath = new QDir(_sInputFileDirectory);
var _dirOutputFilePath = new QDir(_sOutputFileDirectory);
var _nNumberOfAngles = 8; // so each is amount of 360/8 (=45 degrees)
var _bSkipXMLValidation = true;
var _bKeepFirstLoopRandomisation = true;//If set to false then each loop has the same angle but a different step randomisation
var _arrPreservedAngleSteps = [];
var _arrSelectedExperimentTypes = new Array('Run the experiment','Run the experiment (using an existing randomization set)', 'Generating Binary Stimulus Frames');//Please do not append, delete or change order here!
var _sSelectedExperimentType = _arrSelectedExperimentTypes[0];
var _sBinaryStimulusFramesInputFilePath = "";
var _fBinaryStimulusFramesOutputFile;
var _sBinaryStimulusFramesOutputBuffer = "";
//////////////////Make sure that the log and prt file directories exists////////
if(_dirLogFilePath.exists() == false)
	_dirLogFilePath.mkdir(_sLogFileDirectory);
if(_dirPrtFilePath.exists() == false)
	_dirPrtFilePath.mkdir(_sPrtFileDirectory);
if(_dirInputFilePath.exists() == false)
	_dirInputFilePath.mkdir(_sInputFileDirectory);
if(_dirOutputFilePath.exists() == false)
	_dirOutputFilePath.mkdir(_sOutputFileDirectory);

var _nLoopNumber = 0;
var _SelectedGeneratedPngDirectory = "";//User needs to select this one

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Override Public Functions
BasicExperiment.__proto__.ExperimentStateChanged = function(currentState, sDateTimeStamp)
{
	BasicExperiment.LogFunctionSignature("MovingBarsRetinotopy","ExperimentStateChanged", arguments, true);
	if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Initialized)
	{
		BasicExperiment.RetinotopyMapperObj.setCustomOutputDirectoryPath(_sOutputFileDirectory);
	}
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Started)
	{
		_sPrtExperimentFileName = _sPrtExperimentFileName + "_" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyyMMddHHmmss") ;
		if((_sSelectedExperimentType == _arrSelectedExperimentTypes[0]) || (_sSelectedExperimentType == _arrSelectedExperimentTypes[1]))//Run the experiment
		{
			_fBinaryStimulusFramesOutputFile = new QFile();
			_fBinaryStimulusFramesOutputFile.setFileName(_sInputFileDirectory + _sPrtExperimentFileName + ".txt");
			if (_fBinaryStimulusFramesOutputFile.open(QIODevice.WriteOnly || QIODevice.Text)==false)
				BasicExperiment.ExperimentManagerObj.abortExperiment();
		}
         	BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Started(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "MovingBarsRetinotopy");		
	}	
	else if(currentState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)
	{
		var bOverwrite = true;
		if((_sSelectedExperimentType == _arrSelectedExperimentTypes[0]) || (_sSelectedExperimentType == _arrSelectedExperimentTypes[1]))//Run the experiment
			BrainStim.write2OutputWindow("\n\nSaving to output file:\n" + _sBinaryStimulusFramesOutputBuffer + "\n", "MovingBarsRetinotopy");
		BrainStim.write2OutputWindow(BasicExperiment.GetCurrentDateTimeStamp() + ";" + "Experiment Stopped(" + BasicExperiment.ExperimentManagerObj.getCurrentDateTimeStamp("yyyy-MM-dd, HH::mm::ss.zzz") + ")", "MovingBarsRetinotopy");	
		BrainStim.saveOutputWindow(_sLogFileDirectory + _sPrtExperimentFileName + "_log.txt", bOverwrite , "MovingBarsRetinotopy");		
		if((_sSelectedExperimentType == _arrSelectedExperimentTypes[0]) || (_sSelectedExperimentType == _arrSelectedExperimentTypes[1]))//Run the experiment
		{
			_fBinaryStimulusFramesOutputFile.write(new QByteArray(_sBinaryStimulusFramesOutputBuffer));
			_fBinaryStimulusFramesOutputFile.close();
			_savePRTFile();
		}
		else if(_sSelectedExperimentType == _arrSelectedExperimentTypes[2])//Generate Binairy Stimulus Frames
		{
			_ValidateCreateMeanImageAndRename();
			//QProcess.startDetached("explorer " + "C:\\Users\\<username>\\BrainStim(vx.x.x.x, xx-bits)\\Outputs\\RetinoWidget");//Open the outpu directory that contains the *.png files
		}	
	}
}

BasicExperiment.__proto__.PrepareNewInitBlockTrial = function()
{
	BasicExperiment.LogFunctionSignature("MovingBarsRetinotopy","PrepareNewInitBlockTrial", arguments, false);
	
	var _currentBlockID = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_BlockID;
	var _currentBlockNumber = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(_currentBlockID).getBlockNumber();
	var _currentTrialNumber = BasicExperiment.nCurrentExperimentStructureState.CurrentBlock_TrialNumber;
	BrainStim.write2OutputWindow("* BlockNr: " + _currentBlockNumber + ", TrialNr: " + _currentTrialNumber + ", LoopNr: " + _nLoopNumber, "MovingBarsRetinotopy");	

	if(_currentBlockNumber==1)
	{
		var _nTempIndex;
		if(_bKeepFirstLoopRandomisation)
			_nTempIndex = _currentTrialNumber;
		else
			_nTempIndex = (_nLoopNumber * 8) + _currentTrialNumber;
	
		if((_nLoopNumber==0) || (_bKeepFirstLoopRandomisation==false))
		{
			if((_sSelectedExperimentType == _arrSelectedExperimentTypes[0]) || (_sSelectedExperimentType == _arrSelectedExperimentTypes[1]))//Run the experiment
			{
				if((_currentTrialNumber==0) && (_nLoopNumber==0))
				{
					if(_sSelectedExperimentType == _arrSelectedExperimentTypes[0])
						_arrBlockAngles = _generateBlockAngles();
					_sBinaryStimulusFramesOutputBuffer = _sBinaryStimulusFramesOutputBuffer + "_arrBlockAngles = [" + _arrBlockAngles + "];\n\n";
				}
				if(_sSelectedExperimentType == _arrSelectedExperimentTypes[0])
					_arrPreservedAngleSteps[_nTempIndex] = _generateAngleTriggerSteps();
				_sBinaryStimulusFramesOutputBuffer = _sBinaryStimulusFramesOutputBuffer + "_arrPreservedAngleSteps[" + _nTempIndex + "] = [" + _arrPreservedAngleSteps[_nTempIndex] + "];\n";
			}
			else//Generate
			{
				BrainStim.write2OutputWindow("\t-Using Included Angle: " + _arrBlockAngles[_currentTrialNumber], "MovingBarsRetinotopy");
				BrainStim.write2OutputWindow("\t-Using Included Steps: " + _arrPreservedAngleSteps[_nTempIndex], "MovingBarsRetinotopy");
			}
		}
		else
		{
			BrainStim.write2OutputWindow("\t-Using Angle(preserved): " + _arrBlockAngles[_currentTrialNumber], "MovingBarsRetinotopy");
			BrainStim.write2OutputWindow("\t-Angle Steps(preserved): " + _arrPreservedAngleSteps[_nTempIndex], "MovingBarsRetinotopy");
		}
		
		BasicExperiment.arrRandomizeTriggerSteps = _arrPreservedAngleSteps[_nTempIndex];
		BasicExperiment.nMovingBarAngle = _arrBlockAngles[_currentTrialNumber];
		
		if(_currentTrialNumber==7)
		{
			_nLoopNumber = _nLoopNumber + 1;
		}
	}
}

BasicExperiment.__proto__.KeyBoardResponseRecieved = function(response)
{
	//BasicExperiment.LogFunctionSignature("FingerTapping","KeyBoardResponseRecieved", arguments, false);
	//var nTimeStamp = BasicExperiment.GetCurrentDateTimeStamp();
	if(response==53)// = key '5', see above table!
	{
		BasicExperiment.RetinotopyMapperObj.incrementExternalTrigger();
	}
	//BrainStim.write2OutputWindow(nTimeStamp + ";" + "Keyboard pressed, code:" + ";" + response, "MovingBar");		
}


///////////////////////////////////////////////////////////Local Custom Function Definitions////////////////////////////////////////////////
function _getNumberOfLoopRepetitions()
{
	if(BasicExperiment.cExperimentStructure_Object)
	{
		var tmpBlock = BasicExperiment.cExperimentStructure_Object.getBlockPointerByID(1);//Second block
		if(tmpBlock)
		{
			BrainStim.write2OutputWindow("Block found name: " + tmpBlock.getBlockName(), "MovingBarsRetinotopy");
			var tmpLoop = tmpBlock.getLoopPointerByID(0);//First Loop
			if(tmpLoop)
			{
				var _nNumberOfLoopsFound = tmpLoop.getNumberOfLoops();
				BrainStim.write2OutputWindow("\tNumber of defined Loops: " + _nNumberOfLoopsFound, "MovingBarsRetinotopy");
				return _nNumberOfLoopsFound;
			}
			return 0;
		}
	}
	return -1;
}

function _generateBlockAngles()
{
	_objRandomGenerator.clear();
	for(_nTempCounter=0;_nTempCounter<_nNumberOfAngles;_nTempCounter++)
	{
		_objRandomGenerator.append(_nTempCounter * (360/_nNumberOfAngles));
	}
	BrainStim.write2OutputWindow("\t-Block Angles: " + _objRandomGenerator.randomize(), "MovingBarsRetinotopy");
	return _objRandomGenerator.toStringList();
}

function _generateAngleTriggerSteps()
{
	_objRandomGenerator.clear();
	for(_nTempCounter=0;_nTempCounter<BasicExperiment.nMovingBarCycleAmount;_nTempCounter++)
	{
		_objRandomGenerator.append(_nTempCounter);
	}
	BrainStim.write2OutputWindow("\t-Angle Steps: " + _objRandomGenerator.randomize(), "MovingBarsRetinotopy");
	return _objRandomGenerator.toStringList();
}

function _initializePRTFile()
{
	var bRetVal = _objPRTFormatManager.setParameterValue("ResolutionOfTime", "Volumes");
	bRetVal = bRetVal && _objPRTFormatManager.setParameterValue("Experiment", _sPrtExperimentFileName);
	bRetVal = bRetVal && _objPRTFormatManager.setParameterValue("BackgroundColor", "0 0 0");
	bRetVal = bRetVal && _objPRTFormatManager.setParameterValue("TextColor", "255 255 255");
	bRetVal = bRetVal && _objPRTFormatManager.setParameterValue("TimeCourseColor", "255 255 255");
	bRetVal = bRetVal && _objPRTFormatManager.setParameterValue("TimeCourseThick", "3");
	bRetVal = bRetVal && _objPRTFormatManager.setParameterValue("ReferenceFuncColor", "0 0 80");
	bRetVal = bRetVal && _objPRTFormatManager.setParameterValue("ReferenceFuncThick", "3");
	
	for(_nTempCounter=0;_nTempCounter<BasicExperiment.nMovingBarCycleAmount;_nTempCounter++)
	{	
		_arrPrtFileConditions[_nTempCounter] = _objPRTFormatManager.appendCondition("Step_" + _nTempCounter, _objPRTFormatManager.getRGBPaletteColorString(BasicExperiment.nMovingBarCycleAmount + 1, _nTempCounter));
	}
	return bRetVal;
}

function _ValidateCreateMeanImageAndRename()
{
	_SelectedGeneratedPngDirectory = QFileDialog.getExistingDirectory(this, "Select a directory containing the image source file(s)", _sOutputFileDirectory, QFileDialog.Options(CreateMeanImage_searchDialogOptions));
	if(_SelectedGeneratedPngDirectory != "")
	{
		CreateMeanImage_process(_SelectedGeneratedPngDirectory,_sOutputFileDirectory + "Mean_" + _sPrtExperimentFileName + ".png");
		
		var _fileToRenameArray = [];
		_fileToRenameArray = _FileRenaming_parseDirectory(_SelectedGeneratedPngDirectory + "/");
		BrainStim.write2OutputWindow("Found Files(total amount): " + _fileToRenameArray.length);
		if(_FileRenaming_renameFiles(_fileToRenameArray))
		{
			BrainStim.write2OutputWindow("Successfull Renaming Operation!");
			//Validate and Show the results
			_FileRenaming_sAllowedFileExtensions = [_FileRenaming_sNewFilePrefix + "*.png"];//Set for the new renamed file-name convention to search for....
			var _filesToCheckArray = [];
			_filesToCheckArray = _FileRenaming_parseDirectory(_SelectedGeneratedPngDirectory + "/");
			QMessageBox.information(this, "PNG Generation results", "In total there are " + _filesToCheckArray.length + " PNG files made available for analyzing, this amount should correspondent with the total amount of volumes scanned!\nIf not then try to slow down the PNG file generation by incrementing the _generateModeTriggerDuration script variabele.");
		}
		else
		{
			BrainStim.write2OutputWindow("Something failed while renaming!");
			QMessageBox.critical(this, "PNG Generation results","Error: Something failed while renaming the files!");
		}
		
		BrainStim.write2OutputWindow("\n");
		_FileRenaming_cleanupScript();	
	}
}

function _savePRTFile()
{
	BrainStim.write2OutputWindow("Finalizing PRT file", "MovingBarsRetinotopy");
	var nTempStepValue;
	var nVolumeCounter = 1;//Starts with 1 (not 0!)
	var nConditionFix = _objPRTFormatManager.appendCondition("Fixation", _objPRTFormatManager.getRGBPaletteColorString(BasicExperiment.nMovingBarCycleAmount + 1, BasicExperiment.nMovingBarCycleAmount));
	_objPRTFormatManager.appendInterval(nConditionFix, nVolumeCounter, nVolumeCounter + 7);
	nVolumeCounter = nVolumeCounter + 7;//First 8 fixation triggers!
	var _nNumberOfLoops = _getNumberOfLoopRepetitions();
	if(_nNumberOfLoops<0)
	{
		BrainStim.write2OutputWindow("PRT file FAILED, could not fetch the number of loop repetitions!", "MovingBarsRetinotopy");
		return false;
	}
	var _nNumberOfAngleSessions = _nNumberOfLoops + 1;
	for (var nSessionCounter=0;nSessionCounter<_nNumberOfAngleSessions;nSessionCounter++)
	{
		for (var nTrialCounter=0;nTrialCounter<_arrPreservedAngleSteps.length;nTrialCounter++)
		{
			for (var nStepCounter=0;nStepCounter<_arrPreservedAngleSteps[nTrialCounter].length;nStepCounter++)
			{
				nVolumeCounter++;
				nTempStepValue = _arrPreservedAngleSteps[nTrialCounter][nStepCounter];
				_objPRTFormatManager.appendInterval(_arrPrtFileConditions[nTempStepValue], nVolumeCounter, nVolumeCounter);
			}
		}
	}
	nVolumeCounter++;
	_objPRTFormatManager.appendInterval(nConditionFix, nVolumeCounter, nVolumeCounter + 7);
	
	var bRetVal  = _objPRTFormatManager.saveFile(_sPrtFileDirectory + _sPrtExperimentFileName + ".prt", true);
	if(bRetVal)
	{
		BrainStim.write2OutputWindow("PRT file successfully saved as : " + _sPrtExperimentFileName + ".prt", "MovingBarsRetinotopy");
	}
	else
	{
		BrainStim.write2OutputWindow("PRT file FAILED to save!", "MovingBarsRetinotopy");
	}
	return bRetVal;
}

function _getDialogComboBoxItem(title,text,items,current)
{
	var tstDialog = new QInputDialog();
	tstDialog.setWindowTitle(title);
	tstDialog.setComboBoxItems (items);
	tstDialog.setTextValue(items[current]);
	tstDialog.setLabelText(text);
	var bResult = tstDialog.exec();
	var sResult = tstDialog.textValue();
	tstDialog = null;
	if(bResult == QDialog.Accepted)
		return sResult;
	return null;
}

function _getDialogOpenFileNames(_sTitle, _sPath, _sExtensionList)
{
	//_sExtensionList = "All Files (*);;Text Files (*.txt)";
	var options = 0;
	//if (!this.useNative.checked)
	//	options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	var files = QFileDialog.getOpenFileNames(
				this, _sTitle,
				_sPath,
				_sExtensionList,
				selectedFilter,
				options);
	if (files.length != 0) 
		return files[0];
		//this.openFileNamesLabel.text = "[" + files.join(", ") + "]";
	return "";
}

function __Cleanup()
{
	CreateMeanImage_CleanupScript();	
	_fBinaryStimulusFramesOutputFile = null;
	_dirLogFilePath = null;
	_dirPrtFilePath = null;
	_dirInputFilePath = null;
	RandomGeneratorObj = null;
	_objPRTFormatManager = null;
	_getDialogComboBoxItem = null;
	_getDialogOpenFileNames = null;
	_generateBlockAngles = null;
	_getNumberOfLoopRepetitions = null;
	_generateAngleTriggerSteps = null;
	_initializePRTFile = null;
	_savePRTFile = null;
	__Cleanup = null;
	BrainStim.write2OutputWindow("--- __Cleanup() successfully executed", "MovingBarsRetinotopy");
	BrainStim.restartScriptEngine();
}

///////////////////////////////////////////////////////////Run the experiment//////////////////////////////////////////////////////
_sSelectedExperimentType = _getDialogComboBoxItem("Choose what to do next...","Action: ",_arrSelectedExperimentTypes,0);
if(_sSelectedExperimentType == null)
{
	__Cleanup();
}
else
{
	if((_sSelectedExperimentType == _arrSelectedExperimentTypes[0]) || (_sSelectedExperimentType == _arrSelectedExperimentTypes[1]))//Run the experiment
	{
		if(_sSelectedExperimentType == _arrSelectedExperimentTypes[1])//Run the experiment, select the previous session here...
		{
			_sBinaryStimulusFramesInputFilePath = _getDialogOpenFileNames("Open the input file", _sInputFileDirectory, "All Files (*);;Text Files (*.txt)");
			if(_sBinaryStimulusFramesInputFilePath == "")
			{
				__Cleanup();
			}
			else
			{
				Include(_sBinaryStimulusFramesInputFilePath);//Important, here we include the input file!!
			}
		}
		if(_initializePRTFile() == false)
		{
			__Cleanup();
		}
		else
		{
			BasicExperiment.RunExperiment(_bSkipXMLValidation);
		}
	}
	else//Create the Stimulus Binairy Frames
	{
		BasicExperiment.nTestModeTriggerDuration = _generateModeTriggerDuration;
		BasicExperiment.bShowFixPoint = false;
		BasicExperiment.nStimWidth = BasicExperiment.nStimWidth/_nPngGenerationDownScaleFactor;
		BasicExperiment.nStimHeight = BasicExperiment.nStimHeight/_nPngGenerationDownScaleFactor;
		BasicExperiment.bOutputTriggerFrame = true;
		BasicExperiment.sOutputFrameType = "Mask";
		BasicExperiment.sOutputFrameFormat = "PNG";
		var _FileRenaming_sAllowedFileExtensions = ["*_*_*_*_*.png"];
		if(_sBinaryStimulusFramesInputFilePath == "")
		{
			_sBinaryStimulusFramesInputFilePath = _getDialogOpenFileNames("Open the input file", _sInputFileDirectory, "All Files (*);;Text Files (*.txt)");
		}
		if(_sBinaryStimulusFramesInputFilePath == "")
		{
			__Cleanup();
		}
		else
		{
			Include(_sBinaryStimulusFramesInputFilePath);//Important, here we include the input file!!
			BasicExperiment.RunExperiment(_bSkipXMLValidation);
		}
	}
}


