Include("common.qs");
Include("../js/BrainStim.js");

var tmpString = new String();
var sTimeObject = new Date();//gettime() returns Milliseconds since midnight January 1, 1970
//Log(sTimeObject.toTimeString());
//Log(sTimeObject.toTimeString().replace(/.*(\d{2}:\d{2}:\d{2}).*/, "$1"));//returns 08:55:57
//var docVer_sDestPath = "D:/Projects/BrainStim/Publish/";
var sSourceList = [];
var sDestList = [];
var changeSet = [];
var bDoProcess = true;
var nChangeCounter;
var tmpMinimalXMLString = "";
var tmpMinimalXMLString_Array = "";
var sComponentName = "";
var sComponentIndex = "";

Include(BrainStim_Settings_JSConfigurationFilePath);
var tmpBrainStimInfo = new BrainStim_Information();
var tmpBrainStimDocumentInfo = new BrainStimDocuments_Information();
	
/////////////////////////CONFIGURATION(BEGIN)////////////////////////////////////////////
var docVer_sSourcePath 			= "../";
var docVer_sDestPath 			= docVer_sSourcePath;
var docVer_sMainAuthor			= "Sven Gijsen";
var docVer_sCurrentAuthoringMonth 	= "Juli 2015";
var docVer_sComment 			= "Please do not edit the below line(s) manually, see DocumentVersioning.qs (" + CreateTimeStamp(sTimeObject) + ")";
var docVer_sScriptComment  		= "//" + docVer_sComment + ", version " + BrainStim_Settings_CurrentBrainStimReleaseString;
var docVer_sHTMLComment  		= "<!-- " + docVer_sComment + ", version " + BrainStim_Settings_CurrentBrainStimReleaseString + "-->";
/////////////////////////CONFIGURATION(END)//////////////////////////////////////////////

/////Function Definitions/////
function tr(s) { return s; }

function defaultDocumentEditing(sPath, sFileName, nDirOffset)//"documents/BrainStim/settingsDialog.html", "SettingsDialog.html", 2
{
	sSourceList.push(docVer_sSourcePath + sPath);
	sDestList.push(sSourceList[sSourceList.length-1]);
		changeSet[sSourceList.length-1] = CreateArray(3,0);
		changeSet[sSourceList.length-1][0][0] = "<!--doc-version-title-begin-->";
		changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + "<title>" + tmpBrainStimDocumentInfo.GetDocumentTitle(sFileName) + "</title>";
		changeSet[sSourceList.length-1][2][0] = "<!--doc-version-title-end-->";

	sSourceList.push(docVer_sSourcePath + sPath);
	sDestList.push(sSourceList[sSourceList.length-1]);
		changeSet[sSourceList.length-1] = CreateArray(3,0);
		changeSet[sSourceList.length-1][0][0] = "<!--doc-version-default-header-begin-->";
		changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" +
			CreateHTMLHeaderSection(tmpBrainStimDocumentInfo.GetDocumentName(sFileName),
			tmpBrainStimDocumentInfo.GetDocumentVersion(sFileName),
			tmpBrainStimDocumentInfo.GetDocumentAuthors(sFileName),
			tmpBrainStimDocumentInfo.GetDocumentDate(sFileName));
		changeSet[sSourceList.length-1][2][0] = "<!--doc-version-default-header-end-->";

	sSourceList.push(docVer_sSourcePath + sPath);
	sDestList.push(sSourceList[sSourceList.length-1]);
		changeSet[sSourceList.length-1] = CreateArray(3,0);
		changeSet[sSourceList.length-1][0][0] = "<!--doc-version-default-links-begin-->";
		changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
									CreateDefaultHTMLHeaderLinks(nDirOffset);
		changeSet[sSourceList.length-1][2][0] = "<!--doc-version-default-links-end-->";
		
	sSourceList.push(docVer_sSourcePath + sPath);
	sDestList.push(sSourceList[sSourceList.length-1]);
		changeSet[sSourceList.length-1] = CreateArray(3,0);
		changeSet[sSourceList.length-1][0][0] = "<!--doc-version-default-footer-begin-->";
		changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
									CreateHTMLFooterSection();
		changeSet[sSourceList.length-1][2][0] = "<!--doc-version-default-footer-end-->";
}

function CreateDefaultHTMLHeaderLinks(nLevelsAbove)
{
	sDirLevels = "";
	for(var i=0;i<nLevelsAbove;i++)
		sDirLevels = sDirLevels + "../";
	return "<!-- Bootstrap core CSS -->\n" +
	"<link href=\"" + sDirLevels + "bootstrap/css/bootstrap.min.css\" type=\"text/css\" rel=\"stylesheet\"/>\n" +
	"<!-- Bootstrap theme -->\n" +
	"<link href=\"" + sDirLevels + "bootstrap/css/bootstrap-theme.min.css\" type=\"text/css\" rel=\"stylesheet\"/>\n" +
	"<!-- Code-Prettify -->\n" +
	"<link href=\"" + sDirLevels + "google-code-prettify/prettify.css\" type=\"text/css\" rel=\"stylesheet\"/>\n" +
	"<script src=\""+ sDirLevels + "google-code-prettify/prettify.js\" type=\"text/javascript\"></script>\n" +
	"<!-- Custom styles for this template -->\n" +
	"<link href=\"" + sDirLevels + "css/custom_theme.css\" type=\"text/css\" rel=\"stylesheet\"/>";
}

function CreateTocifyHTMLHeaderLinks(nLevelsAbove)
{
	sDirLevels = "";
	for(var i=0;i<nLevelsAbove;i++)
		sDirLevels = sDirLevels + "../";
	return "<!-- Tocify -->\n" +
	"<link href=\"" + sDirLevels + "css/jquery.tocify.css\" type=\"text/css\" rel=\"stylesheet\"/>\n" +
	"<script src=\"" + sDirLevels + "js/jquery.tocify.min.js\" type=\"text/javascript\"></script>";
}

function CreateTocifyHTMLFooter()
{
	return "<script type=\"text/javascript\">$(function() {\n" +
	"var toc = $(\"#toc\").tocify({selectors: \"h2,h3,h4\"}).data(\"toc-tocify\");\n" +
	"toc.setOptions({ smoothScroll: false, showEffect: \"none\" });\n" +
	"if (document.documentElement.clientWidth < 992)\n" +
		"toc.setOptions({ showAndHideOnScroll: false }); 	/*, showAndHide: false*/\n" +
	"});</script>";
}

function CreateJQueryHTMLHeaderLinks(nLevelsAbove)
{
	sDirLevels = "";
	for(var i=0;i<nLevelsAbove;i++)
		sDirLevels = sDirLevels + "../";
	return "<!-- JQuery(UI) -->\n" +
	"<script src=\"" + sDirLevels + "js/jquery.min.js\" type=\"text/javascript\"></script>\n" +
	"<script src=\"" + sDirLevels + "js/jquery-ui/jquery-ui.min.js\" type=\"text/javascript\"></script>";
}

function CreateHTMLHeaderSection(sHeaderTitle, sDocumentVersion, sDocumentAuthors, sDocumentDate)
{
	return "\t<div class=\"doc-header\">\n" +
	"\t\t<div class=\"alert alert-info\" role=\"alert\">\n" +
	"\t\t\t<h1>" + sHeaderTitle + "</h1>\n" +
	"\t\t\t<div class=\"row\">\n" +
	"\t\t\t\t<div class=\"col-md-4\"><p><strong>Document version: </strong>" + sDocumentVersion + "</p></div>\n" +
	"\t\t\t\t<div class=\"col-md-4\"><p><strong>Author(s): </strong>" + sDocumentAuthors + "</p></div>\n" +
	"\t\t\t\t<div class=\"col-md-4\"><p><strong>Date: </strong>" + sDocumentDate + "</p></div>\n" +
	"\t\t\t</div>\n" +
	"\t\t</div>\n" +
	"\t</div>";	
}

function CreateHTMLFooterSection()
{
	return "\t<script type=\"text/javascript\">document.body.onload = function() {prettyPrint()};</script>";
}

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
				BrainStim.getActiveDocumentFileLocation() + "/",
				tr("JavaScript Files (*.js)"),
				selectedFilter,
				QFileDialog.Options(options));
	if (fileName.length != 0)
		return fileName;
	else
		return "";
}

function CreateNewContent(strTemplate,changeSetArray)
{	
	var arrLines = strTemplate.split(/\r?\n|\r/);
	Log("  - Number of lines: " + arrLines.length);
	var nStartIndex = 0;
	var sSearchval = "";
	var bDoNothing = false;
	var nStartLine = -1;
	var nEndLine = -1;
	var nLastUsedLine = -1;
	var arrResult = [];
	
	for(var nChanges=0;nChanges<changeSetArray[0].length;nChanges++)//For all changes
	{
		nStartLine = -1;
		nEndLine = -1;
		for(var nCurrentLine=0;nCurrentLine<arrLines.length;nCurrentLine++)//For all lines
		{
			sSearchval = changeSetArray[0][nChanges];//sSearchval = "<!--doc-version-title-begin-->";
			if(nCurrentLine==0)
				Log("Searching for tag: "+ sSearchval);
			if (arrLines[nCurrentLine].indexOf(sSearchval, 0) != -1) 
			{
				//Log("    - Value(\"" + stringToHTMLCompatibleCode(sSearchval) + "\") found @ line:" + nCurrentLine);
				nStartLine = nCurrentLine;
			}
			sSearchval = changeSetArray[2][nChanges];//"<!--doc-version-title-end-->";
			if (arrLines[nCurrentLine].indexOf(sSearchval, 0) != -1) 
			{
				//Log("    - Value(\"" + stringToHTMLCompatibleCode(sSearchval) + "\") found @ line:" + nCurrentLine);
				nEndLine = nCurrentLine;
			}
			if((nStartLine>=0) && (nEndLine>=nStartLine))
			{
				Log("Found Value(\"" + stringToHTMLCompatibleCode(sSearchval) + "\") @ Start(" + nStartLine + "), End(" + nEndLine + ")");
				if(nLastUsedLine > -1)//already started?
					arrResult = arrResult.concat(arrLines.slice(nLastUsedLine+1,nStartLine));//slice has a start and stop parameter (stop is not included!)
				else
					arrResult = arrResult.concat(arrLines.slice(0,nStartLine));//slice has a start and stop parameter (stop is not included!)
				arrResult = arrResult.concat(changeSetArray[0][nChanges]);
				arrResult = arrResult.concat(changeSetArray[1][nChanges]);
				arrResult = arrResult.concat(changeSetArray[2][nChanges]);
				nLastUsedLine = nEndLine;
				nStartLine = -1;
				nEndLine = -1;
			}
		}
	}
	if(bDoNothing==false)
	{
		if(nLastUsedLine > -1)//already started?
			arrResult = arrResult.concat(arrLines.slice(nLastUsedLine+1));//slice has a start and stop parameter (stop is not included!)
		else
			arrResult = arrResult.concat(arrLines.slice(0));//slice has a start and stop parameter (stop is not included!)
		//Log("--Begin--");
		//for(var i=0;i<arrResult.length;i++)
		//	Log("\t" + arrResult[i]);
		//Log("--End--");
		strTemplate = arrResult.join("\n");
	}
	return strTemplate;
}

function startDocumentEditing()
{
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
			/*
			if(nFileCounter == 0)
			{
				bDoProcess = false;
			}
			else
			{		
				bDoProcess = false;
			}
			*/
			if(bDoProcess)
			{
				Log("\n- File: " + sSourceList[nFileCounter]);
				ProcessInFiles(sSourceList[nFileCounter],sDestList[nFileCounter],changeSet[nFileCounter]);		
			}
		}
	}
}

function docVer_CleanupScript()
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
	startDocumentEditing = null;
	docVer_CleanupScript = null;
	CreateHTMLHeaderSection = null;
	CreateDefaultHTMLHeaderLinks = null;
	CreateTocifyHTMLHeaderLinks = null;
	CreateTocifyHTMLFooter = null;
	CreateJQueryHTMLHeaderLinks = null;
	CreateHTMLFooterSection = null;
	defaultDocumentEditing = null;
	tr = null;
	quotedString = null;
	stringToHTMLCompatibleCode = null;
	tmpBrainStimInfo = null;
	tmpBrainStimDocumentInfo = null;
	BrainStimDocuments_Cleanup();
	BrainStim_Cleanup();
	Log("\nScript ended");
	BrainStim.cleanupScript();
	BrainStim.restartScriptEngine();
}
