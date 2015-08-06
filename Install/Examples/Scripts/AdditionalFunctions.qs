//This is an example of using some global functions.
//Uncomment (by removing the // for each line or /* followed by a */ for a collection of lines) each section to see how it works!

Log("\n<The script starts here>");

/*
for(var i=0;i<3;i++)	//Loop three times
{
	Pause(500);	//Pause for 500 milli-seconds
	Beep();		//Play a system beep sound
}
*/

//ThrowError("Error: this is an error!");	//This throws an error that results in a UncaughtException() inside the script engine, see the Output Log window messages, this stops further script evaluation and aborts.

/*
Log("Before the ExitScript() function...");
ExitScript();					//This immediately stops the script evaluation and exits.
Log("After the ExitScript() function...");	//This code is not executed because the ExitScript() statement.
*/

/*
Include("MathFunctions.qs");			//Include a file containing some mathemetical functions
var multiplyResult = multiply(3,7);			//Call a function that is defined in the included file
Log("The result = " + multiplyResult);
cleanup_MathFunctions();				//Call a function of the included file performs some script cleanup
*/

Log("<The script ends here>\n");

