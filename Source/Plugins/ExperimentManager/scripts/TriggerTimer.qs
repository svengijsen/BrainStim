var sTimerType = "QTimer";//"TriggerTimer"; //Choose: QTimer | TriggerTimer

var sTimer; 
var sTimeObject;
var startTime;
var timespanInMS;

function CleanupScript()
{
	try 
	{		
		sTimer.timeout.disconnect(this, TimerInvoked);
	} 
	catch (e) 
	{
		Log(".*. Something went wrong connecting the slot:" + e);
	}
	if(sTimerType == "QTimer")
		sTimer.stop();
	else	
		sTimer.stopTimer();
	sTimer = null;
	sTimeObject = null;
	TimerInvoked=null;
	CleanupScript=null;
	Log("CleanupScript ended...");	
}

function TimerInvoked()
{
	sTimeObject = new Date();
	timespanInMS = sTimeObject.getTime() - startTime;
	//Log(sTimeObject.getTime());
	Log("TimerInvoked()");
	Log("\t...it took " + timespanInMS + " milliseconds");
	CleanupScript();
}





if((sTimerType == "QTimer") || (sTimerType == "TriggerTimer"))
{
	Log("Choosen type is: " + sTimerType);
	if(sTimerType == "QTimer")
	{
		sTimer = new QTimer();
		sTimer.singleshot = true;		
		sTimer.start(6000);
	}
	else
	{
		sTimer = new TriggerTimer();
		sTimer.startTimer(6000);
	}
	try 
	{		
		sTimer.timeout.connect(this, TimerInvoked);
	} 
	catch (e) 
	{
		Log(".*. Something went wrong connecting the slot:" + e);
	}	
	Log("* Timer started...");
	sTimeObject = new Date();
	startTime = sTimeObject.getTime();
	//Log(startTime);//sTimeObject.getMinutes() sTimeObject.getSeconds() sTimeObject.getMilliseconds()
}	
else
{
	CleanupScript();
}

