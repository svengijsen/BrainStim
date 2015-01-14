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

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QCryptographicHash>
//#include <QDebug>

class LoginDialog : public QDialog
{
	Q_OBJECT

signals :
	/*!
	* A signal emitted when the login is performed.
	* \param username the username entered in the dialog
	* \param password the password entered in the dialog
	* \param index the number of the username selected in the combobox
	*/
	void acceptLogin(QString& username, QString& password, int& indexNumber);

public:
	explicit LoginDialog(QWidget *parent = NULL);
	~LoginDialog();

	struct  strcUserCredential
	{
		QString sUserName;
		QByteArray baPasswordHash;
		strcUserCredential()
		{
			sUserName = "";
		}
	};

	void insertUsers(const QList<strcUserCredential>& lUserCredentialList);
	bool getCurrentUserCredentials(QString &sUserName, QByteArray &baPasswordHash) const;

public slots:
	void slotAcceptLogin();
	void createNewAccount();

private: 
	strcUserCredential strcLatestLoginUserInformation;
	QLabel* labelUsername;
	QLabel* labelPassword;
	QComboBox* comboUsername;
	QLineEdit* editPassword;
	QDialogButtonBox* buttons;
	QList<strcUserCredential> lRegisteredUserCredentials;
	QCryptographicHash *md5Generator;

	void setUpGUI();
	void setUsername(const QString &username, const int &nIndex = -1);
	void setPassword(const QString& password);
};

#endif // LOGINDIALOG_H
