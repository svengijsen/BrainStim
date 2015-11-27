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

#ifndef RETINOTOPYMAPPER_H
#define RETINOTOPYMAPPER_H

#include "experimentengine.h"
#include "retinotopymapperwindow.h"

//Below defines must be all in lower case!

/*!
 * \defgroup RetinoUniversalParamDefs Retinotopic (Universal) parameter Names
 * @{
 */
#define RETINOMAPPER_STIMULI_REFRESHRATE				"stimulirefreshrate"			/*!< "stimulirefreshrate" */
#define RETINOMAPPER_PATTERN_POLARANGLE					"polarangle"					/*!< "polarangle" */
#define RETINOMAPPER_PATTERN_ECCENTRICITY				"eccentricity"					/*!< "eccentricity" */
#define RETINOMAPPER_PATTERN_MOVINGBAR					"movingbar"						/*!< "movingbar" */
#define RETINOMAPPER_PATTERN_MOVINGDOTS					"movingdots"					/*!< "movingdots" */
#define RETINOMAPPER_PATTERN_FIXATION					"fixation"						/*!< "fixation" */
#define RETINOMAPPER_PATTERN_CUSTOM						"custom"						/*!< "custom" */
#define RETINOMAPPER_PATTERN							"retinopattern"					/*!< "retinopattern" */
#define RETINOMAPPER_CYCLE_TRIGGER_AMOUNT				"cycletriggeramount"			/*!< "cycletriggeramount" */
#define RETINOMAPPER_SHOWFIXPOINT						"showfixpoint"					/*!< "showfixpoint" */
#define RETINOMAPPER_FIXSIZE							"fixationsize"					/*!< "fixationsize" */
#define RETINOMAPPER_POS_LEFT							"left"							/*!< "left" */
#define RETINOMAPPER_POS_RIGHT							"right"							/*!< "right" */
#define RETINOMAPPER_POS_BOTH							"both"							/*!< "both" */
#define RETINOMAPPER_GAP_DIAMETER						"gapdiameter"					/*!< "gapdiameter" */
#define RETINOMAPPER_FLICKRSPEED_HZ						"flickrfrequency"				/*!< "flickrfrequency" */
#define RETINOMAPPER_CORTMAG_FACTOR						"corticalmagnitudefactor"		/*!< "corticalmagnitudefactor" */
#define RETINOMAPPER_TRIGGERDURATION_MSEC				"internaltriggerduration"		/*!< "internaltriggerduration" */
#define RETINOMAPPER_DISCRETETRIGGERSTEPS				"discretetriggersteps"			/*!< "discretetriggersteps" */
#define RETINOMAPPER_RANDOMIZETRIGGERSTEPS				"randomizetriggersteps"			/*!< "randomizetriggersteps" */
#define RETINOMAPPER_RANDOMIZETRIGGERSTEPS_ARRAY		"randomizetriggerstepsarray"	/*!< "randomizetriggerstepsarray" */
#define RETINOMAPPER_EMPTYTRIGGERSTEPS					"emptytriggersteps"				/*!< "emptytriggersteps" */
#define RETINOMAPPER_EMPTYTRIGGERSTEPS_ARRAY			"emptytriggerstepsarray"		/*!< "emptytriggerstepsarray" */
#define RETINOMAPPER_OUTPUTTRIGGERFRAME					"outputtriggerframe"			/*!< "outputtriggerframe" */
#define RETINOMAPPER_OUTPUTFRAMETYPE					"outputframetype"				/*!< "outputframetype" */
#define RETINOMAPPER_OUTPUTTYPE_FRAME					"frame"							/*!< "frame" */
#define RETINOMAPPER_OUTPUTTYPE_MASK					"mask"							/*!< "mask" */
#define RETINOMAPPER_OUTPUTFRAMEFORMAT					"outputframeformat"				/*!< "outputframeformat" */
#define RETINOMAPPER_OUTPUTFORMAT_PNG					"png"							/*!< "png" */
#define RETINOMAPPER_OUTPUTFORMAT_DAT					"dat"							/*!< "dat" */
#define RETINOMAPPER_OUTPUTFORMAT_CDAT					"cdat"							/*!< "cdat" */
#define RETINOMAPPER_ANTIALIASING						"antialiasing"					/*!< "antialiasing" */
#define RETINOMAPPER_DISABLECORTMAGFAC					"disablecortmagfac"				/*!< "disablecortmagfac" */
#define RETINOMAPPER_CHECKERCOLOR1						"checkercolor1"					/*!< "checkercolor1" */
#define RETINOMAPPER_CHECKERCOLOR2						"checkercolor2"					/*!< "checkercolor2" */
#define RETINOMAPPER_FIXATIONCOLOR						"fixationcolor"					/*!< "fixationcolor" */
/**@}*/
/*!
 * \defgroup RetinoPolarAngleParamDefs Retinotopic (PolarAngle) parameter Names
 * @{
 */
#define RETINOMAPPER_POLAR_RING_AMOUNT					"polarringamount"				/*!< "polarringamount" */
#define RETINOMAPPER_POLAR_ROTATION_DIRECTION			"polarrotationdirection"		/*!< "polarrotationdirection" */
#define RETINOMAPPER_POLAR_WEDGE_SPAN					"polarwedgespan"				/*!< "polarwedgespan" */
#define RETINOMAPPER_POLAR_CHECK_AMOUNT					"polarcheckamount"				/*!< "polarcheckamount" */
/**@}*/
/*!
 * \defgroup RetinoEccentricityParamDefs Retinotopic (Eccentricity) parameter Names
 * @{
 */
#define RETINOMAPPER_ECCENTRICITY_DIRECTION				"eccentricitydirection"			/*!< "eccentricitydirection" */
#define RETINOMAPPER_ECCENTRICITY_CHECK_AMOUNT			"eccentricitycheckamount"		/*!< "eccentricitycheckamount" */
#define RETINOMAPPER_ECCENTRICITY_RING_AMOUNT			"eccentricityringamount"		/*!< "eccentricityringamount" */
#define RETINOMAPPER_ECCENTRICITY_ENABLE_CORTMAG_TIMING	"enablecortmagtiming"			//Not documented!
/**@}*/
/*!
 * \defgroup RetinoMovingBarParamDefs Retinotopic (MovingBar) parameter Names
 * @{
 */
#define RETINOMAPPER_MOVINGBAR_ANGLE					"movingbarangle"				/*!< "movingbarangle" */
#define RETINOMAPPER_MOVINGBAR_COVERAGE					"movingbarcoverage"				/*!< "movingbarcoverage" */
#define RETINOMAPPER_MOVINGBAR_HEIGHT					"movingbarheight"				/*!< "movingbarheight" */
#define RETINOMAPPER_MOVINGBAR_DIRECTION				"movingbardirection"			/*!< "movingbardirection" */
#define RETINOMAPPER_MOVINGBAR_HEIGTH_CHECK_AMOUNT		"movingbarheightcheckamount"	/*!< "movingbarheightcheckamount" */
//MovingDots specific defines
#define	RETINOMAPPER_MOVINGDOTS_COLOR					"movingdotscolor"				/*!< "movingdotscolor" */
#define	RETINOMAPPER_MOVINGDOTS_MIN_MOVESPEED			"movingdotsminmovespeed"		/*!< "movingdotsminmovespeed" */
#define	RETINOMAPPER_MOVINGDOTS_MAX_MOVESPEED			"movingdotsmaxmovespeed"		/*!< "movingdotsmaxmovespeed" */
#define	RETINOMAPPER_MOVINGDOTS_MIN_MOVEANGLE			"movingdotsminmoveangle"		/*!< "movingdotsminmoveangle" */
#define	RETINOMAPPER_MOVINGDOTS_MAX_MOVEANGLE			"movingdotsmaxmoveangle"		/*!< "movingdotsmaxmoveangle" */
#define RETINOMAPPER_MOVINGDOTS_NROFDOTS				"movingdotsnrofdots"			/*!< "movingdotsnrofdots" */
#define RETINOMAPPER_MOVINGDOTS_DOTSIZE					"movingdotsdotsize"				/*!< "movingdotsdotsize" */
#define RETINOMAPPER_MOVINGDOTS_HEMIFIELD				"movingdotshemifield"			/*!< "movingdotshemifield" */
#define RETINOMAPPER_MOVINGDOTS_PIXELFROMCENTER			"movingdotspixelfromcenter"		/*!< "movingdotspixelfromcenter" */
#define RETINOMAPPER_MOVINGDOTS_STATIONAIRY				"movingdotsstationairy"			/*!< "movingdotsstationairy" */
#define RETINOMAPPER_MOVINGDOTS_HEMIFIELDWIDTH			"movingdotshemifieldwidth"		/*!< "movingdotshemifieldwidth" */
#define RETINOMAPPER_MOVINGDOTS_HEMIFIELDHEIGHT			"movingdotshemifieldheight"		/*!< "movingdotshemifieldheight" */
/**@}*/

#define RETINOMAPPER_OUTPUT_SUBFOLDER					"/RetinotopyMapper/"

typedef struct strcMovingDotsStructure
{
	QPolygonF Pos; //A QPolygonF is a QVector<QPointF>. The easiest way to add points to a QPolygonF is to use its streaming operator: polygon << QPointF(10.4, 20.5) << QPointF(20.2, 30.2);
	QPolygonF OldPos;
	QList<QLineF> Mov;
} MovingDotsStructure;

class ExperimentEngine;

//!  The RetinotopyMapper class. 
/*!
  The RetinotopyMapper class can be used in combination with the ExperimentManager for presenting 
  various Retinotopy Mapping stimuli.
*/
class RetinotopyMapper : public ExperimentEngine
{
	Q_OBJECT

public:
	friend class RetinotopyMapperWindow;

	RetinotopyMapper(QObject *parent = NULL);
	~RetinotopyMapper();
	RetinotopyMapper(const RetinotopyMapper& other ){Q_UNUSED(other);};

	static QScriptValue ctor__RetinotopyMapper(QScriptContext* context, QScriptEngine* engine);

public slots:
	bool initObject();
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script
	bool startObject();
	//bool setObjectID(int nObjID);
	bool initObjectBlockTrial();
	//bool paintObject(int paintFlags = 0, QObject *paintEventObject = NULL);
	bool installCustomScriptHandlerFunction(QString FuncName);
	bool setExperimentManager(ExperimentManager *expManager);
	bool setExperimentObjectReadyToUnlock();
	QScreen* getActiveStimuliOutputScreen();
	bool setActiveStimuliOutputScreen(int nScreenNumber);
	bool setCustomOutputDirectoryPath(const QString &sOutputDirectory);

protected:
	bool eventFilter(QObject *target, QEvent *event);

private:
	void initialize();
	void parseExperimentObjectBlockParameters(bool bInit = false);
	void initializeMovingDotsStructures();
	QString booleanToParamString(bool bValue);
	bool paramStringToBool(QString sValue);
	void adjustStimScreenArea();

	QScriptValue *customScriptHandlerFunction;
	RetinotopyMapperWindow *retinoMapperWindow;
	int nRefreshRate;
	//int nRetinoID;								//This variable stores the ObjectID used to identify the object
	QString currentExpType;						//The experiment type used, see RetinoMapExperimentType
	bool firstBlockTrialPaintFrame;				//To determine whether it's the first frame to paint from a new Block trial
	QColor colorBackground;						//The color of the background
	QBrush brushBackground;						//The background brush
	QFont textFont;								//The font used for text(debug mode)
	QPen textPen;								//The pen used for text(debug mode)
	QColor whiteColor;							//The color used for the activation map (active)
	QColor blackColor;							//The color used for the activation map (inactive)
	QString textContent;						//Variable to store the text string
	QRectF rectScreenRes;						//The screen resolution
	float flickrThreshold;						//Variable for keeping track of the next flickr switch threshold time
	float flickrSpeedFreq;						//The flickr speed frequency
	int flickrSwitch;							//Variable to store the current flickr state
	int cycleTriggerAmount;						//The total block duration in amount of triggers
	int triggerDurationMsec;					//The duration of one trigger in mSec
	float stimWidthPixelAmount;					//The amount of visible stimuli pixels(height)
	float stimHeightPixelAmount;				//The amount of visible stimuli pixels(width)
	QColor fixationColor;						//The color of the fixation dot/cross
	int fixationSize;							//The width of the fixation dot/cross
	bool showFixationPoint;						//Should we show an fixation point?
	bool outputTriggerFrame;					//Defines whether we should output(write to a *.png file) the first frame
	bool discreteTriggerSteps;					//Defines whether the stimuli should run smooth or in discrete steps according to each trigger received
	bool randomizeTriggerSteps;					//Defines whether the stimuli should randomize after each trigger received
	bool antiAliasing;							//Defines whether the anti-aliasing filter should be used for painting to the screen
	//int lastTriggerNumber;						//To keep track of the last trigger number
	int emptyTriggerSteps;						//Defines the number of Trigger steps in which no stimuli should be presented
	QStringList emptyTriggerStepsArray;
	QStringList randomizeTriggerStepsArray;
	bool bNoChangesSinceLastFrame;
	ExperimentManager *experimentManager;
	QString sCustomOutputDirectoryPath;

	QColor movingDotsColor;
	QColor cCheckerColor1;
	QColor cCheckerColor2;
	Qt::PenStyle style;
	Qt::PenCapStyle flatCap;
	Qt::PenCapStyle roundCap;
	QString tmpParamValue;
	//float fStimulusDiameter;
	double dStimulusDiameter;
	float fTrialTimeProgress;
	float fCortMagTrialTimeProgress;
	bool bCreateActivationMap;
	int currExpBlockTrialCycle;
	bool nextNewCycleEntered;
	bool nextNewBlockEntered;
	float cortMagFactor;
	int gapDiameter;
	float wedgeSpanAngle;
	float currentWedgeDiameter;
	float currentCompleteWedgeDiameter;
	//float currentSize;
	double dCurrentSize;
	float currentOuterCompleteRingDiameter;
	float currentXPoint;
	float currentYPoint;
	float currentStartAngle;
	bool disableCortMagFac;
	bool enableCortMagTiming;
	int emptyTriggerLastIndex;
	int emptyTriggerStepCount;
	bool bAllTrialEmptyProcessed;
	//bool allEmptyBlockTrialsProcessed;

	//Polar
	float polarWedgeSpan;
	int polarWedgeNrChecks;
	int polarWedgeNrRings;
	int polarRotationDirection;

	//Eccentricity
	int eccentricityNrChecks;
	int eccentricityNrRings;
	int eccentricityDirection;
	float fCalculatedCortMagFacTimingConst;

	//MovingBar
	float movingBarAngle;
	float movingBarCoverage;
	float movingBarHeight;
	int movingBarWidthCheckAmount;
	int movingBarHeightCheckAmount;
	int movingBarDirection;

	float movingDotsMinMoveSpeed;
	float movingDotsMaxMoveSpeed;
	int movingDotsMinMoveAngle;
	int movingDotsMaxMoveAngle;
	int movingDotsDotSize;
	QLineF movingDotsFirstHemiVisibleArea; //The absolute area of the first visible hemisphere from the left-top to the right-bottom
	int movingDotsNrOfDots;
	bool movingDotsIsStationary;
	QString movingDotsHemifieldPos;
	int movingDotsPixelFromCenter;
	int movingDotsHemiFieldWidth;
	int movingDotsHemiFieldHeight;
	MovingDotsStructure movingDots;
	QString retinoOutputType;
	QString retinoOutputFormat;
	QPixmap StimulusResultImageFrame;
	QPixmap StimulusActivationMap;
	RandomGenerator *randStimStateGenerator;		 //To hold the Stimuli Trigger Step
	RandomGenerator *randEmptyStimGenerator;		 //To hold the Empty Block Trials
	RandomGenerator *previousRandEmptyStimGenerator; //To hold the processed Empty Block Trials
	QScriptValue scriptVal1;
	QScriptValue scriptVal2;
	QRect rStimuliScreenArea;

	double dAdditionalRefreshDelayTime;
	double dWaitTime;
	//double dLastPreSwapTime;
};

Q_DECLARE_METATYPE(RetinotopyMapper)
Q_DECLARE_METATYPE(RetinotopyMapper*)
Q_DECLARE_METATYPE(QPixmap*)
Q_DECLARE_METATYPE(QPainter*)

#endif // RETINOTOPYMAPPER_H
