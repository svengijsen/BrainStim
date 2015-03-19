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


#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QtWidgets>

#include <Qsci/qsciapis.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerperl.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerxml.h>
#include "customqsciscintilla.h"
#include "scifinddialog.h"
#include "mainappinfo.h"

#define WARNING_DOCCHANGEDSAVEFILE_STRING		"The document has been modified.\nDo you want to save your changes?"
#define DOCUMENT_HANDLERS_SLOT_SPLITTER_CHAR	"|"
#define DOCUMENT_TEXTCHANGED_SLOT_SIGNATURES	"textChanged()"

class DocumentManager : public QObject
{
	Q_OBJECT

signals:
	//void DocumentChanged();
	void NrOfLinesChanged(int);
	void MarkerToggled(int);
	void DocumentManagerOutput(QString strText2Output);

public:

	DocumentManager(QObject *parent);
	~DocumentManager();

	enum PluginHandlerSlotType
	{
		PLUGINHANDLER_SLOT_EXECUTE		= 0,
		PLUGINHANDLER_SLOT_NEWFILE		= 1
	};

	struct strcPluginHandlerInterface
	{
		QString sExecuteHandlerSlotSignature;
		QObject *pPluginHandlerExecuteObject;
		QString sNewFileHandlerSlotSignature;
		QObject *pPluginHandlerNewFileObject;
		QObject *pPluginObject;
		strcPluginHandlerInterface()
		{
			sExecuteHandlerSlotSignature = "";
			sNewFileHandlerSlotSignature = "";
			pPluginHandlerExecuteObject = NULL;
			pPluginHandlerNewFileObject = NULL;
			pPluginObject = NULL;
		}
	};

	struct strcPluginDocHandlerInterfaceCollection
	{
		QHash<QString, strcPluginHandlerInterface> hDocHandlerList; //FileExtension/HandlerSlots
	};

	struct strcPluginChildDocCustomMenuDef
	{
		QMenu *customRootMenu;
		QMenu *parentMenu;
		QAction *customAction;
		strcPluginChildDocCustomMenuDef()
		{
			customRootMenu = NULL;
			parentMenu = NULL;
			customAction = NULL;
		}
	};

	struct strcDockLocation
	{
		QRect rGeometry;
		Qt::DockWidgetArea dockArea;
		strcDockLocation()
		{
			rGeometry.setCoords(0, 0, 0, 0);
			dockArea = Qt::DockWidgetArea::NoDockWidgetArea;
		}
	};

	struct strDocManagerDocument
	{
		QString sFileName;
		GlobalApplicationInformation::DocType dDocType;
		QWidget *pWidget;
		QMdiSubWindow *pMDISubWin;
		strcPluginHandlerInterface *pPluginHandlerInterface;
		bool bIsModified;
		bool bIsNewUnsaved;
		QList<strcPluginChildDocCustomMenuDef> lRegisteredMenuActions;
		QMap<MainWindowDockWidget*, strcDockLocation> mapRegisteredDockWidgetToLocationStruct;
		strDocManagerDocument()
		{
			sFileName = "";
			dDocType = GlobalApplicationInformation::DOCTYPE_UNDEFINED;
			pWidget = NULL;
			pMDISubWin = NULL;
			pPluginHandlerInterface = NULL;
			bIsModified = false;
			bIsNewUnsaved = false;
		}
	};

	QWidget *getDocHandler(const int &DocIndex);
	QWidget *getDocHandler(QMdiSubWindow *subWindow);
	bool isModified(QMdiSubWindow *subWindow);
	QMdiSubWindow *getDocSubWindow(QWidget *pDocumentSubWidget);
	int count(void);
	QWidget *add(GlobalApplicationInformation::DocType docType,int &DocIndex, const QString &strExtension, const bool &bIsNewUnsaved, const QString &strCanonicalFilePath = "", const bool &bNativeMainAppView = false);
	bool remove(QMdiSubWindow *subWindow);
	bool setSubWindow(int DocIndex, QMdiSubWindow *subWindow);
	bool initFile(int DocIndex);
	bool loadFile(int DocIndex, const QString &fileName);
	GlobalApplicationInformation::DocType getDocType(QMdiSubWindow *subWindow);
	GlobalApplicationInformation::DocType getDocType(const QString &strExtension);
	QString getDocTypeString(const GlobalApplicationInformation::DocType &docType);
	QString getFileName(QMdiSubWindow *subWindow, bool bFileNameOnly = false);
	QString getFileName(int DocIndex, bool bFileNameOnly = false);
	QString getFileExtension(QMdiSubWindow *subWindow);
	bool isUnsavedNewFile(QMdiSubWindow *subWindow);
	void setFileName(int DocIndex, QString fileName, bool bIsNewNotSavedFile = false);
	QString getFilePath(QMdiSubWindow *subWindow);
	int getDocIndex(QMdiSubWindow *subWindow);
	int getDocIndex(const QString &DocName);
	bool saveFile(int DocIndex, QString fileName = "", bool *bReparseDocumentContentNeeded = NULL, QString &sSavedFilenNamePath = QString());
	bool saveFile(QMdiSubWindow *subWindow, QString fileName = "", bool *bReparseDocumentContentNeeded = NULL, QString &sSavedFilenNamePath = QString());
	void setModFlagAndTitle(const int &DocIndex,bool hasChanges);
	bool maybeSave(QMdiSubWindow *subWindow,bool bAutoSaveChanges = false, QString &sSavedAsFilePath = QString());
	void setAllUnmodified();
	bool getFindParams(QMdiSubWindow *subWindow,QString& str1, QString& str2, DocFindFlags& flags);
	QString lastFindText() const;
	DocFindFlags lastFlags() const; 
	bool appendKnownFileExtensionList(QString strFileExtLst);
	QString getKnownFileExtensionList() {return strFileExtensionList;};
	bool appendKnownDocumentFileHandlerList(const QString &strDocHandlerInfo, QObject *pluginObject);
	bool isKnownDocumentFileHandlerIndex(const QString &strExtension);
	QObject *getKnownDocumentFileHandlerObject(const QString &strExtension, QString &strDocHndlrName, const PluginHandlerSlotType &eHandlerSlotType);
	int addAdditionalApiEntry(const QString &entry);
	bool addMenuActionRegistration(QMdiSubWindow *subWindow, QMenu *firstMenu, QAction *menuAction);
	QList<DocumentManager::strcPluginChildDocCustomMenuDef> getMenuActionRegistrations(QMdiSubWindow *subWindow);
	bool addDockWidgetRegistration(QMdiSubWindow *subWindow, MainWindowDockWidget* dockWidget, const DocumentManager::strcDockLocation &dockWidgetLocation);
	QMap<MainWindowDockWidget*, DocumentManager::strcDockLocation> getDockWidgetRegistrations(QMdiSubWindow *subWindow);
	void removeAllMenuActionRegistrations();
	void appendAllMenuActionRegistration(const DocumentManager::strcPluginChildDocCustomMenuDef &customMenuDef);

public slots:
	void signalDocManagerOutput(QString strText2Output) {emit DocumentManagerOutput(strText2Output);};	

private:
	DocFindFlags lastFlags_;
	QSignalMapper *DocModifiedMapper;
	QSignalMapper *NrOfLinesChangedMapper;
	QString strFileExtensionList;
	strcPluginDocHandlerInterfaceCollection lPluginDefinedPreLoadedHandlerInterfaces;	
	QList<strDocManagerDocument> lChildDocuments;
	QList<strcPluginChildDocCustomMenuDef> lAllRegisteredActions;
	QStringList additionalApiEntries;

	bool getLexer(QsciLexer *lexer, const QString &lexerName, QObject *parent = 0);
	QStringList getAdditionalApiEntries() {return additionalApiEntries;};
	bool customizeDocumentStyle(CustomQsciScintilla *custQsci,GlobalApplicationInformation::DocTypeStyle dStyle = GlobalApplicationInformation::DOCTYPE_STYLE_UNDEFINED, const QString &strAPIFileName = "");

private slots:
	void documentWasModified(QWidget *subWindow);
	void updateLineNumbers(QWidget *tmpSci);
	void onMarginClicked (int margin, int line, Qt::KeyboardModifiers state);
	void configureDocumentModifiedSetting(const QString &sCanoFilePath, const bool &bIsModified);
	void processDocumentClosing(const QString &sCanoFilePath, const bool &bAutoSaveChanges);
	//void updateLineNumbers(void);
};

#endif // DOCUMENTMANAGER_H
