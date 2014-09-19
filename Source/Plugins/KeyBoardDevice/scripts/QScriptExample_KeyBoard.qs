// This example shows how you can access the features of an BrainStim Plugin from the script.

//Declare a global variabele to count the number of key presses while capturing
var KeyPressedCounter = 0;
//Construct a BrainStim Plugin KeyBoard Object
var KeyBoardCaptureObj = new KeyBoardCapture(); 
var lAllowedKeyCodeList = Array[49,50,51,52,53]; // keys 1..5
Log("Allowed key-code(s) to be processed in this script are: " + lAllowedKeyCodeList);

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Cleanup
{
	//Disconnect the signal/slots
	KeyBoardCaptureObj.CaptureThreadStarted.disconnect(this, this.KeyCaptureStartedFunction);
	KeyBoardCaptureObj.CaptureThreadKeyPressed.disconnect(this, this.KeyCaptureDetectFunction);
	KeyBoardCaptureObj.CaptureThreadKeyReleased.disconnect(this, this.KeyCaptureDetectFunction);	
	KeyBoardCaptureObj.CaptureThreadStopped.disconnect(this, this.KeyCaptureStoppedFunction);
	//Set all constructed objects to null 
	KeyBoardCaptureObj = null;
	//Set all functions to null
	KeyCaptureStartedFunction = null;
	KeyCaptureDetectFunction = null;
	KeyCaptureStoppedFunction = null;
	ScriptCleanupFunction = null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script Cleanup!");
	BrainStim.cleanupScript();
}

//This function is called whenever the user presses a key
function KeyCaptureDetectFunction(keyCode)
{
	Log("A key press was detected: " + keyCode); 	
	KeyPressedCounter++;
	Log("KeyPressedCounter incremented to: " + KeyPressedCounter + "\n");
	if (KeyPressedCounter==4)
	{
		KeyBoardCaptureObj.StopCaptureThread();
		BrainStim.enableActiveDocument(true);
	}
}

//This function is called whenever the Key capture status has been started
function KeyCaptureStartedFunction()
{
	Log("\n\n\nThe key capture thread was just started...\n");
	Log("KeyCaptureStartedFunction arguments count: " + arguments.length);
	for (var i = 0; i < arguments.length; ++i)
		Log("KeyCaptureStartedFunction argument(" + i + "): " + arguments[i]);
	Log("\n");
}

//This function is called whenever the Key capture status has been stopped
function KeyCaptureStoppedFunction(DateTimeStampStopped)
{
	Log("\nThe key capture thread was just stopped @ " + DateTimeStampStopped + "\n"); 
	ScriptCleanupFunction();
}

//Connect the signal/slots
KeyBoardCaptureObj.CaptureThreadStarted.connect(this, this.KeyCaptureStartedFunction);
KeyBoardCaptureObj.CaptureThreadKeyPressed.connect(this, this.KeyCaptureDetectFunction);
KeyBoardCaptureObj.CaptureThreadKeyReleased.connect(this, this.KeyCaptureDetectFunction);
KeyBoardCaptureObj.CaptureThreadStopped.connect(this, this.KeyCaptureStoppedFunction);
//Start the capture thread
KeyBoardCaptureObj.StartCaptureThread(0, true, lAllowedKeyCodeList);
BrainStim.enableActiveDocument(false);
//Because the captured key events are forwarded, eg. to make sure we can control something else, we'll disable the this document to make sure that we don't edit it when executing it.


