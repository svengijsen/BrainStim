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

#ifndef EXPERIMENTGRAPHMETHODCONNECTIONITEM_H
#define EXPERIMENTGRAPHMETHODCONNECTIONITEM_H

#include <QGraphicsRectItem>
#include <QPainter>

#define EXPGRAPH_METHODCONNITEM_ARROW_MIN_SIZE			75.0

enum ExperimentGraphMethodConnectionItemDrawOrder
{
	GRAPHMETHODCONN_DRAW_ORDER_UNNESTED_BOTHSIDES		= 0,
	GRAPHMETHODCONN_DRAW_ORDER_NUMBER_MASTERSIDE		= 1,
	GRAPHMETHODCONN_DRAW_ORDER_NUMBER_SLAVESIDE			= 2
};

enum ExperimentGraphMethodConnTypeEnum
{
	GRAPHBLOCK_METHODCONN_INPUT							= 0,
	GRAPHBLOCK_METHODCONN_OUTPUT						= 1
};

class ExperimentGraphMethodConnectionItem : public QGraphicsItem
{
	public:
		ExperimentGraphMethodConnectionItem(QGraphicsItem *parent = NULL);

		void setEndPoint(const QPointF &pEnd, const float &fPerpLenght = 0.0);
		void renderGraphItem();
		int type() const;
		void setAutoMethodConnType(const bool &bIsAutoMethodConn) {bIsAutoMethodConnType = bIsAutoMethodConn;};

	protected:

		virtual QRectF boundingRect() const;
		virtual QPainterPath shape() const;
		virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
		virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
		
	private:
		QColor cSelectedColor;
		QColor cHoveredColor;
		QColor cUnselectedColor;
		QPen pPen;
		QBrush pBrush;
		QRectF rBoundingBox;
		QPainterPath pArrowDrawShape;
		QPainterPath pArrowBoundingShape;
		QLineF lArrowVectorLine;
		float fPerpendicularLenght;
		bool bIsAutoMethodConnType;
};

#endif // EXPERIMENTGRAPHMETHODCONNECTIONITEM_H
