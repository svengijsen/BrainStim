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

#include "customdockwidget.h"
#include "experimentgraphiceditor.h"
#include <QResizeEvent>

customDockWidget::customDockWidget(const QString &sTitle, Qt::DockWidgetArea defaultDocWidgetArea, QWidget *parent, Qt::WindowFlags flags) : MainWindowDockWidget(sTitle, defaultDocWidgetArea, parent, flags), bIsInitialized(false), bVisibilitySetByUser(true)
{
}

customDockWidget::~customDockWidget()
{
}

void customDockWidget::initialize()
{//! Call this after setting the child widget with ->setWidget(QWidget*)
	QWidget *childWidget = widget();
	if (childWidget)
	{
		bIsInitialized = true;
	}
}

void customDockWidget::setVisible(bool visible)
{
	bool bVisibilityAboutToChange = false;
	if(visible != isVisible())
		bVisibilityAboutToChange = true;
	if(bVisibilityAboutToChange)
	{
		if(visible)
		{
			if(bVisibilitySetByUser == false)
				return;
		}
	}
	QDockWidget::setVisible(visible);
}

void customDockWidget::toggleConfiguredVisibility()
{
	bVisibilitySetByUser = !bVisibilitySetByUser;
	setVisible(bVisibilitySetByUser);
}

void customDockWidget::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event);
	bVisibilitySetByUser = false;
}