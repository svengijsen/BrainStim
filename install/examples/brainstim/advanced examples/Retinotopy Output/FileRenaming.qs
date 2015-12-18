//Some global vars
var _FileRenaming_sAllowedFileExtensions = ["*_*_*_*_*.png"];
var _FileRenaming_sNewFilePrefix = "Renamed_";//Creates a auto-numbered file like "Renamed_324.png"
var _FileRenaming_sIncludeDir = true;
var _FileRenaming_sParseSubDirs = true;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

function _FileRenaming_parseDirectory(sDirToParse)
{
	var sParseFoundFiles = [];
	var tmpDir = new QDir();
	tmpDir.setPath(sDirToParse);
	 if (tmpDir.exists())
	 {
		//BrainStim.write2OutputWindow("- Current directory: " + sDirToParse);
		tmpDir.setFilter(QDir.Files);// | QDir.Hidden | QDir.NoSymLinks);
		tmpDir.setNameFilters(_FileRenaming_sAllowedFileExtensions);
		tmpDir.setSorting(QDir.Name);//::Size | QDir::Reversed);
		var _nNumberOfFoundFiles = tmpDir.count();
		//BrainStim.write2OutputWindow("- Number of Files found in directory: " + _nNumberOfFoundFiles + "\n");
		if(_nNumberOfFoundFiles>0)
		{
			var _arrFoundFileList = tmpDir.entryList();
			var nProcessedItems = 0;
			for(i=0;i<_arrFoundFileList.length;i++)
			{
				if(_FileRenaming_sIncludeDir)
				{
					sParseFoundFiles.push(sDirToParse + _arrFoundFileList[i]);
				}
				else
				{
					sParseFoundFiles.push(_arrFoundFileList[i]);
				}
				//BrainStim.write2OutputWindow("Found :" + sParseFoundFiles[sParseFoundFiles.length-1]);
			}
		}
		
		if(_FileRenaming_sParseSubDirs)
		{
			var it = new QDirIterator(sDirToParse);
			while (it.hasNext()) 
			{
				var sSubDir = it.next();
				if(sSubDir.indexOf(".", sSubDir.length - 1) == -1)//No . and ..
				{
					sParseFoundFiles = sParseFoundFiles.concat(_FileRenaming_parseDirectory(sSubDir + "/"));//Append the array
				}
			}
		}
	}
	return sParseFoundFiles;
}

function _FileRenaming_renameFiles(arrFilesToRename)
{
	if(arrFilesToRename.length > 0)
	{
		var fFile = new QFile();
		var fiFileInfo = new QFileInfo();
		var sNewFileName = "";
		for(var nIndex=0;nIndex<arrFilesToRename.length;nIndex++)
		{
			fFile.setFileName(arrFilesToRename[nIndex]);
			fiFileInfo.setFile(arrFilesToRename[nIndex]);
			if(fFile.exists)
			{
				//BrainStim.write2OutputWindow("canonicalPath: " + fiFileInfo.canonicalPath() + "/" + ", fileName: " + fiFileInfo.fileName());
				sNewFileName = fiFileInfo.fileName().split('_')[4];
				sNewFileName = fiFileInfo.canonicalPath() + "/" + _FileRenaming_sNewFilePrefix + sNewFileName;
				if(fFile.rename(sNewFileName))
					BrainStim.write2OutputWindow("Renamed to: " + sNewFileName);
				else
					return false;
			}
		}
	}
	return true;
}

function _FileRenaming_cleanupScript()
{
	_FileRenaming_renameFiles = null;
	_FileRenaming_parseDirectory = null;
	_FileRenaming_cleanupScript = null;
}


////////////////////////Example usage/////////////////////////////
//var _testArray = [];
//_testArray = _FileRenaming_parseDirectory("E:/Projects/Experiments/BrainStim/Judith.Eck/pRF_Mapping_2/outputfiles/test/");
//BrainStim.write2OutputWindow("Found Files(total amount): " + _testArray.length);
//if(_FileRenaming_renameFiles(_testArray))
//	BrainStim.write2OutputWindow("Successfull Operation!");
//else
//	BrainStim.write2OutputWindow("Something failed!");
//BrainStim.write2OutputWindow("\n");
//_FileRenaming_cleanupScript();
