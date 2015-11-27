var objDateTime; 	//For fetching the Date/Time
var nStartTime;	//For storing the Start Time	
var nStopTime;		//For storing the End Time
var nTotalTime;		//For storing the Time difference between Start and Stop Time

objDateTime = new Date(); 			//At this line the current Date/Time is fetched
nStartTime = objDateTime.getTime(); 	//Read the number of milliseconds since midnight Jan 1, 1970, store it in nStartTime
Log("Started at: " + nStartTime);

Pause(500); 						//Wait for 500 mSeconds
Log("The objDateTime object now still returns " + objDateTime.getTime()); //Validate that the object still holds the same time...

objDateTime = new Date();			//Fetch again the current Date/Time
nStopTime = objDateTime.getTime(); 	//Read the number of milliseconds since midnight Jan 1, 1970, store it in nStopTime
Log("Stopped at: " + nStopTime);
nTotalTime = nStopTime - nStartTime;	//Calculate the Total Time
Log("It took a total of : " + nTotalTime + " milliseconds");

objDateTime = null; 					//Set the object to null before ending the script