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

#include "updatemngr.h"

UpdateMngr::UpdateMngr(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
}

UpdateMngr::~UpdateMngr()
{

}

void UpdateMngr::appendDebugMessages(const QList<installationManagerBase::strcMessage> &lMessages)
{
	foreach(installationManagerBase::strcMessage tmpMessage, lMessages)
	{
		QListWidgetItem *tmpItem = NULL;
		tmpItem = new QListWidgetItem(tmpMessage.sMessage);
		if (tmpMessage.nMessageType == installationManagerBase::Informative)
		{
			tmpItem->setIcon(QIcon(":UpdateMngr/resources/Information.png"));
		}
		else if (tmpMessage.nMessageType == installationManagerBase::Succeeded)
		{
			tmpItem->setIcon(QIcon(":UpdateMngr/resources/NoError.png"));
		}
		else if (tmpMessage.nMessageType == installationManagerBase::Failed)
		{
			tmpItem->setIcon(QIcon(":UpdateMngr/resources/Error.png"));
		}
		ui.listWidget->addItem(tmpItem);
	}
}
