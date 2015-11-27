var dialog;
var cExperimentStructure_Object;
var cExperimentStructureState_Object;
var ExperimentManagerObj = new ExperimentManager(); 						//Here we create the Experiment Manager object that can run experiments.
var sCurrentScriptLocation = BrainStim.getActiveDocumentFileLocation();			//Here we store the directory-path from this script file for further usage.
var sExperimentFilePath = sCurrentScriptLocation + "/ExperimentStructure.exml";
var sExperimentFilePathSaved = sCurrentScriptLocation + "/ExperimentStructure_saved.exml";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function Dialog(parent)
{
	QDialog.call(this, parent);
	var frameStyle = QFrame.Sunken | QFrame.Panel;
	var layout = new QGridLayout;
	layout.setColumnStretch(1, 1);	
	layout.setColumnMinimumWidth(1, 250);
	
	this.loadLabel = new QLabel;
	this.loadLabel.setFrameStyle(frameStyle);
	this.loadButton = new QPushButton(tr("Load Experiment"));	
	layout.addWidget(this.loadButton, 0, 0);
	layout.addWidget(this.loadLabel, 0, 1);
	/////////////////////////////////////////////////////
	this.createLabel = new QLabel;
	this.createLabel.setFrameStyle(frameStyle);
	this.createButton = new QPushButton(tr("Create Experiment(script)"));	
	layout.addWidget(this.createButton, 1, 0);
	layout.addWidget(this.createLabel, 1, 1);
	/////////////////////////////////////////////////////
	this.changeLabel = new QLabel;
	this.changeLabel.setFrameStyle(frameStyle);
	this.changeButton = new QPushButton(tr("Change Experiment(script)"));	
	layout.addWidget(this.changeButton, 2, 0);
	layout.addWidget(this.changeLabel, 2, 1);
	/////////////////////////////////////////////////////
	this.showNodeLabel = new QLabel;
	this.showNodeLabel.setFrameStyle(frameStyle);
	this.showGraphEditorButton = new QPushButton(tr("Show Experiment(graph editor)"));	
	layout.addWidget(this.showGraphEditorButton, 3, 0);
	layout.addWidget(this.showNodeLabel, 3, 1);
	/////////////////////////////////////////////////////
	this.showTreeLabel = new QLabel;
	this.showTreeLabel.setFrameStyle(frameStyle);
	this.showTreeEditorButton = new QPushButton(tr("Show Experiment(table editor)"));	
	layout.addWidget(this.showTreeEditorButton, 4, 0);
	layout.addWidget(this.showTreeLabel, 4, 1);
	/////////////////////////////////////////////////////	
	this.runLabel = new QLabel;
	this.runLabel.setFrameStyle(frameStyle);
	this.runButton = new QPushButton(tr("Run Experiment"));	
	layout.addWidget(this.runButton, 5, 0);
	layout.addWidget(this.runLabel, 5, 1);
	/////////////////////////////////////////////////////
	this.saveLabel = new QLabel;
	this.saveLabel.setFrameStyle(frameStyle);
	this.saveButton = new QPushButton(tr("Save Experiment"));	
	layout.addWidget(this.saveButton, 6, 0);
	layout.addWidget(this.saveLabel, 6, 1);
	/////////////////////////////////////////////////////
	this.exitLabel = new QLabel;
	this.exitLabel.setFrameStyle(frameStyle);
	this.exitButton = new QPushButton(tr("Exit"));	
	layout.addWidget(this.exitButton, 7, 0);
	layout.addWidget(this.exitLabel, 7, 1);

	this.setLayout(layout);
	this.windowTitle = tr("Menu Dialog");
}

Dialog.prototype = new QDialog();

function tr(s) { return s; }

Dialog.prototype.loadExperiment = function()
{
	ChangeDialogState("Load");
	if(!ExperimentManagerObj.loadExperiment(sExperimentFilePath,true))
	{
		Log("~~Failed to load the Experiment file");
	}
	else
	{
		cExperimentStructure_Object = ExperimentManagerObj.getExperimentStructure();
		dialog.loadLabel.text = sExperimentFilePath;
	}	
	ChangeDialogState("Idle");
}

Dialog.prototype.createExperiment = function()
{
	ChangeDialogState("Create");
	cExperimentStructure_Object = ExperimentManagerObj.getExperimentStructure(true);
	if(cExperimentStructure_Object != null)
	{
		cExperimentStructure_Object.clearExperiment();
		cExperimentStructure_Object.setExperimentName("Script Created Experiment");
		cExperimentStructure_Object.setExperimentDebugMode(true);
		cExperimentStructure_Object.setExperimentID(0);
		
		var tmpBlock0 = cExperimentStructure_Object.createBlock();//returns a new cBlockStructure()
		tmpBlock0.setBlockID(0);
		tmpBlock0.setBlockNumber(0);
		tmpBlock0.setBlockName("first block");
		tmpBlock0.setNumberOfTrials(2);
		tmpBlock0.setNumberOfInternalTriggers(3);
		tmpBlock0.setNumberOfExternalTriggers(4);
		Log("> Block inserted: " + tmpBlock0.getBlockName());

		var tmpBlock1 = cExperimentStructure_Object.createBlock();//returns a new cBlockStructure()
		tmpBlock1.setBlockID(1);
		tmpBlock1.setBlockNumber(1);
		tmpBlock1.setBlockName("second block");
		tmpBlock1.setNumberOfTrials(5);
		tmpBlock1.setNumberOfInternalTriggers(5);
		tmpBlock1.setNumberOfExternalTriggers(5);
		Log("> Block inserted: " + tmpBlock1.getBlockName());
		
		var tmpObject1 = new cObjectStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
		tmpObject1.setObjectID(1);
		tmpObject1.setObjectName("RetinoMap_RenderWidgetGL");
		tmpObject1.setObjectClass("RetinotopyMapper");
		Log("> Insert object: " + cExperimentStructure_Object.insertObject(tmpObject1));
		
		var tmpObject0 = new cObjectStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
		tmpObject0.setObjectID(0);
		tmpObject0.setObjectName("Timer_Object_1");
		tmpObject0.setObjectClass("TriggerTimer");
		Log("> Insert object: " + cExperimentStructure_Object.insertObject(tmpObject0));
		
		var tmpObjInit0 = new cMethodStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
		tmpObjInit0.setMethodID(0);
		tmpObjInit0.setMethodObjectID(0);
		tmpObjInit0.setMethodOrderNumber(0);//do not specify or set to -1 for auto append
		tmpObjInit0.setMethodType(2);//0=undefined, 1=signal, 2=slot
		tmpObjInit0.setMethodSignature("startTimer");
		var tmpMethodParam0 = new cMethodParameterStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
		tmpMethodParam0.setMethodParameterID(0);
		tmpMethodParam0.setMethodParameterName("msec");
		tmpMethodParam0.setMethodParameterType("double");
		tmpMethodParam0.setMethodParameterValue(500);
		Log("> Insert object initialization parameter: " + tmpObjInit0.insertMethodParameter(tmpMethodParam0));
		Log("> Insert object initialization: " + cExperimentStructure_Object.insertObjectInitialization(tmpObjInit0));
		
		var tmpObjFinit0 = new cMethodStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
		tmpObjFinit0.setMethodID(0);
		tmpObjFinit0.setMethodObjectID(0);
		tmpObjFinit0.setMethodOrderNumber(-1);//do not specify or set to -1 for auto append
		tmpObjFinit0.setMethodType(2);//0=undefined, 1=signal, 2=slot
		tmpObjFinit0.setMethodSignature("stopTimer");
		Log("> Insert object finalization: " + cExperimentStructure_Object.insertObjectFinalization(tmpObjFinit0));
		
		var tmpObjConn0 = new cMethodConnectionStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
		//tmpObjConn0.setMethodConnectionID(-1);//do not specify or set to -1 for auto numbering
		tmpObjConn0.setSourceObjectID(0);
		tmpObjConn0.setSourceMethodType(1);//0=undefined, 1=signal, 2=slot
		tmpObjConn0.setSourceSignature("timeout()");
		tmpObjConn0.setTargetObjectID(1);
		tmpObjConn0.setTargetMethodType(2);//0=undefined, 1=signal, 2=slot
		tmpObjConn0.setTargetSignature("incrementExternalTrigger()");
		Log("> Insert objects connection: " + cExperimentStructure_Object.insertObjectMethodConnection(tmpObjConn0));
		
		var tmpBlockParam0 = new cBlockParameterStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
		tmpBlockParam0.setBlockParameterID(0);
		tmpBlockParam0.setBlockParameterName("RetinoPattern");
		tmpBlockParam0.setBlockParameterValue("fixation");
		tmpBlockParam0.setBlockParameterIsCustom(false);
		var bIsCustom = false;
		Log("> Insert block parameter: " + tmpBlock0.insertObjectParameter(tmpObject1.getObjectID(), tmpBlockParam0, bIsCustom));
		
		var tmpLoop0 = new cLoopStructure();
		tmpLoop0.setLoopID(0);
		tmpLoop0.setLoopNumber(0);
		tmpLoop0.setLoopName("LoopName_1");
		tmpLoop0.setNumberOfLoops(99);
		tmpLoop0.setTargetBlockID(0);
		Log("> Insert loop: " + tmpBlock0.insertLoop(tmpLoop0));
	}
	//Now we only need to force the parsing of the changed or newly created experiment to apply the new settings.
	Log(">> Parse Current ExperimentStructure: " + ExperimentManagerObj.parseCurrentExperimentStructure());
	ChangeDialogState("Idle");
}

Dialog.prototype.changeExperiment = function()
{
	ChangeDialogState("Change");
	if(cExperimentStructure_Object != null)
	{
		var tmpBlock0 = cExperimentStructure_Object.getBlockPointerByID(0);
		tmpBlock0.setBlockName("new block name");
		if(tmpBlock0)
		{
			var tmpBlockParam0 = new cBlockParameterStructure();//although created here in the script, do not delete this object, this is automatically done by the Experiment Structure!
			//tmpBlockParam0.setBlockParameterID(-1); //We don't need to specify this value
			tmpBlockParam0.setBlockParameterName("RetinoPattern");
			tmpBlockParam0.setBlockParameterValue("polarangle");
			tmpBlockParam0.setBlockParameterIsCustom(false);
			var bIsCustom = false;
			var nObjectID = 1;
			Log("> Insert block parameter: " + tmpBlock0.insertObjectParameter(nObjectID, tmpBlockParam0, bIsCustom));
			//Now we only need to force the parsing of the changed or newly created experiment to apply the new settings.
			Log(">> Parse Current ExperimentStructure: " + ExperimentManagerObj.parseCurrentExperimentStructure());
		}
	}
	else
	{
		Log("No Experiment configured...");
	}	
	ChangeDialogState("Idle");
}

Dialog.prototype.showTreeViewEditor = function()
{
	Log("showTreeViewEditor()");
	if(!ExperimentManagerObj.showTreeviewExperimentDialog(cExperimentStructure_Object))
	{
		Log("~~Failed to show the Experiment Structure");
	}	
//	LogState(cExperimentStructure_Object.getCurrentExperimentState());	
}

Dialog.prototype.showGraphEditor = function()
{
	if(!ExperimentManagerObj.showVisualExperimentDialog(cExperimentStructure_Object)) //cExperimentStructure_Object1
	{
		Log("~~Failed to show the Experiment Structure");
	}	
//	LogState(cExperimentStructure_Object.getCurrentExperimentState());	
}

Dialog.prototype.runExperiment = function()
{
	ChangeDialogState("Run");
	if(!ExperimentManagerObj.runExperiment()) //Try to run the experiment
	{
		Log("~~Failed to run the Experiment");
		ChangeDialogState("Idle");
	}
	else
	{
		ConnectDisconnectExperimentStructure(true);
	}	
}

Dialog.prototype.saveExperiment = function()
{
	ChangeDialogState("Save");
	if(!ExperimentManagerObj.saveExperiment(sExperimentFilePathSaved)) //Try to run the experiment
	{
		Log("~~Failed to save the Experiment");
	}
	else
	{
		Log("Experiment Saved!");
	}	
	ChangeDialogState("Idle");
}

Dialog.prototype.keyPressEvent = function(e /*QKeyEvent e*/)
{
	if(e.key() == Qt.Key_Escape)
	{
		this.close();
	}
	else
	{
		QDialog.keyPressEvent(e);
	}
}

Dialog.prototype.closeEvent = function() 
{
	Log("Dialog closeEvent() detected!");
	CleanupScript();
}

function ConnectDisconnectExperimentStructure(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(cExperimentStructure_Object)
	{
		if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
		{
			try 
			{	
				cExperimentStructure_Object.experimentStarted.connect(this, ExperimentStarted);
				cExperimentStructure_Object.experimentStopped.connect(this, ExperimentStopped);
				cExperimentStructure_Object.externalTriggerRecieved.connect(this, ExperimentStateChanged);
			} 
			catch (e) 
			{
				Log(".*. Something went wrong connecting the Experiment Structure:" + e); //If a connection fails warn the user!
			}
		}
		else
		{
			try 
			{	
				cExperimentStructure_Object.experimentStarted.disconnect(this, ExperimentStarted);
				cExperimentStructure_Object.experimentStopped.disconnect(this, ExperimentStopped);
				cExperimentStructure_Object.externalTriggerRecieved.disconnect(this, ExperimentStateChanged);
			} 
			catch (e) 
			{
				Log(".*. Something went wrong disconnecting the Experiment Structure:" + e); //If a disconnection fails warn the user!
			}		
		}
	}
}

function ConnectDisconnectScriptFunctions(Connect)
//This function can connect or disconnect all signal/slot connections defined by the boolean parameter 
{
	if(Connect) //This parameter defines whether we should connect or disconnect the signal/slots.
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			ExperimentManagerObj.ExperimentStateHasChanged.connect(this, this.EM_ExperimentStateChanged);  
			ExperimentManagerObj.WriteToLogOutput.connect(this,this.Log);
				
			dialog.loadButton["clicked()"].connect(dialog, dialog.loadExperiment);
			dialog.createButton["clicked()"].connect(dialog, dialog.createExperiment);
			dialog.changeButton["clicked()"].connect(dialog, dialog.changeExperiment);
			dialog.showGraphEditorButton["clicked()"].connect(dialog, dialog.showGraphEditor);
			dialog.showTreeEditorButton["clicked()"].connect(dialog, dialog.showTreeViewEditor);			
			dialog.runButton["clicked()"].connect(dialog, dialog.runExperiment);
			dialog.saveButton["clicked()"].connect(dialog, dialog.saveExperiment);
			dialog.exitButton["clicked()"].connect(this, this.CleanupScript);			
		} 
		catch (e) 
		{
			Log(".*. Something went wrong connecting the Signal/Slots:" + e); //If a connection fails warn the user!
		}
	}
	else
	{
		Log("... Disconnecting Signal/Slots");
		try 
		{	
			ExperimentManagerObj.ExperimentStateHasChanged.disconnect(this, this.EM_ExperimentStateChanged);  
			ExperimentManagerObj.WriteToLogOutput.disconnect(this,this.Log);
			
			dialog.loadButton["clicked()"].disconnect(dialog, dialog.loadExperiment);
			dialog.createButton["clicked()"].disconnect(dialog, dialog.createExperiment);
			dialog.changeButton["clicked()"].disconnect(dialog, dialog.changeExperiment);
			dialog.showGraphEditorButton["clicked()"].disconnect(dialog, dialog.showGraphEditor);
			dialog.showTreeEditorButton["clicked()"].disconnect(dialog, dialog.showTreeViewEditor);				
			dialog.runButton["clicked()"].disconnect(dialog, dialog.runExperiment);
			dialog.saveButton["clicked()"].disconnect(dialog, dialog.saveExperiment);
			dialog.exitButton["clicked()"].disconnect(this, this.CleanupScript);			
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e); //If a disconnection fails warn the user!
		}		
	}	
}

function EM_ExternalTriggerIncremented() 
{
	Log("*** EM_ExternalTriggerIncremented()");
	//LogState(cExperimentStructure_Object.getCurrentExperimentState());
}

function EM_ExperimentStateChanged(currentEMState,timestamp)
{
	Log("*** EM_ExperimentStateChanged() to:" + currentEMState + " @" + timestamp);
	if(currentEMState == ExperimentManager.ExperimentState.ExperimentManager_Stopped)	//ExperimentManager_Stopped
	{
		ConnectDisconnectExperimentStructure(false);
		ChangeDialogState("Idle");
	}		
}

function LogState(currentExperimentState)
{
//	Log("-----");
//	var tmpLoopID = currentExperimentState.CurrentBlock_LoopID;
//	if (tmpLoopID == -4)
//		Log("CurrentBlock_LoopID: RA_UNDEFINED");
//	else
//		Log("CurrentBlock_LoopID:" + currentExperimentState.CurrentBlock_LoopID);
//	if(cLoopStructure_Object0)
//		Log("CurrentBlock_LoopCounter0:" + cLoopStructure_Object0.getLoopCounter());
//	if(cLoopStructure_Object1)
//		Log("CurrentBlock_LoopCounter1:" + cLoopStructure_Object1.getLoopCounter());
//	if(cLoopStructure_Object2)
//		Log("CurrentBlock_LoopCounter2:" + cLoopStructure_Object2.getLoopCounter());
//	Log("CurrentBlock_BlockID:" + currentExperimentState.CurrentBlock_BlockID);
//	Log("CurrentBlock_TrialNumber:" + currentExperimentState.CurrentBlock_TrialNumber);
//	Log("CurrentBlock_InternalTrigger:" + currentExperimentState.CurrentBlock_InternalTrigger);
//	Log("CurrentBlock_ExternalTrigger:" + currentExperimentState.CurrentBlock_ExternalTrigger);
//	Log("Experiment_ExternalTrigger:" + currentExperimentState.Experiment_ExternalTrigger);
//	Log("-----");
}

function ExperimentStateChanged()
{
	Log("-O- Experiment State Changed -O-");
	LogState(cExperimentStructure_Object.getCurrentExperimentState());
}

function ExperimentStarted()
{
	Log("-*- Experiment Started");
}

function ExperimentStopped()
{
	Log("-*- Experiment Stopped");
}

function CleanupScript()
{
	//Pre
	dialog.close();
	ConnectDisconnectExperimentStructure(false);
	ConnectDisconnectScriptFunctions(false);
	//Functions
	ChangeDialogState = null;
	LogState = null;	
	ExperimentStarted = null;
	ExperimentStopped = null;
	ExperimentStateChanged = null;
	EM_ExperimentStateChanged = null;
	EM_ExternalTriggerIncremented = null;	
	ConnectDisconnectScriptFunctions = null;
	ConnectDisconnectExperimentStructure = null;
	CleanupScript = null;	
	//Objects
	cExperimentStructureState_Object = null;
	ExperimentManagerObj = null;
	dialog = null;
	//Dialog
	Dialog.prototype.keyPressEvent = null;
	Dialog.prototype.closeEvent = null;	
	Dialog.prototype.loadExperiment = null;
	Dialog.prototype.createExperiment = null;
	Dialog.prototype.changeExperiment = null;
	Dialog.prototype.showGraphEditor = null;
	Dialog.prototype.showTreeViewEditor = null;
	Dialog.prototype.runExperiment = null;
	Dialog.prototype.saveExperiment = null;
	Dialog.prototype = null;
	Dialog = null;
	tr = null;	
	//Post
	Log("\nFinished script cleanup, ready for garbage collection!");
	//Beep();
	BrainStim.cleanupScript();	
}

function ChangeDialogState(sType)
{
	//sType corresponds to a button:
	//"Load", "Create", "Change", "Run", "Save", "Idle"
	if(sType == "Load")
	{
		dialog.setWindowTitle("BrainStim - Loading...");
		dialog.setEnabled(false);
	}
	else if(sType == "Create")
	{
		dialog.setWindowTitle("BrainStim - Creating...");
		dialog.setEnabled(false);
	}
	else if(sType == "Change")
	{
		dialog.setWindowTitle("BrainStim - Applying changes...");
		dialog.setEnabled(false);
	}
	else if(sType == "Run")
	{
		dialog.setWindowTitle("BrainStim - Running...");
		dialog.setEnabled(false);
	}
	else if(sType == "Save")
	{
		dialog.setWindowTitle("BrainStim - Saving...");
		dialog.setEnabled(false);
	}
	else if(sType == "Idle")
	{
		dialog.setWindowTitle("BrainStim - Idle...");
		dialog.setEnabled(true);
	}
}

dialog = new Dialog();
dialog.show();
ConnectDisconnectScriptFunctions(true);

