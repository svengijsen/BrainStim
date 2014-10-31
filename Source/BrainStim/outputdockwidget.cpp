//BrainStim
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

#include "outputdockwidget.h"
#include "mainappinfo.h"
#include <QResizeEvent>
#include <QSettings>

OutputDockWidget::OutputDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags) : QDockWidget(title, parent, flags)
{
	bIsInitialized = false;
}

OutputDockWidget::~OutputDockWidget()
{

}

void OutputDockWidget::initialize()
{
	MainAppInfo::loadDockWidgetLayout(this);
	bIsInitialized = true;
}

void OutputDockWidget::resizeEvent(QResizeEvent *event)
{
	if (bIsInitialized == false)
		return;
	QSize sNewSize = event->size();
	if (sNewSize != event->oldSize())
	{
		MainAppInfo::saveDockWidgetLayout(this);
	}
}
