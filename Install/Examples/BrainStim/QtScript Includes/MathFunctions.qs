//function definitions
function add(first, last)
{
	return first + last;
}

function substract(first, last)
{
	return first - last;
}

function divide(first, last)
{
	return first / last;
}

function multiply(first, last)
{
	return first * last;
}

//function cleanups
function cleanup_MathFunctions()
{
	//set all defined functions to null
	add = null;
	substract = null;
	divide = null;
	multiply = null;
	
	//now we also have to set this function to null 
	cleanup_MathFunctions = null;
}
