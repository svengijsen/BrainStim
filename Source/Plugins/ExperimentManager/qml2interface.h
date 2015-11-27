#ifndef QML2INTERFACE_H
#define QML2INTERFACE_H

#include <QObject>
#include <QQmlEngine>
#include "./../../BrainStim/mainappinfo.h"

class QML2Viewer;

//!  The Qml2Interface class. 
/*!
The Qml2Interface can used as a interface from within a QML file to use BrainStim functionality and access the current active script context.
*/
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
	
	/*! \brief Executes some custom provided script code inside the current active script context.
	*
	*  This function invokes the script engine to execute some custom provided script code inside the current active script context.
	*  @param sScriptCode a string containing the script code to execute.
	*/
	Q_INVOKABLE QVariant invokeScriptEngine(const QString &sScriptCode);
	/*! \brief Returns the file path of the QML file.
	*
	*  This function returns the file path of the QML file.
	*/
	Q_INVOKABLE QString getQMLFilePath();
	/*! \brief Returns the current working directory of the QML file.
	*
	*  This function returns the current working directory of the QML file.
	*/
	Q_INVOKABLE QString getCurrentDirectory();
	/*! \brief Writes a message to the default BrainStim Output Log Window.
	*
	*  This function writes a message to the default BrainStim Output Log Window.
	*  @param sMessage a string containing the message.
	*/
	Q_INVOKABLE void writeToOutputWindow(const QString &sMessage);

private:

	static QML2Viewer *parentQMLViewerObject;
	//QString m_message;
};

// Second, define the singleton type provider function (callback).
static QObject *qml2interface_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)
	Qml2Interface *objQml2Interface = new Qml2Interface(NULL);
	return objQml2Interface;
}

#endif //QML2INTERFACE_H