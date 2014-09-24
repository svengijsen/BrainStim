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

#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QStringList>
#include <QFile>

#include "modelroles.h"
//class QMimeData;

class ImageListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	ImageListModel(QObject *parent = 0);
	~ImageListModel();

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
	bool setData(const QModelIndex &mIndex, const QVariant &value, int role = ROLE_PATH);

	//bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	//QMimeData *mimeData(const QModelIndexList &indexes) const;
	//QStringList mimeTypes() const;
	int rowCount(const QModelIndex &parent) const;
	Qt::DropActions supportedDropActions() const;

	QString addPixmap(const QString& path);
	QString addPixmap(const QPixmap &pixmap);
	bool getPixmap(QPixmap &pixmap, const QString &ID);
	bool updatePixmap(QPixmap &pixmap, const QString &ID);
	bool updatePixmapID(const QString &oldID, const QString &newID);
	bool removePixmap(const QString &ID);
	
private:
	struct strcImageListModelItem
	{
		QString strID;
		QPixmap pixMap;
		QString strPath;
	};

	QMap<int, strcImageListModelItem> ImageMap;
	QMap<int, strcImageListModelItem>::iterator iterImageMap;
	QMap<int, strcImageListModelItem>::const_iterator constIterImageMap;
	strcImageListModelItem tmpImageListModelItem;
	int nLatestGeneratedID;

	QString getNewID();
	bool isUniqueID(const QString &ID);
	void resetLstModelItem(strcImageListModelItem &item);
	bool createLstModelItem(strcImageListModelItem &item, const QString strID, const QString path, const int role = ROLE_PATH, const QPixmap *pixmap = NULL);
};

#endif // IMAGELISTMODEL_H
