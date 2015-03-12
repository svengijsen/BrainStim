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

#include "qml2modelindexprovider.h"

QML2ModelIndexProvider::QML2ModelIndexProvider(QAbstractItemModel& model, QObject* parent) : QObject(parent), QQuickImageProvider(QQmlImageProviderBase::Pixmap), mModel(model)
{
	// For each pixmap already in the model, get a mapping between the name and the index
	for(int row = 0; row < mModel.rowCount(); row++)
	{
		QModelIndex index = mModel.index(row, 0);
		QString path = mModel.data(index, ROLE_PATH).value<QString>();
		mPixmapNames[path] = index;
	}
	connect(&mModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(dataUpdated(QModelIndex, QModelIndex)), Qt::ConnectionType(Qt::UniqueConnection));
	connect(&mModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(dataDeleted(QModelIndex, int, int)), Qt::ConnectionType(Qt::UniqueConnection));
	connect(&mModel, SIGNAL(modelReset()), this, SLOT(dataReset()), Qt::ConnectionType(Qt::UniqueConnection));
}

QML2ModelIndexProvider::~QML2ModelIndexProvider()
{

}

QPixmap QML2ModelIndexProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
	QModelIndex index = mPixmapNames[id];
	if (!index.isValid())
	{
		bool bRetval = false;
		int newRow = mModel.rowCount();
		if(mModel.insertRow(newRow))
		{
			index = mModel.index(newRow,0);
			if(mModel.setData(mModel.index(newRow,0),id,ROLE_PATH))
				bRetval = true;
		}
		if (!bRetval)
			return NULL;
	}
	QPixmap image = mModel.data(index, ROLE_PIXMAP).value<QPixmap>();
	if (requestedSize.isValid())
		image = image.scaled(requestedSize, Qt::KeepAspectRatio);
	*size = image.size();
	return image;
}

void QML2ModelIndexProvider::dataUpdated(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	Q_UNUSED(topLeft);
	Q_UNUSED(bottomRight);
	// For each pixmap already in the model, get a mapping between the name and the index
	for(int row = 0; row < mModel.rowCount(); row++) {
		QModelIndex index = mModel.index(row, 0);
		QString name = mModel.data(index, ROLE_ID).value<QString>();
		mPixmapNames[name] = index;
	}
}

void QML2ModelIndexProvider::dataDeleted(const QModelIndex&, int start, int end)
{
	Q_UNUSED(start);
	Q_UNUSED(end);
	mPixmapNames.clear();
	// For each pixmap already in the model, get a mapping between the name and the index
	for(int row = 0; row < mModel.rowCount(); row++) {
		QModelIndex index = mModel.index(row, 0);
		QString name = mModel.data(index, ROLE_ID).value<QString>();
		mPixmapNames[name] = index;
	}
}

void QML2ModelIndexProvider::dataReset()
{
	mPixmapNames.clear();
}