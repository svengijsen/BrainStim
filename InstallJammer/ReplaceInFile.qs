function ReplaceInFile_Cleanup()
{
	ReplaceInFile_ReplaceInFiles = null;
	ReplaceInFile_Cleanup = null;
	//ReplaceInFile_DeleteFile = null;
	ReplaceInFile_CreateArray = null;
	CreateNewContent = null;
	QByteArray.prototype.ReplaceInFile_toString = null;
	Log('ReplaceInFile_Cleanup ready...\n');
}

function ReplaceInFile_ReplaceInFiles(strTemplate, strDestination, changeSetArr)
{
	//if(dstFile.exists())
	//dstFile.remove();//Copy Doesn't Overwrite!
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
	//Log(strDestination);
	Log("output file open result: " + outputFile.open(QIODevice.OpenMode(QIODevice.WriteOnly) || QIODevice.OpenMode(QIODevice.Text)));
	var outStream = new QTextStream(outputFile);
	inData = CreateNewContent(inData,changeSetArr);
	var outData = new QByteArray(inData);// or 'text'
	//Log(outData.ReplaceInFile_toString());
	Log("Number of bytes written: " + outputFile.write(outData));
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

//function ReplaceInFile_DeleteFile(strFile)
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

QByteArray.prototype.ReplaceInFile_toString = function()
{
	ts = new QTextStream( this, QIODevice.ReadOnly );
	return ts.readAll();
}

function ReplaceInFile_CreateArray(length) 
{
	//ReplaceInFile_CreateArray();     	// [] or new Array()
	//ReplaceInFile_CreateArray(2);    	// new Array(2)
	//ReplaceInFile_CreateArray(3, 2); 	// [new Array(2),
					//  new Array(2),
					//  new Array(2)]
	var a = new Array(length || 0);
	if (arguments.length > 1) 
	{
		var args = Array.prototype.slice.call(arguments, 1);
		for (var i = 0; i < length; i++) 
		{
			a[i] = ReplaceInFile_CreateArray.apply(this, args);
		}
	}
	return a;
}

function CreateNewContent(strTemplate,changeSetArray)
{
	var nNumberOfChanges = changeSet.length;
	var regex;
	if(nNumberOfChanges > 0)
	{
		for(var i=0;i<nNumberOfChanges;i++)
		{
			regex = new RegExp(changeSet[i][0] , "g");//Global, replace all
			strTemplate = strTemplate.replace(regex , changeSet[i][1]);
			Log("Replaced: \"" + changeSet[i][0] + "\" with \"" + changeSet[i][1] + "\"");
		}
	}
	return strTemplate;
}






//var fileSource = "PluginTemplate.cfg";
//var fileDest = "test.txt";
//var preFix = "-*/*-";
//var postFix = "-*/*-";
//
//var changeSet = ReplaceInFile_CreateArray(7,2); //See template document for parameter descriptions!!
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
//ReplaceInFile_ReplaceInFiles(fileSource,fileDest,changeSet);
//ReplaceInFile_DeleteFile(fileDest);
//ReplaceInFile_Cleanup();




