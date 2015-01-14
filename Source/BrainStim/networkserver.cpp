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

#include "networkserver.h"
#include "networkserverthread.h"

NetworkServer::NetworkServer(QObject *parent) : QTcpServer(parent)
{
}

NetworkServer::~NetworkServer()
{
	if(!strcNetworkServerClients.isEmpty())
	{
		for (int i=0;i<strcNetworkServerClients.count();i++)
		{
			delete strcNetworkServerClients[i].runningThread;
			strcNetworkServerClients[i].runningThread = NULL;
			strcNetworkServerClients[i].socketDescriptor = 0;
		}		
	}
	strcNetworkServerClients.clear();
}

int NetworkServer::AppendClientConnection(NetworkServerThread* runningThread, qintptr socketDescriptor)
{
	//Return the index of the appended client connection.
	NetworkServerThreadClientStrc tmpStruct;
	tmpStruct.runningThread = runningThread;
	tmpStruct.socketDescriptor = socketDescriptor;
	strcNetworkServerClients.append(tmpStruct);
	return strcNetworkServerClients.count()-1;
}

void NetworkServer::incomingConnection(qintptr socketDescriptor)
{
	// Let us create a autonomous network server thread.
	NetworkServerThread *thread = new NetworkServerThread(this);
	// Our functionality which should be run in a thread.
	NetworkServerThreadClient *client = new NetworkServerThreadClient(socketDescriptor); 
	// Move the whole functionality context to our new thread!
	client->moveToThread(thread); 
	// Event-handling: when our thread is instantiated, we will
	// call our above handling method.
	bool bConnectResult = connect(thread, SIGNAL(started()), client, SLOT(initSocketConnection()));
	//connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));//don't need that we need to keep our socket connected!
	//void error(QAbstractSocket::SocketError);
	int nAppendIndex = AppendClientConnection(thread,socketDescriptor);
	bConnectResult = bConnectResult && connect(client, &NetworkServerThreadClient::NetworkDataAvailable, [=](QString arg) {NetworkDataAvailable(nAppendIndex, arg);});
	thread->start();
}
