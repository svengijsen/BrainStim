#ifndef CUSTOMQUESTIONDIALOG_H
#define CUSTOMQUESTIONDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>

class customQuestionDialog : public QDialog
{
	Q_OBJECT

public:
	customQuestionDialog(QWidget *parent = NULL);
	~customQuestionDialog();

	bool setOptions(const QString &sCaption, const QString &sText, const QStringList &lButtons, const int &nDefaultReturnIndex);

public slots:
	int exec();
	void buttonPressed(const int &nIndex);

private:
	QGridLayout *mainGridLayout;
	QHBoxLayout *horizontalLayout;
	QLabel *custLabel;
	int nLastButtonPressed;
	int nDefValue;
	QStringList lLastSetButtons;
};

#endif // CUSTOMQUESTIONDIALOG_H
