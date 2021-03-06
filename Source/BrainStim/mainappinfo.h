//BrainStim
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

#ifndef MAINAPPINFO_H
#define MAINAPPINFO_H

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QString>
#include <QTime>
#include <iostream>
#include <fstream>
#include <QSettings>
#include <QDockWidget>
#include <QTabWidget>
#include <QTreeView>
#include <QSettings>
#include <QTableWidget>
#include <QResizeEvent>
#include <QTimer>

#include "maindefines.h"

class MainAppInfoData
{
	QString sPluginPath;

	static MainAppInfoData *s_instance;
	MainAppInfoData()
	{
		sPluginPath = "";
	}

public:
	QString getPluginPath()
	{
		return sPluginPath;
	}
	void setPluginPath(const QString &sNewPluginPath)
	{
		sPluginPath = sNewPluginPath;
	}
	static MainAppInfoData *instance()
	{
		if (!s_instance)
			s_instance = new MainAppInfoData;
		return s_instance;
	}
};

class MainAppInfo 
{
private:
	static QFile *mainLogFile;
	static QWidget *mainWindow;
	static QString sMainAppUserPath;
	static QList<int> lRegisteredMetaTypeIds;
	static QHash<int, QObject *> hashRegisteredCustomPropertySettingObjects;
	static QString sPluginsDirPath;

public:
	struct QPairFirstComparer
	{
		template<typename T1, typename T2>
		bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
		{
			return a.first < b.first;
		}
	};

	struct QPairSecondComparer
	{
		template<typename T1, typename T2>
		bool operator()(const QPair<T1,T2> & a, const QPair<T1,T2> & b) const
		{
			return a.second < b.second;
		}
	};

	template<typename T> static int registerMetaTypeClass(const QString &sMetaTypeClassName)
	{
		int nMetaTypeID = qRegisterMetaType<T>(sMetaTypeClassName.toLatin1());
		if(QMetaType::isRegistered(nMetaTypeID))
		{
			if(MainAppInfo::lRegisteredMetaTypeIds.contains(nMetaTypeID) == false)
			{
				MainAppInfo::lRegisteredMetaTypeIds.append(nMetaTypeID);
			}
			return nMetaTypeID;
		}
		return -1;
	}

	static QString getCurrentOSAccountName();
	static bool registerCustomPropertySettingObject(QObject* objCustomPropSettObj, const int &nVariantType);
	static QObject *getCustomPropertySettingObject(const int &nVariantType);
	static QWidget *retrieveCustomPropertySettingEditorWidget(const int &nVariantType);// , const QString &sValue);
	static bool getHexedOrderNumber(const int &nNumber, QString &sResult, const int &nDecimals);
	static bool addRegisteredMetaTypeID(const int &nMetaTypeID);
	static bool setMainWindow(QWidget *mainWin);
	static QString appDirPath()										{return QDir(QCoreApplication::applicationDirPath()).absolutePath();}
	static QString getMainApplicationUserDirectory()				{ return sMainAppUserPath; }//QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);}//ie. Windows -> C:/Users/john.doe/Documents
	static bool setMainApplicationUserDirectory(QString sPath)		{sMainAppUserPath = sPath; return true; }
	static QStringList getUserFolderList()							{return (QStringList() << MAIN_PROGRAM_EXAMPLES_DIRNAME << MAIN_PROGRAM_OUTPUTS_DIRNAME); }
	static QString appDocDirPath()									{return (appDirPath() + "/" + MAIN_PROGRAM_DOC_DIRNAME + "/");}
	static QString appMenuDirPath()									{return (appDirPath() + "/" + MAIN_PROGRAM_MENUS_DIRNAME + "/"); }
	static QString appExampleDirPath()								{ return (MainAppInfo::getMainApplicationUserDirectory() + "/" + MAIN_PROGRAM_EXAMPLES_DIRNAME + "/"); }//appDirPath() + QDir::separator() + MAIN_PROGRAM_EXAMPLES_DIRNAME + QDir::separator());}
	static QString appTempDirPath()									{ return (MainAppInfo::getMainApplicationUserDirectory() + "/" + MAIN_PROGRAM_TEMP_DIRNAME + "/"); }
	static QString appLogFilePath()									{ return (MainAppInfo::getMainApplicationUserDirectory() + "/" + MAIN_PROGRAM_LOGFILE_NAME); }
	static QString appXsdFilePath()									{return (appDirPath() + "/" + MAIN_PROGRAM_XSD_DIRNAME + "/");}
	static QString appUpdatesPath()									{ return (appDirPath() + "/" + MAIN_PROGRAM_UPDATES_DIRNAME + "/"); }
	static void setPluginsDirPath(const QString &sNewPath);
	static QString userPluginsDirPath();
	static QString defaultPluginsDirPath();
	static QString outputsDirPath();
	static QString qmlExtensionsPluginDirPath()
	{
		QString strExtPluginDirPath = appDirPath() + QString("/Qml/plugins/");
#ifdef WIN64
		strExtPluginDirPath = strExtPluginDirPath + QString("x64");
#else
		strExtPluginDirPath = strExtPluginDirPath + QString("Win32");
#endif
		if (QDir(strExtPluginDirPath).exists() == false)
		{
			QDir().mkdir(strExtPluginDirPath);
		}
		else if (QDir(strExtPluginDirPath).exists())
		{
			strExtPluginDirPath = strExtPluginDirPath;
		}
		return QDir(strExtPluginDirPath).absolutePath();
	};

	static QWidget* getMainWindow()							{return mainWindow;};
	static QList<int> getRegisteredMetaTypeIdList()			{return lRegisteredMetaTypeIds;};
	static QString getMainWindowLogSlotName()				{return MAIN_PROGRAM_LOG_SLOT_NAME;};
	static QString getMainWindowReOpenSlotName()			{return MAIN_PROGRAM_REOPEN_SLOT_NAME;};
	static QString getMainWindowScriptIncludePathsName()	{return MAIN_PROGRAM_SCRIPTINCLUDES_NAME;};
	static QString getMainWindowCloseActiveDocSlotName()	{return MAIN_PROGRAM_CLOSEACTIVEDOCUMENT_NAME;};
	static QString getScriptApiClassName()					{return SCRIPT_API_CLASS_NAME_TAG;};
	
	static QString getDefaultFileExtList()					{return QString (MAIN_PROGRAM_FILESEXTENSION_LIST);}
	static QStringList getQTScriptBindingList();
	static QString apiDirPath();
	static QString UntitledDocName()						{return MAIN_PROGRAM_UNTITLED_NAME;}
	static QString stdDateTimeFormat()						{return MAIN_PROGRAM_STANDARD_DATETIME_FORMAT;}

	static bool isCompatibleVersion(const QString &strMinimalRequiredVersion, const QString &strCurrentVersion);

	static void MyOutputHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
	static void CloseMainLogFile();
	static bool CreateHashTableFromEnumeration(const QString &sTypeName, QHash<QString, int> &hTable, const QMetaObject metaObject);

	static void destructCustomPropertySettingObjects();

private:

	static QDir appDebugDirPath()
	{
		QDir appDebugDir = QDir(appDirPath());
#if defined(Q_OS_WIN)
		if (appDebugDir.dirName().toLower() == "debug" || appDebugDir.dirName().toLower() == "release")
		{
			appDebugDir.cdUp();
			if (appDebugDir.dirName().toLower() == "win32" || appDebugDir.dirName().toLower() == "x64")//This is now unnecessary in VS2010! It is solved by changing the debugpath to the install dir.
			{
				appDebugDir.cdUp();
				appDebugDir.cdUp();
				appDebugDir.cdUp();
				appDebugDir.cd("Install");			
			}
		}
#elif defined(Q_OS_MAC)
		if (appDebugDir.dirName() == "MacOS") {
			appDebugDir.cdUp();
			appDebugDir.cdUp();
			appDebugDir.cdUp();
		}
#endif
		return appDebugDir.absolutePath();
	};
};

class MainWindowDockWidget : public QDockWidget
{
	Q_OBJECT

signals :
	void resizingFinished(MainWindowDockWidget *);

public:
	MainWindowDockWidget(const QString &sTitle, Qt::DockWidgetArea defaultDocWidgetArea, QWidget *parent = NULL, Qt::WindowFlags flags = 0) : QDockWidget(sTitle, parent, flags), defaultDocArea(defaultDocWidgetArea)
	{
		currentDocArea = defaultDocArea;
		bEnableResizingSignaling = true;
		bResizeStarted = false;
		m_resizeTimer.setSingleShot(true);
		connect(&m_resizeTimer, SIGNAL(timeout()), SLOT(resizeDone()));
	};
	bool enableResizingSignaling(const bool &bEnable = true)
	{
		bool bTemp = bEnableResizingSignaling;
		bEnableResizingSignaling = bEnable;
		return bTemp;//returns original setting
	};
	Qt::DockWidgetArea getDefaultDocWidgetArea()
	{
		return defaultDocArea;
	};
	void setCurrentDocWidgetArea(const Qt::DockWidgetArea &newDockArea)
	{
		currentDocArea = newDockArea;
	};
	Qt::DockWidgetArea getCurrentDocWidgetArea()
	{
		return currentDocArea;
	};

protected:
	void resizeEvent(QResizeEvent * event) 
	{
		Q_UNUSED(event);
		m_resizeTimer.start(200);
		QDockWidget::resizeEvent(event);
	};

	/*void mouseReleaseEvent(QMouseEvent* event)
	{
		Q_UNUSED(event);
		if (bResizeStarted)
		{
			bResizeStarted = false;
			if (bEnableResizingSignaling)
				emit resizingFinished(this);
		}
	};*/

private slots:
	void resizeDone()
	{
		bResizeStarted = true;
		//add test
		emit resizingFinished(this);
		bResizeStarted = false;
	}

private:
	QTimer m_resizeTimer;
	bool bResizeStarted;
	bool bEnableResizingSignaling;
	Qt::DockWidgetArea defaultDocArea;
	Qt::DockWidgetArea currentDocArea;
};

class CustomChildDockTabWidget : public QTabWidget
{
	Q_OBJECT

public:
	CustomChildDockTabWidget(QWidget *parent = NULL) 
	{ 
		QTabWidget::setParent(parent); 
		sGroupName = ""; 
	};
	~CustomChildDockTabWidget() {};

	void setGroupName(const QString &sNewGroupName) { sGroupName = sNewGroupName; };
	QString getGroupName() const { return sGroupName; };
	QSize minimumSize() const { return QSize(100, 100); };
	QSize sizeHint() const
	{
		QSize sizeCurrentSizeHint = QTabWidget::sizeHint();
		if (this->parentWidget())
		{
			QString sAccessName = this->parentWidget()->accessibleName();
			bool bRetVal;
			QRect rCurrentSizeHint;
			rCurrentSizeHint.setSize(sizeCurrentSizeHint);
			QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_GETSAVEDDOCKWIDGETSIZEHINT_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(QString, sGroupName), Q_ARG(QString, sAccessName), Q_ARG(QRect&, rCurrentSizeHint));
			if (bRetVal)
				return rCurrentSizeHint.size();
		}
		return sizeCurrentSizeHint;
	};

private:
	QString sGroupName;
};

class CustomChildDockTreeViewWidget : public QTreeView
{
	Q_OBJECT

public:
	CustomChildDockTreeViewWidget(QWidget *parent = NULL) { QTreeView::setParent(parent); sGroupName = ""; };
	~CustomChildDockTreeViewWidget() {};

	void setGroupName(const QString &sNewGroupName) { sGroupName = sNewGroupName; };
	QString getGroupName() const { return sGroupName; };
	QSize minimumSize() const { return QSize(100, 100); };
	QSize sizeHint() const
	{
		QSize sizeCurrentSizeHint = QTreeView::sizeHint();
		if (this->parentWidget())
		{
			QString sAccessName = this->parentWidget()->accessibleName();
			bool bRetVal;
			QRect rCurrentSizeHint;
			rCurrentSizeHint.setSize(sizeCurrentSizeHint);
			QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_GETSAVEDDOCKWIDGETSIZEHINT_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(QString, sGroupName), Q_ARG(QString, sAccessName), Q_ARG(QRect&, rCurrentSizeHint));
			if (bRetVal)
				return rCurrentSizeHint.size();
		}
		return sizeCurrentSizeHint;
	};

private:
	QString sGroupName;
};

class CustomChildDockTableWidget : public QTableWidget
{
	Q_OBJECT

public:
	CustomChildDockTableWidget(QWidget *parent = NULL) { QTableWidget::setParent(parent); sGroupName = ""; };
	~CustomChildDockTableWidget() {};

	void setGroupName(const QString &sNewGroupName) { sGroupName = sNewGroupName; };
	QString getGroupName() const { return sGroupName; };
	QSize minimumSize() const { return QSize(100, 100); };
	QSize sizeHint() const
	{
		QSize sizeCurrentSizeHint = QTableWidget::sizeHint();
		if (this->parentWidget())
		{
			QString sAccessName = this->parentWidget()->accessibleName();
			bool bRetVal;
			QRect rCurrentSizeHint;
			rCurrentSizeHint.setSize(sizeCurrentSizeHint);
			QMetaObject::invokeMethod(MainAppInfo::getMainWindow(), MAIN_PROGRAM_GETSAVEDDOCKWIDGETSIZEHINT_NAME, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal), Q_ARG(QString, sGroupName), Q_ARG(QString, sAccessName), Q_ARG(QRect&, rCurrentSizeHint));
			if (bRetVal)
				return rCurrentSizeHint.size();
		}
		return sizeCurrentSizeHint;
	};

private:
	QString sGroupName;
};

#endif // MAINAPPINFO_H

