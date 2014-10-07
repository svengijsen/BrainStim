var sQMLSourceFilePath = "";
var sQMLProjectFilePath = "";
var sProjectName = "";

function CreateQMLProject(Project_Name,QML_ProjectFilePath,QML_SourceFilePath)
{
	var qmlFile = new QFile(QML_SourceFilePath);
	if(qmlFile.exists() == false)
		return false;
	qmlFile = new QFile(QML_ProjectFilePath);
	var bFileExists = qmlFile.exists();
	
	if (qmlFile.open(QIODevice.OpenMode(QIODevice.WriteOnly) || QIODevice.OpenMode(QIODevice.Text)) == false)
		return false;
	
	var sFileInfo = new QFileInfo(QML_SourceFilePath);
	var sData = "/* File generated by " + BrainStim.invokeJavaScriptConfigurationFile("BrainStimInfo.GetMainAppInternalName()") + " version (" + BrainStim.invokeJavaScriptConfigurationFile("BrainStimInfo.GetCurrentRelease()")+ ") */\n\n";
	sData = sData + "import QmlProject 1.1\n\n";
	sData = sData + "Project {\n";
	sData = sData + "\tmainFile: \"./" + sFileInfo.fileName() + "\"\n\n";
	sData = sData + "\t/* Include .qml, .js, and image files from current directory and subdirectories */\n";
	sData = sData + "\tQmlFiles {\n";
	sData = sData + "\t\tdirectory: \".\"\n";
	sData = sData + "\t}\n";
	sData = sData + "\tJavaScriptFiles {\n";
	sData = sData + "\t\tdirectory: \".\"\n";
	sData = sData + "\t}\n";
	sData = sData + "\tImageFiles {\n";
	sData = sData + "\t\tdirectory: \".\"\n";
	sData = sData + "\t}\n";
	sData = sData + "\t/* List of plugin directories passed to QML runtime */\n";
	sData = sData + "\timportPaths: [ \"" + BrainStim.getQMLPluginPath() + "\" ]\n";
	sData = sData + "}\n";
	
	//Log(sData);
	var outData = new QByteArray(sData);
	Log("Number of bytes written: " + qmlFile.write(outData));
	qmlFile.close();
	sFileInfo = null;
	outData = null;
	qmlFile = null;
	return true;
}

function getItem(title,question,items,current)
{
	var item = QInputDialog.getItem(this, title, question, items, current, false, Qt.WindowFlags(0));
	return item;
}

function getText(title, question, current)
{
	var text = QInputDialog.getText(this, title, question, QLineEdit.Normal, current, Qt.WindowFlags(0));
	return text;
}

function getOpenFileName(title, current, filter)
{
	var options = 0;
	//if (!this.useNative.checked)
		//options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	var fileName = QFileDialog.getOpenFileName(this, title, current, filter /*"All Files (*);;Text Files (*.txt)"*/, selectedFilter, QFileDialog.Options(options));
	return fileName;
}

function getSaveFileName(title, current, filter)
{
	var options = 0;
	//if (!this.useNative.checked)
		//options |= QFileDialog.DontUseNativeDialog;
	var selectedFilter;
	var fileName = QFileDialog.getSaveFileName(this, title, current, filter /*"All Files (*);;Text Files (*.txt)"*/, selectedFilter, QFileDialog.Options(options));
	return fileName;
}

//var itemArray = Array["item1", "item2", "item3", "item4"];
//Log(getItem("Select the item", "The items to choose from are:", itemArray, 2));
//Log(getText("Give some text", "The text is:", "enter here..."));
//Log(getSaveFileName("Title", "", "All Files (*);;Text Files (*.txt)"));
Log("\n")


sQMLSourceFilePath = getOpenFileName("Enter the path to the QML input file", "", "QML Files (*.qml)");
if(sQMLSourceFilePath != null && sQMLSourceFilePath.length != 0)
{
	Log("\tQML Input File: " + sQMLSourceFilePath);
	sQMLProjectFilePath = getSaveFileName("Enter the path to the QML Project file", "", "QML Project Files (*.qmlproject)");
	if(sQMLProjectFilePath != null && sQMLProjectFilePath.length != 0)
	{
		Log("\tQML Project File: " + sQMLProjectFilePath);
		//sProjectName = getText("Enter a valid project name", "Project Name (no spaces):", "");
		//if(sProjectName != null && sProjectName.length != 0)
		//{
		//	Log("\tProject name: " + sProjectName);
			if(CreateQMLProject(sProjectName,sQMLProjectFilePath,sQMLSourceFilePath))
				Log("\t*QML project successfully created!*");
			else
				Log("\t*Failed creating the QML project!*");
		//}
		//else
		//{
		//	Log("* No valid project name entered.")
		//}
	}
	else
	{
		Log("* No valid QML Project file defined.")
	}
}
else
{
	Log("* No valid QML input file selected.")
}
	
Log("\n");
getItem = null;
getText = null;
getSaveFileName = null;
getOpenFileName = null;
CreateQMLProject = null;