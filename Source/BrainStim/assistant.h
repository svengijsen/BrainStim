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

#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QtCore/QString>
#include <QFile>
#include "mainappinfo.h"
#include "globalapplicationinformation.h"

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

class Assistant
{
public:
    Assistant();
    ~Assistant();
    void showDocumentation(const QString &file, GlobalApplicationInformation *GlobAppInfo);
	QStringList getPluginDocumentationPaths();
    
private:
    bool startAssistant(GlobalApplicationInformation *GlobAppInfo);
    QProcess *proc;
	QString strHelpPackagePath;
	QString strHelpPackageDir;
};

#endif
