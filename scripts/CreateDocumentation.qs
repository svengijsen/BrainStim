var tmpString;
var InstallProcess = new QProcess();
var tmpByteArray = new QByteArray();
var tmpStringList =  [];
var tmpString = new String();
var tmpFile = new QFile();
var sBinairySteps;
var bDoCleanup;
var bFileProccessed;
var nCounter;
var bSkipStep;

var sQTDirWin32 = tr("E:\\Libraries\\Qt5.3.2_32bit\\5.3\\msvc2013_opengl\\");
var sQTDirx64 = tr("E:\\Libraries\\Qt5.3.2_64bit\\5.3\\msvc2013_64_opengl\\");
var sMainDocName = tr("MainDocumentation");
var sScriptPath = BrainStim.getActiveDocumentFileLocation() + "/";
var sBinairyPath;

var sInstallerAppName = tr("BrainStim");//BrainStim
var sInstallerConfiguration = tr("win32");//win32 or x64
var sInstallerVersion = tr("1.0.0.1");//<Major>.<Minor>.<Build>.<Revision>
var sInstallerPlatform = tr("Windows");

//var sQtDirectory_default = tr("C:/Qt/4.8.0");
//var sQtDirectory = sQtDirectory_default;
var sBrainStimProjectDirectory_default = tr("E:\\Projects\\BrainStim");
var sBrainStimProjectDirectory = sBrainStimProjectDirectory_default;

InstallProcess.setProcessChannelMode(QProcess.MergedChannels);

QByteArray.prototype.toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function ToWindowsPath(path)
{
	return path.replace( /\//gi,"\\");//  first argument = /regex/ (g=all occurrences, i=case insensitive), second = <replace_string>
}

function CleanUpScript()
{
	Log("CleanUpScript started...");
	ConnectDisconnectScriptFunctions(false);
	ToWindowsPath=null;
	tr=null;
	tmpFile=null;
	tmpByteArray=null;
	tmpStringList=null;
	tmpString=null;
	ExecuteBinairy=null;
	QByteArray.prototype.toString=null;
	InstallProcess=null;
	ConnectDisconnectScriptFunctions=null;
	processFinished=null;
	DeleteFile=null;
	processReadyRead=null;
	getString = null;
	CleanUpScript=null;	
	Log("CleanUpScript exited successfully!");
	//Beep();
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
	//Log(__qt_sender__.workingDirectory());
	//Log(this.name);
	//Log(arguments.callee);
	//Log("processFinished() proccessed Successfully (" + __qt_sender__.name() + ", " + this.name + ", " + arguments.callee + ").");
	Log("** STEP "  + nCounter + " ** PASSED!");
	bFileProccessed = true;
	if(bDoCleanup)
	{
		CleanUpScript();
	}
}

function DeleteFile(sPath)
{
	tmpFile.setFileName(sPath);
	if(tmpFile.exists())
	{
		tmpFile.remove();
		if(tmpFile.exists()==false)
			Log("Successfully deleted file: " + sPath);
	}
}

function ExecuteBinairy(binPath, argList)
{
	InstallProcess.start(binPath,argList,QIODevice.ReadWrite);//sQTDirWin32 + "bin/qhelpgenerator", tmpStringList);
	var sCommand = binPath;
	for(var i=0;i<argList.length;i++)
		sCommand = sCommand + " " + argList[i];
	Log("\n\t>> " + sCommand + "\n");
	return InstallProcess.waitForStarted();
}

ConnectDisconnectScriptFunctions(true);
//Log(getString("This is an Title", "Some Text....","default x"));


//Name				Extension	Brief Description
//Qt Help Project			.qhp		The input file for the help generator consisting of the table of contents, indices and references to the actual documentation files (*.html); 
//							it also defines a unique namespace for the documentation.
//Qt Compressed Help		.qch		The output file of the help generator. This binary file contains all information specified in the help project file along with all compressed documentation files.
//Qt Help Collection Project	.qhcp		The input file for the help collection generator. It contains references to compressed help files which should be included in the collection; 
//							it also may contain other information for customizing Qt Assistant.
//Qt Help Collection		.qhc		The output of the help collection generator. This is the file QHelpEngine operates on. It contains references to any number of compressed help files as well as additional information, 
//							such as custom filters.
bDoCleanup = false;
sBinairySteps = 9;
sScriptPath = ToWindowsPath(sScriptPath);//Important!
//sQtDirectory = getString("Choose the Qt directory","Qt directory:",sQtDirectory_default);
//Log("Qt directory = " + sQtDirectory);
sBrainStimProjectDirectory = getString("Choose the BrainStim project directory","BrainStim project directory:",sBrainStimProjectDirectory);
Log("BrainStim project directory = " + sBrainStimProjectDirectory);
for(nCounter=1;nCounter<=sBinairySteps;nCounter++)
{
	bFileProccessed = false;
	bSkipStep = false;
	
	Log("** STEP "  + nCounter + " **, from " + sBinairySteps + " step(s)");
	if (nCounter==1)
	{
		bSkipStep = true;//see below for alternative code!
		DeleteFile(sScriptPath + sMainDocName + ".qhc");
	}
	else if (nCounter==2)
	{
		bSkipStep = true;//see below for alternative code!
		DeleteFile(sScriptPath + sMainDocName + ".qch");
	}
	else if(nCounter==3)
	{
		//qhelpgenerator doc.qhp -o doc.qch
		tmpStringList = [];//Reset list
		tmpStringList[0] = sScriptPath + sMainDocName + ".qhp";
		tmpStringList[1] = "-o"
		tmpStringList[2] = sScriptPath + sMainDocName + ".qch";//	Log(tmpStringList.length);
		sBinairyPath = sQTDirWin32 + "bin/qhelpgenerator";
		bSkipStep = true;
	}
	else if (nCounter==4)
	{
	//	qcollectiongenerator mycollection.qhcp -o mycollection.qhc
		tmpStringList = [];//Reset list
		tmpStringList[0] = sScriptPath + sMainDocName + ".qhcp";
		tmpStringList[1] = "-o"
		tmpStringList[2] = sScriptPath + sMainDocName + ".qhc";
		sBinairyPath = sQTDirWin32 + "bin/qcollectiongenerator";
	}
	else if (nCounter==5)
	{
		bSkipStep = true;//see below for alternative code!
		DeleteFile(sBrainStimProjectDirectory + "\\Install\\documents\\"  + sMainDocName + ".qhc");
	}
	else if (nCounter==6)
	{
		bSkipStep = true;//see below for alternative code!
		DeleteFile(sBrainStimProjectDirectory + "\\Install\\documents\\"  + sMainDocName + ".qch");
	}
	else if (nCounter==7)
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sScriptPath + sMainDocName + ".qhc";//Xcopy only works with "\" for directories!
		tmpStringList[1] = sBrainStimProjectDirectory + "\\Install\\documents\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==8)
	{
		tmpStringList = [];//Reset list
		tmpStringList[0] = sScriptPath + sMainDocName + ".qch";
		tmpStringList[1] = sBrainStimProjectDirectory + "\\Install\\documents\\";
		tmpStringList[2] = "/Y";
		tmpStringList[3] = "/F";
		sBinairyPath = "xcopy";
	}
	else if (nCounter==9)
	{
		//assistant -collectionFile MainDocumentation.qhc
		tmpStringList = [];//Reset list
		tmpStringList[0] = "-collectionFile";
		//tmpStringList[1] = sScriptPath + sMainDocName + ".qhc";
		tmpStringList[1] = sBrainStimProjectDirectory + "\\Install\\documents\\"  + sMainDocName + ".qhc";
		sBinairyPath = "assistant";		
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
			Log("\n+++++++++++ExecuteBinairy Failed!++++++++++++++\n");
			CleanUpScript();
			break;
		}
	}
	else
	{
		Log("** STEP "  + nCounter + " ** SKIPPED");
		if(nCounter == sBinairySteps)
			CleanUpScript();			
		continue;
	}
}

//@echo off
//CALL .\bin\assistant.exe -collectionFile .\documents\MainDocumentation.qhc -enableRemoteControl


