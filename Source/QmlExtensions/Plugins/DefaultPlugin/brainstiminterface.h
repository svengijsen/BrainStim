#ifndef BRAINSTIMINTERFACE_H
#define BRAINSTIMINTERFACE_H

#include <QObject>
#include <QQmlEngine>
#include "./../../../BrainStim/mainappinfo.h"

class QML2Viewer;

//!  The BrainStimInterface class. 
/*!
The BrainStimInterface can used as a interface from within a QML file to use BrainStim functionality and access the current active script context.
*/
class BrainStimInterface : public QObject
{
	Q_OBJECT
	//Q_PROPERTY(QString message READ message WRITE setmessage NOTIFY messageChanged)
	//Q_PROPERTY(QString RootDirectory READ RootDirectory)

//signals:
	//void messageChanged();

public:
	BrainStimInterface();
	BrainStimInterface(QObject *parent);

	// Don't forget to declare these two. You want to make sure they
	// are unacceptable otherwise you may accidentally get copies of
	// your singleton appearing.
	BrainStimInterface(BrainStimInterface const&);	//Don't Implement
	void operator=(BrainStimInterface const&);		//Don't implement

	~BrainStimInterface();

	void setMainApplicationWidget(QWidget *mainAppWidget);
	void setQMLViewerObject(QML2Viewer *qml2ViewerObject);
	static BrainStimInterface* getInstance()
	{
		//static BrainStimInterface *bsInterface = new BrainStimInterface(NULL); // Guaranteed to be destroyed and Instantiated on first use.
		if(bsInterface==NULL)
			bsInterface = new BrainStimInterface(NULL);
		return bsInterface;
	}
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
	static QWidget *parentAppWidget;
	static BrainStimInterface *bsInterface;
	//QString m_message;
};

// Second, define the singleton type provider function (callback).
static QObject *interface_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(engine)
	Q_UNUSED(scriptEngine)
	return BrainStimInterface::getInstance();
}

#endif //BRAINSTIMINTERFACE_H