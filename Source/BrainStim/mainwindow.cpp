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

#include "mainwindow.h"
#include "mainappinfo.h"
#include "globalapplicationinformation.h"
#include "BrainStim.h"
#include "svgview.h"
#include "aboutqtdialog.h"  
#include "sleepthread.h"
#include <QPluginLoader>
#include <QtPlugin>
#include <QFileInfo>
#include <QMetaObject>
#include <QtGlobal>
#include <QGLFramebufferObject>
#include <QDir>
#include <QTabWidget>
#include "outputlistdelegate.h"
#include "custommdisubwindow.h"
#include "propertysettingswidgetcontainer.h"

#include "../Plugins/ParallelPortDevice/parallelport.h"
#include "../Plugins/ExperimentManager/experimentmanager.h"
#include "../Plugins/KeyBoardDevice/keyboardcapture.h"

GlobalApplicationInformation::MainProgramModeFlags MainWindow::BrainStimFlags = GlobalApplicationInformation::Default;
GlobalApplicationInformation *MainWindow::globAppInfo = NULL;

MainWindow::MainWindow() : DocumentWindow(), SVGPreviewer(new SvgView)
{
	Plugins = NULL;
	pMainOptionPage = NULL;
	tcpServer = NULL;
	DocManager = NULL;
	AppScriptEngine = NULL;
	globAppInfo = NULL;
	mainAppInfoStruct = NULL;
	PluginsFound = false;
	bDevicePluginsFound = false;
	bExtensionPluginsFound = false;
	helpAssistant = new Assistant;
	bMainWindowIsInitialized = false;
	resetContextState();

#ifndef QT_NO_DEBUG_OUTPUT	
	qInstallMessageHandler((QtMessageHandler)MainAppInfo::MyOutputHandler);
	qDebug("BrainStim Main Constructor.");
#endif
//When compiling your final application, be sure to add the flags -DQT_NO_DEBUG and -DQT_NO_DEBUG_OUTPUT.  
//These will reduce the application size and ensure that the logfile isn't used. 
//Alternately, you can keep NO_DEBUG_OUTPUT out of the makefile if you still want the logfile.
}

MainWindow::~MainWindow()//see void MainWindow::closeEvent(QCloseEvent *event)!
{
}

bool MainWindow::initialize(GlobalApplicationInformation::MainProgramModeFlags mainFlags)
{
	BrainStimFlags = mainFlags;
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	//Default					= 0x00000,
	//DisablePlugins			= 0x00001,
	//DisableSplash				= 0x00002
	//QWaitCondition sleep;
	qDebug() << "Starting and initializing" << MAIN_PROGRAM_FULL_NAME;
	//registerFileTypeByDefinition("BrainStim.QtScript","BrainStim Qt Script File",".qs");
	//MainAppInfo::Initialize(this);
	//QApplication::setGraphicsSystem("opengl");//"raster");
	AppScriptStatus = GlobalApplicationInformation::NoScript;

	if (BrainStimFlags.testFlag(GlobalApplicationInformation::DisableSplash) == false)
	{
		QPixmap pixmap(":/resources/splash.png");
		MainSplashScreen = new QSplashScreen(pixmap);
		MainSplashScreen->show();
		showSplashMessage("Initializing Program...");
	}
	showSplashMessage("Setup User Interface...");
	setupMDI();
	setWindowTitle(globAppInfo->getTitle());//  MainAppInfo::MainProgramTitle());
	setUnifiedTitleAndToolBarOnMac(true);
	createDockWindows();
	checkUserDirectories(MainAppInfo::getUserFolderList(),true);
	setAppDirectories();
	setupScriptEngine();
	setupStatusBar();
	createDefaultMenus();
	setupDynamicPlugins();
	setupHelpMenu();
	setupToolBars();
	updateMenuControls();
	parseRemainingGlobalSettings();
	setupContextMenus();
	implementCustomActionMenu(":/resources/customMenuEntries.ini");
	implementPluginCustomActionMenus();
	if (startUpFiles.count()> 0) { openFiles(NULL,startUpFiles);}
	if (BrainStimFlags.testFlag(GlobalApplicationInformation::DisableSplash)==false)
	{
		MainSplashScreen->finish(this);
	}
#ifdef Q_OS_WIN32 //Are we on Windows?
	if (timeBeginPeriod(1) == TIMERR_NOCANDO)
		qWarning() << "Could not start the time period!";
#endif
	setDefaultGLFormat();
	statusBar()->showMessage(tr("Ready..."), 2000);
	//if (QApplication::desktop()->numScreens() > 1) {
	//	QLabel *label = new QLabel("Hello");
	//	label->setGeometry(QApplication::desktop()->availableGeometry(1));
	//	label->showMaximized();
	//}
	bMainWindowIsInitialized = true;

	if (globAppInfo->shouldEnableNetworkServer())
	{
		if (BrainStimFlags.testFlag(GlobalApplicationInformation::DisableNetworkServer) == false)
			setupNetworkServer(globAppInfo->getHostAddress(),globAppInfo->getHostPort());
		else
			qDebug() << "Configured Network Server disabled.";
	}
	bExecuteActiveDocument = BrainStimFlags & GlobalApplicationInformation::ExecuteDocument;
	return true;
}

bool MainWindow::implementPluginCustomActionMenus()
{
	QString sMenuFolderPath = MainAppInfo::appMenuDirPath();
	if (sMenuFolderPath.isEmpty())
		return false;
	if (QDir(sMenuFolderPath).exists() == false)
		return false;
	bool bRetVal = false;
	QDirIterator dirIt(sMenuFolderPath, QDirIterator::Subdirectories);
	while (dirIt.hasNext()) 
	{
		dirIt.next();
		if (QFileInfo(dirIt.filePath()).isFile())
		{
			if (QFileInfo(dirIt.filePath()).suffix() == "ini")
			{
				if (implementCustomActionMenu(dirIt.filePath()))
					bRetVal = true;
			}
		}
	}
	return bRetVal;
}

bool MainWindow::implementCustomActionMenu(const QString &sMenuIniFilePath)
{
	if (sMenuIniFilePath.isEmpty())
		return false;
	if (QFile(sMenuIniFilePath).exists() == false)
		return false;
	bool bRetval = false;
	//QString sMenuIniFilePath = ":/resources/customMenuEntries.ini";
	QSettings *sUILayoutSettings = new QSettings(sMenuIniFilePath, QSettings::IniFormat);
	sUILayoutSettings->beginGroup("CustomMenuActions");
	QStringList lAllKeys = sUILayoutSettings->allKeys();
	foreach(QString sTmpString, lAllKeys)
	{
		QStringList sSplittedString = sUILayoutSettings->value(sTmpString).toString().split(MENUACTION_SECTION_SEPERATOR);
		if (sSplittedString.count() == 3)
		{
			//if (sSplittedString.at(0).toLower() == MENUACTION_TYPE_SCRIPTPATH)
			//{
				QStringList lSubMenuList = sSplittedString.at(2).split(MENUACTION_SUBMENU_SEPERATOR,QString::SkipEmptyParts);
				//if (lSubMenuList.isEmpty() == false)
				//{
					//if (sSplittedString.at(1).isEmpty() == false)
					//{
						QAction *tmpAction = registerMainMenuAction(lSubMenuList, NULL, true);
						if (sSplittedString.at(0).toLower() == MENUACTION_TYPE_SCRIPTPATH)
						{
							QString sCommand = QString("%1%2%3").arg(sSplittedString.at(0).toLower()).arg(MENUACTION_SECTION_SEPERATOR).arg(sSplittedString.at(1));
							tmpAction->setData(sCommand);
							//tmpAction->setShortcut(tr("Ctrl+Shift+C"));
							//bool bConnectResult = 
							if (connect(tmpAction, SIGNAL(triggered()), this, SLOT(executeCustomActionMenu())))
								bRetval = true;
						}

					//}
				//}
			//}
			//else if (sSplittedString.at(0).toLower() == MENUACTION_TYPE_MENUITEM)
			//{
			
			//}
		}
	}
	sUILayoutSettings->endGroup();

	//QAction *testAction = registerMainMenuAction(QStringList() << "File" << "Wizards" << "Wiz1", true);
	//QString sCommand;
	//sCommand = QString("%1%2%3").arg(MENUACTION_TYPE_SCRIPTPATH).arg(MENUACTION_SECTION_SEPERATOR).arg("C:/Users/svengijsen/Desktop/test.qs");
	//testAction->setData(sCommand);
	////testAction->setShortcut(tr("Ctrl+Shift+C"));
	//bool bConnectResult = connect(testAction, SIGNAL(triggered()), this, SLOT(executeCustomActionMenu()));
	return bRetval;
}

bool MainWindow::executeCustomActionMenu()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
	{
		QString sActionData = action->data().toString();
		if (sActionData.isEmpty() == false)
		{
			QStringList lActionDataList = sActionData.split(MENUACTION_SECTION_SEPERATOR);
			if (lActionDataList.count() == 2)
			{
				if (lActionDataList.at(0) == MENUACTION_TYPE_SCRIPTPATH)
				{
					QScriptValue scriptRetValue = executeScriptFile(lActionDataList.at(1));
					return true;
				}
			}
		}
	}
	else//direct function call, exit program
	{
	}
	return false;
}

void MainWindow::showEvent(QShowEvent * event)
{
	Q_UNUSED(event);
	//bool bresult;
	//if(event->spontaneous())
	//	bresult = true;
	if(bExecuteActiveDocument)//see BrainStimFlags & GlobalApplicationInformation::ExecuteDocument)
	{
		if (activeMdiChild())
			executeActiveDocument();
		bExecuteActiveDocument = false;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	if(!closeSubWindow())
	{
		event->ignore();
		return;
	}
	mdiArea->closeAllSubWindows();
	if (activeMdiChild()) 
	{
		event->ignore();
	} 
	else 
	{
		writeMainWindowSettings();
		event->accept();
	}
	delete helpAssistant;
#ifndef QT_NO_DEBUG_OUTPUT
	MainAppInfo::CloseMainLogFile();	
#endif
	if(Plugins)
		delete Plugins;
#ifdef Q_OS_WIN32 //Are we on Windows?
	timeEndPeriod(1);
#endif
	MainAppInfo::setMainWindow(NULL);
	if(DocManager)
	{
		disconnect(DocManager, SIGNAL(DocumentManagerOutput(QString)), this, SLOT(write2OutputWindow(QString)));
		disconnect(DocManager, SIGNAL(NrOfLinesChanged(int)), this, SLOT(NumberOfLinesChangedHandler(int)));	
		delete DocManager;
		DocManager = NULL;
	}
	if(globAppInfo)
	{
		delete globAppInfo;
		globAppInfo = NULL;
	}
	if(mainAppInfoStruct)
	{
		delete mainAppInfoStruct;
		mainAppInfoStruct = NULL;
	}
	//if (hashUILayoutSettings.isEmpty() == false)
	//{
	//	foreach(QSettings *sTmpSetting, hashUILayoutSettings)
	//	{
	//		if (sTmpSetting)
	//		{
	//			sTmpSetting->sync();
	//			delete sTmpSetting;
	//			sTmpSetting = NULL;
	//		}
	//	}
	//}
	if (pMainOptionPage)
	{
		delete pMainOptionPage;
		pMainOptionPage = NULL;
	}
	MainAppInfo::destructCustomPropertySettingObjects();
	shutdownNetworkServer();
}

bool MainWindow::setContextState(const QString &sContextName)
{
	if(sContextName == "")
		return false;
	if(currentScriptEngineContexes.isEmpty())
		return true;
	for(int i=0;i<currentScriptEngineContexes.count();i++)
	{
		if(currentScriptEngineContexes.at(i).sContextName == sContextName)//Does it exist?
		{
			pCurrentSetContextState.first = sContextName;
			pCurrentSetContextState.second = lCurrentRunningScriptIDList.last();
			return true;
		}
	}	
	return false;
}

int MainWindow::getCurrentContextStateScriptID()
{
	return pCurrentSetContextState.second;
}

bool MainWindow::resetContextState(const quint64 &nScriptId)
{
	if(nScriptId == 0)
	{
		pCurrentSetContextState.first = "";
		pCurrentSetContextState.second = 0;
		return true;
	}
	else if(pCurrentSetContextState.second == nScriptId)
	{
		pCurrentSetContextState.first = "";
		pCurrentSetContextState.second = 0;
		return true;
	}
	return false;
}

bool MainWindow::deleteContextState(const QString &sContextName)
{
	if(sContextName == "")
		return false;
	if(currentScriptEngineContexes.isEmpty() == false)
	{		
		for(int i=0;i<currentScriptEngineContexes.count();i++)
		{
			if(currentScriptEngineContexes.at(i).sContextName == sContextName)//Does it exist?
			{
				currentScriptEngineContexes.removeAt(i);
				return true;
			}
		}
	}		
	return false;
}

bool MainWindow::saveContextState(const QString &sContextName)
{
	if(AppScriptEngine)
	{
		if(AppScriptEngine->eng)
		{
			if(sContextName == "")
				return false;
			QScriptValue act_Object = AppScriptEngine->eng->currentContext()->parentContext()->activationObject();
			QScriptValue this_Object = AppScriptEngine->eng->currentContext()->parentContext()->thisObject();

			if(currentScriptEngineContexes.isEmpty() == false)
			{		
				for(int i=0;i<currentScriptEngineContexes.count();i++)
				{
					if(currentScriptEngineContexes.at(i).sContextName == sContextName)//Should we overwrite?
					{
						currentScriptEngineContexes[i].activationObject = act_Object;
						currentScriptEngineContexes[i].thisObject = this_Object;
						return true;
					}
				}
			}
			QScriptContextStructure tmpStruct;
			tmpStruct.sContextName = sContextName;
			tmpStruct.activationObject = act_Object;
			tmpStruct.thisObject = this_Object;
			currentScriptEngineContexes.append(tmpStruct);
			return true;
		}
	}
	return false;
}

#ifdef DEBUG
QString MainWindow::testFunction(QString inp)
{
	//if(AppScriptEngine)
	//{
	//}
	return inp;
}
#endif

bool MainWindow::openDroppedFiles(const QStringList &pathList)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	this->openFiles(NULL,pathList);
	return true;
}

QVariant MainWindow::invokeJavaScriptConfigurationFile(const QString &sCode)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	return globAppInfo->invokeJavaScriptConfigurationFile(sCode);
}

QString MainWindow::getJavaScriptConfigurationFileContents()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	return globAppInfo->getJavaScriptConfigurationFileContents();
}

void MainWindow::showJavaScriptConfigurationFile()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	globAppInfo->showJavaScriptConfigurationFile();
}

bool MainWindow::addScriptIncludePath(const QString &sPath)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	return globAppInfo->addAppScriptIncludePath(sPath);
}

QStringList MainWindow::getScriptIncludePaths()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	return globAppInfo->getAppScriptIncludePaths();
}

bool MainWindow::receiveExchangeMessage(const QString &sMessage)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	bool bWasNotJetInitialized = false;
	while(bMainWindowIsInitialized == false)
	{
		processEvents();
		bWasNotJetInitialized = true;
	}
	if(bWasNotJetInitialized)
		write2OutputWindow("-> SocketData (delayed) Received [" + sMessage + "]");
	else
		write2OutputWindow("-> SocketData Received [" + sMessage + "]");
	
	QScriptValue scriptVal = executeScriptContent(sMessage);

	QString strResult = scriptVal.toString();
	if (scriptVal.isError()) 
	{
		write2OutputWindow("... Script stopped Evaluating due to an error: --> " + strResult + "...");
	}
	else
	{
		write2OutputWindow("-> SocketData Successfully executed by the Script Engine.");
	}
	return true;
}

void MainWindow::DebugcontextMenuEvent(const QPoint &pos, const QString &sTabName)
{
	if(mTabNameToOutputWindowList.contains(sTabName)==false)
		return;
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	QMenu menu(this);
	if(mClearDebuggerAction.contains(sTabName))
		menu.addAction(mClearDebuggerAction.value(sTabName));
	if(mCopyDebuggerAction.contains(sTabName))
		menu.addAction(mCopyDebuggerAction.value(sTabName));
	if(mSaveDebuggerAction.contains(sTabName))
		menu.addAction(mSaveDebuggerAction.value(sTabName));
	//if(mRemoveDebuggerAction.contains(sTabName))
	//	menu.addAction(mRemoveDebuggerAction.value(sTabName));
	menu.exec(mTabNameToOutputWindowList[sTabName]->viewport()->mapToGlobal(pos));
}

void MainWindow::setupContextMenus()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	mTabNameToOutputWindowList[MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME]->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(mTabNameToOutputWindowList[MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME], &QTextEdit::customContextMenuRequested, [=](const QPoint &arg) {DebugcontextMenuEvent(arg, MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);});
}

void MainWindow::showSplashMessage(const QString message)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	if (BrainStimFlags.testFlag(GlobalApplicationInformation::DisableSplash) == false)
	{
		MainSplashScreen->showMessage(message,Qt::AlignBottom ,Qt::white);
	}	
}

void MainWindow::setGlobalApplicationInformationObject(GlobalApplicationInformation *globAppInformation)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	globAppInfo = globAppInformation;
	mainAppInfoStruct = new GlobalApplicationInformation::MainAppInformationStructure(globAppInformation->getMainAppInformationStructure());
}

void MainWindow::setupMDI()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	mdiArea = new QMdiArea;
	mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	mdiArea->setViewMode(QMdiArea::TabbedView);
		//QMdiArea::SubWindowView	0	Display sub-windows with window frames (default).
		//QMdiArea::TabbedView		1	Display sub-windows with tabs in a tab bar.
	mdiArea->setTabsClosable(true);
	mdiArea->setTabsMovable(true);
	mdiArea->setTabShape(QTabWidget::Rounded);
		//QTabWidget::Rounded		0	The tabs are drawn with a rounded look. This is the default shape.
		//QTabWidget::Triangular	1	The tabs are drawn with a triangular look.
	mdiArea->setTabPosition(QTabWidget::North);
	setCentralWidget(mdiArea);
	bool bResult = connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(updateMenuControls(QMdiSubWindow *)));
	bResult = connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow *)), this, SLOT(subDocumentWindowActivated(QMdiSubWindow *)));
	windowMapper = new QSignalMapper(this);
	bResult = connect(windowMapper, SIGNAL(mapped(QWidget *)), this, SLOT(setActiveSubWindow(QWidget *)));
	setupDocumentManager();
}

void MainWindow::setupDocumentManager()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	DocManager = new DocumentManager (this);
	connect(DocManager, SIGNAL(DocumentManagerOutput(QString)), this, SLOT(write2OutputWindow(QString)));
	DocManager->appendKnownFileExtensionList(MainAppInfo::getDefaultFileExtList());
	connect(DocManager, SIGNAL(NrOfLinesChanged(int)), this, SLOT(NumberOfLinesChangedHandler(int)));
	connect(DocManager, SIGNAL(MarkerToggled(int)), this, SLOT(handleToggledMarker(int)));
}

QScriptValue myPauseFunction(QScriptContext *ctx, QScriptEngine *eng)
{
	Q_UNUSED(eng);
	int nWaitTime = ctx->argument(0).toInteger();
	
	QTime dieTime = QTime::currentTime().addMSecs(nWaitTime);
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents(QEventLoop::AllEvents);
	return 0;

	//QMutex mutTmp;
	//mutTmp.lock();
	//QWaitCondition waitCondition;
	//waitCondition.wait(&mutTmp, nWaitTime);
	//mutTmp.unlock();
	//return 0;
}

QScriptValue myThrowScriptErrorFunction(QScriptContext *ctx, QScriptEngine *eng)
{
	Q_UNUSED(eng);
	QString result;
	for (int i = 0; i < ctx->argumentCount(); ++i) {
		if (i > 0)
			result.append(" ");
		result.append(ctx->argument(i).toString());
	}
	ctx->throwError(result);
	return 0;
}

QScriptValue myBeepFunction(QScriptContext *ctx, QScriptEngine *eng)
{
	Q_UNUSED(eng);
	Q_UNUSED(ctx);
	//DWORD Tone(2);//frequency, duration
	//Tone[0] = 100;
	//Tone[1] = 100;
	//for (int i = 0; i < ctx->argumentCount(); ++i) {
	//if (i > 0)
	//	result.append(" ");
	//result.append(ctx->argument(i).toString());
	//}
	qApp->beep();

	//system("beep -f 200 -l 100");
	return 0;
}

QScriptValue myIncludeFunction(QScriptContext *ctx, QScriptEngine *eng)
{
	QString fileName = ctx->argument(0).toString();
	QString fileText;
	QFile file(fileName);
	bool bIncludePathUsed = false;
	QStringList lIncludePaths =  MainWindow::globAppInfo->getAppScriptIncludePaths();
	if(file.exists()==false)//Not found? Use one of the include paths
	{
		foreach(QString tmpStringPath, lIncludePaths)
		{
			if((tmpStringPath.endsWith("/") || tmpStringPath.endsWith("\\")) == false)
				tmpStringPath = tmpStringPath + "/";
			tmpStringPath = tmpStringPath + fileName;
			file.setFileName(tmpStringPath);
			if(file.exists())
			{
				bIncludePathUsed = true;
				break;
			}
		}
	}
	if(bIncludePathUsed || file.exists())
	{
		if ( file.open(QFile::ReadOnly | QFile::Truncate) ) 
		{       
			// file opened successfully
			QTextStream textStream(&file);        // use a text stream
			// until end of file...
			//while ( !textStream.eof() ) {           
			// read and parse the command line
			//fileText = textStream.readLine();         // line of text excluding '\n'
			// do something with the line
			//...
			//}

			fileText = textStream.readAll();
			// Close the file
			file.close();
		}
	}
	ctx->setActivationObject(ctx->parentContext()->activationObject());
	ctx->setThisObject(ctx->parentContext()->thisObject());
	return eng->evaluate(fileText, fileName);
}

QScriptValue myPrintFunction(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine);
	QString result;
	for (int i = 0; i < context->argumentCount(); ++i)
	{
		if (i > 0)
			result.append(" ");
		result.append(context->argument(i).toString());
	}
	QScriptValue calleeData = context->callee().data();
	QTextEdit *outputObject = qobject_cast<QTextEdit*>(calleeData.toQObject());
	outputObject->append(result);
	return QScriptValue(true);
}


QScriptValue myExitScriptFunction(QScriptContext *context, QScriptEngine *engine)
{
	context->setActivationObject(context->parentContext()->activationObject());
	context->setThisObject(context->parentContext()->thisObject());
	engine->collectGarbage();
	return 0;
}

#ifdef DEBUG
QScriptValue myScriptTestFunction(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine);
	Q_UNUSED(context);
	return NULL;
}
#endif

bool MainWindow::setupNetworkServer(const QString &sAddress, quint16 port)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	if (globAppInfo->shouldEnableNetworkServer())
	{
		tcpServer = new NetworkServer(this);
		QHostAddress address = QHostAddress::Any;
		if(sAddress != "")
			address.setAddress(sAddress); 
		if (!tcpServer->listen(address,port)) 
		{
			QString sInfo = QString("Unable to start the network server: %1.").arg(tcpServer->errorString());
			qDebug() << __FUNCTION__ << sInfo;
			//write2OutputWindow(QString("Unable to start the network server: %1.").arg(tcpServer->errorString()));
			QMessageBox msgBox(QMessageBox::Warning,"Network Server",sInfo, QMessageBox::Ok);
			msgBox.exec();
			return false;
		}
		QString ipAddress;
		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		// use the first non-localhost IPv4 address
		for (int i = 0; i < ipAddressesList.size(); ++i) 
		{
			if (ipAddressesList.at(i) != QHostAddress::LocalHost &&	ipAddressesList.at(i).toIPv4Address()) 
			{
					ipAddress = ipAddressesList.at(i).toString();
					break;
			}
		}
		// if we did not find one, use IPv4 localhost
		if (ipAddress.isEmpty())
			ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

		bool bResult;
		//bResult = connect(tcpServer, SIGNAL(newConnection()), tcpServer, SLOT(newIncomingServerConnection()));
		bResult = connect(tcpServer, SIGNAL(NetworkDataAvailable(int,QString)), this, SLOT(externalNetworkDataRecieved(int,QString)));
		QString usedAddress = sAddress;
		if(usedAddress == "")
			usedAddress = QString("%1").arg(ipAddress);
		QString sInfo = QString("Network Server started @ %1:%2").arg(usedAddress).arg(tcpServer->serverPort());
		qDebug() << sInfo;
		//write2OutputWindow(sInfo);
		//QMessageBox msgBox(QMessageBox::Information,"Network Server",QString("Network Server started @ : %1:%2").arg(ipAddress).arg(tcpServer->serverPort()), QMessageBox::Ok);
		//msgBox.exec();
		return true;
	}
	return false;
}

void MainWindow::externalNetworkDataRecieved(int nClientIndex, QString sAvailableData)
{
	Q_UNUSED(nClientIndex);
	//write2OutputWindow("-> Network Data Recieved(from client: " + QString::number(nClientIndex) +  ") going to execute...");
	QScriptValue scriptVal = executeScriptContent(sAvailableData);
	QString strResult = scriptVal.toString();
	if (scriptVal.isError()) 
	{
		write2OutputWindow("... Script stopped Evaluating due to an error: --> " + strResult + "...");
	}
	//else
	//{
	//	write2OutputWindow("-> Network Data Successfully executed by the Script Engine.");
	//}
}

void MainWindow::shutdownNetworkServer()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	if(tcpServer)
	{
		delete tcpServer;
		tcpServer = NULL;
	}
}

void MainWindow::setupScriptEngine()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	//qDebug() << "setupScriptEngine() - start";
	if(AppScriptEngine)
	{
		delete AppScriptEngine;
		AppScriptEngine = NULL;
	}
	AppScriptEngine = new QTScriptEngine(this);
	if (BrainStimFlags.testFlag(GlobalApplicationInformation::DisableAllScriptBindings) == false)
	{
		if (globAppInfo->shouldLoadScriptBindings())
		{
			if (!AppScriptEngine->ImportScriptExtensions())
			{
				QMessageBox msgBox(QMessageBox::Warning,"QT Script Extensions","Some of the defined QT Script Extensions could not be loaded!", QMessageBox::Ok);
				msgBox.exec();
			}
		}
	}
	lCurrentRunningScriptIDList.clear();
	currentMainRunningScriptID = 0;
	connect(AppScriptEngine, SIGNAL(ScriptLoaded(qint64)), this, SLOT(scriptLoaded(qint64)));
	connect(AppScriptEngine, SIGNAL(ScriptUnloaded(qint64)), this, SLOT(scriptUnloaded(qint64)));

	QScriptValue logVal = AppScriptEngine->eng->newFunction(myPrintFunction);//wrap function to QScriptValue
	logVal.setData(AppScriptEngine->eng->newQObject(mTabNameToOutputWindowList.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME)));//this data is not script-accessible, but the newQObject can be retrieved using QScriptContext::callee() 
	AppScriptEngine->eng->globalObject().setProperty("Log", logVal);

	//QScriptValue keypressVal = AppScriptEngine->eng->newFunction(myKeyPressFunction);//wrap function to QScriptValue
	//AppScriptEngine->eng->globalObject().setProperty("KeyPress", keypressVal);

	QScriptValue beepVal = AppScriptEngine->eng->newFunction(myBeepFunction);
	AppScriptEngine->eng->globalObject().setProperty("Beep", beepVal);

	QScriptValue pauseVal = AppScriptEngine->eng->newFunction(myPauseFunction);
	AppScriptEngine->eng->globalObject().setProperty("Pause", pauseVal);

	QScriptValue throwVal = AppScriptEngine->eng->newFunction(myThrowScriptErrorFunction);
	AppScriptEngine->eng->globalObject().setProperty("ThrowError", throwVal);

	QScriptValue includeVal = AppScriptEngine->eng->newFunction(myIncludeFunction);
	AppScriptEngine->eng->globalObject().setProperty("Include", includeVal);

	QScriptValue exitVal = AppScriptEngine->eng->newFunction(myExitScriptFunction);
	AppScriptEngine->eng->globalObject().setProperty("ExitScript", exitVal);

	QScriptValue AppScriptThisObject = AppScriptEngine->eng->newQObject(this);
	AppScriptEngine->eng->globalObject().setProperty(MAIN_PROGRAM_INTERNAL_NAME, AppScriptThisObject);

#ifdef DEBUG
	QScriptValue TestFunctionVal = AppScriptEngine->eng->newFunction(myScriptTestFunction);
	AppScriptEngine->eng->globalObject().setProperty("Test", TestFunctionVal);
#endif

	//We must declare it so that the type will be known to QMetaType, find "Q_DECLARE_METATYPE(DocFindFlags)"
	//Q_DECLARE_METATYPE(DocFindFlags)
	//Next, the DocFindFlags conversion functions. We represent the DocFindFlags value as a script object and just copy the properties, see below after this function body!

	//Now we can register DocFindFlags with the engine:	
	qScriptRegisterMetaType(AppScriptEngine->eng, sciFindDialog::DocFindFlagstoScriptValue, sciFindDialog::DocFindFlagsfromScriptValue);
	
	//Working with DocFindFlags values is now easy:
	////DocFindFlags s = qscriptvalue_cast<DocFindFlags>(AppScriptEngine->eng->currentContext()->argument(0));
	////DocFindFlags s2;
	////s2.x = s.x + 10;
	////s2.y = s.y + 20;
	////QScriptValue v = engine->toScriptValue(s2);
	//If you want to be able to construct values of your custom type from script code, you have to register a constructor function for the type. For example:
	//QScriptValue createMyStruct(QScriptContext *, QScriptEngine *engine)
	//{
	//	MyStruct s;
	//	s.x = 123;
	//	s.y = 456;
	//	return engine->toScriptValue(s);
	//}
	//QScriptValue ctor = AppScriptEngine->eng.newFunction(createMyStruct);
	//engine.globalObject().setProperty("MyStruct", ctor);
	QScriptValue ctor = AppScriptEngine->eng->newFunction(sciFindDialog::DocFindFlagsConstructor);
	AppScriptEngine->eng->globalObject().setProperty("DocFindFlags", ctor);

	//int nShortListId = 
		qScriptRegisterSequenceMetaType<QList<short>>(AppScriptEngine->eng);
	//int nFloatListId = 
		qScriptRegisterSequenceMetaType<QList<float>>(AppScriptEngine->eng);
	//int nDoubleListId = 
		qScriptRegisterSequenceMetaType<QList<double>>(AppScriptEngine->eng);
}

bool MainWindow::restartScriptEngine()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	setupScriptEngine();
	for (int i=0;i<Plugins->Count()-1;i++)
	{
		configurePluginScriptEngine(i);
	}
	configureDebugger();
	updateMenuControls();
	resetContextState();
	return true;
}

bool MainWindow::emulateKeyPress(QWindow *pWindow, const int keyCode, const QString &sKeys, const Qt::KeyboardModifiers modifier, const int nDelay)
{
	QTestEventList events;
	events.addKeyClicks(sKeys);
	events.addDelay(500);
	if(pWindow)
	{
		QTest::sendKeyEvent(QTest::Click,pWindow,(Qt::Key)keyCode,sKeys,modifier,nDelay);
	}
	else
	{
		QMdiSubWindow *currentSub = activeMdiChild();
		CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(currentSub));
		events.simulate(tmpCustomQsciScintilla);
	}
	return true;
}

void MainWindow::scriptLoaded(qint64 id)
{
	lCurrentRunningScriptIDList.append(id);
	if(AppScriptStatus != GlobalApplicationInformation::Executing)
		currentMainRunningScriptID = id;
    setScriptRunningStatus(GlobalApplicationInformation::Executing);
}

void MainWindow::scriptUnloaded(qint64 id)
{
	int nIndex = lCurrentRunningScriptIDList.indexOf(id);
	if(nIndex >= 0)
	{
		lCurrentRunningScriptIDList.removeAt(nIndex);
	}
	if(lCurrentRunningScriptIDList.isEmpty())
	{
		resetContextState();
		setScriptRunningStatus(GlobalApplicationInformation::Pending);
	}
	else
	{
		resetContextState(id);
	}
}

void MainWindow::outputTabCloseRequest(int nIndex)
{
	QString sTabName = mTabNameToOutputWindowList.key((QTextEdit *)outputTabWidget->widget(nIndex),QString(""));
	if(sTabName.isEmpty())
		return;
	if(sTabName == MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME)
	{
		clearDebugger(sTabName);
	}
	else
	{
		removeOutputWindow(sTabName);
	}
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
	if (window)
	{
		QMdiSubWindow *tmpSubWin = qobject_cast<QMdiSubWindow *>(window);
		if (tmpSubWin)
			mdiArea->setActiveSubWindow(tmpSubWin);
	}
}

void MainWindow::unregisterDockWidgets(CustomMDISubWindow* custSubWin)
{
	QString sTmpDocTypeString = DocManager->getDocTypeString(DocManager->getDocType(custSubWin));
	if (sTmpDocTypeString.isEmpty())
	{
		QStringList lFileNameItems = DocManager->getFileName(custSubWin, false).split(".", QString::SkipEmptyParts);
		if (lFileNameItems.count() > 1)
			sTmpDocTypeString = lFileNameItems.at(lFileNameItems.count() - 1);
	}
	if (sTmpDocTypeString.isEmpty() == false)
	{
		QList<QDockWidget*> lTmpDockWidgetList = mapMDISubWindowToDockWidget.values(custSubWin);
		foreach(QDockWidget* tmpDockWidget, lTmpDockWidgetList)
		{
			if (tmpDockWidget)
				saveDockWidgetConfiguration(UI_SETTINGS_FILENAME, sTmpDocTypeString, tmpDockWidget);
		}
		saveDockWidgetConfiguration(UI_SETTINGS_FILENAME, sTmpDocTypeString, debugLogDock);
		saveWindowLayout(UI_SETTINGS_FILENAME, sTmpDocTypeString);
	}
}

bool MainWindow::addRegisteredDockWidgets(const Qt::DockWidgetArea debugDockArea)
{
	bool bOutputDockAdded = false;
	QMap<QDockWidget*, DocumentManager::strcDockLocation> mapRegisteredDockWidgetToLocationStruct = DocManager->getDockWidgetRegistrations(activeMdiChild());
	while (mapRegisteredDockWidgetToLocationStruct.isEmpty() == false)
	{
		bool bIsFirst = true;
		QMap<QDockWidget*, DocumentManager::strcDockLocation>::iterator iter;
		DocumentManager::strcDockLocation tmpLocation;
		QDockWidget* tmpDockWidget = NULL;
		for (iter = mapRegisteredDockWidgetToLocationStruct.begin(); iter != mapRegisteredDockWidgetToLocationStruct.end(); ++iter)
		{
			if (bIsFirst)
			{
				tmpLocation = iter.value();
				tmpDockWidget = iter.key();
				bIsFirst = false;
			}
			else
			{
				if ((iter.value().dockArea == tmpLocation.dockArea) && (tmpDockWidget != iter.key()))
				{
					if ((tmpLocation.dockArea == Qt::DockWidgetArea::LeftDockWidgetArea) || (tmpLocation.dockArea == Qt::DockWidgetArea::RightDockWidgetArea))
					{
						if (iter.value().rGeometry.y() < tmpLocation.rGeometry.y())
						{
							tmpLocation = iter.value();
							tmpDockWidget = iter.key();
						}
					}
					else if ((tmpLocation.dockArea == Qt::DockWidgetArea::BottomDockWidgetArea) || (tmpLocation.dockArea == Qt::DockWidgetArea::TopDockWidgetArea))
					{
						if (iter.value().rGeometry.x() < tmpLocation.rGeometry.x())
						{
							tmpLocation = iter.value();
							tmpDockWidget = iter.key();
						}
					}
				}
			}
		}
		bool bPlaceOutputDockFirst = false;
		if (bOutputDockAdded == false)
		{
			if (debugDockArea == tmpLocation.dockArea)
			{
				if ((tmpLocation.dockArea == Qt::DockWidgetArea::LeftDockWidgetArea) || (tmpLocation.dockArea == Qt::DockWidgetArea::RightDockWidgetArea))
				{
					if (debugLogDock->geometry().y() < tmpLocation.rGeometry.y())
						bPlaceOutputDockFirst = true;
				}
				else if ((tmpLocation.dockArea == Qt::DockWidgetArea::BottomDockWidgetArea) || (tmpLocation.dockArea == Qt::DockWidgetArea::TopDockWidgetArea))
				{
					if (debugLogDock->geometry().x() < tmpLocation.rGeometry.x())
						bPlaceOutputDockFirst = true;
				}
			}
			if (bPlaceOutputDockFirst)
			{
				addDockWidget(debugDockArea, debugLogDock);
				debugLogDock->show();
				bOutputDockAdded = true;
			}
		}
		addDockWidget(tmpLocation.dockArea, tmpDockWidget);
		mapRegisteredDockWidgetToLocationStruct.remove(tmpDockWidget);
	}
	if (bOutputDockAdded == false)
	{
		addDockWidget(debugDockArea, debugLogDock);
		debugLogDock->show();
	}
	return true;
}

bool MainWindow::registerDockWidget(QWidget *pMDIWindowSubWidget, QDockWidget *pDockWidget, int nDockWidgetAreaEnum)
{
	if(pMDIWindowSubWidget && pDockWidget && DocManager)
	{
		QMdiSubWindow *tmpMDISubWindow = DocManager->getDocSubWindow(pMDIWindowSubWidget);
		if(tmpMDISubWindow)
		{
			Qt::DockWidgetArea dArea;
			mapMDISubWindowToDockWidget.insertMulti(tmpMDISubWindow,pDockWidget);
			QString sTmpDocTypeString = DocManager->getDocTypeString(DocManager->getDocType(tmpMDISubWindow));
			if (sTmpDocTypeString.isEmpty())
			{
				QStringList lFileNameItems = DocManager->getFileName(tmpMDISubWindow, false).split(".", QString::SkipEmptyParts);
				if (lFileNameItems.count() > 1)
					sTmpDocTypeString = lFileNameItems.at(lFileNameItems.count() - 1);
			}
			if((nDockWidgetAreaEnum < 0) && (debugLogDock))
			{
				dArea = Qt::BottomDockWidgetArea;
				//QString sTmpDocTypeString = DocManager->getDocTypeString(DocManager->getDocType(tmpMDISubWindow));
				if (sTmpDocTypeString.isEmpty() == false)
					loadSavedDockWidgetConfiguration(UI_SETTINGS_FILENAME, sTmpDocTypeString, pDockWidget, dArea);
				addDockWidget(dArea, pDockWidget);
				pDockWidget->show();
				tabifyDockWidget(debugLogDock, pDockWidget);
			}
			else
			{
				dArea = (Qt::DockWidgetArea)nDockWidgetAreaEnum;
				if (sTmpDocTypeString.isEmpty() == false)
				{
					loadSavedDockWidgetConfiguration(UI_SETTINGS_FILENAME, sTmpDocTypeString, pDockWidget, dArea);
					DocumentManager::strcDockLocation tmpDockLocation;
					tmpDockLocation.dockArea = dArea;
					tmpDockLocation.rGeometry = pDockWidget->geometry();
					if (activeMdiChild())
						DocManager->addDockWidgetRegistration(activeMdiChild(), pDockWidget, tmpDockLocation);
				}
				else
				{
					addDockWidget(dArea, pDockWidget);
				}
			}
			connect(pDockWidget, SIGNAL(destroyed(QObject *)), this, SLOT(dockWidgetDestroyed(QObject *)));
			return true;
		}
	}
	return false;
}

void MainWindow::dockWidgetDestroyed(QObject *destoyedDockWidget)
{
	if(destoyedDockWidget == NULL)
		return;
	for(int i=0;i<mapMDISubWindowToDockWidget.count();i++)
	{
		if(mapMDISubWindowToDockWidget.values()[i] = (QDockWidget *)destoyedDockWidget)
		{
			mapMDISubWindowToDockWidget.remove(mapMDISubWindowToDockWidget.keys()[i],(QDockWidget *)destoyedDockWidget);
		}
	}
}

void MainWindow::subDocumentWindowActivated(QMdiSubWindow *subWindow)
{
	DocManager->removeAllMenuActionRegistrations();
	if(subWindow)
	{
		QList<DocumentManager::strcPluginChildDocCustomMenuDef> lPluginChildDocCustomMenuDefs = DocManager->getMenuActionRegistrations(subWindow);
		for (int j = 0; j<lPluginChildDocCustomMenuDefs.count(); j++)
		{
			if (lPluginChildDocCustomMenuDefs[j].customRootMenu)
			{
				if (lPluginChildDocCustomMenuDefs[j].parentMenu)
					lPluginChildDocCustomMenuDefs[j].parentMenu->addMenu(lPluginChildDocCustomMenuDefs[j].customRootMenu);
				else
					menuBar()->addMenu(lPluginChildDocCustomMenuDefs[j].customRootMenu);
			}
			else if (lPluginChildDocCustomMenuDefs[j].customAction)
			{
				if (lPluginChildDocCustomMenuDefs[j].parentMenu)
					lPluginChildDocCustomMenuDefs[j].parentMenu->addAction(lPluginChildDocCustomMenuDefs[j].customAction);
				else
					menuBar()->addAction(lPluginChildDocCustomMenuDefs[j].customAction);
			}
			DocManager->appendAllMenuActionRegistration(lPluginChildDocCustomMenuDefs[j]);
		}

		for(int i=0;i<mapMDISubWindowToDockWidget.count();i++)
		{
			if(mapMDISubWindowToDockWidget.keys().at(i) == subWindow)
			{
				if (dockWidgetArea(mapMDISubWindowToDockWidget.values()[i]) == Qt::NoDockWidgetArea)
				{
					QMap<QDockWidget*, DocumentManager::strcDockLocation> mapRegisteredDockWidgetToLocationStruct = DocManager->getDockWidgetRegistrations(subWindow);
					if (mapRegisteredDockWidgetToLocationStruct.contains(mapMDISubWindowToDockWidget.values()[i]))
					{
						DocumentManager::strcDockLocation tmpLocationStructure = mapRegisteredDockWidgetToLocationStruct.value(mapMDISubWindowToDockWidget.values()[i]);
						addDockWidget(tmpLocationStructure.dockArea, mapMDISubWindowToDockWidget.values()[i]);
					}
					else
					{
						addDockWidget(Qt::BottomDockWidgetArea, mapMDISubWindowToDockWidget.values()[i]);
					}
					mapMDISubWindowToDockWidget.values()[i]->show();
				}
			}
			else
			{
				if (dockWidgetArea(mapMDISubWindowToDockWidget.values()[i]) != Qt::NoDockWidgetArea)
					removeDockWidget(mapMDISubWindowToDockWidget.values()[i]);
			}
		}
	}
	updateMenuControls();
}

void MainWindow::updateMenuControls(QMdiSubWindow *mdiSubWin)
{ 
	if (DocManager == NULL)
		return;
	CustomQsciScintilla *tmpCustomQsciScintilla = NULL;
	bool hasMdiChild = false;

	if (DocManager->count() > 0)
	{
		tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(mdiSubWin));
		if (mdiSubWin)
			hasMdiChild = true;
	}
	if ((tmpCustomQsciScintilla) || (hasMdiChild == false))
	{
		StatusLinesLabel->setHidden(!hasMdiChild);
		StatusPositionLabel->setHidden(!hasMdiChild);
		StatusFilePathLabel->setHidden(!hasMdiChild);

		addRemMarkerAction->setEnabled(hasMdiChild);
		nextMarkerAction->setEnabled(hasMdiChild);
		prevMarkerAction->setEnabled(hasMdiChild);
		remAllMarkerAction->setEnabled(hasMdiChild);

		foreach(QAction* tmpClearAction, mClearDebuggerAction)
			tmpClearAction->setEnabled(true);
		foreach(QAction* tmpCopyAction, mCopyDebuggerAction)
			tmpCopyAction->setEnabled(true);
		foreach(QAction* tmpSaveAction, mSaveDebuggerAction)
			tmpSaveAction->setEnabled(true);
		//foreach(QAction* tmpRemoveAction,mRemoveDebuggerAction)
		//	tmpRemoveAction->setEnabled(true);		

		goToLineAction->setEnabled(hasMdiChild);
		goToMatchingBraceAction->setEnabled(hasMdiChild);
		selToMatchingBraceAction->setEnabled(hasMdiChild);
		lineCommentAction->setEnabled(hasMdiChild);
		blockCommentAction->setEnabled(hasMdiChild);
		duplicateLineAction->setEnabled(hasMdiChild);
		//moveLineUpAction->setEnabled(hasMdiChild);
		deleteCurrentLineAction->setEnabled(hasMdiChild);
		toUpperCaseAction->setEnabled(hasMdiChild);
		toLowerCaseAction->setEnabled(hasMdiChild);

		findAction->setEnabled(hasMdiChild);
		findNextAction->setEnabled(hasMdiChild);
		findPrevAction->setEnabled(hasMdiChild);
		replaceAction->setEnabled(hasMdiChild);

		//searchAction->setEnabled(hasMdiChild);
		saveAction->setEnabled(hasMdiChild);
		saveAsAction->setEnabled(hasMdiChild);
		pasteAction->setEnabled(hasMdiChild);
		closeAction->setEnabled(hasMdiChild);
		reloadAction->setEnabled(hasMdiChild);
		closeAllAction->setEnabled(hasMdiChild);
		//tileAction->setEnabled(hasMdiChild);
		//cascadeAction->setEnabled(hasMdiChild);
		nextAction->setEnabled(hasMdiChild);
		previousAction->setEnabled(hasMdiChild);
		separatorAct->setVisible(hasMdiChild);

		bool hasSelection = false;
		CustomQsciScintilla *tmpCustomQsciScintilla = NULL;
		if (DocManager->count() > 0)
		{
			tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(mdiSubWin));
			if (tmpCustomQsciScintilla)
				hasSelection = (hasMdiChild && (tmpCustomQsciScintilla->hasSelectedText()));
		}

		cutAction->setEnabled(hasSelection);
		copyAction->setEnabled(hasSelection);
	}
	else
	{
		StatusLinesLabel->setHidden(true);
		StatusPositionLabel->setHidden(true);
		StatusFilePathLabel->setHidden(false);
		addRemMarkerAction->setEnabled(false);
		nextMarkerAction->setEnabled(false);
		prevMarkerAction->setEnabled(false);
		remAllMarkerAction->setEnabled(false);
		foreach(QAction* tmpClearAction, mClearDebuggerAction)
			tmpClearAction->setEnabled(true);
		foreach(QAction* tmpCopyAction, mClearDebuggerAction)
			tmpCopyAction->setEnabled(true);
		foreach(QAction* tmpSaveAction, mClearDebuggerAction)
			tmpSaveAction->setEnabled(true);
		goToLineAction->setEnabled(false);
		goToMatchingBraceAction->setEnabled(false);
		selToMatchingBraceAction->setEnabled(false);
		lineCommentAction->setEnabled(false);
		blockCommentAction->setEnabled(false);
		duplicateLineAction->setEnabled(false);
		//moveLineUpAction->setEnabled(false);
		deleteCurrentLineAction->setEnabled(false);
		toUpperCaseAction->setEnabled(false);
		toLowerCaseAction->setEnabled(false);
		//searchAction->setEnabled(hasMdiChild);
		saveAction->setEnabled(hasMdiChild);
		saveAsAction->setEnabled(hasMdiChild);
		pasteAction->setEnabled(false);
		closeAction->setEnabled(hasMdiChild);
		reloadAction->setEnabled(hasMdiChild);
		closeAllAction->setEnabled(hasMdiChild);
		//tileAction->setEnabled(hasMdiChild);
		//cascadeAction->setEnabled(hasMdiChild);
		nextAction->setEnabled(hasMdiChild);
		previousAction->setEnabled(hasMdiChild);
		separatorAct->setVisible(hasMdiChild);

		cutAction->setEnabled(false);
		copyAction->setEnabled(false);

		findAction->setEnabled(false);
		findNextAction->setEnabled(false);
		findPrevAction->setEnabled(false);
		replaceAction->setEnabled(false);
	}
	if (hasMdiChild)
	{
		QString docFilePath = DocManager->getFileName(mdiSubWin);
		QString docTitle = strippedName(docFilePath);
		if (docTitle == "")
			docTitle = MainAppInfo::UntitledDocName();
		setWindowTitle(tr("%1 - %2").arg(globAppInfo->getTitle()).arg(docTitle));
		GlobalApplicationInformation::DocType d;
		d = DocManager->getDocType(mdiSubWin);
		printAction->setEnabled(false);
		switch (d)
		{
		case GlobalApplicationInformation::DOCTYPE_UNDEFINED:
		{
			if (tmpCustomQsciScintilla)
				printAction->setEnabled(false);
			setScriptRunningStatus(GlobalApplicationInformation::NoScript);
			break;
		}
		default:
		{
			if (lCurrentRunningScriptIDList.isEmpty())
				setScriptRunningStatus(GlobalApplicationInformation::Pending);
			if (tmpCustomQsciScintilla)
				printAction->setEnabled(true);
			break;
		}
		}

		CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(mdiSubWin));
		if (tmpCustomQsciScintilla)
			NumberOfLinesChangedHandler(tmpCustomQsciScintilla->lines());
		StatusFilePathLabel->setText(QString("%1").arg(docFilePath));
	}
	else
	{
		setWindowTitle(tr("%1").arg(globAppInfo->getTitle()));
		if (lCurrentRunningScriptIDList.isEmpty())
			setScriptRunningStatus(GlobalApplicationInformation::NoScript);
	}
}

void MainWindow::updateMenuControls()
{
	QMdiSubWindow *currentSub = activeMdiChild();
	updateMenuControls(currentSub);
}

void MainWindow::CursorPositionChangedHandler(int line, int col) 
{
	StatusPositionLabel->setText(tr("Row: %1, Col: %2").arg(line+1).arg(col+1));
}

bool MainWindow::setDefaultGLFormat()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;

	//Doesn't work..
	//QString versionString1(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
	//qDebug() << "Graphics Driver Version String:" << versionString1;
	
	QGLFormat BrainStimQGLFormat;
	QString sOpenGLVersionString = QString::number(BrainStimQGLFormat.majorVersion()) + "." + QString::number(BrainStimQGLFormat.minorVersion());
	qDebug() << "Supported OpenGL version: " << sOpenGLVersionString;
	BrainStimQGLFormat.setSwapInterval(1); // sync with vertical refresh
	BrainStimQGLFormat.setSampleBuffers(true);

	//bool bHasOpenGLFramebufferObjects = QGLFramebufferObject::hasOpenGLFramebufferObjects();
	BrainStimQGLFormat.setDoubleBuffer(true);


	if (!QGLFormat::hasOpenGL())// || !QGLFramebufferObject::hasOpenGLFramebufferObjects())
	{
		qDebug() << "This system does not support OpenGL.";///framebuffer objects
	}

	//BrainStimQGLFormat.setVersion(3,1);
	//int a = BrainStimQGLFormat.majorVersion();
	//int aa = BrainStimQGLFormat.minorVersion();
	//QStringList lSupportedOpenGLVersions;
	//QGLFormat::OpenGLVersionFlags aaa = QGLFormat::openGLVersionFlags();

	//QGLFormat::OpenGL_Version_None	0x00000000	If no OpenGL is present or if no OpenGL context is current.
	//QGLFormat::OpenGL_Version_1_1	0x00000001	OpenGL version 1.1 or higher is present.
	//QGLFormat::OpenGL_Version_1_2	0x00000002	OpenGL version 1.2 or higher is present.
	//QGLFormat::OpenGL_Version_1_3	0x00000004	OpenGL version 1.3 or higher is present.
	//QGLFormat::OpenGL_Version_1_4	0x00000008	OpenGL version 1.4 or higher is present.
	//QGLFormat::OpenGL_Version_1_5	0x00000010	OpenGL version 1.5 or higher is present.
	//QGLFormat::OpenGL_Version_2_0	0x00000020	OpenGL version 2.0 or higher is present. Note that version 2.0 supports all the functionality of version 1.5.
	//QGLFormat::OpenGL_Version_2_1	0x00000040	OpenGL version 2.1 or higher is present.
	//QGLFormat::OpenGL_Version_3_0	0x00001000	OpenGL version 3.0 or higher is present.
	//QGLFormat::OpenGL_Version_3_1	0x00002000	OpenGL version 3.1 or higher is present. Note that OpenGL version 3.1 or higher does not necessarily support all the features of version 3.0 and lower.
	//QGLFormat::OpenGL_Version_3_2	0x00004000	OpenGL version 3.2 or higher is present.
	//QGLFormat::OpenGL_Version_3_3	0x00008000	OpenGL version 3.3 or higher is present.
	//QGLFormat::OpenGL_Version_4_0	0x00010000	OpenGL version 4.0 or higher is present.
	//QGLFormat::OpenGL_Version_4_1	0x00020000	OpenGL version 4.1 or higher is present.
	//QGLFormat::OpenGL_Version_4_2	0x00040000	OpenGL version 4.2 or higher is present.
	//QGLFormat::OpenGL_Version_4_3	0x00080000	OpenGL version 4.3 or higher is present.
	//QGLFormat::OpenGL_ES_CommonLite_Version_1_0	0x00000100	OpenGL ES version 1.0 Common Lite or higher is present.
	//QGLFormat::OpenGL_ES_Common_Version_1_0	0x00000080	OpenGL ES version 1.0 Common or higher is present. The Common profile supports all the features of Common Lite.
	//QGLFormat::OpenGL_ES_CommonLite_Version_1_1	0x00000400	OpenGL ES version 1.1 Common Lite or higher is present.
	//QGLFormat::OpenGL_ES_Common_Version_1_1	0x00000200	OpenGL ES version 1.1 Common or higher is present. The Common profile supports all the features of Common Lite.
	//QGLFormat::OpenGL_ES_Version_2_0	0x00000800	OpenGL ES version 2.0 or higher is present. Note that OpenGL ES version 2.0 does not support all the features of OpenGL ES 1.x. So if OpenGL_ES_Version_2_0 is returned, none of the ES 1.x flags are returned.


	//int aaaa = 0x00000077;
	//setVersion()
	QGLFormat::setDefaultFormat(BrainStimQGLFormat);
	return true;
}

void MainWindow::createDefaultMenus()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	fileMenu = new QMenu(tr("&File"), this);
	editMenu = new QMenu(tr("&Edit"), this);
	markersMenu = new QMenu(tr("&Markers"), this);
	//viewMenu = new QMenu(tr("&View"), this);
	documentMenu = new QMenu(tr("&Document"), this);
	toolsMenu = new QMenu(tr("&Tools"), this);
	windowMenu = new QMenu(tr("&Window"), this);

#ifdef DEBUG
	newTestAction = new QAction(tr("Test(debug)"), this);
	newTestAction->setStatusTip(tr("Only visible/compiled inside debug version, calls the slot debugTestSlot()"));
	//newTestAction->setData(QVariant(tmpNewActionMapping));
	connect(newTestAction, SIGNAL(triggered()), this, SLOT(debugTestSlot()));
	fileMenu->addAction(newTestAction);
#endif

	fileNewMenu = fileMenu->addMenu(QIcon(":/resources/new.png"),tr("&New"));
	newDocumentAction = new QAction(tr("Undefined Document"), this);
	newDocumentAction->setStatusTip(tr("Create an empty document"));
	tmpNewActionMapping.clear();
	tmpNewActionMapping.insert("",GlobalApplicationInformation::DOCTYPE_UNDEFINED);
	newDocumentAction->setData(QVariant(tmpNewActionMapping));
	connect(newDocumentAction, SIGNAL(triggered()), this, SLOT(newFile()));
	fileNewMenu->addAction(newDocumentAction);

	tmpNewActionMapping.clear();
	tmpNewActionMapping.insert("",GlobalApplicationInformation::DOCTYPE_QTSCRIPT);
	newQtScriptAction = new QAction(tr("QtScript Document"), this);
	newQtScriptAction->setStatusTip(tr("Create an empty QtScript document"));
	newQtScriptAction->setData(QVariant(tmpNewActionMapping));
	connect(newQtScriptAction, SIGNAL(triggered()), this, SLOT(newFile()));
	fileNewMenu->addAction(newQtScriptAction);

	tmpNewActionMapping.clear();
	tmpNewActionMapping.insert("",GlobalApplicationInformation::DOCTYPE_JAVASCRIPT);
	newJavaScriptAction = new QAction(tr("JavaScript Document"), this);
	newJavaScriptAction->setStatusTip(tr("Create an empty JavaScript document"));
	newJavaScriptAction->setData(QVariant(tmpNewActionMapping));
	connect(newJavaScriptAction, SIGNAL(triggered()), this, SLOT(newFile()));
	fileNewMenu->addAction(newJavaScriptAction);

	tmpNewActionMapping.clear();
	tmpNewActionMapping.insert("",GlobalApplicationInformation::DOCTYPE_SVG);
	newSVGAction = new QAction(tr("SVG Document"), this);
	newSVGAction->setStatusTip(tr("Create an empty SVG document"));
	newSVGAction->setData(QVariant(tmpNewActionMapping));
	connect(newSVGAction, SIGNAL(triggered()), this, SLOT(newFile()));
	fileNewMenu->addAction(newSVGAction);


	openAction = new QAction(QIcon(":/resources/open.png"),tr("&Open..."), this);
	openAction->setShortcut(QKeySequence(QKeySequence::Open));
	openAction->setStatusTip(tr("Open an existing file"));
	connect(openAction, SIGNAL(triggered()), this, SLOT(openFiles()));
	fileMenu->addAction(openAction);

	reloadAction = new QAction(QIcon(":/resources/reload.png"), tr("Reload"), this);
	//reloadAction->setData("Reload");
	reloadAction->setShortcut(tr("Ctrl+R"));
	reloadAction->setStatusTip(tr("Close the active window"));
	connect(reloadAction, SIGNAL(triggered()), this, SLOT(reloadSubWindow()));
	fileMenu->addAction(reloadAction);

	saveAction = new QAction(QIcon(":/resources/save.png"), tr("&Save"), this);
	saveAction->setShortcuts(QKeySequence::Save);
	saveAction->setStatusTip(tr("Save the document to disk"));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
	fileMenu->addAction(saveAction);

	saveAsAction = new QAction(QIcon(":/resources/saveas.png"), tr("Save &As..."), this);
	saveAsAction->setShortcut(tr("Ctrl+Shift+s"));
	saveAsAction->setStatusTip(tr("Save the document under a new name"));
	connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));
	fileMenu->addAction(saveAsAction);

	printAction = new QAction(QIcon(":/resources/print.png"), tr("&Print..."), this);
	printAction->setShortcuts(QKeySequence::Print);
	printAction->setStatusTip(tr("Print the current document"));
	printAction->setEnabled(false);
	connect(printAction, SIGNAL(triggered()), this, SLOT(print()));
	fileMenu->addAction(printAction);

	for (int i = 0; i < MaxRecentFiles; ++i) 
	{
		recentFileActs.append(new QAction(this));
		recentFileActs.last()->setVisible(false);
		recentFileActs.last()->setShortcut(QString("Ctrl+Shift+") + QString::number(i+1));
		connect(recentFileActs.last(), SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}

	separatorAct = fileMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
	{
		fileMenu->addAction(recentFileActs[i]);
	}
	fileMenu->addSeparator();
	updateRecentFileActions();

	closeAction = new QAction(QIcon(":/resources/close.png"), tr("&Close"), this);
	closeAction->setData("Close");
	closeAction->setShortcut(tr("Ctrl+Shift+C"));
	closeAction->setStatusTip(tr("Close the active window"));
	connect(closeAction, SIGNAL(triggered()), this, SLOT(closeSubWindow()));
	fileMenu->addAction(closeAction);

	closeAllAction = new QAction(QIcon(":/resources/closeall.png"), tr("Close A&ll"), this);
	closeAllAction->setData("CloseAll");
	closeAllAction->setShortcut(tr("Ctrl+Shift+F4"));
	closeAllAction->setStatusTip(tr("Close all the windows"));
	connect(closeAllAction, SIGNAL(triggered()), this, SLOT(closeSubWindow()));
	fileMenu->addAction(closeAllAction);

	quitAction = new QAction(QIcon(":/resources/exit.png"), tr("E&xit"), this);
	quitAction->setShortcut(QKeySequence(tr("Alt+F4")));
	quitAction->setStatusTip(tr("Exit the application"));
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
	fileMenu->addAction(quitAction);
	menuBar()->addMenu(fileMenu);//the file menu..........................................................

	editOutputMenu = editMenu->addMenu(QIcon(":/resources/log.png"), tr("&Output Log Pane"));

	signalMapperCopyDebugger = new QSignalMapper(this);
	mCopyDebuggerAction.insert(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME, new QAction(QIcon(":/resources/logCopy.png"), QObject::tr("&Copy Selected Text"), 0));
	//copyDebuggerAction->setShortcut(QKeySequence(""));
	mCopyDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME)->setStatusTip(tr("Copy the Selected Debugger Output window line(s)."));
	connect(mCopyDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME), SIGNAL(triggered()), signalMapperCopyDebugger, SLOT(map()));
	signalMapperCopyDebugger->setMapping(mCopyDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME), MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	editOutputMenu->addAction(mCopyDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME));
	connect(signalMapperCopyDebugger, SIGNAL(mapped(const QString &)),this, SLOT(copyDebugger(const QString &)));

	signalMapperClearDebugger = new QSignalMapper(this);
	mClearDebuggerAction.insert(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME, new QAction(QIcon(":/resources/logClear.png"), QObject::tr("C&lear All Output Item(s)"), 0));
	//clearDebuggerAction->setShortcut(QKeySequence(""));
	mClearDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME)->setStatusTip(tr("Clear the Debugger Output window."));
	connect(mClearDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME), SIGNAL(triggered()), signalMapperClearDebugger, SLOT(map()));
	signalMapperClearDebugger->setMapping(mClearDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME), MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	editOutputMenu->addAction(mClearDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME));
	connect(signalMapperClearDebugger, SIGNAL(mapped(const QString &)),this, SLOT(clearDebugger(const QString &)));

	signalMapperSaveDebugger = new QSignalMapper(this);
	mSaveDebuggerAction.insert(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME, new QAction(QIcon(":/resources/logSave.png"), QObject::tr("&Save All Output information to a text file..."), 0));
	//mSaveDebuggerAction->setShortcut(QKeySequence(""));
	mSaveDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME)->setStatusTip(tr("Save the Debugger Output window."));
	connect(mSaveDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME), SIGNAL(triggered()), signalMapperSaveDebugger, SLOT(map()));
	signalMapperSaveDebugger->setMapping(mSaveDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME), MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	editOutputMenu->addAction(mSaveDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME));
	connect(signalMapperSaveDebugger, SIGNAL(mapped(const QString &)),this, SLOT(saveDebugger(const QString &)));

	//signalMapperRemoveDebugger = new QSignalMapper(this);
	//signalMapperRemoveDebugger->setMapping(mRemoveDebuggerAction.value(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME), MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	//connect(signalMapperRemoveDebugger, SIGNAL(mapped(const QString &)),this, SLOT(removeOutputWindow(const QString &)));

	editMenu->addSeparator();

	cutAction = new QAction(QIcon(":/resources/cut.png"), tr("Cu&t"), this);
	cutAction->setShortcuts(QKeySequence::Cut);
	cutAction->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
	connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
	editMenu->addAction(cutAction);

	copyAction = new QAction(QIcon(":/resources/copy.png"), tr("&Copy"), this);
	copyAction->setShortcuts(QKeySequence::Copy);
	copyAction->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
	editMenu->addAction(copyAction);

	pasteAction = new QAction(QIcon(":/resources/paste.png"), tr("P&aste"), this);
	pasteAction->setShortcuts(QKeySequence::Paste);
	pasteAction->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
	editMenu->addAction(pasteAction);

	editMenu->addSeparator();

	findAction = new QAction(QIcon(":/resources/find.png"), tr("&Find"), this);
	findAction->setShortcuts(QKeySequence::Find);
	findAction->setStatusTip(tr("Find"));
	connect(findAction, SIGNAL(triggered()), this, SLOT(find()));
	editMenu->addAction(findAction);

	findNextAction = new QAction(QIcon(":/resources/findnext.png"), tr("Find &Next"), this);
	findNextAction->setShortcuts(QKeySequence::FindNext);
	findNextAction->setStatusTip(tr("Find Next"));
	connect(findNextAction, SIGNAL(triggered()), this, SLOT(findNext()));
	editMenu->addAction(findNextAction);

	findPrevAction = new QAction(QIcon(":/resources/findprev.png"), tr("Find &Previous"), this);
	findPrevAction->setShortcuts(QKeySequence::FindPrevious);
	findPrevAction->setStatusTip(tr("Find Previous"));
	connect(findPrevAction, SIGNAL(triggered()), this, SLOT(findPrev()));
	editMenu->addAction(findPrevAction);

	replaceAction = new QAction(QIcon(":/resources/replace.png"), tr("&Replace"), this);
	replaceAction->setShortcuts(QKeySequence::Replace);
	replaceAction->setStatusTip(tr("Replace"));
	connect(replaceAction, SIGNAL(triggered()), this, SLOT(replace()));
	editMenu->addAction(replaceAction);
	
	//searchAction = new QAction(tr("Search"), this);//QIcon(":/resources/comment.png")
	//searchAction->setShortcut(tr("Ctrl+F"));
	//searchAction->setStatusTip(tr("Search through the current document."));
	//connect(searchAction, SIGNAL(triggered()), this, SLOT(search()));
	//editMenu->addAction(searchAction);

	editMenu->addSeparator();

	//commentAction = new QAction(QIcon(":/resources/comment.png"), tr("Comment"), this);
	//commentAction->setShortcut(tr("Ctrl+K"));
	//commentAction->setStatusTip(tr("Comment out the selected lines."));
	//connect(commentAction, SIGNAL(triggered()), this, SLOT(comment()));
	//editMenu->addAction(commentAction);

	//uncommentAction = new QAction(QIcon(":/resources/uncomment.png"), tr("Uncomment"), this);
	//uncommentAction->setShortcut(tr("Ctrl+U"));
	//uncommentAction->setStatusTip(tr("Uncomment out the selected lines."));
	//connect(uncommentAction, SIGNAL(triggered()), this, SLOT(unComment()));
	//editMenu->addAction(uncommentAction);
	goToLineAction = new QAction(QObject::tr("&Go to Line"), 0);
	goToLineAction->setShortcut(QKeySequence("CTRL+N"));
	goToLineAction->setStatusTip(tr("Go to Line."));
	connect(goToLineAction, SIGNAL(triggered()), this, SLOT(goToLine()));
	editMenu->addAction(goToLineAction);
	goToMatchingBraceAction = new QAction(QObject::tr("&Jump to the matching brace"), 0);
	goToMatchingBraceAction->setShortcut(QKeySequence("Ctrl+E"));
	goToMatchingBraceAction->setStatusTip(tr("Jump to the matching brace."));
	connect(goToMatchingBraceAction, SIGNAL(triggered()), this, SLOT(jumpToMatchingBrace()));
	editMenu->addAction(goToMatchingBraceAction);
	selToMatchingBraceAction = new QAction(QObject::tr("&Select to the matching brace"), 0);
	selToMatchingBraceAction->setShortcut(QKeySequence(tr("Shift+Ctrl+E")));
	selToMatchingBraceAction->setStatusTip(tr("Select to the matching brace."));
	connect(selToMatchingBraceAction, SIGNAL(triggered()), this, SLOT(selectToMatchingBrace()));
	editMenu->addAction(selToMatchingBraceAction);
	lineCommentAction = new QAction(QObject::tr("(Un)Comment the selected l&ine(s)"), 0);
	lineCommentAction->setShortcut(QKeySequence(tr("Ctrl+k")));
	lineCommentAction->setStatusTip(tr("(Un)Comment the selected line(s)."));
	connect(lineCommentAction, SIGNAL(triggered()), this, SLOT(lineComment()));
	editMenu->addAction(lineCommentAction);
	blockCommentAction = new QAction(QObject::tr("(Un)Comment the selected &block"), 0);
	blockCommentAction->setShortcut(QKeySequence(tr("Ctrl+b")));
	blockCommentAction->setStatusTip(tr("(Un)Comment the selected block."));
	connect(blockCommentAction, SIGNAL(triggered()), this, SLOT(blockComment()));
	editMenu->addAction(blockCommentAction);
	duplicateLineAction = new QAction(QObject::tr("&Duplicate the current line"), 0);
	duplicateLineAction->setShortcut(QKeySequence(tr("Ctrl+D")));
	duplicateLineAction->setStatusTip(tr("Duplicate the selected line."));
	connect(duplicateLineAction, SIGNAL(triggered()), this, SLOT(duplicateLine()));
	editMenu->addAction(duplicateLineAction);
	//moveLineUpAction = new QAction(QObject::tr("&Move the current line up"), 0);
	//moveLineUpAction->setShortcut(QKeySequence(tr("Ctrl+T")));
	//moveLineUpAction->setStatusTip(tr("Move the current line up."));
	//connect(moveLineUpAction, SIGNAL(triggered()), this, SLOT(moveLineUp()));
	//editMenu->addAction(moveLineUpAction);
	deleteCurrentLineAction = new QAction(QObject::tr("D&elete the current line"), 0);
	deleteCurrentLineAction->setShortcut(QKeySequence(tr("Ctrl+L")));
	deleteCurrentLineAction->setStatusTip(tr("Delete the current line."));
	connect(deleteCurrentLineAction, SIGNAL(triggered()), this, SLOT(deleteCurrentLine()));
	editMenu->addAction(deleteCurrentLineAction);
	toUpperCaseAction = new QAction(QObject::tr("Change the line to &upper case"), 0);
	toUpperCaseAction->setShortcut(QKeySequence(tr("Ctrl+Shift+U")));
	toUpperCaseAction->setStatusTip(tr("Change the line to upper case."));
	connect(toUpperCaseAction, SIGNAL(triggered()), this, SLOT(toUpperCase()));
	editMenu->addAction(toUpperCaseAction);
	toLowerCaseAction = new QAction(QObject::tr("Change the line to &lower case"), 0);
	toLowerCaseAction->setShortcut(QKeySequence(tr("Ctrl+U")));
	toLowerCaseAction->setStatusTip(tr("Change the line to lower case."));
	connect(toLowerCaseAction, SIGNAL(triggered()), this, SLOT(toLowerCase()));
	editMenu->addAction(toLowerCaseAction);
	menuBar()->addMenu(editMenu);//the edit menu..........................................................

	//tileAction = new QAction(tr("&Tile"), this);
	//tileAction->setShortcut(QKeySequence(tr("Ctrl+Alt+T")));
	//tileAction->setStatusTip(tr("Tile the windows"));
	//connect(tileAction, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

	//cascadeAction = new QAction(tr("C&ascade"), this);
	//cascadeAction->setShortcut(QKeySequence(tr("Ctrl+Alt+C")));
	//cascadeAction->setStatusTip(tr("Cascade the windows"));
	//connect(cascadeAction, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

	nextAction = new QAction(QIcon(":/resources/next.png"), tr("&Next"), this);
	nextAction->setShortcut(QKeySequence(tr("Ctrl+Tab")));//QKeySequence::NextChild);
	nextAction->setStatusTip(tr("Move the focus to the next window"));
	connect(nextAction, SIGNAL(triggered()), mdiArea, SLOT(activateNextSubWindow()));

	previousAction = new QAction(QIcon(":/resources/previous.png"), tr("&Previous"), this);
	previousAction->setShortcut(QKeySequence(tr("Ctrl+Shift+Tab")));//QKeySequence::PreviousChild);
	previousAction->setStatusTip(tr("Move the focus to the previous window"));
	connect(previousAction, SIGNAL(triggered()), mdiArea, SLOT(activatePreviousSubWindow()));
	menuBar()->addMenu(windowMenu);
	updateWindowMenu();
	connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));//the window menu..........................................................

	addRemMarkerAction = new QAction(QIcon(":/resources/togglemarker.png"), tr("Add/Remove &marker"), this);
	addRemMarkerAction->setShortcut(QKeySequence(tr("F2")));
	addRemMarkerAction->setStatusTip(tr("Add/Remove marker"));
	connect(addRemMarkerAction, SIGNAL(triggered()), this, SLOT(toggleMarker()));
	//markersMenu->addAction(addRemMarkerAction);

	nextMarkerAction = new QAction(QIcon(":/resources/nextmarker.png"), tr("&Next marker"), this);
	nextMarkerAction->setShortcut(QKeySequence(tr("F4")));
	nextMarkerAction->setStatusTip(tr("Next marker"));
	connect(nextMarkerAction, SIGNAL(triggered()), this, SLOT(nextMarker()));
	//markersMenu->addAction(nextMarkerAction);

	prevMarkerAction = new QAction(QIcon(":/resources/prevmarker.png"), tr("&Previous marker"), this);
	prevMarkerAction->setShortcut(QKeySequence(tr("F6")));
	prevMarkerAction->setStatusTip(tr("Previous marker"));
	connect(prevMarkerAction, SIGNAL(triggered()), this, SLOT(prevMarker()));
	//markersMenu->addAction(prevMarkerAction);

	remAllMarkerAction = new QAction(QIcon(":/resources/remallmarker.png"), tr("&Remove all markers"), this);
	remAllMarkerAction->setShortcut(QKeySequence(tr("F8")));
	remAllMarkerAction->setStatusTip(tr("Remove all markers"));
	connect(remAllMarkerAction, SIGNAL(triggered()), this, SLOT(removeAllMarkers()));
	//markersMenu->addAction(remAllMarkerAction);

	connect(markersMenu, SIGNAL(aboutToShow()), SLOT(updateMarkersMenu()));

	menuBar()->addMenu(markersMenu);
	updateMarkersMenu();//the markers menu..........................................................

	runDocumentAction = documentMenu->addAction(QIcon(":/resources/runscript.png"),tr("&Execute"));
	runDocumentAction->setEnabled(false);
	runDocumentAction->setShortcut(QKeySequence(tr("F5")));
	runDocumentAction->setStatusTip(tr("Execute the current document"));
	runDocumentAction->setData(GlobalApplicationInformation::Execute);
	connect(runDocumentAction, SIGNAL(triggered()), this, SLOT(executeActiveDocument()));

	//debugScriptAction = scriptMenu->addAction(tr("Debug Script"));//QIcon(":/resources/runscript.png"),tr("&Run Script"));
	//debugScriptAction->setEnabled(false);//(false);
	//debugScriptAction->setShortcut(QKeySequence(tr("F6")));
	//debugScriptAction->setStatusTip(tr("Open the current script in the Debugger"));
	//debugScriptAction->setData(GlobalApplicationInformation::Debug);
	//connect(debugScriptAction, SIGNAL(triggered()), this, SLOT(executeScript()));

	abortDocumentAction = documentMenu->addAction(QIcon(":/resources/abort.png"), tr("&Abort"));//QIcon(":/resources/runscript.png"),tr("&Run Script"));
	abortDocumentAction->setEnabled(false);//false);
	abortDocumentAction->setShortcut(QKeySequence(tr("F7")));
	abortDocumentAction->setStatusTip(tr("Abort the current document"));
	connect(abortDocumentAction, SIGNAL(triggered()), this, SLOT(abortScript()));

	restartScriptEngineAction = documentMenu->addAction(QIcon(":/resources/restart.png"), tr("&Restart Script Engine"));
	restartScriptEngineAction->setEnabled(false);//false);
	restartScriptEngineAction->setShortcut(QKeySequence(tr("F9")));
	restartScriptEngineAction->setStatusTip(tr("Force a restart of the script engine"));
	connect(restartScriptEngineAction, SIGNAL(triggered()), this, SLOT(restartScriptEngine()));


	menuBar()->addMenu(documentMenu);//the script menu..........................................................

	//m_backgroundAction = viewMenu->addAction(tr("&Background"));
	//m_backgroundAction->setStatusTip(tr("Toggle Background"));
	//m_backgroundAction->setEnabled(false);
	//m_backgroundAction->setCheckable(true);
	//m_backgroundAction->setChecked(false);
	//connect(m_backgroundAction, SIGNAL(toggled(bool)), SVGPreviewer, SLOT(setViewBackground(bool)));

	//m_outlineAction = viewMenu->addAction(tr("&Outline"));
	//m_outlineAction->setStatusTip(tr("Toggle Outline"));
	//m_outlineAction->setEnabled(false);
	//m_outlineAction->setCheckable(true);
	//m_outlineAction->setChecked(true);
	//connect(m_outlineAction, SIGNAL(toggled(bool)), SVGPreviewer, SLOT(setViewOutline(bool)));
	
	//viewMenu->addAction(debugLogDock->toggleViewAction());
	//viewMenu->addAction(debuggerDock->toggleViewAction());
	//menuBar()->addMenu(viewMenu);//the view menu..........................................................

	optionsAction = toolsMenu->addAction(QIcon(":/resources/configure.png"), tr("&Options"));
	optionsAction->setShortcut(QKeySequence(tr("Ctrl+Alt+O")));
	optionsAction->setStatusTip(tr("Open the options dialog"));
	//optionsAction->setEnabled(true);
	connect(optionsAction, SIGNAL(triggered()), this, SLOT(openOptionsDialog()));
	menuBar()->addMenu(toolsMenu);//the tools menu..........................................................

	//debuggerMenu = AppScriptEngine->createDebuggerMenu(menuBar());//debugger->createStandardMenu(menuBar());
	//menuBar()->addMenu(debuggerMenu);
}

void MainWindow::setupHelpMenu()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	helpMenu = menuBar()->addMenu(tr("&Help"));
	
	assistantAct = new QAction(QIcon(":/resources/help.png"), tr("Help &Contents"), this);
	assistantAct->setStatusTip(tr("Show the BrainStim Documentation"));
	helpMenu->addAction(assistantAct);
	assistantAct->setShortcut(QKeySequence::HelpContents);
	connect(assistantAct, SIGNAL(triggered()), this, SLOT(showDocumentation()));
	
	aboutBrainStimAct = new QAction(QIcon(":/resources/about.png"), tr("&About BrainStim"), this);
	aboutBrainStimAct->setStatusTip(tr("Show the BrainStim About Dialog"));
	helpMenu->addAction(aboutBrainStimAct);
	connect(aboutBrainStimAct, SIGNAL(triggered()), this, SLOT(aboutBrainStim()));

	aboutQtAct = new QAction(QIcon(":/resources/aboutQt.png"), tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt About Dialog"));
	helpMenu->addAction(aboutQtAct);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	historyQtAct = new QAction(QIcon(":/resources/history.png"), tr("BrainStim &History"), this);
	historyQtAct->setStatusTip(tr("Show the BrainStim History"));
	helpMenu->addAction(historyQtAct);
	connect(historyQtAct, SIGNAL(triggered()), this, SLOT(showJavaScriptConfigurationFile()));

	menuBar()->addMenu(helpMenu);//the help menu..........................................................
}

void MainWindow::showDocumentation()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	helpAssistant->showDocumentation("index.html", globAppInfo);
}

void MainWindow::write2OutputWindow(const QString &text2Write, const QString &sTabName)// See the defined MAIN_PROGRAM_LOG_SLOT_NAME
{
	if(mTabNameToOutputWindowList.contains(sTabName))
	{
		if(bMainWindowIsInitialized)
			mTabNameToOutputWindowList[sTabName]->append(text2Write);
	}
}

void MainWindow::reloadSubWindow()
{
	reOpenCurrentFile("", false);
}

void MainWindow::reOpenCurrentFile(const QString &strCanonicalFilePath, const bool &bNativeFileViewer)// See the defined MAIN_PROGRAM_REOPEN_SLOT_NAME
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QMdiSubWindow *existing = NULL;
	QString sFinalPath = strCanonicalFilePath;
	if (sFinalPath.isEmpty())
	{
		existing = activeMdiChild();
		sFinalPath = DocManager->getFileName(existing);
	}
	else
	{
		existing = findMdiChild(sFinalPath);
	}

	if (existing)
	{
		QString sNewFileName;
		if (DocManager->maybeSave(existing, false, sNewFileName))
		{
			QApplication::setOverrideCursor(Qt::WaitCursor);
			mdiArea->setActiveSubWindow(existing);
			DocManager->setModFlagAndTitle(DocManager->getDocIndex(existing),false);//Trick for a successful closing of document, see next
			closeActiveDocument();
			QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), "openFiles", Qt::QueuedConnection, Q_ARG(QString, sFinalPath), Q_ARG(QStringList, QStringList()), Q_ARG(bool, bNativeFileViewer));
			QApplication::restoreOverrideCursor(); 
		}
		else if (sNewFileName.isEmpty() == false)
		{
			updateRecentFileList(sNewFileName);
		}
	}
	QApplication::restoreOverrideCursor();
}

void MainWindow::clearOutputWindow(const QString &sTabName) 
{
	clearDebugger(sTabName);
}

bool MainWindow::addOutputWindow(const QString &sTabName) 
{
	if(mTabNameToOutputWindowList.contains(sTabName))
		return false;
	QTextEdit *tmpTextEdit = new QTextEdit();
	tmpTextEdit->setReadOnly(true);
	mTabNameToOutputWindowList.insert(sTabName,tmpTextEdit);
	outputTabWidget->addTab(tmpTextEdit, sTabName);
	mTabNameToOutputWindowList[sTabName]->setContextMenuPolicy(Qt::CustomContextMenu);
	mTabNameToConnectionList.insert(sTabName,connect(mTabNameToOutputWindowList[sTabName], &QTextEdit::customContextMenuRequested, [=](const QPoint &arg) {DebugcontextMenuEvent(arg, sTabName);}));
	
	mClearDebuggerAction.insert(sTabName, new QAction(QIcon(":/resources/logClear.png"), QObject::tr("C&lear All Output Item(s)"), 0));
	//clearDebuggerAction->setShortcut(QKeySequence(""));
	mClearDebuggerAction.value(sTabName)->setStatusTip(tr("Clear the Debugger Output window."));
	connect(mClearDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperClearDebugger, SLOT(map()));
	signalMapperClearDebugger->setMapping(mClearDebuggerAction.value(sTabName), sTabName);

	mCopyDebuggerAction.insert(sTabName, new QAction(QIcon(":/resources/logCopy.png"), QObject::tr("&Copy Selected Text"), 0));
	//CopyDebuggerAction->setShortcut(QKeySequence(""));
	mCopyDebuggerAction.value(sTabName)->setStatusTip(tr("Copy the Selected Debugger Output window line(s)."));
	connect(mCopyDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperCopyDebugger, SLOT(map()));
	signalMapperCopyDebugger->setMapping(mCopyDebuggerAction.value(sTabName), sTabName);

	mSaveDebuggerAction.insert(sTabName, new QAction(QIcon(":/resources/logSave.png"), QObject::tr("&Save All Output information to a text file..."), 0));
	//SaveDebuggerAction->setShortcut(QKeySequence(""));
	mSaveDebuggerAction.value(sTabName)->setStatusTip(tr("Save the Debugger Output window."));
	connect(mSaveDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperSaveDebugger, SLOT(map()));
	signalMapperSaveDebugger->setMapping(mSaveDebuggerAction.value(sTabName), sTabName);

	/*
	mRemoveDebuggerAction.insert(sTabName,new QAction(QObject::tr("&Delete this tabbed Output window"), 0));
	//mRemoveDebuggerAction->setShortcut(QKeySequence(""));
	mRemoveDebuggerAction.value(sTabName)->setStatusTip(tr("Delete the Debugger Output window."));
	connect(mRemoveDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperRemoveDebugger, SLOT(map()));
	signalMapperRemoveDebugger->setMapping(mRemoveDebuggerAction.value(sTabName), sTabName);
	*/

	return true;
}

bool MainWindow::removeOutputWindow(const QString &sTabName)
{
	if(sTabName == MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME)
		return false;
	//if(mTabNameToOutputWindowList.contains(sTabName) == false)
	//	return false;
	int nTabIndex = -1;
	for(int i=0;i<outputTabWidget->count();i++)
	{
		if(outputTabWidget->tabText(i) == sTabName)
		{
			nTabIndex = i;
			break;
		}
	}
	if(nTabIndex >= 0)
	{
		if(mTabNameToConnectionList.contains(sTabName))
			disconnect(mTabNameToConnectionList.value(sTabName));
		outputTabWidget->removeTab(nTabIndex);
		mTabNameToOutputWindowList.remove(sTabName);

		if(mClearDebuggerAction.contains(sTabName))
			disconnect(mClearDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperClearDebugger, SLOT(map()));
		if(mCopyDebuggerAction.contains(sTabName))
			disconnect(mCopyDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperCopyDebugger, SLOT(map()));
		if(mSaveDebuggerAction.contains(sTabName))
			disconnect(mSaveDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperSaveDebugger, SLOT(map()));
		//if(mRemoveDebuggerAction.contains(sTabName))
		//	disconnect(mRemoveDebuggerAction.value(sTabName), SIGNAL(triggered()), signalMapperRemoveDebugger, SLOT(map()));

		return true;
	}
	mTabNameToOutputWindowList.remove(sTabName);
	return false;
}

bool MainWindow::saveOutputWindow(const QString &sFilePath, const bool &bOverwrite, const QString &sTabName)
{
	if(mTabNameToOutputWindowList.contains(sTabName) == false)
		return false;
	QString fileName;
	QFile file;
	if (sFilePath.isEmpty())
	{
		fileName = QFileDialog::getSaveFileName(NULL, "Save Output", QDir::currentPath(), "Text Files (*.txt);;Any file (*)");
		file.setFileName(fileName);
	}
	else
	{
		fileName = sFilePath;
		file.setFileName(fileName);
		if(file.exists() && (bOverwrite == false))
		{
			qDebug() << __FUNCTION__ << "Destination file already exists, cannot overwrite. Select another file destination or set the bOverwrite parameter.";	
			return false;
		}
	}
	if (fileName.isEmpty())
	{
		qDebug() << __FUNCTION__ << "No valid file chosen.";
		return false;
	}
	if (file.open(QFile::WriteOnly | QFile::Text)) 
	{
		if(mTabNameToOutputWindowList[sTabName]->toPlainText() != "")
			file.write(mTabNameToOutputWindowList[sTabName]->toPlainText().toLatin1());
		//if(outputWindowList->count() > 0)
		//{
			/*for(int i=0;i<outputWindowList->count();i++) 
			{
			file.write(QString(outputWindowList->item(i)->text() + "\n").toLatin1());
			}*/
		//}
		else
		{
			qWarning() << "No data available in Log Output Pane to write.";
		}		
		file.close();
		return true;
	}
	qDebug() << __FUNCTION__ << "Could not write to file: " << fileName;
	return false;

}

void MainWindow::clearDebugger(const QString &sTabName)
{
	if(mTabNameToOutputWindowList.contains(sTabName))
	{
		if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
			qDebug() << "Verbose Mode: " << __FUNCTION__;
		mTabNameToOutputWindowList[sTabName]->clear();
	}
}

void MainWindow::copyDebugger(const QString &sTabName)
{
	if(mTabNameToOutputWindowList.contains(sTabName))
	{
		if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
			qDebug() << "Verbose Mode: " << __FUNCTION__;
		mTabNameToOutputWindowList[sTabName]->copy();
	}
}

void MainWindow::saveDebugger(const QString &sTabName)
{
	saveOutputWindow("", false,sTabName);
}

void MainWindow::createDockWindows()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	debugLogDock = new QDockWidget(MAINWINDOW_DOCK_OUTPUTWINDOW_NAME, this);
	debugLogDock->setAccessibleName(MAINWINDOW_DOCK_OUTPUTWINDOW_NAME);
	debugLogDock->setObjectName(MAINWINDOW_DOCK_OUTPUTWINDOW_NAME);
	debugLogDock->setAllowedAreas(Qt::AllDockWidgetAreas);// Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
	mTabNameToOutputWindowList.insert(MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME,new QTextEdit());
	mTabNameToOutputWindowList[MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME]->setReadOnly(true);
	outputTabWidget = new CustomChildDockTabWidget();
	outputTabWidget->setGroupName(MAINWINDOW_NAME);
	outputTabWidget->setTabsClosable(true);
	outputTabWidget->setMovable(true);
	outputTabWidget->setTabShape(QTabWidget::Rounded);
	outputTabWidget->setTabPosition(QTabWidget::North);
	outputTabWidget->addTab(mTabNameToOutputWindowList[MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME],MAINWINDOW_DEFAULT_OUTPUTWINDOW_TABNAME);
	connect(outputTabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(outputTabCloseRequest(int)));
	//outputWindowList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	//QString tmpColor = QString::number(BRAINSTIM_DEFAULT_WINDOW_BACKGROUND_COLOR_RED) + "," + QString::number(BRAINSTIM_DEFAULT_WINDOW_BACKGROUND_COLOR_GREEN) + "," + QString::number(BRAINSTIM_DEFAULT_WINDOW_BACKGROUND_COLOR_BLUE);
	//outputWindowList->setStyleSheet("* { background-color:rgb(" + tmpColor + "); padding: 10px ; color:rgb(136,0,21)}");
	//debugLogDock->setWidget(outputWindowList);
	debugLogDock->setWidget(outputTabWidget);
	Qt::DockWidgetArea dArea = Qt::BottomDockWidgetArea;
	loadSavedDockWidgetConfiguration(UI_SETTINGS_FILENAME, MAINWINDOW_NAME, debugLogDock, dArea);
	addDockWidget(dArea, debugLogDock);
	//addDockWidget(Qt::BottomDockWidgetArea, testrr);//(Qt::RightDockWidgetArea, debugLogDock);
	//viewMenu->addAction(debugLogDock->toggleViewAction());
	//connect(debugList, SIGNAL(currentTextChanged(const QString &)),
	//	this, SLOT(insertCustomer(const QString &)));
	//////////////////////////////////////////////////////////////////////////
	//debuggerDock = new QDockWidget(tr("Debugger"), this);
	//debuggerDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);
	//stackWidget = AppScriptEngine->debugger->widget(QScriptEngineDebugger::StackWidget);
	//debuggerMainWindow = AppScriptEngine->DebuggerStandardWindow();//debugger->standardWindow();
	//debuggerMainWindow->setWindowFlags(Qt::WindowFlags::)
	//debuggerMainWindow->showMaximized();
	//debugList2 = new QListWidget(debuggerDock);
	//QLayout *layout = new QHBoxLayout;
	//layout->addWidget(debugList2);
	//layout->addWidget(codeWindow);
	//layout->addWidget(stackWidget);
	//debuggerDock->setWidget(stackWidget);//codeWindow);//debugList2);
	//stackWidget->setParent(debuggerDock);
	//debuggerMainWindow->setParent(debuggerDock);
	//debuggerDock->setLayout(layout);
	//addDockWidget(Qt::BottomDockWidgetArea, debuggerDock);
	//tabifyDockWidget(
}

void MainWindow::setupDynamicPlugins()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	if (BrainStimFlags.testFlag(GlobalApplicationInformation::DisableAllPlugins) == false)
	{
		extendAPICallTips(this->metaObject());
		showSplashMessage("Loading Static Plugins...");
        Q_IMPORT_PLUGIN(ParallelPortPlugin)// see below
        Q_IMPORT_PLUGIN(ExperimentManagerPlugin)// see below
		Q_IMPORT_PLUGIN(KeyBoardPlugin)// see below

		Q_UNUSED(qt_static_plugin_ExperimentManagerPlugin());
		Q_UNUSED(qt_static_plugin_KeyBoardPlugin());
		Q_UNUSED(qt_static_plugin_ParallelPortPlugin());

		bool bRetVal = false;
		QString strRetVal = "";
		const QMetaObject* metaObject = NULL;
		bool bInterfaceResolved = false;

		foreach (QObject *plugin, QPluginLoader::staticInstances())
		{
			DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);
			if (iDevice) 
			{
				iDevice->fetchGlobalAppInfo();
				QString sTmpInternalPluginName = iDevice->GetPluginInternalName();
				bool bCustomConfFoundAndAdded = getPluginCustomConfigurationOptions(sTmpInternalPluginName);
				bInterfaceResolved = true;
			}
			else 
			{
				ExtensionInterface *iExtension = qobject_cast<ExtensionInterface *>(plugin);
				if (iExtension) 
				{
					iExtension->fetchGlobalAppInfo();
					QString sTmpInternalPluginName = iExtension->GetPluginInternalName();
					//bool bCustomConfFoundAndAdded = 
						getPluginCustomConfigurationOptions(sTmpInternalPluginName);
					bInterfaceResolved = true;
				}				
			}
			metaObject = plugin->metaObject();
			//QStringList methods;
			//for(i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
			//{
			//	methods << QString::fromLatin1(metaObject->method(i).signature());
			//	DocManager->addAdditionalApiEntry(QString::fromLatin1(metaObject->method(i).signature()));
			//}
			if(bInterfaceResolved)
			{
				if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(QString(FUNC_PLUGIN_ISCOMPATIBLE_FULL).toLatin1())) == -1))//Is the slot present?
				{				
					if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(QString(FUNC_PLUGIN_GETSCRIPTMETAOBJECT_FULL).toLatin1())) == -1))//Is the slot present?
					{
						QObject *pointerQObject = NULL;
						const QMetaObject* metaScriptObject;
						int i = 0;
						bool bResult;
						while(true)
						{
							//Invoke the slot
							pointerQObject = NULL;
							bResult = metaObject->invokeMethod(plugin, FUNC_PLUGIN_GETSCRIPTMETAOBJECT,Qt::DirectConnection, Q_RETURN_ARG(QObject*,(QObject*)pointerQObject), Q_ARG(int,(int)i));				
							if(bResult)
							{
								if(pointerQObject == NULL)
									break;
								if(i > 99)
								{
									qDebug() << __FUNCTION__ << "::Wrong Plugin(" << metaObject->className() << ") implementation, no limit or too many meta-objects(" << FUNC_PLUGIN_GETSCRIPTMETAOBJECT << ")!";
									break;
								}
								metaScriptObject = (const QMetaObject*) pointerQObject;
								if (metaScriptObject && bResult)
									extendAPICallTips(metaScriptObject);
							}
							else
							{
								break;
							}
							i++;
						}
					}

					popPluginIntoMenu(plugin);
					//Additional File Extensions defined?
					if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(QString(FUNC_PLUGIN_GETADDFILEEXT_FULL).toLatin1())) == -1))//Is the slot present?
					{
						//Invoke the slot
						QStringList strRetValList;
						metaObject->invokeMethod(plugin, FUNC_PLUGIN_GETADDFILEEXT,Qt::DirectConnection, Q_RETURN_ARG(QStringList, strRetValList));//if(!metaObject->invokeMethod(plugin, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal)))				
						for (int i=0;i<strRetValList.count();i++)
						{
							DocManager->appendKnownFileExtensionList(strRetValList[i]);
						}

						if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(QString(FUNC_PLUGIN_GETADDFILE_SLOT_HANDLERS_FULL).toLatin1())) == -1))//Is the slot present?
						{
							//Invoke the slot
							metaObject->invokeMethod(plugin, FUNC_PLUGIN_GETADDFILE_SLOT_HANDLERS,Qt::DirectConnection, Q_RETURN_ARG(QStringList, strRetValList));			
							for (int i=0;i<strRetValList.count();i++)
							{
								DocManager->appendKnownDocumentFileHandlerList(strRetValList[i],plugin);
								//QString a = strRetValList[i];
							}
						}
					}
				}
				else
				{
					qDebug() << __FUNCTION__ << ", Could not invoke the Static Plugin slot(" << QString(FUNC_PLUGIN_ISCOMPATIBLE_FULL) << ")!";	
				}
			}
			else
			{
				qDebug() << __FUNCTION__ << ", Could not resolve the Static Plugin interface!";	
			}
			metaObject = NULL;
			bRetVal = false;
		}
		showSplashMessage("Loading Dynamic Plugins...");
		bInterfaceResolved = false;
		foreach (QString fileName, QDir(MainAppInfo::pluginsDirPath()).entryList(QDir::Files)) 
		{
			if (QFileInfo(fileName).completeSuffix().toLower() != "dll")
				continue;
			QPluginLoader loader(QDir(MainAppInfo::pluginsDirPath()).absoluteFilePath(fileName));
			QObject *plugin = loader.instance();//The QObject provided by the plugin, if it was compiled against an incompatible version of the Qt library, QPluginLoader::instance() returns a null pointer.
			if (plugin) 
			{
				DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);
				if (iDevice) 
				{
					iDevice->fetchGlobalAppInfo();
					QString sTmpInternalPluginName = iDevice->GetPluginInternalName();
					bool bCustomConfFoundAndAdded = getPluginCustomConfigurationOptions(sTmpInternalPluginName);
					bInterfaceResolved = true;
				}
				else 
				{
					ExtensionInterface *iExtension = qobject_cast<ExtensionInterface *>(plugin);
					if (iExtension) 
					{
						iExtension->fetchGlobalAppInfo();
						QString sTmpInternalPluginName = iExtension->GetPluginInternalName();
						bool bCustomConfFoundAndAdded = getPluginCustomConfigurationOptions(sTmpInternalPluginName);
						bInterfaceResolved = true;
					}				
				}
				metaObject = plugin->metaObject();
				//QStringList properties;
				//for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
				//	properties << QString::fromLatin1(metaObject->method(i).signature());
				if(bInterfaceResolved)
				{
					if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(QString(FUNC_PLUGIN_ISCOMPATIBLE_FULL).toLatin1())) == -1))//Is the slot present?
					{
						//qWarning() << __FUNCTION__ << ", Checking plugin compatibility(" << fileName << ")...";	
						//Invoke the slot
						metaObject->invokeMethod(plugin, FUNC_PLUGIN_ISCOMPATIBLE,Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal));//if(!metaObject->invokeMethod(plugin, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal)))				
						if (bRetVal)
						{
							bool bResult;
							QObject *pointerQObject = NULL;
							const QMetaObject* metaScriptObject = NULL;
							int i=0;
							while(true)
							{
								//Invoke the slot
								pointerQObject = NULL;
								bResult = metaObject->invokeMethod(plugin, FUNC_PLUGIN_GETSCRIPTMETAOBJECT,Qt::DirectConnection, Q_RETURN_ARG(QObject*,(QObject*)pointerQObject), Q_ARG(int,(int)i));				
								if(bResult)
								{
									if(pointerQObject == NULL)
										break;
									if(i > 99)
									{
										qDebug() << __FUNCTION__ << "::Wrong Plugin(" << metaObject->className() << ") implementation, no limit or too many meta-objects(" << FUNC_PLUGIN_GETSCRIPTMETAOBJECT << ")!";
										break;
									}
									metaScriptObject = (const QMetaObject*) pointerQObject;
									if (metaScriptObject && bResult)
										extendAPICallTips(metaScriptObject);
								}
								else
								{
									break;
								}
								i++;
							}
						
							//qWarning() << __FUNCTION__ << ", Plugin is compatibility(" << fileName << ")";
							if (popPluginIntoMenu(plugin))
							{
								pluginFileNames += fileName;
								//Additional File Extensions defined?
								if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(QString(FUNC_PLUGIN_GETADDFILEEXT_FULL).toLatin1())) == -1))//Is the slot present?
								{
									//Invoke the slot
									QStringList strRetValList;
									metaObject->invokeMethod(plugin, FUNC_PLUGIN_GETADDFILEEXT,Qt::DirectConnection, Q_RETURN_ARG(QStringList, strRetValList));//if(!metaObject->invokeMethod(plugin, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal)))				
									for (int i=0;i<strRetValList.count();i++)
									{
										DocManager->appendKnownFileExtensionList(strRetValList[i]);
									}	
									if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(QString(FUNC_PLUGIN_GETADDFILE_SLOT_HANDLERS_FULL).toLatin1())) == -1))//Is the slot present?
									{
										//Invoke the slot
										metaObject->invokeMethod(plugin, FUNC_PLUGIN_GETADDFILE_SLOT_HANDLERS,Qt::DirectConnection, Q_RETURN_ARG(QStringList, strRetValList));			
										for (int i=0;i<strRetValList.count();i++)
										{
											DocManager->appendKnownDocumentFileHandlerList(strRetValList[i],plugin);
											//QString a = strRetValList[i];
										}
									}
								}
							}
						}
						else
						{
							qDebug() << __FUNCTION__ << "::The Dynamic Plugin is incompatible(" << fileName << ")!"; //metaObject->className() << ")!";
						}
					}
					else
					{
						qDebug() << __FUNCTION__ << "::Could not invoke the Dynamic Plugin slot(" << fileName << ", " << QString(FUNC_PLUGIN_ISCOMPATIBLE_FULL) << ")!";	
					}
				}
				else
				{
					qDebug() << __FUNCTION__ << ", Could not resolve the Dynamic Plugin interface!";	
				}
				metaObject = NULL;
				bRetVal = false;
			}
			else
			{
				qDebug() << __FUNCTION__ << "::Could not load the Dynamic Plugin (" << fileName << ")!";
			}
		}
	}
	DocManager->appendKnownFileExtensionList(MAIN_PROGRAM_POST_FILESEXTENSION_LIST);
	////example 1(static plugin):
	//see line Q_IMPORT_PLUGIN(parallelportplugin)
	//ParallelPort *a = new ParallelPort(888,NULL); //see Q_DECLARE_METATYPE(ParallelPort*), must include header etc
	//a->IsPortEcp();
	//delete a;
	//ExperimentManager *b = new ExperimentManager(this);
	//b->openExperiment();
	//delete b;
}

bool MainWindow::extendAPICallTips(const QMetaObject* metaScriptObject)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	if (metaScriptObject)
	{
		// <item signature> ::SuperClassName||ScriptClassName::Type::ReturnType::Description

		int i;
		QString tmpString;
		QString sScriptClassName;
		QString sSuperClassName;
		QStringList sClassInfoList;
		QString sDivider = "::";

		sScriptClassName = metaScriptObject->className();
		sSuperClassName = metaScriptObject->superClass()->className();

		//int amount = 
			metaScriptObject->classInfoCount();
		for(i = metaScriptObject->classInfoOffset(); i < metaScriptObject->classInfoCount(); ++i)
		{
			if (QString::fromLatin1(metaScriptObject->classInfo(i).name()) == QString(SCRIPT_API_CLASS_NAME_TAG))
			{
				sScriptClassName = QString::fromLatin1(metaScriptObject->classInfo(i).value());				
			}			
		}
		DocManager->addAdditionalApiEntry(sScriptClassName + " " + sDivider + sSuperClassName);
		
		for(i = metaScriptObject->methodOffset(); i < metaScriptObject->methodCount(); ++i)
		{
			sClassInfoList << QString::fromLatin1(metaScriptObject->method(i).methodSignature());

			//is this an member signal or slot?
			QString mType = METHOD_TYPE_MEMBER_TAG;
			if(metaScriptObject->indexOfSignal(sClassInfoList.at(sClassInfoList.count()-1).toLatin1()) >= 0)
				mType = METHOD_TYPE_SIGNAL_TAG;
			else if(metaScriptObject->indexOfSlot(sClassInfoList.at(sClassInfoList.count()-1).toLatin1()) >= 0)
				mType = METHOD_TYPE_SLOT_TAG;
			DocManager->addAdditionalApiEntry(QString::fromLatin1(metaScriptObject->method(i).methodSignature()) + " " + sDivider + sScriptClassName + sDivider + mType);
		}

		for(i = metaScriptObject->propertyOffset(); i < metaScriptObject->propertyCount(); ++i)
		{
			sClassInfoList << QString::fromLatin1(metaScriptObject->property(i).name());
			DocManager->addAdditionalApiEntry(QString::fromLatin1(metaScriptObject->property(i).name()) + " " + sDivider + sScriptClassName + sDivider + METHOD_TYPE_PROPERTY_TAG);
		}

		for(i = metaScriptObject->enumeratorOffset(); i < metaScriptObject->enumeratorCount(); ++i)
		{
			sClassInfoList << QString::fromLatin1(metaScriptObject->enumerator(i).name());
			//DocManager->addAdditionalApiEntry(QString::fromLatin1(metaScriptObject->method(i).signature()) + " " + sDivider + sScriptClassName + sDivider + METHOD_TYPE_ENUMERATOR_TAG);
		}

		//QStringList constructor;
		//int constructorcount = metaScriptObject->constructorCount();
		//if(constructorcount>0)
		//	constructorcount = constructorcount;
		//for(i = metaScriptObject->indexOfConstructor(); i < metaScriptObject->constructorCount(); ++i)
		//{
		//	constructor << QString::fromLatin1(metaScriptObject->method(i).signature());
		//	DocManager->addAdditionalApiEntry(QString::fromLatin1(metaScriptObject->method(i).signature()));
		//}
		return true;
	}
	return false;
};

bool MainWindow::checkPluginCompatibility(QObject *plugin)
{
	bool bRetval = false;

	DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);
	if (iDevice) 
	{
		bRetval = iDevice->IsCompatible();
	}

	ExtensionInterface *iExtension = qobject_cast<ExtensionInterface *>(plugin);
	if (iExtension) 
	{
		bRetval = iExtension->IsCompatible();
	}

	//qobject_cast<DeviceInterface *>(PlgInterfaces.at(index));

	//plugin->ConfigureScriptEngine(* AppScriptEngine->eng);
	////collection->GetLoaderName(tmpIndex) +")");
	////collection->GetInterface(tmpIndex)->ConfigureScriptEngine(* AppScriptEngine->eng);

	return bRetval;
}

void MainWindow::parsePluginDefinedFileExtensions(QObject *plugin)
{
	QStringList lstExtensions;
	QStringList tmpList;
	bool bRetval = false;
	int i;

	DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);
	if (iDevice) 
	{
		lstExtensions = iDevice->GetAdditionalFileSlotHandlers();
	}

	ExtensionInterface *iExtension = qobject_cast<ExtensionInterface *>(plugin);
	if (iExtension) 
	{
		lstExtensions = iExtension->GetAdditionalFileSlotHandlers();
	}

	if(lstExtensions.isEmpty() == false)
	{		
		for (i=0;i<lstExtensions.count();i++)
		{
			tmpList = lstExtensions.at(i).split("|",QString::SkipEmptyParts);
			tmpNewActionMapping.clear();
			tmpNewActionMapping.insert(tmpList.at(0),GlobalApplicationInformation::DOCTYPE_PLUGIN_DEFINED);
			lNewPluginFileAction.append(new QAction(tmpList.at(0).toUpper() + " Document", this));
			lNewPluginFileAction.last()->setStatusTip("Create an empty " + tmpList.at(0).toUpper() + " document");
			lNewPluginFileAction.last()->setData(QVariant(tmpNewActionMapping));
			connect(lNewPluginFileAction.last(), SIGNAL(triggered()), this, SLOT(newFile()));
			fileNewMenu->addAction(lNewPluginFileAction.last());
			bRetval = true;
		}
	}	
}

bool MainWindow::popPluginIntoMenu(QObject *plugin)
{
	QAction *pluginAction;

	if (!PluginsFound)
	{
		pluginsMenu  = menuBar()->addMenu(tr("&Plugins"));
		Plugins = new PluginCollection(this);
		menuBar()->addMenu(pluginsMenu);//the Plugins menu..........................................................
		PluginsFound = true;
	}

	DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);//For each plugin (static or dynamic), we check which interfaces it implements using qobject_cast()
	if (iDevice) 
	{
		if(checkPluginCompatibility(plugin))
		{
			if (!bDevicePluginsFound)
			{
				devicePluginMenu = pluginsMenu->addMenu(QIcon(":/resources/pluginDevice.png"), tr("&Device Plugins"));
				bDevicePluginsFound = true;
			}
			pluginAction = integratePlugin(plugin,Plugins);
			devicePluginMenu->addAction(pluginAction);
			pluginsMenu->addMenu(devicePluginMenu);//the devices menu..........................................................
			parsePluginDefinedFileExtensions(plugin);
			return true;
		}
		return false;
	}

	ExtensionInterface *iExtension = qobject_cast<ExtensionInterface *>(plugin);//For each plugin (static or dynamic), we check which interfaces it implements using qobject_cast()
	if (iExtension) 
	{
		if(checkPluginCompatibility(plugin))
		{
			if (!bExtensionPluginsFound)
			{
				extensionPluginMenu = pluginsMenu->addMenu(QIcon(":/resources/pluginExtension.png"), tr("&Extension Plugins"));
				bExtensionPluginsFound = true;
			}
			pluginAction = integratePlugin(plugin,Plugins);
			extensionPluginMenu->addAction(pluginAction);
			pluginsMenu->addMenu(extensionPluginMenu);//the extension menu..........................................................
			parsePluginDefinedFileExtensions(plugin);
			return true;
		}
		return false;
	}
	return false;
}

QAction* MainWindow::integratePlugin(QObject *plugin, PluginCollection *collection)
{
	int tmpIndex;
	tmpIndex = Plugins->Add(plugin);
	if (tmpIndex >=0)//if Add succeeded...
	{
		QAction *action1 = new QAction(collection->GetInterface(tmpIndex)->GetPluginInformation(), plugin);
		action1->setData(collection->GetLoaderName(tmpIndex));
		connect(action1, SIGNAL(triggered()), this, SLOT(showPluginGUI()));
		action1->setStatusTip(tr("Show the Plugins UI(") + collection->GetLoaderName(tmpIndex) +")");
		//collection->GetInterface(tmpIndex)->ConfigureScriptEngine(* AppScriptEngine->eng);
		int nNewRegisteredMetaID = configurePluginScriptEngine(tmpIndex);
		MainAppInfo::addRegisteredMetaTypeID(nNewRegisteredMetaID);
		return action1;
	}
	return 0;
}

int MainWindow::configurePluginScriptEngine(const int nIndex)
{
	return Plugins->GetInterface(nIndex)->ConfigureScriptEngine(* AppScriptEngine->eng);
}

bool MainWindow::checkUserDirectories(QStringList &lPathsToCheck, bool bShowWarning)
{
	//Do we already have an custom valid user path defined in the registry?
	QString sUserPath = "";
	bool bNoRegistrySetting = false;
	QVariant tmpVariant;
	if (globAppInfo->getRegistryInformation(REGISTRY_USERDOCUMENTSROOTDIRECTORY, tmpVariant))
	{
		sUserPath = QDir::fromNativeSeparators(tmpVariant.toString());
	}
	else
	{
		bNoRegistrySetting = true;
	}

	QDir tmpDir;
	tmpDir.setPath(sUserPath);
	//Does the directory not exist?
	if ((tmpDir.exists() == false) || bNoRegistrySetting)
	{
		if (bNoRegistrySetting == false)
			qDebug() << __FUNCTION__ << "User Document Path (" << sUserPath << ") doesn't exist. Switching to default path";
		sUserPath = QDir::homePath() + "/" + globAppInfo->getTitle();//MainAppInfo::MainProgramTitle());;//QDir::toNativeSeparators(MainAppInfo::appDirPath());//Use the default BrainStim installation root path
		if (globAppInfo->setRegistryInformation(REGISTRY_USERDOCUMENTSROOTDIRECTORY, sUserPath, "string") == false)
		{
			qDebug() << __FUNCTION__ << "Could not set the default User Document Path (" << sUserPath << ") in the registry.";
			MainAppInfo::setAppUserPath(sUserPath);
			return false;
		}
		tmpDir.setPath(sUserPath);
		if (bNoRegistrySetting && (tmpDir.exists() == false))
		{
			if(tmpDir.mkpath(".")==false)
				qDebug() << __FUNCTION__ << "Could not create the User Document Path (" << sUserPath << ").";
		}

		if(tmpDir.exists() == false) 
		{
			MainAppInfo::setAppUserPath(sUserPath);
			return false;
		}
	}
	MainAppInfo::setAppUserPath(sUserPath);
	if(lPathsToCheck.isEmpty())
			return true;
	int i;
	QString tmpString = QDir::fromNativeSeparators(tmpDir.canonicalPath() + QDir::separator());
	QString sTempFolder;

	for(i=0;i<lPathsToCheck.size();i++) 
	{
		sTempFolder = lPathsToCheck[i];
		lPathsToCheck[i] = tmpString + lPathsToCheck.at(i);
		tmpDir.setPath(lPathsToCheck.at(i));
		if(tmpDir.exists() == false)
		{
			if(tmpDir.mkpath(lPathsToCheck.at(i))==false)
			{
				qDebug() << __FUNCTION__ << "Could not create the User Document Directory (" << lPathsToCheck.at(i) << ") ";
			}
			else
			{
				if (QDir(MainAppInfo::appDirPath() + "/" + sTempFolder).exists())
				{
					if (copyDirectoryRecursively(MainAppInfo::appDirPath() + "/" + sTempFolder, QDir::fromNativeSeparators(lPathsToCheck[i])) == false)
						qDebug() << __FUNCTION__ << "Could not create and copy default files to a custom (writable) user directory (" << sUserPath + lPathsToCheck.at(i) << ") ";
				}
			}
		}
	}

	tmpDir.setPath(sUserPath);
	tmpDir.setFilter(QDir::Dirs | QDir::Writable | QDir::Readable);
	tmpDir.setSorting(QDir::Name);
	QFileInfoList tmpList = tmpDir.entryInfoList();
	QFileInfo fileInfo;
	for(i=0;i<tmpList.size();i++) 
	{
		fileInfo = tmpList.at(i);
		tmpString = QDir::fromNativeSeparators(fileInfo.canonicalFilePath());
		if(lPathsToCheck.contains(tmpString, Qt::CaseInsensitive))
		{
			bool bWriteRights = false;
			bool bSkipTest = false;
			QString tmpString2 = tmpString + QDir::separator() + "checkPermissions";
			QFile tmpFile(tmpString2 + ".tmp");
			int nUniqueRetries = 5;
			while(tmpFile.exists())
			{
				if(nUniqueRetries == 0)
				{
					bSkipTest = true;
					break;
				}
				tmpString2 = tmpString2 + "_";
				tmpFile.setFileName(tmpString2 + ".tmp");
				nUniqueRetries--;
			}
			if(bSkipTest == false)
			{
				if(tmpFile.open(QIODevice::ReadWrite | QIODevice::Append))
				{
					tmpFile.close();
					tmpFile.remove();
					bWriteRights = true;
				}
				if(bWriteRights)
					lPathsToCheck.removeAll(tmpString);
			}
		}
	}
	if(lPathsToCheck.size() > 0)
	{
		if(bShowWarning)
		{
			QMessageBox::critical(this, globAppInfo->getInternalName(),
				QObject::tr("%1 does not have write permission for certain user directories, please fix this in the Tools->Options->Directories->'User Documents Root Directory' setting!\n\nWrite permissions are insufficient in:\n%2").arg(globAppInfo->getInternalName()).arg(lPathsToCheck.join("\n")));
		}
		return false;
	}
	return true;
}

bool MainWindow::copyDirectoryRecursively(const QString &srcFilePath, const QString &tgtFilePath)
{
	QFileInfo srcFileInfo(srcFilePath);
	if (srcFileInfo.isDir()) 
	{
		QDir targetDir(tgtFilePath);
		targetDir.cdUp();
		//if (QFileInfo(tgtFilePath).dir().exists() == false)
		//{
			if (targetDir.mkpath(tgtFilePath) == false)//QFileInfo(tgtFilePath).fileName()) == false)
				return false;
		//}
		QDir sourceDir(srcFilePath);
		QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
		foreach(const QString &fileName, fileNames) 
		{
			const QString newSrcFilePath = srcFilePath + QLatin1Char('/') + fileName;
			const QString newTgtFilePath = tgtFilePath + QLatin1Char('/') + fileName;
			if (copyDirectoryRecursively(newSrcFilePath, newTgtFilePath)==false)
				return false;
		}
	}
	else 
	{
		if (QFile::copy(srcFilePath, tgtFilePath)==false)
			return false;
	}
	return true;
}

void MainWindow::setAppDirectories()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	QStringList paths = qApp->libraryPaths();
	paths <<  MainAppInfo::pluginsDirPath();// pluginsDir.absolutePath();
#ifndef QT_NO_DEBUG_OUTPUT	
	qDebug() << "Plugin path defined: " << MainAppInfo::pluginsDirPath();	
#endif		
	qApp->setLibraryPaths(paths);
}

void MainWindow::showPluginGUI()
{
	QAction *action = qobject_cast<QAction *>(sender());
	Plugins->GetInterface(Plugins->GetIndex(action->data().toString()))->ShowGUI();	
}

void MainWindow::setScriptRunningStatus(GlobalApplicationInformation::ActiveScriptMode state)
{
	AppScriptStatus = state;
	switch (state)
	{
	case GlobalApplicationInformation::Debugging:
		{ 
			runDocumentAction->setEnabled(false);
//			debugScriptAction->setEnabled(false);
			abortDocumentAction->setEnabled(true);
			restartScriptEngineAction->setEnabled(true);
			break; 
		}	
	case GlobalApplicationInformation::Executing:
		{ 
			runDocumentAction->setEnabled(false);
//			debugScriptAction->setEnabled(false);
			abortDocumentAction->setEnabled(true);
			restartScriptEngineAction->setEnabled(true);
			break; 
		}	
	case GlobalApplicationInformation::NoScript:
		{ 
			runDocumentAction->setEnabled(false);
//			debugScriptAction->setEnabled(false);
			abortDocumentAction->setEnabled(false);
			restartScriptEngineAction->setEnabled(false);
			currentMainRunningScriptID = 0;
			break; 
		}	
	case GlobalApplicationInformation::Pending:
		{ 
			runDocumentAction->setEnabled(true);
//			debugScriptAction->setEnabled(true);
			abortDocumentAction->setEnabled(false);
			restartScriptEngineAction->setEnabled(false);
			currentMainRunningScriptID = 0;
			break; 
		}	
	case GlobalApplicationInformation::Stopping:
		{ 
			runDocumentAction->setEnabled(false);
//			debugScriptAction->setEnabled(false);
			abortDocumentAction->setEnabled(true);
			restartScriptEngineAction->setEnabled(true);
			currentMainRunningScriptID = 0;
			break; 
		}	
	}
}

QString MainWindow::getActiveDocumentFileLocation()
{
	return DocManager->getFilePath(activeMdiChild());
}

QString MainWindow::getQMLPluginPath()
{
	return MainAppInfo::qmlExtensionsPluginDirPath();
}

QString MainWindow::getApplicationRootDirPath() 
{
	return QDir(QCoreApplication::applicationDirPath()).absolutePath();
}

QString MainWindow::getActiveDocumentFileName()
{
	return DocManager->getFileName(activeMdiChild(),true);
}

void MainWindow::enableActiveDocument(bool bEnable)
{
	activeMdiChild()->setEnabled(bEnable);
}

QString MainWindow::getEnvironmentVariabele(QString strName) 
{
	return QProcessEnvironment::systemEnvironment().value(strName);
}

void MainWindow::onSubWindowClosed(CustomMDISubWindow* custSubWin)
{
	mdiArea->setActiveSubWindow(custSubWin);
	if (custSubWin)
		unregisterDockWidgets(custSubWin);
	closeAction->activate(QAction::Trigger);
	DocManager->remove(custSubWin);
	delete custSubWin;
	if (DocManager->count() == 0)
	{
		loadMainWindowLayout(MAINWINDOW_NAME);
	}
}

void MainWindow::loadMainWindowLayout(const QString &sGroupSection)
{
	Qt::DockWidgetArea tmpDockWidgetArea;
	removeDockWidget(debugLogDock);
	outputTabWidget->setGroupName(sGroupSection);
	loadSavedDockWidgetConfiguration(UI_SETTINGS_FILENAME, sGroupSection, debugLogDock, tmpDockWidgetArea);
	addRegisteredDockWidgets(tmpDockWidgetArea);
	loadSavedWindowLayout(UI_SETTINGS_FILENAME, sGroupSection);
}

void MainWindow::onSubWindowDestroyed(CustomMDISubWindow* custSubWin)
{
	Q_UNUSED(custSubWin);
	//DocManager->remove(custSubWin);//just to be sure..
}

void MainWindow::closeActiveDocument()
{
	QMdiSubWindow* activeWin = activeMdiChild();
	if(activeWin)
	{
		mdiArea->closeActiveSubWindow();
	}
	else
	{
		qWarning() << __FUNCTION__ << "Currently there's no active window to close!";
	}
}

void MainWindow::executeActiveDocument()
{
	QAction *action = qobject_cast<QAction *>(sender());

	if(action == NULL)
		BrainStimScriptRunMode = GlobalApplicationInformation::Execute;
	else
		BrainStimScriptRunMode = (GlobalApplicationInformation::ScriptRunMode)action->data().toInt();
	QMdiSubWindow *currentActiveWindow = activeMdiChild();
	QString strFileName = DocManager->getFileName(currentActiveWindow);

	switch (DocManager->getDocType(currentActiveWindow))
	{
		case GlobalApplicationInformation::DOCTYPE_QTSCRIPT:
		{ 
			break; 
		}
		case GlobalApplicationInformation::DOCTYPE_JAVASCRIPT:
		{ 
			break; 
		}		
		case GlobalApplicationInformation::DOCTYPE_SVG: 
		{
			CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(currentActiveWindow));
			if ((tmpCustomQsciScintilla) && ((tmpCustomQsciScintilla->isModified()) || (DocManager->isModified(currentActiveWindow))))
			{
				QMessageBox::StandardButton ret;
				ret = QMessageBox::warning(this, tr("Save Changes?"),
					tr("'%1' has unsaved changes that need to be saved to execute\n"
					"Do you want to save your changes first?")
					.arg(QFileInfo(activeMdiChildFilePath()).fileName()),
					QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		 		if (ret == QMessageBox::Save)
				{
					save();
					SVGPreviewer->openFile(DocManager->getFileName(currentActiveWindow));
					//SVGPreviewer->openFile(activeMdiChildFilePath()); abov code worked, this one didn't received an empty path...
					SVGPreviewer->showMaximized();
					//SVGPreviewer->showFullScreen();
				}
			}
			else 
			{
				SVGPreviewer->openFile(DocManager->getFileName(currentActiveWindow));
				//SVGPreviewer->openFile(activeMdiChildFilePath()); abov code worked, this one didn't received an empty path...
				SVGPreviewer->showMaximized();
				//GraphScene = new QGraphicsScene();
				//QGraphicsRectItem *rect = GraphScene->addRect(QRectF(0, 0, 100, 100));
				//QGraphicsItem *item = GraphScene->itemAt(50, 50);
				//GraphView = new QGraphicsView(GraphScene);
				//GraphView->showMaximized();
			}
			return;
			break;
		}
		case GlobalApplicationInformation::DOCTYPE_PLUGIN_DEFINED:
		{
			clearDebugger();
			//QTime t;
			QDir::setCurrent(getActiveDocumentFileLocation());

			QString strDocumentContent = "";
			CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(currentActiveWindow));
			if (tmpCustomQsciScintilla)
			{
				strDocumentContent = tmpCustomQsciScintilla->text();
			}
			else
			{
				strDocumentContent = strFileName;
			}
			QString strExtension = QFileInfo(DocManager->getFileName(currentActiveWindow)).completeSuffix();
			QString strDocHandlerSlotName = "";
			QObject* pluginObject = DocManager->getKnownDocumentFileHandlerObject(strExtension,strDocHandlerSlotName,DocumentManager::PLUGINHANDLER_SLOT_EXECUTE);
			
			if(pluginObject)
			{
				if (!(pluginObject->metaObject()->indexOfMethod(QMetaObject::normalizedSignature(strDocHandlerSlotName.toLatin1())) == -1))//Is the slot present?
				{
					//Invoke the slot
					bool bResult = false;
					bool bRetVal = false;
					QString shortSignature = strDocHandlerSlotName.left(strDocHandlerSlotName.indexOf("("));
					bResult = QMetaObject::invokeMethod(pluginObject,QMetaObject::normalizedSignature(shortSignature.toLatin1()),Qt::DirectConnection, Q_RETURN_ARG(bool,bRetVal), Q_ARG(QString,strDocumentContent), Q_ARG(QString,getActiveDocumentFileLocation()));				
				}
			}
			return;
			break;
		}
		default: //none of them
		{ 
			return;
			break;
		}
	}

	clearDebugger();
	QTime t;
	QDir::setCurrent(getActiveDocumentFileLocation());

	QString strScriptProgram = "";
	CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(currentActiveWindow));
	if (tmpCustomQsciScintilla)
		strScriptProgram = tmpCustomQsciScintilla->text();
	if (BrainStimScriptRunMode == GlobalApplicationInformation::Debug)
	{
		if (!AppScriptEngine->eng->canEvaluate(strScriptProgram))
		{
			write2OutputWindow("... DebugScript -> Script syntax error ...");
		}
	}
	QScriptSyntaxCheckResult syntaxChkResult = AppScriptEngine->eng->checkSyntax(strScriptProgram);
	bool bSyntaxValidated = false;
	if (syntaxChkResult.state() == QScriptSyntaxCheckResult::Error)
	{
		write2OutputWindow("... Invalid Script Syntax, the program contains a syntax error ...");
	}
	else if (syntaxChkResult.state() == QScriptSyntaxCheckResult::Intermediate)
	{
		write2OutputWindow("... Invalid Script Syntax, the program is incomplete ...");
	}
	else if(syntaxChkResult.state() == QScriptSyntaxCheckResult::Valid)
	{
		write2OutputWindow("... Valid Script Syntax ...");
		bSyntaxValidated = true;
	}
	if(!bSyntaxValidated)
	{
		write2OutputWindow("... Script Syntax error at(line " + QString::number(syntaxChkResult.errorLineNumber()) + ", col " + QString::number(syntaxChkResult.errorColumnNumber()) + "): " + syntaxChkResult.errorMessage() + " ...");
		CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(currentActiveWindow));
		if (tmpCustomQsciScintilla)
			tmpCustomQsciScintilla->setCursorPosition(syntaxChkResult.errorLineNumber()-1,syntaxChkResult.errorColumnNumber());
	}
	else
	{
		write2OutputWindow("... Script started Evaluating on " + t.currentTime().toString() + "...");
		t.start();

		QScriptValue result;

		CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(currentActiveWindow));
		if (tmpCustomQsciScintilla)
			result = executeScriptContent(tmpCustomQsciScintilla->text());
		QString strResult;
		strResult = result.toString();
		if (result.isError()) 
		{
			write2OutputWindow("... Script stopped Evaluating due to error on line " + result.property("lineNumber").toString() + ": --> " + result.toString() + "...");
			if (tmpCustomQsciScintilla)
				tmpCustomQsciScintilla->setCursorPosition(result.property("lineNumber").toInteger()-1,0);
		}
		else
		{
			int timeElapsed = t.elapsed();
			write2OutputWindow("... Total Time(ms): " + QString::number(timeElapsed));
			write2OutputWindow("... Script fully Evaluated on " + t.currentTime().toString() + " ...");
		}
	}	
	AppScriptEngine->eng->collectGarbage();
	//outputWindowList->scrollToBottom();
}

QScriptValue MainWindow::executeScriptFile(const QString &sFilePath)
{
	QFile scriptFile(sFilePath);
	if (scriptFile.exists())
	{
		QString sScriptContent;
		if (scriptFile.open(QFile::ReadOnly | QFile::Text))
			sScriptContent = scriptFile.readAll();
		scriptFile.close();
		return executeScriptContent(sScriptContent);
	}
	return NULL;
}

QScriptValue MainWindow::executeScriptContent(const QString &sContent)
{
	if(AppScriptEngine)
	{
		if(AppScriptEngine->eng)
		{
			if(pCurrentSetContextState.first != "")
			{
				if(currentScriptEngineContexes.isEmpty() == false)
				{		
					for(int i=0;i<currentScriptEngineContexes.count();i++)
					{
						if(currentScriptEngineContexes.at(i).sContextName == pCurrentSetContextState.first)//Should we overwrite?
						{
							QScriptContext *tmpContext = AppScriptEngine->eng->currentContext();
							tmpContext->setActivationObject(currentScriptEngineContexes[i].activationObject);
							tmpContext->setThisObject(currentScriptEngineContexes[i].thisObject);
							break;
						}
					}
				}
			}
			if (!AppScriptEngine->eng->canEvaluate(sContent))
			{
				QString tmpString = "... Could not evaluate the script object (" + sContent + ")!";
				write2OutputWindow(tmpString);
				qDebug() << __FUNCTION__ << tmpString;
				return NULL;
			}
			else
			{
				QScriptValue tmpScriptValue = AppScriptEngine->eng->evaluate(sContent);
				//QScriptValue tmpScriptValue = currentScriptEngine->evaluate(adjScriptContextStatement);
				if(AppScriptEngine->eng->hasUncaughtException())
				{
					QString tmpString = "... Evaluate script::UncaughtException() (" + sContent + ")!";
					write2OutputWindow(tmpString);
					qDebug() << __FUNCTION__ << tmpString;
				}
				//sScriptContextReturnValue = tmpScriptValue.toVariant();
				return tmpScriptValue;
			}
		}
	}
	return NULL;
}

void MainWindow::cleanupScript()
{
	QTimer::singleShot(10, this, SLOT(abortScript()));
	emit CleanUpScriptExecuted();
}

void MainWindow::activateMainWindow()
{
//#ifdef Q_OS_WIN32 //Are we on Windows?
//	::SetWindowPos(effectiveWinId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//	::SetWindowPos(effectiveWinId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//#endif
	this->raise();
	this->activateWindow();
}

void MainWindow::abortScript()
{
	AppScriptEngine->prepareObjectForGarbageCleanup(AppScriptEngine->eng->globalObject(),MAIN_PROGRAM_INTERNAL_NAME);
	AppScriptEngine->eng->collectGarbage();
	if((lCurrentRunningScriptIDList.count() == 1) && (currentMainRunningScriptID>0))
	{
		if(lCurrentRunningScriptIDList[0]!=currentMainRunningScriptID)
		{
			lCurrentRunningScriptIDList.clear();
			setScriptRunningStatus(GlobalApplicationInformation::Pending);
		}
	}
}

void MainWindow::openOptionsDialog()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	int returnVal;
	if (pMainOptionPage)
	{
		delete pMainOptionPage;
	}
	pMainOptionPage = new OptionPage(this, globAppInfo);
	pMainOptionPage->setPluginTabControlWidgets(mapPluginOptionWidgetsByPluginName);
	returnVal = pMainOptionPage->exec();

	switch (returnVal) 
	{
	case QDialog::Accepted:
		// Ok was clicked
		parseRemainingGlobalSettings();
		checkUserDirectories(MainAppInfo::getUserFolderList(),true);
		break;
	case QDialog::Rejected:
		// Cancel was clicked
		break;
	default:
		// should never be reached
		break;
	}		
	return;
}

void MainWindow::aboutBrainStim()
{
	aboutQTDialog aboutQTDlg(globAppInfo->getTitle(),pluginFileNames,MainAppInfo::pluginsDirPath(), pluginFileNames, this);
	aboutQTDlg.exec();
}

void MainWindow::setStartupFiles(const QString &path)
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	startUpFiles = path.split(";");
}

void MainWindow::openFiles(const QString &fileToLoad, const QStringList &filesToLoad, const bool &bViewAsText)
{
	if (DocManager->count() == 0)
	{
		saveWindowLayout(UI_SETTINGS_FILENAME, MAINWINDOW_NAME);
		saveDockWidgetConfiguration(UI_SETTINGS_FILENAME, MAINWINDOW_NAME, debugLogDock);
	}
	QStringList fileNames;
	if ((fileToLoad.isNull()) && (filesToLoad.count() == 0))
	{
		QString sPath = m_currentPath;
		if(sPath.isEmpty())
			sPath = MainAppInfo::appExampleDirPath();			
		fileNames = QFileDialog::getOpenFileNames(this, tr("Open File(s)"),sPath,DocManager->getKnownFileExtensionList());
	}
	if (!fileToLoad.isNull())
	{
		fileNames.append(fileToLoad);
	}
	if (filesToLoad.count() > 0)
	{
		foreach (QString file, filesToLoad)
		{
			fileNames.append(file);
		}
	}
	foreach (QString fileName, fileNames)
	{
		if (!fileName.isEmpty()) 
		{
			QFile file(fileName);
			if (file.exists()==false) 
			{
				QMessageBox::critical(this, tr("Open File"), QString("Could not open file '%1'.").arg(fileName));;
				return;
			}
			if(parseFile(file,bViewAsText))
			{
				updateMenuControls();
			}
		}
	}
}

bool MainWindow::parseFile(const QFile &file, const bool &bParseAsText)
{
	QMdiSubWindow *existing = findMdiChild(file.fileName());
	if (existing) 
	{
		mdiArea->setActiveSubWindow(existing);
		statusBar()->showMessage(tr("File already opened"), 2000);
		return true;
	}
	QFileInfo fileInfo(file);
	QString fileExtension;
	fileExtension = fileInfo.suffix();
	GlobalApplicationInformation::DocType tempDocType = DocManager->getDocType(fileExtension);
	int DocIndex;
	newDocument(tempDocType,DocIndex,fileExtension, fileInfo.canonicalFilePath(), bParseAsText);

	QApplication::setOverrideCursor(Qt::WaitCursor);
	if (DocManager->loadFile(DocIndex,file.fileName()))
	{	
		setCurrentFile(file.fileName());
		statusBar()->showMessage(tr("File loaded"), 2000);
		DocManager->initFile(DocIndex);
	} 
	else 
	{
		mdiArea->removeSubWindow(findMdiChild(file.fileName()));
		statusBar()->showMessage(tr("Could not load File"), 2000);
		QApplication::restoreOverrideCursor();
		return false;
	}
	loadMainWindowLayout(fileExtension);
	QApplication::restoreOverrideCursor();
	return true;
}

void MainWindow::setRenderer()
{
	QVariant tmpVariant;
	if (globAppInfo->getRegistryInformation(REGISTRY_RENDERTYPE, tmpVariant))
	{
		SvgView::RendererType type;
		type = (SvgView::RendererType)tmpVariant.toInt();
		switch (type)
		{
		case SvgView::Native://Native
			{
				SVGPreviewer->setHighQualityAntialiasing(false);
				SVGPreviewer->setRenderer(SvgView::Native);
				break;
			}
		case SvgView::OpenGL://OpenGL
			{
				if (globAppInfo->getRegistryInformation(REGISTRY_HQANTIALIAS,tmpVariant))
				{
					SVGPreviewer->setHighQualityAntialiasing(tmpVariant.toInt());
				}
				else
				{
					SVGPreviewer->setHighQualityAntialiasing(false);
				}
				SVGPreviewer->setRenderer(SvgView::OpenGL);
				break;
			}
		case SvgView::Image://Image
			{
				SVGPreviewer->setHighQualityAntialiasing(false);
				SVGPreviewer->setRenderer(SvgView::Image);
				break;
			}
		}
	}
	else //key doesn't exist, default value!
	{
		SVGPreviewer->setRenderer(SvgView::Native);
	}
}

void MainWindow::setCurrentFile(const QString &fileName, bool bIsNotSaved)
{
	if (fileName.isEmpty())
	{
		setWindowTitle(globAppInfo->getTitle());//MainAppInfo::MainProgramTitle());
	}
	else if(fileName == MainAppInfo::UntitledDocName())
	{
		setWindowTitle(tr("%1 - %2").arg(globAppInfo->getTitle()).arg(MainAppInfo::UntitledDocName()));
	}	
	else if (fileName.startsWith(":/") == false) 
	{
		m_currentPath = fileName;
		setWindowTitle(tr("%1 - %2").arg(globAppInfo->getTitle()).arg(strippedName(m_currentPath)));
		if (bIsNotSaved == false)
			updateRecentFileList(fileName);
	}
}

void MainWindow::updateRecentFileList(const QString &fileName)
{
	QStringList files;
	QVariant tmpVariant;
	if (globAppInfo->getRegistryInformation(REGISTRY_RECENTFILELIST, tmpVariant))
		files = tmpVariant.toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	globAppInfo->setRegistryInformation(REGISTRY_RECENTFILELIST,files,"stringlist");

	foreach (QWidget *widget, QApplication::topLevelWidgets()) {
		MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
		if (mainWin)
			mainWin->updateRecentFileActions();
	}
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	if (action)
		openFiles(action->data().toString());
}

void MainWindow::updateRecentFileActions()
{
	QStringList files;
	QVariant tmpVariant;
	if (globAppInfo->getRegistryInformation(REGISTRY_RECENTFILELIST, tmpVariant))
		files = tmpVariant.toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
		recentFileActs[i]->setText(text);
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		recentFileActs[j]->setVisible(false);

	separatorAct->setVisible(numRecentFiles > 0);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
	return QFileInfo(fullFileName).fileName();
}

void MainWindow::newFile()
{
	QAction *action = qobject_cast<QAction *>(sender());
	GlobalApplicationInformation::DocType docType;
	QString sExtension = "";
	if (action)
	{
		//QMap<QString, QVariant> tmpNewActionMapping;
		tmpNewActionMapping = action->data().toMap();
		if(tmpNewActionMapping.isEmpty() == false)
		{
			docType = (GlobalApplicationInformation::DocType)tmpNewActionMapping.values().at(0).toInt();//action->data().toInt();
			sExtension = DocManager->getDocTypeString(docType);
			if (sExtension.isEmpty())
				sExtension = tmpNewActionMapping.keys().at(0);
		}
		else
		{
			docType = GlobalApplicationInformation::DOCTYPE_UNDEFINED;
		}
	}
	else
	{
		docType = GlobalApplicationInformation::DOCTYPE_UNDEFINED;
	}

	int DocIndex;
	QString sCanoFileDir = QDir(MainAppInfo::appExampleDirPath()).canonicalPath() + "/";
	QString sCanoFilePath = sCanoFileDir + MainAppInfo::UntitledDocName() + "." + sExtension;
	int nUniqueUntitledCounter = 1;
	while (QFile(sCanoFilePath).exists())
	{
		sCanoFilePath = sCanoFileDir + MainAppInfo::UntitledDocName() + "_" + QString::number(nUniqueUntitledCounter) + "." + sExtension;
		nUniqueUntitledCounter++;
	}
	newDocument(docType, DocIndex, sExtension, sCanoFilePath, false, true);
	setCurrentFile(sCanoFilePath, true);// MainAppInfo::UntitledDocName());
	loadMainWindowLayout(sExtension);
	statusBar()->showMessage(tr("New File created"), 2000);
}

void MainWindow::newDocument(const GlobalApplicationInformation::DocType &docType, int &DocIndex, const QString &strExtension, const QString &strCanonicalFilePath, const bool &bNativeMainAppView, const bool &bTryToInitialize)
{
	DocIndex = 0;
	QWidget *newChild = DocManager->add(docType,DocIndex,strExtension,strCanonicalFilePath, bNativeMainAppView);
	CustomMDISubWindow *subWindow = new CustomMDISubWindow(this);
	subWindow->setWidget(newChild);
	mdiArea->addSubWindow(subWindow);
	subWindow->setAttribute(Qt::WA_DeleteOnClose);
	newChild->setAttribute(Qt::WA_DeleteOnClose);
	//subWindow->setToolTip(strCanonicalFilePath);
	DocManager->setSubWindow(DocIndex,subWindow);
	bool bResult = connect(subWindow, SIGNAL(SubWindowClosed(CustomMDISubWindow*)), this, SLOT(onSubWindowClosed(CustomMDISubWindow*)), Qt::DirectConnection);
	bResult = connect(subWindow, SIGNAL(SubWindowDestroyed(CustomMDISubWindow*)), this, SLOT(onSubWindowDestroyed(CustomMDISubWindow*)), Qt::DirectConnection);

	if(newChild->metaObject()->indexOfSignal(newChild->metaObject()->normalizedSignature(FUNC_PLUGIN_COPYAVAILABLE_FULL))>0)
	{
		bResult = connect(newChild, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));
		bResult = bResult && connect(newChild, SIGNAL(copyAvailable(bool)),	copyAction, SLOT(setEnabled(bool)));
	}
	if(newChild->metaObject()->indexOfSignal(newChild->metaObject()->normalizedSignature(FUNC_PLUGIN_CURSORPOSCHANGED_FULL))>0)
		bResult = connect(newChild, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(CursorPositionChangedHandler(int, int)));
	if (bTryToInitialize)
	{
		DocManager->setFileName(DocIndex, strCanonicalFilePath, true);
		DocManager->initFile(DocIndex);
	}
	newChild->showMaximized();
}

void MainWindow::setupStatusBar()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	StatusPositionLabel = new QLabel("");
	StatusFilePathLabel = new QLabel("");
	StatusLinesLabel = new QLabel("");
	StatusPositionLabel->setToolTip(QObject::tr("Cursor Position"));
	StatusFilePathLabel->setToolTip(QObject::tr("Document Path"));
	StatusLinesLabel->setToolTip(QObject::tr("Number of Lines"));
	this->statusBar()->addPermanentWidget(StatusFilePathLabel);
	this->statusBar()->addPermanentWidget(StatusLinesLabel);
	this->statusBar()->addPermanentWidget(StatusPositionLabel);
}

void MainWindow::NumberOfLinesChangedHandler(int nrOfLines)
{
	StatusLinesLabel->setText(QString("Lines: %1").arg(nrOfLines));
}

void MainWindow::save()
{
	QMdiSubWindow *tmpMdiChildPointer = activeMdiChild();
	if (tmpMdiChildPointer == NULL)
		return;
	QString tmpFileName = DocManager->getFileName(tmpMdiChildPointer);
	if (tmpMdiChildPointer)
	{
		if (tmpFileName == "")
		{
			return saveAs();
		}
		else
		{
			bool bShouldReparse = false;
			QString sNewSavedFileName;
			if (DocManager->saveFile(tmpMdiChildPointer, tmpFileName, &bShouldReparse, sNewSavedFileName))
			{
				if (sNewSavedFileName.isEmpty() == false)
				{
					tmpFileName = sNewSavedFileName;
					updateRecentFileList(sNewSavedFileName);
				}
				statusBar()->showMessage(tr("File saved"), 2000);
				if(bShouldReparse)
					check4ReParseFile(tmpFileName);
				return;
			}
			return;
		}
	}
}

void MainWindow::saveAs()
{
	QMdiSubWindow *tmpMdiChildPointer = activeMdiChild();
	if (tmpMdiChildPointer)
	{
		//QString selExt = "SVG files (*.svg *.svgz *.svg.gz)";
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),DocManager->getFileName(tmpMdiChildPointer));//,DocManager->getKnownFileExtensionList(),&selExt);
		if (fileName.isEmpty())
		{
			return;
		}
		else
		{
			bool bShouldReparse = false;
			QString sNewSavedFileName = "";
			if (DocManager->saveFile(tmpMdiChildPointer, fileName, &bShouldReparse, sNewSavedFileName))
			{
				if (sNewSavedFileName.isEmpty() == false)
					fileName = sNewSavedFileName;
				setCurrentFile(fileName);
				statusBar()->showMessage(tr("File saved"), 2000);
				if(bShouldReparse)
					check4ReParseFile(fileName);
				return;
			}
			return;
		}
	}
}

bool MainWindow::check4ReParseFile(const QString &sFilename)
{
	QMessageBox::StandardButton ret;	
	ret = QMessageBox::information(this, tr("New known file extension detected."),
		tr("The extension used for saving the file is recognized as a known extension and therefore the editor can be automatically configured for this document style format.\n"
		"The current document style set for the editor is different or has not yet been set.\n"
		"Do you allow BrainStim to re-load the document, which re-configures the editor, after it has been saved ?"),
		QMessageBox::Yes | QMessageBox::No);
	if (ret == QMessageBox::Yes)
	{
		closeActiveDocument();
		openFiles(sFilename);
		return true;
	}
	return false;
}

void MainWindow::cut()
{
	QMdiSubWindow *tmpMdiChildPointer = activeMdiChild();
	CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(tmpMdiChildPointer));
	if (tmpCustomQsciScintilla)
		tmpCustomQsciScintilla->cut();
}

void MainWindow::copy()
{
	QMdiSubWindow *tmpMdiChildPointer = activeMdiChild();
	CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(tmpMdiChildPointer));
	if (tmpCustomQsciScintilla)
		tmpCustomQsciScintilla->copy();
}

void MainWindow::paste()
{
	QMdiSubWindow *tmpMdiChildPointer = activeMdiChild();
	CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(tmpMdiChildPointer));
	if (tmpCustomQsciScintilla)
		tmpCustomQsciScintilla->paste();
}

void MainWindow::lineComment()
{
	CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpCustomQsciScintilla)
		tmpCustomQsciScintilla->toggleLineComment();
}
void MainWindow::blockComment()
{
	CustomQsciScintilla *tmpCustomQsciScintilla = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpCustomQsciScintilla)
		tmpCustomQsciScintilla->toggleBlockComment();
}
void MainWindow::updateWindowMenu()
{
	windowMenu->clear();
	windowMenu->addAction(reloadAction);
	windowMenu->addAction(closeAction);
	windowMenu->addAction(closeAllAction);
	//windowMenu->addSeparator();
	//windowMenu->addAction(tileAction);
	//windowMenu->addAction(cascadeAction);
	windowMenu->addSeparator();
	windowMenu->addAction(nextAction);
	windowMenu->addAction(previousAction);

	QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	separatorAct->setVisible(!windows.isEmpty());

	for (int i = 0; i < windows.size(); ++i) 
	{
		QMdiSubWindow *child = qobject_cast<QMdiSubWindow *>(windows.at(i));
		QString text = QFileInfo(DocManager->getFileName(child)).fileName();
		if (text == "")
		{
			text = MainAppInfo::UntitledDocName();
		}
		if (i < 9) {
			text = tr("&%1 %2").arg(i + 1).arg(text);
		} else {
			text = tr("%1 %2").arg(i + 1).arg(text);
		}
		QAction *action  = windowMenu->addAction(text);
		action->setShortcut(QString("Ctrl+") + QString::number(i+1));
		action->setCheckable(true);
		//action ->setChecked(child == DocManager->getDocHandler(activeMdiChild()));
		action->setChecked(windows.at(i) == activeMdiChild());
		connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
		windowMapper->setMapping(action, windows.at(i));
	}
}

void MainWindow::updateMarkersMenu()
{
	if (markersMenu) 
	{
		for (int i = 0; i < markersMenu->actions().count(); ++i)
		{
			markersMenu->removeAction(markersMenu->actions().at(i));
		}
		markersMenu->clear();
		markersMenu->addAction(addRemMarkerAction);
		markersMenu->addAction(nextMarkerAction);
		markersMenu->addAction(prevMarkerAction);
		markersMenu->addAction(remAllMarkerAction);

		if(DocManager->count()>0)
		{
			CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
			if ( tmpSci && !tmpSci==0 ) 
			{
				QList<int> list = tmpSci->markers();
				if ( !list.isEmpty() ) {
					markersMenu->addSeparator();
					//foreach (int line, list) 
					for (int i = 0; i < list.size(); ++i)
					{
						QString lineStr = tmpSci->getLine(list.at(i)).simplified();
						if ( lineStr.length() > 40 )
							lineStr = lineStr.left(40) + " ...";
						QAction *action  = markersMenu->addAction(QString("%1: %2").arg(list.at(i) + 1).arg(lineStr), this, SLOT(gotoMarker()));
						action->setShortcut(QString("Ctrl+Alt+") + QString::number(i+1));
					}
				}
			}
		}
	}
}

void MainWindow::gotoMarker() 
{
	QAction* a = qobject_cast<QAction*>(sender());
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if ( tmpSci && !tmpSci == 0 && a ) {
		int line = a->text().section(':', 0, 0).toInt();
		tmpSci->gotoLine(line - 1);
	}
}

void MainWindow::setupToolBars()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->setObjectName("File");
	//fileToolBar->addAction(newDocumentAction);
	//fileToolBar->addWidget(fileNewMenu);	
	fileToolBar->addAction(openAction);
	fileToolBar->addAction(saveAction);

	editToolBar = addToolBar(tr("Edit"));
	editToolBar->setObjectName("Edit");
	editToolBar->addAction(cutAction);
	editToolBar->addAction(copyAction);
	editToolBar->addAction(pasteAction);

	toolsToolBar = addToolBar(tr("Tools"));
	toolsToolBar->setObjectName("Tools");
	toolsToolBar->addAction(runDocumentAction);
	//toolsToolBar->addAction(debugScriptAction);
	toolsToolBar->addAction(abortDocumentAction);
	toolsToolBar->addAction(restartScriptEngineAction);
}

void MainWindow::parseRemainingGlobalSettings()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	setRenderer();
	configureDebugger();	
}

void MainWindow::recoverLastScreenWindowSettings()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	loadSavedWindowLayout(UI_SETTINGS_FILENAME, MAINWINDOW_NAME);
}

void MainWindow::loadSavedDockWidgetConfiguration(const QString &sSettingsFileName, const QString &sGroupName, QDockWidget *dockWidget, Qt::DockWidgetArea &defaultArea)
{
	//QSettings *sUILayoutSettings = NULL;
	QString tmpString;
	//if (hashUILayoutSettings.contains(sSettingsFileName))
	//{
	//	sUILayoutSettings = hashUILayoutSettings.value(sSettingsFileName);
	//}
	//else
	//{
	//	sUILayoutSettings = new QSettings(sSettingsFileName, QSettings::IniFormat);
	//	hashUILayoutSettings.insert(sSettingsFileName, sUILayoutSettings);
	//}
	QString sAccesName = dockWidget->accessibleName();
	//sUILayoutSettings->beginGroup(sGroupName);

	tmpString = sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_ISFLOATING;
	bool bIsFloating = false;
	//if (sUILayoutSettings->contains(tmpString))
	//{
	//	bIsFloating = sUILayoutSettings->value(tmpString).toBool();
	//	dockWidget->setFloating(bIsFloating);
	//}
	QVariant tmpVariantValue;
	bool bResult = globAppInfo->getRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_ISFLOATING), tmpVariantValue);
	if (bResult)
		dockWidget->setFloating(tmpVariantValue.toBool());

	tmpString = sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_ISVISIBLE;
//	if (sUILayoutSettings->contains(tmpString))
//		dockWidget->setVisible(sUILayoutSettings->value(tmpString).toBool());
	bResult = globAppInfo->getRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_ISVISIBLE), tmpVariantValue);
	if (bResult)
		dockWidget->setVisible(tmpVariantValue.toBool());
	
	if (bIsFloating==false)
	{
		tmpString = sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_DOCKWIDGETAREA;
//		if (sUILayoutSettings->contains(tmpString))
//			defaultArea = (Qt::DockWidgetArea)sUILayoutSettings->value(tmpString).toInt();
		bResult = globAppInfo->getRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_DOCKWIDGETAREA), tmpVariantValue);
		if (bResult)
			defaultArea = (Qt::DockWidgetArea)tmpVariantValue.toInt();
	}
	tmpString = sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_GEOMETRY;
	//if (sUILayoutSettings->contains(tmpString))
	//{
		//QRect tmpGeometry = sUILayoutSettings->value(tmpString).toRect();
		//dockWidget->setGeometry(tmpRect);
		//mapLoadedDockWindowRects.insert(sGroupName + EXPERIMENT_DOCKWIDGET_GROUPSEP_CHAR + sAccesName, tmpGeometry);
		//if (bIsFloating)
		//	dockWidget->setGeometry(tmpGeometry);
	//}
	bResult = globAppInfo->getRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_GEOMETRY), tmpVariantValue);
	if (bResult)
	{
		QRect tmpGeometry = tmpVariantValue.toRect();
		//dockWidget->setGeometry(tmpRect);
		mapLoadedDockWindowRects.insert(sGroupName + EXPERIMENT_DOCKWIDGET_GROUPSEP_CHAR + sAccesName, tmpGeometry);
		//if (bIsFloating)
			dockWidget->setGeometry(tmpGeometry);
	}
	//sUILayoutSettings->endGroup();
}

void MainWindow::saveDockWidgetConfiguration(const QString &sSettingsFileName, const QString &sGroupName, QDockWidget *dockWidget)
{
	//QSettings *sUILayoutSettings = NULL;
	//if (hashUILayoutSettings.contains(sSettingsFileName))
	//{
	//	sUILayoutSettings = hashUILayoutSettings.value(sSettingsFileName);
	//}
	//else
	//{
	//	sUILayoutSettings = new QSettings(sSettingsFileName, QSettings::IniFormat);
	//	hashUILayoutSettings.insert(sSettingsFileName, sUILayoutSettings);
	//}
	if (dockWidget)
	{
		QString sAccesName = dockWidget->accessibleName();
		//sUILayoutSettings->beginGroup(sGroupName);
		int nNewDockWidgetArea = (int)dockWidgetArea(dockWidget);
		//sUILayoutSettings->setValue(sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_DOCKWIDGETAREA, nNewDockWidgetArea);
		bool bIsFloating = dockWidget->isFloating();
		//sUILayoutSettings->setValue(sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_ISFLOATING, bIsFloating);
		//sUILayoutSettings->setValue(sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_GEOMETRY, dockWidget->geometry());
		//sUILayoutSettings->setValue(sAccesName + EXPERIMENT_DOCKWIDGET_PROPSEP_CHAR + DOCKITEM_SETTINGNAME_ISVISIBLE, dockWidget->isVisible());
		//sUILayoutSettings->endGroup();

		bool bResult = globAppInfo->setRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_DOCKWIDGETAREA), nNewDockWidgetArea, "int");
		bResult = globAppInfo->setRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_ISFLOATING), bIsFloating, "bool");
		bResult = globAppInfo->setRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_GEOMETRY), dockWidget->geometry(), "rect");
		bResult = globAppInfo->setRegistryInformation(QString("%1/%2/%3/%4").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(sAccesName).arg(DOCKITEM_SETTINGNAME_ISVISIBLE), dockWidget->isVisible(), "bool");
	}
}

void MainWindow::loadSavedWindowLayout(const QString &sSettingsFileName, const QString &sGroupName)
{
	//QSettings *sUILayoutSettings = NULL;
	//if (hashUILayoutSettings.contains(sSettingsFileName))
	//{
	//	sUILayoutSettings = hashUILayoutSettings.value(sSettingsFileName);
	//}
	//else
	//{
	//	sUILayoutSettings = new QSettings(sSettingsFileName, QSettings::IniFormat);
	//	hashUILayoutSettings.insert(sSettingsFileName, sUILayoutSettings);
	//}
	//sUILayoutSettings->beginGroup(sGroupName);
	//restoreState(sUILayoutSettings->value("state").toByteArray());
	//if (sGroupName == MAINWINDOW_NAME)
		//restoreGeometry(sUILayoutSettings->value("geometry").toByteArray());
	//sUILayoutSettings->endGroup();

	QVariant tmpVariantValue;
	bool bResult = globAppInfo->getRegistryInformation(QString("%1/%2/%3").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(DOCKITEM_SETTINGNAME_GEOMETRY), tmpVariantValue);
	if (bResult)
		restoreGeometry(tmpVariantValue.toByteArray());
	bResult = globAppInfo->getRegistryInformation(QString("%1/%2/%3").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(DOCKITEM_SETTINGNAME_STATE), tmpVariantValue);
	if (bResult)
		restoreState(tmpVariantValue.toByteArray());

	QMdiSubWindow *subWindow = activeMdiChild();
	if (subWindow)
	{
		for (int i = 0; i < mapMDISubWindowToDockWidget.count(); i++)
		{
			if (mapMDISubWindowToDockWidget.keys().at(i) != subWindow)
			{
				if (dockWidgetArea(mapMDISubWindowToDockWidget.values()[i]) != Qt::NoDockWidgetArea)
					removeDockWidget(mapMDISubWindowToDockWidget.values()[i]);
			}
		}
	}
}

void MainWindow::saveWindowLayout(const QString &sSettingsFileName, const QString &sGroupName)
{
	//QSettings *sUILayoutSettings = NULL;
	//if (hashUILayoutSettings.contains(sSettingsFileName))
	//{
	//	sUILayoutSettings = hashUILayoutSettings.value(sSettingsFileName);
	//}
	//else
	//{
	//	sUILayoutSettings = new QSettings(sSettingsFileName, QSettings::IniFormat);
	//	hashUILayoutSettings.insert(sSettingsFileName, sUILayoutSettings);
	//}
	//sUILayoutSettings->beginGroup(sGroupName);
	//if (sGroupName == MAINWINDOW_NAME)
	//	sUILayoutSettings->setValue("geometry", saveGeometry());
	//sUILayoutSettings->setValue("state", saveState());
	//sUILayoutSettings->endGroup();

	bool bResult = globAppInfo->setRegistryInformation(QString("%1/%2/%3").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(DOCKITEM_SETTINGNAME_GEOMETRY), saveGeometry(), "bytearray");
	bResult = globAppInfo->setRegistryInformation(QString("%1/%2/%3").arg(DOCKITEM_SETTINGNAME_USERINTERFACE).arg(sGroupName).arg(DOCKITEM_SETTINGNAME_STATE), saveState(), "bytearray");

}

QAction *MainWindow::registerMainMenuAction(const QStringList &lmenuItemSpecifier, QIcon *iMenuIcon, const bool &bSkipSubWindowRegistration)
{
	QMenuBar *mainMenuBar = menuBar();
	if (lmenuItemSpecifier.isEmpty() == false)
	{
		QList<QAction *> currentMenuActions = mainMenuBar->actions();
		if (mainMenuBar)
		{
			QAction *currentAction = NULL;
			QMenu *currentMenu = NULL;
			QAction *lastAction = NULL;
			QMenu *firstMenu = NULL;
			bool bItemFound;
			int nCurrentSpecItemIndex = 0;
			bool bStartAppendingMenus = false;
			foreach(QString sMenuItemSpec, lmenuItemSpecifier)
			{
				nCurrentSpecItemIndex++;
				bItemFound = false;
				if (bStartAppendingMenus == false)
				{
					foreach(currentAction, currentMenuActions)
					{
						QString sActionText = currentAction->text();
						if (sActionText.remove("&").remove(".") == sMenuItemSpec)//Item found?
						{
							if (nCurrentSpecItemIndex == lmenuItemSpecifier.count())//Last Index?
							{
								//bool bResult = DocManager->menuActionRegistration(mdiArea->activeSubWindow(), currentAction);
								//return currentAction;
								return NULL; //! Cannot use already present menu actions...
							}
							bItemFound = true;
							//if (nCurrentSpecItemIndex > 1)
								currentMenu = currentAction->menu();
							//else
							//	currentMenuActions = currentMenu->actions();
							if (currentMenu)
							{
								currentMenuActions = currentMenu->actions();
								break;
							}
						}
					}
				}
				if (bItemFound == false)
				{
					if (nCurrentSpecItemIndex == lmenuItemSpecifier.count())
					{
						if (currentMenu)
						{
							if (sMenuItemSpec == "_")//Separator?
							{
								currentAction = currentMenu->addSeparator();
							}
							else
							{
								currentAction = currentMenu->addAction(sMenuItemSpec);
							}
						}
						else
						{
							if (sMenuItemSpec == "_")//Separator?
							{
								currentAction = mainMenuBar->addSeparator();
							}
							else
							{
								currentAction = mainMenuBar->addAction(sMenuItemSpec);
							}
						}
						lastAction = currentAction;
						//if (bStartAppendingMenus == false)
						//	lastMenu = currentMenu;
						//bool bResult =
						if (bSkipSubWindowRegistration == false)
							DocManager->addMenuActionRegistration(mdiArea->activeSubWindow(), firstMenu, lastAction);
						if (iMenuIcon)
							currentAction->setIcon(*iMenuIcon);
						return currentAction;
					}
					else
					{
						if (currentMenu)
							currentMenu = currentMenu->addMenu(sMenuItemSpec);
						else
							currentMenu = mainMenuBar->addMenu(sMenuItemSpec);
						if (bStartAppendingMenus == false)
							firstMenu = currentMenu;
					}
					bStartAppendingMenus = true;
				}
			}
		}
	}
	return NULL;
}

bool MainWindow::getSavedDockWidgetSizeHint(const QString &sDockWidgetGroupName, const QString &sDockWidgetAccessName, QRect &rSizeHint)
{
	if (mapLoadedDockWindowRects.contains(sDockWidgetGroupName + EXPERIMENT_DOCKWIDGET_GROUPSEP_CHAR + sDockWidgetAccessName))
	{
		rSizeHint = mapLoadedDockWindowRects.value(sDockWidgetGroupName + EXPERIMENT_DOCKWIDGET_GROUPSEP_CHAR + sDockWidgetAccessName);
		return true;
	}
	return false;
}

bool MainWindow::configureDebugger()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	QVariant tmpVariant;
	if (globAppInfo->getRegistryInformation(REGISTRY_OPENINEXTERNALDEBUGGER, tmpVariant))
		AppScriptEngine->ConfigureDebugger((bool)tmpVariant.toInt());
	else//default
		AppScriptEngine->ConfigureDebugger(false);
	return true;
}

void MainWindow::writeMainWindowSettings()
{
	if(BrainStimFlags & GlobalApplicationInformation::VerboseMode)
		qDebug() << "Verbose Mode: " << __FUNCTION__;
	saveWindowLayout(UI_SETTINGS_FILENAME, MAINWINDOW_NAME);
	saveDockWidgetConfiguration(UI_SETTINGS_FILENAME, MAINWINDOW_NAME, debugLogDock);
}

bool MainWindow::closeSubWindow(bool bAutoSaveChanges)
{
	QAction *action = qobject_cast<QAction *>(sender());
	QString sNewFileName = "";
	if (action == NULL)//direct function call, exit program
	{
		QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
		for (int i = 0; i < windows.size(); ++i) 
		{
			mdiArea->setActiveSubWindow(windows.at(i));
			if (!DocManager->maybeSave(windows.at(i), bAutoSaveChanges, sNewFileName))
				return false;
			else if (sNewFileName.isEmpty() == false)
				updateRecentFileList(sNewFileName);
			sCurrentClosingSubWindowIds.append(i);
		}
		DocManager->setAllUnmodified();//to prevent a maybesave() check loop
		return true;
	}
	else if (action->data().toString() == "Close")//From menu(Close)
	{
		QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
		int nSubWindowIndex = windows.indexOf(activeMdiChild());
		if (sCurrentClosingSubWindowIds.contains(nSubWindowIndex))//already closing?
			return true;
		if (nSubWindowIndex >= 0)
		{
			if (!DocManager->maybeSave(windows.at(nSubWindowIndex), bAutoSaveChanges, sNewFileName))
				return false;
			else if (sNewFileName.isEmpty() == false)
				updateRecentFileList(sNewFileName);
		}
		sCurrentClosingSubWindowIds.append(nSubWindowIndex);
		mdiArea->closeActiveSubWindow();
		sCurrentClosingSubWindowIds.removeOne(nSubWindowIndex);
		return true;
	}
	else if (action->data().toString() == "CloseAll")//From menu(Close All)
	{
		QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
		for (int i = 0; i < windows.size(); ++i) 
		{
			if (sCurrentClosingSubWindowIds.contains(i))//already closing?
				continue;
			mdiArea->setActiveSubWindow(windows.at(i));
			if (!DocManager->maybeSave(windows.at(i), bAutoSaveChanges, sNewFileName))
				return false;
			else if (sNewFileName.isEmpty() == false)
				updateRecentFileList(sNewFileName);
			sCurrentClosingSubWindowIds.append(i);
		}
		DocManager->setAllUnmodified();//to prevent a maybesave() check loop
		mdiArea->closeAllSubWindows();
		return true;
	}
	else //This must be called from the script?
	{
		mdiArea->closeActiveSubWindow();
		return true;
	}
	return false;
}

QString MainWindow::activeMdiChildFilePath()
{
	if (activeMdiChild())
		return activeMdiChild()->windowFilePath();
	return QString();
}

QMdiSubWindow *MainWindow::activeMdiChild()
{
	QMdiSubWindow* tmpSubWin = mdiArea->activeSubWindow();
	if (tmpSubWin)
		return tmpSubWin;
	return NULL;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
	QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
	foreach (QMdiSubWindow *window, mdiArea->subWindowList()) 
	{
		if (DocManager->getFileName(window) == canonicalFilePath)
			return window;
	}
	return 0;
}

void MainWindow::jumpToMatchingBrace()
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->moveToMatchingBrace();
}

void MainWindow::selectToMatchingBrace()
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->selectToMatchingBrace();
}

void MainWindow::duplicateLine()
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
	{
		if(tmpSci->hasSelectedText())
			tmpSci->SendScintilla(QsciScintilla::SCI_SELECTIONDUPLICATE);
		else
			tmpSci->SendScintilla(QsciScintilla::SCI_LINEDUPLICATE);
	}
}

void MainWindow::moveLineUp()
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
	{
		tmpSci->SendScintilla(QsciScintilla::SCI_LINETRANSPOSE);
		tmpSci->SendScintilla(QsciScintilla::SCI_LINEUP);
	}
}

void MainWindow::deleteCurrentLine()
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->SendScintilla(QsciScintilla::SCI_LINEDELETE);
}

void MainWindow::toUpperCase()
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->SendScintilla(QsciScintilla::SCI_UPPERCASE);
}

void MainWindow::toLowerCase()
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->SendScintilla(QsciScintilla::SCI_LOWERCASE);
}

void MainWindow::print()
{
#ifndef QT_NO_PRINTDIALOG
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
	{
		if (tmpSci->print())
		{
			statusBar()->showMessage(tr("Printing..."), 2000);
		}
//	QTextDocument *document = activeMdiChild()->sciObject->document();
//	QPrinter printer;
//
//	QPrintDialog *dlg = new QPrintDialog(&printer, this);
//	if (dlg->exec() != QDialog::Accepted)
//		return;
//	document->print(&printer);
//	
	}
#endif
}

void MainWindow::handleToggledMarker(int nLine) 
{
	Q_UNUSED(nLine);
	updateMarkersMenu();
}

void MainWindow::toggleMarker(int nLine) 
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
	{
		if(nLine>=0)
			tmpSci->toggleMarker(nLine);
		else
			tmpSci->toggleMarker();
		updateMarkersMenu();
	}
}

void MainWindow::nextMarker() 
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->nextMarker();
}

void MainWindow::prevMarker() 
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->prevMarker();
}

void MainWindow::removeAllMarkers() 
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
		tmpSci->removeAllMarkers();
}

void MainWindow::goToLine() 
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
	{
		bool ok = false;
		int line = QInputDialog::getInt(mdiArea, tr("Go to line"), 
			tr("Go to line") + QString(" (1 - %1):").arg(tmpSci->lineCount()), 
			1, 1, tmpSci->lineCount(), 1, &ok);
		if ( ok )
			tmpSci->gotoLine(line - 1);
	}
}

void MainWindow::findImpl(bool bReplace, bool useParams, QString strFindString, QString strReplaceString, DocFindFlags findFlags, bool bReplaceAll) 
{
	QMdiSubWindow *tmpMdiChildPointer = activeMdiChild();
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(tmpMdiChildPointer));
	if (tmpSci)
	{	
		DocFindFlags flags(bReplace);
		QString str1, str2;
		int line1, col1, line2, col2;
		tmpSci->getSelection(&line1, &col1, &line2, &col2);
		if ( line1 == line2 ) {
			if ( col1 != col2 ) {
				str1 = tmpSci->selectedText();
			}
			else {
				str1 = tmpSci->wordUnderCursor();
			}
		}
		if(useParams)
		{
			if ( bReplace ){//findFlags.replace ) {
				tmpSci->replace(strFindString,strReplaceString, findFlags, bReplaceAll);
			}
			else {
				tmpSci->find(strFindString, findFlags);
			}
		}
		else
		{
			if ( DocManager->getFindParams(tmpMdiChildPointer,str1, str2, flags) ) {
				if ( flags.replace ) {
					tmpSci->replace(str1, str2, flags, bReplaceAll);
				}
				else {
					tmpSci->find(str1, flags);
				}
			}	
		}
	}
}

void MainWindow::find(bool useParams, QString strFindString, DocFindFlags findFlags) 
{
	findImpl(false,useParams,strFindString,"",findFlags,false);
}

void MainWindow::replace(bool bReplaceAll, bool useParams, QString strFindString, QString strReplaceString, DocFindFlags findFlags) 
{
	findImpl(true,useParams,strFindString,strReplaceString,findFlags,bReplaceAll);
}

void MainWindow::findNext() 
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
	{	
		QString lastText = DocManager->lastFindText();
		if ( lastText.isEmpty() ) {
			find();
		}
		else {
			DocFindFlags flags = DocManager->lastFlags();
			flags.replace = false;
			flags.backwards = false;
			tmpSci->find(lastText, flags);
		}
	}
}

void MainWindow::findPrev() 
{
	CustomQsciScintilla *tmpSci = qobject_cast<CustomQsciScintilla*>(DocManager->getDocHandler(activeMdiChild()));
	if (tmpSci)
	{
		QString lastText = DocManager->lastFindText();
		if ( lastText.isEmpty() ) {
			find();
		}
		else {
			DocFindFlags flags = DocManager->lastFlags();
			flags.replace = false;
			flags.backwards = true;
			tmpSci->find(lastText, flags);
		}
	}
}

QString MainWindow::getCurrentDateTimeStamp(const QString &sFormat)
{
	return (QDateTime::currentDateTime().toString(sFormat));
}

bool MainWindow::getStoredSettingValue(const QString &sSettingPath, QString *sResult)
{
	QVariant varTmpValue;
	if (globAppInfo->getRegistryInformation(sSettingPath, varTmpValue))
	{
		*sResult = varTmpValue.toString();
		return true;
	}
	return false;
}

bool MainWindow::getPluginCustomConfigurationOptions(const QString &sInternalPluginName)
{
	if (sInternalPluginName.isEmpty())
		return false;
	QString sPath = ":/resources/";
	QString sResolvedFileName = sInternalPluginName.toLower() + ".xdef";
	if (QFile(sPath + sResolvedFileName).exists())
	{
		PropertySettingsWidgetContainer *tmpExpParWidgets = PropertySettingsWidgetContainer::instance();
		if (tmpExpParWidgets->loadExperimentParameterDefinition(sResolvedFileName, sInternalPluginName.toLower(), true, sPath))
		{
			PropertySettingsWidget *tmpExpParamViz = tmpExpParWidgets->getExperimentParameterWidget(sInternalPluginName.toLower());
			if (tmpExpParamViz)
			{
				mapPluginOptionWidgetsByPluginName.insert(sInternalPluginName, tmpExpParamViz);
				return true;
			}
		}
	}
	return false;
}

#ifdef DEBUG
void MainWindow::debugTestSlot()
{
	//QString sName = "ExperimentManager";
	//QString sPath = ":/resources/";
	//QString sResolvedFileName = sName.toLower() + ".xdef";

	//if (QFile(sPath + sResolvedFileName).exists())
	//{
	//	PropertySettingsWidgetContainer *tmpExpParWidgets = PropertySettingsWidgetContainer::instance();
	//	if (tmpExpParWidgets->loadExperimentParameterDefinition(sResolvedFileName, sName.toLower(), true, sPath))
	//	{
	//		PropertySettingsWidget *tmpExpParamViz = tmpExpParWidgets->getExperimentParameterWidget(sName.toLower());
	//		if (tmpExpParamViz)
	//		{
	//			mapPluginOptionWidgetsByPluginName.insert(sName, tmpExpParamViz);
	//		}
	//	}
	//}
}
#endif
