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


#ifndef QML2VIEWER_H
#define QML2VIEWER_H

#include <QQmlContext>
#include "qtquick2applicationviewer.h"
#include "experimentengine.h"
#include "imagelistmodel.h"

#define QML2VIEWER_MAINFILEPATH							"qmlmainfilepath"
#define QML2VIEWER_MAX_EVENT_TIME						"qmlmineventtime"

//!  The QML2Viewer class. 
/*!
  The QML2Viewer class can be used in combination with the ExperimentManager for presenting visual or 
  other kind of stimuli using QML/QtQuick.
*/
class QML2Viewer : public ExperimentEngine
{
	Q_OBJECT

signals:
	//! The NewSourceLoaded Signal.
	/*!
		You can use this Signal to detect whenever internal QML2 Engine loads a new source.
		@param sFileURL a string containing the URL of the loaded file.
	*/
	void NewSourceLoaded(QString sFileURL);

public:
	QML2Viewer(QObject *parent = NULL);
	~QML2Viewer();
	QML2Viewer(const QML2Viewer& other){Q_UNUSED(other);};//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro
	static QScriptValue ctor_QML2Viewer(QScriptContext* context, QScriptEngine* engine);

	QScreen *grabScreenUnderMouseCursor();
	ExperimentManager *getExperimentManager() { return experimentManager; };
	QString getCurrentFilePath() { return qmlMainFilePath; };
	
public slots:

	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

	/*! \brief Executes a QML document.
	*
	*  This function can execute a QML document.
	* @param strSource the path to a QML file or the QML code itself.
	* @param bIsFile a boolean value determining whether the @param strSource is a path to a QML file, if false then the @param strSource is interpreted as QML code.
	* @return a boolean value determining whether the function could be executed successfully.
	*/
	bool executeQML2Document(const QString &strSource, bool bIsFile = true);	
	
	bool initObject();
	
	bool startObject();
	
	bool stopObject();
	
	bool setObjectID(int nObjID);
	
	bool initObjectBlockTrial();
	
	/*! \brief Invokes a QML Method.
	*
	*  This function can invoke/call a defined function within the current QML context.
	* @param strRootObjectName the name of the root item containing the function definition in the QML context.
	* @param strMethodName the name of the function to be called
	* @param inputValue1 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue2 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue3 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue4 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue5 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue6 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue7 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue8 - a parameters that can be automatically passed to the function if it is provided.
	* @param inputValue9 - a parameters that can be automatically passed to the function if it is provided.
	* @return a QVariant containing the invoked function return value converted to a QVariant.
	*/
	QVariant invokeQml2Method(QString strRootObjectName, QString strMethodName, QVariant inputValue1 = QVariant(), QVariant inputValue2 = QVariant(), QVariant inputValue3 = QVariant(), QVariant inputValue4 = QVariant(), QVariant inputValue5 = QVariant(), QVariant inputValue6 = QVariant(), QVariant inputValue7 = QVariant(), QVariant inputValue8 = QVariant(), QVariant inputValue9 = QVariant());	
	/*! \brief Adds a QPixmap to the internal Image Buffer.
	*
	*  This function can add a QPixmap to the internal Image Buffer.
	*  These images can then be rapidly presented because they are already loaded in memory.
	* @param pixmap the QPixmap to be added to the internal Image Buffer.
	* @return a Unique string created by the Image Buffer holding a key to access the newly added image.
	* See QML2Viewer::getPixmapFromImageBuffer and QML2Viewer::updatePixmapFromImageBuffer.
	*/	
	QString addPixmapToImageBuffer(const QPixmap &pixmap);
	/*! \brief Retrieves a QPixmap to the internal Image Buffer.
	*
	*  This function can retrieve a QPixmap to the internal Image Buffer.
	* @param pixmap the retrieved QPixmap from the internal Image Buffer.
	* @param ID the String containing the unique String previously created by the Image Buffer.
	* @return a boolean value representing whether the function executed successfully.
	* See QML2Viewer::addPixmapToImageBuffer.
	*/	
	bool getPixmapFromImageBuffer(QPixmap *pixmap, const QString &ID);
	/*! \brief Updates a QPixmap inside the internal Image Buffer.
	*
	*  This function can update a QPixmap that is stored in the internal Image Buffer.
	* @param pixmap the new QPixmap to which the stored QPixmap inside the Image Buffer should update to.
	* @param ID the String containing the unique String previously created by the Image Buffer.
	* @return a boolean value representing whether the function executed successfully.
	* See QML2Viewer::addPixmapToImageBuffer.
	*/	
	bool updatePixmapFromImageBuffer(QPixmap *pixmap, const QString &ID);
	/*! \brief Updates the ID string of a corresponding QPixmap inside the internal Image Buffer.
	*
	*  This function can update the ID string of a corresponding QPixmap that is stored in the internal Image Buffer.
	* @param oldID the String containing the current ID String, created by the Image Buffer, that should change.
	* @param newID the String containing the new unique ID string.
	* @return a boolean value representing whether the function could execute successfully.	
	*/	
	bool updatePixmapIDFromImageBuffer(const QString &oldID, const QString &newID);
	/*! \brief Removes a QPixmap inside the internal Image Buffer.
	*
	*  This function can remove a QPixmap that is stored in the internal Image Buffer.
	* @param ID the String containing the unique String previously created by the Image Buffer.
	* @return a boolean value representing whether the function executed successfully.
	* See QML2Viewer::addPixmapToImageBuffer.
	*/	
	bool removePixmapFromImageBuffer(const QString &ID);

	bool setExperimentManager(ExperimentManager *expManager);
	
	bool setExperimentObjectReadyToUnlock();
	
	QScriptValue getWindow();

	QScreen* getActiveStimuliOutputScreen();
	bool setActiveStimuliOutputScreen(int nScreenNumber);
	/*! \brief Sets the presentation window mode.
	*
	*  This function sets the mode for the presentation window.
	*  It can been shown either full-screen where the root object defined in the QML file is automatically resized to the full-screen window size or
	*  in a windowed mode, see also QML2Viewer::setTopLeftWindowPosition, where the size of the window is automatically sized to the size of the root object defined in the QML file.
	* @param bIsFullScreen a boolean value specifying whether the mode for the presentation window should be full-screen (=default mode).
	* @param bIsFrameless a boolean value specifying whether the presentation window should be displayed without a frame (=default mode).
	* See QML2Viewer::setTopLeftWindowPosition.
	*/
	void setWindowMode(const bool &bIsFullScreen, const bool &bIsFrameless = true);
	/*! \brief Sets the top-left position of the presentation window and allows to specify whether this window should be a frame-less window or not.
	*
	*  This function sets the top-left position of the presentation window and allows to specify whether this window should be a frame-less window or not.
	*  this function can only make a difference in a non full-screen presentation window mode, see QML2Viewer::setWindowMode.
	* @param nXposition a integer value specifying the top-left X position of the presentation window.
	* @param nYposition a integer value specifying the top-left Y position of the presentation window.
	* See QML2Viewer::setWindowMode.
	*/
	void setTopLeftWindowPosition(const int &nXposition, const int &nYposition);

protected:
	bool eventFilter(QObject *target, QEvent *event);

private slots:
	void onStatusChanged(QQuickView::Status status);
	void onQuick2ViewWindowClosed();
	void onStartTriggerRecieved();

private:
	void initialize();
	void parseExperimentObjectBlockParameters(bool bInit = false, bool bSetOnlyToDefault = false);
	bool qml2EventRoutine(QString strContent = "");
	void deleteQML2ViewerWindow();

	ExperimentManager *experimentManager;
	cExperimentStructure *currentExperimentStructure;
	QFile tmpFile;
	ImageListModel *imgLstModel;
	QtQuick2ApplicationViewer *quick2ViewerWindow;
	QString qmlMainFilePath;
	QString last_qmlMainFilePath;
	QObject *rootObject;
	int nQML2ViewerID;							//The ObjectID used to identify the object
	QRectF rectScreenRes;						//The screen resolution
	QColor colorBackground;						//The color of the background
	float stimWidthPixelAmount;					//The amount of visible stimuli pixels(height)
	float stimHeightPixelAmount;				//The amount of visible stimuli pixels(width)
	bool bParameterChanged;
	bool bFirstQuickWindowActivation;
	bool bExperimentUnlocked;
	bool bFullScreenMode;
	bool bIsFramelessWindow;
	QPoint pointWindowStartPosition;
};
Q_DECLARE_METATYPE(QML2Viewer)
Q_DECLARE_METATYPE(QML2Viewer*)
Q_DECLARE_METATYPE(QWindow*)

#endif // QML2VIEWER_H
