var sScriptPath = BrainStim.getActiveDocumentFileLocation() + "/";
var tmpString = new String();
var sTimeObject = new Date();//gettime() returns Milliseconds since midnight January 1, 1970
//Log(sTimeObject.toTimeString());
//Log(sTimeObject.toTimeString().replace(/.*(\d{2}:\d{2}:\d{2}).*/, "$1"));//returns 08:55:57

function tr(s) { return s; }

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
				Log("    - Value(\"" + sSearchval + "\") found @ line:" + nLines);
				arrLines[nLines] = changeSetArray[1][nChanges];
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
	jsConfigurationFile = null;
	sTimeObject = null;
	CreateTimeStamp = null;
	tmpString = null;
	changeSet= null;
	ProcessInFiles = null;
	QByteArray.prototype.toString =null;
	CreateNewContent = null;
	CreateArray = null;
	sSourceList = null;
	getFileName = null;
	CleanupScript = null;
	tr = null;
	BrainStimInfo = null;
	quotedString = null;
	Log("\nScript ended");
	BrainStim.cleanupScript();
}

var sSourcePath = "E:/Projects/BrainStim/Source/";
var sDestPath = sSourcePath;
//var sDestPath = "D:/Projects/BrainStim/Publish/";
var sSourceList = new CreateArray();
var sDestList = new CreateArray();
sSourceList.push(sSourcePath + "BrainStim/maindefines.h");//"E:/Projects/BrainStim/Source/BrainStim/maindefines.h");
sDestList.push(sDestPath + "BrainStim/maindefines.h");//0
sSourceList.push(sSourcePath + "Plugins/ExperimentManager/defines.h");
sDestList.push(sDestPath + "Plugins/ExperimentManager/defines.h");//1
sSourceList.push(sSourcePath + "Plugins/KeyBoardDevice/defines.h");
sDestList.push(sDestPath + "Plugins/KeyBoardDevice/defines.h");//2
sSourceList.push(sSourcePath + "Plugins/ParallelPortDevice/defines.h");
sDestList.push(sDestPath + "Plugins/ParallelPortDevice/defines.h");//3
sSourceList.push(sSourcePath + "QmlExtensions/Plugins/DefaultPlugin/defines.h");
sDestList.push(sDestPath + "QmlExtensions/Plugins/DefaultPlugin/defines.h");//4

//BrainStim.openFiles("",sSourceList);

var nFiles = sSourceList.length;
var changeSet;
var sComment = "   //Please do not edit this line manually, see BinaryVersioning.qs (" + CreateTimeStamp(sTimeObject) + ").";
var bDoProcess = true;
var nChangeCounter;
var jsConfigurationFile = new QFile("E:/Projects/BrainStim/Source/BrainStim/resources/BrainStim.js");

Log("");
if(jsConfigurationFile.exists() == false)
{
	jsConfigurationFile.setFileName("D:/Projects/BrainStim/Source/BrainStim/resources/BrainStim.js");
	if(jsConfigurationFile.exists() == false)
	{
		jsConfigurationFile.setFileName(getFileName());
		if(jsConfigurationFile.exists() == false)
		{
			Log("Javascript configuration file not found.\n");
			bDoProcess = false;
		}
	}
}
Include(jsConfigurationFile.fileName());
//BrainStim.openFiles(jsConfigurationFile.fileName());
var BrainStimInfo = new BrainStim_Information();
var BrainStim_CurrentReleaseString_Index = BrainStimInfo.GetNumberOfReleases() - 1;
var BrainStim_LatestCompReleaseString_Index = "";
var tmpCurrentReleaseString = "";
var tmpCurrentReleaseString_Array = "";
var tmpMinimalXMLString = "";
var tmpMinimalXMLString_Array = "";
var tmpProductVersionString = "";
var tmpProductVersionString_Array = "";
var sComponentName = "";
var sComponentIndex = "";

if(bDoProcess)
{
	Log("Total " + nFiles + " file(s).");
	for(var nFileCounter=0;nFileCounter<nFiles;nFileCounter++)
	{
		changeSet = CreateArray(nFiles,2,0);
		nChangeCounter = 0;
		bDoProcess = false;
		if(nFileCounter == 0)//"BrainStim/maindefines.h"
		{
			tmpCurrentReleaseString = BrainStimInfo.GetCurrentRelease();
			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
			tmpProductVersionString = BrainStimInfo.GetReleaseProductVersionByIndex(BrainStim_CurrentReleaseString_Index);
			tmpProductVersionString_Array = tmpProductVersionString.split(".");
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_FILE_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_FILE_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_FILE_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_FILE_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
			nChangeCounter++;			
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
			nChangeCounter++;
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_LEGAL_COPYRIGHT";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetMainAppCopyrightString()) + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_COMPANY_NAME";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetMainAppCompanyName()) + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_INTERNAL_NAME";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetMainAppInternalName()) + sComment;
			nChangeCounter++;			

			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_DEVICE_INTERFACE";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetMainAppPluginDeviceInterfaceString()) + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define MAIN_PROGRAM_EXTENSION_INTERFACE";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetMainAppPluginExtensionInterfaceString()) + sComment;
			nChangeCounter++;	
			bDoProcess = true;
		}
		else if(nFileCounter == 1)//"Plugins/ExperimentManager/defines.h"
		{
			sComponentName = "ExperimentManagerPlugin";
			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
			
			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
			tmpProductVersionString = BrainStimInfo.GetComponentProductVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpProductVersionString_Array = tmpProductVersionString.split(".");
			tmpMinimalXMLString = BrainStimInfo.GetComponentMinimalEXMLVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpMinimalXMLString_Array = tmpMinimalXMLString.split(".");			
					
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
			nChangeCounter++;			
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
			nChangeCounter++;
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_EXMLDOC_MINIMAL_VERSION_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpMinimalXMLString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_EXMLDOC_MINIMAL_VERSION_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpMinimalXMLString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_EXMLDOC_MINIMAL_VERSION_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpMinimalXMLString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_EXMLDOC_MINIMAL_VERSION_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpMinimalXMLString_Array[3] + sComment;
			nChangeCounter++;			
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_EXMLDOC_VALIDATION_NAME";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + "PLUGIN_EXMLDOC_EXTENSION \"_v" + tmpMinimalXMLString_Array.join('') + ".xsd\"" + sComment;//PLUGIN_EXMLDOC_EXTENSION "_2001.xsd"
			nChangeCounter++;

			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetReleaseByIndex(BrainStim_LatestCompReleaseString_Index)) + sComment;
			nChangeCounter++;	
			bDoProcess = true;
		}
		else if(nFileCounter == 2)//"Plugins/KeyBoardDevice/defines.h"
		{
			sComponentName = "KeyBoardPlugin";
			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
			
			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
			tmpProductVersionString = BrainStimInfo.GetComponentProductVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpProductVersionString_Array = tmpProductVersionString.split(".");
			tmpMinimalXMLString = BrainStimInfo.GetComponentMinimalEXMLVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpMinimalXMLString_Array = tmpMinimalXMLString.split(".");			
					
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
			nChangeCounter++;			
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
			nChangeCounter++;

			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetReleaseByIndex(BrainStim_LatestCompReleaseString_Index)) + sComment;
			nChangeCounter++;
			bDoProcess = true;	
		}
		else if(nFileCounter == 3)//"Plugins/ParallelPortDevice/defines.h"
		{
			sComponentName = "ParallelPortPlugin";			
			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
			
			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
			tmpProductVersionString = BrainStimInfo.GetComponentProductVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpProductVersionString_Array = tmpProductVersionString.split(".");
			tmpMinimalXMLString = BrainStimInfo.GetComponentMinimalEXMLVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpMinimalXMLString_Array = tmpMinimalXMLString.split(".");			
					
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
			nChangeCounter++;			
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
			nChangeCounter++;

			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetReleaseByIndex(BrainStim_LatestCompReleaseString_Index)) + sComment;
			nChangeCounter++;
			bDoProcess = true;	
		}
		else if(nFileCounter == 4)//"QmlExtensions/Plugins/DefaultPlugin/defines.h"
		{
			sComponentName = "BrainStim_QMLExtensions";			
			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
			
			changeSet[nFileCounter][0][nChangeCounter] = "#define QML_VERSION_STRING_MAJOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
			nChangeCounter++;
			changeSet[nFileCounter][0][nChangeCounter] = "#define QML_VERSION_STRING_MINOR";
			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
			nChangeCounter++;
			
//			sComponentName = "MediaPlayer";			
//			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
//			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
//			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PHONONPLAYER_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString + sComment;
//						
//			nChangeCounter++;
			bDoProcess = true;	
		}
//		else if(nFileCounter == 7)//"Plugins/FirebirdClient/defines.h"
//		{
//			sComponentName = "FireBirdClientPlugin";			
//			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
//			
//			Log(BrainStim_LatestCompReleaseString_Index);
//			
//			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
//			
//			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
//			tmpProductVersionString = BrainStimInfo.GetComponentProductVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpProductVersionString_Array = tmpProductVersionString.split(".");
//			tmpMinimalXMLString = BrainStimInfo.GetComponentMinimalEXMLVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpMinimalXMLString_Array = tmpMinimalXMLString.split(".");			
//					
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
//			nChangeCounter++;			
//			
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
//			nChangeCounter++;
//
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetReleaseByIndex(BrainStim_LatestCompReleaseString_Index)) + sComment;
//			nChangeCounter++;	
//			bDoProcess = true;		
//		}
//		else if(nFileCounter == 8)//"Plugins/PiezoStimDevice/defines.h"
//		{
//			sComponentName = "PiezoStimDevicePlugin";			
//			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
//			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
//			
//			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
//			tmpProductVersionString = BrainStimInfo.GetComponentProductVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpProductVersionString_Array = tmpProductVersionString.split(".");
//			tmpMinimalXMLString = BrainStimInfo.GetComponentMinimalEXMLVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpMinimalXMLString_Array = tmpMinimalXMLString.split(".");			
//					
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
//			nChangeCounter++;			
//			
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
//			nChangeCounter++;
//
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetReleaseByIndex(BrainStim_LatestCompReleaseString_Index)) + sComment;
//			nChangeCounter++;	
//			bDoProcess = true;	
//		}		
//		else if(nFileCounter == 9)//"Plugins/TBVExchanger/defines.h"
//		{
//			sComponentName = "TBVExchangerPlugin";			
//			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
//			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
//			
//			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
//			tmpProductVersionString = BrainStimInfo.GetComponentProductVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpProductVersionString_Array = tmpProductVersionString.split(".");
//			tmpMinimalXMLString = BrainStimInfo.GetComponentMinimalEXMLVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpMinimalXMLString_Array = tmpMinimalXMLString.split(".");			
//					
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
//			nChangeCounter++;			
//			
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
//			nChangeCounter++;
//
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetReleaseByIndex(BrainStim_LatestCompReleaseString_Index)) + sComment;
//			nChangeCounter++;	
//			bDoProcess = true;	
//		}		
//		else if(nFileCounter == 10)//"Plugins/TCPNetworkServer/defines.h"
//		{
//			sComponentName = "TCPNetworkServerPlugin";			
//			BrainStim_LatestCompReleaseString_Index = BrainStimInfo.GetLatestComponentReleaseByName(sComponentName);
//			sComponentIndex = BrainStimInfo.GetLatestComponentIndexByName(BrainStim_LatestCompReleaseString_Index,sComponentName);
//			
//			tmpCurrentReleaseString = BrainStimInfo.GetComponentVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpCurrentReleaseString_Array = tmpCurrentReleaseString.split(".");
//			tmpProductVersionString = BrainStimInfo.GetComponentProductVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpProductVersionString_Array = tmpProductVersionString.split(".");
//			tmpMinimalXMLString = BrainStimInfo.GetComponentMinimalEXMLVersionByIndexes(BrainStim_LatestCompReleaseString_Index,sComponentIndex);
//			tmpMinimalXMLString_Array = tmpMinimalXMLString.split(".");			
//					
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_FILE_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpCurrentReleaseString_Array[3] + sComment;
//			nChangeCounter++;			
//			
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[0] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_MINOR";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[1] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_REVISION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[2] + sComment;
//			nChangeCounter++;
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_PRODUCT_VERSION_STRING_BUILD";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + tmpProductVersionString_Array[3] + sComment;
//			nChangeCounter++;
//
//			changeSet[nFileCounter][0][nChangeCounter] = "#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION";
//			changeSet[nFileCounter][1][nChangeCounter] = changeSet[nFileCounter][0][nChangeCounter] + "\t" + quotedString(BrainStimInfo.GetReleaseByIndex(BrainStim_LatestCompReleaseString_Index)) + sComment;
//			nChangeCounter++;	
//			bDoProcess = true;	
//		}		
		else
		{		
			bDoProcess = false;
		}
		
		if(bDoProcess)
		{
			Log("\n- File: " + sSourceList[nFileCounter]);
			ProcessInFiles(sSourceList[nFileCounter],sDestList[nFileCounter],changeSet[nFileCounter]);		
		}
	}
}
CleanupScript();
