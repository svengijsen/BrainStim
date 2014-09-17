#include "XmlMessageHandler.h"

XmlMessageHandler::XmlMessageHandler() : QAbstractMessageHandler(0)
{

}

XmlMessageHandler::~XmlMessageHandler()
{

}

QString XmlMessageHandler::statusMessage() const
{
	return m_description;
}

int XmlMessageHandler::line() const
{
	return m_sourceLocation.line();
}

int XmlMessageHandler::column() const
{
	return m_sourceLocation.column();
}

