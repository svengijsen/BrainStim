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

#ifndef EXPERIMENTGRAPHOBJECTITEM_H
#define EXPERIMENTGRAPHOBJECTITEM_H

#include <QGraphicsRectItem>
#include <QPainter>
#include "experimentstructures.h"

#define EXPGRAPHOBJECTITEM_OBJECT_WIDTH									1500
#define EXPGRAPHOBJECTITEM_OBJECT_HEIGHT								250
#define EXPGRAPHOBJECTITEM_OBJECT_DISTANCE								250
#define EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_DISTANCE					100
#define EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_START_WIDTH_DISTANCE		EXPGRAPHOBJECTITEM_OBJECT_CONNECTION_DISTANCE * 1.5
#define EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE							30
#define EXPGRAPHOBJECTITEM_METHOD_HEIGHT								EXPGRAPHOBJECTITEM_OBJECT_HEIGHT / 2
#define EXPGRAPHOBJECTITEM_HEADER_HEIGHT								EXPGRAPHOBJECTITEM_METHOD_HEIGHT

struct strcMethodInfo
{
	ExperimentStructuresNameSpace::MethodType mExperimentVisualizerMethodType;
	QString sMethodSignature;
	bool bIsInitialization;
	bool bIsFinalization;
	strcMethodInfo()
	{
		bIsInitialization = false;
		bIsFinalization = false;
		mExperimentVisualizerMethodType = ExperimentStructuresNameSpace::METHOD_TYPE_UNDEFINED;
		sMethodSignature = "";
	}
};

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
		//void setSignalsAndSlots(const QList<strcMethodInfo> &lMethodSignatureList);
		//void setInitializations(const QList<strcMethodInfo> &lMethodSignatureList);
		//void setFinalizations(const QList<strcMethodInfo> &lMethodSignatureList);
		int getMethodLocationIndex(const QString &sSignature, const ExperimentStructuresNameSpace::MethodType &mMethodType);
		int getMethodLocationPosition(const QString &sSignature, const ExperimentStructuresNameSpace::MethodType &mMethodType);

	protected:
		void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
		
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
		QVector<QRectF> vSignalAndSlotsRectList;
		QVector<QRectF> vInitializationsRectList;
		QVector<QRectF> vFinalizationsRectList;
		QStringList lSignalSignatureList;
		QStringList lSlotSignatureList;
		QStringList lInitializationsList;
		QStringList lFinalizationsList;
};

#endif // EXPERIMENTGRAPHOBJECTITEM_H
