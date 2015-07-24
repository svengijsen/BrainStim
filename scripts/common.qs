///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
var BrainStim_Settings_QTDirWin32 = "E:/Libraries/Qt5.3.2_32bit/5.3/msvc2013_opengl/";
var BrainStim_Settings_QTDirWin64 = "E:/Libraries/Qt5.3.2_64bit/5.3/msvc2013_64_opengl/";
var BrainStim_Settings_ProjectDirectory = "../../BrainStim";
var BrainStim_Settings_JSConfigurationFilePath = BrainStim_Settings_ProjectDirectory + "/Source/BrainStim/resources/brainstim.js";
var BrainStim_Settings_CurrentBrainStimReleaseIndex = "";
var BrainStim_Settings_CurrentBrainStimReleaseString = "";
var BrainStim_Settings_CurrentBrainStimReleaseStringArray = "";
var BrainStim_Settings_CurrentBrainStimProductVersionString = "";
var BrainStim_Settings_CurrentBrainStimProductVersionStringArray = "";
var BrainStim_Settings_LatestBrainStimReleaseIndex = -1;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


var tmpCommonbProceed = true;
var tmpCommonBrainStimInfo = null;
var tmpCommonFile = new QFile();

function commonGetFileName()
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

tmpCommonFile.setFileName(BrainStim_Settings_JSConfigurationFilePath);
if(tmpCommonFile.exists() == false)
{
	BrainStim_Settings_ProjectDirectory = setFileName();
	tmpCommonFile.setFileName(BrainStim_Settings_ProjectDirectory);
	if(QFile(BrainStim_Settings_JSConfigurationFilePath).exists() == false)
	{
		Log("Javascript configuration file not found.\n");
		tmpCommonbProceed = false
	}
}

if(tmpCommonbProceed)
{	
	Log("JS Configuration file: " + BrainStim_Settings_JSConfigurationFilePath);
	Include(BrainStim_Settings_JSConfigurationFilePath);
	tmpCommonBrainStimInfo = new BrainStim_Information();
	
	BrainStim_Settings_CurrentBrainStimReleaseIndex = tmpCommonBrainStimInfo.GetNumberOfReleases() - 1;
	BrainStim_Settings_CurrentBrainStimReleaseString = tmpCommonBrainStimInfo.GetCurrentRelease();
	BrainStim_Settings_CurrentBrainStimReleaseString_Array = BrainStim_Settings_CurrentBrainStimReleaseString.split(".");
	BrainStim_Settings_CurrentBrainStimProductVersionString = tmpCommonBrainStimInfo.GetReleaseProductVersionByIndex(BrainStim_Settings_CurrentBrainStimReleaseIndex);
	BrainStim_Settings_CurrentBrainStimProductVersionString_Array = BrainStim_Settings_CurrentBrainStimProductVersionString.split(".");
	BrainStim_Settings_LatestBrainStimReleaseIndex = tmpCommonBrainStimInfo.GetNumberOfReleases()-1;
	
}

tmpCommonFile = null;
commonGetFileName = null;
BrainStim_Cleanup();
tmpCommonBrainStimInfo = null;