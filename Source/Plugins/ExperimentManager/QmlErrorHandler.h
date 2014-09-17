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

#ifndef QMLERRORHANDLER_H
#define QMLERRORHANDLER_H

#include <QObject>
#include <QtQml/QQmlError>
#include <QtQuick/QQuickView>
#include <QMessageBox>

class QmlErrorHandler : public QObject
{
	Q_OBJECT

public:
	explicit QmlErrorHandler(QQuickView &view, QObject *parent = NULL);//QmlErrorHandler(QObject *parent = NULL);
	~QmlErrorHandler();

	bool errorOccured() const;

private slots:
	void handleQmlStatusChange(QQuickView::Status status);
	void handleQmlErrors(const QList<QQmlError>& qmlErrors);

private:
	QQuickView &mView;
	bool mErrorOccured;
};

#endif // QMLERRORHANDLER_H
