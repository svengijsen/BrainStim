//ExperimentManagerplugin
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


#ifndef EXPERIMENTMANAGERDEFINES_H
#define EXPERIMENTMANAGERDEFINES_H
#include "./../../BrainStim/maindefines.h"

//Only edit the below!
#define PLUGIN_FILE_VERSION_STRING_MAJOR	1   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_FILE_VERSION_STRING_MINOR	0   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_FILE_VERSION_STRING_REVISION	0   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_FILE_VERSION_STRING_BUILD	1   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_PRODUCT_VERSION_STRING_MAJOR	1   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_PRODUCT_VERSION_STRING_MINOR	0   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_PRODUCT_VERSION_STRING_REVISION	0   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_PRODUCT_VERSION_STRING_BUILD	1   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_LEGAL_COPYRIGHT						MAIN_PROGRAM_LEGAL_COPYRIGHT
#define PLUGIN_AUTHOR_EMAIL							MAIN_PROGRAM_AUTHOR_EMAIL
#define PLUGIN_AUTHOR_NAME							MAIN_PROGRAM_AUTHOR_NAME
#define PLUGIN_COMPANY_NAME							MAIN_PROGRAM_COMPANY_SHORT_NAME
#define PLUGIN_INTERNAL_NAME						"ExperimentManager"
#define PLUGIN_PRODUCT_NAME							PLUGIN_INTERNAL_NAME
#define PLUGIN_INTERNAL_EXTENSION					"lib"
#define PLUGIN_ORIGINAL_FILENAME					PLUGIN_INTERNAL_NAME "." PLUGIN_INTERNAL_EXTENSION
#define PLUGIN_FULL_NAME							PLUGIN_INTERNAL_NAME "(v" PLUGIN_FILE_VERSION_STRING ")"
#define PLUGIN_FILE_DESCRIPTION						"Library that implements the Experiment Manager"
#define PLUGIN_INFORMATION							PLUGIN_INTERNAL_NAME " Plugin(v" PLUGIN_FILE_VERSION_STRING ")"
#define PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION	"1.0.0.1"   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_EXMLDOC_MINIMAL_VERSION_MAJOR	1   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_EXMLDOC_MINIMAL_VERSION_MINOR	0   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_EXMLDOC_MINIMAL_VERSION_REVISION	0   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_EXMLDOC_MINIMAL_VERSION_BUILD	1   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_EXMLDOC_EXTENSION					"exml"
#define PLUGIN_EXMLDOC_VALIDATION_NAME	PLUGIN_EXMLDOC_EXTENSION "_v1001.xsd"   //Please do not edit this line manually, see BinaryVersioning.qs (14825141414).
#define PLUGIN_QMLDOC_EXTENSION						"qml"
#define PLUGIN_QML_INTERFACE_PROVIDER_URI			"QML.Interface"
#define PLUGIN_QML_INTERFACE_PROVIDER_APINAME		"BrainStimAPI"
#define PLUGIN_QML_INTERFACE_PROVIDER_MAJOR			1
#define PLUGIN_QML_INTERFACE_PROVIDER_MINOR			0
//Only edit until here!


#define PLUGIN_FILE_VERSION_STRING				VERSION_STRING_INTERMEDIATE(PLUGIN_FILE_VERSION_STRING_MAJOR,PLUGIN_FILE_VERSION_STRING_MINOR,PLUGIN_FILE_VERSION_STRING_REVISION,PLUGIN_FILE_VERSION_STRING_BUILD)
#define PLUGIN_PRODUCT_VERSION_STRING			VERSION_STRING_INTERMEDIATE(PLUGIN_PRODUCT_VERSION_STRING_MAJOR,PLUGIN_PRODUCT_VERSION_STRING_MINOR,PLUGIN_PRODUCT_VERSION_STRING_REVISION,PLUGIN_PRODUCT_VERSION_STRING_BUILD)

#define SETTING_ENABLEAUTOEXMLVALIDATION		"enableautomaticexmlfilevalidation"

#endif // EXPERIMENTMANAGERDEFINES_H




