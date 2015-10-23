#ifndef MAINAPPEXCHANGE_H
#define MAINAPPEXCHANGE_H

#include <QApplication>
#include <QSharedMemory>
#include <QLocalServer>
#include <QBuffer>
#include <globalapplicationinformation.h>

#define SEGMENT_ALLOWMULTIPLEINSTANCE  "AllowMultipleInstance"

class MainAppExchange : public QApplication
{	
	Q_OBJECT

signals:
	void messageAvailable(QString message);

public:
	MainAppExchange(int &argc, char *argv[], const QString uniqueKey);
	~MainAppExchange();

	bool isRunning();
	bool sendMessage(const QString &message);

	GlobalApplicationInformation *getGlobalAppInformationObjectPointer(bool &bFirstTimeUserInitialization);

public slots:
	void receiveMessage();
	void stopRunning();

private:
	//QString getSharedDataSegment(const QString &sName);
	//bool setSharedDataSegment(const QString &sName, const QString &sValue);

	GlobalApplicationInformation *m_globAppInformation;
	bool _isRunning;
	QString _uniqueKey;
	QSharedMemory sharedMemory;
	QLocalServer *localServer;
	static const int timeout = 1000;
};

#endif // MAINAPPEXCHANGE_H
