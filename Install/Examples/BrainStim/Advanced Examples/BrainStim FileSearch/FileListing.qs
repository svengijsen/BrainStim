//Configuration begin//////////////////////////
var sRootDirectory = "";
var sAllowedFileExtensions = ["*.jpg", "*.png"];
var sIncludeDir = true;
var sParseSubDirs = true;
//Configuration end////////////////////////////


//Functions///////////////////////////////////
function parseDirectory(sDirToParse)
{
	var tmpDir = new QDir();
	tmpDir.setPath(sDirToParse);
	 if (tmpDir.exists())
	 {
		//BrainStim.write2OutputWindow("- Current directory: " + sDirToParse);
		tmpDir.setFilter(QDir.Files);// | QDir.Hidden | QDir.NoSymLinks);
		tmpDir.setNameFilters(sAllowedFileExtensions);
		tmpDir.setSorting(QDir.Name);//::Size | QDir::Reversed);
		var _nNumberOfFoundFiles = tmpDir.count();
		//BrainStim.write2OutputWindow("- Number of Files found in directory: " + _nNumberOfFoundFiles + "\n");
		if(_nNumberOfFoundFiles>0)
		{
			var _arrFoundFileList = tmpDir.entryList();
			var nProcessedItems = 0;
			for(i=0;i<_arrFoundFileList.length;i++)
			{
				if(sIncludeDir)
					BrainStim.write2OutputWindow(sDirToParse + _arrFoundFileList[i]);
				else
					BrainStim.write2OutputWindow(_arrFoundFileList[i]);
			}
		}
		
		if(sParseSubDirs)
		{
			var it = new QDirIterator(sDirToParse);
			while (it.hasNext()) 
			{
				var sSubDir = it.next();
				if(sSubDir.indexOf(".", sSubDir.length - 1) == -1)//No . and ..
				{
					parseDirectory(sSubDir + "/");
				}
			}
		}
	}
}

//Main Routine//////////////////////
sRootDirectory = QFileDialog.getExistingDirectory(this, "Select a root directory", BrainStim.getActiveDocumentFileLocation() + "/", QFileDialog.Options(QFileDialog.DontResolveSymlinks | QFileDialog.ShowDirsOnly | QFileDialog.DontUseNativeDialog));
BrainStim.write2OutputWindow("\n");
if(sRootDirectory != "")
	parseDirectory(sRootDirectory);
else
	BrainStim.write2OutputWindow("No valid directory choosen...");
BrainStim.write2OutputWindow("\n");
parseDirectory = null;
tmpDir = null;