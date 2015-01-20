#ifndef QML2INTERFACE_H
#define QML2INTERFACE_H

#include <QObject>
#include <QQmlEngine>
#include "./../../BrainStim/mainappinfo.h"

class QML2Viewer;
// First, define your QObject which provides the functionality.
class Qml2Interface : public QObject
{
	Q_OBJECT
	//Q_PROPERTY(QString message READ message WRITE setmessage NOTIFY messageChanged)
	//Q_PROPERTY(QString RootDirectory READ RootDirectory)

//signals:
	//void messageChanged();

public:
	Qml2Interface(QObject *parent);
	~Qml2Interface();

	void setQMLViewerObject(QML2Viewer *qml2ViewerObject);
	
	//void setmessage(const QString &a); 
	//QString message() const;
	//QString RootDirectory() const;
	
	Q_INVOKABLE QVariant invokeScriptEngine(const QString &sScriptCode);
	Q_INVOKABLE QString getQMLFileName();
	Q_INVOKABLE QString getCurrentDirectory();
	Q_INVOKABLE void writeToOutputWindow(const QString &sMessage);

private:

	static QML2Viewer *parentQMLViewerObject;
	//QString m_message;
};

// Second, define the singleton type provider function (callback).
static QObject *interface_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)
	Qml2Interface *example = new Qml2Interface(NULL);
	return example;
}

#endif //QML2INTERFACE_H