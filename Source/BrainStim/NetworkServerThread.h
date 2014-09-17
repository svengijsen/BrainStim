//BrainStim
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

#ifndef NetworkServerThread_H
#define NetworkServerThread_H

#include <QThread>
#include <QTcpSocket>

class NetworkServerThreadClient : public QObject 
{
	Q_OBJECT

signals:
	void error(QAbstractSocket::SocketError);
	void NetworkDataAvailable(QString);

public:
	NetworkServerThreadClient(int sockDescriptor);
	~NetworkServerThreadClient();

protected slots:
	bool sendData(const QString &sData);
	void initSocketConnection(); 
	void clientReadyRead();
	void clientDisconnected();
	void clientError(QAbstractSocket::SocketError socketError);

private:
	int nSockDescriptor;
	QTcpSocket *m_socket;
	quint16 networkDataBlockSize;
};

class NetworkServerThread : public QThread
{
	Q_OBJECT

public:
	NetworkServerThread(QObject *parent) {Q_UNUSED(parent);};
};

#endif
