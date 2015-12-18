// JavaScript Document
//
//var BrainStimInfo = new BrainStim_Information();
//var sComponentName = "FirebirdClientPlugin";			
//Log(BrainStimInfo.GetLatestComponentReleaseByName(sComponentName));

function BrainStim_Cleanup()
{
	//window.BrainStimInfo = null;
	BrainStim_CreateArray = null;
	BrainStim_Information.prototype.GetCurrentRelease = null;
	BrainStim_Information.prototype.GetReleaseByIndex = null;
	BrainStim_Information.prototype.GetReleaseQtVersionByIndex = null;
	BrainStim_Information.prototype.GetReleaseProductVersionByIndex = null;
	BrainStim_Information.prototype.GetNumberOfReleases = null;
	BrainStim_Information.prototype.GetNumberOfComponentsByReleaseIndex = null;
	BrainStim_Information.prototype.CheckComponentByIndexes = null;
	BrainStim_Information.prototype.GetLatestComponentReleaseByName = null;
	BrainStim_Information.prototype.GetLatestComponentIndexByName = null;
	BrainStim_Information.prototype.GetComponentNameByIndexes = null;
	BrainStim_Information.prototype.GetComponentBrainStimVersionByIndexes = null;
	BrainStim_Information.prototype.GetComponentVersionByIndexes = null;
	BrainStim_Information.prototype.GetComponentProductVersionByIndexes = null;
	BrainStim_Information.prototype.GetComponentMinimalEXMLVersionByIndexes = null;
	BrainStim_Information.prototype.GetComponentDeviceInterfaceVersionByIndexes = null;
	BrainStim_Information.prototype.GetComponentExtensionInterfaceVersionByIndexes = null;
	BrainStim_Information.prototype.GetMainAppCopyrightString = null;
	BrainStim_Information.prototype.GetMainAppCompanyName = null;
	BrainStim_Information.prototype.GetMainAppInternalName = null;
	BrainStim_Information.prototype.GetMainAppPluginDeviceInterfaceString = null;
	BrainStim_Information.prototype.GetMainAppPluginExtensionInterfaceString = null;
	BrainStim_Information.prototype = null;
	BrainStim_Information = null;
	BrainStim_Cleanup = null;
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
	this.BrainStimReleases = [['1.0.0.1', '5.3.2', '1.0.0.1'], ['1.1.0.1', '5.3.2', '1.1.0.1']];//, ['Exml', 'Qt', 'Product']];
	var PropCounter = 0;
	var ComponentCounter = 0;
	var ReleaseCounter = 0;
	var tmpArray;

	this.Name_Index = PropCounter++;
	this.Version_Index = PropCounter++;
	this.ProductVersion_Index = PropCounter++;
	this._BrainStimVersion_Index = PropCounter++;
	this.EXML_Index = PropCounter++;
	this.DevInterface_Index = PropCounter++;
	this.ExtInterface_Index = PropCounter++;
	
	var ExperimentManagerPlugin_Comp_Index = ComponentCounter++;
	var BrainStim_QMLExtensions_Comp_Index = ComponentCounter++;
	this.componentVersioning = BrainStim_CreateArray(this.BrainStimReleases.length, ComponentCounter, PropCounter);//Default constructor!
	
	this.currentCopyright = 'Copyright (C) 2016';
	this.appCompany = 'MBIC';
	this.appName = 'BrainStim';
	
	//BrainStim version (1.0.0.1)
	///////////////////////////////////////////////////////////////
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.Name_Index] = 'ExperimentManagerPlugin';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.Version_Index] = '1.0.0.1';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.ProductVersion_Index] = '1.0.0.1';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this._BrainStimVersion_Index] = ReleaseCounter; //-->1.0.0.1
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.EXML_Index] = '1.0.0.1';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.DevInterface_Index] = '1.0';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.ExtInterface_Index] = '1.0';
	///////////////////////////////////////////////////////////////
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.Name_Index] = 'BrainStim_QMLExtensions';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.Version_Index] = '1.0';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.ProductVersion_Index] = 'x';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this._BrainStimVersion_Index] = ReleaseCounter; //-->1.0.0.1
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.EXML_Index] = 'x';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.DevInterface_Index] = 'x';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.ExtInterface_Index] = 'x';
	ReleaseCounter++;
	///////////////////////////////////////////////////////////////

	//BrainStim version (1.1.0.1)
	///////////////////////////////////////////////////////////////
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.Name_Index] = 'ExperimentManagerPlugin';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.Version_Index] = '1.1.0.1';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.ProductVersion_Index] = '1.1.0.1';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this._BrainStimVersion_Index] = ReleaseCounter; //-->1.1.0.1
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.EXML_Index] = '1.1.0.1';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.DevInterface_Index] = '1.1';
	this.componentVersioning[ReleaseCounter][ExperimentManagerPlugin_Comp_Index][this.ExtInterface_Index] = '1.1';
	///////////////////////////////////////////////////////////////
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.Name_Index] = 'BrainStim_QMLExtensions';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.Version_Index] = '1.0';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.ProductVersion_Index] = 'x';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this._BrainStimVersion_Index] = ReleaseCounter; //-->1.1.0.1
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.EXML_Index] = 'x';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.DevInterface_Index] = 'x';
	this.componentVersioning[ReleaseCounter][BrainStim_QMLExtensions_Comp_Index][this.ExtInterface_Index] = 'x';
	ReleaseCounter++;
	///////////////////////////////////////////////////////////////
	
	
	
	///////////////////////////////////////////////////////////////
	this.currentDevInterface = 'BrainStim.Plugins.DeviceInterface/1.1';
	this.currentExtInterface = 'BrainStim.Plugins.ExtensionInterface/1.1';
}













BrainStim_Information.prototype.GetMainAppCopyrightString = function ()
{ return this.currentCopyright; }

BrainStim_Information.prototype.GetMainAppCompanyName = function ()
{	return this.appCompany; }

BrainStim_Information.prototype.GetMainAppInternalName = function ()
{	return this.appName; }

BrainStim_Information.prototype.GetMainAppPluginDeviceInterfaceString = function ()
{	return this.currentDevInterface; }

BrainStim_Information.prototype.GetMainAppPluginExtensionInterfaceString = function ()
{	return this.currentExtInterface; }

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

BrainStim_Information.prototype.GetComponentBrainStimVersionByIndexes = function (nReleaseIndex, nComponentIndex)
{
	//alert('GetComponentBrainStimVersionByIndexes ' + nReleaseIndex + ", " + nComponentIndex);
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
