// This example shows how you can make use of functions and includes.

//This includes a document
Include("FunctionsAndIncludes2.qs") ;

//Define some functions
function testFunction1()
{
	Log("testFunction1 called, arguments count: " + arguments.length);  
	for (var i = 0; i < arguments.length; ++i)  //Loop trough the passed parameters
	{
		Log("\ttestFunction1 argument(" + i +"): " + arguments[i]);  
	}
};

function testFunction2(Text, Number)
{
	Log("testFunction2 called");  
	Log("\ttestFunction2 argument(0): " + Text);  
	Log("\ttestFunction2 argument(1): " + Number);  	
};

//Invoke the functions
Log("\n\n");
testFunction1(); //No Parameter
testFunction1("someText1"); //A String parameter
testFunction1("someText2", 1.2345); //A string parameter and a number
testFunction2("someText3", 2.3456);
testFunction2("someText4", 3.4567);
Log("testFunction3 returned: " + testFunction3("someText4...", "someExtraText...")); // This function has an return value
Log("testFunction3 returned: " + testFunction3(10, 20));

Log("\n\n");

//Set each function again to null before the script ends
testFunction1=null;
testFunction2=null;
testFunction3=null;