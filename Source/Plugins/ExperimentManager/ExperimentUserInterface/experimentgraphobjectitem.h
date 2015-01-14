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

#ifndef EXPERIMENTGRAPHOBJECTITEM_H
#define EXPERIMENTGRAPHOBJECTITEM_H

#include "global.h"
#include <QGraphicsRectItem>
#include <QPainter>
//#include "experimentstructures.h"

#define EXPGRAPHOBJECTITEM_OBJECT_WIDTH									1500
#define EXPGRAPHOBJECTITEM_OBJECT_HEIGHT								250
#define EXPGRAPHOBJECTITEM_OBJECT_DISTANCE								250
#define EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_DISTANCE					100
#define EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_START_WIDTH_DISTANCE		EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_DISTANCE * 1.5
#define EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE							30
#define EXPGRAPHOBJECTITEM_METHOD_HEIGHT								EXPGRAPHOBJECTITEM_OBJECT_HEIGHT / 2
#define EXPGRAPHOBJECTITEM_HEADER_HEIGHT								EXPGRAPHOBJECTITEM_METHOD_HEIGHT

//enum GraphObjectItemMethodType
//{
//	GRAPHOBJECT_METHOD_TYPE_UNDEFINED = 0,
//	GRAPHOBJECT_METHOD_TYPE_SIGNAL_SLOT = 1,
//	GRAPHOBJECT_METHOD_TYPE_INIT = 2,
//	GRAPHOBJECT_METHOD_TYPE_FINIT = 3
//};

struct strcMethodInfo
{
	ExperimentManagerNameSpace::ExperimentStructureItemType mExperimentVisualizerMethodType;
	QString sMethodSignature;
	int nMethodID;
	bool bIsInitialization;
	bool bIsFinalization;
	strcMethodInfo()
	{
		nMethodID = -1;
		bIsInitialization = false;
		bIsFinalization = false;
		mExperimentVisualizerMethodType = ExperimentManagerNameSpace::TypeUndefined;
		sMethodSignature = "";
	}
};

//////////////////////////////////////////////////////////////////

class ExperimentGraphSubObjectTextItem : public QGraphicsItem
{
public:
	ExperimentGraphSubObjectTextItem(QGraphicsItem *parent = NULL);

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void setGeometry(const QRectF &rectGeometry);
	void setItemText(const QString &sText);
	void setItemID(const int &nID) { nItemID = nID; };
	int getItemID() { return nItemID; };
	void setActiveStates(int nFlags);
	void setGraphItemType(ExperimentManagerNameSpace::ExperimentStructureItemType graphItemType);
	int type() const;

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	void renderGraphItem();

	QRectF rBoundingBox;
	QPainterPath sBoundingBoxPath;
	QString sItemText;
	QFont fCurrent;
	QPen pPen;
	QBrush pBrush;

	int nItemID;
	int nItemStateFlags;
	ExperimentManagerNameSpace::ExperimentStructureItemType eGraphItemType;
};

//////////////////////////////////////////////////////////////////

class ExperimentGraphSubObjectItem : public QGraphicsItem
{
public:
	ExperimentGraphSubObjectItem(QGraphicsItem *parent = NULL);

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void setGeometry(const QRectF &rectGeometry);
	void setSubItems(const QMap<int, QString> &mapSubItemIDsToStringList);
	void setGraphItemType(ExperimentManagerNameSpace::ExperimentStructureItemType graphItemType);
	ExperimentManagerNameSpace::ExperimentStructureItemType getGraphItemType() { return (ExperimentManagerNameSpace::ExperimentStructureItemType)type(); };
	void setHeaderText(const QString &sText);
	void setActiveStates(int nFlags);
	int type() const;

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	//QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
	void renderGraphItem();

	QRectF rBoundingBox;
	QPainterPath sBoundingBoxPath;
	QString sHeaderText;
	QMap<int, QString> mapSubItemIDToStringList;
	QFont fCurrent;
	QPen pPen;
	QBrush pBrush;
	QMap <QString, ExperimentGraphSubObjectTextItem *> mExpGraphSubObjectNameToTextItems;
	ExperimentManagerNameSpace::ExperimentStructureItemType eGraphItemType;
	int nItemStateFlags;
};

//////////////////////////////////////////////////////////////////

class ExperimentGraphObjectItem : public QGraphicsItem
{
	public:
        ExperimentGraphObjectItem(QGraphicsItem *parent = NULL);

		QRectF boundingRect() const;
		QPainterPath shape() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		void setCaption(const QString &sName);
		int type() const;
		void setMethods(const QList<strcMethodInfo> &lMethodSignatureList);
		int getMethodLocationIndex(const QString &sSignature, const ExperimentManagerNameSpace::ExperimentStructureItemType &graphItemType);
		int getMethodLocationPosition(const QString &sSignature, const ExperimentManagerNameSpace::ExperimentStructureItemType &graphItemType);

	protected:
		void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
		//QVariant itemChange(GraphicsItemChange change, const QVariant &value);
		
	private:
		void renderGraphItem();

		int nObjectWidth;
		int nHalfObjectWidth;
		int nObjectHeight;
		int nHalfObjectHeight;
		int nObjectDistance;
		int nHalfObjectDistance;
		qreal rAdditionalMethodConnectionsHeight;
		qreal rAdditionalInitializationsHeight;
		qreal rAdditionalFinalizationsHeight;
		qreal rTotalHeight;
		QString sCurrentText;
		QFont fCurrent;
		QPen pHeaderPen;
		QPen pObjectOutlinePen;
		QBrush pBrush;
		QRectF rBoundingBox;
		QRectF rectInnerObject;
		QRectF rConnectionsBox;
		QRectF rInitializationBox;
		QRectF rFinalizationBox;
		QPainterPath pHeaderShapePath;
		QPainterPath sBoundingBoxPath;
		QColor cHeaderCaption;
		QColor cHeaderSelectedColor;
		QColor cHeaderHoveredColor;
		QColor cHeaderUnselectedColor;
		QMap <int, QString> mapSignalIDToSignatureList;
		QMap <int, QString> mapSlotIDToSignatureList;
		QMap <int, QString> mapInitializationsIDToSignatureList;
		QMap <int, QString> mapFinalizationsIDToSignatureList;
		QMap <int, QString> mapSignalLocationIndexToSignatureList;
		QMap <int, QString> mapSlotLocationIndexToSignatureList;
		QMap <int, QString> mapInitializationsLocationIndexToSignatureList;
		QMap <int, QString> mapFinalizationsLocationIndexToSignatureList;
		ExperimentGraphSubObjectItem *subGraphItemInitializations;
		ExperimentGraphSubObjectItem *subGraphItemFinalizations;
		ExperimentGraphSubObjectItem *subGraphItemSignalSlots;
};

#endif // EXPERIMENTGRAPHOBJECTITEM_H
