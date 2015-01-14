//BrainStim
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

#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtCore/QList>
//#include <windows.h>
//#include <QtGui/QMessageBox>
//#include <QtGui/QApplication>
//#include <QtCore/QDir>
//#include <QtCore/QFileInfo>
//#include <QtCore/QRegExp>

#include "documentwindow.h"

//DocumentWindow::DocumentWindow(QWidget* parent, Qt::WindowFlags flags) : 
//			QMainWindow(parent, flags),
//			m_registerForDDE(false),
//			m_appAtomName(),
//			m_systemTopicAtomName("system"),
//			m_appAtom(0),
//			m_systemTopicAtom(0)
//{
//    setAcceptDrops(true);
//
//	QFileInfo fi(qApp->applicationFilePath());
//	m_appAtomName = fi.baseName();
//}

DocumentWindow::DocumentWindow(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
	setAcceptDrops(true);	
}

DocumentWindow::~DocumentWindow()
{
}

void DocumentWindow::dragEnterEvent(QDragEnterEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}

void DocumentWindow::dragMoveEvent(QDragMoveEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}

void DocumentWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void DocumentWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            pathList.append(urlList.at(i).toLocalFile());
        }

        if(openDroppedFiles(pathList))
            event->acceptProposedAction();
    }
}
