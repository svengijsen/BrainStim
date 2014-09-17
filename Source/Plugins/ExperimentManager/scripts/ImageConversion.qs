var ImageProcessorObj = new ImageProcessor();
var bResult = false;
var i = 0; 
var j = 0; 
var sSourceDirectory;
var sSourceFileNamePre;
var sSourceFileNamePreExtra;
var sSourceFileNamePost;
var sSourcePath;
var sDestDirectory;
var sDestFileNamePre;
var sDestFileNamePreExtra;
var sDestFileNamePost;
var sDestPath;
var sFixationPath = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\fixation");
var sEmptyPath = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\empty");
var sTmpString = new String("");
var StimRandomListArray;
var EmptyRandomList;
var MovingBarAngleList;
var MovingBarRandomIndexList;
var nNumberOfEmptySteps;
var nPreFixationTriggers;
var nPostFixationTriggers;
var nTotalItems = 0;
var nTotalEmptyItems = 0;
var nEmptyListIndex = 0;
var nProcessedEmptyItems=0;
var nCycleNrOfSteps;
var nScaleRatio;
var nScaleMethod = 3;
var nScaleThreshold = 10;

function InitVars(ExperimentRunName)
{
	nTotalItems = 0;
	switch (ExperimentRunName)
	{
	case "Polar1":
		nProcessedEmptyItems = 0;
		nEmptyListIndex = 0;
		sSourceDirectory = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\Polar\\");
		sSourceFileNamePre = new String("Polar_Dir1 (");
		sSourceFileNamePost = new String(")");
		sDestDirectory = new String("C:\\Users\\John\\Desktop\\GeneratedFiles\\Polar1\\");//"D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ConvertedFiles\\Polar1\\");
		sDestFileNamePre = new String("PolarDir1");
		sDestFileNamePost = new String("");	
		sSourceFileNamePreExtra = new String("");
		sDestFileNamePreExtra = new String("");	
		StimRandomListArray =  Array[
		6,2,4,10,3,5,1,11,0,7,9,8,
		10,2,11,6,3,1,5,4,0,7,9,8,
		10,2,11,6,5,3,4,1,0,7,9,8,
		6,2,4,10,3,5,1,11,0,7,9,8,
		10,2,11,6,3,1,5,4,0,7,9,8,
		10,2,11,6,5,3,4,1,0,7,9,8,
		6,2,4,10,3,5,1,11,0,7,9,8,
		10,2,11,6,3,1,5,4,0,7,9,8,
		10,2,11,6,5,3,4,1,0,7,9,8,
		6,2,4,10,3,5,1,11,0,7,9,8,
		10,2,11,6,3,1,5,4,0,7,9,8,
		10,2,11,6,5,3,4,1,0,7,9,8,
		6,2,4,10,3,5,1,11,0,7,9,8,
		10,2,11,6,3,1,5,4,0,7,9,8,
		10,2,11,6,5,3,4,1,0,7,9,8,
		6,2,4,10,3,5,1,11,0,7,9,8,
		10,2,11,6,3,1,5,4,0,7,9,8,
		10,2,11,6,5,3,4,1,0,7,9,8
		];
		EmptyRandomList = Array[
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7
		];
		MovingBarAngleList = Array[];
		nNumberOfEmptySteps = 4;
		nPreFixationTriggers = 8;
		nPostFixationTriggers = 8;
		nCycleNrOfSteps = 12;
		nScaleRatio = 8;
		nTotalItems = StimRandomListArray.length;
		nTotalEmptyItems = EmptyRandomList.length;		
		break;
	case "Polar2":
		nProcessedEmptyItems = 0;
		nEmptyListIndex = 0;
		sSourceDirectory = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\Polar\\");
		sSourceFileNamePre = new String("Polar_Dir1 (");
		sSourceFileNamePost = new String(")");
		sDestDirectory = new String("C:\\Users\\John\\Desktop\\GeneratedFiles\\Polar2\\");
		sDestFileNamePre = new String("PolarDir2");
		sDestFileNamePost = new String("");
		sSourceFileNamePreExtra = new String("");
		sDestFileNamePreExtra = new String("");	
		StimRandomListArray =  Array[
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11
		];
		EmptyRandomList = Array[
		0,1,2,3,8,9,10,11,4,5,6,7,
		0,1,2,3,8,9,10,11,4,5,6,7,
		0,1,2,3,8,9,10,11,4,5,6,7,
		0,1,2,3,8,9,10,11,4,5,6,7,
		0,1,2,3,8,9,10,11,4,5,6,7,
		0,1,2,3,8,9,10,11,4,5,6,7
		];
		MovingBarAngleList = Array[];		
		nNumberOfEmptySteps = 4;
		nPreFixationTriggers = 8;
		nPostFixationTriggers = 8;
		nCycleNrOfSteps = 12;
		nScaleRatio = 8;
		nTotalItems = StimRandomListArray.length;
		nTotalEmptyItems = EmptyRandomList.length;		
		break;
	case "Eccentricity1":
		nProcessedEmptyItems = 0;
		nEmptyListIndex = 0;
		sSourceDirectory = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\Eccentricity\\");
		sSourceFileNamePre = new String("Eccentricity_Dir1 (");
		sSourceFileNamePost = new String(")");
		sDestDirectory = new String("C:\\Users\\John\\Desktop\\GeneratedFiles\\Eccentricity1\\");
		sDestFileNamePre = new String("Eccentricity1");
		sDestFileNamePost = new String("");
		sSourceFileNamePreExtra = new String("");
		sDestFileNamePreExtra = new String("");	
		StimRandomListArray =  Array[
		6,1,7,4,8,3,2,0,5,10,11,9,
		6,3,4,2,7,8,0,1,5,10,11,9,
		3,6,4,2,7,8,0,1,5,10,11,9,
		6,1,7,4,8,3,2,0,5,10,11,9,
		6,3,4,2,7,8,0,1,5,10,11,9,
		3,6,4,2,7,8,0,1,5,10,11,9,
		6,1,7,4,8,3,2,0,5,10,11,9,
		6,3,4,2,7,8,0,1,5,10,11,9,
		3,6,4,2,7,8,0,1,5,10,11,9,
		6,1,7,4,8,3,2,0,5,10,11,9,
		6,3,4,2,7,8,0,1,5,10,11,9,
		3,6,4,2,7,8,0,1,5,10,11,9,
		6,1,7,4,8,3,2,0,5,10,11,9,
		6,3,4,2,7,8,0,1,5,10,11,9,
		3,6,4,2,7,8,0,1,5,10,11,9,
		4,9,3,8,0,7,6,11,1,2,10,5,
		6,0,3,7,8,11,9,4,1,2,10,5,
		6,0,3,7,9,8,11,4,1,2,10,5
		];
		EmptyRandomList = Array[
		8,9,10,11,4,5,6,7,0,1,2,3,
		8,9,10,11,4,5,6,7,0,1,2,3,
		8,9,10,11,4,5,6,7,0,1,2,3,
		8,9,10,11,4,5,6,7,0,1,2,3,
		8,9,10,11,4,5,6,7,0,1,2,3,
		8,9,10,11,0,1,2,3,4,5,6,7
		];
		MovingBarAngleList = Array[];		
		nNumberOfEmptySteps = 4;
		nPreFixationTriggers = 8;
		nPostFixationTriggers = 8;
		nCycleNrOfSteps = 12;
		nScaleRatio = 8;
		nTotalItems = StimRandomListArray.length;
		nTotalEmptyItems = EmptyRandomList.length;		
		break;		
	case "Eccentricity2":
		nProcessedEmptyItems = 0;
		nEmptyListIndex = 0;
		sSourceDirectory = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\Eccentricity\\");
		sSourceFileNamePre = new String("Eccentricity_Dir1 (");
		sSourceFileNamePost = new String(")");
		sDestDirectory = new String("C:\\Users\\John\\Desktop\\GeneratedFiles\\Eccentricity2\\");
		sDestFileNamePre = new String("Eccentricity2");
		sDestFileNamePost = new String("");
		sSourceFileNamePreExtra = new String("");
		sDestFileNamePreExtra = new String("");	
		StimRandomListArray =  Array[
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11,
		0,1,2,3,4,5,6,7,8,9,10,11
		];
		EmptyRandomList = Array[
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7,
		8,9,10,11,0,1,2,3,4,5,6,7
		];
		MovingBarAngleList = Array[];		
		nNumberOfEmptySteps = 4;
		nPreFixationTriggers = 8;
		nPostFixationTriggers = 8;
		nCycleNrOfSteps = 12;
		nScaleRatio = 8;
		nTotalItems = StimRandomListArray.length;
		nTotalEmptyItems = EmptyRandomList.length;		
		break;
	case "MovingBar1":
		nProcessedEmptyItems = 0;
		nEmptyListIndex = 0;
		sSourceDirectory = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\MovingBar\\");
		sSourceFileNamePre = new String("MovingBar_Angle");
		sSourceFileNamePost = new String(")");
		sDestDirectory = new String("C:\\Users\\John\\Desktop\\GeneratedFiles\\MovingBar1\\");
		sDestFileNamePre = new String("MovingBar1Angle");
		sDestFileNamePost = new String("");
		sSourceFileNamePreExtra = new String("");
		sDestFileNamePreExtra = new String("");	
		StimRandomListArray =  Array[
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5,
		9,0,1,2,10,4,11,8,3,7,6,5
		];
		EmptyRandomList = Array[];
		MovingBarAngleList = Array[0,45,90,135,180,225,270,315];
		MovingBarRandomIndexList = Array[0,7,4,1,2,5,6,3,6,0,1,4,3,2,7,5,7,5,0,1,4,2,3,6];//MovingBar1
		//MovingBarRandomIndexList = Array[1,2,7,5,3,0,6,4,3,1,4,2,0,5,7,6,0,7,1,4,3,5,6,2];//MovingBar2
		nNumberOfEmptySteps = 0;
		nPreFixationTriggers = 8;
		nPostFixationTriggers = 8;
		nCycleNrOfSteps = 12;
		nScaleRatio = 8;
		nTotalItems = StimRandomListArray.length;
		nTotalEmptyItems = EmptyRandomList.length;		
		break;	
	}	
	Log(ExperimentRunName + " initialized");
}

function ProcessItem(source,destination)
{
	bResult = false;
	sTmpString = source + ":" + destination;
	Log(sTmpString);	
	var intermediate = new String(destination + "_Scaled.png");
	ProcessPngScale(source + ".png", intermediate);
	if(bResult)
		ProcessPngToDat(intermediate, destination + ".dat")
}

function ProcessPngScale(source,destination)
{
	bResult = false;
	bResult = ImageProcessorObj.ScalePngFile(source, destination, nScaleRatio, nScaleMethod, nScaleThreshold, true);
	//bool ImageProcessor::ScalePngFile(QString strSource, QString strDestination, int nRatio, int nMethod, int nColorThreshold, bool bOverwrite)
	if(bResult==false)
		Log("An Error occured during ProcessPngScale(), breaking!");	
}

function ProcessPngToDat(source,destination)
{
	bResult = false;
	bResult = ImageProcessorObj.ConvertPngToDatFile(source, destination, true);
	//bResult = ImageProcessorObj.ConvertDatToPngFile(sSourcePath, sDestPath, true); //For reverting or to check it again
	if(bResult==false)
		Log("An Error occured during ProcessPngToDat(), breaking!");	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Log("Starting conversion");
InitVars("MovingBar1");

//nTotalItems=0;
if(nTotalItems>0)
{
	Log("Going to process " + nTotalItems + " items...."); 
	
	//Process the pre Fixation triggers
	for(i=0;i<nPreFixationTriggers;i++)
	{
		sSourcePath = sFixationPath;
		sDestPath = sDestDirectory + (i) + "_" + sDestFileNamePre + "(fixation)" + sDestFileNamePost;
		ProcessItem(sSourcePath,sDestPath);
		if(bResult==false)
			break;			
	}

	if(bResult)
	{//Process the Stimulation triggers
		var isEmpty = false;
		for(i=1;i<=nTotalItems;i++)
		{
			isEmpty = false;
			if(nTotalEmptyItems > 0) 
			{
				if((i-1)%nCycleNrOfSteps == 0)
				{
					nProcessedEmptyItems = 0;
					//Log("Resetting Empty Processing" + ", index=" + (i-1));
				}
				if((nProcessedEmptyItems<nNumberOfEmptySteps) && (((i-1)%nCycleNrOfSteps) == EmptyRandomList[nEmptyListIndex]) && (nEmptyListIndex < nTotalEmptyItems))
				{
					nProcessedEmptyItems++;
					nEmptyListIndex++;
					//Log("***This one should be empty" + ", index=" + (i-1));
					isEmpty = true;
				}
			}
			if(MovingBarAngleList.length > 0)
			{			
				//Log("This is an MovingBar");
				
				//sSourceDirectory = new String("D:\\Dropbox\\ERC Grant\\Projects\\Retinotopic Mapping Project\\Results\\Subject1\\ActivityMasks\\MovingBar\\");
				
				//sSourceFileNamePre = new String("MovingBar_Angle""0 (");
				//sSourceFileNamePost = new String(")");
				//sDestDirectory = new String("C:\\Users\\John\\Desktop\\GeneratedFiles\\MovingBar1\\");
				//sDestFileNamePre = new String("MovingBar1");	
				//10_MovingBar1_Angle45(2).dat	
				var angleIndex;// = new String();
				angleIndex = ((i-1)-((i-1)%12))/12;//((i-1)/nCycleNrOfSteps);
				var angle = MovingBarAngleList[MovingBarRandomIndexList[angleIndex]];
				
				//sSourceDirectory + sSourceFileNamePre + angle +" (" + (StimRandomListArray[i-1]+1) + sSourceFileNamePost;
				//sDestDirectory + (i-1+nPreFixationTriggers) + "_" + sDestFileNamePre + "_Angle" + angle + "(" + StimRandomListArray[i-1] + ")" + sDestFileNamePost;
				
				sSourceFileNamePreExtra = angle + " (";
				sDestFileNamePreExtra = angle;
				
				//sSourceFileNamePre = sSourceFileNamePre + angle + " (";
				//sDestFileNamePre = sDestFileNamePre + angle;
				//Log("***" + MovingBarAngleList[MovingBarRandomIndexList[angleIndex+1]] + "," + nCycleNrOfSteps + "," + angle);
				//Log(sSourceDirectory + ":" + sDestDirectory);
				//bResult = false;
			}
			else
			{
				sSourceFileNamePreExtra = "";
				sDestFileNamePreExtra = "";				
			}
			if(isEmpty)
			{
				sSourcePath = sEmptyPath;
				sDestPath = sDestDirectory + (i-1+nPreFixationTriggers) + "_" + sDestFileNamePre + sDestFileNamePreExtra + "(empty)" + sDestFileNamePost;
			}
			else
			{
				sSourcePath = sSourceDirectory + sSourceFileNamePre + sSourceFileNamePreExtra + (StimRandomListArray[i-1]+1) + sSourceFileNamePost;
				sDestPath = sDestDirectory + (i-1+nPreFixationTriggers) + "_" + sDestFileNamePre + sDestFileNamePreExtra + "(" + StimRandomListArray[i-1] + ")" + sDestFileNamePost;
			}
			//Log("*** " + sSourcePath + "  :  " + sDestPath);
			ProcessItem(sSourcePath,sDestPath);
			if(bResult==false)
				break;
		}	
	}
	
	if(bResult)
	{//Process the post Fixation triggers		
		for(i=0;i<nPostFixationTriggers;i++)
		{
			sSourcePath = sFixationPath;
			sDestPath = sDestDirectory + (i+nPreFixationTriggers+nTotalItems) + "_" + sDestFileNamePre + "(fixation)" + sDestFileNamePost;
			ProcessItem(sSourcePath,sDestPath);
			if(bResult==false)
			{
				Log("An Error occured during conversion, breaking!");	
				break;
			}		
		}	
	}
	if(bResult)
		Log("Successfull Conversion!");
}
else
{
	Log("No item(s) to process...");
}


ImageProcessorObj.deleteLater();
InitVars = null;
ProcessPngToDat = null;
ProcessPngScale = null;
ProcessItem = null;
Log("Stopped conversion");




//"StimRandomList" results:
//Value
//6,2,4,10,3,5,1,11,0,7,9,8
//10,2,11,6,3,1,5,4,0,7,9,8
//10,2,11,6,5,3,4,1,0,7,9,8
//6,2,4,10,3,5,1,11,0,7,9,8
//10,2,11,6,3,1,5,4,0,7,9,8
//10,2,11,6,5,3,4,1,0,7,9,8
//6,2,4,10,3,5,1,11,0,7,9,8
//10,2,11,6,3,1,5,4,0,7,9,8
//10,2,11,6,5,3,4,1,0,7,9,8
//6,2,4,10,3,5,1,11,0,7,9,8
//10,2,11,6,3,1,5,4,0,7,9,8
//10,2,11,6,5,3,4,1,0,7,9,8
//6,2,4,10,3,5,1,11,0,7,9,8
//10,2,11,6,3,1,5,4,0,7,9,8
//10,2,11,6,5,3,4,1,0,7,9,8
//6,2,4,10,3,5,1,11,0,7,9,8
//10,2,11,6,3,1,5,4,0,7,9,8
//10,2,11,6,5,3,4,1,0,7,9,8

//"EmptyRandomList" results:
//Value
//8,9,10,11,0,1,2,3,4,5,6,7
//8,9,10,11,0,1,2,3,4,5,6,7
//8,9,10,11,0,1,2,3,4,5,6,7
//8,9,10,11,0,1,2,3,4,5,6,7
//8,9,10,11,0,1,2,3,4,5,6,7
//8,9,10,11,0,1,2,3,4,5,6,7



