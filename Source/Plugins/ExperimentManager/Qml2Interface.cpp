#include "Qml2Interface.h"
#include <QMetaObject>

Qml2Interface::Qml2Interface(QObject *parent) : QObject(parent), m_message("")
{

}

Qml2Interface::~Qml2Interface()
{

}

void Qml2Interface::setmessage(const QString &a) 
{
	if (a != m_message) 
	{
		m_message = a;
		emit messageChanged();
	}
}

QString Qml2Interface::message() const 
{
	return m_message;
}

QString Qml2Interface::RootDirectory() const 
{
	return MainAppInfo::appDirPath();
}

QString Qml2Interface::doSomething()
{ 
	setmessage("something"); 
	return m_message; 
}

void Qml2Interface::Log(const QString &sMessage)
{
	QMetaObject::invokeMethod((QObject*)MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, sMessage));
}
