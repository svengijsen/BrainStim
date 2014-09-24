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


#include "qmlerrorhandler.h"
#include <QDebug>

QmlErrorHandler::QmlErrorHandler(QQuickView &view, QObject *parent) : QObject(parent), mView(view), mErrorOccured(false)
{
	connect(&view, SIGNAL(statusChanged(QQuickView::Status)), this, SLOT(handleQmlStatusChange(QQuickView::Status)));
	connect((QObject*)view.engine(), SIGNAL(warnings(QList<QQmlError>)), this, SLOT(handleQmlErrors(QList<QQmlError>)));
}

QmlErrorHandler::~QmlErrorHandler()
{
	disconnect(this, SLOT(handleQmlStatusChange(QQuickView::Status)));
	disconnect(this, SLOT(handleQmlErrors(QList<QDeclarativeError>)));
}

void QmlErrorHandler::handleQmlStatusChange(QQuickView::Status status)
{
	if (status == QQuickView::Error) 
	{
		handleQmlErrors(mView.errors());
	}
}

void QmlErrorHandler::handleQmlErrors(const QList<QQmlError>& qmlErrors)
{
	QStringList errors;
	foreach (const QQmlError& error, qmlErrors) 
	{
		// Special case for bug in QtComponents 1.1
		// https://bugreports.qt-project.org/browse/QTCOMPONENTS-1217
		if (error.url().toString().endsWith("PageStackWindow.qml") && error.line() == 70)
			continue;

		errors.append(error.toString());
	}

	if (errors.isEmpty())
		return;

	mErrorOccured = true;

	//QMessageBox msgBox;
	//msgBox.setText(errors.join("\n"));
	//msgBox.exec();
	qWarning() << errors.join("\n");
}

bool QmlErrorHandler::errorOccured() const
{
	return mErrorOccured;
}