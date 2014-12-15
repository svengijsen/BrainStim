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

#include "experimentgraphmethodconnectionitem.h"
#include "experimentstructurescene.h"
#include <QtCore/qmath.h>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

ExperimentGraphMethodConnectionItem::ExperimentGraphMethodConnectionItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	bIsAutoMethodConnType = false;
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
	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable);
	//setFlag(QGraphicsItem::ItemIsMovable);
}

void ExperimentGraphMethodConnectionItem::setEndPoint(const QPointF &pEnd, const float &fPerpLenght)
{
	lArrowVectorLine.setP1(QPointF(0.0,0.0));
	lArrowVectorLine.setP2(pEnd);
	fPerpendicularLenght = fPerpLenght;
	prepareGeometryChange();//Needs to be called before a geometry change which affect the bounding region
	renderGraphItem();
}

QRectF ExperimentGraphMethodConnectionItem::boundingRect() const
{
	return rBoundingBox;
}

QPainterPath ExperimentGraphMethodConnectionItem::shape() const
{
	return pArrowBoundingShape;
}

int ExperimentGraphMethodConnectionItem::type() const
{
	return ExperimentManagerNameSpace::TypeMethodConnectionItem;
}

void ExperimentGraphMethodConnectionItem::renderGraphItem()
{
	float fArrLenght = qMin(lArrowVectorLine.length() * 0.4, EXPGRAPH_METHODCONNITEM_ARROW_MIN_SIZE);
	float fHalfArrLenght = fArrLenght / 2;
	pArrowDrawShape = QPainterPath();
	pArrowBoundingShape = QPainterPath();
	QMatrix matrix;

	matrix.rotate(lArrowVectorLine.angle()+90.0);	
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
		if(lArrowVectorLine.p2().y() < 0.0)
			fPerpendicularLenght = -fPerpendicularLenght;
		float fQuadLenght = fPerpendicularLenght/10.0;
		pArrowDrawShape.moveTo(fPerpendicularLenght, 0.0);
		pArrowDrawShape.lineTo(fQuadLenght, 0.0);
		if(lArrowVectorLine.p2().y() < 0.0)
		{
			pArrowDrawShape.quadTo(0.0, 0.0, 0.0, fQuadLenght);
			pArrowDrawShape.lineTo(0.0, lArrowVectorLine.length()-fQuadLenght);
		}
		else
		{
			pArrowDrawShape.quadTo(0.0, 0.0, 0.0, -fQuadLenght);
			pArrowDrawShape.lineTo(0.0, lArrowVectorLine.length()+fQuadLenght);
		}
		pArrowDrawShape.quadTo(0.0, lArrowVectorLine.length(), fQuadLenght, lArrowVectorLine.length());
		
		pArrowDrawShape.lineTo(fPerpendicularLenght, lArrowVectorLine.length());
		//Draw Arrow Head
		if(lArrowVectorLine.p2().y() < 0.0)//   fPerpendicularLenght>0)
		{
			pArrowDrawShape.lineTo(fPerpendicularLenght-fArrLenght, lArrowVectorLine.length()-fHalfArrLenght);
			pArrowDrawShape.lineTo(fPerpendicularLenght-fArrLenght, lArrowVectorLine.length()+fHalfArrLenght);	
		}
		else
		{
			pArrowDrawShape.lineTo(fPerpendicularLenght+fArrLenght, lArrowVectorLine.length()-fHalfArrLenght);
			pArrowDrawShape.lineTo(fPerpendicularLenght+fArrLenght, lArrowVectorLine.length()+fHalfArrLenght);	
		}
		pArrowDrawShape.lineTo(fPerpendicularLenght, lArrowVectorLine.length());
		
		//Return again...		
		pArrowDrawShape.lineTo(fQuadLenght, lArrowVectorLine.length());
		if(lArrowVectorLine.p2().y() < 0.0)//   if(fPerpendicularLenght<0)
		{
			
			pArrowDrawShape.quadTo(0.0, lArrowVectorLine.length(), 0.0, lArrowVectorLine.length()-fQuadLenght);
			pArrowDrawShape.lineTo(0.0, fQuadLenght);
		}
		else
		{
			pArrowDrawShape.quadTo(0.0, lArrowVectorLine.length(), 0.0, lArrowVectorLine.length()+fQuadLenght);
			pArrowDrawShape.lineTo(0.0, -fQuadLenght);
		}
		pArrowDrawShape.quadTo(0.0, 0.0, fQuadLenght, 0.0);
		
		//The Bounding Shape
		pArrowBoundingShape.addRect(0.0,-fHalfArrLenght,fPerpendicularLenght,fArrLenght);
		pArrowBoundingShape.addRect(-fHalfArrLenght,0.0,fArrLenght,lArrowVectorLine.length());
		pArrowBoundingShape.addRect(0.0,lArrowVectorLine.length()-fHalfArrLenght,fPerpendicularLenght,fArrLenght);
	}
	pArrowDrawShape.closeSubpath();
	pArrowDrawShape = matrix.map(pArrowDrawShape);	
	pArrowBoundingShape = matrix.map(pArrowBoundingShape);
	rBoundingBox = pArrowBoundingShape.boundingRect();
}

void ExperimentGraphMethodConnectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

void ExperimentGraphMethodConnectionItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(2);
	update();
	QGraphicsItem::hoverEnterEvent(event);
}

//void ExperimentGraphMethodConnectionItem::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )

void ExperimentGraphMethodConnectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(0);
	else
		this->setZValue(1);
	update();
	QGraphicsItem::hoverLeaveEvent(event);
}