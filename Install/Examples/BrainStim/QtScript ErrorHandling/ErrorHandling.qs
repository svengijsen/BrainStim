BrainStim.addOutputWindow("Script Output");
BrainStim.clearOutputWindow("Script Output");
BrainStim.write2OutputWindow("<<Script Start>>", "Script Output");

//1. The following code contains a sytaxt error, because the first string parameter was not properly closed with a ending quote (") symbol the
//    syntaxt error is detected at evaluation before the code is actually executed.
//write2OutputWindow("forgot the last quote for closing this message, "Script Output");

//2. The following code raises an exception at the moment the non-existing unction is tried to execute, this script exception causes the script to exit and
//    remaining script is not executed anymore.
/*
BrainStim.write2OutputWindow("<<Before NonExistingFunctionCall()>>", "Script Output");
NonExistingFunctionCall();//This function is nowhere defined
BrainStim.write2OutputWindow("<<After NonExistingFunctionCall()>>", "Script Output");
*/

//3. You can catch a script exception and obtain an generated error message from the resulting Error object. Catching the
//    script exception gives you the opputunity to handle it and proceed with the remaining script execution.

/*
try 
{
	BrainStim.write2OutputWindow("<<Before NonExistingFunctionCall()>>", "Script Output");
	NonExistingFunctionCall();//This function is nowhere defined
	BrainStim.write2OutputWindow("<<After NonExistingFunctionCall()>>", "Script Output");
} 
catch (e) 
{
	BrainStim.write2OutputWindow(">> Error" + e, "Script Output");
}
*/

BrainStim.write2OutputWindow("<<Script End>>", "Script Output");