var NumberArray = Array[1,2,3,4,5,6,7,8,9];
var TmpArray = new Array();
var TmpString = "";
var RandomGeneratorObj = new RandomGenerator();
var nCounter1;
var nCounter2;
var nMaxCounter;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Log("Starting the RandomGenerator Example");

TmpString = "";
nMaxCounter = 10;
for(nCounter1=0;nCounter1<nMaxCounter;nCounter1++)
{
	TmpString = TmpString + RandomGeneratorObj.randomizeInt(1,9);
	if(nCounter1+1<nMaxCounter)
		TmpString = TmpString + " - ";
}
Log(TmpString);

TmpString = "";
nMaxCounter = 10;
for(nCounter1=0;nCounter1<nMaxCounter;nCounter1++)
{
	TmpString = RandomGeneratorObj.randomizeDouble(1,2);
	Log(TmpString);
}

TmpString = "";
for(nCounter1=0;nCounter1<10;nCounter1++)
{
	RandomGeneratorObj.append(nCounter1);
}

for(nCounter2=0;nCounter2<10;nCounter2++)
{
	TmpString = "";
	NumberArray = RandomGeneratorObj.randomize(0);
	Log("ScriptArray: " + NumberArray);
	for(nCounter1=0;nCounter1<NumberArray.length;nCounter1++)
	{
		TmpString = TmpString + NumberArray[nCounter1];
		if(nCounter1+1<NumberArray.length)
			TmpString = TmpString + " - ";	
	}
	Log(TmpString);
}

for(nCounter1=0;nCounter1<RandomGeneratorObj.count()/2;nCounter1++)
{
	Log("First item : " + RandomGeneratorObj.takeFirst());
}
Log("IsEmpty()? : " + RandomGeneratorObj.isEmpty());
var nPosition = 2;
Log("Item at position(" + nPosition + ") : " + RandomGeneratorObj.takeAt(nPosition));
Log("Last item : " + RandomGeneratorObj.takeLast());
var nSearchNumber = 99;
Log("Contains (" + nSearchNumber + ") times : " + RandomGeneratorObj.count(nSearchNumber));
var nSearchNumber = RandomGeneratorObj.at(0);
Log("Contains (" + nSearchNumber + ") times : " + RandomGeneratorObj.count(nSearchNumber));
Log("Count() : " + RandomGeneratorObj.count());
RandomGeneratorObj.removeAt(RandomGeneratorObj.count()-1);
Log("Count() : " + RandomGeneratorObj.count());
RandomGeneratorObj.clear();
Log("IsEmpty()? : " + RandomGeneratorObj.isEmpty());

for(nCounter1=0;nCounter1<5;nCounter1++)
{
	for(nCounter2=0;nCounter2<2;nCounter2++)
	{
		RandomGeneratorObj.append(nCounter1);
	}
}
nSearchNumber = 3;
Log("Contains (" + nSearchNumber + ") times : " + RandomGeneratorObj.count(nSearchNumber));
RandomGeneratorObj.removeAll(nSearchNumber);
Log("Contains (" + nSearchNumber + ") times : " + RandomGeneratorObj.count(nSearchNumber));
nPosition = 4;
RandomGeneratorObj.insert(nPosition, nSearchNumber);
Log("Index of (" + nSearchNumber + ") is : " + RandomGeneratorObj.indexOf(nSearchNumber,0));
RandomGeneratorObj.swap(1,nPosition);
Log("Index of (" + nSearchNumber + ") is : " + RandomGeneratorObj.indexOf(nSearchNumber,0));
TmpArray = RandomGeneratorObj.toStringList();
Log(TmpArray);

RandomGeneratorObj.clear();
RandomGeneratorObj = new RandomGenerator();
Log("Count() : " + RandomGeneratorObj.count());
Log("Count() : " + RandomGeneratorObj.appendStringList(TmpArray));
Log("Count() : " + RandomGeneratorObj.appendStringList(TmpArray));
Log(RandomGeneratorObj.toStringList());

RandomGeneratorObj.clear();
RandomGeneratorObj = new RandomGenerator(TmpArray);
Log("Count() : " + RandomGeneratorObj.count());

RandomGeneratorObj.deleteLater();
NumberArray = null;
TmpArray = null;
TmpStringList = null;
RandomGeneratorObj = null;
Log("Stopped the RandomGenerator Example");