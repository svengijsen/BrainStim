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


#ifndef EXPMANAGER_GLOBAL_H
#define EXPMANAGER_GLOBAL_H

#include <QGraphicsItem>
#include "../../BrainStim/mainappinfo.h"
#include "randomgenerator.h"
class ExperimentManager;

/*! The namespace for the Experiment Manager */
namespace ExperimentManagerNameSpace
{
	/*! 
	 * The enum (TriggerTimerType) can store a TriggerTimer type 
	*/
	enum TriggerTimerType
	{
		QPC_TriggerTimerType_Old		= 0, //!< 0: The Query Performance Counter Timer type, the old implementation, this one became obsolete.
		QTimer_TriggerTimerType			= 1, //!< 1: The QTimer Timer type, only use if it's necesary to force it to use a QTimer Timer type.
		QPC_TriggerTimerType			= 2, //!< 2: The Query Performance Counter Timer type, the new default implementation, Low CPU absorption, No interval jittering, Interval > 15 mSecs.
		Fast_TriggerTimerType			= 3  //!< 3: The Fast Timer type, another new implementation, Medium CPU absorption, interval jittering, Fast -> Interval > 5 mSecs.
	};

	enum ExperimentTimerType //The type of a experiment timer, used for logging(flags, combinations possible)
	{
		Experiment_Timer_None		= 0,
		Experiment_Timer_Experiment	= 1,
		Experiment_Timer_Frame		= 2
		//Experiment_Timer_Experiment	= 4,
		//Experiment_Timer_Experiment	= 8,
		//Experiment_Timer_Experiment	= 16,
		//Experiment_Timer_Experiment	= 32
	};

	/*! 
	 * The enum ExperimentSubObjectState represents the state of the an Experiment (sub)-object 
	 */
	enum ExperimentSubObjectState
	{	
		Experiment_SubObject_Constructing	= 0, /*!< enum value 0 */
		Experiment_SubObject_Initialized	= 1, /*!< enum value 1 */
		Experiment_SubObject_Started		= 2, /*!< enum value 2 */
		Experiment_SubObject_Abort			= 3, /*!< enum value 3 */
		Experiment_SubObject_IsAborting		= 4, /*!< enum value 4 */
		Experiment_SubObject_Stop			= 5, /*!< enum value 5 */
		Experiment_SubObject_IsStopping		= 6, /*!< enum value 6 */
		Experiment_SubObject_Stopped		= 7, /*!< enum value 7 */
		Experiment_SubObject_Destructing	= 8, /*!< enum value 8 */
		Experiment_SubObject_MaxState		= 9  /*!< enum value 9 */
	};

	enum EXML_TAG_SECTIONS //These are the main section Tag's
	{
		EXML_Declarations,
		EXML_Actions,
		EXML_Object,
		EXML_Event
	};

	enum ExperimentStructureItemType
	{
		TypeUndefined					= 0,
		TypeBlockItem					= QGraphicsItem::UserType + 1,
		TypeAutoLoopConnectionItem		= QGraphicsItem::UserType + 2,
		TypeLoopConnectionItem			= QGraphicsItem::UserType + 3,
		TypeObjectItem					= QGraphicsItem::UserType + 4,
		TypeMethodConnectionItem		= QGraphicsItem::UserType + 5,
		TypeObjectInitializationItem	= QGraphicsItem::UserType + 6,
		TypeObjectFinalizationItem		= QGraphicsItem::UserType + 7,
		TypeObjectSignalMethodItem		= QGraphicsItem::UserType + 8,
		TypeObjectSlotMethodItem		= QGraphicsItem::UserType + 9,
		TypeGroupInitializationsItem	= QGraphicsItem::UserType + 10,
		TypeGroupFinalizationsItem		= QGraphicsItem::UserType + 11,
		TypeGroupConnectionsItem		= QGraphicsItem::UserType + 12,
		TypeParametersFirstBlock		= QGraphicsItem::UserType + 13
	};

	struct EXML_DocVersion //Structure to hold the version information
	{
		int major;
		int minor;
		int version;
		int build;
	};

	typedef struct strcExperimentSubObjectStateHistory
	{
		QList<ExperimentManagerNameSpace::ExperimentSubObjectState> nState;
		QList<QString> sDateTimeStamp;
	} ExperimentSubObjectStateHistory;


	//!  The strcParsedParameterDefinition structure. 
	/*! \struct strcParsedParameterDefinition
	 * A structure for the Experiment Manager parameter definitions.
	 */
	struct strcParsedParameterDefinition
	{
		//! The default constructor of the structure
		strcParsedParameterDefinition()
		{
			sValue = "<undefined>";
			bHasChanged = true;
			bIsInitialized = true;
			bIsCustom = false;
			sLastUsedScriptRef = "";
		}	
		QString sValue;					//!< A String that stores the value of the parameter
		bool bHasChanged;				//!< A Boolean value determining whether the parameter has been changed since it's initialization
		bool bIsInitialized;			//!< A Boolean value determining whether the parameter already has been initialized
		bool bIsCustom;					//!< A Boolean value determining whether the parameter is defined by the is custom (defined by the user)
		QString sLastUsedScriptRef;		//!< A String that stores the script-reference that was last used for resolving the value of the parameter, this value is buffered until changed, if no script-reference was used then this value is an empty string
	};

	/*! Creates a type name for strcParsedParameterDefinition */ 
	typedef strcParsedParameterDefinition ParsedParameterDefinition;

	typedef QHash<QString, ParsedParameterDefinition> tParsedParameterList;

	#define GLWIDGET_BACKGROUNDCOLOR				"backgroundcolor"
	#define GLWIDGET_WIDTH_PIXEL_AMOUNT				"stimuluswidthspan"
	#define GLWIDGET_HEIGHT_PIXEL_AMOUNT			"stimulusheightspan"

	#define QML2VIEWER_BACKGROUNDCOLOR				"backgroundcolor"
	#define QML2VIEWER_WIDTH_PIXEL_AMOUNT			"stimuluswidthspan"
	#define QML2VIEWER_HEIGHT_PIXEL_AMOUNT			"stimulusheightspan"

	#define FUNC_PAINTOBJECT						"paintObject"
	#define FUNC_PAINTOBJECT_FULL					FUNC_PAINTOBJECT "(int,QObject *)"
	#define FUNC_SETOBJECTID						"setObjectID"
	#define FUNC_SETOBJECTID_FULL					FUNC_SETOBJECTID "(int)"
	#define FUNC_INITOBJECT							"initObject"
	#define FUNC_INITOBJECT_FULL					FUNC_INITOBJECT "()"
	#define FUNC_STARTOBJECT						"startObject"
	#define FUNC_STARTOBJECT_FULL					FUNC_STARTOBJECT "()"
	#define FUNC_STOPOBJECT							"stopObject"
	#define FUNC_STOPOBJECT_FULL					FUNC_STOPOBJECT "()"
	#define FUNC_SETEXPERIMENTMANAGER				"setExperimentManager"
	#define FUNC_SETEXPERIMENTMANAGER_FULL			FUNC_SETEXPERIMENTMANAGER "(ExperimentManager *)" 
	//#define FUNC_SETOBJECTCONFIGURATION				"setObjectConfiguration"
	//#define FUNC_SETOBJECTCONFIGURATION_FULL		FUNC_SETOBJECTCONFIGURATION "(QObject *)"
	#define FUNC_INITOBJECTBLOCKTRIAL				"initObjectBlockTrial"
	#define FUNC_INITOBJECTBLOCKTRIAL_FULL			FUNC_INITOBJECTBLOCKTRIAL "()"
	#define FUNC_MAKETHISAVAILABLEINSCRIPT			"makeThisAvailableInScript"
	#define FUNC_MAKETHISAVAILABLEINSCRIPT_FULL		FUNC_MAKETHISAVAILABLEINSCRIPT "(QString,QObject *)"

	#define SIGNAL_USERCLOSE_FULL					"UserWantsToClose(void)"
	#define SIGNAL_OBJECTSTOP_FULL					"ObjectShouldStop(void)"
	#define FUNC_OBJECTSTATECHANGED_FULL			"ObjectStateHasChanged(ExperimentSubObjectState)"
	//#define FUNC_SETBLOCKTRIALDOMNODELIST			"setBlockTrialDomNodeList"
	//#define FUNC_SETBLOCKTRIALDOMNODELIST_FULL	FUNC_SETBLOCKTRIALDOMNODELIST "(QDomNodeList*)"

	#define FUNC_SETMETAOBJECT						"setExperimentMetaObject"
	#define FUNC_SETMETAOBJECT_FULL					FUNC_SETMETAOBJECT "()"
	#define FUNC_SETEXPERIMENTOBJECTID				"setExperimentObjectID"
	#define FUNC_SETEXPERIMENTOBJECTID_FULL			FUNC_SETEXPERIMENTOBJECTID "(int)"
	//#define FUNC_SETEXPERIMENTCONFIGURATION		"setExperimentConfiguration"
	//#define FUNC_SETEXPERIMENTCONFIGURATION_FULL	FUNC_SETEXPERIMENTCONFIGURATION "(ExperimentConfiguration*)"
	#define FUNC_OBJECT_INIT						"initExperimentObject"
	#define FUNC_OBJECT_START						"startExperimentObject"
	#define FUNC_OBJECT_STOP						"stopExperimentObject"
	#define FUNC_OBJECT_ABORT						"abortExperimentObject"
	#define ARCHIVER_NAME							"Archiver"
	//#define RETINOMAP_WIDGET_NAME					"RetinoMap_glwidget"
	//#define QML_WIDGET_NAME						"qmlWidget"
	#define QML2VIEWER_NAME							"QML2Viewer"
	#define TRIGGERTIMER_NAME						"TriggerTimer"
	#define EXPERIMENTTIMER_NAME					"ExperimentTimer"
	#define RANDOMGENERATOR_NAME					"RandomGenerator"
	#define IMAGEPROCESSOR_NAME						"ImageProcessor"
	#define PRTFORMATMANAGER_NAME					"PRTFormatManager"
	#define EXPERIMENTTREEMODEL_NAME				"ExperimentTreeModel"
	#define CEXPERIMENTSTRUCTURE_NAME				"cExperimentStructure"
	#define CEXPERIMENTSTRUCTURESTATE_NAME			"cExperimentStructureState"
	#define CMETHODCONNECTIONSTRUCTURE_NAME			"cMethodConnectionStructure"
	#define CMETHODSTRUCTURE_NAME					"cMethodStructure"
	#define CMETHODPARAMETERSTRUCTURE_NAME			"cMethodParameterStructure"
	#define CBLOCKPARAMETERSTRUCTURE_NAME			"cBlockParameterStructure"
	#define CBLOCKSTRUCTURE_NAME					"cBlockStructure"
	#define COBJECTSTRUCTURE_NAME					"cObjectStructure"
	#define CLOOPSTRUCTURE_NAME						"cLoopStructure"
	#define EXPERIMENTSTRUCTURE_NAME				"ExperimentStructure"
	#define RETINOTOPYMAPPER_NAME					"RetinotopyMapper"
	#define EXPERIMENTMANAGER_NAME					"ExperimentManager"
	#define EXPERIMENTSTATE_NAME					"ExperimentState"
	#define MODULE_NAME								"Experiment Parser"
	#define MAX_INVOKE_ARG_COUNT					10
	//#define MIN_SCREEN_UPDATE_TIME				10
	//#define MAX_SCREEN_UPDATE_TIME				16.6667f

	#define RETINOTOPYMAPPER_PARAMDEF_PATH			":/resources/" + QString(RETINOTOPYMAPPER_NAME).toLower() + ".xdef"
	#define QML2VIEWER_PARAMDEF_PATH				":/resources/" + QString(QML2VIEWER_NAME).toLower() + ".xdef"
	#define EXPERIMENT_PARAMDEF_PATH				":/resources/experimentnode.xdef"
	#define DECLARATIONS_OBJECT_TAG					DECLARATIONS_TAG "_" OBJECT_TAG
	#define DECLARATIONS_OBJECT_PARAMDEF_PATH		":/resources/declarationsobjectnode.xdef"
	#define INITIALIZATION_OBJECT_TAG				INITIALIZATION_TAG "_" OBJECT_TAG
	#define INITIALIZATION_OBJECT_PARAMDEF_PATH		":/resources/initializationobjectnode.xdef"
	#define FINALIZATION_OBJECT_TAG					FINALIZATION_TAG "_" OBJECT_TAG
	#define FINALIZATION_OBJECT_PARAMDEF_PATH		":/resources/finalizationobjectnode.xdef"
	#define CONNECTION_OBJECT_TAG					CONNECTION_TAG "_" OBJECT_TAG
	#define CONNECTION_OBJECT_PARAMDEF_PATH			":/resources/connectionobjectnode.xdef"
	#define BLOCK_OBJECT_PARAMDEF_PATH				":/resources/blocknode.xdef"
	#define LOOP_OBJECT_PARAMDEF_PATH				":/resources/blockloopnode.xdef"
	#define INITIALIZATIONS_PARAMETER_TAG			INITIALIZATIONS_TAG "_" PARAMETER_TAG
	#define FINALIZATIONS_PARAMETER_TAG				FINALIZATIONS_TAG "_" PARAMETER_TAG
	#define INITIALIZATIONS_PARAMETER_PARAMDEF_PATH	":/resources/initializationsparameternode.xdef"
	#define FINALIZATIONS_PARAMETER_PARAMDEF_PATH	INITIALIZATIONS_PARAMETER_PARAMDEF_PATH
	#define BLOCK_PARAMETER_TAG						BLOCK_TAG "_" PARAMETER_TAG
	#define BLOCK_PARAMETER_PARAMDEF_PATH			":/resources/blockparameternode.xdef"
	#define OBJECT_DEFINITION_TAG					OBJECT_TAG "_" DEFINITION_TAG
	#define OBJECT_DEFINITION_PARAMDEF_PATH			":/resources/objectdefinitionnode.xdef"

	#define DEFAULT_OUTPUTFILE				"Out.txt"
	#define ARGB_FORMAT_HEADER				0xCAFE1234
	#define ARGB_FORMAT_HEADER_CONC			0xCAFE5678
	#define DOUBLE_PI_VALUE					3.141592653589793238462643383279502884197
	#define ROOT_TAG						"EXML"
	#define VERSION_TAG						"version"
	#define BOOKMARK_TAG					"bookmark"
	#define ID_TAG							"ID"
	#define OBJECT_TAG						"object"
	#define DEFINITIONS_TAG					"definitions"
	#define DEFINITION_TAG					"definition"
	#define FILE_TAG						"file"
	#define DEFINES_TAG						"defines"
	#define ACTIONS_TAG						"actions"
	#define LOOPS_TAG						"loops"
	#define LOOP_TAG						"loop"
	#define LOOP_NUMBER_TAG					"loop_number"	
	#define LOOP_AMOUNT_TAG					"nr_of_loops"
	#define LOOP_TARGETBLOCKID_TAG			"target_block"
	#define BLOCKTRIALS_TAG					"blocks"
	#define BLOCK_TAG						"block"
	#define PARAMETER_TAG					"parameter"
	#define PARAMETERS_TAG					"parameters"
	#define CUSTOM_PARAMETERS_TAG			"custom_parameters"
	#define BLOCKNUMBER_TAG					"block_number"
	#define TRIALAMOUNT_TAG					"nr_of_trials"
	#define INTERNALTRIGGERAMOUNT_TAG		"nr_of_internal_triggers"
	#define EXTERNALTRIGGERAMOUNT_TAG		"nr_of_external_triggers"
	#define DECLARATIONS_TAG				"declarations"
	#define CONNECTIONS_TAG					"connections"
	#define CONNECTION_TAG					"connection"
	#define INITIALIZATIONS_TAG				"initializations"
	#define INITIALIZATION_TAG				"initialization"
	#define FINALIZATIONS_TAG				"finalizations"
	#define FINALIZATION_TAG				"finalization"
	#define CONNECTION_TYPE_TAG				"type"
	#define CONNECTION_TYPE_SIGNAL_TAG		METHOD_TYPE_SIGNAL_TAG
	#define CONNECTION_TYPE_SLOT_TAG		METHOD_TYPE_SLOT_TAG
	#define CONNECTION_SIGNATURE_TAG		"signature"
	#define CONNECTION_SOURCE_TAG			"source"
	#define CONNECTION_TARGET_TAG			"target"
	#define INIT_FINIT_NUMBER_TAG			"number"
	#define INIT_FINIT_TYPE_TAG				"type"
	#define INIT_FINIT_TYPE_SLOT_TAG		METHOD_TYPE_SLOT_TAG
	#define INIT_FINIT_SIGNATURE_TAG		"signature"
	#define MEMBER_TYPE_TAG					"type"
	#define MEMBER_TYPE_SLOT_TAG			METHOD_TYPE_SLOT_TAG
	#define DEBUGMODE_TAG					"debugmode"
	#define EXPERIMENT_TAG					"experiment"
	#define CLASS_TAG						"class"
	#define NAME_TAG						"name"
	#define VALUE_TAG						"value"
	#define HREF_TAG						"href"
	#define FOLDER_TAG						"folder"
	#define TITLE_TAG						"title"
	#define FOLDED_TAG						"folded"
	#define EVENT_TAG						"event"
}

#endif //EXPMANAGER_GLOBAL_H
