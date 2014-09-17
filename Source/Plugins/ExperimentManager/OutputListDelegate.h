#ifndef OUTPUTLISTDELEGATE_H
#define OUTPUTLISTDELEGATE_H

#include <QAbstractItemDelegate>

class OutputListDelegate : public QAbstractItemDelegate
{
	Q_OBJECT

public:
	OutputListDelegate(QObject *parent);
	~OutputListDelegate();

private:
	
};

#endif // OUTPUTLISTDELEGATE_H
