// This example shows how you can make use of a include statement.

//The below includes a external script
Include("MathFunctions.qs") ;

//Call the included function
var result = add(6, 7);		// 6+7 = 13
result = substract(result, 3);	// 13-3 = 10
result = divide(result, 2);		// 10/2 = 5
result = multiply(result, 10);	// 5*10 = 50

Log("The final result is: " + result);

//function cleanups
cleanup_MathFunctions();