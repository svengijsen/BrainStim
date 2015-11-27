// This example shows how you can make use of some additional Qt Script bindings.

var color = new QColor(Qt.green); //QColor Constructor with a color parameter to pre-set the color

function ScriptCleanupFunction()//Cleanup
{
	//Disconnect the signal/slots, not implemented here...
	//Set all constructed objects to null
	color=null;
	//Set all functions to null
	showTheColor=null;
	getTheColor=null;
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
	color = QColorDialog.getColor(defaultColor,this,"Please select a color, also set the alpha level!"); 	
	//The static getColor() member function shows the dialog, and allows the user to specify a color. 
	//The script receives control back after the user accepted or rejected the color dialog!
	//This function can also be used to let users choose a color with a level of transparency.
	
	if (color.isValid()) 
	{
		return color;
	}
	return defaultColor;	
}

showTheColor();
color.setRgb(255,0,0,255);	//Here we set the color object with an RGBA (Red, Green, Blue, Alpha) value.
					//The alpha channel of a color specifies the transparency effect, 0 represents a fully transparent color, while 255 represents a fully opaque color.
showTheColor();
color = getTheColor(color);	//Let the user choose a color, makes use of the QColorDialog class its static member function QColorDialog.getColor().
showTheColor();
ScriptCleanupFunction();
