var countDownTimer = new QTimer();
countDownTimer.interval = 500; 		// set the time in milliseconds
countDownTimer.timeout.connect(this, this.countDownTimerInvoked);
var nTimerCounter = 5;
countDownTimer.start();
		
function countDownTimerInvoked()
{
	nTimerCounter--;
	Log("Timer invoked (remaining: " + nTimerCounter + ")");
	if(nTimerCounter<1)
	{
		//Stop the timer 
		countDownTimer.stop();
		ScriptCleanupFunction();
	}
}

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Perform a successfull garbage collection
{
//	Disconnect all signal/slot connections...				
	//countDownTimer.timeout.disconnect(this, countDownTimerInvoked);		
// 	Delete all allocated objects...
	countDownTimer = null;
//	Set all defined functions to null...
	countDownTimerInvoked = null;
	ScriptCleanupFunction = null;
//	Write something to the OutputLog Pane so we know that this Function executed successfully.
	Log("Finished script cleanup, ready for garbage collection!");
	BrainStim.cleanupScript();
}