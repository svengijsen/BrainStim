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

#ifndef EXPERIMENTGRAPHBLOCKITEM_H
#define EXPERIMENTGRAPHBLOCKITEM_H

#include <QGraphicsRectItem>
#include <QPainter>

#define EXPGRAPHBLOCKITEM_BLOCK_WIDTH					1500
#define EXPGRAPHBLOCKITEM_BLOCK_HEIGHT					250
#define EXPGRAPHBLOCKITEM_BLOCK_DISTANCE				50	//Defined by the input and output connectors, not the connection itself!
#define EXPGRAPHLOOPITEM_LOOP_DISTANCE					120
#define EXPGRAPHLOOPITEM_LOOP_START_WIDTH_DISTANCE		(EXPGRAPHBLOCKITEM_BLOCK_WIDTH/2) + EXPGRAPHLOOPITEM_LOOP_DISTANCE
#define EXPGRAPHLOOPITEM_LOOP_START_HEIGHT_DISTANCE		EXPGRAPH_AUTOLOOP_ADDITIONAL_LOOP_DISTANCE / 2 //EXPGRAPHCONNITEM_LOOP_DISTANCE / 4

enum ExperimentGraphBlockItemTypeEnum
{
	GRAPHBLOCK_TYPE_START			= 0,
	GRAPHBLOCK_TYPE_END				= 1,
	GRAPHBLOCK_TYPE_BLOCK			= 2
};

class ExperimentGraphBlockItem : public QGraphicsItem
{
	public:
        ExperimentGraphBlockItem(QGraphicsItem *parent = NULL);

		QRectF boundingRect() const;
		QPainterPath shape() const;
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		void setType(const ExperimentGraphBlockItemTypeEnum &eBlockType);
		void setCaption(const QString &sName);
		int type() const;

	protected:
		void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
		
	private:
		int nBlockWidth;
		int nHalfBlockWidth;
		int nBlockHeight;
		int nHalfBlockHeight;
		int nBlockDistance;
		int nHalfBlockDistance;
		//bool bIsCurrentlyHovered;
		QString sCurrentText;
		QPen pPen;
		QBrush pBrush;
		QRectF rBoundingBox;	
		QPainterPath pShape;
		ExperimentGraphBlockItemTypeEnum bType;
		QColor cCaption;
		QColor cSelectedColor;
		QColor cHoveredColor;
		QColor cUnselectedColor;
};

#endif // EXPERIMENTGRAPHBLOCKITEM_H
