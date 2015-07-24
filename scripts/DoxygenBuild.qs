Include("common.qs");
Include("doxygen/GenerateDoxyConfigFile.qs");//Make sure to call the cleanup when not needed anymore!
Include(BrainStim_Settings_JSConfigurationFilePath);

var tmpString;
var InstallProcess = new QProcess();
var tmpByteArray = new QByteArray();
var tmpStringList =  new Array();
var tmpString = new String();
var sBinairySteps;
var bDoCleanup;
var bFileProccessed;
var nCounter;
var bSkipStep;
var fileDest;
var changeSet;
var fileSource = "doxygen/PluginTemplate.cfg";
var preFix = "-*/*-";
var postFix = "-*/*-";
var sScriptPath = BrainStim.getActiveDocumentFileLocation();
var sBinairyPath = "doxygen";
var sDoxyFileDir = sScriptPath + "/";
var BrainStim_Info = new BrainStim_Information();
var LastComponentIndex;
var LastComponentReleaseIndex;
var LastComponentVersion;
var ComponentName;

InstallProcess.setProcessChannelMode(QProcess.MergedChannels);

QByteArray.prototype.toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function doxyCleanUpScript()
{
	Log("CleanUpScript started...");
	ConnectDisconnectScriptFunctions(false);
	GDCF_Cleanup();
	BrainStim_Cleanup();
	tr=null;
	tmpByteArray=null;
	tmpStringList=null;
	tmpString=null;
	ExecuteBinairy=null;
	processReadyRead=null;
	QByteArray.prototype.toString=null;
	InstallProcess=null;
	ConnectDisconnectScriptFunctions=null;
	processFinished=null;
	doxyCleanUpScript=null;
	getString = null;
	//BrainStim_Cleanup();
	Log("CleanUpScript exited Successfully!");
	Beep();
	BrainStim.cleanupScript();
}

function ConnectDisconnectScriptFunctions(Connect)
{
	if(Connect)
	{
		Log("Connecting Signal/Slots...");
		try 
		{	
			InstallProcess.readyRead.connect(this, this.processReadyRead);	
			InstallProcess["finished(int,QProcess::ExitStatus)"].connect(this, this.processFinished);	 
		} 
		catch (e) 
		{
			Log(".*. Something went wrong connecting the Signal/Slots:" + e);
		}		
	}
	else
	{
		Log("Disconnecting Signal/Slots...");
		try 
		{	
			InstallProcess.readyRead.disconnect(this, this.processReadyRead);
			InstallProcess.finished.disconnect(this, this.processFinished);	 
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}

function tr(s) { return s; }

function getString(title,text,defaultValue)
{
	tmpString = QInputDialog.getText(this, tr(title), tr(text), QLineEdit.Normal, tr(defaultValue), Qt.WindowFlags(0));
	if (tmpString != null && tmpString.length != 0)
	{
		return tr(tmpString);
	}
	else
	{	
		return tr("");
	}
}

function processReadyRead()
{
	tmpByteArray = InstallProcess.readAll();
	Log(tmpByteArray.toString());	
}

function processFinished()
{
	Log("processFinished() called!");
	Log("** STEP "  + nCounter + " ** PASSED!");
	bFileProccessed = true;
	if(bDoCleanup)
	{
		doxyCleanUpScript();
	}
}

function ExecuteBinairy(binPath, argList)
{
	InstallProcess.start(binPath,argList);
	for(var i=0;i<argList.length;i++)
		binPath = binPath + " " + argList[i];
	Log("\n\t>> " + binPath + "\n");
	return InstallProcess.waitForStarted();
}

ConnectDisconnectScriptFunctions(true);

bDoCleanup = false;
sBinairySteps = 3;
for(nCounter=1;nCounter<=sBinairySteps;nCounter++)
{
	bFileProccessed = false;
	bSkipStep = false;
	
	Log("** STEP "  + nCounter + " **");
	
		
	changeSet = BrainStim_CreateArray(8,2);//(sBinairySteps+1,2); //See the doxygen template document for parameter descriptions!!
	if(nCounter==1)
	{
		ComponentName = "ExperimentManagerPlugin";		
		Log("Component Name: " + ComponentName);
		LastComponentReleaseIndex = BrainStim_Info.GetLatestComponentReleaseByName(ComponentName);
		Log("Last Component Release Index: " + LastComponentReleaseIndex);
		LastComponentIndex = BrainStim_Info.GetLatestComponentIndexByName(LastComponentReleaseIndex,ComponentName);
		Log("Last Component Index: " + LastComponentIndex);
		LastComponentVersion = BrainStim_Info.GetComponentVersionByIndexes(LastComponentReleaseIndex,LastComponentIndex);
		Log("Last Component Version: " + LastComponentVersion);
		Log("\n");
		fileDest = "generated/" + ComponentName + ".cfg";
		changeSet[0][0] = preFix + "PROJECT_NAME" + postFix;
		changeSet[0][1] = "\"ExperimentManager script class\"";
		changeSet[1][0] = preFix + "PROJECT_NUMBER" + postFix;
		changeSet[1][1] = "\"" + LastComponentVersion + "\"";
		changeSet[2][0] = preFix + "PROJECT_BRIEF" + postFix;
		changeSet[2][1] = "\"The Experiment Manager script class reference.\"";
		changeSet[3][0] = preFix + "PROJECT_LOGO" + postFix;
		changeSet[3][1] = "";//No quotes for empty!
		changeSet[4][0] = preFix + "OUTPUT_DIRECTORY" + postFix;
		changeSet[4][1] = "\"../documents/ExperimentManagerPlugin\"";
		changeSet[5][0] = preFix + "INPUT" + postFix;
		changeSet[5][1] = BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ExperimentManager.cpp \\\n" +
		                          BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ExperimentManager.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/QML2Viewer.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/QML2Viewer.cpp \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/RetinotopyMapper.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/RetinotopyMapper.cpp \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/TriggerTimer.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/TriggerTimer.cpp \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ExperimentTimer.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ExperimentTimer.cpp \\\n" +					  
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/prtformatmanager.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/prtformatmanager.cpp \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ImageProcessor.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ImageProcessor.cpp \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/RandomGenerator.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/RandomGenerator.cpp \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ExperimentStructures.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/ExperimentEngine.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/Global.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/Plugins/ExperimentManager/qml2interface.h \\\n";
		changeSet[6][0] = preFix + "QHP_NAMESPACE" + postFix;
		changeSet[6][1] = "BrainStim.doxygen.Project";
		changeSet[7][0] = preFix + "PREDEFINED" + postFix; //see http://www.stack.nl/~dimitri/doxygen/preprocessing.html
		changeSet[7][1] = "";		
	}
	else if (nCounter==2)
	{
		ComponentName = "BrainStim_QMLExtensions";		
		Log("Component Name: " + ComponentName);
		LastComponentReleaseIndex = BrainStim_Info.GetLatestComponentReleaseByName(ComponentName);
		Log("Last Component Release Index: " + LastComponentReleaseIndex);
		LastComponentIndex = BrainStim_Info.GetLatestComponentIndexByName(LastComponentReleaseIndex,ComponentName);
		Log("Last Component Index: " + LastComponentIndex);
		LastComponentVersion = BrainStim_Info.GetComponentVersionByIndexes(LastComponentReleaseIndex,LastComponentIndex);
		Log("Last Component Version: " + LastComponentVersion);
		Log("\n");
		fileDest = ComponentName + ".cfg";
		changeSet[0][0] = preFix + "PROJECT_NAME" + postFix;
		changeSet[0][1] = "\"BrainStim QML Extensions\"";
		changeSet[1][0] = preFix + "PROJECT_NUMBER" + postFix;
		changeSet[1][1] = "\"" + LastComponentVersion + "\"";
		changeSet[2][0] = preFix + "PROJECT_BRIEF" + postFix;
		changeSet[2][1] = "\"The BrainStim QML Extensions class references.\"";
		changeSet[3][0] = preFix + "PROJECT_LOGO" + postFix;
		changeSet[3][1] = "";//No quotes for empty!
		changeSet[4][0] = preFix + "OUTPUT_DIRECTORY" + postFix;
		changeSet[4][1] = "\"../documents/BrainStim_QMLExtensions\"";
		changeSet[5][0] = preFix + "INPUT" + postFix;
		changeSet[5][1] = BrainStim_Settings_ProjectDirectory + "/Source/QmlExtensions/Plugins/DefaultPlugin/ogreitem.h \\\n" +		
					  BrainStim_Settings_ProjectDirectory + "/Source/QmlExtensions/Plugins/DefaultPlugin/OgreCameraNode.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/QmlExtensions/Plugins/DefaultPlugin/OgreLightNode.h \\\n" +
					  //BrainStim_Settings_ProjectDirectory + "/Source/QmlExtensions/Plugins/DefaultPlugin/TimeModel.h \\\n" +
		                          BrainStim_Settings_ProjectDirectory + "/Source/QmlExtensions/Plugins/DefaultPlugin/defines.h \\\n";
		changeSet[6][0] = preFix + "QHP_NAMESPACE" + postFix;
		changeSet[6][1] = "BrainStim.doxygen.Project";
		changeSet[7][0] = preFix + "PREDEFINED" + postFix; //see http://www.stack.nl/~dimitri/doxygen/preprocessing.html //see http://www.stack.nl/~dimitri/doxygen/preprocessing.html
		changeSet[7][1] = "";
	}
	else if (nCounter==3)
	{
		ComponentName = "BrainStimMainApplication";		
		//Log("Component Name: " + ComponentName);
		LastComponentVersion = BrainStim_Info.GetCurrentRelease();
		Log("BrainStim Release Index: " + LastComponentVersion);
		//LastComponentIndex = BrainStim_Info.GetLatestComponentIndexByName(LastComponentReleaseIndex,ComponentName);
		//Log("Last Component Index: " + LastComponentIndex);
		//LastComponentVersion = BrainStim_Info.GetComponentVersionByIndexes(LastComponentReleaseIndex,LastComponentIndex);
		//Log("Last Component Version: " + LastComponentVersion);
		Log("\n");
		fileDest = ComponentName + ".cfg";
		changeSet[0][0] = preFix + "PROJECT_NAME" + postFix;
		changeSet[0][1] = "\"BrainStim Main Application\"";
		changeSet[1][0] = preFix + "PROJECT_NUMBER" + postFix;
		changeSet[1][1] = "\"" + LastComponentVersion + "\"";
		changeSet[2][0] = preFix + "PROJECT_BRIEF" + postFix;
		changeSet[2][1] = "\"The BrainStim Main Application class references.\"";
		changeSet[3][0] = preFix + "PROJECT_LOGO" + postFix;
		changeSet[3][1] = "";//No quotes for empty!
		changeSet[4][0] = preFix + "OUTPUT_DIRECTORY" + postFix;
		changeSet[4][1] = "\"../documents/BrainStim\"";
		changeSet[5][0] = preFix + "INPUT" + postFix;
		changeSet[5][1] = BrainStim_Settings_ProjectDirectory + "/Source/BrainStim/mainwindow.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/BrainStim/mainwindow.cpp \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/BrainStim/GlobalApplicationInformation.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/BrainStim/archiver.h \\\n" +
					  BrainStim_Settings_ProjectDirectory + "/Source/BrainStim/scifinddialog.h \\\n";
//		changeSet[6][0] = preFix + "QHP_NAMESPACE" + postFix;
//		changeSet[6][1] = "BrainStim.doxygen.Project";
//		changeSet[7][0] = preFix + "PREDEFINED" + postFix; //see http://www.stack.nl/~dimitri/doxygen/preprocessing.html //see http://www.stack.nl/~dimitri/doxygen/preprocessing.html
//		changeSet[7][1] = "";		
	}	
	else
	{
		bSkipStep = true;
	}
	
	if(!bSkipStep)
	{
		GDCF_ReplaceInFiles(fileSource,fileDest,changeSet);
		//doxygen -w html header.html footer.html stylesheet.css <config_file>
		tmpStringList = [];//Reset list
		tmpStringList[0] = sDoxyFileDir + fileDest;
	}
	
	if(!bSkipStep)
	{
		if(ExecuteBinairy(sBinairyPath,tmpStringList))
		{
			Log("ExecuteBinairy started Successfully!");
			if(nCounter == sBinairySteps)
				bDoCleanup = true;
			while(!bFileProccessed)
			{
				BrainStim.processEvents();
				//Log("Waiting...");
			}
		}
		else
		{
			Log("ExecuteBinairy Failed!");
			doxyCleanUpScript();
		}
	}
	else
	{
		Log("** STEP "  + nCounter + " ** SKIPPED");
		continue;
	}
}
