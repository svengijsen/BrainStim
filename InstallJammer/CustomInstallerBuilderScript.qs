Include("../Install/Include/QtScript/ModuleReplaceInFile.qs");
Include("../Install/Include/QtScript/ModuleXMLReader.qs");

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
var sBinairyPath;

var sScriptPath = ToWindowsPath(BrainStim.getActiveDocumentFileLocation());
var sInstallJammerPath = BrainStim.getEnvironmentVariabele("PROGRAMFILES(x86)") + "/InstallJammer/installjammer.exe";
var strInstallConfigurationFileName = "InstallJammer_(32+64bit)";
var strInstallConfigurationFile = ToWindowsPath(BrainStim.getActiveDocumentFileLocation() + "/" + strInstallConfigurationFileName + ".mpi");
var strInstallConfigurationFileCopy = sScriptPath + "\\" + strInstallConfigurationFileName + "_tmpCopy.mpi" ;
var sInstallerAppName = tr("BrainStim");//BrainStim
var arrConfigList = new Array("win32","x64");
var sInstallerConfiguration = tr(arrConfigList[0]);//win32 or x64
var sInstallerVersion = tr("1.1.0.1");//<Major>.<Minor>.<Build>.<Revision>
var sInstallerPlatform = tr("Windows");



var installJammerChangesCount = 10;
var installJammerProjectFileChanges = ReplaceInFile_CreateArray(2*installJammerChangesCount, 2);
//-------per change-(x0 = 32, x1 = 64)---------
//[change+0][x] = new replacement string(32-bit or 64-bit)
//[change+1][x] = jammer-file string to replace(32-bit or 64-bit)
//----------------------------------------------
var installJammerProjectFileFromToChangesSelection = ReplaceInFile_CreateArray(installJammerChangesCount,2);
//-------per change----------------------------
//[change+0] = final from replacement string(32-bit and 64-bit)
//[change+1] = final to replacement string(32-bit and 64-bit)
//----------------------------------------------
var nCurrentChangeCounter = 0;
var nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
var sLibraryDir = getXmlFileItemValue("..\\Source\\base.props", ["Project","PropertyGroup","LIBRARY_DIR"]);
if(typeof sLibraryDir === "undefined")
	sLibraryDir = "E:/Libraries";
else
	Log("Found Library directory: " + sLibraryDir);
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0] = tr(sLibraryDir + "/Qt5.3.2_32bit/5.3/msvc2013_opengl");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0] = installJammerProjectFileChanges[0][0];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1] = tr(sLibraryDir + "/Qt5.3.2_64bit/5.3/msvc2013_64_opengl");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0];
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0] = tr(sLibraryDir + "/Qt5.3.2_ScriptBindings/Win32/bindings/script");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1] = tr(sLibraryDir + "/Qt5.3.2_ScriptBindings/x64/bindings/script");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0];
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = tr("E:/Projects/BrainStim");//From
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0];//To

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0] = tr(sLibraryDir + "/Firebird/Firebird-2.5.3.26778-0/Win32/Release");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1] = tr(sLibraryDir + "/Firebird/Firebird-2.5.3.26778-0/x64/Release");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0];
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0] = tr(sLibraryDir + "/InpOut_1500/Win32");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1] = tr(sLibraryDir + "/InpOut_1500/x64");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0];
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0] = tr(sLibraryDir + "/QScintilla-gpl-2.7.2/lib/Win32/Release");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1] = tr(sLibraryDir + "/QScintilla-gpl-2.7.2/lib/x64/Release");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0];
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0] = tr(sLibraryDir + "/Ogre/OGRE-SDK-1.8.2-vc110-x86-28.05.2013/bin/Release");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1] = tr(sLibraryDir + "/Ogre/OGRE-SDK-1.8.2-vc110-x64-28.05.2013/bin/Release");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0];
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0] = tr(sLibraryDir + "/HIDAPI/Win32/bin");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1] = tr(sLibraryDir + "/HIDAPI/x64/bin");
installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][1];//make sure this is also defined like this in the default installjammer file
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter+1][0];
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileChanges[nCurrentDoubleChangeCounter][0];

nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = tr(sLibraryDir + "/VC_Redist/VC_2013");//From
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0];//To
nCurrentChangeCounter = nCurrentChangeCounter + 1;
nCurrentDoubleChangeCounter = nCurrentChangeCounter*2;
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0] = tr(sLibraryDir + "/VC_Redist/VC_2013/x86/Microsoft.VC120");//From
installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][1] = installJammerProjectFileFromToChangesSelection[nCurrentChangeCounter][0];//To

var srcFile = new QFile(strInstallConfigurationFile);
var dstFile = new QFile(strInstallConfigurationFileCopy);

QByteArray.prototype.toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function CleanUpScript()
{
	Log("CleanUpScript started...");
	ConnectDisconnectScriptFunctions(false);
	ReplaceInFile_Cleanup();
	cleanupXmlFileReader();
	tr=null;
	tmpByteArray=null;
	tmpStringList=null;
	tmpString=null;
	ExecuteBinairy=null;
	ToWindowsPath = null;
	informationMessage=null;
	processReadyRead=null;
	selectItem=null;
	QByteArray.prototype.toString=null;
	InstallProcess=null;
	srcFile = null;
	dstFile = null;
	ConnectDisconnectScriptFunctions=null;
	processFinished=null;
	CleanUpScript=null;	
	Log("CleanUpScript exited successfully!");
	BrainStim.cleanupScript();
}

function getItem(title,text,items)
{
	var item = QInputDialog.getItem(this, tr(title), tr(text), items, 0, false, Qt.WindowFlags(0));
	if (item != null & item.length != 0)
		return item;
}

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

function informationMessage(message)
{
    var reply = QMessageBox.information(this, tr("Information"), tr(message));//Dialog.MESSAGE);
    if (reply == QMessageBox.Ok)
        //this.informationLabel.text = tr("OK");
	return true;
    else
        //this.informationLabel.text = tr("Escape");
    return false;
}

function ToWindowsPath(path)
{
	return path.replace( /\//gi,"\\");//  first argument = /regex/ (g=all occurrences, i=case insensitive), second = <replace_string>
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

function ExecuteBinairy(binPath, argList)
{
	InstallProcess.start(binPath,argList);
	return InstallProcess.waitForStarted();
}

ConnectDisconnectScriptFunctions(true);
//Log(getString("This is an Title", "Some Text....","default x"));

bDoCleanup = false;
sBinairySteps = 6;

for(nCounter=1;nCounter<=sBinairySteps;nCounter++)
{
	bFileProccessed = false;
	bSkipStep = false;
	
	Log("** STEP "  + nCounter + " **");
	
	if (nCounter==1)
	{
		
		sInstallerVersion = getString("Set the BrainStim version","BrainStim version:",sInstallerVersion);
		Log("BrainStim version = " + sInstallerVersion);
		
		sInstallerConfiguration = getItem("Choose the BrainStim configuration","BrainStim configuration:",arrConfigList);
		Log("BrainStim configuration = " + sInstallerConfiguration);
		
		if(sInstallerConfiguration == "x64")
		{
			installJammerProjectFileFromToChangesSelection[0][0] = installJammerProjectFileChanges[1][1];
			installJammerProjectFileFromToChangesSelection[0][1] = installJammerProjectFileChanges[0][1];
			installJammerProjectFileFromToChangesSelection[1][0] = installJammerProjectFileChanges[3][1];
			installJammerProjectFileFromToChangesSelection[1][1] = installJammerProjectFileChanges[2][1];
		}
		
		installJammerProjectFileFromToChangesSelection[0][1] = getString("Choose the Qt directory","Qt directory:",installJammerProjectFileFromToChangesSelection[0][1]);
		Log("Qt directory = " + installJammerProjectFileFromToChangesSelection[0][1]);

		installJammerProjectFileFromToChangesSelection[1][1]  = getString("Choose the Qt bindings directory","Qt bindings directory:",installJammerProjectFileFromToChangesSelection[1][1]);
		Log("Qt bindings directory = " + installJammerProjectFileFromToChangesSelection[1][1]);
		
		installJammerProjectFileFromToChangesSelection[2][1] = getString("Choose the BrainStim project directory","BrainStim project directory:",installJammerProjectFileFromToChangesSelection[2][1]);
		Log("BrainStim project directory = " + installJammerProjectFileFromToChangesSelection[2][1]);	
		
		if(getItem("Start Installer Building?","Do you really whish to proceed?:", Array("Yes","No")) == "No")
			nCounter = sBinairySteps;
		
		bSkipStep = true;
	}	
	else if(nCounter==2)
	{
		if(dstFile.exists())
			dstFile.remove();//Copy Doesn't Overwrite!
		//if (srcFile.copy(strInstallConfigurationFileCopy))//Copy Doesn't Overwrite!
		//	Log("File (" + strInstallConfigurationFile + ") copied!");
		ReplaceInFile_ReplaceInFiles(strInstallConfigurationFile,strInstallConfigurationFileCopy,installJammerProjectFileFromToChangesSelection);//
		bSkipStep = true;
	}
	else if (nCounter==3)
	{
		informationMessage("Please remove the unwanted component(s) from the copied installer script file (except the Default and the [" + sInstallerConfiguration + "] components.");
		tmpStringList = new Array();//Reset list
		tmpStringList[0] = strInstallConfigurationFileCopy;
		sBinairyPath = sInstallJammerPath;
		//bSkipStep = true;
	}
	else if (nCounter==4)
	{
		tmpStringList = new Array();//Reset list
		tmpStringList[0] = "-DAppName";
		tmpStringList[1] = sInstallerAppName;
		tmpStringList[2] = "-DPlatformName";
		tmpStringList[3] = sInstallerConfiguration;
		tmpStringList[4] = "-DInstallType";
		tmpStringList[5] = sInstallerConfiguration;
		tmpStringList[6] = "-DInstallVersion";
		tmpStringList[7] = sInstallerVersion;
		tmpStringList[8] = "-DVersion"
		tmpStringList[9] = sInstallerVersion;
		tmpStringList[10] = "--platform"
		tmpStringList[11] = sInstallerPlatform;
		//Insert new options here!	
		tmpStringList[12] = "--build-for-release";
		tmpStringList[13] = "--build";
		tmpStringList[14] = strInstallConfigurationFileCopy;
		sBinairyPath = sInstallJammerPath;
		//bSkipStep = true;
	}
	else if (nCounter==5)
	{
		if(dstFile.exists())
		{
			if (dstFile.remove())//Copy Doesn't Overwrite!
				Log("Copied file removed");
			else
				Log("Couldn't remove the Copied file!");
		}
		else
		{
			Log("File doesn't exist!");
		}
		bSkipStep = true;
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
