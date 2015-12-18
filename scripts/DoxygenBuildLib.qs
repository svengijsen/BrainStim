Include("common.qs");
Include("doxygen/GenerateDoxyConfigFile.qs");//Make sure to call the cleanup when not needed anymore!
Include(BrainStim_Settings_JSConfigurationFilePath);

//Global vars....
var DoxyBuildLib_InstallProcess = new QProcess();
DoxyBuildLib_InstallProcess.setProcessChannelMode(QProcess.MergedChannels);

QByteArray.prototype.toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function DoxyBuildLib_CleanUpScript()
{
	Log("CleanUpScript started...");
	DoxyBuildLib_ConnectDisconnectScriptFunctions(false);
	GDCF_Cleanup();
	BrainStim_Cleanup();
	tr=null;
	tmpByteArray=null;
	tmpStringList=null;
	tmpString=null;
	DoxyBuildLib_ExecuteBinairy=null;
	DoxyBuildLib_ProcessReadyRead=null;
	QByteArray.prototype.toString=null;
	DoxyBuildLib_InstallProcess=null;
	DoxyBuildLib_ConnectDisconnectScriptFunctions=null;
	DoxyBuildLib_ProcessFinished=null;
	DoxyBuildLib_CleanUpScript=null;
	DoxyBuildLib_GetString = null;
	Log("CleanUpScript exited Successfully!");
	Beep();
	BrainStim.cleanupScript();
}

function DoxyBuildLib_ConnectDisconnectScriptFunctions(Connect)
{
	if(Connect)
	{
		Log("Connecting Signal/Slots...");
		try 
		{	
			DoxyBuildLib_InstallProcess.readyRead.connect(this, this.DoxyBuildLib_ProcessReadyRead);	
			DoxyBuildLib_InstallProcess["finished(int,QProcess::ExitStatus)"].connect(this, this.DoxyBuildLib_ProcessFinished);	 
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
			DoxyBuildLib_InstallProcess.readyRead.disconnect(this, this.DoxyBuildLib_ProcessReadyRead);
			DoxyBuildLib_InstallProcess.finished.disconnect(this, this.DoxyBuildLib_ProcessFinished);	 
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}

function tr(s) { return s; }

function DoxyBuildLib_GetString(title,text,defaultValue)
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

function DoxyBuildLib_ProcessReadyRead()
{
	tmpByteArray = DoxyBuildLib_InstallProcess.readAll();
	Log(tmpByteArray.toString());	
}

function DoxyBuildLib_ProcessFinished()
{
	Log("DoxyBuildLib_ProcessFinished() called!");
	Log("** STEP "  + nCounter + " ** PASSED!");
	bFileProccessed = true;
	if(bDoCleanup)
	{
		DoxyBuildLib_CleanUpScript();
	}
}

function DoxyBuildLib_ExecuteBinairy(binPath, argList)
{
	DoxyBuildLib_InstallProcess.start(binPath,argList);
	for(var i=0;i<argList.length;i++)
		binPath = binPath + " " + argList[i];
	Log("\n\t>> " + binPath + "\n");
	return DoxyBuildLib_InstallProcess.waitForStarted();
}

DoxyBuildLib_ConnectDisconnectScriptFunctions(true);
