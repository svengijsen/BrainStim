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

#include "experimentgraphblockitem.h"
#include "experimentstructurescene.h"
#include <QStyleOptionGraphicsItem>

ExperimentGraphBlockItem::ExperimentGraphBlockItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	bType = GRAPHBLOCK_TYPE_BLOCK;
	pPen.setWidth(2.0);
	cCaption.setRgb(63,72,204);
	cUnselectedColor.setRgb(195,195,195);
	cSelectedColor.setRgb(0,228,0);
	cHoveredColor.setRgb(239,228,176);
	pPen.setColor(cCaption);
	pBrush.setColor(cUnselectedColor);
	pBrush.setStyle(Qt::SolidPattern);

	nBlockWidth = EXPGRAPHBLOCKITEM_BLOCK_WIDTH;
	nHalfBlockWidth = nBlockWidth/2;
	nBlockHeight = EXPGRAPHBLOCKITEM_BLOCK_HEIGHT;
	nHalfBlockHeight = nBlockHeight/2;
	nBlockDistance = EXPGRAPHBLOCKITEM_BLOCK_DISTANCE;
	nHalfBlockDistance = nBlockDistance/2;
	rBoundingBox.setCoords(-nHalfBlockWidth,-nHalfBlockHeight-nHalfBlockDistance,nHalfBlockWidth,nHalfBlockHeight+nHalfBlockDistance); 

	//bIsCurrentlyHovered = false;
	//this->setToolTip("ExperimentGraphBlockItem");
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable);
	
	setAcceptDrops(true);
	//setFlag(QGraphicsItem::ItemIsMovable);
}

void ExperimentGraphBlockItem::setType(const ExperimentGraphBlockItemTypeEnum &eBlockType)
{
	bType = eBlockType;
	update();
}

void ExperimentGraphBlockItem::setCaption(const QString &sName)
{
	sCurrentText = sName;
	update();
}

int ExperimentGraphBlockItem::type() const
{
	return ExperimentManagerNameSpace::TypeBlockItem;
}

QPainterPath ExperimentGraphBlockItem::shape() const
{
	return pShape;
}

QRectF ExperimentGraphBlockItem::boundingRect() const
{
	return rBoundingBox;
}

void ExperimentGraphBlockItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	QFont fCurrent("Times");

	painter->setRenderHint(QPainter::Antialiasing);
	fCurrent.setBold(true);
	if(option->state & QStyle::State_Selected)
	{
		//fCurrent.setBold(false);
		pBrush.setColor(cSelectedColor);
		pBrush.setColor(cSelectedColor);
		pPen.setColor(cSelectedColor);
	}
	else if(option->state & QStyle::State_MouseOver)
	{
		//fCurrent.setBold(true);
		pBrush.setColor(cHoveredColor);
		pBrush.setColor(cHoveredColor);
		pPen.setColor(cHoveredColor);
	}
	else
	{
		//fCurrent.setBold(false);
		pBrush.setColor(cUnselectedColor);
		pBrush.setColor(cUnselectedColor);
		pPen.setColor(cUnselectedColor);
	}
	painter->setPen(pPen);
	painter->setBrush(pBrush);

	pShape = QPainterPath();
	if((bType == GRAPHBLOCK_TYPE_BLOCK) || (bType == GRAPHBLOCK_TYPE_END))
	{
		pShape.moveTo(0, -nHalfBlockHeight-nHalfBlockDistance);
		pShape.lineTo(0, -nHalfBlockHeight);
	}
	QRectF rectInnerBlock(-nHalfBlockWidth, -nHalfBlockHeight, nBlockWidth, nBlockHeight);	
	pShape.addRoundedRect(rectInnerBlock,50,50);	
	if((bType == GRAPHBLOCK_TYPE_BLOCK) || (bType == GRAPHBLOCK_TYPE_START))
	{
		pShape.moveTo(0, nHalfBlockHeight);
		pShape.lineTo(0, nHalfBlockHeight+nHalfBlockDistance);
	}
	painter->drawPath(pShape);

	fCurrent.setPointSizeF(rectInnerBlock.height()/4);
	painter->setFont(fCurrent);
	pPen.setColor(cCaption);
	painter->setPen(pPen);

	if(bType == GRAPHBLOCK_TYPE_START)
	{
		rBoundingBox.setCoords(-nHalfBlockWidth,-nHalfBlockHeight,nHalfBlockWidth,nHalfBlockHeight+nHalfBlockDistance);
		painter->drawText(rectInnerBlock,Qt::AlignCenter, "START");
	}
	else if(bType == GRAPHBLOCK_TYPE_BLOCK)
	{
		rBoundingBox.setCoords(-nHalfBlockWidth,-nHalfBlockHeight-nHalfBlockDistance,nHalfBlockWidth,nHalfBlockHeight+nHalfBlockDistance);
		painter->drawText(rectInnerBlock,Qt::AlignLeft | Qt::AlignVCenter, " " + sCurrentText);
	}
	else if(bType == GRAPHBLOCK_TYPE_END)
	{
		rBoundingBox.setCoords(-nHalfBlockWidth,-nHalfBlockHeight-nHalfBlockDistance,nHalfBlockWidth,nHalfBlockHeight);
		painter->drawText(rectInnerBlock,Qt::AlignCenter, "END");
	}
}

void ExperimentGraphBlockItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(2);
	update();
}

//void QGraphicsItem::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )

void ExperimentGraphBlockItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(0);
	else
		this->setZValue(1);
	update();
}

void ExperimentGraphBlockItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	Q_UNUSED(event);
	return;
	//qDebug() << "I'm on the main window!";
	//if (event->mimeData()->hasColor()) {
	//	event->setAccepted(true);
	//	dragOver = true;
	//	update();
	//}
	//else {
	//	event->setAccepted(false);
	//}
}

void ExperimentGraphBlockItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	Q_UNUSED(event);
	return;
	//dragOver = false;
	//update();
}

void ExperimentGraphBlockItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	//qDebug() << "I'm on the main window!";
	//dragOver = false;
	QString sDroppedText = "";
	if (event->mimeData()->hasText())//hasColor())
	{
		
		sDroppedText = event->mimeData()->text();
		//ExperimentStructureScene *expStructScene = qobject_cast<ExperimentStructureScene *>(this->scene());
		ExperimentStructureScene *expStructScene = (ExperimentStructureScene *)(this->scene());
		if (expStructScene)
			expStructScene->doSomething();
	}
	//	color = qvariant_cast<QColor>(event->mimeData()->colorData());
	//update();
}

void ExperimentGraphBlockItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton)).length() < 5) //QApplication::startDragDistance())//not enough dragged?
		return;

	QDrag *drag = new QDrag(event->widget());
	QMimeData *mime = new QMimeData;
	mime->setText("Test123");
	drag->setMimeData(mime);
	drag->exec();
	//setCursor(Qt::OpenHandCursor);
}