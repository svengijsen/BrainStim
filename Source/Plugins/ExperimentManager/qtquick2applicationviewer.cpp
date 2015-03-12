//ExperimentManagerplugin
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

#include "qtquick2applicationviewer.h"

#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QtQml>
#include "defines.h"
#include "qml2viewer.h"

class QtQuick2ApplicationViewerPrivate
{
    QString mainQmlFile;
    friend class QtQuick2ApplicationViewer;
    static QString adjustPath(const QString &path);
};

QString QtQuick2ApplicationViewerPrivate::adjustPath(const QString &path)
{
#if defined(Q_OS_MAC)
    if (!QDir::isAbsolutePath(path))
        return QString::fromLatin1("%1/../Resources/%2")
                .arg(QCoreApplication::applicationDirPath(), path);
#elif defined(Q_OS_UNIX)
    const QString pathInInstallDir =
            QString::fromLatin1("%1/../%2").arg(QCoreApplication::applicationDirPath(), path);
    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
    return path;
}

QtQuick2ApplicationViewer::QtQuick2ApplicationViewer(QObject *parent) : QQuickView(), d(new QtQuick2ApplicationViewerPrivate()), parentObject(parent)
{
	Q_UNUSED(parent);
	//configureEventFilter(parent);
    //bool bResult = connect(engine(), &QQmlEngine::quit, this, &QWindow::close);
	//bool bResult = 
	connect(engine(), &QQmlEngine::quit, this, &QtQuick2ApplicationViewer::qtQuick2EngineQuit, Qt::ConnectionType(Qt::UniqueConnection));
    setResizeMode(QQuickView::SizeRootObjectToView);
	this->setFlags(Qt::FramelessWindowHint);
	qml2InterfaceObject = new Qml2Interface(this);
	if (parentObject)
	{
		QML2Viewer *tmpParentViewer = qobject_cast<QML2Viewer*>(parentObject);
		if (tmpParentViewer)
		{
			qml2InterfaceObject->setQMLViewerObject(tmpParentViewer);
		}
	}
}

void QtQuick2ApplicationViewer::qtQuick2EngineQuit()
{
	QWindow::close();
	emit QtQuickEngineQuit();
}

QtQuick2ApplicationViewer::~QtQuick2ApplicationViewer()
{
    delete d;
	if(qml2InterfaceObject)
		delete qml2InterfaceObject;
}

QScreen *QtQuick2ApplicationViewer::grabScreenUnderMouseCursor()
{
	QPoint pCurrMouseCursorPos = this->mapFromGlobal(QCursor::pos());
	int nScreenNumber = QApplication::desktop()->screenNumber(pCurrMouseCursorPos);
	if(nScreenNumber>=0)
	{
		QList<QScreen*> lAvailableScreens = QGuiApplication::screens();
		if(lAvailableScreens.count() > nScreenNumber)
			return lAvailableScreens.at(nScreenNumber);
	}
	return QGuiApplication::primaryScreen();
}

int QtQuick2ApplicationViewer::registerDefaultCustomQMLTypes()
{
	int nTypeID = qmlRegisterSingletonType<Qml2Interface>(PLUGIN_QML_INTERFACE_PROVIDER_URI, PLUGIN_QML_INTERFACE_PROVIDER_MAJOR, PLUGIN_QML_INTERFACE_PROVIDER_MINOR, PLUGIN_QML_INTERFACE_PROVIDER_APINAME, interface_singletontype_provider);
	return nTypeID;
}

void QtQuick2ApplicationViewer::setMainQmlFile(const QString &file)
{
    d->mainQmlFile = QtQuick2ApplicationViewerPrivate::adjustPath(file);
    setSource(QUrl::fromLocalFile(d->mainQmlFile));
}

void QtQuick2ApplicationViewer::addImportPath(const QString &path)
{
    engine()->addImportPath(QtQuick2ApplicationViewerPrivate::adjustPath(path));
}

void QtQuick2ApplicationViewer::showExpanded()
{
#if defined(Q_WS_SIMULATOR)
    showFullScreen();
#else
    show();
#endif
}
