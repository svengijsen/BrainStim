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

#include "networkserverthread.h"
#include <QtNetwork>

NetworkServerThreadClient::NetworkServerThreadClient(int sockDescriptor) : QObject()
{
	nSockDescriptor = sockDescriptor;
	m_socket  = new QTcpSocket(this);
	if (!m_socket->setSocketDescriptor(nSockDescriptor)) 
	{
	    emit error(m_socket->error());
	    return;
	}
	bool bConnectResult = connect(m_socket, SIGNAL(readyRead()), this, SLOT(clientReadyRead()));
	bConnectResult = bConnectResult && connect(m_socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
	bConnectResult = bConnectResult && connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientError(QAbstractSocket::SocketError)));	
}

NetworkServerThreadClient::~NetworkServerThreadClient()
{
	if(m_socket)
	{
		delete m_socket;
		m_socket = NULL;
	}
}

void NetworkServerThreadClient::initSocketConnection() 
{
	networkDataBlockSize = 0;
	if(!m_socket)
		return;
	sendData(QString("Welcome you're connected to BrainStim"));// @ %1:%2").arg(parentServer->serverAddress().toString()).arg(parentServer->serverPort());
	qDebug() << QString("The Network Server accepted an incoming connection");		
}

bool NetworkServerThreadClient::sendData(const QString &sData) 
{
	if(!m_socket)
		return false;
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);
	out << (quint16)0;
	out << sData; 
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));
	if(m_socket->write(block))
		return true;
	return false;
}

void NetworkServerThreadClient::clientReadyRead()
{
	if(!m_socket)
		return;
	QDataStream in(m_socket);
	in.setVersion(QDataStream::Qt_5_0);
	if (networkDataBlockSize == 0) 
	{
		if (m_socket->bytesAvailable() < (int)sizeof(quint16))
			return;
		in >> networkDataBlockSize;
	}
	if (m_socket->bytesAvailable() < networkDataBlockSize)
		return;
	QString sAvailableData;
	in >> sAvailableData;
	emit NetworkDataAvailable(sAvailableData);
	networkDataBlockSize = 0;
	clientReadyRead();
}

void NetworkServerThreadClient::clientDisconnected()
{
	qDebug() << __FUNCTION__ << "A client disconnected an active network connection.";
}

void NetworkServerThreadClient::clientError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) 
	{
	case QAbstractSocket::RemoteHostClosedError:		
		break;
	case QAbstractSocket::HostNotFoundError:
		qDebug() << __FUNCTION__ << "The host was not found. Please check the host name and port settings.";
		break;
	case QAbstractSocket::ConnectionRefusedError:
		qDebug() << __FUNCTION__ << "The connection was refused by the peer. "
			"Make sure the network server is running, "
			"and check that the host name and port "
			"settings are correct.";
		break;
	default:
		qDebug() << __FUNCTION__ << QString("The following Network error occurred: %1.").arg(m_socket->errorString());
	}
}
