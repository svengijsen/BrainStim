function GDCF_Cleanup()
{
	GDCF_ReplaceInFiles = null;
	GDCF_Cleanup = null;
	//GDCF_DeleteFile = null;
	GDCF_CreateArray = null;
	CreateNewContent = null;
	QByteArray.prototype.GDCF_toString = null;
	Log('GDCF_Cleanup ready...\n');
}

function GDCF_ReplaceInFiles(strTemplate, strDestination, changeSetArr)
{
	//if(dstFile.exists())
	//dstFile.remove();//Copy Doesn't Overwrite!
	var templateFile = new QFile(strTemplate);
	templateFile.open(QIODevice.OpenMode(QIODevice.ReadOnly) || QIODevice.OpenMode(QIODevice.Text));
	var inStream = new QTextStream(templateFile);
	var inData = new String();
	var dDestDir = new QDir(); 
	var fiDestFileInfo = new QFileInfo();
	//inData = inStream.readLine();
	inData = inStream.readAll();
	//Log(inData);
	templateFile.close();
	///////////////////////////////////////////////////////////////////////////////////////
	fiDestFileInfo.setFile(strDestination);
	dDestDir = fiDestFileInfo.absoluteDir();
	if(dDestDir.exists()==false)
	{
		if(dDestDir.mkpath(dDestDir.path())==false)
			Log("<font color=\"red\">Could not create destination path: " + dDestDir.path() + "</font>");
	}	
	var outputFile = new QFile(strDestination);
	if(outputFile.open(QIODevice.OpenMode(QIODevice.WriteOnly) || QIODevice.OpenMode(QIODevice.Text))==false)
	{
		Log("<font color=\"red\">Could not open output file!</font>");
		return false;
	}
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
	dDestDir = null;
	fiDestFileInfo = null;
	return true;	
}

//function GDCF_DeleteFile(strFile)
//{
//	var remFile = new QFile(strFile);
//	if(remFile.remove())
//	{
//		Log("File removed.");
//		return true;
//	}
//	Log("Could not removed the file.");
//	return false;		
//}

QByteArray.prototype.GDCF_toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function GDCF_CreateArray(length) 
{
	//GDCF_CreateArray();     	// [] or new Array()
	//GDCF_CreateArray(2);    	// new Array(2)
	//GDCF_CreateArray(3, 2); 	// [new Array(2),
					//  new Array(2),
					//  new Array(2)]
	var a = new Array(length || 0);
	if (arguments.length > 1) 
	{
		var args = Array.prototype.slice.call(arguments, 1);
		for (var i = 0; i < length; i++) 
		{
			a[i] = GDCF_CreateArray.apply(this, args);
		}
	}
	return a;
}

function CreateNewContent(strTemplate,changeSetArray)
{
	var nNumberOfChanges = changeSet.length;
	if(nNumberOfChanges > 0)
	{
		for(var i=0;i<nNumberOfChanges;i++)
		{
			strTemplate = strTemplate.replace(changeSet[i][0] , changeSet[i][1]);
		}
	}
	return strTemplate;
}






//var fileSource = "PluginTemplate.cfg";
//var fileDest = "test.txt";
//var preFix = "-*/*-";
//var postFix = "-*/*-";
//
//var changeSet = GDCF_CreateArray(7,2); //See template document for parameter descriptions!!
//changeSet[0][0] = preFix + "PROJECT_NAME" + postFix;
//changeSet[0][1] = "\"ExperimentManager script class\"";
//changeSet[1][0] = preFix + "PROJECT_NUMBER" + postFix;
//changeSet[1][1] = "\"2.0.0.2\"";
//changeSet[2][0] = preFix + "PROJECT_BRIEF" + postFix;
//changeSet[2][1] = "\"The Experiment Manager script class definition.\"";
//changeSet[3][0] = preFix + "PROJECT_LOGO" + postFix;
//changeSet[3][1] = "";//No quotes for empty!
//changeSet[4][0] = preFix + "OUTPUT_DIRECTORY" + postFix;
//changeSet[4][1] = "\"../References/Script/ExperimentManagerPlugin\"";
//changeSet[5][0] = preFix + "INPUT" + postFix;
//changeSet[5][1] = "../../BrainStim/Source/Plugins/ExperimentManager/ExperimentManager.cpp \\\n" +
//                          "../../BrainStim/Source/Plugins/ExperimentManager/ExperimentManager.h \\\n" +
//                          "../../BrainStim/Source/Plugins/ExperimentManager/Global.h \\\n";
//changeSet[6][0] = preFix + "QHP_NAMESPACE" + postFix;
//changeSet[6][1] = "BrainStim.doxygen.Project";			  
//
//GDCF_ReplaceInFiles(fileSource,fileDest,changeSet);
//GDCF_DeleteFile(fileDest);
//GDCF_Cleanup();




