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

#include "QGraphicsViewEc.h"
#include <QWheelEvent>
#include <qmath.h>

QGraphicsViewEc::QGraphicsViewEc(QWidget* parent) : QGraphicsView(parent)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	setMaximumSize(10000,10000);
	setMinimumSize(100,100);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


void QGraphicsViewEc::wheelEvent(QWheelEvent* event)
{
    qreal scaleFactor = qPow(2.0, event->delta() / 240.0); //How fast we zoom
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(0.05 < factor && factor < 10) //Zoom factor limitation
        scale(scaleFactor, scaleFactor);
}
