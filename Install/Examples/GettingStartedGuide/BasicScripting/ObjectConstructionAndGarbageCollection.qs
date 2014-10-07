// This example shows how you should cleanup your constructed script objects to make a correct garbage collection possible.
var textString = "some text..."; //Declare a String primitive
var textStringObject = new String(textString); //Here we dynamically Construct a global (ECMA built-in type) String object

//This function makes sure that everything gets nicely cleaned before the script ends
function ScriptCleanupFunction()//Perform a successfull garbage collection
{
	//Disconnect the signal/slots, not implemented here...

	//Set all constructed objects to null
	textStringObject=null;
	//Set all functions to null
	getCharacter=null;
	replaceFirstCharacter=null;
	ScriptCleanupFunction=null;
	//Write something to the Log Output Pane so we know that this Function executed successfully.
	Log("Finished script cleanup, ready for garbage collection!");
	//BrainStim.cleanupScript();
}

function getCharacter(position)
{	
	if(textStringObject.length > position)
	{
		return textStringObject.charAt(position);
	}
	else
	{
		return null;
	}
}

function replaceFirstCharacter(charOld, charNew)
{	
	return textStringObject.replace(charOld, charNew);
}

Log("textString is typeof: " + typeof  textString);
Log("textStringObject is typeof: " + typeof  textStringObject);
Log("The character at position(2) is: " + getCharacter(2));
textStringObject = replaceFirstCharacter('e','E');
Log("The textStringObject value is now: " + textStringObject);

ScriptCleanupFunction();//Before ending the script we call the custom ScriptCleanupFunction().