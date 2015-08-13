//doc-version-brainstimrelease-begin
//Please do not edit the below line(s) manually, see DocumentVersioning.qs (1571011526), version 1.0.0.1
var currentBrainStimRelease = '1.0.0.1';
//doc-version-brainstimrelease-end


///////////////////////////////////////////////////////////
function BrainStimDocuments_Information()
{
	var AuthorCounter = 0;
	this.Doc_FileName_Index = AuthorCounter++;
	this.Doc_Title_Index = AuthorCounter++;
	this.Doc_Version_Index = AuthorCounter++;
	this.Doc_Date_Index = AuthorCounter++;
	this.Doc_Authors_Index = AuthorCounter++;
	///////////////////////////////////////////////////////////////
	this.DocumentAuditing = BrainStimDocuments_CreateArray(5, AuthorCounter); //Default constructor!
	///////////////////////////////////////////////////////////////
	var nCurrentDocIndex = 0;
	this.DocumentAuditing[nCurrentDocIndex][this.Doc_FileName_Index] = 'GettingStartedGuide.html';
	this.DocumentAuditing[nCurrentDocIndex][this.Doc_Title_Index] = 'Getting Started Guide';
	this.DocumentAuditing[nCurrentDocIndex][this.Doc_Version_Index] = '1.0';
	this.DocumentAuditing[nCurrentDocIndex][this.Doc_Date_Index] = 'July 2015';
	this.DocumentAuditing[nCurrentDocIndex][this.Doc_Authors_Index] = 'Sven Gijsen';
	nCurrentDocIndex++;
	
	this.DocumentAuditing[nCurrentDocIndex++] = ['PreBuildDevelopmentSetup.html','Pre-Build Development Setup','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['ExperimentManagerPlugin.html','Experiment Manager Plug-in','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['ExtensionPluginTemplate.html','Extension Plug-in Template','1.0','July 2015','Sven Gijsen'];	
	this.DocumentAuditing[nCurrentDocIndex++] = ['BrainStim_QMLExtensions.html','BrainStim QML Extensions Plug-in','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['Development.html','BrainStim Development','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['ExamplesAndTutorials.html','Examples and Tutorials','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['RetinoTopicMappingOutput.html','Retinotopic Mapping output files Tutorial','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['SettingsDialog.html','Settings Dialog','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['QtLibraryScriptBindings.html','Qt Library Script Bindings','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['FileTypeAssociations.html','Automatic File Type Associations','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['CustomUserLogins.html','Custom User Logins','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['FunctionsAndIncludes.html','Functions and Includes','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['GarbageCollection.html','Garbage Collection','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['CommandLineOptions.html','Command-Line Options','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['InternalNetworkInterface.html','Internal Network Interface','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['SignalSlotConnections.html','Signal-Slot Connections','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['DocumentTypes.html','Document Types','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['CustomMenus.html','Custom Menu\'s','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['MainLogFile.html','Main Log-file','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['UserInterface.html','Default User-Interface','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['ScriptDebugging.html','Debugging scripts','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['PluginFrameWork.html','Plugin Framework','1.0','July 2015','Sven Gijsen'];
	this.DocumentAuditing[nCurrentDocIndex++] = ['DefaultPlugins.html','Default Plugins','1.0','July 2015','Sven Gijsen'];
	
	
	
	
	
}

BrainStimDocuments_Information.prototype.GetStrippedFileName = function (strFileUrl)//var strFileUrl=window.location.pathname;
{
	return strFileUrl.replace(/^.*[\\\/]/, '');
}

function BrainStimDocuments_Cleanup()
{
	BrainStimDocuments_CreateArray = null;
	BrainStimDocuments_Information.prototype.GetStrippedFileName = null;
	BrainStimDocuments_Information.prototype.GetDocumentIndex = null;
	BrainStimDocuments_Information.prototype.GetDocumentTitle = null;
	BrainStimDocuments_Information.prototype.GetDocumentName = null;
	BrainStimDocuments_Information.prototype = null;
	BrainStimDocuments_Information = null;
	BrainStimDocuments_Cleanup = null;
}

function BrainStimDocuments_CreateArray(length) 
{
	//BrainStimDocuments_CreateArray();     // [] or new Array()
	//BrainStimDocuments_CreateArray(2);    // new Array(2)
	//BrainStimDocuments_CreateArray(3, 2); // [new Array(2),
                    	 //  new Array(2),
                    	 //  new Array(2)]
    var a = new Array(length || 0);
    if (arguments.length > 1) 
	{
        var args = Array.prototype.slice.call(arguments, 1);
        for (var i = 0; i < length; i++) 
		{
            a[i] = BrainStimDocuments_CreateArray.apply(this, args);
        }
    }
    return a;
}

BrainStimDocuments_Information.prototype.GetDocumentIndex = function (strFileUrl)//var strFileUrl=window.location.pathname;
{
	//alert('GetDocumentIndex ' + strFileUrl);
	if ((strFileUrl===undefined) || (strFileUrl==''))
		return -1;
	var filename = this.GetStrippedFileName(strFileUrl)
	var nDocuments = this.DocumentAuditing.length;
	for (var i=0;i<nDocuments;i++) 
	{
		if(this.DocumentAuditing[i][this.Doc_FileName_Index] == filename)
		{
			//alert('found it! -> ' + filename);
			return i;
		}
	}
	//alert('not found it... -> ' + filename);
	return -1;
}

BrainStimDocuments_Information.prototype.GetDocumentName = function (strFileUrl)//without version information
{
	//alert('GetDocumentTitle');
	var nDocIndex = this.GetDocumentIndex(strFileUrl);
	if(nDocIndex >= 0)
		return this.DocumentAuditing[nDocIndex][this.Doc_Title_Index];
	return this.GetStrippedFileName(strFileUrl);
}

BrainStimDocuments_Information.prototype.GetDocumentTitle = function (strFileUrl)
{
	//alert('GetDocumentTitle');
	var nDocIndex = this.GetDocumentIndex(strFileUrl);
	if(nDocIndex >= 0)
		return this.DocumentAuditing[nDocIndex][this.Doc_Title_Index] + " v" + this.DocumentAuditing[nDocIndex][this.Doc_Version_Index];
	return this.GetStrippedFileName(strFileUrl);
}

BrainStimDocuments_Information.prototype.GetDocumentVersion = function (strFileUrl)
{
	//alert('GetDocumentTitle');
	var nDocIndex = this.GetDocumentIndex(strFileUrl);
	if(nDocIndex >= 0)
		return this.DocumentAuditing[nDocIndex][this.Doc_Version_Index];
	return "?"
}

BrainStimDocuments_Information.prototype.GetDocumentAuthors = function (strFileUrl)
{
	//alert('GetDocumentTitle');
	var nDocIndex = this.GetDocumentIndex(strFileUrl);
	if(nDocIndex >= 0)
		return this.DocumentAuditing[nDocIndex][this.Doc_Authors_Index];
	return "?"
}

BrainStimDocuments_Information.prototype.GetDocumentDate = function (strFileUrl)
{
	//alert('GetDocumentTitle');
	var nDocIndex = this.GetDocumentIndex(strFileUrl);
	if(nDocIndex >= 0)
		return this.DocumentAuditing[nDocIndex][this.Doc_Date_Index];
	return "?"
}

//BrainStimDocuments_Cleanup();
