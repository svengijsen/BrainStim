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


#ifndef ABOUTQTDIALOG_H
#define ABOUTQTDIALOG_H

#include <QDialog>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QStringList;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class aboutQTDialog : public QDialog
{
	Q_OBJECT

public:
	aboutQTDialog(const QString &title, QWidget *parent = 0);

private:
	//void findPlugins(const QString &title, const QString &path, const QStringList &fileNames);
	//void populateTreeWidget(QObject *plugin, const QString &text);
	//void addItems(QTreeWidgetItem *pluginItem, const char *interfaceName,const QStringList &features);

	QLabel *label;
	QLabel *labWebsite;
	//QTreeWidget *treeWidget;
	QPushButton *okButton;
	QIcon interfaceIcon;
	QIcon featureIcon;
	
};

#endif // ABOUTQTDIALOG_H
