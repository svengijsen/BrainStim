var CreateMeanImage_ImageProcessorObj = new ImageProcessor();
var CreateMeanImage_currentScriptLocation = BrainStim.getActiveDocumentFileLocation();
var CreateMeanImage_searchDialogOptions = QFileDialog.DontResolveSymlinks | QFileDialog.ShowDirsOnly | QFileDialog.DontUseNativeDialog;
var CreateMeanImage_saveDialogOptions = QFileDialog.DontUseNativeDialog;//0
var CreateMeanImage_searchDirectory;

function CreateMeanImage_process(searchDirectory,resultImagePath)
{
	var dirDirectory;
	var dirFileCount;
	var fileArray = new Array();
	var filePathArray = new Array();
	var selectedSaveFilter;
	var i;
	
	if (searchDirectory.length != 0)
	{
		dirDirectory = new QDir();
		dirDirectory.setPath(searchDirectory);
		 if (dirDirectory.exists())
		 {
			searchDirectory = dirDirectory.absolutePath();
			Log("Choosen directory: " + searchDirectory);
			dirDirectory.setFilter(QDir.Files);// | QDir.Hidden | QDir.NoSymLinks);
			dirDirectory.setNameFilters(Array["*.png"]);
			dirDirectory.setSorting(QDir.Name);//::Size | QDir::Reversed);
			dirFileCount = dirDirectory.count();
			if(dirFileCount>0)
			{
				fileArray = dirDirectory.entryList();
				Log("The directory contains " + dirFileCount + " file(s): ");
				for(i=0;i<fileArray.length;i++)
				{
					Log("\t- " + fileArray[i]);
					filePathArray[i] = searchDirectory + "/" + fileArray[i];
				}
				if (resultImagePath.length == 0)
				{
					resultImagePath = QFileDialog.getSaveFileName(this, "Save the resulting file to...?",
								searchDirectory, "All Files (*);;Text Files (*.txt)", selectedSaveFilter, CreateMeanImage_saveDialogOptions);
				}
				if (resultImagePath.length != 0)
				{
					if(CreateMeanImage_ImageProcessorObj.CreateMeanImageFromPngFiles(filePathArray,resultImagePath,true))
						Log("Mean Image(" + resultImagePath + ") Created!");
					return true;
				}
			}
		 }
	}
	return false;
}

function CreateMeanImage_CleanupScript()
{
	CreateMeanImage_ImageProcessorObj = null;
	CreateMeanImage_currentScriptLocation = null;
	dirDirectory = null;
	fileArray = null;
	filePathArray = null;
	CreateMeanImage_process = null;
	CreateMeanImage_CleanupScript = null;
	Log("Finished CreateMeanImage_CleanupScript()");
}

//CreateMeanImage_searchDirectory = QFileDialog.getExistingDirectory(this, "Select a directory containing the source file(s)",
//												CreateMeanImage_currentScriptLocation, QFileDialog.Options(CreateMeanImage_searchDialogOptions));
//CreateMeanImage_process(CreateMeanImage_searchDirectory,"");
//CreateMeanImage_CleanupScript();
