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

#include "mainappinfo.h"
#include <QTextStream>
#include <QString>
#include <QMetaEnum>
#include <QDebug>
#include <QMessageBox>

QFile *MainAppInfo::mainLogFile = NULL;//Needed to initialize the static variable!
QWidget *MainAppInfo::mainWindow = NULL;//Needed to initialize the static variable!
QString MainAppInfo::sAppUserPath = "";//Needed to initialize the static variable!
QList<int> MainAppInfo::lRegisteredMetaTypeIds = QList<int>();//Needed to initialize the static variable!
QHash<int, QObject *> MainAppInfo::hashRegisteredCustomPropertySettingObjects = QHash<int, QObject *>();//Needed to initialize the static variable!

QString MainAppInfo::outputsDirPath()
{
	QDir OutputsDir = appUserPath();
	if(QDir(appUserPath() + QString("/" MAIN_PROGRAM_OUTPUTS_DIRNAME "/")).exists()==false)
	{
		QDir().mkdir(QString(appUserPath()) + QString("/" MAIN_PROGRAM_OUTPUTS_DIRNAME "/"));
	}
	OutputsDir.cd(MAIN_PROGRAM_OUTPUTS_DIRNAME);
	//QString tmpString = OutputsDir.absolutePath();
	//return tmpString;
	return OutputsDir.absolutePath();
}

QStringList MainAppInfo::getQTScriptBindingList()	
{
#ifdef WIN32
	return	QStringList() << "qt.core" << "qt.gui" << "qt.uitools" << "qt.webkit" << "qt.xml" << "qt.xmlpatterns" << "qt.svg" << "qt.sql" << "qt.opengl" << "qt.network";
#else//64 bit
	return	QStringList() << "qt.core" << "qt.gui" << "qt.uitools" << "qt.webkit" << "qt.xml" << "qt.xmlpatterns" << "qt.svg" << "qt.sql" << "qt.opengl" << "qt.network";
#endif
}

QString MainAppInfo::apiDirPath()
{
	QDir pluginsDir = appDebugDirPath();
	pluginsDir.cd("apis");
	return pluginsDir.absolutePath();
}

void MainAppInfo::MyOutputHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) 
{
	bool bDoAbort = false;
	if(MainAppInfo::mainLogFile == NULL)
	{
		//QString aa = MainAppInfo::appLogFilePath();
		MainAppInfo::mainLogFile = new QFile(MainAppInfo::appLogFilePath());
	}
	if(!MainAppInfo::mainLogFile->isOpen())
	{
		qint64 nSize = MainAppInfo::mainLogFile->size();
		if(nSize > 41943040)//40 MegaByte
		{
			QMessageBox::StandardButton ret = QMessageBox::warning(NULL, "LogFile exceeds file size!",
				"The BrainStim LogFile(" + MainAppInfo::appLogFilePath() + ") exceeds the 40Mb file size.\n"
					"Do you want BrainStim to clear this LogFile for you?", QMessageBox::Yes | QMessageBox::No);
				if (ret == QMessageBox::Yes)
				{
					MainAppInfo::mainLogFile->open(QIODevice::WriteOnly);
				}
				else
				{
					MainAppInfo::mainLogFile->open(QIODevice::WriteOnly | QIODevice::Append);
				}
		}
		else
		{
			MainAppInfo::mainLogFile->open(QIODevice::WriteOnly | QIODevice::Append);
		}
		if(!MainAppInfo::mainLogFile->isOpen())
		{
            MainAppInfo::mainLogFile = NULL;
			return;
		}
	}
	QString strMessage;
	bool bSkipErrorForLogWindow = false;
	bSkipErrorForLogWindow = ( (msg == QString("Unknown error")) || (msg == QString("QGLShader::link: \"No errors.\" ")) );
	bSkipErrorForLogWindow = bSkipErrorForLogWindow || (msg.contains("Remove me: fixing toplevel window flags"));
	bSkipErrorForLogWindow = bSkipErrorForLogWindow || (msg.contains("warning X3206: implicit truncation of vector type"));
	bSkipErrorForLogWindow = bSkipErrorForLogWindow || (msg.contains("BitBlt failed (The handle is invalid.)"));
	bSkipErrorForLogWindow = bSkipErrorForLogWindow || (msg.contains("QWindowsNativeFileDialogBase::selectNameFilter: Invalid parameter 'Directories' not found in"));
	switch (type) 
	{
	case QtDebugMsg:
#ifdef QT_NO_DEBUG
		strMessage = QString("Debug: %1").arg(msg);
#else
		strMessage = QString("Debug: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
#endif
		break;
	case QtWarningMsg:
#ifdef QT_NO_DEBUG
		strMessage = QString("Warning: %1").arg(msg);
#else
		strMessage = QString("Warning: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
#endif
		break;
	case QtCriticalMsg:
		strMessage = QString("Critical: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
		break;
	case QtFatalMsg:
		strMessage = QString("Fatal: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
		bDoAbort = true;		
	}
	QTextStream ts(mainLogFile);
	ts << QDate::currentDate().toString().toLatin1().data() << QString("\t") << QTime::currentTime().toString().toLatin1().data() << QString("\t") << strMessage << QString("\n"); //endl macro doesn't work?
	if(MainAppInfo::getMainWindow())
	{
		if(bSkipErrorForLogWindow == false)
			QMetaObject::invokeMethod((QObject *)MainAppInfo::getMainWindow(), MainAppInfo::getMainWindowLogSlotName().toLatin1(), Qt::DirectConnection, Q_ARG(QString, strMessage));
	}	
	if (bDoAbort)
	{
		CloseMainLogFile();
		abort();
	}
}

void MainAppInfo::CloseMainLogFile()
{
	if(MainAppInfo::mainLogFile != NULL)
	{
		if(MainAppInfo::mainLogFile->isOpen())
		{
			MainAppInfo::mainLogFile->close();			
		}
	}	
}

bool MainAppInfo::getHexedOrderNumber(const int &nNumber, QString &sResult, const int &nDecimals)
{
	if (nNumber < 0)
		return false;
	sResult = QString("%1").arg(nNumber, nDecimals, 16, QChar('0'));
	//sResult = QString::number(nNumber).toLatin1().toHex();
	//while (sResult.length() < nDecimals)
	//	sResult = "0" + sResult;
	return true;
}

bool MainAppInfo::addRegisteredMetaTypeID(const int &nMetaTypeID)
{
	if(nMetaTypeID > 0)
	{
		if(MainAppInfo::lRegisteredMetaTypeIds.contains(nMetaTypeID) == false)
		{
			if(QMetaType::isRegistered(nMetaTypeID))
			{
				MainAppInfo::lRegisteredMetaTypeIds.append(nMetaTypeID);
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool MainAppInfo::setMainWindow(QWidget *mainWin)
{
	mainWindow = mainWin;
	return true;
}

bool MainAppInfo::isCompatibleVersion(const QString &strMinimalRequiredVersion, const QString &strCurrentVersion)
{
	QStringList lstCurrentVersion = strCurrentVersion.split(".");
	QStringList lstMinimalVersion = strMinimalRequiredVersion.split(".");
	if ((lstCurrentVersion.count() == 4) && (lstMinimalVersion.count() == 4))
	{
		for (int i = 0;i<4;i++)
		{
			if(lstCurrentVersion.at(i).toInt() > lstMinimalVersion.at(i).toInt())
			{
				return true;//Later version
			}
			else if(lstCurrentVersion.at(i).toInt() < lstMinimalVersion.at(i).toInt())
			{
				return false;//Earlier version
			}
		}
		return true;//same version
	}
	return false;//wrong arguments
}

bool MainAppInfo::CreateHashTableFromEnumeration(const QString &sTypeName, QHash<QString, int> &hTable, const QMetaObject metaObject)
{
	if(!sTypeName.isEmpty())
	{
		QStringList tmpList = sTypeName.split("::",QString::SkipEmptyParts);
		QString sShortTypeName = tmpList.at(tmpList.count()-1);
		int enumeratorIndex = metaObject.indexOfEnumerator(sShortTypeName.toLatin1());
		if (enumeratorIndex >= 0)
		{
			QMetaEnum metaEnum = metaObject.enumerator(enumeratorIndex);
			int nKeyCount = metaEnum.keyCount();
			for (int i=0;i<nKeyCount;i++)
			{
				hTable.insert(metaEnum.key(i),metaEnum.value(i));
			}
			if (hTable.count() > 0)
				return true;
			return false;
		}
		else
		{
			qDebug() << __FUNCTION__ << "::Unknown Enumeration Type (" << sShortTypeName << ")!";	
			return false;
		}
		return true;
	}
	return false;
}

bool MainAppInfo::registerCustomPropertySettingObject(QObject* objCustomPropSettObj, const int &nVariantType)
{
	if ((objCustomPropSettObj) && (MainAppInfo::hashRegisteredCustomPropertySettingObjects.contains(nVariantType) == false))
	{
		MainAppInfo::hashRegisteredCustomPropertySettingObjects.insert(nVariantType, objCustomPropSettObj);
	}
	return true;
}

QObject *MainAppInfo::getCustomPropertySettingObject(const int &nVariantType)
{
	if (MainAppInfo::hashRegisteredCustomPropertySettingObjects.contains(nVariantType))
		return MainAppInfo::hashRegisteredCustomPropertySettingObjects.value(nVariantType);
	return NULL;
}

QWidget *MainAppInfo::setAndRetrieveCustomPropertySettingEditorWidget(const int &nVariantType, const QString &sValue)
{
	QWidget *tmpEditorObject = NULL;
	if (MainAppInfo::hashRegisteredCustomPropertySettingObjects.contains(nVariantType))
	{
		QObject *tmpObject = getCustomPropertySettingObject(nVariantType);
		if (tmpObject)
		{
			QByteArray normalizedSignature = QMetaObject::normalizedSignature(PLUGIN_CUSTOMTYPE_GETNEWEDITOR_METHOD_FULL);
			int methodIndex = tmpObject->metaObject()->indexOfMethod(normalizedSignature);
			QMetaMethod method = tmpObject->metaObject()->method(methodIndex);
			bool bInvokeResult = method.invoke(tmpObject,
				Qt::DirectConnection,
				Q_RETURN_ARG(QWidget *, tmpEditorObject));
			if (tmpEditorObject)
			{
				QByteArray normalizedSignature = QMetaObject::normalizedSignature("setValue(QString)");
				int methodIndex = tmpEditorObject->metaObject()->indexOfMethod(normalizedSignature);
				QMetaMethod method = tmpEditorObject->metaObject()->method(methodIndex);
				bool bInvokeResult = method.invoke(tmpEditorObject,
					Qt::DirectConnection,
					Q_RETURN_ARG(QWidget *, tmpEditorObject));
			}
		}
	}
	return tmpEditorObject;
}

void MainAppInfo::destructCustomPropertySettingObjects()
{
	foreach(QObject *tmpObject,hashRegisteredCustomPropertySettingObjects.values())
	{
		if (tmpObject)
			delete tmpObject;
	}
	hashRegisteredCustomPropertySettingObjects.clear();
}


