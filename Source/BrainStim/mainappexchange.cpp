#include <QLocalSocket>
#include "mainappexchange.h"

MainAppExchange::MainAppExchange(int &argc, char *argv[], const QString uniqueKey) : QApplication(argc, argv), _uniqueKey(uniqueKey)
{
	m_globAppInformation = NULL;
	sharedMemory.setKey(_uniqueKey);//Sets a new key for this shared memory object. If key and the current key are the same, the function returns without doing anything.
	if (sharedMemory.attach(QSharedMemory::ReadWrite))//Attempts to attach the process to the shared memory segment identified by the key that was passed to the constructor.
	{
		_isRunning = true;
	}
	else
	{
		_isRunning = false;
		// create shared memory.
		int nBytes = 1;
		if (!sharedMemory.create(nBytes,QSharedMemory::ReadWrite))//Creates a shared memory segment of size bytes with the key previously passed, attaches to the new shared memory segment with the given access mode, and returns true.
		{
			qDebug() << __FUNCTION__ << "Unable to create MainAppExchange, " << sharedMemory.errorString();
			return;
		}
		//setSharedDataSegment(SEGMENT_ALLOWMULTIPLEINSTANCE,"true");
		//QString sRetVal = getSharedDataSegment(SEGMENT_ALLOWMULTIPLEINSTANCE);

		// create local server and listen to incoming messages from other instances.
		localServer = new QLocalServer(this);
		connect(localServer, SIGNAL(newConnection()), this, SLOT(receiveMessage()));
		localServer->listen(_uniqueKey);
	}
}

GlobalApplicationInformation *MainAppExchange::getGlobalAppInformationObjectPointer()
{
	if(m_globAppInformation == NULL)
		m_globAppInformation = new GlobalApplicationInformation();
	return m_globAppInformation;
}

bool MainAppExchange::isRunning()
{
	return _isRunning;
}

void MainAppExchange::receiveMessage()
{
	QLocalSocket *localSocket = localServer->nextPendingConnection();
	if (!localSocket->waitForReadyRead(timeout))
	{
		qDebug(localSocket->errorString().toLatin1());
		return;
	}
	QByteArray byteArray = localSocket->readAll();
	QString message = QString::fromUtf8(byteArray.constData());
	emit messageAvailable(message);
	localSocket->disconnectFromServer();
}

bool MainAppExchange::sendMessage(const QString &message)
{
	if (!_isRunning)
		return false;
	QLocalSocket localSocket(this);
	localSocket.connectToServer(_uniqueKey, QIODevice::WriteOnly);
	if (!localSocket.waitForConnected(timeout))
	{
		qDebug() << __FUNCTION__ << localSocket.errorString().toLatin1();
		return false;
	}
	localSocket.write(message.toUtf8());
	if (!localSocket.waitForBytesWritten(timeout))
	{
		qDebug() << __FUNCTION__ << localSocket.errorString().toLatin1();
		return false;
	}
	localSocket.disconnectFromServer();
	return true;
}

//QString MainAppExchange::getSharedDataSegment(const QString &sName)
//{
//	if(sharedMemory.isAttached() == false)
//		return "";
//
//	QBuffer buffer;
//	QDataStream inStream(&buffer);
//	QString sValue;
//
//	sharedMemory.lock();
//	buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
//	buffer.open(QBuffer::ReadOnly);
//	inStream >> sValue;
//	sharedMemory.unlock();
//
//	return sValue;
//}
//
//bool MainAppExchange::setSharedDataSegment(const QString &sName, const QString &sValue)
//{
//	if(sharedMemory.isAttached() == false)
//		return false;
//	
//	QBuffer buffer;
//	buffer.open(QBuffer::ReadWrite);
//	QDataStream outStream(&buffer);
//	outStream << sValue;
//	int nSize = buffer.size();
//
//	sharedMemory.lock();
//	char *cDestination = (char*)sharedMemory.data();
//	const char *cSource = buffer.data().data();
//	memcpy(cDestination, cSource, qMin(sharedMemory.size(), nSize));
//	sharedMemory.unlock();
//	//SEGMENT_ALLOWMULTIPLEINSTANCE
//	return true;
//}
