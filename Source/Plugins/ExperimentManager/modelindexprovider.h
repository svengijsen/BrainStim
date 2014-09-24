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

#ifndef MODELINDEXPROVIDER_H
#define MODELINDEXPROVIDER_H

#include <QObject>
#include <QAbstractItemModel>
#include <QQuickImageProvider>// <qQm qdeclarativeimageprovider.h>
#include "modelroles.h"

class ModelIndexProvider : public QObject, public QQuickImageProvider
{
	Q_OBJECT

public:
	ModelIndexProvider(QAbstractItemModel& model, QObject* parent = 0);
	~ModelIndexProvider();
	QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

	public slots:
		void dataUpdated(const QModelIndex &topLeft, const QModelIndex &bottomRight);
		void dataDeleted(const QModelIndex &parent, int start, int end);
		void dataReset();

private:
	QAbstractItemModel& mModel;
	QMap<QString,QModelIndex> mPixmapNames;
};

#endif // MODELINDEXPROVIDER_H
