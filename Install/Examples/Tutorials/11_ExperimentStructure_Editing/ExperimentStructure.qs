var dialog;
var cExperimentStructure_Object;
var cBlockStructure_Object0;
var cBlockStructure_Object1;
var cBlockStructure_Object2;
var cBlockStructure_Object3;
var cBlockStructure_Object4;
var cLoopStructure_Object0;
var cLoopStructure_Object1;
var cLoopStructure_Object2;
var cLoopStructure_Object3;
var cLoopStructure_Object4;
var cLoopStructure_Object;
var cExperimentStructureState_Object;
var sTimer = new TriggerTimer();
var ExperimentManagerObj = new ExperimentManager(); 						//Here we create the Experiment Manager object that can run experiments.
var sCurrentScriptLocation = StimulGL.getActiveDocumentFileLocation();			//Here we store the directory-path from this script file for further usage.
var sExperimentFilePath = sCurrentScriptLocation + "/ExperimentStructure2.exml";
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
	this.showNodeButton = new QPushButton(tr("Show Experiment(node editor)"));	
	layout.addWidget(this.showNodeButton, 3, 0);
	layout.addWidget(this.showNodeLabel, 3, 1);
	/////////////////////////////////////////////////////
	this.showTreeLabel = new QLabel;
	this.showTreeLabel.setFrameStyle(frameStyle);
	this.showTreeButton = new QPushButton(tr("Show Experiment(tree editor)"));	
	layout.addWidget(this.showTreeButton, 4, 0);
	layout.addWidget(this.showTreeLabel, 4, 1);
	/////////////////////////////////////////////////////	
	this.executeLabel = new QLabel;
	this.executeLabel.setFrameStyle(frameStyle);
	this.executeButton = new QPushButton(tr("Execute Experiment"));	
	layout.addWidget(this.executeButton, 5, 0);
	layout.addWidget(this.executeLabel, 5, 1);
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
	//var items = Array[tr("Spring"), tr("Summer"), tr("Fall"), tr("Winter")];
	//var item = QInputDialog.getItem(this, tr("QInputDialog::getItem()"), tr("Season:"), items, 0, false, Qt.WindowFlags(0));
	//if (item != null & item.length != 0)
		//this.itemLabel.text = item;
	
	if(!ExperimentManagerObj.loadExperiment(sExperimentFilePath,true))
	{
		Log("~~Failed to load the Experiment file");
	}
	else
	{
		cExperimentStructure_Object = ExperimentManagerObj.getExperimentStructure();//new cExperimentStructure(ExperimentManagerObj.getExperimentStructure());//ExperimentManagerObj.getExperimentStructure();
		Log(cExperimentStructure_Object);
		Log(cExperimentStructure_Object.ExperimentName);
		dialog.loadLabel.text = sExperimentFilePath;
	}	
}

Dialog.prototype.createExperiment = function()
{
	CreateExperimentFromScript();
	Log(cExperimentStructure_Object);
	Log(cExperimentStructure_Object.ExperimentName);	
}

Dialog.prototype.changeExperiment = function()
{
	if(cExperimentStructure_Object != null)
	{
		if(cBlockStructure_Object0 == null)
		{
			cBlockStructure_Object0 = cExperimentStructure_Object.getBlockPointerByID(0);
		}
		Log("Current block name: " + cBlockStructure_Object0.BlockName);
		cBlockStructure_Object0.BlockName = "Custom Block Name";
		//cLoopStructure_Object2 = new cLoopStructure();
		//CreateLoop(cLoopStructure_Object2,2,1,cBlockStructure_Object0.getBlockID());
		//Log(">> Add a defined Loop result: " + cBlockStructure_Object0.insertLoop(cLoopStructure_Object2));
		Log("SetExperiment: " + ExperimentManagerObj.setExperimentStructure(cExperimentStructure_Object));
	}
	else
	{
		Log("No Experiment configured...")
	}	
}

Dialog.prototype.showExperimentTree = function()
{
	Log("showExperimentTree()");
	//Log(ExperimentManagerObj.Test("abc"));
	//bool ExperimentManager::loadExperiment(QString strSource, bool bViewEditTree, bool bIsFile)
	//ExperimentManagerObj.runExperiment(); //Try to run the experiment
	if(!ExperimentManagerObj.showVisualExperimentEditor(cExperimentStructure_Object)) //cExperimentStructure_Object1
	{
		Log("~~Failed to show the Experiment Structure");
	}	
//	LogState(cExperimentStructure_Object.getCurrentExperimentState());	
}

Dialog.prototype.showExperiment = function()
{
	if(!ExperimentManagerObj.showVisualExperimentEditor(cExperimentStructure_Object)) //cExperimentStructure_Object1
	{
		Log("~~Failed to show the Experiment Structure");
	}	
//	LogState(cExperimentStructure_Object.getCurrentExperimentState());	
}

Dialog.prototype.executeExperiment = function()
{
	if(!ExperimentManagerObj.runExperiment()) //Try to run the experiment
	{
		Log("~~Failed to execute the Experiment");
	}
	else
	{
		ConnectDisconnectExperimentStructure(true);
	}	
}

Dialog.prototype.saveExperiment = function()
{
	if(!ExperimentManagerObj.saveExperiment(sExperimentFilePathSaved)) //Try to run the experiment
	{
		Log("~~Failed to save the Experiment");
	}
	else
	{
		Log("Experiment Saved!");
	}	
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
				sTimer.timeout.connect(cExperimentStructure_Object, cExperimentStructure_Object.incrementExternalTrigger);
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
				sTimer.timeout.disconnect(cExperimentStructure_Object, cExperimentStructure_Object.incrementExternalTrigger);
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
			dialog.showNodeButton["clicked()"].connect(dialog, dialog.showExperiment);
			dialog.showTreeButton["clicked()"].connect(dialog, dialog.showExperimentTree);			
			dialog.executeButton["clicked()"].connect(dialog, dialog.executeExperiment);
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
			dialog.showNodeButton["clicked()"].disconnect(dialog, dialog.showExperiment);
			dialog.showTreeButton["clicked()"].disconnect(dialog, dialog.showExperimentTree);				
			dialog.executeButton["clicked()"].disconnect(dialog, dialog.executeExperiment);
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
	}		
}

function LogState(currentExperimentState)
{
	Log("-----");
	var tmpLoopID = currentExperimentState.CurrentBlock_LoopID;
	if (tmpLoopID == -4)
		Log("CurrentBlock_LoopID: RA_UNDEFINED");
	else
		Log("CurrentBlock_LoopID:" + currentExperimentState.CurrentBlock_LoopID);
	if(cLoopStructure_Object0)
		Log("CurrentBlock_LoopCounter0:" + cLoopStructure_Object0.getLoopCounter());
	if(cLoopStructure_Object1)
		Log("CurrentBlock_LoopCounter1:" + cLoopStructure_Object1.getLoopCounter());
	if(cLoopStructure_Object2)
		Log("CurrentBlock_LoopCounter2:" + cLoopStructure_Object2.getLoopCounter());
	Log("CurrentBlock_BlockID:" + currentExperimentState.CurrentBlock_BlockID);
	Log("CurrentBlock_TrialNumber:" + currentExperimentState.CurrentBlock_TrialNumber);
	Log("CurrentBlock_InternalTrigger:" + currentExperimentState.CurrentBlock_InternalTrigger);
	Log("CurrentBlock_ExternalTrigger:" + currentExperimentState.CurrentBlock_ExternalTrigger);
	Log("Experiment_ExternalTrigger:" + currentExperimentState.Experiment_ExternalTrigger);
	Log("-----");
}

function ExperimentStateChanged()
{
	Log("-O- Experiment State Changed -O-");
	//LogState(cExperimentStructure_Object.getCurrentExperimentState());
}

function ExperimentStarted()
{
	Log("-*- Experiment Started");
}

function ExperimentStopped()
{
	Log("-*- Experiment Stopped");
}

function CreateBlock(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
{	
	//BlockObject = new cBlockStructure();	
	BlockObject.BlockName = "Block " + ID;
	//Log("Block Name: " + BlockObject.BlockName);
	BlockObject.BlockID = ID;
	//Log("Block ID: " + BlockObject.BlockID);
	BlockObject.BlockNumber = ID;
	//Log("Block Number: " + BlockObject.BlockNumber);
	BlockObject.NumberOfTrials = Trials;
	//Log("Block Number Of Trials: " + BlockObject.NumberOfTrials);
	BlockObject.NumberOfInternalTriggers = IntTriggers;
	//Log("Block Number Of Internal Triggers: " + BlockObject.NumberOfInternalTriggers);
	BlockObject.NumberOfExternalTriggers = ExtTriggers;
	//Log("Block Number Of External Triggers: " + BlockObject.NumberOfExternalTriggers);
	//return BlockObject;
	Log("\t-*- new Block Created");
}

function CreateLoop(LoopObject,ID,NumberOfLoops,TargetBlockID)
{
	//LoopObject = new cLoopStructure();	
	LoopObject.LoopName = "Loop " + ID;
	//Log("Loop Name: " + LoopObject.LoopName);
	LoopObject.LoopID = ID;
	//Log("Loop ID: " + LoopObject.LoopID);
	LoopObject.LoopNumber = ID;
	//Log("Loop Number: " + LoopObject.LoopNumber);
	LoopObject.NumberOfLoops = NumberOfLoops;
	//Log("Loop Number Of Loops: " + LoopObject.NumberOfLoops);
	LoopObject.TargetBlockID = TargetBlockID;
	//Log("Loop Target Block ID: " + LoopObject.TargetBlockID);
	Log("\t-*- new Loop Created");
}

function CreateExperimentFromScript()
{
	cExperimentStructure_Object = new cExperimentStructure();
	Log("-*- Going to create a new experiment from script code");
	cExperimentStructure_Object.ExperimentName = "Test Experiment";
	Log("Experiment Name: " + cExperimentStructure_Object.ExperimentName);
	cExperimentStructure_Object.ExperimentID = 99;
	Log("Experiment ID: " + cExperimentStructure_Object.ExperimentID);
	cExperimentStructure_Object.ExperimentDebugMode = true;
	Log("Experiment Debug Mode: " + cExperimentStructure_Object.ExperimentDebugMode);

	cBlockStructure_Object0 = new cBlockStructure();
	CreateBlock(cBlockStructure_Object0,0,2,5,2);//(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
	var bResult = cExperimentStructure_Object.insertBlock(cBlockStructure_Object0);
	//Log("Add a defined Block result: " + bResult);

	cBlockStructure_Object1 = new cBlockStructure();
	CreateBlock(cBlockStructure_Object1,1,1,1,1);//(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
	bResult = cExperimentStructure_Object.insertBlock(cBlockStructure_Object1);
	//Log("Add a defined Block result: " + bResult);
	cBlockStructure_Object2 = new cBlockStructure();
	CreateBlock(cBlockStructure_Object2,2,1,1,1);//(BlockObject,ID,Trials,IntTriggers,ExtTriggers)
	bResult = cExperimentStructure_Object.insertBlock(cBlockStructure_Object2);
	//Log("Add a defined Block result: " + bResult);
	
	cLoopStructure_Object0 = new cLoopStructure();
	CreateLoop(cLoopStructure_Object0,0,2,cBlockStructure_Object0.getBlockID());//(LoopObject,ID,NumberOfLoops,TargetBlockID)
	bResult = cBlockStructure_Object1.insertLoop(cLoopStructure_Object0);
	//Log("Add a defined Loop result: " + bResult);
	//Log("\tLoop Count: " + cBlockStructure_Object1.getLoopCount());	

	Log("-*- Experiment Created");
}

function CleanupScript()
{
	//Pre
	sTimer.stopTimer();
	dialog.close();
	ConnectDisconnectExperimentStructure(false);
	ConnectDisconnectScriptFunctions(false);
	//Functions
	CreateExperimentFromScript = null;
	LogState = null;	
	ExperimentStarted = null;
	ExperimentStopped = null;
	ExperimentStateChanged = null;
	CreateBlock = null;
	CreateLoop = null;
	EM_ExperimentStateChanged = null;
	EM_ExternalTriggerIncremented = null;	
	ConnectDisconnectScriptFunctions = null;
	ConnectDisconnectExperimentStructure = null;
	CleanupScript = null;	
	//Objects
	cLoopStructure_Object0 = null;
	cLoopStructure_Object1 = null;
	cLoopStructure_Object2 = null;
	cLoopStructure_Object3 = null;
	cLoopStructure_Object4 = null;	
	cBlockStructure_Object0 = null;
	cBlockStructure_Object1 = null;
	cBlockStructure_Object2 = null;
	cBlockStructure_Object3 = null;
	cBlockStructure_Object4 = null;
	cExperimentStructure_Object = null;	
	cExperimentStructureState_Object = null;
	ExperimentManagerObj = null;
	sTimer = null;	
	ExperimentManagerObj = null;
	dialog = null;
	//Dialog
	Dialog.prototype.keyPressEvent = null;
	Dialog.prototype.closeEvent = null;	
	Dialog.prototype.loadExperiment = null;
	Dialog.prototype.createExperiment = null;
	Dialog.prototype.changeExperiment = null;
	Dialog.prototype.showExperiment = null;
	Dialog.prototype.showExperimentTree = null;
	Dialog.prototype.executeExperiment = null;
	Dialog.prototype.saveExperiment = null;
	Dialog.prototype = null;
	Dialog = null;
	tr = null;	
	//Post
	Log("\nFinished script cleanup, ready for garbage collection!");
	Beep();
	StimulGL.cleanupScript();	
}

sTimer.startTimer(1000);
dialog = new Dialog();
dialog.show();
ConnectDisconnectScriptFunctions(true);
//
dialog.loadExperiment();
dialog.showExperimentTree();

