#ifndef XMLMESSAGEHANDLER_H
#define XMLMESSAGEHANDLER_H

#include <QtXmlPatterns/QAbstractMessageHandler>
//#include <QtXmlPatterns/>

class XmlMessageHandler : public QAbstractMessageHandler
{
	//Q_OBJECT

public:
	XmlMessageHandler();
	~XmlMessageHandler();

	QString statusMessage() const;
	int line() const;
	int column() const;

protected:
	virtual void handleMessage(QtMsgType type, const QString &description, const QUrl &identifier, const QSourceLocation &sourceLocation)
	{
		Q_UNUSED(type);
		Q_UNUSED(identifier);

		m_messageType = type;
		m_description = description;
		m_sourceLocation = sourceLocation;
	}

private:
	QtMsgType m_messageType;
	QString m_description;
	QSourceLocation m_sourceLocation;
	
};

#endif // XMLMESSAGEHANDLER_H