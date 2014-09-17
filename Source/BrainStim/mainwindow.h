//BrainStim
//Copyright (C) 2014  Sven Gijsen
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


/****************************************************************************
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#define QT_NO_PLUGIN_CHECK

#ifndef QT_WIDGETS_LIB
#define QT_WIDGETS_LIB
#endif

#define MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME		"Default"

#include <QString>
#include <QStringList>
#include <QDir>
#include <QtWidgets>
#include <QTime>
#include <QDateTime> //QDateTime::currentDateTime().toString(MainAppInfo::stdDateTimeFormat())!!!!!
#include <QtNetwork\QNetworkInterface>
#include <QtTest\QTest>
#include <QtTest\QTestEventList>

#include "documentwindow.h"
#include "documentmanager.h"
#include "plugininterface.h"
#include "plugincollection.h"
#include "qtscriptengine.h"
#include "mainappinfo.h"
#include "sciFindDialog.h"
#include "assistant.h"
#include "NetworkServer.h"

class SvgView;
class DeviceControl;

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsRectItem;
class QSplashScreen;
QT_END_NAMESPACE

typedef struct QScriptContextStructure
{
	QScriptValue activationObject;
	QScriptValue thisObject;
	QString sContextName;
} QScriptContextStrc;

struct QPairFirstComparer
{
	template<typename T1, typename T2>
	bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
	{
		return a.first < b.first;
	}
};

struct QPairSecondComparer
{
	template<typename T1, typename T2>
	bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
	{
		return a.second < b.second;
	}
};

//!  The BrainStim Main Application. 
/*!
  The BrainStim Main Application can be directly accessed within the script by using the default 'BrainStim' object.
*/
class MainWindow : public DocumentWindow//QMainWindow
{
    Q_OBJECT
	Q_CLASSINFO("ScriptAPIClassName", "BrainStim");//Can't use defines here!, moc doesn't handle defines

public:
	MainWindow();
	~MainWindow();//see void MainWindow::closeEvent(QCloseEvent *event)!

signals:
	void CleanUpScriptExecuted();
	//void NetworkDataAvailable(QString);
		
public slots:
	/*! \brief Executes and returns a result from a provided Javascript function that is embedded inside the BrainStim application.
	 *
	 *  This function executes and returns a result from a provided Javascript function that is embedded inside the BrainStim application.
	 *  The embedded available Javascript function calls provide a way to request all kind of application and plugin (version) information.
	 *  The menu command 'Help' --> 'BrainStim History' can also be used to retrieve this kind of information, see also MainWindow.getJavaScriptConfigurationFileContents().
	 * @param sCode a String containing the Javascript function call that should be executed.
	 * @return a QVariant value that holds the result of the executed Javascript function call.
	 */
	QVariant invokeJavaScriptConfigurationFile(const QString &sCode);
	/*! \brief returns the content from the embedded JavaScript file inside the BrainStim application.
	 *  
	 *  This function returns the content from the embedded JavaScript file inside the BrainStim application that is used for storing all kind of application and plugin (version) information.
	 *  See also MainWindow.invokeJavaScriptConfigurationFile().
	 * @return a string containing a copy of the code contents of the embedded JavaScript file.
	 */
	QString getJavaScriptConfigurationFileContents();
	/*! \brief executes the content from the embedded JavaScript file inside the BrainStim application in a QWebView object.
	 *  
	 *  This function executes the content from the embedded JavaScript file (of the BrainStim application that is used for storing all kind of application and plugin (version) information) in a QWebView object.
	 *  See also MainWindow.invokeJavaScriptConfigurationFile().
	 */
	void showJavaScriptConfigurationFile();
	/*! \brief Adds an include path/directory for the script engine to search in for an included file.
	 *
	 *  This function adds an include path/directory for the script engine to search in for an included file.
	 *  Whenever the script "Include()" command cannot be resolved automatically (using the active docments directory) all the included paths appended with this method are then used for resolving the command.
	 * @param sPath a String containing the path of the path/directory that should be appended to the include script paths.
	 * @return a boolean value determining whether the function could remove the tab successfully.
	 */
	bool addScriptIncludePath(const QString &sPath);
	/*! \brief Returns an array containing all the appended include paths/directories for the script engine to search in for an included file.
	 *
	 *  This function returns an array containing all the appended include paths/directories for the script engine to search in for an included file.
	 *  See also MainWindow::addScriptIncludePath()
	 * @return a string array containing the requested paths/directories.
	 */
	QStringList getScriptIncludePaths();
	/*! \brief Opens one or more files.
	 *
	 *  This function can open one or more files.
	 * @param fileToLoad a String containing a single path to a file that should be loaded.
	 * @param filesToLoad a String Array containing multiple Strings containing the paths to the files that should be loaded.
	 */
	void openFiles(const QString &fileToLoad = QString(), const QStringList &filesToLoad = QStringList(), const bool &bViewAsText = false);
	/*! \brief Executes the current active document.
	 *
	 * This function executes the current active document (file that is opened and active).
	 */
	void executeActiveDocument();
	/*! \brief Returns the path to the current active document.
	 *
	 * This function returns the path to the document that is currently opened and active.
	 * @return a String value holding a path to the requested document.
	 */
	QString getActiveDocumentFileLocation();
	/*! \brief Returns the path to the BrainStim QML plugin directory.
	 *
	 * This function returns the path to the BrainStim QML plugin directory.
	 * @return a String value holding the path to the BrainStim QML plugin directory.
	 */
	QString getQMLPluginPath();
	/*! \brief Returns the path to the BrainStim Root directory.
	 *
	 * This function returns the path to the BrainStim Root directory, where the binary is running in.
	 * @return a String value holding the path to the Root directory.
	 */
	QString getApplicationRootDirPath();
	/*! \brief Returns the filename of the current active document.
	 *
	 * This function returns the filename of the document that is currently opened and active.
	 * @return a String value holding the filename of the requested document.
	 */
	QString getActiveDocumentFileName();
		/*! \brief configures the Active(edit) state for the current document.
	 *
	 * This function can configures the Active(edit) state for the current document, this can be convenient for (un-)locking the document for editing
	 * @param bEnable a Boolean value (default = true) determining whether the document should be Activated (Editable).
	 */
	void enableActiveDocument(bool bEnable = true);
	/*! \brief Returns the requested Environment Variable.
	 *
	 * This function can return a Environment Variable value by providing the name of the variable.
	 * @param strName a String value holding the name of the requested Environment Variable.
	 * @return a String value holding the value of the requested Environment Variable.
	 */
	QString getEnvironmentVariabele(QString strName);
	/*! \brief Closes the current active document.
	 *
	 * This function closes the document that is currently opened and active.
	 */
	void closeActiveDocument();
	//void debugScript();
	/*! \brief Initializes the main BrainStim application and the dynamic plugin loading.
	 *
	 * This function Initializes the main BrainStim application and the dynamic plugin loading, this is automatically done during startup.
	 * @param mainFlags a GlobalApplicationInformation::MainProgramModeFlags value holding the flags used for the initialization.
	 * @return a boolean value determining whether the function could remove the tab successfully.
	 */
	bool initialize(GlobalApplicationInformation::MainProgramModeFlags mainFlags = 0);
	/*! \brief Appends a text String to the Output Log Window.
	 *
	 * This function appends a provided String to the Output Log Window.
	 * @param text2Write a String value holding the text that should be appended.
	 * @param sTabName a String value holding the name of the tab where the text should be appended, if left empty then the text is automatically send to the first "Default" tab.
	 */
	void write2OutputWindow(const QString &text2Write = "", const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	/*! \brief Re-opens the document.
	 *
	 * This function first closes and re-opens the document.
	 * @param strCanonicalFilePath a String value containing the canonical file path of the document.
	 * @param bNativeFileViewer a Boolean value determining whether the file should be opened again by the default set file-handler (defined by a plug-in) or by BrainStim directly.
	 */
	void reOpenCurrentFile(const QString &strCanonicalFilePath, const bool &bNativeFileViewer = false);
	/*! \brief Clears the Output Log Window.
	 *
	 * This function clears the Output Log Window.
	 * @param sTabName a String value holding the name of the tab that should be cleared, if left empty then the first "Default" tab is automatically cleared.
	 */
	void clearOutputWindow(const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	/*! \brief Add a new tab to the Output Log Window.
	 *
	 * This function adds a new tab to the Output Log Window.
	 * @param sTabName a String value holding the name of the new tab.
	 * @return a boolean value determining whether the function did insert a new tab successfully.
	 */
	bool removeOutputWindow(const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	/*! \brief Removes a named tab from the Output Log Window.
	 *
	 * This function removes a named tab from the Output Log Window.
	 * @param sTabName a String value holding the name of the tab that should be removed.
	 * @return a boolean value determining whether the function could remove the tab successfully.
	 */
	bool addOutputWindow(const QString &sTabName);
	/*! \brief Saves the Output Log Window.
	 *
	 * This function saves the named Output Log Window to a text file.
	 * @param sFilePath a String value holding the path to the destination file.
	 * @param bOverwrite a Boolean value determining whether the destination file may be overwritten in case the file already exists.
	 * @param sTabName a String value holding the name of the tab that should be saved.
	 * @return a boolean value determining whether the function could remove the tab successfully.
	 */
	bool saveOutputWindow(const QString &sFilePath = "", const bool &bOverwrite = false, const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	/*! \brief forces the main application event loop to process its events.
	 *
	 * This function forces the main application event loop to process its events, see http://qt-project.org/doc/qt-5.1/qtcore/qcoreapplication.html#processEvents.
	 */
	void processEvents() {qApp->processEvents();};
	/*! \brief Forces the script engine to perform a garbage collection.
	 *
	 * This function forces the script engine to perform a garbage collection and is therefore a safe and good practice to execute as last command before ending the script.
	 */
	void cleanupScript();
	/*! \brief Forces the BrainStim User Interface to become activated.
	*
	* This function forces the the BrainStim User Interface to become activated. An active window is a visible top-level window that has the keyboard input focus.
	* It is the same operation as clicking the mouse on the title bar of a top-level window.
	*/
	void activateMainWindow();
	/*! \brief Executes the provided script content in the Internal Script Engine and returns the result of that operation.
	*
	* This function executes the provided script content in the Internal Script Engine and returns the result of that operation.
	* This function acts the same as like executing a script file (*.qs) from the BrainStim application, but now the content of the script is directly provided by a string of text.
	* Before the internal script engine executes the custom script it firsts creates a so called script context state from within the script is then executed. 
	* This context state holds the stack and can be saved/recovered for later usage, see also MainWindow::saveContextState(), MainWindow::setContextState(), MainWindow.resetContextState(), MainWindow.deleteContextState().
	* @param sContent a string value holding the script content to execute.
	* @return a QScriptValue value holding the return value of the script execution.
	*/
	QScriptValue executeScriptContent(const QString &sContent);
	/*! \brief Saves the current internal script engine context state marked with a provided context name for later usage.
	*
	* This function saves the current internal script engine context state marked with a provided name for later usage.
	* @param sContextName a string value holding the script context name.
	* @return a boolean value determining whether the function could execute successfully.
	*/
	bool saveContextState(const QString &sContextName);
	/*! \brief Sets/switches the current internal script engine to the context state marked with the provided context name.
	*
	* This function sets/switches the current internal script engine to the context state marked with the provided context name.
	* @param sContextName a string value holding the script context name.
	* @return a boolean value determining whether the function could execute successfully.
	*/
	bool setContextState(const QString &sContextName);
	/*! \brief Resets the current internal script engine context state to the context state marked with a script identifier.
	*
	* This function resets the current internal script engine context state to the context state marked with a script identifier, see also MainWindow.getCurrentContextStateScriptID().
	* If the script identifier is left empty or 0 then the current script state is reset.
	* @param nScriptId a integer value holding the script identifier (default = 0).
	* @return a boolean value determining whether the function could execute successfully.
	*/
	bool resetContextState(const quint64 &nScriptId = 0);
	/*! \brief Returns the current internal script engine context state script identifier.
	*
	* This function returns the current internal script engine context state script identifier.
	* @return a integer value holding the requested context state script identifier.
	*/
	int getCurrentContextStateScriptID();
	/*! \brief Deletes the provided context name of the internal script engine context state list.
	*
	* This function deletes the provided context name of the internal script engine context state list.
	* @return a boolean value determining whether the function could execute successfully.
	*/
	bool deleteContextState(const QString &sContextName);
	/*! \brief Forces a complete restart of the internal script engine.
	*
	* This function forces a complete restart of the internal script engine.
	* The engine restarts which means that all running scripts and context states are deleted.
	* @return a boolean value determining whether the function could execute successfully.
	*/
	bool restartScriptEngine();
	/*! \brief Emulates one or more key press event(s).
	*
	* This function emulates one or more key press event(s). These events are executed in a specified QWindow, there's also a option to set a delay.
	* @param pWindow a pointer to a QWindow, this is the QWindow where the key press event should be executed in (default = NULL), if this parameter is NULL then the current active BrainStim document window is used.
	* @param keyCode a value (default = 0), this is the key code for the key to emulate, see http://qt-project.org/doc/qt-5.1/qtcore/qt.html#Key-enum.
	* @param sKeys a string (default = ""), this is a string of keys to emulate.
	* @param modifier a integer value holding a keyboard modifier value (default = Qt::NoModifier), see http://qt-project.org/doc/qt-5.1/qtcore/qt.html#KeyboardModifier-enum.
	* @param nDelay a integer value (default = -1) holding the delay in mSeconds between the key presses.
	* @return a boolean value determining whether the function could execute successfully.
	*/
	bool emulateKeyPress(QWindow *pWindow = NULL, const int keyCode = 0, const QString &sKeys = "", const Qt::KeyboardModifiers modifier = Qt::NoModifier, const int nDelay = -1);
	/*! \brief Searches for a provided String inside the currently active document.
	*
	* This function searches for a provided String of text inside the currently active document. Furthermore this action can be specified with special flags.
	* @param useParams a boolean value that determines whether the optional provided parameters should be used. If useParams is false then a Search window appears allowing the user to
	* make further changes. If some text from the document was selected than this String is automatically used for the find String, otherwise the word around the cursor position is used.
	* If useParams is true then no Search window appears and the optional provided parameters are used for the search action, the first found occurrence (starting from the cursor position) 
	* of the find result is then automatically selected.
	* @param strFindString a String holding the text value to use.
	* @param DocFindFlags a String holding the text value to use, you can create this structure in the script like: 
	* \code 
	* var varName=DocFindFlags; 
	* varName.backwards = true; 
	* \endcode 
	* see _DocFindFlags.
	*/
	void find(bool useParams = false, QString strFindString = "", DocFindFlags findFlags = _DocFindFlags());
	/*! \brief Searches and replaces a provided String inside the currently active document.
	 *
	 * This function searches and replaces a provided String of text inside the currently active document. Furthermore this action can be specified with special flags.
	 * @param bReplaceAll a boolean value that determines whether all occurrences of the found String should be replaced.
	 * @param useParams a boolean value that determines whether the optional provided parameters should be used. If useParams is false then a Replace window appears allowing the user to
	 * make further changes. If some text from the document was selected than this String is automatically used for the find String, otherwise the word around the cursor position is used.
	 * If useParams is true then no Search window appears and the optional provided parameters are used for the search action, the first found occurrence (starting from the cursor position) 
	 * of the find result is then automatically selected.
	 * @param strFindString a String holding the String value to search for.
	 * @param strReplaceString a String holding the text value to replace the found String strFindString with.
	 * @param DocFindFlags a String holding the text value to use, you can create this structure in the script like: 
	 * \code 
	 * var varName=DocFindFlags; 
	 * varName.backwards = true; 
	 * \endcode 
	 * see _DocFindFlags.
	 */	
	void replace(bool bReplaceAll = false, bool useParams = false, QString strFindString = "", QString strReplaceString = "", DocFindFlags findFlags = _DocFindFlags());
	/*! \brief Searches for the last provided String inside the currently active document.
	 *
	 * This function searches (forwards) for the last provided String of text inside the currently active document starting from the current cursor position.
	 */	
	void findNext();
	/*! \brief Searches for the last provided String inside the currently active document.
	 *
	 * This function searches (backwards) for the last provided String of text inside the currently active document starting from the current cursor position.
	 */	
	void findPrev();
	/*! \brief Closes the BrainStim application.
	 *
	 * This function closes the BrainStim application.
	 */		
	void quit(){qApp->closeAllWindows();};

#ifdef DEBUG
	QString testFunction(QString inp = "");
#endif

private slots:
	void ExternalNetworkDataRecieved(int nClientIndex, QString sAvailableData);
	bool receiveExchangeMessage(const QString &sMessage);
	void returnToOldMaxMinSizes();
	void abortScript();
	void setupContextMenus();
	void DebugcontextMenuEvent(const QPoint &pos, const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	void clearDebugger(const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	void copyDebugger(const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	void saveDebugger(const QString &sTabName = MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	void setupScriptEngine();
	bool setupNetworkServer(const QString &sAddress = "" /*=QHostAddress::Any*/, quint16 port = 0);
	void shutdownNetworkServer();
	void setScriptRunningStatus(GlobalApplicationInformation::ActiveScriptMode state);
	void showPluginGUI();
	void openOptionsDialog();
	void aboutBrainStim();
	void showDocumentation();
	void openRecentFile();
	bool closeSubWindow(bool bAutoSaveChanges = false);
	void newFile();
	void save();
	void saveAs();
	void cut();
	void copy();
	void paste();
	void lineComment();
	void findImpl(bool bReplace, bool useParams = false, QString strFindString = "", QString strReplaceString = "", DocFindFlags findFlags = _DocFindFlags(), bool bReplaceAll = false);
	void blockComment();
	void jumpToMatchingBrace();
	void selectToMatchingBrace();
	void duplicateLine();
	void moveLineUp();
	void deleteCurrentLine();
	void toUpperCase();
	void toLowerCase();
	void print();
	void subDocumentWindowActivated(QMdiSubWindow *subWindow);
	void setActiveSubWindow(QWidget *window);
	void updateWindowMenu();
	void updateMarkersMenu();
	void toggleMarker(int nLine = -1);
	void handleToggledMarker(int nLine = -1);
	void nextMarker();
	void prevMarker();
	void removeAllMarkers();
	void goToLine();
	void gotoMarker();
	void NumberOfLinesChangedHandler(int nrOfLines);
	void CursorPositionChangedHandler(int line, int col);
	void scriptLoaded(qint64 id);
	void scriptUnloaded(qint64 id);
	void outputTabCloseRequest(int nIndex);
	void dockWidgetDestroyed(QObject *obj);

private:
	//void registerFileTypeByDefinition(const QString &DocTypeName, const QString &DocTypeDesc, const QString &DocTypeExtension);
	QMultiMap<QMdiSubWindow *, QDockWidget*> mapMDISubWindowToDockWidget;
	QPair<QString,quint64> pCurrentSetContextState;
	QList<QScriptContextStrc> currentScriptEngineContexes;
	QSize oldDockMaxSize, oldDockMinSize;
	QString strAdditionalFileExtensions;
	GlobalApplicationInformation::ScriptRunMode BrainStimScriptRunMode;
	NetworkServer *tcpServer;
	QSplashScreen *MainSplashScreen;
	Assistant *helpAssistant;
	QStringList startUpFiles;
    QAction *m_nativeAction;
    QAction *m_glAction;
    QAction *m_imageAction;
    QAction *m_highQualityAntialiasingAction;
    QAction *m_backgroundAction;
    QAction *m_outlineAction;
	QAction *openAction;
	QAction *quitAction;
	QAction *saveAction;
	QAction *optionsAction;
	QAction *saveAsAction;
	QAction *printAction;
	QAction *runDocumentAction;
	//QAction *debugScriptAction;
	QAction *abortDocumentAction;
	QAction *restartScriptEngineAction;
	QAction *newAction;
	QAction *newDocumentAction;
	QAction *newSVGAction;
	QAction *newQtScriptAction;
	QList<QAction *> lNewPluginFileAction;
	QAction *newJavaScriptAction;
	QAction *cutAction;
	QAction *copyAction;
	QMap<QString,QAction *> mClearDebuggerAction;
	QMap<QString,QAction *> mCopyDebuggerAction;
	QMap<QString,QAction *> mSaveDebuggerAction;
	//QMap<QString,QAction *> mRemoveDebuggerAction;
	QAction *pasteAction;
	QAction *goToLineAction;
	QAction *findAction;
	QAction *findNextAction;
	QAction *findPrevAction;
	QAction *replaceAction;
	QAction *goToMatchingBraceAction;
	QAction *selToMatchingBraceAction;
	QAction *lineCommentAction;
	QAction *blockCommentAction;
	QAction *duplicateLineAction;
	//QAction *moveLineUpAction;
	QAction *deleteCurrentLineAction;
	QAction *toUpperCaseAction;
	QAction *toLowerCaseAction;
	QAction *closeAction;
	QAction *closeAllAction;
    QAction *tileAction;
    QAction *cascadeAction;
    QAction *nextAction;
    QAction *previousAction;
	QAction *addRemMarkerAction;
	QAction *nextMarkerAction;
	QAction *prevMarkerAction;
	QAction *remAllMarkerAction;
	QAction *aboutBrainStimAct;
	QAction *assistantAct;
	QAction *aboutQtAct;
	QAction *historyQtAct;
	QAction *separatorAct;
	QMap<QString, QVariant> tmpNewActionMapping;//Can contain the extension and/or file type.. used for creating a new specific document type

	QLabel *StatusPositionLabel;
	QLabel *StatusFilePathLabel;
	QLabel *StatusLinesLabel;

	QGraphicsScene *GraphScene;
	QGraphicsView *GraphView;
	SvgView *SVGPreviewer;

	bool DevicePluginsFound;
	bool ExtensionPluginsFound;
	bool PluginsFound;
	bool bMainWindowIsInitialized;
	bool bExecuteActiveDocument;

	QTScriptEngine *AppScriptEngine;
	GlobalApplicationInformation::ActiveScriptMode AppScriptStatus;
	qint64 currentMainRunningScriptID;	
	QList<qint64> lCurrentRunningScriptIDList;

	QPushButton button;
    QMenu *pluginsMenu;
    QMenu *devicePluginMenu;
	QMenu *extensionPluginMenu;
    QMenu *helpMenu;
	QMenu *recentFilesMenu;
	QMenu *fileMenu;
	QMenu *fileNewMenu;
	//QMenu *viewMenu;
	QMenu *markersMenu;
	QMenu *editMenu;
	QMenu *editOutputMenu;
	QMenu *toolsMenu;
	QMenu *windowMenu;
	QMenu *documentMenu;
	//QMenu *debuggerMenu;

    //QAction *deviceAct1;
    //QAction *deviceAct2;
    //QAction *deviceAct3;
    //DeviceControl *devices;
	//QString curFile;
	//QSettings *settings;

	QDockWidget *debugLogDock;
	QDockWidget *debuggerDock;

	QTabWidget *outputTabWidget;
	QMap<QString, QTextEdit *> mTabNameToOutputWindowList;
	QMap<QString, QMetaObject::Connection> mTabNameToConnectionList;
	MainWindow *debuggerMainWindow;
	
	DocumentManager *DocManager;
    QMdiArea *mdiArea;
	QSignalMapper *windowMapper;
	QSignalMapper *signalMapperClearDebugger;
	QSignalMapper *signalMapperCopyDebugger;
	QSignalMapper *signalMapperSaveDebugger;
	//QSignalMapper *signalMapperRemoveDebugger;
	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QToolBar *toolsToolBar;

    QString m_currentPath;
    QStringList pluginFileNames;
	PluginCollection *Plugins;
	GlobalApplicationInformation::MainAppInformationStructure *mainAppInfoStruct;

	enum { MaxRecentFiles = 10 };
	QList<QAction *> recentFileActs;

	void showSplashMessage(const QString message);
	void setupMDI();
	void setupDocumentManager();
	bool checkUserDirectories(QStringList &lPathsToCheck, bool bShowWarning = false);
	void setAppDirectories();
	void createDockWindows();
	void setupStatusBar();
	bool setDefaultGLFormat();
    void createDefaultMenus();
	void setupHelpMenu();
    void setupDynamicPlugins();
	QAction* integratePlugin(QObject *plugin, PluginCollection *collection);
	void setupToolBars();
	void setRenderer();
	void newDocument(const GlobalApplicationInformation::DocType &docType, int &DocIndex, const QString &strExtension = "", const QString &strCanonicalFilePath = "", const bool &bNativeMainAppView = false);
	//void setupSyntaxHighlighting(MdiChild *childWindow,MDIDocumentType tempFileType);
	void parseRemainingGlobalSettings();
	bool configureDebugger();
	int configurePluginScriptEngine(const int nIndex);
	void writeMainWindowSettings();
	bool checkPluginCompatibility(QObject *plugin);
	void parsePluginDefinedFileExtensions(QObject *plugin);
    bool popPluginIntoMenu(QObject *plugin);
	bool parseFile(const QFile &file, const bool &bParseAsText = false);
	void setCurrentFile(const QString &fileName);
	void updateRecentFileActions();
	QString strippedName(const QString &fullFileName);
	QMdiSubWindow *activeMdiChild();
	QString activeMdiChildFilePath();
	QMdiSubWindow *findMdiChild(const QString &fileName);
	void updateRecentFileList(const QString &fileName);
	void setDockSize(QDockWidget *dock, int setWidth, int setHeight);
	bool check4ReParseFile(const QString &sFilename);
	void updateMenuControls();

public:
	bool extendAPICallTips(const QMetaObject* metaScriptObject = NULL);
	void setGlobalApplicationInformationObject(GlobalApplicationInformation *globAppInformation);
	void RecoverLastScreenWindowSettings();
	void setStartupFiles(const QString &path = QString());
	Q_INVOKABLE bool registerDockWidget(QWidget *pMDIWindowSubWidget, QDockWidget *pDockWidget, int nDockWidgetAreaEnum);

	static GlobalApplicationInformation::MainProgramModeFlags BrainStimFlags;
	static GlobalApplicationInformation *globAppInfo;

protected:
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent * event);
	virtual bool openDroppedFiles(const QStringList &pathList);	
};

#endif
