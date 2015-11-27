#include "brainstiminterface.h"
#include <QMetaObject>
#include ".\..\Plugins\ExperimentManager\qml2viewer.h"

QML2Viewer *BrainStimInterface::parentQMLViewerObject = NULL;	//Needed to initialize the static variable!
QWidget *BrainStimInterface::parentAppWidget = NULL;			//Needed to initialize the static variable!
BrainStimInterface* BrainStimInterface::bsInterface = NULL;		//Needed to initialize the static variable!

BrainStimInterface::BrainStimInterface()
{
}

BrainStimInterface::BrainStimInterface(QObject *parent) : QObject(parent)
{
}

BrainStimInterface::~BrainStimInterface()
{
	BrainStimInterface::bsInterface->deleteLater();
	BrainStimInterface::bsInterface = NULL;
}

void BrainStimInterface::setQMLViewerObject(QML2Viewer *qml2ViewerObject)
{
	parentQMLViewerObject = qml2ViewerObject;
}

void BrainStimInterface::setMainApplicationWidget(QWidget *mainAppWidget)
{
	parentAppWidget = mainAppWidget;
}

QVariant BrainStimInterface::invokeScriptEngine(const QString &sScriptCode)
{
	if (parentQMLViewerObject)
	{
		QScriptEngine *tmpScriptEngine = parentQMLViewerObject->getScriptEngine();
		if (tmpScriptEngine)
		{
			QScriptValue tmpValue = tmpScriptEngine->evaluate(sScriptCode);
			return tmpValue.toVariant();
		}
	}
	return QVariant::Invalid;
}

QString BrainStimInterface::getQMLFilePath()
{
	if (parentQMLViewerObject)
		return parentQMLViewerObject->getCurrentFilePath();
	return "";
}

QString BrainStimInterface::getCurrentDirectory()
{
	return QDir::currentPath();
}

void BrainStimInterface::writeToOutputWindow(const QString &sMessage)
{
	if (parentAppWidget)
		QMetaObject::invokeMethod((QObject*)parentAppWidget, MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, sMessage));


	//if (parentQMLViewerObject)
	//{
	//	QScriptEngine *tmpScriptEngine = parentQMLViewerObject->getScriptEngine();
	//	if (tmpScriptEngine)
	//	{
	//		tmpScriptEngine->evaluate("Log(\"aaa\");");
	//	}

	//	ExperimentManager *tmpExpManager = parentQMLViewerObject->getExperimentManager();
	//	if (tmpExpManager)
	//	{
	//		QString sCurrentFilePath = tmpExpManager->getExperimentFileName();
	//	QMetaObject::invokeMethod((QObject*)MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, parentQMLViewerObject->));
	//	}
	//}
}






//void BrainStimInterface::setmessage(const QString &a) 
//{
//	if (a != m_message) 
//	{
//		m_message = a;
//		emit messageChanged();
//	}
//}

//QString BrainStimInterface::message() const 
//{
//	return m_message;
//}

//QString BrainStimInterface::RootDirectory() const 
//{
//	return MainAppInfo::appDirPath();
//}

//QString BrainStimInterface::doSomething()
//{ 
//	setmessage("something"); 
//	return m_message; 
//}