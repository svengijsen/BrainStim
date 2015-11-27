// This example shows how you can make use of functions.

//function definitions
function sayHello(firstName, lastName)
{
	Log("Hello " + firstName + " " + lastName + ".");
}

function add(first, last)
{
	var full;
	full = first + last;
	return full;
}

//function calls
sayHello("John", "Doe");
var result = add(3, 5);
Log("The result is: " + result);

//function cleanups
sayHello = null;
add = null;

