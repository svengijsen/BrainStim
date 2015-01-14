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

#include "logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent), md5Generator(NULL)
{
	setUpGUI();
	setWindowTitle(tr("User Login"));
	setModal(true);
}

LoginDialog::~LoginDialog()
{
	if (md5Generator)
		delete md5Generator;
}

void LoginDialog::setUpGUI()
{
	// set up the layout
	QGridLayout* formGridLayout = new QGridLayout(this);
	// initialize the username combo box so that it is editable
	comboUsername = new QComboBox(this);
	comboUsername->setEditable(true);
	// initialize the password field so that it does not echo
	// characters
	editPassword = new QLineEdit(this);
	editPassword->setEchoMode(QLineEdit::Password);
	// initialize the labels
	labelUsername = new QLabel(this);
	labelPassword = new QLabel(this);
	labelUsername->setText(tr("Username"));
	labelUsername->setBuddy(comboUsername);
	labelPassword->setText(tr("Password"));
	labelPassword->setBuddy(editPassword);
	// initialize buttons
	buttons = new QDialogButtonBox(this);
	buttons->addButton(QDialogButtonBox::Ok);
	buttons->addButton(QDialogButtonBox::Cancel);
	buttons->addButton(QDialogButtonBox::Apply);
	buttons->button(QDialogButtonBox::Ok)->setText(tr("Login"));
	buttons->button(QDialogButtonBox::Cancel)->setText(tr("Skip"));
	buttons->button(QDialogButtonBox::Apply)->setText(tr("Create new account"));
	// connects slots
	connect(buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(close()));
	connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this,	SLOT(slotAcceptLogin()));
	connect(buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(createNewAccount()));
	// place components into the dialog
	formGridLayout->addWidget(labelUsername, 0, 0);
	formGridLayout->addWidget(comboUsername, 0, 1);
	formGridLayout->addWidget(labelPassword, 1, 0);
	formGridLayout->addWidget(editPassword, 1, 1);
	formGridLayout->addWidget(buttons, 2, 0, 1, 3);
	setLayout(formGridLayout);
}

void LoginDialog::setUsername(const QString &username, const int &nIndex)
{
	bool found = false;
	for (int i = 0; i < comboUsername->count() && !found; i++)
	{
		if (comboUsername->itemText(i) == username)
		{
			comboUsername->setCurrentIndex(i);
			found = true;
		}
	}

	if (found==false)
	{
		int nFinalIndex;
		if (nIndex >= 0)
			nFinalIndex = nIndex;
		else
			nFinalIndex = comboUsername->count();
		//qDebug() << "Select username " << nFinalIndex;
		comboUsername->insertItem(nFinalIndex, username);
		//comboUsername->setCurrentIndex(nFinalIndex);
	}
	// place the focus on the password field
	editPassword->setFocus();
}


void LoginDialog::setPassword(const QString &password)
{
	editPassword->setText(password);
}

void LoginDialog::insertUsers(const QList<strcUserCredential>& lUserCredentialList)
{
	lRegisteredUserCredentials.append(lUserCredentialList);
	comboUsername->clear();
	QStringList lCurentUserNames;
	for (int i = 0; i<lRegisteredUserCredentials.count(); i++)//each(strcUserCredential tmpUserCredential, lRegisteredUserCredentials)
	{
		if (lCurentUserNames.contains(lRegisteredUserCredentials[i].sUserName))
		{
			lRegisteredUserCredentials.removeAt(i);
			i--;
		}
		else
		{
			lCurentUserNames.append(lRegisteredUserCredentials[i].sUserName);
		}
	}
	comboUsername->addItems(lCurentUserNames);
}

void LoginDialog::createNewAccount()
{
	QString sNewUserName = comboUsername->currentText();
	QString sNewUserPass = editPassword->text();
	comboUsername->setCurrentText("");
	editPassword->clear();

	if (sNewUserName.isEmpty())
	{
		QMessageBox::critical(this, "Invalid username", QObject::tr("Unable to create a new account, please make sure to enter first a valid unique username and password(optional) using the accompanied text controls."));
		return;
	}
	for (int i = 0; i < comboUsername->count(); i++)
	{
		if (comboUsername->itemText(i).toLower() == sNewUserName.toLower())
		{
			QMessageBox::critical(this, "Username already registered", QObject::tr("Unable to create a new account, please make sure to enter a username that is not yet used."));
			return;
		}
	}
	strcUserCredential tmpUserCredential;
	tmpUserCredential.sUserName = sNewUserName;
	if (md5Generator)
		delete md5Generator;
	md5Generator = new QCryptographicHash(QCryptographicHash::Md5);
	md5Generator->addData(sNewUserPass.toUtf8());
	tmpUserCredential.baPasswordHash = md5Generator->result().toHex();
	lRegisteredUserCredentials.append(tmpUserCredential);
	setUsername(sNewUserName);
	comboUsername->setCurrentIndex(comboUsername->count() - 1);
	editPassword->setText(sNewUserPass);
	QMessageBox::information(this, "New user created", QObject::tr("The user account is successfully created, you'll now be automatically logged on using this account."));
	slotAcceptLogin();
}

void LoginDialog::slotAcceptLogin()
{
	QString sCurrentUserName = comboUsername->currentText();
	QString sCurrentUserPass = editPassword->text();
	for (int i = 0; i < comboUsername->count(); i++)
	{
		if (comboUsername->itemText(i).toLower() == sCurrentUserName.toLower())
		{
			foreach(strcUserCredential tmpUserCredential, lRegisteredUserCredentials)
			{
				if (tmpUserCredential.sUserName == sCurrentUserName.toLower())
				{
					if (md5Generator)
						delete md5Generator;
					md5Generator = new QCryptographicHash(QCryptographicHash::Md5);
					md5Generator->addData(sCurrentUserPass.toUtf8());
					QByteArray byteArrayHexedHash = md5Generator->result().toHex();
					if (byteArrayHexedHash == tmpUserCredential.baPasswordHash)
					{
						strcLatestLoginUserInformation.sUserName = sCurrentUserName;
						strcLatestLoginUserInformation.baPasswordHash = byteArrayHexedHash;
						emit acceptLogin(strcLatestLoginUserInformation.sUserName, sCurrentUserPass, i);
						close();
						return;
					}
				}
			}
		}
	}
	QMessageBox::critical(this, "Invalid login credentials provided", QObject::tr("Unable to login using the provided credentials, please make sure to enter a valid registered username and password."));
}

bool LoginDialog::getCurrentUserCredentials(QString &sUserName, QByteArray &baPasswordHash) const
{
	if (strcLatestLoginUserInformation.sUserName.isEmpty() == false)
	{
		sUserName = strcLatestLoginUserInformation.sUserName;
		baPasswordHash = strcLatestLoginUserInformation.baPasswordHash;
		return true;
	}
	return false;
}
