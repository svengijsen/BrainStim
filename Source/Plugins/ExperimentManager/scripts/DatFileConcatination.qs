var DatFileConcat_ImageProcessorObj = new ImageProcessor();
var DatFileConcat_currentScriptLocation = BrainStim.getActiveDocumentFileLocation();
var DatFileConcat_searchDialogOptions = QFileDialog.DontResolveSymlinks | QFileDialog.ShowDirsOnly | QFileDialog.DontUseNativeDialog;
var DatFileConcat_saveDialogOptions = QFileDialog.DontUseNativeDialog;//0
var DatFileConcat_DatDirectory;
var DatFileConcat_CDatDirectory;
var DatFileConcat_CDatFile;
var fileArray = new Array();

function DatFileConcat_concat(searchDatDirectory,resultConcatDatPath)
{
	var dirDirectory;
	var dirFileCount;
	var filePathArray = new Array();
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
	//var fileArray = new Array();
	var filePathArray = new Array();
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
	Log("\nFinished DatFileConcat_CleanupScript()");
}

Log("\n");
if(true)
{
	 //QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
         //                                        "/home",
         //                                       tr("Images (*.png *.xpm *.jpg)"));
	
	DatFileConcat_CDatFile = QFileDialog.getOpenFileName(this, "Select the (*.cdat) file to split", DatFileConcat_currentScriptLocation, "CDAT (*.cdat)");
	if(DatFileConcat_CDatFile != "")
	{
		var DatFileConcat_CDatFile_Info = new QFileInfo(DatFileConcat_CDatFile);
		DatFileConcat_CDatDirectory = DatFileConcat_CDatFile_Info.canonicalPath();
		//Log(DatFileConcat_CDatDirectory);
		DatFileConcat_DatDirectory = DatFileConcat_CDatDirectory + "/splitted/";
		var DatFileConcat_DatDir_Object = new QDir(DatFileConcat_DatDirectory);
		var bDirExists = DatFileConcat_DatDir_Object.exists();
		if(bDirExists == false)
			bDirExists = DatFileConcat_DatDir_Object.mkpath(DatFileConcat_DatDirectory);
		if(bDirExists)
		{
			//DatFileConcat_DatDirectory = QFileDialog.getExistingDirectory(this, "Select a directory containing the source (*.dat) file(s)",DatFileConcat_currentScriptLocation, QFileDialog.Options(DatFileConcat_searchDialogOptions));
			//DatFileConcat_concat(DatFileConcat_DatDirectory,DatFileConcat_DatDirectory + "/result.cdat");
			//DatFileConcat_CDatDirectory = DatFileConcat_DatDirectory + "/";
			var FilesSplitted = DatFileConcat_split(DatFileConcat_CDatFile,DatFileConcat_DatDirectory);
			//var FilesSplitted = DatFileConcat_split(DatFileConcat_CDatDirectory,DatFileConcat_CDatDirectory + "/splitted");
			if (FilesSplitted > 0)
			{
				var tmpFilestr;
				var bConversionSucceeded = true;
				for(var i=0;i<FilesSplitted;i++)
				{
					tmpFilestr = DatFileConcat_DatDirectory + "result_" + i;
					bConversionSucceeded = DatFileConcat_ImageProcessorObj.ConvertDatToPngFile(tmpFilestr + ".dat", tmpFilestr + ".png", true);
					if(bConversionSucceeded == false)
					{
						Log("Conversion failed!")
						break;
					}
				}
				if(bConversionSucceeded)
				{
					DatFileConcat_concat(DatFileConcat_DatDirectory,DatFileConcat_DatDirectory + "/concat.cdat");
				}
			}
		}
	}
}
else
{
	DatFileConcat_DatDirectory = "<enter path here...>";
	var FilesSplitted = DatFileConcat_split(DatFileConcat_DatDirectory + "1_masks.cdat",DatFileConcat_DatDirectory);
	if (FilesSplitted > 0)
	{
		var tmpFilestr;
		for(var i=0;i<FilesSplitted;i++)
		{
			tmpFilestr = DatFileConcat_DatDirectory + "result_" + i;
			Log(DatFileConcat_ImageProcessorObj.ConvertDatToPngFile(tmpFilestr + ".dat", tmpFilestr + ".png", true));	
		}
	}	
}	
DatFileConcat_CleanupScript();
