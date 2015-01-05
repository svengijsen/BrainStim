var KeyBoardCaptureObj = new KeyBoardCapture();
var currentScriptLocation = StimulGL.getActiveDocumentFileLocation();
var tmpTimeObject;
var LastMeasuredTime;
var timespanInMS;
var meanTime = 0;
var minTime = 0;
var maxTime = 0;
var callCounter = 0;
var myTimer;
var nTimerInterval = 5;
var nFirstIterationsToIgnore;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function setupTimer(sType)
{
	nFirstIterationsToIgnore = 5;
	if(sType == "QTimer")
	{
		myTimer = new QTimer();
		myTimer.interval = nTimerInterval; // set the time in milliseconds
		myTimer.singleShot = false; // in-case if setTimout and false in-case of setInterval 
		myTimer.timeout.connect(this, TimerInvoked);
		myTimer.start();
	}
	else if((sType == "QPCNew_TriggerTimerType") || (sType == "QPC_TriggerTimerType") || (sType == "QTimer_TriggerTimerType") || (sType == "Fast_TriggerTimerType"))
	{
		myTimer = new TriggerTimer();
		Log("setTimerType returned: " + myTimer.setTimerType(sType));
		myTimer.timeout.connect(this, TimerInvoked);
		myTimer.startTimer(nTimerInterval);	
	}
	else if(sType == "ExperimentTimer")
	{
		myTimer = new ExperimentTimer();
		myTimer.triggeredTimerLoopInvoked.connect(this, TimerInvoked);
		myTimer.startTriggeredTimerLoop(nTimerInterval);	
	}	
	else
	{
		Log("Could not setup the requested timer... exiting script");
		return false;
	}
	Log(myTimer);
	KeyBoardCaptureObj.StopCaptureThread();
	return true;
}

function cleanupTimer()
{
	Log("cleanupTimer() called...");
	if(myTimer)
	{
		var sObjectType = myTimer.toString().split("(");
		Log(sObjectType[0]);
		if(sObjectType[0] == "QTimer")
		{
			myTimer.stop();
			try 
			{				
				myTimer.timeout.disconnect(this, TimerInvoked);			
			} 
			catch (e) 
			{
			//	Log(".*. Something went wrong disconnecting the timer Signal/Slots:" + e);
			}
		}	
		else if(sObjectType[0]  == "TriggerTimer")
		{
			myTimer.stopTimer();
			try 
			{				
				myTimer.timeout.disconnect(this, TimerInvoked);			
			} 
			catch (e) 
			{
			//	Log(".*. Something went wrong disconnecting the timer Signal/Slots:" + e);
			}
		}			
		else if(sObjectType[0]  == "ExperimentTimer")
		{
			myTimer.stopTriggeredTimerLoop(nTimerInterval);
			try 
			{				
				myTimer.triggeredTimerLoopInvoked.disconnect(this, TimerInvoked);			
			} 
			catch (e) 
			{
			//	Log(".*. Something went wrong disconnecting the timer Signal/Slots:" + e);
			}
		}
	}
	return true;
}

function KeyCaptureDetectFunction(keyCode)
{
	if(keyCode == 49)
	{
		Log("1 pressed. -> (setupTimer('QTimer'))");
		if(setupTimer("QTimer") == false)
			CleanupScript();			
	}	
	else if(keyCode == 50)
	{
		Log("2 pressed. -> (setupTimer('QPCNew_TriggerTimerType'))");
		if(setupTimer("QPCNew_TriggerTimerType") == false)
			CleanupScript();	
	}
	else if(keyCode == 51)
	{
		Log("3 pressed. -> (setupTimer('QPC_TriggerTimerType'))");
		if(setupTimer("QPC_TriggerTimerType") == false)
			CleanupScript();	
	}
	else if(keyCode == 52)
	{
		Log("4 pressed. -> (setupTimer('QTimer_TriggerTimerType'))");
		if(setupTimer("QTimer_TriggerTimerType") == false)
			CleanupScript();	
	}	
	else if(keyCode == 53)
	{
		Log("5 pressed. -> (setupTimer('Fast_TriggerTimerType'))");
		if(setupTimer("Fast_TriggerTimerType") == false)
			CleanupScript();	
	}
	else if(keyCode == 54)
	{
		Log("6 pressed. -> (setupTimer('ExperimentTimer'))");
		if(setupTimer("ExperimentTimer") == false)
			CleanupScript();	
	}	
	else if(keyCode == 27)//Escape
	{
		Log("Escape pressed. -> (CleanupScript)");
		CleanupScript();
	}
	else
	{
		Log("A key press was detected: " + keyCode); 
	}
}

function TimerInvoked()
{
	tmpTimeObject = new Date();
	timespanInMS = (tmpTimeObject.getTime() - LastMeasuredTime);	
	if(nFirstIterationsToIgnore == 0)
	{
		meanTime+= timespanInMS;
		if((timespanInMS > maxTime) || (maxTime==0))
			maxTime = timespanInMS;
		if((timespanInMS < minTime) || (minTime==0))
			minTime = timespanInMS;	
		callCounter++;
		Log("\t...it took " + timespanInMS + " milliseconds");
	}
	else
	{
		minTime = 0;
		maxTime = 0;
		nFirstIterationsToIgnore--;
	}
	LastMeasuredTime = tmpTimeObject.getTime();
	tmpTimeObject = null;
	if(callCounter >= 100)
	{
		CleanupScript();
	}	
}

function ConnectDisconnectScriptFunctions(Connect)
{
	if(Connect)
	{
		Log("... Connecting Signal/Slots");
		try 
		{	
			KeyBoardCaptureObj.CaptureThreadKeyPressed.connect(this, this.KeyCaptureDetectFunction);			
		} 
		catch (e) 
		{
			Log(".*. Something went wrong connecting the Signal/Slots:" + e);
		}		
	}
	else
	{
		Log("... Disconnecting Signal/Slots");
		try 
		{	
			KeyBoardCaptureObj.CaptureThreadKeyPressed.disconnect(this, this.KeyCaptureDetectFunction);			
		} 
		catch (e) 
		{
			Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
		}		
	}	
}

function CleanupScript()
{
	KeyBoardCaptureObj.StopCaptureThread();
	StimulGL.enableActiveDocument(true);
	cleanupTimer();
	ConnectDisconnectScriptFunctions(false);
	
	//Show Final Information
	Log("SumMeanTime: "+meanTime);
	Log("callCounter: "+callCounter);
	Log("MeanTime: "+(meanTime/callCounter));
	Log("MinTime: "+minTime);
	Log("MaxTime: "+maxTime);
	//Delete Allocated variabeles and Objects
	tmpTimeObject = null;
	myTimer = null;
	KeyBoardCaptureObj = null;
	//Delete Functions
	TimerInvoked = null;
	cleanupTimer = null;
	setupTimer = null;
	KeyCaptureDetectFunction = null;
	ConnectDisconnectScriptFunctions = null;
	CleanupScript=null;	
	//Do Finalization
	Log("CleanUpScript exited successfully!");
	StimulGL.cleanupScript();
}

ConnectDisconnectScriptFunctions(true);
KeyBoardCaptureObj.StartCaptureThread(0, false);
StimulGL.enableActiveDocument(false);
