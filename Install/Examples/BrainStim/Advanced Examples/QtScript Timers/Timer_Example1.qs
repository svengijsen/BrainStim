//Fixed variables (do not change)///////////
var currentScriptLocation = BrainStim.getActiveDocumentFileLocation();
var tmpTimeObject;
var LastMeasuredTime;
var timespanInMS;
var meanTime = 0;
var minTime = 0;
var maxTime = 0;
var callCounter = 0;
var myTimer;
var arrTimerTypes = ["QTimer", "QPCNew_TriggerTimerType", "QPC_TriggerTimerType", "QTimer_TriggerTimerType", "Fast_TriggerTimerType", "ExperimentTimer"];	
//Configurable variables///////////
var sTimerType = arrTimerTypes[0];	//"QTimer", the timer type to test
var nTimerInterval = 5;			//Interval time between samples in milliseconds
var nFirstIterationsToIgnore = 5;	//Number of samples to ignore at start of test
var nNumberOfMeasurements = 110;	//Number of samples

//Local Functions/////////////////
function setupTimer(sType)
{
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
	Log("setupTimer('" + sType + "') called successfully");
	return true;
}

function cleanupTimer()
{
	Log("cleanupTimer() called...");
	if(myTimer)
	{
		var sObjectType = myTimer.toString().split("(");
		//Log(sObjectType[0]);
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
	if(callCounter >= nNumberOfMeasurements)
	{
		CleanupScript();
	}	
}

function CleanupScript()
{
	cleanupTimer();
	//Show Final Information
	Log("\n------------ Results ------------");
	Log("SumMeanTime(mSecs): "+ meanTime);
	Log("Processed(mSecs): "+ callCounter);
	Log("MeanTime(mSecs): "+ (meanTime/callCounter));
	Log("MinTime(mSecs): "+ minTime);
	Log("MaxTime(mSecs): "+ maxTime);
	Log("---------------------------------\n");
	//Delete Allocated variabeles and Objects
	tmpTimeObject = null;
	myTimer = null;
	//Delete Functions
	TimerInvoked = null;
	cleanupTimer = null;
	setupTimer = null;
	CleanupScript=null;	
	//Do Finalization
	Log("CleanUpScript exited successfully!");
	BrainStim.cleanupScript();
}

//Main Script/////////////////
if(setupTimer(sTimerType) == false)
	CleanupScript();