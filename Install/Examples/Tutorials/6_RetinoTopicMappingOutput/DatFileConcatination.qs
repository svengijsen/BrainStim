var DatFileConcat_ImageProcessorObj = new ImageProcessor();
var DatFileConcat_currentScriptLocation = StimulGL.getActiveDocumentFileLocation();
var DatFileConcat_searchDialogOptions = QFileDialog.DontResolveSymlinks | QFileDialog.ShowDirsOnly | QFileDialog.DontUseNativeDialog;
var DatFileConcat_saveDialogOptions = QFileDialog.DontUseNativeDialog;//0
var DatFileConcat_searchDatDirectory = "";
var DatFileConcat_resultCDatDirectory = "";
var sCDATFileName = "result.cdat";
var nFilesSplitted;
var fileArray = Array[];
var dirInfo = new QDir();
var bProceed = true;

function DatFileConcat_concat(searchDatDirectory,resultConcatDatPath)
{
	var dirDirectory;
	var dirFileCount;
	var filePathArray = Array[];
	var selectedSaveFilter;
	var i;
	
	if (searchDatDirectory.length != 0)
	{
		dirDirectory = new QDir();
		dirDirectory.setPath(searchDatDirectory);
		 if (dirDirectory.exists())
		 {
			searchDatDirectory = dirDirectory.absolutePath();
			Log("Choosen source (*.dat) directory: " + searchDatDirectory);
			dirDirectory.setFilter(QDir.Files);// | QDir.Hidden | QDir.NoSymLinks);
			dirDirectory.setNameFilters(Array["*.dat"]);
			dirDirectory.setSorting(QDir.Name);//::Size | QDir::Reversed);
			dirFileCount = dirDirectory.count();
			if(dirFileCount>0)
			{
				fileArray = dirDirectory.entryList();
				Log("The directory contains " + dirFileCount + " file(s): ");
				for(i=0;i<fileArray.length;i++)
				{
					Log("\t- " + fileArray[i]);
					filePathArray[i] = searchDatDirectory + "/" + fileArray[i];
				}
				if (resultConcatDatPath.length == 0)
				{
					resultConcatDatPath = QFileDialog.getSaveFileName(this, "Save the resulting file to...?",
								searchDatDirectory, "All Files (*);;Text Files (*.txt)", selectedSaveFilter, DatFileConcat_saveDialogOptions);
				}
				if (resultConcatDatPath.length != 0)
				{
					if(DatFileConcat_ImageProcessorObj.ConcatenateDatFiles(filePathArray,resultConcatDatPath,true))
						Log("CDAT File(" + resultConcatDatPath + ") Created!");
					return true;
				}
			}
		 }
	}
	return false;
}

function DatFileConcat_split(sourceCDatPath,resultDatDirectory)
{
	var dirDirectory;
	var dirFileCount;
	//var fileArray = Array[];
	var filePathArray = Array[];
	var selectedSaveFilter;
	var i;
	
	dirDirectory = new QDir();
	dirDirectory.setPath(resultDatDirectory);
	if (dirDirectory.exists())
	{
		dirSourceCDATFile = new QFile();
		dirSourceCDATFile.setFileName(sourceCDatPath);
		if(dirSourceCDATFile.exists())
		{
			 Log("Writing (*.dat) file(s) to directory: " + resultDatDirectory);
			//SplitCDatFile(const QString &sourceImagePath,const QString &destinationPath, const QString &destPreFileName, const bool bOverwrite)
			var filesProcessed = DatFileConcat_ImageProcessorObj.SplitCDatFile(sourceCDatPath,resultDatDirectory,"result",true);
			if(filesProcessed >= 0)
			{
				Log(filesProcessed + " (*.dat) File(s) in directory (" + resultDatDirectory + ") Created!");
				return filesProcessed;
			}
		}
	 }
	return -1;
}

function DatFileConcat_CleanupScript()
{
	DatFileConcat_ImageProcessorObj = null;
	DatFileConcat_currentScriptLocation = null;
	dirDirectory = null;
	fileArray = null;
	filePathArray = null;
	DatFileConcat_concat = null;
	DatFileConcat_split = null;
	DatFileConcat_CleanupScript = null;
	dirInfo = null;
	Log("\nFinished DatFileConcat_CleanupScript()");
}





Log("\n");
DatFileConcat_searchDatDirectory = QFileDialog.getExistingDirectory(this, "Select a directory containing the source (*.dat) file(s)",DatFileConcat_currentScriptLocation, QFileDialog.Options(DatFileConcat_searchDialogOptions));
dirInfo.setPath(DatFileConcat_searchDatDirectory);
if(dirInfo.exists() == false)
{
	Log("No valid directory choosen...");
	bProceed = false;
	DatFileConcat_CleanupScript();
}
DatFileConcat_resultCDatDirectory = DatFileConcat_searchDatDirectory + "/processed/";
bProceed = DatFileConcat_concat(DatFileConcat_searchDatDirectory,DatFileConcat_resultCDatDirectory + sCDATFileName);
nFilesSplitted = DatFileConcat_split(DatFileConcat_resultCDatDirectory + sCDATFileName,DatFileConcat_resultCDatDirectory);
if (nFilesSplitted > 0)
{
	var tmpFilestr;
	for(var i=0;i<nFilesSplitted;i++)
	{
		tmpFilestr = DatFileConcat_resultCDatDirectory + "result_" + i;
		Log(DatFileConcat_ImageProcessorObj.ConvertDatToPngFile(tmpFilestr + ".dat", tmpFilestr + ".png", true));	
	}
}
DatFileConcat_CleanupScript();
