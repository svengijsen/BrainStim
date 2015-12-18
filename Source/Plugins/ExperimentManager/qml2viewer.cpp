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

#include "qml2viewer.h"
#include "qml2modelindexprovider.h"
#include <QQmlError>
#include <QQmlEngine>
#include <QMetaObject>
#include <QQuickItem>
#include <QQmlProperty>

/*! \brief The QML2Viewer constructor.
*
*   You do not need to specify the parent object. 
*	The BrainStim script engine automatically retrieves the parent role
*/
QML2Viewer::QML2Viewer(QObject *parent) : ExperimentEngine(parent)
{
	initialize();
}

/*! \brief The QML2Viewer destructor.
*
*   You do not need call the destructor. 
*	The BrainStim script engine automatically performs the garbage collection after you set the object to NULL and the script ends
*/
QML2Viewer::~QML2Viewer()
{
	if (imgLstModel)
	{
		delete imgLstModel;
		imgLstModel = NULL;
	}	
	deleteQML2ViewerWindow();
	if (rootObject)
	{		
		rootObject = NULL;//delete rootObject; only a pointer..
	}
	//if (qmlErrorHandler)
	//{
	//	delete qmlErrorHandler;
	//	qmlErrorHandler = NULL;
	//}
	if(tmpFile.exists())
		if(tmpFile.remove() == false)
			qWarning() << __FUNCTION__ << "Could not remove a temporarily file(" << tmpFile.fileName() << ")"; 
}

void QML2Viewer::deleteQML2ViewerWindow()
{
	last_qmlMainFilePath = "";
	if(quick2ViewerWindow)
	{
		bool bResult = disconnect(quick2ViewerWindow, SIGNAL(statusChanged(QQuickView::Status)), this, SLOT(onStatusChanged(QQuickView::Status)));
		bResult = disconnect(quick2ViewerWindow, &QtQuick2ApplicationViewer::QtQuickEngineQuit, this, &QML2Viewer::onQuick2ViewWindowClosed);
		//parentWindow->close();
		//delete parentWindow;
		//parentWindow = NULL;
		quick2ViewerWindow->close();
		delete quick2ViewerWindow;
		quick2ViewerWindow = NULL;
	}
}

bool QML2Viewer::executeQML2Document(const QString &strSource, bool bIsFile)//QVBoxLayout *layout) 
{	
	bool bRetVal = false;
	if(bIsFile)
		qmlMainFilePath = strSource;
	else
		qmlMainFilePath = "";
	if(initObject())
	{
		if(bIsFile)
		{
			bRetVal = startObject();
			if((experimentManager == NULL) && (currentExperimentStructure == NULL))//Here we should just run the QML file
			{
				qml2EventRoutine();
			}
		}
		else
		{
			bRetVal = startObject();
			qml2EventRoutine(strSource);
			return true;
		}
	}
	return bRetVal;
}

bool QML2Viewer::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		QScriptEngine *currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		setScriptEngine(currentScriptEngine);
		return true;
	}
	return false;
}

QScriptValue QML2Viewer::ctor_QML2Viewer(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	QML2Viewer *tmpObject = new QML2Viewer();
	tmpObject->setScriptEngine(engine);
	return engine->newQObject(tmpObject, QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
} 

//bool QML2Viewer::setScriptEngine(QScriptEngine *pScriptEngine) 
//{
//	currentScriptEngine = pScriptEngine;
//	return true;
//}

void QML2Viewer::initialize()
{
	//Default values if none defined.
	experimentManager = NULL;
	currentExperimentStructure = NULL;
	nQML2ViewerID = -1;
	rectScreenRes = QGuiApplication::primaryScreen()->geometry();	
	quick2ViewerWindow = NULL;
	rootObject = NULL;
	imgLstModel = NULL;
	last_qmlMainFilePath = "";
	bIsFramelessWindow = true;
	bFullScreenMode = true;
	pointWindowStartPosition = QPoint(0, 0);
}

bool QML2Viewer::initObject()
{
	bFirstQuickWindowActivation = true;
	bExperimentUnlocked = false;
	//parentWindow = new QWindow();
	//parentWindow->setFlags(Qt::FramelessWindowHint);
	quick2ViewerWindow = new QtQuick2ApplicationViewer(this);//parentWindow);
	quick2ViewerWindow->installEventFilter(this);
	bool bResult = connect(quick2ViewerWindow, SIGNAL(statusChanged(QQuickView::Status)), this, SLOT(onStatusChanged(QQuickView::Status)), Qt::ConnectionType(Qt::UniqueConnection));
	bResult = connect(quick2ViewerWindow, &QtQuick2ApplicationViewer::QtQuickEngineQuit, this, &QML2Viewer::onQuick2ViewWindowClosed, Qt::ConnectionType(Qt::UniqueConnection));
	if(experimentManager)
	{
		currentExperimentStructure = experimentManager->getExperimentStructure();
		bResult = connect(currentExperimentStructure, &cExperimentStructure::experimentStarted, this, &QML2Viewer::onStartTriggerRecieved, Qt::ConnectionType(Qt::UniqueConnection));
	}
	else
	{
		currentExperimentStructure = NULL;
	}		
	if (!imgLstModel)
	{
		imgLstModel = new ImageListModel();
		quick2ViewerWindow->engine()->addImageProvider(DEFAULT_IMAGEBUFFER_NAME, new QML2ModelIndexProvider(*imgLstModel));//Qt::DisplayRole
	}	
	QString extPluginPath = MainAppInfo::qmlExtensionsPluginDirPath();
	QStringList importPaths = quick2ViewerWindow->engine()->importPathList();
	if (!importPaths.contains(extPluginPath))
	{
		quick2ViewerWindow->engine()->addImportPath(extPluginPath);
		qDebug() << __FUNCTION__ << "::Added the QML2 extension Plugin path (" << extPluginPath << ").";
	}

	QStringList importIncludePaths;
	QWidget *tmpWidget = MainAppInfo::getMainWindow();
	if(QMetaObject::invokeMethod(tmpWidget, MainAppInfo::getMainWindowScriptIncludePathsName().toLatin1(), Qt::DirectConnection, Q_RETURN_ARG(QStringList, importIncludePaths)))
	{
		foreach(QString tmpString,importIncludePaths)
		{
			quick2ViewerWindow->engine()->addImportPath(tmpString);
			qDebug() << __FUNCTION__ << "::Added the BrainStim script Include path (" << tmpString << ").";
		}
	}
	//quick2ViewerWindow->registerDefaultCustomQMLTypes();//removed to be compatible with QtCreator design, everything moved to the DefaultQMLPlugin
	//qmlErrorHandler = new QmlErrorHandler(*qmlViewer,this);
	parseExperimentObjectBlockParameters(true,false);
	setExperimentMetaObject();
	return true;
}

bool QML2Viewer::eventFilter(QObject *target, QEvent *event)
{
	if (target == quick2ViewerWindow)
	{
		QEvent::Type tmpEvent = event->type();
		if (tmpEvent == QEvent::KeyPress) 
		{
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

			switch (keyEvent->key())
			{
			case Qt::Key_A:	//To abort the experiment
				if((keyEvent->modifiers() & Qt::ControlModifier))// && (keyEvent->modifiers() & Qt::AltModifier))
				{
					emit ExperimentEngine::UserWantsToClose();
				}	
				break;
			case Qt::Key_T:
				if((keyEvent->modifiers() & Qt::ControlModifier))// && (keyEvent->modifiers() & Qt::AltModifier))
				{
					ExperimentEngine::incrementExternalTrigger();
				}	
				break;
			case Qt::Key_Alt:	//To start the experiment
				if(experimentManager)//otherwise we'll get problems when only a *.qml file is executed
					setExperimentObjectReadyToUnlock();				
				break;
			}
		}
		else if(tmpEvent == QEvent::Close)
		{
			onQuick2ViewWindowClosed();
		}
	}
	return false;
}

bool QML2Viewer::setExperimentObjectReadyToUnlock()
{
	bool bResult = ExperimentEngine::setExperimentObjectReadyToUnlock();
	qml2EventRoutine();
	return bResult;
}

QScriptValue QML2Viewer::getWindow()
{
	QWindow *tmpWindow = qobject_cast<QWindow*>(quick2ViewerWindow);
	QScriptEngine *currScriptEngine = getScriptEngine();
	if(currScriptEngine)
		return currScriptEngine->newQObject(tmpWindow);
	return NULL;
}

QScreen* QML2Viewer::getActiveStimuliOutputScreen()
{
	return sConfiguredActiveScreen;
}

bool QML2Viewer::setActiveStimuliOutputScreen(int nScreenNumber)
{
	if (nScreenNumber >= 0)
	{
		if (nScreenNumber < QGuiApplication::screens().count())
		{
			sConfiguredActiveScreen = QGuiApplication::screens().at(nScreenNumber);
			if (sConfiguredActiveScreen)
				rectScreenRes = sConfiguredActiveScreen->geometry();
			return true;
		}
	}
	return false;
}

void QML2Viewer::setWindowMode(const bool &bIsFullScreen, const bool &bIsFrameless)
{
	bFullScreenMode = bIsFullScreen;
	bIsFramelessWindow = bIsFrameless;
}

void QML2Viewer::setTopLeftWindowPosition(const int &nXposition, const int &nYposition)
{
	pointWindowStartPosition.setX(nXposition);
	pointWindowStartPosition.setY(nYposition);
}

bool QML2Viewer::startObject()
{	
	int nSetScreenNumber = 0;
	QPoint tmpPointOffsetScreen(0, 0);
	if(sConfiguredActiveScreen == NULL)
	{
		sConfiguredActiveScreen = quick2ViewerWindow->grabScreenUnderMouseCursor();
		if (sConfiguredActiveScreen)
		{
			rectScreenRes = sConfiguredActiveScreen->geometry();
			quick2ViewerWindow->setScreen(sConfiguredActiveScreen);
			tmpPointOffsetScreen = rectScreenRes.topLeft().toPoint();
		}
	}
	else
	{
		quick2ViewerWindow->setScreen(sConfiguredActiveScreen);
		nSetScreenNumber = QGuiApplication::screens().indexOf(sConfiguredActiveScreen);
	}	

	if (bFullScreenMode)
	{
		quick2ViewerWindow->setResizeMode(QQuickView::SizeRootObjectToView);
		quick2ViewerWindow->showFullScreen();
	}
	else
	{
		if (bIsFramelessWindow == false)
			quick2ViewerWindow->setFlags(Qt::Window);
		if (nSetScreenNumber > 0)
		{
			QRect tmpScreenRect = sConfiguredActiveScreen->geometry();
			tmpPointOffsetScreen = tmpScreenRect.topLeft();
		}
		quick2ViewerWindow->setPosition(pointWindowStartPosition + tmpPointOffsetScreen);
		quick2ViewerWindow->setResizeMode(QQuickView::SizeViewToRootObject);
		quick2ViewerWindow->show();
	}
	return true;
}

void QML2Viewer::onStartTriggerRecieved()
{	
	if(bExperimentUnlocked == false)
	{
		bExperimentUnlocked = true;
		qml2EventRoutine();
	}
}

bool QML2Viewer::stopObject()
{
	if(currentExperimentStructure)
		disconnect(currentExperimentStructure,&cExperimentStructure::experimentStarted,this, &QML2Viewer::onStartTriggerRecieved);
	return true;
}

bool QML2Viewer::setObjectID(int nObjID)
{
	nQML2ViewerID = nObjID;//or use getObjectID();//Because we need it often we'll buffer it here
	return true;
}

bool QML2Viewer::initObjectBlockTrial()
{
	parseExperimentObjectBlockParameters(false);
	return true;
}

/*
QVariant QML2Viewer::invokeQml2JavaScriptCode(const QString &sCode)//"functionsID.set3DMeshAnimation('',10,-10,3800);");
{
	QQmlEngine *tmpEngine = quick2ViewerWindow->engine();
	QQmlContext *tmpRootContext = tmpEngine->rootContext();
	////QQmlContext *tmpRootContext = new QQmlContext(tmpEngine->rootContext());
	//QObject *tmpQObject = tmpRootContext.contextObject();
	tmpEngine = tmpRootContext->engine();
	//QVariant tmpVar = tmpRootContext->contextProperty("id");//"width");
	////void QQmlContext::setContextProperty(const QString & name, const QVariant & value)
	QJSValue jsValue = tmpEngine->globalObject();
	QString sError = jsValue.toString();
	jsValue = jsValue.property("width");//width);
	sError = jsValue.toString();
	if(tmpEngine)
	{
		QJSValue jsValue = tmpEngine->evaluate(sCode);//"functionsID.set3DMeshAnimation('',10,-10,3800);");
		if(jsValue.isError())
		{
			QString sError = jsValue.toString();
			qDebug() << __FUNCTION__ << sError;
				return NULL;
		}
		return jsValue.toVariant();
	}
	return NULL;
}
*/

QVariant QML2Viewer::invokeQml2Method(QString strRootObjectName, QString strMethodName, QVariant inputValue1, QVariant inputValue2, QVariant inputValue3, QVariant inputValue4, QVariant inputValue5, QVariant inputValue6, QVariant inputValue7, QVariant inputValue8, QVariant inputValue9)
{
	if (!rootObject)
	{
		rootObject = quick2ViewerWindow->rootObject();// get root object
		if (!rootObject)
		{
			qDebug() << __FUNCTION__ << "::Failed to retrieve the root QML object.";
			return NULL;
		}
	}
	QObject *methodsContainerObject;
	if (strRootObjectName.isEmpty())
	{
		methodsContainerObject = rootObject;
	}
	else
	{
		//QString sName = rootObject->objectName();
		//sName = QQmlProperty::read(rootObject, "someNumber").toString();
		//sName = QQmlProperty::read(rootObject, "objectName").toString();
		//int nrChildren = rootObject->children().count();
		//for (int i=0;i<nrChildren;i++)
		//{
		//	sName = rootObject->children().at(i)->objectName();
		//}		
		methodsContainerObject = rootObject->findChild<QObject *>(strRootObjectName);
	}
	if (methodsContainerObject)  // element found
	{
		QVariant returnedValue;
		bool bInvokeSucceeded = false;

		if ((inputValue1.type() == 0) && (inputValue1.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue));	
		} 
		else if ((inputValue2.type() == 0) && (inputValue2.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1));
		}
		else if ((inputValue3.type() == 0) && (inputValue3.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2));
		}
		else if ((inputValue4.type() == 0) && (inputValue4.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2), Q_ARG(QVariant, inputValue3));
		}
		else if ((inputValue5.type() == 0) && (inputValue5.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2), Q_ARG(QVariant, inputValue3), Q_ARG(QVariant, inputValue4));
		}
		else if ((inputValue6.type() == 0) && (inputValue6.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2), Q_ARG(QVariant, inputValue3), Q_ARG(QVariant, inputValue4), Q_ARG(QVariant, inputValue5));
		}
		else if ((inputValue7.type() == 0) && (inputValue7.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2), Q_ARG(QVariant, inputValue3), Q_ARG(QVariant, inputValue4), Q_ARG(QVariant, inputValue5), Q_ARG(QVariant, inputValue6));
		}
		else if ((inputValue8.type() == 0) && (inputValue8.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2), Q_ARG(QVariant, inputValue3), Q_ARG(QVariant, inputValue4), Q_ARG(QVariant, inputValue5), Q_ARG(QVariant, inputValue6), Q_ARG(QVariant, inputValue7));
		}
		else if ((inputValue9.type() == 0) && (inputValue9.value<int>() == NULL))
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2), Q_ARG(QVariant, inputValue3), Q_ARG(QVariant, inputValue4), Q_ARG(QVariant, inputValue5), Q_ARG(QVariant, inputValue6), Q_ARG(QVariant, inputValue7), Q_ARG(QVariant, inputValue8));
		}
		else
		{
			bInvokeSucceeded = QMetaObject::invokeMethod(methodsContainerObject, strMethodName.toLatin1(), Q_RETURN_ARG(QVariant, returnedValue), Q_ARG(QVariant, inputValue1), Q_ARG(QVariant, inputValue2), Q_ARG(QVariant, inputValue3), Q_ARG(QVariant, inputValue4), Q_ARG(QVariant, inputValue5), Q_ARG(QVariant, inputValue6), Q_ARG(QVariant, inputValue7), Q_ARG(QVariant, inputValue8), Q_ARG(QVariant, inputValue9));
		}

		if(bInvokeSucceeded)
		{
			//QString ss = returnedValue.type();
			//if (returnedValue.type() == QMetaType::QVariant)
			//{
			//}
			//QString sRetVal = returnedValue.toString();
			//qDebug() << "QML method returned:" << sRetVal;
			return returnedValue;// QScriptValue();//sRetVal);
		}
	}
	qDebug() << __FUNCTION__ << "::Failed to invoke the QML method(" << strMethodName << ")";
	return NULL;
}

void QML2Viewer::onQuick2ViewWindowClosed()
{
	emit ExperimentEngine::UserWantsToClose();
}

void QML2Viewer::onStatusChanged(QQuickView::Status status)
{
	if (status == QQuickView::Error)
	{
		QList<QQmlError> errList;
		errList = quick2ViewerWindow->errors();
		for (int i=0;i<errList.count();i++)
		{
			QString errMessage = "... QML error at(col " + QString::number(errList.at(i).column()) + ", line " + QString::number(errList.at(i).line()) + "): " + errList.at(i).description() + " ...";
			if(experimentManager)
				experimentManager->sendToMainAppLogOutput(errMessage);
			else
				QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, errMessage));
		}
	}
}

void QML2Viewer::parseExperimentObjectBlockParameters(bool bInit, bool bSetOnlyToDefault)
{
	if (bInit)
	{	
		colorBackground = QColor(87,87,87);//gives "#575757";
		if(!bSetOnlyToDefault)
			insertExpObjectParameter(nQML2ViewerID,QML2VIEWER_BACKGROUNDCOLOR,colorBackground);
		//qmlMainFilePath = "";
		if(!bSetOnlyToDefault)
			insertExpObjectParameter(nQML2ViewerID,QML2VIEWER_MAINFILEPATH,qmlMainFilePath);
		stimHeightPixelAmount = 480;
		if(!bSetOnlyToDefault)
			insertExpObjectParameter(nQML2ViewerID,QML2VIEWER_HEIGHT_PIXEL_AMOUNT,stimHeightPixelAmount);
		stimWidthPixelAmount = stimHeightPixelAmount;
		if(!bSetOnlyToDefault)
			insertExpObjectParameter(nQML2ViewerID,QML2VIEWER_WIDTH_PIXEL_AMOUNT,stimWidthPixelAmount);
	} 
	else
	{
		ParsedParameterDefinition pParDef;//((pParDef.bIsInitialized) && (pParDef.bHasChanged == false))
		bParameterChanged = false;
		pParDef = getExpObjectBlockParameter(nQML2ViewerID,QML2VIEWER_HEIGHT_PIXEL_AMOUNT,QString::number(stimHeightPixelAmount));
		if (pParDef.bHasChanged)
		{
			stimHeightPixelAmount = pParDef.sValue.toInt();
			bParameterChanged = true;
		}
		pParDef = getExpObjectBlockParameter(nQML2ViewerID,QML2VIEWER_WIDTH_PIXEL_AMOUNT,QString::number(stimWidthPixelAmount));
		if (pParDef.bHasChanged)
		{
			stimWidthPixelAmount = pParDef.sValue.toInt();
			bParameterChanged = true;
		}
		//if (bResolutionChanged)
		//setStimuliResolution(stimWidthPixelAmount,stimHeightPixelAmount);

		pParDef = getExpObjectBlockParameter(nQML2ViewerID,QML2VIEWER_BACKGROUNDCOLOR,colorBackground.name());
		if (pParDef.bHasChanged)
		{
			colorBackground = QColor(pParDef.sValue.toLower());
			bParameterChanged = true;
			//GlWidgetPallette = glWidget->palette();
			//GlWidgetPallette.setColor(glWidget->backgroundRole(), colorBackground);
			//glWidget->setPalette(GlWidgetPallette);
		}
		pParDef = getExpObjectBlockParameter(nQML2ViewerID,QML2VIEWER_MAINFILEPATH,qmlMainFilePath);
		if (pParDef.bHasChanged)
		{
			qmlMainFilePath = pParDef.sValue;
			bParameterChanged = true;
			qml2EventRoutine();
		}			
	}
}

bool QML2Viewer::qml2EventRoutine(QString strContent)
{
	QString fileString;
	bool bEmitSourceChange = false;
	QUrl fileUrl;

	if(strContent.isEmpty())
	{
		if((experimentManager == NULL) && (currentExperimentStructure == NULL))//Here we should just run the QML file
		{
			bExperimentUnlocked = true;
			if(last_qmlMainFilePath == qmlMainFilePath)
			{
				if(qmlMainFilePath.isEmpty())
				{
					QString errMessage = __FUNCTION__ + QString(":Empty File Path, no valid QML file to run!");
					QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, errMessage));
					this->abortExperimentObject();//this seems to work...
					return false;
				}
				return true;
			}
			else
			{
				last_qmlMainFilePath = qmlMainFilePath;
			}
			bEmitSourceChange = true;
			fileString = QFileInfo(qmlMainFilePath).canonicalFilePath();
			fileUrl = QUrl::fromLocalFile(fileString);//fileString);
			QDir::setCurrent(QFileInfo(fileString).canonicalPath());
		}
		else if(bFirstQuickWindowActivation)
		{
			bFirstQuickWindowActivation = false;
			bExperimentUnlocked = false;
			fileString = "qrc:/resources/startqmlfile.qml";
			fileUrl = QUrl(fileString);
		}
		else if(bExperimentUnlocked == false)
		{
			fileString = "qrc:/resources/unlockedqmlfile.qml";
			fileUrl = QUrl(fileString);
		}
		else
		{
			if(last_qmlMainFilePath == qmlMainFilePath)
			{
				if(qmlMainFilePath.isEmpty())
				{
					QString errMessage = __FUNCTION__ + QString(":Empty File Path, no valid QML file to run!");
					QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, errMessage));
					this->abortExperimentObject();//this seems to work...
					return false;
				}
				return true;
			}
			else
			{
				last_qmlMainFilePath = qmlMainFilePath;
			}
			bEmitSourceChange = true;
			fileString = QFileInfo(qmlMainFilePath).canonicalFilePath();
			fileUrl = QUrl::fromLocalFile(fileString);
		}
	}
	else   //if(strContent.isEmpty()==false)	
	{
		int nRetries = 1;
		QString filename("tmp");
		tmpFile.setFileName(filename + QString::number(nRetries) + ".txt");
		while (!tmpFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			nRetries++;
			tmpFile.setFileName(filename + QString::number(nRetries) + ".txt");
			if (nRetries == 10)
			{
				QString errMessage = __FUNCTION__ + QString(": Could not create temporarily file.");
				QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, errMessage));
				this->abortExperimentObject();//this seems to work...
				return false;
			}
		}
		tmpFile.write(strContent.toLatin1());
		tmpFile.close();
		fileString = QFileInfo(tmpFile.fileName()).canonicalFilePath();

		//return;
		fileUrl = QUrl::fromLocalFile(fileString);
		//fileString = qmlMainFilePath;
	}
	if(quick2ViewerWindow)
	{
		//if (quick2ViewerWindow->source() != fileUrl)
		//{
			//if(fileUrl == QUrl(""))
			//	quick2ViewerWindow->setMainQmlFile(fileString);
			//else
			//quick2ViewerWindow->hide();
			//quick2ViewerWindow->setVisible(false);
			quick2ViewerWindow->setSource(fileUrl);

			if (quick2ViewerWindow->status() == QQuickView::Error)
			{
				this->abortExperimentObject();
				if(experimentManager)
					experimentManager->abortExperiment();
				return false;
			}
			if(bEmitSourceChange)
				emit NewSourceLoaded(fileUrl.toString());
			return true;
			//qApp->processEvents();
			//quick2ViewerWindow->setVisible(true);
			//quick2ViewerWindow->resize(600,600);

			//quick2ViewerWindow->focusObject();
			//quick2ViewerWindow->setWindowState(Qt::WindowActive);
			//quick2ViewerWindow->raise();
			//quick2ViewerWindow->showFullScreen();
		//}
	}
	return false;
}

bool QML2Viewer::setExperimentManager(ExperimentManager *expManager)
{
	if(experimentManager!=expManager)
	{
		experimentManager = expManager;
		if(expManager)
		{
			if (sConfiguredActiveScreen == NULL)
			{
				sConfiguredActiveScreen = expManager->getActiveStimuliOutputScreen();
				if (sConfiguredActiveScreen)
					rectScreenRes = sConfiguredActiveScreen->geometry();
			}
		}
		ExperimentEngine::setExperimentManager(expManager);//Important!
	}
	return true;
}

QString QML2Viewer::addPixmapToImageBuffer(const QPixmap &pixmap)
{
	if (!imgLstModel)
		return false;
	return imgLstModel->addPixmap(pixmap);
}

bool QML2Viewer::getPixmapFromImageBuffer(QPixmap *pixmap, const QString &ID)
{
	if (!imgLstModel)
		return false;
	if(imgLstModel->getPixmap(*pixmap,ID))
	{
		return true;
	}
	return false;
}

bool QML2Viewer::updatePixmapFromImageBuffer(QPixmap *pixmap, const QString &ID)
{
	if (!imgLstModel)
		return false;
	if(imgLstModel->updatePixmap(*pixmap,ID))
	{
		return true;
	}
	return false;
}

bool QML2Viewer::updatePixmapIDFromImageBuffer(const QString &oldID, const QString &newID)
{
	if (!imgLstModel)
		return false;
	if(imgLstModel->updatePixmapID(oldID,newID))
	{
		return true;
	}
	return false;
}

bool QML2Viewer::removePixmapFromImageBuffer(const QString &ID)
{
	if (!imgLstModel)
		return false;
	if(imgLstModel->removePixmap(ID))
	{
		return true;
	}
	return false;
}