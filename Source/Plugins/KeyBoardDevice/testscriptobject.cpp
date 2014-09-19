#include "testscriptobject.h"

testScriptObject::testScriptObject(  QScriptEngine *pEngine, QObject *parent) 
	: QObject(parent),m_pEngine(pEngine)
{
	// create this object in the scripting land:
	m_thisObject = m_pEngine->newQObject(this);

}

testScriptObject::~testScriptObject()
{

}

void testScriptObject::slot1()
{

}

void testScriptObject::displayMsg(QString strMsg)
{

}

void testScriptObject::runScript(const QString &strScriptName)
{

	//c:\Qt\4.5.0\examples\script\customclass\Resources\testScriptObject.js
	QFile file(strScriptName);

	if (! file.exists())
	{
		QMessageBox::critical(0, "Error", "Could not find program file!");
		return;
	}

	if (! file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::critical(0, "Error", "Could not open program file!");
		return;
	}

	QString strProgram = file.readAll();

	// do static check so far of code:
	if (! m_pEngine->canEvaluate(strProgram) )
	{
		QMessageBox::critical(0, "Error", "canEvaluate returned false!");
		return;
	}

	// actually do the eval:
	m_pEngine->evaluate(strProgram);

	// uncaught exception?
	if (m_pEngine->hasUncaughtException())
	{
		QScriptValue exception = m_pEngine->uncaughtException();
		QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception: ") + exception.toString());
		return;
	}

	QScriptValue createFunc = m_pEngine->evaluate("create");

	if (m_pEngine->hasUncaughtException())
	{
		QScriptValue exception = m_pEngine->uncaughtException();
		QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
		return;
	}

	if (!createFunc.isFunction())
	{
		QMessageBox::critical(0, "Script Error", "createFunc is not a function!");
	}

	createFunc.call(m_thisObject);

	if (m_pEngine->hasUncaughtException())
	{
		QScriptValue exception = m_pEngine->uncaughtException();
		QMessageBox::critical(0, "Script error", QString("Script threw an uncaught exception while looking for create func: ") + exception.toString());
		return;
	}
	// now emit our test signal:
	emit signal1();
}