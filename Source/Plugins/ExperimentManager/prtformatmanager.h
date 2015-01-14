//PrtFormatManager
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

#ifndef PRTFORMATMANAGER_H
#define PRTFORMATMANAGER_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QtScript>
#include <QScriptable>
#include <QColor>

/*! The namespace for the PrtFormat Manager */
namespace PrtFormatManagerNameSpace
{
/*!
 * \defgroup PrtFormatManagerDefines PrtFormat Manager Definitions
 * @{
 */
	#define PRT_DEFAULT_VERSION				2							/*!< Default PRT Document Type */
	#define PRT_UNDEF_VALUE_STRING			"<undefined value>"			/*!< "<undefined value>" */ 
	#define PRT_UNDEF_DESC_STRING			"<undefined description>"   /*!< "<undefined description>" */
	#define PRT_UNDEF_NAME_STRING			"<undefined name>"			/*!< "<undefined name>" */
	#define PRT_UNDEF_COLOR_STRING			"192 192 192"				/*!< "192 192 192" */
	#define PRT_LAST_PARAM_NAME				"NrOfConditions"
	#define PRT_COLOR_PARAM_NAME			"Color"
/**@}*/

	typedef struct strcPRTParameters
	{
		strcPRTParameters()
		{
			sValue =						PRT_UNDEF_VALUE_STRING;
			sDescription =					PRT_UNDEF_DESC_STRING;
		}
		QString sValue;
		QString sDescription;
	} PRTDefinitionParameters;
	typedef QHash<QString, PRTDefinitionParameters> PRTDefinitionParameterList;

	typedef struct strcIntervals
	{
		strcIntervals()
		{
			nBegin =						0;
			nEnd =							1;
		}
		int nBegin;
		int nEnd;
	} IntervalsDefinition;

	typedef struct strcConditions
	{
		strcConditions()
		{
			sName =							PRT_UNDEF_NAME_STRING;
			sColor =						PRT_UNDEF_COLOR_STRING;
		}
		QString sName;
		QString sColor;
		QList<IntervalsDefinition> Intervals;
	} ConditionsDefinition;

	typedef QList<ConditionsDefinition> PRTConditionsDefinitionList;
}

using namespace PrtFormatManagerNameSpace;

//!  The PrtFormatManager class. 
/*!
  The PrtFormat Manager can be used to create and edit stimulation protocol (*.prt) files.
  See also http://www.brainvoyager.de/BV2000OnlineHelp/BrainVoyagerWebHelp/mergedProjects/FileFormats/The_format_of_PRT_files.htm
*/
class PrtFormatManager : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_CLASSINFO("ScriptAPIClassName", "PrtFormatManager");//Can't use defines here!, moc doesn't handle defines, not needed here

public:
	PrtFormatManager(QObject *parent = NULL);
	PrtFormatManager(const PrtFormatManager& other ){Q_UNUSED(other);}//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	~PrtFormatManager();

	static QScriptValue ctor__PrtFormatManager(QScriptContext* context, QScriptEngine* engine);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script
	/*! \brief Loads an stimulation protocol (*.prt) file into memory.
	 *
	 *  This function can loads an existing stimulation protocol (*.prt) file into memory for further editing.
	 * @param sFileName a String containing the path to the stimulation protocol (*.prt) file.
	 * @return a Boolean value representing whether the stimulation protocol (*.prt) file could be loaded.
	 */
	bool loadFile(const QString sFileName);
	/*! \brief Saves an stimulation protocol from memory to a stimulation protocol(*.prt) file.
	 *
	 *  This function can save the current  an existing stimulation protocol file in memory to a file.
	 * @param sFileName a String containing the path to the target stimulation protocol (*.prt) file.
	 * @param bOverWrite a Boolean value determing whether the target file should be automatically overwritten when it already exists.
	 * @return a Boolean value representing whether the stimulation protocol could be saved to the file.
	 */
	bool saveFile(const QString sFileName, const bool bOverWrite = false);
	/*! \brief Sets an Parameter value for the stimulation protocol.
	 *
	 *  This function sets a Parameter value for the current stimulation protocol in memory.
	 * @param sParamName a String containing the Parameter Name.
	 * @param sParamValue a String containing the Parameter Value.
	 * @param sParamDescription a String containing the Parameter Description.
	 * @return a Boolean value representing whether the Parameter value for the current stimulation protocol could be set.
	 */
	bool setParameterValue(const QString sParamName, const QString sParamValue = PRT_UNDEF_VALUE_STRING, const QString sParamDescription = PRT_UNDEF_DESC_STRING);
	/*! \brief Returns a Parameter value of the stimulation protocol.
	 *
	 *  This function can return a Parameter value of the current stimulation protocol in memory.
	 * @param sParamName a String containing the requested Parameter's Name.
	 * @return a String value holding the requested Parameter value of the current in-memory stimulation protocol. 
	 *  If no parameter with the defined Name could be found then this function will return a \ref PrtFormatManagerDefines::PRT_UNDEF_VALUE_STRING.
	 */	
	QString getParameterValue(const QString sParamName);
	/*! \brief Returns a Parameter description of the stimulation protocol.
	*
	*  This function can return a Parameter description of the current stimulation protocol in memory.
	* @param sParamName a String containing the requested Parameter's Name.
	* @return a String value holding the requested Parameter description of the current in-memory stimulation protocol. 
	*  If no parameter with the defined Name could be found then this function will return a \ref PrtFormatManagerDefines::PRT_UNDEF_DESC_STRING.
	*/	
	QString getParameterDescription(const QString sParamName);
	/*! \brief Clears all Conditions of the stimulation protocol.
	*
	*  This function clears all Conditions of the current stimulation protocol in memory.
	* @return a Boolean value representing whether the Conditions could be cleared.
	*/	
	bool clearConditions();
	/*! \brief Clears all Parameters of the stimulation protocol.
	*
	*  This function clears all Parameters of the current stimulation protocol in memory.
	* @return a Boolean value representing whether the Parameters could be cleared.
	*/	
	bool clearParameters();
	/*! \brief Clears all Parameters and Conditions of the stimulation protocol.
	*
	*  This function clears all Parameters and Conditions of the current stimulation protocol in memory.
	*  See also PrtFormatManager::clearConditions() and PrtFormatManager::clearParameters().
	* @return a Boolean value representing whether the Parameters and the Conditions could be cleared.
	*/	
	bool clearAll(); 
	/*! \brief Appends a new Condition to the in-memory stimulation protocol.
	*
	*  This function can append a new Condition to the current stimulation protocol in memory.
	* @param sConditionName a String containing the new Conditions Name.
	*  If no Condition Name is defined than the default \ref PrtFormatManagerDefines::PRT_UNDEF_NAME_STRING is used. 
	* @param sConditionColor a String containing the new Conditions Color attribute. The format must be "R G B", Example: "255 0 0".
	*  See also PrtFormatManager::getRGBPaletteColorString().
	*  If no Condition Color is defined than the default \ref PrtFormatManagerDefines::PRT_UNDEF_COLOR_STRING is used.
	* @return an integer value holding the new internal index of the appended Condition. 
	*  See also PrtFormatManager::appendInterval().
	*/	
	int appendCondition(const QString sConditionName = PRT_UNDEF_NAME_STRING, const QString sConditionColor = PRT_UNDEF_COLOR_STRING);
	/*! \brief Appends a Interval to an existing Condition to the in-memory stimulation protocol.
	*
	*  This function can append a Interval to an existing Condition to the current in-memory stimulation protocol.
	* @param nConditionIndex an integer value containing the index of the Condition to which the Interval should be appended.
	* @param nBegin an integer value specifying the Start of the first piece of time belonging to the Interval that should be appended.
	* @param nEnd an integer value specifying the End of the first piece of time belonging to the Interval that should be appended.
	* @return an integer value holding the new internal index of the appended Interval. 
	*  See also PrtFormatManager::appendCondition().
	*/	
	int appendInterval(const int nConditionIndex = 0, const int nBegin = 0, const int nEnd = 1);
	/*! \brief Returns a specific color ("R G B" string).
	*
	*  Returns a Color String from a location of a color palette divided in steps.
	* @param nSteps an integer representing the number of steps the color palette should be divided to.
	* @param nIndex an integer representing the index location (0-based) of color at the divided color palette.
	* @return a String (a "R G B" String value) holding the Color value.
	* See also PrtFormatManager::appendCondition();
	*/	
	QString getRGBPaletteColorString(const int nSteps, const int nIndex);
	/*! \brief sets a specific color for a condition ("R G B" string).
	*
	*  Sets a Color String for a specific condition.
	* @param nConditionIndex an integer representing the index of the condition to set the color from.
	* @param sColor a String (a "R G B" String value) holding the Color value for the condition.
	* @return a Boolean value determining whether the color value for that condition could be successfully changed.
	* See also PrtFormatManager::appendCondition();
	*/		
	bool setConditionColor(const int nConditionIndex, const QString sColor);
	/*! \brief Sorts a Merges all the Condition Intervals of the current Stimulation Protocol.
	*
	*  This function tries to Sort and Merge the Condition Intervals of the current in-memory Stimulation Protocol.
	*  After editing a Stimulation Protocol this function could be called prior to the PrtFormatManager::saveFile() to optimize the protocol structure.
	* @return a Boolean value representing whether the Condition Intervals could be parsed.
	*/	
	bool SortandMergeAllConditionIntervals();

private:
	bool setDefaultParameters(const int nVersion = PRT_DEFAULT_VERSION);
	PRTDefinitionParameters getParameter(const QString sParamName);

	QScriptEngine* currentScriptEngine;

	PRTDefinitionParameterList m_PRTParameters;
	PRTConditionsDefinitionList m_PRTConditions;
	QStringList m_AllowedParameterNames;

	IntervalsDefinition tmpIntervalsDefinition;
	ConditionsDefinition tmpConditionsDefinition;
	PRTDefinitionParameters tmpPRTDefinitionParameters;
};

Q_DECLARE_METATYPE(PrtFormatManager*)

#endif // PRTFORMATMANAGER_H
