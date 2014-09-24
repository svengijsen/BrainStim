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

#include "experimentgraphobjectitem.h"
#include "experimentstructurescene.h"
#include <QStyleOptionGraphicsItem>

ExperimentGraphObjectItem::ExperimentGraphObjectItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	fCurrent.setFamily("Times");
	pHeaderPen.setWidth(2.0);
	cHeaderCaption.setRgb(63,72,204);
	cHeaderUnselectedColor.setRgb(195,195,195);
	cHeaderSelectedColor.setRgb(0,228,0);
	cHeaderHoveredColor.setRgb(239,228,176);
	pHeaderPen.setColor(cHeaderCaption);
	pBrush.setColor(cHeaderUnselectedColor);
	pBrush.setStyle(Qt::SolidPattern);
	pObjectOutlinePen.setStyle(Qt::DashLine);
	pObjectOutlinePen.setColor(Qt::black);
	pObjectOutlinePen.setWidth(3.0);

	nObjectWidth = EXPGRAPHOBJECTITEM_OBJECT_WIDTH;
	nHalfObjectWidth = nObjectWidth/2;
	nObjectHeight = EXPGRAPHOBJECTITEM_OBJECT_HEIGHT;
	nHalfObjectHeight = nObjectHeight/2;
	nObjectDistance = EXPGRAPHOBJECTITEM_OBJECT_DISTANCE;
	nHalfObjectDistance = nObjectDistance/2;
	rAdditionalMethodConnectionsHeight = 0;
	rAdditionalInitializationsHeight = 0;
	rAdditionalFinalizationsHeight = 0;
	rTotalHeight = 0;

	prepareGeometryChange();//Needs to be called before a geometry change which affect the bounding region
	renderGraphItem();

	setAcceptHoverEvents(true);
	setFlag(QGraphicsItem::ItemIsSelectable);
}

void ExperimentGraphObjectItem::setCaption(const QString &sName)
{
	sCurrentText = sName;
	update();
}

int ExperimentGraphObjectItem::type() const
{
	return ExperimentStructureItemType::TypeObjectItem;
}

QPainterPath ExperimentGraphObjectItem::shape() const
{
	return sBoundingBoxPath;//pShapePath;
}

QRectF ExperimentGraphObjectItem::boundingRect() const
{
	return rBoundingBox;
}

void ExperimentGraphObjectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(2);
	update();
}

//void QGraphicsItem::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )

void ExperimentGraphObjectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if(this->isSelected() == false)
		this->setZValue(0);
	else
		this->setZValue(1);
	update();
}

void ExperimentGraphObjectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);

	painter->setRenderHint(QPainter::Antialiasing);
	if(option->state & QStyle::State_Selected)
	{
		fCurrent.setBold(true);
		pBrush.setColor(cHeaderSelectedColor);
		pBrush.setColor(cHeaderSelectedColor);
		pHeaderPen.setColor(cHeaderSelectedColor);
		pObjectOutlinePen.setColor(Qt::darkGreen);
	}
	else if(option->state & QStyle::State_MouseOver)
	{
		fCurrent.setBold(true);
		pBrush.setColor(cHeaderHoveredColor);
		pBrush.setColor(cHeaderHoveredColor);
		pHeaderPen.setColor(cHeaderHoveredColor);
		pObjectOutlinePen.setColor(Qt::darkYellow);
	}
	else
	{
		fCurrent.setBold(false);
		pBrush.setColor(cHeaderUnselectedColor);
		pBrush.setColor(cHeaderUnselectedColor);
		pHeaderPen.setColor(cHeaderUnselectedColor);
		pObjectOutlinePen.setColor(Qt::black);
	}
	//Header
	painter->setPen(pHeaderPen);
	painter->setBrush(pBrush);
	painter->drawPath(pHeaderShapePath);

	//Header Text
	fCurrent.setBold(true);
	fCurrent.setItalic(false);
	painter->setFont(fCurrent);
	pHeaderPen.setColor(cHeaderCaption);
	painter->setPen(pHeaderPen);
	painter->drawText(rectInnerObject,Qt::AlignLeft | Qt::AlignVCenter, " " + sCurrentText);

	painter->setBrush(Qt::NoBrush);
	painter->setFont(fCurrent);

	//Outlines
	painter->setPen(pObjectOutlinePen);
	if(vInitializationsRectList.isEmpty() == false)
		painter->drawRect(rInitializationBox);
	if(vSignalAndSlotsRectList.isEmpty() == false)
		painter->drawRect(rConnectionsBox);
	if(vFinalizationsRectList.isEmpty() == false)
		painter->drawRect(rFinalizationBox);
	painter->drawRect(rBoundingBox);

	painter->setPen(pHeaderPen);

	//INITIALIZATIONS
	if(vInitializationsRectList.isEmpty() == false)
	{
		painter->drawText(rInitializationBox, Qt::AlignHCenter | Qt::AlignTop, "INITIALIZATIONS:");
		painter->drawRects(vInitializationsRectList);
		fCurrent.setBold(false);
		fCurrent.setItalic(true);
		int nInitializationsCounter = 0;
		foreach(QString sSignature, lOrderedInitializationsList)
		{
			painter->drawText(vInitializationsRectList.at(nInitializationsCounter),Qt::AlignLeft | Qt::AlignVCenter," " + sSignature);
			nInitializationsCounter++;
		}
	}

	//METHODS
	if(vSignalAndSlotsRectList.isEmpty() == false)
	{
		painter->drawText(rConnectionsBox, Qt::AlignHCenter | Qt::AlignTop, "SIGNAL/SLOTS:");
		painter->drawRects(vSignalAndSlotsRectList);
		fCurrent.setBold(false);
		fCurrent.setItalic(true);
		int nMethodCounter = 0;
		foreach(QString sSignature, lOrderedSignalSignatureList)
		{
			painter->drawText(vSignalAndSlotsRectList.at(nMethodCounter),Qt::AlignLeft | Qt::AlignVCenter," " + sSignature);
			nMethodCounter++;
		}
		foreach(QString sSignature, lOrderedSlotSignatureList)
		{
			painter->drawText(vSignalAndSlotsRectList.at(nMethodCounter),Qt::AlignLeft | Qt::AlignVCenter," " + sSignature);
			nMethodCounter++;
		}
	}

	//FINALIZATIONS
	if(vFinalizationsRectList.isEmpty() == false)
	{
		painter->drawText(rFinalizationBox, Qt::AlignHCenter | Qt::AlignTop, "FINALIZATIONS:");
		painter->drawRects(vFinalizationsRectList);
		fCurrent.setBold(false);
		fCurrent.setItalic(true);
		int nFinalizationsCounter = 0;
		foreach(QString sSignature, lOrderedFinalizationsList)
		{
			painter->drawText(vFinalizationsRectList.at(nFinalizationsCounter),Qt::AlignLeft | Qt::AlignVCenter," " + sSignature);
			nFinalizationsCounter++;
		}
	}
}

void ExperimentGraphObjectItem::renderGraphItem()
{
	pHeaderShapePath = QPainterPath();
	sBoundingBoxPath = QPainterPath();
	rectInnerObject = QRectF(-nHalfObjectWidth,-nHalfObjectHeight,nObjectWidth,nObjectHeight);	
	fCurrent.setPointSizeF(rectInnerObject.height()/6);	
	rBoundingBox.setCoords(-nHalfObjectWidth,-nHalfObjectHeight,nHalfObjectWidth,nHalfObjectHeight+rTotalHeight+EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE);
	sBoundingBoxPath.addRect(rBoundingBox);
	pHeaderShapePath.addRoundedRect(rectInnerObject,50,50);
}

void ExperimentGraphObjectItem::setMethods(const QList<strcMethodInfo> &lMethodSignatureList)
{
	rAdditionalInitializationsHeight = 0;
	rAdditionalMethodConnectionsHeight = 0;
	rAdditionalFinalizationsHeight = 0;

	vInitializationsRectList.clear();
	lOrderedInitializationsList.clear();
	vSignalAndSlotsRectList.clear();
	lOrderedSignalSignatureList.clear();
	lOrderedSlotSignatureList.clear();
	vFinalizationsRectList.clear();
	lOrderedFinalizationsList.clear();
	QRectF tmpMethodRect;

	if(lMethodSignatureList.isEmpty() == false)
	{
		//INITIALIZATIONS//
		rInitializationBox.setCoords(rectInnerObject.left()+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2), rectInnerObject.bottom()+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2),rectInnerObject.right()-(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2),rectInnerObject.bottom());
		int nInitCounter = 0;
		for(int i=0;i<lMethodSignatureList.count();i++)
		{
			if(lMethodSignatureList.at(i).bIsInitialization)
			{
				rAdditionalInitializationsHeight = rInitializationBox.bottom()+EXPGRAPHOBJECTITEM_HEADER_HEIGHT+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2) + (nInitCounter*(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
				tmpMethodRect.setCoords(rectInnerObject.left()+EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE, rAdditionalInitializationsHeight,rectInnerObject.right()-EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE,rAdditionalInitializationsHeight+EXPGRAPHOBJECTITEM_METHOD_HEIGHT);
				vInitializationsRectList.append(tmpMethodRect);
				//if(lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
				//	lOrderedSignalSignatureList.append(lMethodSignatureList.at(i).sMethodSignature);
				//else if(lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
				//	lOrderedSlotSignatureList.append(lMethodSignatureList.at(i).sMethodSignature);
				lOrderedInitializationsList.append(lMethodSignatureList.at(i).sMethodSignature);
				nInitCounter++;
			}
		}
		if(nInitCounter > 0)
		{
			rInitializationBox.setBottom(rAdditionalInitializationsHeight + EXPGRAPHOBJECTITEM_METHOD_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
			lOrderedInitializationsList.sort();
			rTotalHeight = rAdditionalInitializationsHeight;
		}

		//METHODS//
		rConnectionsBox.setCoords(rInitializationBox.left(), rInitializationBox.bottom()+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2),rInitializationBox.right(),rInitializationBox.bottom()+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
		int nMethodCounter = 0;
		for(int i=0;i<lMethodSignatureList.count();i++)
		{
			if((lMethodSignatureList.at(i).bIsInitialization==false)&&(lMethodSignatureList.at(i).bIsFinalization==false))
			{
				QRectF tmpMethodRect;
				rAdditionalMethodConnectionsHeight = rConnectionsBox.bottom() + EXPGRAPHOBJECTITEM_HEADER_HEIGHT + (nMethodCounter*(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
				tmpMethodRect.setCoords(rInitializationBox.left()+EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE, rAdditionalMethodConnectionsHeight,rInitializationBox.right()-EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE,rAdditionalMethodConnectionsHeight+EXPGRAPHOBJECTITEM_METHOD_HEIGHT);
				vSignalAndSlotsRectList.append(tmpMethodRect);
				if(lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
					lOrderedSignalSignatureList.append(lMethodSignatureList.at(i).sMethodSignature);
				else if(lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
					lOrderedSlotSignatureList.append(lMethodSignatureList.at(i).sMethodSignature);
				nMethodCounter++;
			}
		}
		if(nMethodCounter>0)
		{
			rConnectionsBox.setBottom(rAdditionalMethodConnectionsHeight + EXPGRAPHOBJECTITEM_METHOD_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
			lOrderedSignalSignatureList.sort();
			lOrderedSlotSignatureList.sort();
			rTotalHeight = rAdditionalMethodConnectionsHeight;
		}

		//FINALIZATIONS//
		rFinalizationBox.setCoords(rConnectionsBox.left(), rConnectionsBox.bottom()+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2),rConnectionsBox.right(),rConnectionsBox.bottom());
		int nFinitCounter = 0;
		for(int i=0;i<lMethodSignatureList.count();i++)
		{
			if(lMethodSignatureList.at(i).bIsFinalization)
			{
				rAdditionalFinalizationsHeight = rConnectionsBox.bottom()+EXPGRAPHOBJECTITEM_HEADER_HEIGHT+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2) + (nFinitCounter*(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
				tmpMethodRect.setCoords(rectInnerObject.left()+EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE,rAdditionalFinalizationsHeight,rectInnerObject.right()-EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE,rAdditionalFinalizationsHeight+EXPGRAPHOBJECTITEM_METHOD_HEIGHT);
				vFinalizationsRectList.append(tmpMethodRect);
				//if(lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
				//	lOrderedSignalSignatureList.append(lMethodSignatureList.at(i).sMethodSignature);
				//else if(lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
				//	lOrderedSlotSignatureList.append(lMethodSignatureList.at(i).sMethodSignature);
				lOrderedFinalizationsList.append(lMethodSignatureList.at(i).sMethodSignature);
				nFinitCounter++;
			}
		}
		if(nFinitCounter > 0)
		{
			rFinalizationBox.setBottom(rAdditionalFinalizationsHeight + EXPGRAPHOBJECTITEM_METHOD_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
			lOrderedFinalizationsList.sort();
			rTotalHeight = rAdditionalFinalizationsHeight;
		}
	}

	prepareGeometryChange();//Needs to be called before a geometry change which affect the bounding region
	renderGraphItem();
}

int ExperimentGraphObjectItem::getMethodLocationIndex(const QString &sSignature, const ExperimentStructuresNameSpace::MethodType &mMethodType)
{
	if(mMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SIGNAL)
	{
		return lOrderedSignalSignatureList.indexOf(sSignature);
	}
	else if(mMethodType == ExperimentStructuresNameSpace::METHOD_TYPE_SLOT)
	{
		int nTmpSlotIndex = lOrderedSlotSignatureList.indexOf(sSignature);
		if(nTmpSlotIndex >= 0)
			return lOrderedSignalSignatureList.count() + lOrderedSlotSignatureList.indexOf(sSignature);
	}
	return -1;
}

int ExperimentGraphObjectItem::getMethodLocationPosition(const QString &sSignature, const ExperimentStructuresNameSpace::MethodType &mMethodType)
{
	int nIndex = getMethodLocationIndex(sSignature,mMethodType)+1;
	if(nIndex>0)
	{
		return this->pos().y() + rAdditionalInitializationsHeight + EXPGRAPHOBJECTITEM_HEADER_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_HEIGHT / 2) + (nIndex * (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
	}
	return -1;	
}