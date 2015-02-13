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


#include "imagelistmodel.h"

ImageListModel::ImageListModel(QObject *parent)	: QAbstractListModel(parent), nLatestGeneratedID(-1)
{

}

ImageListModel::~ImageListModel()
{
	ImageMap.clear();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if(index.row()<ImageMap.count())
	{
		switch (role)
		{
		case ROLE_PATH:
			return ImageMap.value(index.row()).strPath;
			break;
		case ROLE_PIXMAP:
			return ImageMap.value(index.row()).pixMap;
			break;
		case ROLE_ID:
			return ImageMap.value(index.row()).strID;
			break;
		default:
			break;
		}
	}
	return QVariant();
}

Qt::ItemFlags ImageListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())
		return (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
	return Qt::ItemIsDropEnabled;
}

bool ImageListModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (parent.isValid())
		return false;
	if (row >= ImageMap.size() || row + count <= 0 || row < 0)
		return false;
	int beginRow = row;
	int endRow = qMin(row + count - 1, ImageMap.size() - 1);
	beginRemoveRows(parent, beginRow, endRow);
	while (beginRow <= endRow) 
	{
		ImageMap.remove(ImageMap.keys().at(beginRow));
		//ImageMap.removeAt(beginRow);
		//locations.removeAt(beginRow);
		++beginRow;
	}
	endRemoveRows();
	return true;
}

int ImageListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	else
		return ImageMap.count();//size();
}

Qt::DropActions ImageListModel::supportedDropActions() const
{
	return Qt::IgnoreAction;// Qt::CopyAction | Qt::MoveAction;
}

bool ImageListModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if ((row > rowCount(parent)) || (count < 1))
		return false;
	resetLstModelItem(tmpImageListModelItem);
	beginInsertRows(parent, row, row + count -1);
	for (int i=row;i<row+count;i++)
	{
		ImageMap.insert(i, tmpImageListModelItem);
	}
	endInsertRows();
	return true;
}

bool ImageListModel::setData(const QModelIndex &mIndex, const QVariant &value, int role)
{
	int row = mIndex.row();
	bool bRetVal = false;
	switch (role)
	{
	case ROLE_PATH:
		bRetVal = createLstModelItem(tmpImageListModelItem,QString(""),value.toString(),role);
		break;
	case ROLE_PIXMAP:
		bRetVal = createLstModelItem(tmpImageListModelItem,QString(""),QString(""),role, value.value<QPixmap>());
		break;
	case ROLE_ID:
		bRetVal = createLstModelItem(tmpImageListModelItem,value.toString(),QString(""),role);
		break;
	default:
		return false;
		break;
	}
	if(bRetVal)
	{
		ImageMap.insert(row,tmpImageListModelItem);
		//QModelIndex topLeftIndex = index(row,0);
		//QModelIndex bottomRightIndex = index(row,0);
		emit dataChanged(mIndex,mIndex);
		return true;
	}
	return false;
}

QString ImageListModel::addPixmap(const QString &path)
{
	int row = ImageMap.count();
	if(createLstModelItem(tmpImageListModelItem,QString(""),path,ROLE_PATH))
	{
		beginInsertRows(QModelIndex(), row, row);
		ImageMap.insert(row, tmpImageListModelItem);
		endInsertRows();
		return tmpImageListModelItem.strID;
	}
	return "";
}

QString ImageListModel::addPixmap(const QPixmap &pixmap)//, const QString &name)
{
	//QString strRetval = "";
	int newRow = rowCount(QModelIndex());
	if(insertRows(newRow,1))
	{
		QModelIndex mIndex = index(newRow,0);
		if(setData(mIndex,pixmap,ROLE_PIXMAP))
			return ImageMap.value(newRow).strID;
			//strRetval = tmpImageListModelItem;
	}
	return "";
}

bool ImageListModel::getPixmap(QPixmap &pixmap, const QString &ID)
{
	constIterImageMap = ImageMap.constBegin();
	while (constIterImageMap != ImageMap.constEnd()) 
	{
		if(constIterImageMap.value().strID == ID)
		{
			pixmap = constIterImageMap.value().pixMap.copy();
			return true;
		}
		++constIterImageMap;
	}
	return false;
}

bool ImageListModel::updatePixmap(QPixmap &pixmap, const QString &ID)
{
	iterImageMap = ImageMap.begin();
	while (iterImageMap != ImageMap.end()) 
	{
		if(iterImageMap.value().strID == ID)
		{
			iterImageMap.value().pixMap = pixmap.copy();
			return true;
		}
		++iterImageMap;
	}
	return false;
}

bool ImageListModel::updatePixmapID(const QString &oldID, const QString &newID)
{
	iterImageMap = ImageMap.begin();
	while (iterImageMap != ImageMap.end()) 
	{
		if(iterImageMap.value().strID == oldID)
		{
			iterImageMap.value().strID = newID;
			return true;
		}
		++iterImageMap;
	}
	return false;
}

bool ImageListModel::removePixmap(const QString &ID)
{
	for (int i=0;i<ImageMap.size();++i)
	{		
		if(ImageMap.values().at(i).strID == ID)
		{
			return (removeRows(i,1));
		}
	}
	return false;
}

bool ImageListModel::createLstModelItem(strcImageListModelItem &item, const QString strID, const QString path, const int role, const QPixmap &pixmap)
{
	resetLstModelItem(item);
	switch (role)
	{
	case ROLE_PATH:
	{
		QString strStrippedPath;
		int nSepPoint = path.lastIndexOf(ROLE_PATH_SEPERATOR);
		if (nSepPoint >= 0)
			strStrippedPath = path.left(nSepPoint);
		else
			strStrippedPath = path;

		QFile fileSource(strStrippedPath);
		if (!fileSource.exists())
			return false;
		if (!item.pixMap.load(strStrippedPath))
			return false;
		item.strPath = strStrippedPath;
		if (strID == "")						//If no name is defined then use the path as the Unique ID
			item.strID = path;
		else
			item.strID = strID;
		break;
	}
	case ROLE_PIXMAP:
		item.pixMap = pixmap.copy();
		if (strID == "")						//If no name is defined then generate a new Unique ID
			item.strID = getNewID();
		else
			item.strID = strID;
		break;
	case ROLE_ID:
		if (strID == "")						//If no name is defined then generate a new Unique ID
			item.strID = getNewID();
		else
			item.strID = strID;
		break;
	default:
		return false;
		break;
	}
	return true;
}

QString ImageListModel::getNewID()
{
	QString nCurrentGeneratedIDString = QString::number(++nLatestGeneratedID);
	while(isUniqueID(nCurrentGeneratedIDString) == false)
	{
		nCurrentGeneratedIDString = QString::number(++nLatestGeneratedID);
	}
	return nCurrentGeneratedIDString;
}

bool ImageListModel::isUniqueID(const QString &ID)
{
	constIterImageMap = ImageMap.constBegin();
	while (constIterImageMap != ImageMap.constEnd()) 
	{
		if(constIterImageMap.value().strID == ID)
			return false;
		++constIterImageMap;
	}
	return true;
}

void ImageListModel::resetLstModelItem(strcImageListModelItem &item)
{
	item.pixMap.fill();// = NULL;
	item.strPath = "";
	item.strID = "";
};

//QStringList ImageListModel::mimeTypes() const
//{
//	QStringList types;
//	types << "image/x-puzzle-piece";
//	return types;
//}
//
//QMimeData *ImageListModel::mimeData(const QModelIndexList &indexes) const
//{
//	QMimeData *mimeData = new QMimeData();
//	QByteArray encodedData;
//
//	QDataStream stream(&encodedData, QIODevice::WriteOnly);
//
//	foreach (QModelIndex index, indexes) {
//		if (index.isValid()) {
//			QPixmap pixmap = qvariant_cast<QPixmap>(data(index, Qt::UserRole));
//			QPoint location = data(index, Qt::UserRole+1).toPoint();
//			stream << pixmap << location;
//		}
//	}
//
//	mimeData->setData("image/x-puzzle-piece", encodedData);
//	return mimeData;
//}
//
//bool ImageListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
//	int row, int column, const QModelIndex &parent)
//{
//	if (!data->hasFormat("image/x-puzzle-piece"))
//		return false;
//
//	if (action == Qt::IgnoreAction)
//		return true;
//
//	if (column > 0)
//		return false;
//
//	int endRow;
//
//	if (!parent.isValid()) {
//		if (row < 0)
//			endRow = pixmaps.size();
//		else
//			endRow = qMin(row, pixmaps.size());
//	} else
//		endRow = parent.row();
//
//	QByteArray encodedData = data->data("image/x-puzzle-piece");
//	QDataStream stream(&encodedData, QIODevice::ReadOnly);
//
//	while (!stream.atEnd()) {
//		QPixmap pixmap;
//		QPoint location;
//		stream >> pixmap >> location;
//
//		beginInsertRows(QModelIndex(), endRow, endRow);
//		pixmaps.insert(endRow, pixmap);
//		locations.insert(endRow, location);
//		endInsertRows();
//
//		++endRow;
//	}
//
//	return true;
//}