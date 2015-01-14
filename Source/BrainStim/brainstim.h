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


#ifndef BRAINSTIM_H
#define BRAINSTIM_H

#define BRAINSTIM_DEFAULT_WINDOW_BACKGROUND_COLOR_RED	235
#define BRAINSTIM_DEFAULT_WINDOW_BACKGROUND_COLOR_GREEN	235
#define BRAINSTIM_DEFAULT_WINDOW_BACKGROUND_COLOR_BLUE	235

#define FUNC_PLUGIN_GETSCRIPTMETAOBJECT						"GetScriptMetaObject"
#define FUNC_PLUGIN_GETSCRIPTMETAOBJECT_FULL				FUNC_PLUGIN_GETSCRIPTMETAOBJECT "(int)"
#define FUNC_PLUGIN_ISCOMPATIBLE							"IsCompatible"
#define FUNC_PLUGIN_ISCOMPATIBLE_FULL						FUNC_PLUGIN_ISCOMPATIBLE "()"
#define FUNC_PLUGIN_GETADDFILEEXT							"GetAdditionalFileExtensions"
#define FUNC_PLUGIN_GETADDFILEEXT_FULL						FUNC_PLUGIN_GETADDFILEEXT "()"
#define FUNC_PLUGIN_GETADDFILE_SLOT_HANDLERS				"GetAdditionalFileSlotHandlers"
#define FUNC_PLUGIN_GETADDFILE_SLOT_HANDLERS_FULL			FUNC_PLUGIN_GETADDFILE_SLOT_HANDLERS "()"
#define FUNC_PLUGIN_GETADDFILE_TYPESTYLE					"GetAdditionalFileTypeStyle"
#define FUNC_PLUGIN_GETADDFILE_TYPESTYLE_FULL				FUNC_PLUGIN_GETADDFILE_TYPESTYLE "(QString)"
#define FUNC_PLUGIN_GETADDFILE_API_FILENAME					"GetAdditionalFileTypeApiName"
#define FUNC_PLUGIN_GETADDFILE_API_FILENAME_FULL			FUNC_PLUGIN_GETADDFILE_API_FILENAME "(QString)"
#define FUNC_PLUGIN_GETADDFILE_TYPEEDITOR					"GetAdditionalFileTypeEditor"
#define FUNC_PLUGIN_GETADDFILE_TYPEEDITOR_FULL				FUNC_PLUGIN_GETADDFILE_TYPEEDITOR "(QString,QString)"
#define FUNC_PLUGIN_INITADDFILE_TYPEEDITOR					"InitializeAdditionalFileTypeEditor"
#define FUNC_PLUGIN_INITADDFILE_TYPEEDITOR_FULL				FUNC_PLUGIN_INITADDFILE_TYPEEDITOR "(QString)"
//#define FUNC_PLUGIN_REMADDFILE_TYPEEDITOR					"RemAdditionalFileTypeEditor"
//#define FUNC_PLUGIN_REMADDFILE_TYPEEDITOR_FULL			FUNC_PLUGIN_REMADDFILE_TYPEEDITOR "(QString,QString)"
#define FUNC_PLUGIN_LOADADDFILE								"LoadAdditionalFile"
#define FUNC_PLUGIN_LOADADDFILE_FULL						FUNC_PLUGIN_LOADADDFILE "(QString)"
//#define FUNC_PLUGIN_SAVEADDFILE							"SaveAdditionalFile"
//#define FUNC_PLUGIN_SAVEADDFILE_FULL						FUNC_PLUGIN_SAVEADDFILE "(QString)"
#define FUNC_PLUGIN_REMOVEADDFILE							"RemoveAdditionalFile"
#define FUNC_PLUGIN_REMOVEADDFILE_FULL						FUNC_PLUGIN_REMOVEADDFILE "(QString)"

#define FUNC_PLUGIN_WIDGET_SAVEFILE							"saveFile"
#define FUNC_PLUGIN_WIDGET_SAVEFILE_FULL					FUNC_PLUGIN_WIDGET_SAVEFILE "(QString)"

#define FUNC_PLUGIN_DOCUMENTHASCHANGED						"DocumentHasChanged"
#define FUNC_PLUGIN_DOCUMENTHASCHANGED_FULL					FUNC_PLUGIN_DOCUMENTHASCHANGED "(QString,bool)"
#define FUNC_PLUGIN_DOCUMENTISCLOSING						"DocumentIsClosing"
#define FUNC_PLUGIN_DOCUMENTISCLOSING_FULL					FUNC_PLUGIN_DOCUMENTISCLOSING "(QString,bool)"

#define FUNC_PLUGIN_COPYAVAILABLE_FULL						"copyAvailable(bool)"				//fixed signal signature, do not change
#define FUNC_PLUGIN_CURSORPOSCHANGED_FULL					"cursorPositionChanged(int, int)"	//fixed signal signature, do not change

#define FUNC_BRAINSTIM_CONFDOCMODSETTING					"configureDocumentModifiedSetting"
#define FUNC_BRAINSTIM_CONFDOCMODSETTING_FULL				FUNC_BRAINSTIM_CONFDOCMODSETTING "(QString,bool)"

#define FUNC_BRAINSTIM_PROCDOCCLOSING						"processDocumentClosing"
#define FUNC_BRAINSTIM_PROCDOCCLOSING_FULL					FUNC_BRAINSTIM_PROCDOCCLOSING "(QString,bool)"


#endif // BRAINSTIM_H
//#ifdef Q_OS_WIN32 //Are we on Windows?
//#ifdef Q_WS_X11
//#ifdef Q_WS_MACX