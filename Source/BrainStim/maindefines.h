//BrainStim
//Copyright (C) 2015  Sven Gijsen
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef MAINDEFINES_H
#define MAINDEFINES_H

#define QUOTE(name) #name
#define MAKESTRING(macro) QUOTE(macro)
#define VERSION_STRING_INTERMEDIATE(a,b,c,d) MAKESTRING(a) "." MAKESTRING(b) "." MAKESTRING(c) "." MAKESTRING(d)
#define VERSION_STRING_INTERMEDIATE_NOSEP(a,b,c,d) MAKESTRING(a) MAKESTRING(b) MAKESTRING(c) MAKESTRING(d)
//#define JOIN_STRINGS(a,b) a " - " EXPANDMACROx1(b)

//Only edit the below!

//Also search and change "Q_CLASSINFO("ScriptAPIClassName"..!!!

#define MAIN_PROGRAM_COMPANY_SHORT_NAME	"MBIC"
#define MAIN_PROGRAM_LEGAL_COPYRIGHT	"Copyright (C) 2016"   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_FILE_VERSION_STRING_MAJOR	1   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_FILE_VERSION_STRING_MINOR	1   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_FILE_VERSION_STRING_REVISION	0   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_FILE_VERSION_STRING_BUILD	1   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_MAJOR	1   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_MINOR	1   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_REVISION	0   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_PRODUCT_VERSION_STRING_BUILD	1   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_INTERNAL_NAME	"BrainStim"   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_FULL_NAME							MAIN_PROGRAM_INTERNAL_NAME "(v" MAIN_PROGRAM_FILE_VERSION_STRING ")"
#define MAIN_PROGRAM_PRODUCT_NAME                       MAIN_PROGRAM_INTERNAL_NAME
#define MAIN_PROGRAM_AUTHOR_NAME						"Sven Gijsen"
#define MAIN_PROGRAM_AUTHOR_EMAIL						"svengijsen@gmail.com"
#define MAIN_PROGRAM_ORIGINAL_FILENAME					MAIN_PROGRAM_INTERNAL_NAME ".exe"
#define MAIN_PROGRAM_INI_FILENAME						MAIN_PROGRAM_INTERNAL_NAME ".ini"
#define MAIN_PROGRAM_32BIT_STRING						"32-bits"
#define MAIN_PROGRAM_64BIT_STRING						"64-bits"
#define MAIN_PROGRAM_LOGFILE_NAME						"logfile.txt"
#define MAIN_PROGRAM_DOC_DIRNAME						"Documents"
#define MAIN_PROGRAM_DOC_INCLUDENAME					"Include"
#define MAIN_PROGRAM_JAVASCRIPT_DIRNAME					"Javascript"
#define MAIN_PROGRAM_QML_DIRNAME						"QML"
#define MAIN_PROGRAM_QTSCRIPT_DIRNAME					"QtScript"
#define MAIN_PROGRAM_EXAMPLES_DIRNAME					"Examples"
#define MAIN_PROGRAM_TEMP_DIRNAME						"Temp"
#define MAIN_PROGRAM_MENUS_DIRNAME						"Menus"
#define MAIN_PROGRAM_OUTPUTS_DIRNAME					"Outputs"
#define MAIN_PROGRAM_PLUGINS_DIRNAME					"Plugins"
#define MAIN_PROGRAM_PLUGINS_REGISTRY_DIRNAME			"Plugins"
#define MAIN_PROGRAM_XSD_DIRNAME						"Xsd"
#define MAIN_PROGRAM_UPDATES_DIRNAME					"Updates"
#define MAIN_PROGRAM_DOC_BINARYCOLLFILE					"MainDocumentation.qhc"
#define MAIN_PROGRAM_DOC_COMPHELPFILE					"MainDocumentation.qch"
#define MAIN_PROGRAM_FILE_DESCRIPTION					MAIN_PROGRAM_INTERNAL_NAME ", a Open Source Cross Platform Scriptable Application"
#define MAIN_PROGRAM_SHARED_MEM_KEY						MAIN_PROGRAM_INTERNAL_NAME "_SHARED"

#define MAIN_PROGRAM_FILESEXTENSION_LIST				"Any files (*);;" MAIN_PROGRAM_INTERNAL_NAME " standard files (*.qs *.svg *.svgz *.svg.gz);;QT Script files (*.qs);;SVG files (*.svg *.svgz *.svg.gz);;"
#define MAIN_PROGRAM_POST_FILESEXTENSION_LIST			""//"Any files (*)"
#define MAIN_PROGRAM_UNTITLED_NAME						"Untitled"
#define UNKNOWNENUMSTRING								"Unknown_Enumeration Value"
#define MAIN_PROGRAM_STANDARD_DATETIME_FORMAT			"yyyyMMddHHmmsszzz"
#define MAIN_PROGRAM_INSTALLATIONFILE_EXT_POSTSTRING	"_installed"
#define MAIN_PROGRAM_INSTALLATION_LOGFILE_POSTSTRING	MAIN_PROGRAM_INSTALLATIONFILE_EXT_POSTSTRING ".log"
#define MAIN_PROGRAM_DEVICE_INTERFACE	"BrainStim.Plugins.DeviceInterface/1.1"   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).
#define MAIN_PROGRAM_EXTENSION_INTERFACE	"BrainStim.Plugins.ExtensionInterface/1.1"   //Please do not edit this line manually, see BinaryVersioning.qs (151216115531).

#define REGISTRY_SETTINGS_SYSTEM						"SystemSettings"
#define REGISTRY_SETTINGS_USER							"UserSettings"
#define REGISTRY_USER_PASSWORDHASH						"LogonUserPassHash"
#define REGISTRY_DONOTLOADSCRIPTEXTENSION				"General/DoNotLoadScriptExtension"
#define REGISTRY_ALLOWMULTIPLEINHERITANCE				"General/AllowMultipleInheritance"
#define REGISTRY_RECENTFILELIST							"General/RecentFileList"
#define REGISTRY_MAINAPPUSERDIRECTORY					"General/MainAppUserDirectory"			//contains BrainStim.ini, Examples and Outputs directories
#define REGISTRY_USERPLUGINSROOTDIRECTORY				"General/UserPluginsRootDirectory"		//contains custom plugins
#define REGISTRY_DEFAULTPLUGINSROOTDIRECTORY			"General/DefaultPluginsRootDirectory"	//cannot be configured through the settings dialog, is derived!
#define REGISTRY_EXAMPLESROOTDIRECTORY					"General/ExamplesRootDirectory"			//cannot be configured through the settings dialog, is derived!
#define REGISTRY_ENABLECUSTOMUSERLOGINS					"General/EnableCustomUserLogins"
//#define REGISTRY_OPENINEXTERNALDEBUGGER					"Debugging/OpenExtDebug"
#define REGISTRY_RENDERTYPE								"Rendering/RenderType"
#define REGISTRY_HQANTIALIAS							"Rendering/HQAntiAlias"
#define REGISTRY_SCRIPTING_INCLUDEPATHS					"Scripting/IncludeDirectories"
#define REGISTRY_ENABLENETWORKSERVER					"Network/EnableInternalNetworkServer"
#define REGISTRY_SERVERHOSTADDRESS						"Network/ServerHostAddress"
#define REGISTRY_SERVERHOSTPORT							"Network/ServerHostPort"

//Only edit until here!
#define SCRIPT_API_CLASS_NAME_TAG						"ScriptAPIClassName"
#define MAIN_PROGRAM_OPENFILES_SLOT_NAME				"openFiles"
#define MAIN_PROGRAM_LOG_SLOT_NAME						"write2OutputWindow"
#define MAIN_PROGRAM_REOPEN_SLOT_NAME					"reOpenCurrentFile"
#define MAIN_PROGRAM_SCRIPTINCLUDES_NAME				"getScriptIncludePaths"
#define MAIN_PROGRAM_CLOSEACTIVEDOCUMENT_NAME			"closeActiveDocument"
#define MAIN_PROGRAM_GETSAVEDDOCKWIDGETSIZEHINT_NAME	"getSavedDockWidgetSizeHint"
#define MAIN_PROGRAM_REGISTERMAINMENUACTION_NAME		"registerMainMenuAction"
#define MAIN_PROGRAM_GETSTOREDSETTINGVALUE_NAME			"getStoredSettingValue"
#define MAIN_PROGRAM_LOADDYNAMICPLUGINS_NAME			"loadDynamicPlugins"
#define MAIN_PROGRAM_UNLOADDYNAMICPLUGINS_NAME			"unloadDynamicPlugins"
#define MAIN_PROGRAM_STOPAPPEXCHANGE_NAME				"stopAppExchange"
#define MAIN_PROGRAM_QUITANDINSTALLUPDATES_NAME			"quitAndInstallUpdates"
#define METHOD_TYPE_SIGNAL_TAG							"signal"
#define METHOD_TYPE_SLOT_TAG							"slot"
#define METHOD_TYPE_MEMBER_TAG							"member"
#define METHOD_TYPE_PROPERTY_TAG						"property"
#define METHOD_TYPE_ENUMERATOR_TAG						"enumerator"

#define PLUGIN_CUSTOMTYPE_GETNEWEDITOR_METHOD_FULL		"getNewEditorObject()"
#define PLUGIN_CUSTOMTYPE_RESOLVEPARAM_METHOD_FULL		"resolveParameterValueType(QVariant, bool)"

#define MENUACTION_SECTION_SEPERATOR					"|"
#define MENUACTION_SUBMENU_SEPERATOR					"/"
#define MENUACTION_TYPE_SCRIPTPATH						"scriptpath"
#define MENUACTION_TYPE_MENUITEM						"menuitem"

#define TYPE_BOOL_TRUE									"true"
#define TYPE_BOOL_FALSE									"false"

#define EXPERIMENT_DOCKWIDGET_GROUPSEP_CHAR				";"
#define EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR				"/"

#define MAIN_PROGRAM_FILE_VERSION_STRING				VERSION_STRING_INTERMEDIATE(MAIN_PROGRAM_FILE_VERSION_STRING_MAJOR,MAIN_PROGRAM_FILE_VERSION_STRING_MINOR,MAIN_PROGRAM_FILE_VERSION_STRING_REVISION,MAIN_PROGRAM_FILE_VERSION_STRING_BUILD)
#define MAIN_PROGRAM_PRODUCT_VERSION_STRING				VERSION_STRING_INTERMEDIATE(MAIN_PROGRAM_PRODUCT_VERSION_STRING_MAJOR,MAIN_PROGRAM_PRODUCT_VERSION_STRING_MINOR,MAIN_PROGRAM_PRODUCT_VERSION_STRING_REVISION,MAIN_PROGRAM_PRODUCT_VERSION_STRING_BUILD)

#endif // MAINDEFINES_H


