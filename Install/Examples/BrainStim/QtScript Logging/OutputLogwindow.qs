Log("My first logged message.");
var sumResult = 3 + 4;
BrainStim.write2OutputWindow("3 + 4 =" + sumResult);

Log("\nAdding\tSome\tTabs \nand Line-breaks.\n");
Log("<h1>Header 1</h1>");
Log("<h2>Header 2</h2>");
Log("<h3>Header 3</h3>");
Log("<h4>Header 4</h4>");
Log("<font color=\"red\">This is some <em>custom</em> <strong>formatted</strong> text.</font>");

BrainStim.addOutputWindow("custom sub-window");
BrainStim.write2OutputWindow("Custom sub-window logged message.", "custom sub-window");

//See also description of the following functions in the script reference:
/*
- BrainStim.clearOutputWindow() : Clears the Output Log window.
- BrainStim.removeOutputWindow() : Removes a named tab from the Output Log window.
- BrainStim.saveOutputWindow() : Saves the content of a Output Log window to a file.
*/