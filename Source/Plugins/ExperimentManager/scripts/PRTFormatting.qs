var PRTFormatManagerObj = new PRTFormatManager();//See also http://www.brainvoyager.de/BV2000OnlineHelp/BrainVoyagerWebHelp/mergedProjects/FileFormats/The_format_of_PRT_files.htm

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Log("Starting PRT Formatting");
var sReturnString = new String("bb");
var bRetVal = new Boolean;
var nCondition;
var nInterval;

bRetVal = PRTFormatManagerObj.setParameterValue("xx", "appel");//Doesn't work, only allowed valid keys work!
if(bRetVal == true)
{
	sReturnString = PRTFormatManagerObj.getParameterValue("xx");
	if(sReturnString != "")
		Log("getParameter() successfully returned: " + sReturnString);
}
else
{
	Log("getParameter() failed");
}

bRetVal = PRTFormatManagerObj.setParameterValue("TimeCourseThick", "99");//This does work for PRT-file version 2
if(bRetVal == true)
{
	sReturnString = PRTFormatManagerObj.getParameterValue("TimeCourseThick");
	if(sReturnString != "")
	{
		Log("getParameter() successfully returned: " + sReturnString);
		sReturnString = PRTFormatManagerObj.getParameterDescription("TimeCourseThick");
		if(sReturnString != "")
			Log("getParameterDescription() successfully returned: " + sReturnString);
	}	
}
else
{
	Log("getParameter() failed");
}

nCondition = PRTFormatManagerObj.appendCondition("Fixation", "255 0 0");
if(nCondition != -1)
{
	Log("appendCondition() successfully returned: " + nCondition);
	nInterval = PRTFormatManagerObj.appendInterval(nCondition, 3, 27);
	if(nInterval != -1)
		Log("appendInterval() successfully returned: " + nInterval);
		nInterval = PRTFormatManagerObj.appendInterval(nCondition, 55, 99);
	if(nInterval != -1)
		Log("appendInterval() successfully returned: " + nInterval);
}
if(PRTFormatManagerObj.loadFile("SubjectName_Date_ExperimentName_run1.prt"))
{
	Log("loadFile() successfull");
}

if(PRTFormatManagerObj.saveFile("SubjectName_Date_ExperimentName_run1_new.prt", true))
{
	Log("saveFile() successfull");
}

PRTFormatManagerObj.deleteLater();
PRTFormatManagerObj = null;
Log("Stopped PRT Formatting");

