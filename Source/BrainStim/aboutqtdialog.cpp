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


/****************************************************************************
**
**
****************************************************************************/
#include "aboutqtdialog.h"

#include <QPluginLoader>
#include <QStringList>
#include <QDir>
#include <QMetaObject>
#include <QMetaMethod>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include "maindefines.h"
#include "BrainStim.h"

aboutQTDialog::aboutQTDialog(const QString &title,const QStringList &pluginFileNames,const QString &path, const QStringList &fileNames, QWidget *parent) : 
	QDialog(parent),
	label(new QLabel),
	treeWidget(new QTreeWidget),
	okButton(new QPushButton(tr("OK")))
{
	Q_UNUSED(fileNames);
	labWebsite = new QLabel;
	treeWidget->setAlternatingRowColors(false);
	treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
	treeWidget->setColumnCount(1);
	treeWidget->header()->hide();

	okButton->setDefault(true);

	connect(okButton, SIGNAL(clicked()), this, SLOT(close()));

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->setColumnStretch(0, 1);
	mainLayout->setColumnStretch(2, 1);
	mainLayout->addWidget(labWebsite, 0, 0, 1, 3);
	mainLayout->addWidget(label, 1, 0, 1, 3);
	mainLayout->addWidget(treeWidget, 2, 0, 1, 3);
	mainLayout->addWidget(okButton, 3, 1);
	setLayout(mainLayout);

	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
		QIcon::Normal, QIcon::On);
	interfaceIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
		QIcon::Normal, QIcon::Off);
	featureIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

	setWindowTitle("About " + title);
	//findPlugins(title,path, fileNames);

	bool bRetVal = false;
	const QMetaObject* metaObject = NULL;
	QString strSlot(FUNC_PLUGIN_ISCOMPATIBLE_FULL);
	foreach (QObject *plugin, QPluginLoader::staticInstances())
	{
		metaObject = plugin->metaObject();
		//QStringList methods;
		//for(int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i)
		//	methods << QString::fromLatin1(metaObject->method(i).signature());
		if (!(metaObject->indexOfMethod(QMetaObject::normalizedSignature(strSlot.toLatin1())) == -1))//Is the slot present?
		{
			//Invoke the slot
			metaObject->invokeMethod(plugin, FUNC_PLUGIN_ISCOMPATIBLE,Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal));//if(!metaObject->invokeMethod(plugin, Qt::DirectConnection, Q_RETURN_ARG(bool, bRetVal)))				
				//qDebug() << "aboutQTDialog(), Could not invoke the slot(" << strSlot << ")!";		
		}
		if (bRetVal)
			populateTreeWidget(plugin, tr("%1 (Static Plugin)").arg(plugin->metaObject()->className()));
		else
			populateTreeWidget(plugin, tr("%1 (Static Plugin --> NOT COMPATIBLE)").arg(plugin->metaObject()->className()));
		metaObject = NULL;
		bRetVal = false;
	}

	label->setText(title + tr(" found the following plugins\n"
		"(Dynamic plugins looked for in %1):")
		.arg(QDir::toNativeSeparators(path)));
	QString tmpStr = "<html>"
		"<body>"
		"<h1 style=""font-family:verdana;font-size:16px;"">" MAIN_PROGRAM_FULL_NAME "</h1>"
		"<p style=""font-family:arial;font-size:12px;"">"
		MAIN_PROGRAM_INTERNAL_NAME ", " MAIN_PROGRAM_LEGAL_COPYRIGHT "<br />"
		MAIN_PROGRAM_COMPANY_NAME ", " MAIN_PROGRAM_AUTHOR_NAME "<br />"
		"BrainStim is free software: you can redistribute it and/or modify<br />"
		"it under the terms of the GNU General Public License as published by<br />"
		"the Free Software Foundation, either version 3 of the License, or<br />"
		"(at your option) any later version.<br />"
		"This program is distributed in the hope that it will be useful,<br />"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of<br />"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br />"
		"GNU General Public License for more details.<br />"
		"You should have received a copy of the GNU General Public License<br />"
		"along with this program. If not, see<br />"
		"<a href=\"http://www.gnu.org/licenses\">http://www.gnu.org/licenses/</a> and <br />"
		"<a href=\"https://github.com/svengijsen/BrainStim\">https://github.com/svengijsen/BrainStim/</a><br />"
		"for more information.<br /><br />"
		"</p>"
		"</body>"
		"</html>";
	labWebsite->setOpenExternalLinks(true);
	labWebsite->setTextFormat(Qt::RichText);
	labWebsite->setText(tmpStr);

	foreach (QString pluginName,pluginFileNames)
		populateTreeWidget(0, tr("%1 (Device Plugin)").arg(pluginName));

}

//! [0]
void aboutQTDialog::findPlugins(const QString &title,const QString &path, const QStringList &fileNames)
{
	Q_UNUSED(title);
	const QDir dir(path);

	foreach (QObject *plugin, QPluginLoader::staticInstances())
		populateTreeWidget(plugin, tr("%1 (Static Plugin)")
		.arg(plugin->metaObject()->className()));

	foreach (QString fileName, fileNames) {
		QPluginLoader loader(dir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		if (plugin)
			populateTreeWidget(plugin, fileName);
	}
}

void aboutQTDialog::populateTreeWidget(QObject *plugin, const QString &text)
{
	QTreeWidgetItem *pluginItem = new QTreeWidgetItem(treeWidget);
	pluginItem->setText(0, text);
	treeWidget->setItemExpanded(pluginItem, true);

	QFont boldFont = pluginItem->font(0);
	boldFont.setBold(true);
	pluginItem->setFont(0, boldFont);

	if (plugin) {
//	DeviceInterface *iDevice = qobject_cast<DeviceInterface *>(plugin);
//	if (iDevice)
	   // addItems(pluginItem, "DeviceInterface", text);
//	//ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plug/in);
//	//if (iShape)
//	//    addItems(pluginItem, "ShapeInterface", iShape->shapes());
//
//	//FilterInterface *iFilter =
//	//        qobject_cast<FilterInterface *>(plugin);
//	//if (iFilter)
//	//    addItems(pluginItem, "FilterInterface", iFilter->filters());
	}
}

void aboutQTDialog::addItems(QTreeWidgetItem *pluginItem, const char *interfaceName, const QStringList &features)
{
	QTreeWidgetItem *interfaceItem = new QTreeWidgetItem(pluginItem);
	interfaceItem->setText(0, interfaceName);
	interfaceItem->setIcon(0, interfaceIcon);

	foreach (QString feature, features) {
		if (feature.endsWith("..."))
			feature.chop(3);
		QTreeWidgetItem *featureItem = new QTreeWidgetItem(interfaceItem);
		featureItem->setText(0, feature);
		featureItem->setIcon(0, featureIcon);
	}
}
