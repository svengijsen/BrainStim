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


#include "qtscriptworker.h"

static QScriptValue importExtension(QScriptContext *ctx, QScriptEngine *eng)
{
	return eng->importExtension(ctx->argument(0).toString());
}

QTScriptWorker::QTScriptWorker(QObject *parent)
	: QThread(parent)
{
	//debugger.attachTo(this);
	//debugger.action(QScriptEngineDebugger::InterruptAction)->trigger();
	eng = new QScriptEngine(this);
	//attachedToDebugger = false;

#if QT_VERSION >= 0x040500
	debugger = new QScriptEngineDebugger();
	//debugger->attachTo(eng);
#endif

	//QScriptValue globalObject = eng->globalObject();
	//globalObject.setProperty("App", eng->newQObject(parent));
	QScriptValue qScriptVal = eng->newObject();
	qScriptVal.setProperty("importExtension", eng->newFunction(importExtension));
	//globalObject.property("qt").setProperty("script", qScriptVal);

	agent = new QTScriptEngineAgent(eng);
	eng->setAgent(agent);	
	connect(agent, SIGNAL(ScriptUnloaded(qint64)), parent, SLOT(scriptHasUnloaded(qint64)));
	connect(agent, SIGNAL(ScriptLoaded(qint64)), parent, SLOT(scriptHasLoaded(qint64)));
}

QTScriptWorker::~QTScriptWorker()
{

}

void QTScriptWorker::setScript(const QString &script)
{
	currentScript= script;
}

void QTScriptWorker::run()
{
	if (!(currentScript == ""))
	{
		//eng->evaluate(currentScript);
		//while (true)
		//{
		//	
		//}
	}
	// this method is called by the newly created thread...
	//while ( TRUE ) {
		// lock the application mutex, and set the caption of
		// the window to indicate that we are waiting to
		// start working
		//qApp->lock());
		//setCaption( "Waiting" );
		//qApp->unlock();

		// wait until we are told to continue
		//mycond.wait();

		// if we get here, we have been woken by another
		// thread... let's set the caption to indicate
		// that we are working
		//qApp->lock();
		//setCaption( "Working!" );
		//qApp->unlock();

		// this could take a few seconds, minutes, hours, etc.
		// since it is in a separate thread from the GUI thread
		// the gui will not stop processing events...
	//	do_complicated_thing();
	//}
}


//#include <qapplication.h>
//#include <qpushbutton.h>

// global condition variable
//QWaitCondition mycond;

// Worker class implementation
//class Worker : public QPushButton, public QThread
//{
//	Q_OBJECT

//public:
//	Worker(QWidget *parent = 0, const char *name = 0)
//		: QPushButton(parent, name)
//	{
//		setText("Start Working");
//
//		// connect the clicked() signal inherited from QPushButton to our
//		// slotClicked() method
//		connect(this, SIGNAL(clicked()), SLOT(slotClicked()));
//
//		// call the start() method inherited from QThread... this starts
//		// execution of the thread immediately
//		QThread::start();
//	}
//
//	public slots:
//		void slotClicked()
//		{
//			// wake up one thread waiting on this condition variable
//			mycond.wakeOne();
//		}


//};


bool QTScriptWorker::ImportScriptExtensions()//QDir extensionsDir)
{
	QStringList extensions;
	extensions = MainAppInfo::getQTScriptBindingList();
	QStringList failExtensions;
	foreach (const QString &ext, extensions) {
		QScriptValue ret = eng->importExtension(ext);
		if (ret.isError())
			failExtensions.append(ext);
	}
	if (!failExtensions.isEmpty()) {
		if (failExtensions.size() == extensions.size()) {
			qWarning("Failed to import Qt bindings!\n"
				"Plugins directory searched: %s/script\n"
				"Make sure that the bindings have been built, "
				"and that this executable and the plugins are "
				"using compatible Qt libraries.");//, qPrintable(extensionsDir.absolutePath()));
		} else {
			qWarning("Failed to import some Qt bindings: %s\n"
				"Plugins directory searched: %s/script\n"
				"Make sure that the bindings have been built, "
				"and that this executable and the plugins are "
				"using compatible Qt libraries.");//, qPrintable(failExtensions.join(", ")), qPrintable(extensionsDir.absolutePath()));
		}
	}
	else
	{
		return true;
	}
	return false;
}