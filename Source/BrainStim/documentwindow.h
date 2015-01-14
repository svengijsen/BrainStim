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


#ifndef DOCUMENTWINDOW_H
#define DOCUMENTWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QStringList>

class DocumentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DocumentWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~DocumentWindow();

protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dropEvent(QDropEvent* event);
    virtual bool openDroppedFiles(const QStringList& pathList) = 0;
};

#endif // DOCUMENTWINDOW_H
