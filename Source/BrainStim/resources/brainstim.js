// JavaScript Document
//
//var BrainStimInfo = new BrainStim_Information();
//var sComponentName = "FirebirdClientPlugin";			
//Log(BrainStimInfo.GetLatestComponentReleaseByName(sComponentName));

function BrainStim_Cleanup()
{
	//window.BrainStimInfo = null;
	BrainStim_CreateArray = null;
}

function BrainStim_CreateArray(length) 
{
	//BrainStim_CreateArray();     // [] or new Array()
	//BrainStim_CreateArray(2);    // new Array(2)
	//BrainStim_CreateArray(3, 2); // [new Array(2),
                    	 //  new Array(2),
                    	 //  new Array(2)]
    var a = new Array(length || 0);
    if (arguments.length > 1) 
	{
        var args = Array.prototype.slice.call(arguments, 1);
        for (var i = 0; i < length; i++) 
		{
            a[i] = BrainStim_CreateArray.apply(this, args);
        }
    }
    return a;
}

///////////////////////////////////////////////////////////
//window.BrainStimInfo = new BrainStim_Information();
function BrainStim_Information()
{
	//alert('BrainStim constructor');
	this.BrainStimReleases = [['1.0.0.1', '5.3.2', '1.0.0.1']];//, ['Exml', 'Qt', 'Product']];
	var PropCounter = 0;
	var AuthorCounter = 0;
	var ComponentCounter = 0;
	var tmpArray;
	var nNumberOfComponentChanges = 0;
	
	this.Doc_FileName_Index = AuthorCounter++;
	this.Doc_Title_Index = AuthorCounter++;
	this.Doc_Version_Index = AuthorCounter++;
	this.Doc_Date_Index = AuthorCounter++;
	this.Doc_Authors_Index = AuthorCounter++;
	
	this.Name_Index = PropCounter++;
	this.Version_Index = PropCounter++;
	this.ProductVersion_Index = PropCounter++;
	this._BrainStimVersion_Index = PropCounter++;
	this.EXML_Index = PropCounter++;
	this.DevInterface_Index = PropCounter++;
	this.ExtInterface_Index = PropCounter++;
	
	var ExperimentManagerPlugin_Comp_Index = ComponentCounter++;
	var ParallelPortPlugin_Comp_Index = ComponentCounter++;
	var KeyBoardPlugin_Comp_Index = ComponentCounter++;
	var _extensionname_plugin_Comp_Index = ComponentCounter++;
	var BrainStim_QMLExtensions_Comp_Index = ComponentCounter++;
	
	this.componentVersioning = BrainStim_CreateArray(1,ComponentCounter,PropCounter);//Default constructor!
	//BrainStim version (1.0.0.1)
	///////////////////////////////////////////////////////////////
	this.componentVersioning[0][ExperimentManagerPlugin_Comp_Index][this.Name_Index] = 'ExperimentManagerPlugin';
	this.componentVersioning[0][ExperimentManagerPlugin_Comp_Index][this.Version_Index] = '1.0.0.1';
	this.componentVersioning[0][ExperimentManagerPlugin_Comp_Index][this.ProductVersion_Index] = '1.0.0.1';
	this.componentVersioning[0][ExperimentManagerPlugin_Comp_Index][this._BrainStimVersion_Index] = 0; //-->1.0.0.1
	this.componentVersioning[0][ExperimentManagerPlugin_Comp_Index][this.EXML_Index] = '1.0.0.1';
	this.componentVersioning[0][ExperimentManagerPlugin_Comp_Index][this.DevInterface_Index] = '1.0';
	this.componentVersioning[0][ExperimentManagerPlugin_Comp_Index][this.ExtInterface_Index] = '1.0';
	///////////////////////////////////////////////////////////////
	this.componentVersioning[0][ParallelPortPlugin_Comp_Index][this.Name_Index] = 'ParallelPortPlugin';
	this.componentVersioning[0][ParallelPortPlugin_Comp_Index][this.Version_Index] = '1.0.0.1';
	this.componentVersioning[0][ParallelPortPlugin_Comp_Index][this.ProductVersion_Index] = '1.0.0.1';
	this.componentVersioning[0][ParallelPortPlugin_Comp_Index][this._BrainStimVersion_Index] = 0; 
	this.componentVersioning[0][ParallelPortPlugin_Comp_Index][this.EXML_Index] = 'x';
	this.componentVersioning[0][ParallelPortPlugin_Comp_Index][this.DevInterface_Index] = '1.0';
	this.componentVersioning[0][ParallelPortPlugin_Comp_Index][this.ExtInterface_Index] = '1.0';
	///////////////////////////////////////////////////////////////
	this.componentVersioning[0][KeyBoardPlugin_Comp_Index][this.Name_Index] = 'KeyBoardPlugin';
	this.componentVersioning[0][KeyBoardPlugin_Comp_Index][this.Version_Index] = '1.0.0.1';
	this.componentVersioning[0][KeyBoardPlugin_Comp_Index][this.ProductVersion_Index] = '1.0.0.1';
	this.componentVersioning[0][KeyBoardPlugin_Comp_Index][this._BrainStimVersion_Index] = 0;
	this.componentVersioning[0][KeyBoardPlugin_Comp_Index][this.EXML_Index] = 'x';
	this.componentVersioning[0][KeyBoardPlugin_Comp_Index][this.DevInterface_Index] = '1.0';
	this.componentVersioning[0][KeyBoardPlugin_Comp_Index][this.ExtInterface_Index] = '1.0';
	///////////////////////////////////////////////////////////////
	this.componentVersioning[0][BrainStim_QMLExtensions_Comp_Index][this.Name_Index] = 'BrainStim_QMLExtensions';
	this.componentVersioning[0][BrainStim_QMLExtensions_Comp_Index][this.Version_Index] = '1.0';
	this.componentVersioning[0][BrainStim_QMLExtensions_Comp_Index][this.ProductVersion_Index] = 'x';
	this.componentVersioning[0][BrainStim_QMLExtensions_Comp_Index][this._BrainStimVersion_Index] = 0;
	this.componentVersioning[0][BrainStim_QMLExtensions_Comp_Index][this.EXML_Index] = 'x';
	this.componentVersioning[0][BrainStim_QMLExtensions_Comp_Index][this.DevInterface_Index] = 'x';
	this.componentVersioning[0][BrainStim_QMLExtensions_Comp_Index][this.ExtInterface_Index] = 'x';
	///////////////////////////////////////////////////////////////
	this.DocumentAuditing = BrainStim_CreateArray(8, AuthorCounter); //Default constructor!
	///////////////////////////////////////////////////////////////
	this.DocumentAuditing[0][this.Doc_FileName_Index] = 'GettingStartedGuide.html';
	this.DocumentAuditing[0][this.Doc_Title_Index] = 'Getting Started Guide';
	this.DocumentAuditing[0][this.Doc_Version_Index] = '1.0.1.0';
	this.DocumentAuditing[0][this.Doc_Date_Index] = 'Oktober 2014';
	this.DocumentAuditing[0][this.Doc_Authors_Index] = 'Sven Gijsen';
	
	this.DocumentAuditing[1] = ['PreBuildDevelopmentSetup.html','Pre Build Development Setup','1.0.1.0','Oktober 2014','Sven Gijsen'];
	this.DocumentAuditing[2] = ['ExperimentManagerPlugin.html','Experiment Manager Plug-in Documentation','1.0.1.0','Oktober 2014','Sven Gijsen'];
	this.DocumentAuditing[3] = ['ParallelPortDevicePlugin.html','Parallel Port Plug-in Documentation','2.0.0.2','Oktober 2014','Sven Gijsen'];	
	this.DocumentAuditing[4] = ['ExtensionPluginTemplate.html','Extension Plug-in Template Documentation','2.0.0.2','Oktober 2014','Sven Gijsen'];	
	this.DocumentAuditing[5] = ['KeyBoardDevicePlugin.html','Keyboard Device Plug-in Documentation','1.0.0.1','Oktober 2014','Sven Gijsen'];		
	this.DocumentAuditing[6] = ['BrainStim_QMLExtensions.html','BrainStim QML Extensions Plug-in Documentation','1.0.0.1','Oktober 2014','Sven Gijsen'];					
	this.DocumentAuditing[7] = ['6_RetinoTopicMappingOutput.html','Retinotopic Mapping output files Tutorial','1.0.0.1','Oktober 2014','Sven Gijsen'];	
	
	
	
	
	//BrainStim version (?.?.?.?)
	///////////////////////////////////////////////////////////////
//	nNumberOfComponentChanges = 4;//See below, increment by adding new changes for this release!
//	tmpArray = BrainStim_CreateArray(1,nNumberOfComponentChanges,PropCounter);
//	this.componentVersioning.push.apply(this.componentVersioning, tmpArray)
//	this.componentVersioning[1][ExperimentManagerPlugin_Comp_Index] = 	['ExperimentManagerPlugin',	'1.0.0.1', '1.0.0.1', 1, '1.0.0.1', '1.0',  '1.0'];
//	this.componentVersioning[1][ParallelPortPlugin_Comp_Index] =        ['ParallelPortPlugin',      '1.0.0.1', '1.0.0.1', 0, 'x',       '1.0',  '1.0'];
//	this.componentVersioning[1][KeyBoardPlugin_Comp_Index] =            ['KeyBoardPlugin',          '1.0.0.1', '1.0.0.1', 0, 'x',       '1.0',  '1.0'];
//	this.componentVersioning[1][BrainStim_QMLExtensions_Comp_Index] =    ['BrainStim_QMLExtensions',  '1.0',     '1.0.0.1', 0, 'x',       'x',    'x'];
//	this.DocumentAuditing = BrainStim_CreateArray(8, AuthorCounter); //Default constructor!
	///////////////////////////////////////////////////////////////
//	this.DocumentAuditing[0][this.Doc_FileName_Index] = 'GettingStartedGuide.html';
//	this.DocumentAuditing[0][this.Doc_Title_Index] = 'Getting Started Guide';
//	this.DocumentAuditing[0][this.Doc_Version_Index] = '1.0.1.0';
//	this.DocumentAuditing[0][this.Doc_Date_Index] = 'Oktober 2014';
//	this.DocumentAuditing[0][this.Doc_Authors_Index] = 'Sven Gijsen';
//	
//	this.DocumentAuditing[1] = ['PreBuildDevelopmentSetup.html','Pre Build Development Setup','1.0.1.0','Oktober 2014','Sven Gijsen'];
//	this.DocumentAuditing[2] = ['ExperimentManagerPlugin.html','Experiment Manager Plug-in Documentation','1.0.1.0','Oktober 2014','Sven Gijsen'];
//	this.DocumentAuditing[3] = ['ParallelPortDevicePlugin.html','Parallel Port Plug-in Documentation','2.0.0.2','Oktober 2014','Sven Gijsen'];	
//	this.DocumentAuditing[4] = ['ExtensionPluginTemplate.html','Extension Plug-in Template Documentation','2.0.0.2','Oktober 2014','Sven Gijsen'];	
//	this.DocumentAuditing[5] = ['KeyBoardDevicePlugin.html','Keyboard Device Plug-in Documentation','1.0.0.1','Oktober 2014','Sven Gijsen'];		
//	this.DocumentAuditing[6] = ['BrainStim_QMLExtensions.html','BrainStim QML Extensions Plug-in Documentation','1.0.0.1','Oktober 2014','Sven Gijsen'];					
//	this.DocumentAuditing[7] = ['6_RetinoTopicMappingOutput.html','Retinotopic Mapping output files Tutorial','1.0.0.1','Oktober 2014','Sven Gijsen'];
}

BrainStim_Information.prototype.GetStrippedFileName = function (strFileUrl)//var strFileUrl=window.location.pathname;
{
	return strFileUrl.replace(/^.*[\\\/]/, '');
}

BrainStim_Information.prototype.GetDocumentIndex = function (strFileUrl)//var strFileUrl=window.location.pathname;
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

BrainStim_Information.prototype.GetDocumentTitle = function (strFileUrl)
{
	//alert('GetDocumentTitle');
	var nDocIndex = this.GetDocumentIndex(strFileUrl);
	if(nDocIndex >= 0)
		return this.DocumentAuditing[nDocIndex][this.Doc_Title_Index];
	return this.GetStrippedFileName(strFileUrl);
}

BrainStim_Information.prototype.GetDocumentHeader = function (strFileUrl)
{
	//alert('GetDocumentHeader ' + strFileUrl);
	var nDocIndex = this.GetDocumentIndex(strFileUrl);
	//alert('GetDocumentHeader ' + nDocIndex);
	if(nDocIndex >= 0)
	{
		document.write('<h1>' + this.DocumentAuditing[nDocIndex][this.Doc_Title_Index] + '</h1>');
		document.write('<p><em><strong>Version ' + this.DocumentAuditing[nDocIndex][this.Doc_Version_Index] + '</strong></em></p>'); //+ ', for BrainStim v' + + '</p>');
		document.write('<p><em><strong>' + this.DocumentAuditing[nDocIndex][this.Doc_Date_Index] + ', by ' + this.DocumentAuditing[nDocIndex][this.Doc_Authors_Index] + '</strong></em></p>');
	}
	else
	{
		document.write('<h1>' + this.GetStrippedFileName(strFileUrl) + '</h1>');
	}
	document.write('<p>&nbsp;</p>');
}

BrainStim_Information.prototype.GetCurrentRelease = function ()
{
	//alert('GetCurrentRelease');
	return this.BrainStimReleases[this.BrainStimReleases.length-1][0];
}

BrainStim_Information.prototype.GetReleaseByIndex = function (nIndex)
{
	//alert('GetReleaseByIndex');
	return this.BrainStimReleases[nIndex][0];
}

BrainStim_Information.prototype.GetReleaseQtVersionByIndex = function (nIndex)
{
	//alert('GetReleaseQtVersionByIndex');
	return this.BrainStimReleases[nIndex][1];
}

BrainStim_Information.prototype.GetReleaseProductVersionByIndex = function (nIndex) {
    //alert('GetReleaseProductVersionByIndex');
    return this.BrainStimReleases[nIndex][2];
}

BrainStim_Information.prototype.GetNumberOfReleases = function ()
{
	//alert('GetNumberOfReleases');
	return this.BrainStimReleases.length;
}

BrainStim_Information.prototype.GetNumberOfComponentsByReleaseIndex = function (nReleaseIndex)
{
	//alert('GetComponentRelease ' + nReleaseIndex);
	if(this.componentVersioning.length > nReleaseIndex)
	{
		return this.componentVersioning[nReleaseIndex].length;
	}
	return 0;
}

BrainStim_Information.prototype.CheckComponentByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('CheckComponentByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
	if(this.componentVersioning.length > nReleaseIndex)
	{
		if(this.componentVersioning[nReleaseIndex].length > nComponentIndex)
		{		
			return true;
		}
	}
	return false;
}

BrainStim_Information.prototype.GetLatestComponentReleaseByName = function (strComponentName)
{
	//alert('GetLatestComponentReleaseByName ' + strComponentName);
	for(var i=this.componentVersioning.length-1;i>=0;i--)
	{
		for(var j=0;j<this.componentVersioning[i].length;j++)
		{
		    if (this.componentVersioning[i][j][this.Name_Index] == strComponentName)
			{		
				return i;
			}
		}
	}	
	return -1;
}

BrainStim_Information.prototype.GetLatestComponentIndexByName = function (nReleaseIndex, strComponentName)
{
	//alert('GetLatestComponentIndexByName ' + strComponentName);
	for(var j=0;j<this.componentVersioning[nReleaseIndex].length;j++)
	{
	    if (this.componentVersioning[nReleaseIndex][j][this.Name_Index] == strComponentName)
		{		
			return j;
		}
	}	
	return -1;
}

BrainStim_Information.prototype.GetComponentNameByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('GetComponentNameByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
	if(this.CheckComponentByIndexes(nReleaseIndex, nComponentIndex))
		return this.componentVersioning[nReleaseIndex][nComponentIndex][this.Name_Index];
	return '';
}

BrainStim_Information.prototype.GetComponentStimGLVersionByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('GetComponentStimGLVersionByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
	if(this.CheckComponentByIndexes(nReleaseIndex, nComponentIndex))
	{
		return this.BrainStimReleases[this.componentVersioning[nReleaseIndex][nComponentIndex][this._BrainStimVersion_Index]][0];
	}
	return '';
}

BrainStim_Information.prototype.GetComponentVersionByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('GetComponentVersionByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
	if(this.CheckComponentByIndexes(nReleaseIndex, nComponentIndex))
		return this.componentVersioning[nReleaseIndex][nComponentIndex][this.Version_Index];
	return '';
}

BrainStim_Information.prototype.GetComponentProductVersionByIndexes = function (nReleaseIndex, nComponentIndex) {
    //alert('GetComponentProductVersionByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
    if (this.CheckComponentByIndexes(nReleaseIndex, nComponentIndex))
        return this.componentVersioning[nReleaseIndex][nComponentIndex][this.ProductVersion_Index];
    return '';
}

BrainStim_Information.prototype.GetComponentMinimalEXMLVersionByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('GetComponentMinimalEXMLVersionByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
	if(this.CheckComponentByIndexes(nReleaseIndex, nComponentIndex))
		return this.componentVersioning[nReleaseIndex][nComponentIndex][this.EXML_Index];
	return '';
}

BrainStim_Information.prototype.GetComponentDeviceInterfaceVersionByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('GetComponentDeviceInterfaceVersionByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
	if(this.CheckComponentByIndexes(nReleaseIndex, nComponentIndex))
		return this.componentVersioning[nReleaseIndex][nComponentIndex][this.DevInterface_Index];
	return '';
}

BrainStim_Information.prototype.GetComponentExtensionInterfaceVersionByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('GetComponentExtensionInterfaceVersionByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
	if(this.CheckComponentByIndexes(nReleaseIndex, nComponentIndex))
		return this.componentVersioning[nReleaseIndex][nComponentIndex][this.ExtInterface_Index];
	return '';
}

BrainStim_Information.prototype.GetMainAppCopyrightString = function ()
{ return 'Copyright (C) 2015'; }

BrainStim_Information.prototype.GetMainAppCompanyName = function ()
{	return 'MBIC, Maastricht Brain Imaging Center'; }

BrainStim_Information.prototype.GetMainAppInternalName = function ()
{	return 'BrainStim'; }

BrainStim_Information.prototype.GetMainAppPluginDeviceInterfaceString = function ()
{	return 'BrainStim.Plugins.DeviceInterface/1.0'; }

BrainStim_Information.prototype.GetMainAppPluginExtensionInterfaceString = function ()
{	return 'BrainStim.Plugins.ExtensionInterface/1.0'; }
