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

#include "finddialog.h"
#include "ui_FindDialog.h"
#include "experimenttreemodel.h"

FindDialog::FindDialog(QWidget *parent) : QWidget(parent), ui(new Ui::FindDialog)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
	connect(ui->findButton, SIGNAL(clicked()), this, SLOT(findSlot()), Qt::ConnectionType(Qt::UniqueConnection));
	connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()), Qt::ConnectionType(Qt::UniqueConnection));
	connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()), Qt::ConnectionType(Qt::UniqueConnection));
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::findSlot()
{
    QStringList filters;

    if (ui->cbTags->isChecked())
        filters.append(EXPERIMENTTREEMODEL_FILTER_TAGS);
    if (ui->cbValues->isChecked())
        filters.append(EXPERIMENTTREEMODEL_FILTER_VALUES);
    if (ui->cbAttributes->isChecked())
        filters.append(EXPERIMENTTREEMODEL_FILTER_ATTRIBUTES);
    if (ui->cbCaseSensitive->isChecked())
        filters.append(EXPERIMENTTREEMODEL_FILTER_CASE_SENSITIVE);

    emit findSignal(ui->LEFind->text(), filters);

    close();
}
