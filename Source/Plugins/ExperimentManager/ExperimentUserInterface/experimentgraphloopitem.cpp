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

#include "experimentgraphloopitem.h"
#include "experimentstructurescene.h"
#include <QtCore/qmath.h>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

ExperimentGraphLoopItem::ExperimentGraphLoopItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	//bIsCurrentlyHovered = false;
	bIsAutoLoopType = false;
	pPen.setWidth(10.0);
	cSelectedColor.setRgb(0,255,0);
	cUnselectedColor.setRgb(31,6,130);
	cHoveredColor.setRgb(0,0,255);
	pBrush.setColor(cUnselectedColor);
	pBrush.setStyle(Qt::SolidPattern);
	pPen.setColor(cUnselectedColor);
	lArrowVectorLine.setP1(QPointF(0.0,0.0));
	lArrowVectorLine.setP2(QPoint(0.0,1.0));
	fPerpendicularLenght = 0.0;
	prepareGeometryChange();//Needs to be called before a geometry change which affect the bounding region
	renderGraphItem();
	//this->setToolTip("ExperimentGraphLoopItem");//Done by the owner of this class
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable);
	setAcceptDrops(true);
	//setFlag(QGraphicsItem::ItemIsMovable);
}

void ExperimentGraphLoopItem::setEndPoint(const QPointF &pEnd, const float &fPerpLenght)
{
	lArrowVectorLine.setP1(QPointF(0.0,0.0));
	lArrowVectorLine.setP2(pEnd);
	fPerpendicularLenght = fPerpLenght;
	prepareGeometryChange();//Needs to be called before a geometry change which affect the bounding region
	renderGraphItem();
}

QRectF ExperimentGraphLoopItem::boundingRect() const
{
	return rBoundingBox;
}

QPainterPath ExperimentGraphLoopItem::shape() const
{
	return pArrowBoundingShape;//pArrowDrawShape;
}

int ExperimentGraphLoopItem::type() const
{
	if(bIsAutoLoopType)
		return ExperimentManagerNameSpace::TypeAutoLoopConnectionItem;
	else
		return ExperimentManagerNameSpace::TypeLoopConnectionItem;
}

void ExperimentGraphLoopItem::renderGraphItem()
{
	float fArrLenght = qMin(lArrowVectorLine.length() * 0.4, EXPGRAPHLOOPITEM_ARROW_MIN_SIZE);
	float fHalfArrLenght = fArrLenght / 2;
	pArrowDrawShape = QPainterPath();
	pArrowBoundingShape = QPainterPath();
	QMatrix matrix;

	matrix.rotate(lArrowVectorLine.angle()+90.0);
	//matrix.scale(pArrowLine.length(),pArrowLine.length());	
	pArrowDrawShape.setFillRule(Qt::WindingFill);
	pArrowBoundingShape.setFillRule(Qt::WindingFill);
	
	if(fPerpendicularLenght == 0.0)
	{
		pArrowDrawShape.lineTo(0.0, lArrowVectorLine.length());
		//Draw Arrow Head
		pArrowDrawShape.lineTo(-fHalfArrLenght, lArrowVectorLine.length() - fArrLenght);
		pArrowDrawShape.lineTo(fHalfArrLenght, lArrowVectorLine.length() - fArrLenght);	
		pArrowDrawShape.lineTo(0.0, lArrowVectorLine.length());
		//The Bounding Shape
		pArrowBoundingShape.addRect(-fHalfArrLenght,0.0,fArrLenght,lArrowVectorLine.length());
	}
	else
	{
		float fQuadLenght = fPerpendicularLenght/10.0;
		pArrowDrawShape.moveTo(fPerpendicularLenght, 0.0);
		pArrowDrawShape.lineTo(fQuadLenght, 0.0);
		if(fPerpendicularLenght < 0)
		{
			pArrowDrawShape.quadTo(0.0, 0.0, 0.0, -fQuadLenght);
			pArrowDrawShape.lineTo(0.0, lArrowVectorLine.length()+(fQuadLenght));
		}
		else
		{
			pArrowDrawShape.quadTo(0.0, 0.0, 0.0, fQuadLenght);
			pArrowDrawShape.lineTo(0.0, lArrowVectorLine.length()-(fQuadLenght));
		}
		pArrowDrawShape.quadTo(0.0, lArrowVectorLine.length(), fQuadLenght, lArrowVectorLine.length());
		pArrowDrawShape.lineTo(fPerpendicularLenght, lArrowVectorLine.length());
		//Draw Arrow Head
		if(fPerpendicularLenght>0)
		{
			pArrowDrawShape.lineTo(fPerpendicularLenght-fArrLenght, lArrowVectorLine.length()-fHalfArrLenght);
			pArrowDrawShape.lineTo(fPerpendicularLenght-fArrLenght, lArrowVectorLine.length()+fHalfArrLenght);	
			pArrowDrawShape.lineTo(fPerpendicularLenght, lArrowVectorLine.length());
		}
		else
		{
			pArrowDrawShape.lineTo(fPerpendicularLenght+fArrLenght, lArrowVectorLine.length()-fHalfArrLenght);
			pArrowDrawShape.lineTo(fPerpendicularLenght+fArrLenght, lArrowVectorLine.length()+fHalfArrLenght);	
			pArrowDrawShape.lineTo(fPerpendicularLenght, lArrowVectorLine.length());
		}
		//Return again...	
		
		pArrowDrawShape.lineTo(fQuadLenght, lArrowVectorLine.length());
		if(fPerpendicularLenght<0)
		{
			pArrowDrawShape.quadTo(0.0, lArrowVectorLine.length(), 0.0, lArrowVectorLine.length()+fQuadLenght);
			pArrowDrawShape.lineTo(0.0, -fQuadLenght);
		}
		else
		{
			pArrowDrawShape.quadTo(0.0, lArrowVectorLine.length(), 0.0, lArrowVectorLine.length()-fQuadLenght);
			pArrowDrawShape.lineTo(0.0, fQuadLenght);
		}
		pArrowDrawShape.quadTo(0.0, 0.0, fQuadLenght, 0.0);
		
		//pArrowDrawShape.lineTo(0.0, lArrowVectorLine.length());
		//pArrowDrawShape.lineTo(0.0, 0.0);
		//pArrowDrawShape.lineTo(fPerpendicularLenght, 0.0);

		//The Bounding Shape
		pArrowBoundingShape.addRect(0.0,-fHalfArrLenght,fPerpendicularLenght,fArrLenght);
		pArrowBoundingShape.addRect(-fHalfArrLenght,0.0,fArrLenght,lArrowVectorLine.length());
		pArrowBoundingShape.addRect(0.0,lArrowVectorLine.length()-fHalfArrLenght,fPerpendicularLenght,fArrLenght);
	}

	pArrowDrawShape.closeSubpath();
	pArrowDrawShape = matrix.map(pArrowDrawShape);	
	pArrowBoundingShape = matrix.map(pArrowBoundingShape);
	rBoundingBox = pArrowBoundingShape.boundingRect();//pArrowDrawShape.boundingRect();
}

void ExperimentGraphLoopItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{	
	Q_UNUSED(widget);
	painter->setRenderHint(QPainter::Antialiasing);
	if(option->state & QStyle::State_Selected)
	{
		pBrush.setColor(cSelectedColor);
		pPen.setColor(cSelectedColor);
	}
	else if(option->state & QStyle::State_MouseOver)
	{
		pBrush.setColor(cHoveredColor);
		pPen.setColor(cHoveredColor);
	}
	else
	{
		pBrush.setColor(cUnselectedColor);
		pPen.setColor(cUnselectedColor);
	}
	painter->setPen(pPen);
	painter->setBrush(pBrush);
	//renderGraphItem();
	painter->drawPath(pArrowDrawShape);
		
	//if(bIsCurrentlyHovered) 
	//if (option->state & QStyle::State_MouseOver)	
	//{
	//	painter->setBrush(Qt::NoBrush); painter->setPen(QColor(255,0,0)); painter->drawPath(pArrowBoundingShape);
	//	painter->setBrush(Qt::NoBrush); painter->setPen(QColor(255,0,0)); painter->drawRect(rBoundingBox); 
	//	painter->setPen(Qt::DotLine); painter->setPen(QColor(0,0,255)); painter->drawPath(pShape);
	//}
}

void ExperimentGraphLoopItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(2);
	update();
	QGraphicsItem::hoverEnterEvent(event);
}

//void ExperimentGraphLoopItem::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )

void ExperimentGraphLoopItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(0);
	else
		this->setZValue(1);
	update();
	QGraphicsItem::hoverLeaveEvent(event);
}

void ExperimentGraphLoopItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	Q_UNUSED(event);
	qDebug() << __FUNCTION__;
	return;
	
	//if (event->mimeData()->hasColor()) {
	//	event->setAccepted(true);
	//	dragOver = true;
	//	update();
	//}
	//else {
	//	event->setAccepted(false);
	//}
}

void ExperimentGraphLoopItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
	Q_UNUSED(event);
	qDebug() << __FUNCTION__;
	return;
	//dragOver = false;
	//update();
}

void ExperimentGraphLoopItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	qDebug() << __FUNCTION__;
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