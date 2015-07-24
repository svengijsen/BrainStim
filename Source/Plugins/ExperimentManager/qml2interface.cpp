#include "qml2interface.h"
#include <QMetaObject>
#include "qml2viewer.h"

QML2Viewer *Qml2Interface::parentQMLViewerObject = NULL;//Needed to initialize the static variable!

Qml2Interface::Qml2Interface(QObject *parent) : QObject(parent)//, m_message("")
{

}

Qml2Interface::~Qml2Interface()
{

}

void Qml2Interface::setQMLViewerObject(QML2Viewer *qml2ViewerObject)
{
	parentQMLViewerObject = qml2ViewerObject;
}

//void Qml2Interface::setmessage(const QString &a) 
//{
//	if (a != m_message) 
//	{
//		m_message = a;
//		emit messageChanged();
//	}
//}

//QString Qml2Interface::message() const 
//{
//	return m_message;
//}

//QString Qml2Interface::RootDirectory() const 
//{
//	return MainAppInfo::appDirPath();
//}

//QString Qml2Interface::doSomething()
//{ 
//	setmessage("something"); 
//	return m_message; 
//}

QVariant Qml2Interface::invokeScriptEngine(const QString &sScriptCode)
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

QString Qml2Interface::getQMLFilePath()
{
	if (parentQMLViewerObject)
	{
		return parentQMLViewerObject->getCurrentFilePath();
	}
	return "";
}

QString Qml2Interface::getCurrentDirectory()
{
	return QDir::currentPath();
	//QString tmpFileName = getQMLFileName();
	//if (tmpFileName.isEmpty() == false)
	//{
	//	return QFileInfo(tmpFileName).canonicalPath();
	//}
	//return "";
}

void Qml2Interface::writeToOutputWindow(const QString &sMessage)
{
	QMetaObject::invokeMethod((QObject*)MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, sMessage));

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
