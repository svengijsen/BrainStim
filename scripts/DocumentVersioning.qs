Include("common.qs");

var sScriptPath = BrainStim.getActiveDocumentFileLocation() + "/";
var tmpString = new String();
var sTimeObject = new Date();//gettime() returns Milliseconds since midnight January 1, 1970
//Log(sTimeObject.toTimeString());
//Log(sTimeObject.toTimeString().replace(/.*(\d{2}:\d{2}:\d{2}).*/, "$1"));//returns 08:55:57
//var sDestPath = "D:/Projects/BrainStim/Publish/";
var sSourceList = [];
var sDestList = [];
var changeSet = [];
var bDoProcess = true;
var nChangeCounter;
var tmpMinimalXMLString = "";
var tmpMinimalXMLString_Array = "";
var sComponentName = "";
var sComponentIndex = "";
var BrainStimInfo = null;

/////////////////////////CONFIGURATION(BEGIN)////////////////////////////////////////////
var sSourcePath 			= "../";
var sDestPath 				= sSourcePath;
var sMainAuthor			= "Sven Gijsen";
var sCurrentAuthoringMonth 	= "Juli 2015";
var sComment 				= "Please do not edit this line manually, see DocumentVersioning.qs (" + CreateTimeStamp(sTimeObject) + ")";
var sHTMLComment  		= "<!-- " + sComment + ", version " + BrainStim_Settings_CurrentBrainStimReleaseString + "-->"

sSourceList.push(sSourcePath + "js/BrainStim.js");
sDestList.push(sSourceList[sSourceList.length-1]);
	//changeSet = CreateArray(sSourceList.length,2,0);
	changeSet[sSourceList.length-1] = CreateArray(2,0);
	changeSet[sSourceList.length-1][0][0] = "var currentBrainStimRelease = ";
	changeSet[sSourceList.length-1][1][0] = changeSet[sSourceList.length-1][0][0] + "'" + BrainStim_Settings_CurrentBrainStimReleaseString + "'";

sSourceList.push(sSourcePath + "MainDocumentation.qhcp");
sDestList.push(sSourceList[sSourceList.length-1]);
	//changeSet = CreateArray(sSourceList.length,2,0);
	changeSet[sSourceList.length-1] = CreateArray(2,0);
	changeSet[sSourceList.length-1][0][0] = "<title>";
	changeSet[sSourceList.length-1][1][0] = "\t" + changeSet[sSourceList.length-1][0][0] + "BrainStim v" + BrainStim_Settings_CurrentBrainStimReleaseString + "</title>" + sHTMLComment;

//sSourceList.push(sSourcePath + "Tutorials/6_RetinoTopicMappingOutput/6_RetinoTopicMappingOutput.html");
//sDestList.push(sSourceList[sSourceList.length-1]);
//	changeSet[sSourceList.length-1] = CreateArray(2,0);
//	changeSet[sSourceList.length-1][0][0] = "<div class=\"doctitle\"><title>";
//	changeSet[sSourceList.length-1][1][0] = changeSet[sSourceList.length-1][0][0] + "Retinotopic Mapping output files Tutorial" + "</title></div>" + sHTMLComment;
//	changeSet[sSourceList.length-1][0][1] = "<div class=\"topheader\"><h1>";
//	changeSet[sSourceList.length-1][1][1] = changeSet[sSourceList.length-1][0][1] + "Retinotopic Mapping output files Tutorial</h1></div>" + sHTMLComment;
//	changeSet[sSourceList.length-1][0][2] = "<div class=\"versioning\">";
//	changeSet[sSourceList.length-1][1][2] = changeSet[sSourceList.length-1][0][2] + "<p><em><strong>Version 1.0.0.1</strong></em></p></div>" + sHTMLComment;
//	changeSet[sSourceList.length-1][0][3] = "<div class=\"authoring\">";
//	changeSet[sSourceList.length-1][1][3] = changeSet[sSourceList.length-1][0][3] + "<p><em><strong>" + sCurrentAuthoringMonth + " by " + sMainAuthor + "</strong></em></p></div>" + sHTMLComment;
//
//sSourceList.push(sSourcePath + "Tutorials/15_NeuroFeedBack/15_NeuroFeedBack.html");
//sDestList.push(sSourceList[sSourceList.length-1]);
//	changeSet[sSourceList.length-1] = CreateArray(2,0);
//	changeSet[sSourceList.length-1][0][0] = "<div class=\"doctitle\"><title>";
//	changeSet[sSourceList.length-1][1][0] = changeSet[sSourceList.length-1][0][0] + "Neurofeedback Tutorial" + "</title></div>" + sHTMLComment;
//	changeSet[sSourceList.length-1][0][1] = "<div class=\"topheader\"><h1>";
//	changeSet[sSourceList.length-1][1][1] = changeSet[sSourceList.length-1][0][1] + "Neurofeedback Tutorial</h1></div>" + sHTMLComment;
//	changeSet[sSourceList.length-1][0][2] = "<div class=\"versioning\">";
//	changeSet[sSourceList.length-1][1][2] = changeSet[sSourceList.length-1][0][2] + "<p><em><strong>Version 1.0.0.1</strong></em></p></div>" + sHTMLComment;
//	changeSet[sSourceList.length-1][0][3] = "<div class=\"authoring\">";
//	changeSet[sSourceList.length-1][1][3] = changeSet[sSourceList.length-1][0][3] + "<p><em><strong>" + sCurrentAuthoringMonth + " by " + sMainAuthor + "</strong></em></p></div>" + sHTMLComment;

/////////////////////////CONFIGURATION(END)//////////////////////////////////////////////

//BrainStim.openFiles("",sSourceList);
var nFiles = sSourceList.length;
if(bDoProcess)
{
	Log("Total " + nFiles + " file(s).");

	for(var nFileCounter=0;nFileCounter<nFiles;nFileCounter++)
	{
		//changeSet = CreateArray(nFiles,2,0);
		nChangeCounter = 0;
		bDoProcess = true;
		if(nFileCounter == 0)
		{
			//bDoProcess = false;
		}
		else
		{		
			//bDoProcess = false;
		}
		
		if(bDoProcess)
		{
			Log("\n- File: " + sSourceList[nFileCounter]);
			ProcessInFiles(sSourceList[nFileCounter],sDestList[nFileCounter],changeSet[nFileCounter]);		
		}
	}
}

CleanupScript();


/////Function Definitions/////
function tr(s) { return s; }

function stringToHTMLCompatibleCode(sInput)
{
	sInput = sInput.replace(/\</g,"&lt;")   //for <
	sInput = sInput.replace(/\>/g,"&gt;")   //for >
	return sInput;
}

function CreateTimeStamp(sTime)
{
	var sep = "";//".";
	return (sTime.getFullYear()-2000) + sep + sTime.getMonth() + sep + sTime.getDate() + sep + sTime.getHours() + sep + sTime.getMinutes() + sep + sTime.getSeconds();
}

function CreateArray(length) 
{
	var a = new Array(length || 0);
	if (arguments.length > 1) 
	{
		var args = Array.prototype.slice.call(arguments, 1);
		for (var i = 0; i < length; i++) 
		{
			a[i] = CreateArray.apply(this, args);
		}
	}
	return a;
}

QByteArray.prototype.toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function quotedString(sValue)
{
	return "\"" + sValue + "\"";
}

function CreateNewContent(strTemplate,changeSetArray)
{	
	var arrLines = strTemplate.split(/\r?\n|\r/);
	Log("  - Number of lines: " + arrLines.length);
	var nStartIndex = 0;
	var sSearchval = "";
	for(var nChanges=0;nChanges<changeSetArray[0].length;nChanges++)//For all changes
	{
		sSearchval = changeSetArray[0][nChanges];
		for(var nLines=0;nLines<arrLines.length;nLines++)//For all lines
		{
			if (arrLines[nLines].indexOf(sSearchval, nStartIndex) != -1) 
			{
				Log("    - Value(\"" + stringToHTMLCompatibleCode(sSearchval) + "\") found @ line:" + nLines);
				arrLines[nLines] = changeSetArray[1][nChanges];
				//Log(stringToHTMLCompatibleCode(changeSetArray[1][nChanges]));
			}
		}
	}
	strTemplate = arrLines.join("\n");
	return strTemplate;
}

function ProcessInFiles(strTemplate, strDestination, changeSetArr)
{
	var templateFile = new QFile(strTemplate);
	templateFile.open(QIODevice.OpenMode(QIODevice.ReadOnly) || QIODevice.OpenMode(QIODevice.Text));
	var inStream = new QTextStream(templateFile);
	var inData = new String();
	//inData = inStream.readLine();
	inData = inStream.readAll();
	//Log(inData);
	templateFile.close();
	///////////////////////////////////////////////////////////////////////////////////////
	var outputFile = new QFile(strDestination);
	outputFile.open(QIODevice.OpenMode(QIODevice.WriteOnly) || QIODevice.OpenMode(QIODevice.Text));
	var outStream = new QTextStream(outputFile);
	inData = CreateNewContent(inData,changeSetArr);
	var outData = new QByteArray(inData);// or 'text'
	//Log(outData.GDCF_toString());
	outputFile.write(outData);
	outputFile.close();
	//ofile.rename("E:/workplace/file2.txt","E:/workplace/file1.txt");	
	templateFile = null;
	inStream = null;
	inData = null;
	outputFile = null;
	outStream = null;
	outData = null;
	return true;
}

function getFileName()
{
	var options = 0;
	//options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	//( parent, caption, dir, filter, selectedFilter, options)
	var fileName = QFileDialog.getOpenFileName(this,
				tr("QFileDialog::getOpenFileName()"),
				sScriptPath,
				tr("JavaScript Files (*.js)"),
				selectedFilter,
				QFileDialog.Options(options));
	if (fileName.length != 0)
		return fileName;
	else
		return "";
}

function CleanupScript()
{
	sSourceList = null;
	sDestList = null;
	sTimeObject = null;
	CreateTimeStamp = null;
	tmpString = null;
	changeSet= null;
	ProcessInFiles = null;
	QByteArray.prototype.toString =null;
	//QByteArray.prototype = null;
	CreateNewContent = null;
	CreateArray = null;
	sSourceList = null;
	getFileName = null;
	CleanupScript = null;
	tr = null;
	BrainStimInfo = null;
	quotedString = null;
	stringToHTMLCompatibleCode = null;
	Log("\nScript ended");
	BrainStim.cleanupScript();
	BrainStim.restartScriptEngine();
}
