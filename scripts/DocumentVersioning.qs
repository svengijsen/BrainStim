Include("DocumentVersioningLib.qs");
	
/////////////////////////CONFIGURATION(BEGIN)////////////////////////////////////////////
docVer_sSourcePath 				= "../";
docVer_sDestPath 				= docVer_sSourcePath;
docVer_sMainAuthor				= "Sven Gijsen";
docVer_sCurrentAuthoringMonth 	= "Juli 2015";
docVer_sComment 				= "Please do not edit the below line(s) manually, see DocumentVersioning.qs (" + CreateTimeStamp(sTimeObject) + ")";
docVer_sScriptComment  			= "//" + docVer_sComment + ", version " + BrainStim_Settings_CurrentBrainStimReleaseString;
docVer_sHTMLComment  			= "<!-- " + docVer_sComment + ", version " + BrainStim_Settings_CurrentBrainStimReleaseString + "-->";

//defaultDocumentEditing("documents/Development/PreBuildDevelopmentSetup.html","PreBuildDevelopmentSetup.html", 2);
//defaultDocumentEditing("documents/Development/Development.html","Development.html", 2);
//defaultDocumentEditing("documents/Development/ExtensionPluginTemplate.html","ExtensionPluginTemplate.html", 2);
defaultDocumentEditing("index.html", "index.html", 0);
defaultDocumentEditing("pluginrefs.html", "pluginrefs.html", 0);
defaultDocumentEditing("documents/ExperimentManagerPlugin/ExperimentManagerPlugin.html", "ExperimentManagerPlugin.html", 2);
defaultDocumentEditing("documents/GettingStartedGuide/GettingStartedGuide.html","GettingStartedGuide.html", 2);
defaultDocumentEditing("documents/Examples/RetinoTopicMappingOutput/RetinoTopicMappingOutput.html","RetinoTopicMappingOutput.html", 3);
defaultDocumentEditing("documents/Examples/AdvancedExamples.html","AdvancedExamples.html", 2);
defaultDocumentEditing("documents/BrainStim/SettingsDialog.html", "SettingsDialog.html", 2);
defaultDocumentEditing("documents/BrainStim/QtLibraryScriptBindings.html", "QtLibraryScriptBindings.html", 2);
defaultDocumentEditing("documents/BrainStim/FileTypeAssociations.html", "FileTypeAssociations.html", 2);
defaultDocumentEditing("documents/BrainStim/CustomUserLogins.html", "CustomUserLogins.html", 2);
defaultDocumentEditing("documents/BrainStim/FunctionsAndIncludes.html", "FunctionsAndIncludes.html", 2);
defaultDocumentEditing("documents/BrainStim/GarbageCollection.html", "GarbageCollection.html", 2);
defaultDocumentEditing("documents/BrainStim/CommandLineOptions.html", "CommandLineOptions.html", 2);
defaultDocumentEditing("documents/BrainStim/InternalNetworkInterface.html", "InternalNetworkInterface.html", 2);
defaultDocumentEditing("documents/BrainStim/SignalSlotConnections.html", "SignalSlotConnections.html", 2);
defaultDocumentEditing("documents/BrainStim/DocumentTypes.html", "DocumentTypes.html", 2);
defaultDocumentEditing("documents/BrainStim/CustomMenus.html", "CustomMenus.html", 2);
defaultDocumentEditing("documents/BrainStim/MainLogFile.html", "MainLogFile.html", 2);
defaultDocumentEditing("documents/BrainStim/UserInterface.html", "UserInterface.html", 2);
defaultDocumentEditing("documents/BrainStim/ScriptDebugging.html", "ScriptDebugging.html", 2);
defaultDocumentEditing("documents/BrainStim/DefaultPlugins.html", "DefaultPlugins.html", 2);
defaultDocumentEditing("documents/BrainStim/PluginFrameWork.html", "PluginFrameWork.html", 2);
defaultDocumentEditing("documents/BrainStim/OsUserLogins.html", "OsUserLogins.html", 2);
defaultDocumentEditing("documents/ExperimentManagerPlugin/CreatingExperiments.html", "CreatingExperiments.html", 2);
defaultDocumentEditing("documents/ExperimentManagerPlugin/RetinoMapper.html", "RetinoMapper.html", 2);
defaultDocumentEditing("documents/ExperimentManagerPlugin/QML2Viewer.html", "QML2Viewer.html", 2);
defaultDocumentEditing("documents/BrainStim_QMLExtensions/BrainStim_QMLExtensions.html", "BrainStim_QMLExtensions.html", 2);
defaultDocumentEditing("documents/BrainStim/ExamplesAndTutorials.html", "ExamplesAndTutorials.html", 2);
defaultDocumentEditing("documents/BrainStim/ScriptClassReferences.html", "ScriptClassReferences.html", 2);
defaultDocumentEditing("documents/BrainStim/UpdateManager.html", "UpdateManager.html", 2);

//Special (non-default document edits)
sSourceList.push(docVer_sSourcePath + "js/BrainStim.js");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "//doc-version-brainstimrelease-begin";
	changeSet[sSourceList.length-1][1][0] = docVer_sScriptComment + "\n" + "var currentBrainStimRelease = " + "'" + BrainStim_Settings_CurrentBrainStimReleaseString + "';";
	changeSet[sSourceList.length-1][2][0] = "//doc-version-brainstimrelease-end";

sSourceList.push(docVer_sSourcePath + "MainDocumentation.qhcp");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-title-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + "\t<title>BrainStim v" + BrainStim_Settings_CurrentBrainStimReleaseString + "</title>";
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-title-end-->";

sSourceList.push(docVer_sSourcePath + "documents/ExperimentManagerPlugin/ExperimentManagerPlugin.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-default-header-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
								CreateHTMLHeaderSection(tmpBrainStimDocumentInfo.GetDocumentName("ExperimentManagerPlugin.html") + " (v" + tmpBrainStimInfo.GetComponentVersionByIndexes(BrainStim_Settings_LatestBrainStimReleaseIndex, tmpBrainStimInfo.GetLatestComponentReleaseByName("ExperimentManagerPlugin")) + ")",
								tmpBrainStimDocumentInfo.GetDocumentVersion("ExperimentManagerPlugin.html"),
								tmpBrainStimDocumentInfo.GetDocumentAuthors("ExperimentManagerPlugin.html"),
								tmpBrainStimDocumentInfo.GetDocumentDate("ExperimentManagerPlugin.html"));
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-default-header-end-->";
		
	sSourceList.push(docVer_sSourcePath + "documents/GettingStartedGuide/GettingStartedGuide.html");
	sDestList.push(sSourceList[sSourceList.length-1]);
		changeSet[sSourceList.length-1] = CreateArray(3,0);
		changeSet[sSourceList.length-1][0][0] = "<!--doc-version-jquery-links-begin-->";
		changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
									CreateJQueryHTMLHeaderLinks(2);
		changeSet[sSourceList.length-1][2][0] = "<!--doc-version-jquery-links-end-->";

	sSourceList.push(docVer_sSourcePath + "documents/GettingStartedGuide/GettingStartedGuide.html");
	sDestList.push(sSourceList[sSourceList.length-1]);
		changeSet[sSourceList.length-1] = CreateArray(3,0);
		changeSet[sSourceList.length-1][0][0] = "<!--doc-version-tocify-links-begin-->";
		changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
									CreateTocifyHTMLHeaderLinks(2);
		changeSet[sSourceList.length-1][2][0] = "<!--doc-version-tocify-links-end-->";

	sSourceList.push(docVer_sSourcePath + "documents/GettingStartedGuide/GettingStartedGuide.html");
	sDestList.push(sSourceList[sSourceList.length-1]);
		changeSet[sSourceList.length-1] = CreateArray(3,0);
		changeSet[sSourceList.length-1][0][0] = "<!--doc-version-tocify-footer-begin-->";
		changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + 
									CreateTocifyHTMLFooter();
		changeSet[sSourceList.length-1][2][0] = "<!--doc-version-tocify-footer-end-->";

sSourceList.push(docVer_sSourcePath + "documents/ExperimentManagerPlugin/CreatingExperiments.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qtCreatorDownloadName-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + tmpBrainStimDocumentInfo.GetDocumentInformation("QtCreatorDownloadName");
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qtCreatorDownloadName-end-->";
	
sSourceList.push(docVer_sSourcePath + "documents/ExperimentManagerPlugin/CreatingExperiments.html");
sDestList.push(sSourceList[sSourceList.length-1]);
	changeSet[sSourceList.length-1] = CreateArray(3,0);
	changeSet[sSourceList.length-1][0][0] = "<!--doc-version-qtCreatorDownloadDir-begin-->";
	changeSet[sSourceList.length-1][1][0] = docVer_sHTMLComment + "\n" + tmpBrainStimDocumentInfo.GetDocumentInformation("QtCreatorDownloadDir");
	changeSet[sSourceList.length-1][2][0] = "<!--doc-version-qtCreatorDownloadDir-end-->";
/////////////////////////CONFIGURATION(END)//////////////////////////////////////////////

startDocumentEditing();
docVer_CleanupScript();
