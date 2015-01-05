// This example shows how you can make use of some additional BrainStim functions.

var result = "";

Log("\n\n\n");//This appends some empty lines (\n) to the Log Output Pane

result = BrainStim.getActiveDocumentFileLocation();
Log("This (active) document file location is: " + result);

result = BrainStim.getApplicationRootDirPath();
Log("The BrainStim root directory is: " + result);

result = BrainStim.getActiveDocumentFileName();
Log("This (active) document file is named: " + result);

result = BrainStim.getEnvironmentVariabele("USERNAME");
Log("User: " + result + " is currently logged on this system.");

Beep();//This function plays a default beep sound
Pause(500);//This function Pauses for 500 milliseconds before it proceeds (Important, this blocks all script context execution! Therefore only for testing purpose.)
Beep();

var sTextToOutput = "You can also make use of the BrainStim.write2OutputWindow() function \
to write to the (Default) Log Output Window";
BrainStim.write2OutputWindow(sTextToOutput);
Log(sTextToOutput);//This is the same as the above function 
//Create a new tabbular named Output Window
BrainStim.addOutputWindow("Window Name");
BrainStim.write2OutputWindow(sTextToOutput + " and write to a specific named window.", "Window Name");
//BrainStim.clearOutputWindow("Window Name");
//BrainStim.removeOutputWindow("Window Name");

Log("\tTAB is inserted first");//This appends an TAB (\t) and some text to the Log Output Pane
Log("\n\n\n");//This appends some empty lines (\n) to the Log Output Pane

//BrainStim.find(false);
//BrainStim.clearOutputWindow();
//BrainStim.closeSelectedScriptFile();
//ThrowError("This is an dummy error message"); This function Throw an error, see output log window and aborts the script immediately (this is not a clean way of exiting your script!).
