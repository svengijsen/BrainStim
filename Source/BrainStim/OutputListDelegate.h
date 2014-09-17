#ifndef OUTPUTLISTDELEGATE_H
#define OUTPUTLISTDELEGATE_H

#include <QPainter>
#include <QAbstractItemDelegate>

class OutputListDelegate : public QAbstractItemDelegate
{
	Q_OBJECT

public:
	OutputListDelegate(QObject *parent = 0);

	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
	QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

	virtual ~OutputListDelegate();

private:
	
};

#endif // OUTPUTLISTDELEGATE_H
