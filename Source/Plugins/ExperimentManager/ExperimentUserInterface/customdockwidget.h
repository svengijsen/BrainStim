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

#ifndef CUSTOMDOCKWIDGET_H
#define CUSTOMDOCKWIDGET_H

#include <QDockWidget>

class customDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	customDockWidget(const QString &sTitle, QWidget *parent = NULL, Qt::WindowFlags flags = 0);
	~customDockWidget();

	void setVisible(bool visible);
	void initialize();

public slots:
	void toggleConfiguredVisibility();

protected:
	void closeEvent(QCloseEvent * event);

private:
	bool bVisibilitySetByUser;
	bool bIsInitialized;
};

#endif // CUSTOMDOCKWIDGET_H
