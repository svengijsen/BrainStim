// This example shows how you can make use of the Qt Signal/Slot mechanism.

var color = new QColor(Qt.green); //QColor Constructor with a color parameter to pre-set the color
var dialog = new QColorDialog();   //QColorDialog

function ScriptCleanupFunction()//Cleanup
{
	//Disconnect the signal/slots
	try
	{	
		dialog.finished.disconnect(this, dialogFinished);		
	}
	catch (e) 
	{
		Log(".*. Something went wrong disconnecting the Signal/Slots:" + e);
	}
	//Set all constructed objects to null
	color=null;
	dialog=null;
	//Set all functions to null
	showTheColor=null;
	getTheColor=null;
	dialogFinished = null;
	ScriptCleanupFunction=null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script cleanup, ready for garbage collection!");
	BrainStim.cleanupScript();
}

function showTheColor()
{
	Log("color current value is: " + color.name()); //Here we use a property QColor::name() for accessing the color data from the QColor object
}

function getTheColor(defaultColor)
{
	dialog.setCurrentColor(defaultColor);
	dialog.open();					//The non-static getColor() member function shows the dialog, and allows the user to specify a color. 
								//The script receives control back immediately, so we must make use of signal slots to determine when the user accepted or rejected the color dialog!
								//This function can also be used to let users choose a color with a level of transparency.
}

function dialogFinished(result)
{
	Log("dialog Finished(" + result + ")");
	if(result == 0) //dialog Rejected
	{
		Log("dialog Rejected");
	}
	else if(result == 1) //dialog Accepted
	{
		Log("dialog Accepted");
		color = dialog.currentColor;
		showTheColor();
	}
	ScriptCleanupFunction();	
}

//Connecting the signal/slots
try 
{	
	dialog.finished.connect(this, dialogFinished);	
} 
catch (e) 
{
	Log(".*. Something went wrong connecting the Signal/Slots:" + e);
}



showTheColor();
color.setRgb(255,0,0,255);	//Here we set the color object with an RGBA (Red, Green, Blue, Alpha) value.
					//The alpha channel of a color specifies the transparency effect, 0 represents a fully transparent color, while 255 represents a fully opaque color.
showTheColor();
getTheColor(color);		//Let the user choose a color, makes use of the dialog object non-static member function dialog.open().
