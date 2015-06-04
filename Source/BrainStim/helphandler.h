#ifndef HELPHANDLER_H
#define HELPHANDLER_H

#include <QObject>
#include <QUrl>

class helpHandler : public QObject
{
	Q_OBJECT

public:
	helpHandler(QObject *parent);
	~helpHandler();

private:

public slots :
	void showHelp(const QUrl &url)
	{
		Q_UNUSED(url);
		//QString aa = url.path();
		//QString bb = url.fileName();
		//int a = 9 + 1;
	};
};

#endif // HELPHANDLER_H