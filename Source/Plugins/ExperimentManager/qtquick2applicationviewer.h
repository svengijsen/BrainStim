//ExperimentManagerplugin
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

#ifndef QTQUICK2APPLICATIONVIEWER_H
#define QTQUICK2APPLICATIONVIEWER_H

#include <QtQuick/QQuickView>
#include "qml2interface.h"

class QtQuick2ApplicationViewer : public QQuickView
{
    Q_OBJECT

signals:
	void QtQuickEngineQuit();

private slots:
	void qtQuick2EngineQuit();

public:
    explicit QtQuick2ApplicationViewer(QObject *parent = NULL);
    virtual ~QtQuick2ApplicationViewer();

	QScreen *grabScreenUnderMouseCursor();

    void setMainQmlFile(const QString &file);
    void addImportPath(const QString &path);
	int registerDefaultCustomQMLTypes();
	void showExpanded();

protected:
	//bool event(QEvent*);
	//bool eventFilter(QObject*, QEvent*);
	//void closeEvent(QCloseEvent *event);

private:
	//QQuickWindow* window_;
	QObject *parentObject;
	//void configureEventFilter(QObject* parentObject);
    class QtQuick2ApplicationViewerPrivate *d;
	Qml2Interface *qml2InterfaceObject;
};

#endif // QTQUICK2APPLICATIONVIEWER_H
