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

#include "experimentgraphobjectitem.h"
#include "experimentstructurescene.h"
#include <QStyleOptionGraphicsItem>

ExperimentGraphSubObjectTextItem::ExperimentGraphSubObjectTextItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	nItemStateFlags = 0;
	nItemID = -1;
	sItemText = "<undefined>";
	eGraphItemType = ExperimentManagerNameSpace::TypeUndefined;
}

void ExperimentGraphSubObjectTextItem::setGeometry(const QRectF &rectGeometry)
{
	rBoundingBox = rectGeometry;
	sBoundingBoxPath = QPainterPath();
	sBoundingBoxPath.addRect(rectGeometry);
	update();
}

void ExperimentGraphSubObjectTextItem::setItemText(const QString &sText)
{
	sItemText = sText;
	update();
}

QPainterPath ExperimentGraphSubObjectTextItem::shape() const
{
	return sBoundingBoxPath;
}

QRectF ExperimentGraphSubObjectTextItem::boundingRect() const
{
	qreal penWidth = 1;
	return QRectF(rBoundingBox.x() - penWidth / 2, rBoundingBox.y() - penWidth / 2, rBoundingBox.width() + penWidth, rBoundingBox.height() + penWidth);
}

void ExperimentGraphSubObjectTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if (this->isSelected() == false)
		this->setZValue(2);
	update();
}

//void QGraphicsItem::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )

void ExperimentGraphSubObjectTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if (this->isSelected() == false)
		this->setZValue(0);
	else
		this->setZValue(1);
	update();
}

void ExperimentGraphSubObjectTextItem::setGraphItemType(ExperimentManagerNameSpace::ExperimentStructureItemType graphItemType)
{
	eGraphItemType = graphItemType;
}

int ExperimentGraphSubObjectTextItem::type() const 
{ 
	return eGraphItemType;
}

void ExperimentGraphSubObjectTextItem::setActiveStates(int nFlags)
{//Use StateFlag
	nItemStateFlags = nFlags;
	if(nItemStateFlags & QStyle::State_Selected)
		setFlag(QGraphicsItem::ItemIsSelectable);
	else
		setFlag(QGraphicsItem::ItemIsSelectable, false);
	if (nItemStateFlags & QStyle::State_MouseOver)
		setAcceptHoverEvents(true);
	else
		setAcceptHoverEvents(false);
}

void ExperimentGraphSubObjectTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (rBoundingBox.height() == 0)
		return;
	Q_UNUSED(widget);
	if ((option->state & QStyle::State_Selected) && (nItemStateFlags & QStyle::State_Selected))
	{
		fCurrent.setBold(true);
		fCurrent.setItalic(true);
		pBrush.setColor(Qt::green);
		pBrush.setStyle(Qt::SolidPattern);
		pPen.setColor(Qt::green);
	}
	else if ((option->state & QStyle::State_MouseOver) && (nItemStateFlags & QStyle::State_MouseOver))
	{
		fCurrent.setBold(true);
		fCurrent.setItalic(false);
		pBrush.setColor(Qt::yellow);
		pBrush.setStyle(Qt::SolidPattern);
		pPen.setColor(Qt::yellow);
	}
	else
	{
		fCurrent.setBold(false);
		fCurrent.setItalic(false);
		pBrush.setColor(Qt::blue);
		//pBrush.setStyle(Qt::NoBrush);
		pBrush.setColor(Qt::white);
		pBrush.setStyle(Qt::SolidPattern);
		pPen.setColor(Qt::blue);
	}

	////Outer-Box
	painter->setPen(pPen);
	painter->setBrush(pBrush);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->drawPath(sBoundingBoxPath);

	////Header-Text
	QRectF rectInnerObject = QRectF(rBoundingBox.left(), rBoundingBox.top(), rBoundingBox.width(), rBoundingBox.height());
	fCurrent.setPointSizeF(rectInnerObject.height() / 3);
	pBrush.setStyle(Qt::NoBrush);
	painter->setBrush(pBrush);
	if (eGraphItemType == ExperimentManagerNameSpace::TypeParametersFirstBlock)
	{
		fCurrent.setItalic(true);
		painter->setFont(fCurrent);
		pPen.setColor(Qt::darkRed);
		painter->setPen(pPen);
		painter->drawText(rectInnerObject, Qt::AlignCenter, " " + sItemText);
	}
	else
	{
		fCurrent.setItalic(false);
		painter->setFont(fCurrent);
		pPen.setColor(Qt::darkBlue);
		painter->setPen(pPen);
		painter->drawText(rectInnerObject, Qt::AlignLeft | Qt::AlignVCenter, " " + sItemText);
	}
}

//QVariant ExperimentGraphSubObjectTextItem::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//	if (change == QGraphicsItem::ItemSelectedChange)
//	{
//		if (value == true)
//		{
//			// do stuff if selected
//			int a = 99;
//		}
//		else
//		{
//			int b = 99;
//			// do stuff if not selected
//		}
//	}
//	return QGraphicsItem::itemChange(change, value);
//}

void ExperimentGraphSubObjectTextItem::renderGraphItem()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////

ExperimentGraphSubObjectItem::ExperimentGraphSubObjectItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	eGraphItemType = ExperimentManagerNameSpace::TypeUndefined;
	sHeaderText = "<undefined>";
	nItemStateFlags = 0;
}

void ExperimentGraphSubObjectItem::setGeometry(const QRectF &rectGeometry)
{
	rBoundingBox = rectGeometry;
	sBoundingBoxPath = QPainterPath();
	sBoundingBoxPath.addRoundedRect(rectGeometry, 5, 5);
	update();
}

void ExperimentGraphSubObjectItem::setHeaderText(const QString &sText)
{
	sHeaderText = sText;
	update();
}

void ExperimentGraphSubObjectItem::setGraphItemType(ExperimentManagerNameSpace::ExperimentStructureItemType graphItemType)
{
	eGraphItemType = graphItemType;
}

int ExperimentGraphSubObjectItem::type() const
{
	return eGraphItemType;
}

void ExperimentGraphSubObjectItem::setSubItems(const QMap<int, QString> &mapSubItemIDsToStringList)
{
	mapSubItemIDToStringList = mapSubItemIDsToStringList;
	foreach(ExperimentGraphSubObjectTextItem* tmpSubTextItem, mExpGraphSubObjectNameToTextItems)
	{
		if (tmpSubTextItem)
			delete tmpSubTextItem;
	}
	mExpGraphSubObjectNameToTextItems.clear();
	QMapIterator<int, QString> iter(mapSubItemIDToStringList);
	while (iter.hasNext())
	{
		iter.next();
		ExperimentGraphSubObjectTextItem* tmpNewSubTextItem = new ExperimentGraphSubObjectTextItem(this);
		tmpNewSubTextItem->setItemText(iter.value());
		tmpNewSubTextItem->setItemID(iter.key());
		int nActiveStates;
		if (eGraphItemType == ExperimentManagerNameSpace::TypeGroupConnectionsItem)
		{
			tmpNewSubTextItem->setGraphItemType(ExperimentManagerNameSpace::TypeMethodConnectionItem);
			nActiveStates = QStyle::State_MouseOver;
		}
		else if (eGraphItemType == ExperimentManagerNameSpace::TypeGroupInitializationsItem)
		{
			if (iter.key() == -1)
			{
				tmpNewSubTextItem->setGraphItemType(ExperimentManagerNameSpace::TypeParametersFirstBlock);
				nActiveStates = QStyle::State_Selected + QStyle::State_MouseOver;
			}
			else
			{
				tmpNewSubTextItem->setGraphItemType(ExperimentManagerNameSpace::TypeObjectInitializationItem);
				nActiveStates = QStyle::State_Selected + QStyle::State_MouseOver;
			}
			if (iter.value().endsWith("()") == false)
			{
				tmpNewSubTextItem->setItemText(iter.value() + "()");
			}
		}
		else if (eGraphItemType == ExperimentManagerNameSpace::TypeGroupFinalizationsItem)
		{
			tmpNewSubTextItem->setGraphItemType(ExperimentManagerNameSpace::TypeObjectFinalizationItem);
			nActiveStates = QStyle::State_Selected + QStyle::State_MouseOver;
			if (iter.value().endsWith("()") == false)
			{
				tmpNewSubTextItem->setItemText(iter.value() + "()");
			}
		}
		else if (eGraphItemType == ExperimentManagerNameSpace::TypeUndefined)
		{
			nActiveStates = 0;
		}
		else
		{
			nActiveStates = QStyle::State_Selected + QStyle::State_MouseOver;
		}
		tmpNewSubTextItem->setActiveStates(nActiveStates);
		mExpGraphSubObjectNameToTextItems.insertMulti(iter.value(), tmpNewSubTextItem);
	}
	update();
}

QPainterPath ExperimentGraphSubObjectItem::shape() const
{
	return sBoundingBoxPath;
}

QRectF ExperimentGraphSubObjectItem::boundingRect() const
{
	qreal penWidth = 1;
	return QRectF(rBoundingBox.x() - penWidth / 2, rBoundingBox.y() - penWidth / 2, rBoundingBox.width() + penWidth, rBoundingBox.height() + penWidth);
}

void ExperimentGraphSubObjectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if (this->isSelected() == false)
		this->setZValue(2);
	update();
}

//void QGraphicsItem::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )

void ExperimentGraphSubObjectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	Q_UNUSED(event);
	if (this->isSelected() == false)
		this->setZValue(0);
	else
		this->setZValue(1);
	update();
}

void ExperimentGraphSubObjectItem::setActiveStates(int nFlags)
{//Use StateFlag
	nItemStateFlags = nFlags;
	if (nItemStateFlags & QStyle::State_Selected)
		setFlag(QGraphicsItem::ItemIsSelectable);
	else
		setFlag(QGraphicsItem::ItemIsSelectable, false);
	if (nItemStateFlags & QStyle::State_MouseOver)
		setAcceptHoverEvents(true);
	else
		setAcceptHoverEvents(false);
}

void ExperimentGraphSubObjectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(widget);
	if ((option->state & QStyle::State_Selected) && (nItemStateFlags & QStyle::State_Selected))
	{
		fCurrent.setBold(true);
		pBrush.setColor(Qt::green);
		pBrush.setStyle(Qt::SolidPattern);
		pPen.setColor(Qt::green);
	}
	else if ((option->state & QStyle::State_MouseOver) && (nItemStateFlags & QStyle::State_MouseOver))
	{
		fCurrent.setBold(true);
		pBrush.setColor(Qt::lightGray);
		pBrush.setStyle(Qt::SolidPattern);
		pPen.setColor(Qt::yellow);
	}
	else
	{
		fCurrent.setBold(false);
		pBrush.setColor(Qt::blue);
		//pBrush.setStyle(Qt::NoBrush);
		pBrush.setColor(QColor(233,233,233));
		pBrush.setStyle(Qt::SolidPattern);
		pPen.setColor(Qt::blue);
	}

	////Outer-Box
	painter->setPen(pPen);
	painter->setBrush(pBrush);
	painter->setRenderHint(QPainter::Antialiasing);
	painter->drawPath(sBoundingBoxPath);

	////Header-Text
	QRectF rectInnerObject = QRectF(rBoundingBox.left(), rBoundingBox.top(), rBoundingBox.width(), rBoundingBox.height() / (1 + mapSubItemIDToStringList.count()));
	fCurrent.setPointSizeF(rectInnerObject.height()/3);
	fCurrent.setItalic(false);
	painter->setFont(fCurrent);
	pPen.setColor(Qt::darkBlue);
	painter->setPen(pPen);
	pBrush.setStyle(Qt::NoBrush);
	painter->setBrush(pBrush);
	painter->drawText(rectInnerObject, Qt::AlignCenter | Qt::AlignVCenter, " " + sHeaderText);//Qt::AlignHCenter | Qt::AlignTop, "FINALIZATIONS:"
	
	////Sub-Items
	int nItemHeight = rBoundingBox.height() / (mapSubItemIDToStringList.count() + 1);
	if (mapSubItemIDToStringList.count() == mExpGraphSubObjectNameToTextItems.count())
	{
		QMapIterator<int, QString> iterSub(mapSubItemIDToStringList);
		while (iterSub.hasNext())
		{
			iterSub.next();
			rectInnerObject.setTop(rectInnerObject.top() + nItemHeight);
			rectInnerObject.setBottom(rectInnerObject.top() + nItemHeight);
			ExperimentGraphSubObjectTextItem *tmpExpGraphSubTextItem = mExpGraphSubObjectNameToTextItems.value(iterSub.value(), NULL); //mExpGraphSubObjectNameToTextItems.value(lSubItemTextList[i]);
			if (tmpExpGraphSubTextItem)
				tmpExpGraphSubTextItem->setGeometry(rectInnerObject);
		}
	}
}

//QVariant ExperimentGraphSubObjectItem::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//	if (change == QGraphicsItem::ItemSelectedChange)
//	{
//		if (value == true)
//		{
//			// do stuff if selected
//			int a = 99;
//		}
//		else
//		{
//			int b = 99;
//			// do stuff if not selected
//		}
//	}
//	return QGraphicsItem::itemChange(change, value);
//}

void ExperimentGraphSubObjectItem::renderGraphItem()
{

}


//////////////////////////////////////////////////////////////////////////////////////////////////

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

	subGraphItemInitializations = NULL;
	subGraphItemFinalizations = NULL;
	subGraphItemSignalSlots = NULL;
	pIcon = NULL;
}

ExperimentGraphObjectItem::~ExperimentGraphObjectItem()
{
	if (pIcon)
	{
		delete pIcon;
		pIcon = NULL;
	}
}

void ExperimentGraphObjectItem::setIcon(const QIcon &constIcon)
{
	if (pIcon)
		delete pIcon;
	pIcon = new QIcon(constIcon);
	update();
}

void ExperimentGraphObjectItem::setCaption(const QString &sName)
{
	sCurrentText = sName;
	update();
}

int ExperimentGraphObjectItem::type() const
{ 
	return ExperimentManagerNameSpace::TypeObjectItem;
}

QPainterPath ExperimentGraphObjectItem::shape() const
{
	return sBoundingBoxPath;
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
	if (pIcon)
	{
		int nSize = rectInnerObject.height() / 2;
		painter->drawPixmap(QRect(rectInnerObject.right() - (1.5 * nSize), rectInnerObject.top() + (0.5 * nSize), nSize, nSize), pIcon->pixmap(nSize, nSize));
	}
	painter->setBrush(Qt::NoBrush);
	painter->setFont(fCurrent);

	//Outlines
	painter->setPen(pObjectOutlinePen);
	if (mapInitializationsIDToSignatureList.isEmpty() == false)
	{
		if (subGraphItemInitializations == NULL)
		{
			subGraphItemInitializations = new ExperimentGraphSubObjectItem(this);
			subGraphItemInitializations->setActiveStates(QStyle::State_MouseOver);//QStyle::State_Selected + 
			subGraphItemInitializations->setGeometry(rInitializationBox);
			subGraphItemInitializations->setHeaderText("INITIALIZATIONS:");
			subGraphItemInitializations->setGraphItemType(ExperimentManagerNameSpace::TypeGroupInitializationsItem);
			//if (mapInitializationsIDToSignatureList.isEmpty() == false)
				subGraphItemInitializations->setSubItems(mapInitializationsIDToSignatureList);
		}
	}

	//Lets combine the two lists because an ID is not needed and a combined list generates non-unique IDs a ID with value -1 is always used here...
	QMap<int,QString> mapConnectionsSignatureList;
	QMapIterator<int, QString> iter1(mapSignalLocationIndexToSignatureList);
	while (iter1.hasNext())
	{
		iter1.next();
		mapConnectionsSignatureList.insertMulti(iter1.key(), iter1.value());
	}
	QMapIterator<int, QString> iter2(mapSlotLocationIndexToSignatureList);
	while (iter2.hasNext())
	{
		iter2.next();
		mapConnectionsSignatureList.insertMulti(iter2.key(), iter2.value());
	}

	if (mapConnectionsSignatureList.isEmpty() == false)
	{
		if (subGraphItemSignalSlots == NULL)
		{
			subGraphItemSignalSlots = new ExperimentGraphSubObjectItem(this);
			subGraphItemSignalSlots->setActiveStates(QStyle::State_MouseOver);
			subGraphItemSignalSlots->setGeometry(rConnectionsBox);
			subGraphItemSignalSlots->setHeaderText("CONNECTIONS:");
			subGraphItemSignalSlots->setGraphItemType(ExperimentManagerNameSpace::TypeGroupConnectionsItem);
			subGraphItemSignalSlots->setSubItems(mapConnectionsSignatureList);
		}
	}
	if (mapFinalizationsIDToSignatureList.isEmpty() == false)
	{
		if (subGraphItemFinalizations == NULL)
		{
			subGraphItemFinalizations = new ExperimentGraphSubObjectItem(this);
			subGraphItemFinalizations->setActiveStates(QStyle::State_MouseOver);
			subGraphItemFinalizations->setGeometry(rFinalizationBox);
			subGraphItemFinalizations->setHeaderText("FINALIZATIONS:");
			subGraphItemFinalizations->setGraphItemType(ExperimentManagerNameSpace::TypeGroupFinalizationsItem);
			subGraphItemFinalizations->setSubItems(mapFinalizationsIDToSignatureList);
		}
	}
	painter->drawRect(rBoundingBox);
	painter->setPen(pHeaderPen);
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

	if (subGraphItemInitializations)
	{
		delete subGraphItemInitializations;
		subGraphItemInitializations = NULL;
	}
	if (subGraphItemFinalizations)
	{
		delete subGraphItemFinalizations;
		subGraphItemFinalizations = NULL;
	}
	if (subGraphItemSignalSlots)
	{
		delete subGraphItemSignalSlots;
		subGraphItemSignalSlots = NULL;
	}

	mapInitializationsIDToSignatureList.clear();
	mapSignalIDToSignatureList.clear();
	mapSlotIDToSignatureList.clear();
	mapFinalizationsIDToSignatureList.clear();
	mapInitializationsLocationIndexToSignatureList.clear();
	mapSignalLocationIndexToSignatureList.clear();
	mapSlotLocationIndexToSignatureList.clear();
	mapFinalizationsLocationIndexToSignatureList.clear();

	//INITIALIZATIONS//
	rInitializationBox.setCoords(rectInnerObject.left() + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE / 2), rectInnerObject.bottom() + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE / 2), rectInnerObject.right() - (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE / 2), rectInnerObject.bottom());
	int nInitCounter = 0;

	//rAdditionalInitializationsHeight = rInitializationBox.bottom() + EXPGRAPHOBJECTITEM_HEADER_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE / 2) + (nInitCounter*(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
	//mapInitializationsIDToSignatureList.insert(-1, "<PARAMETERS>");
	//nInitCounter++;

	if(lMethodSignatureList.isEmpty() == false)
	{
		for(int i=0;i<lMethodSignatureList.count();i++)
		{
			if(lMethodSignatureList.at(i).bIsInitialization)
			{
				rAdditionalInitializationsHeight = rInitializationBox.bottom()+EXPGRAPHOBJECTITEM_HEADER_HEIGHT+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2) + (nInitCounter*(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
				mapInitializationsIDToSignatureList.insert(lMethodSignatureList.at(i).nMethodID, lMethodSignatureList.at(i).sMethodSignature);
				nInitCounter++;
			}
		}
		if(nInitCounter > 0)
		{
			rInitializationBox.setBottom(rAdditionalInitializationsHeight + EXPGRAPHOBJECTITEM_METHOD_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
			rTotalHeight = rAdditionalInitializationsHeight;
		}

		//METHODS//
		rConnectionsBox.setCoords(rInitializationBox.left(), rInitializationBox.bottom()+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2),rInitializationBox.right(),rInitializationBox.bottom()+(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
		int nMethodCounter = 0;
		//int nSignalCounter = 0;
		//int nSlotCounter = 0;
		for(int i=0;i<lMethodSignatureList.count();i++)
		{
			if((lMethodSignatureList.at(i).bIsInitialization==false)&&(lMethodSignatureList.at(i).bIsFinalization==false))
			{
				rAdditionalMethodConnectionsHeight = rConnectionsBox.bottom() + EXPGRAPHOBJECTITEM_HEADER_HEIGHT + (nMethodCounter*(EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
				if (lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentManagerNameSpace::TypeObjectSignalMethodItem)
				{
					mapSignalIDToSignatureList.insert(lMethodSignatureList.at(i).nMethodID, lMethodSignatureList.at(i).sMethodSignature);
					mapSignalLocationIndexToSignatureList.insert(mapSignalLocationIndexToSignatureList.count(), lMethodSignatureList.at(i).sMethodSignature);
					//nSignalCounter++;
				}
				else if (lMethodSignatureList.at(i).mExperimentVisualizerMethodType == ExperimentManagerNameSpace::TypeObjectSlotMethodItem)
				{
					mapSlotIDToSignatureList.insert(lMethodSignatureList.at(i).nMethodID, lMethodSignatureList.at(i).sMethodSignature);
					mapSlotLocationIndexToSignatureList.insert(mapSlotLocationIndexToSignatureList.count(), lMethodSignatureList.at(i).sMethodSignature);
					//nSlotCounter++;
				}
				nMethodCounter++;
			}
		}
		if(nMethodCounter>0)
		{
			rConnectionsBox.setBottom(rAdditionalMethodConnectionsHeight + EXPGRAPHOBJECTITEM_METHOD_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
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
				mapFinalizationsIDToSignatureList.insert(lMethodSignatureList.at(i).nMethodID, lMethodSignatureList.at(i).sMethodSignature);
				nFinitCounter++;
			}
		}
		if(nFinitCounter > 0)
		{
			rFinalizationBox.setBottom(rAdditionalFinalizationsHeight + EXPGRAPHOBJECTITEM_METHOD_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE/2));
			rTotalHeight = rAdditionalFinalizationsHeight;
		}
	}
	prepareGeometryChange();//Needs to be called before a geometry change which affect the bounding region
	renderGraphItem();
}

int ExperimentGraphObjectItem::getMethodLocationIndex(const QString &sSignature, const ExperimentManagerNameSpace::ExperimentStructureItemType &mItemGraphType)
{
	if ((mItemGraphType == ExperimentManagerNameSpace::TypeObjectSlotMethodItem) || (mItemGraphType == ExperimentManagerNameSpace::TypeObjectSignalMethodItem))
	{
		if (mapSignalLocationIndexToSignatureList.values().contains(sSignature))
		{
			return mapSignalLocationIndexToSignatureList.key(sSignature);
		}
		else
		{
			int nTmpSlotIndex = mapSlotLocationIndexToSignatureList.key(sSignature);
			if (nTmpSlotIndex >= 0)
				return mapSignalLocationIndexToSignatureList.count() + nTmpSlotIndex;
		}
	}
	return -1;
}

int ExperimentGraphObjectItem::getMethodLocationPosition(const QString &sSignature, const ExperimentManagerNameSpace::ExperimentStructureItemType &mItemGraphType)
{
	int nIndex = getMethodLocationIndex(sSignature, mItemGraphType) + 1;
	if(nIndex>0)
	{
		return this->pos().y() + rAdditionalInitializationsHeight + EXPGRAPHOBJECTITEM_HEADER_HEIGHT + (EXPGRAPHOBJECTITEM_METHOD_HEIGHT / 2) + (nIndex * (EXPGRAPHOBJECTITEM_METHOD_SEPDISTANCE + EXPGRAPHOBJECTITEM_METHOD_HEIGHT));
	}
	return -1;	
}