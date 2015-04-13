#include "customquestiondialog.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalMapper>

customQuestionDialog::customQuestionDialog(QWidget *parent) : QDialog(parent)
{
	QGridLayout *mainGridLayout = new QGridLayout(this);
	mainGridLayout->setObjectName(QStringLiteral("mainGridLayout"));
	custLabel = new QLabel(this);
	custLabel->setObjectName(QStringLiteral("custLabel"));
	mainGridLayout->addWidget(custLabel, 0, 0, 1, 1);
	horizontalLayout = new QHBoxLayout();
	mainGridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);
	nLastButtonPressed = -1;
}

customQuestionDialog::~customQuestionDialog()
{}

bool customQuestionDialog::setOptions(const QString &sCaption, const QString &sText, const QStringList &lButtons, const int &nDefaultReturnIndex)
{
	if (objectName().isEmpty())
		setObjectName("customQuestionDialog");
	setWindowTitle(sCaption);
	setWindowModality(Qt::ApplicationModal);

	custLabel->setText(sText);
	bool bResult = false;
	QSignalMapper *buttonPressedSignalMapper = NULL;
	if (lButtons.isEmpty() == false)
	{
		buttonPressedSignalMapper = new QSignalMapper(this);
		for (int i = 0; i < lButtons.count(); i++)
		{
			QPushButton *custButton = new QPushButton(this);
			custButton->setObjectName(lButtons[i]);
			custButton->setText(lButtons[i]);
			horizontalLayout->addWidget(custButton);

			bResult = connect(custButton, SIGNAL(clicked()), buttonPressedSignalMapper, SLOT(map()));
			buttonPressedSignalMapper->setMapping(custButton, i);
		}
	}
	if (buttonPressedSignalMapper)
		bResult = connect(buttonPressedSignalMapper, SIGNAL(mapped(int)), this, SLOT(buttonPressed(int)));
	lLastSetButtons = lButtons;
	nDefValue = nDefaultReturnIndex;
	return true;
}

void customQuestionDialog::buttonPressed(const int &nIndex)
{
	QString sButtonPressedText = lLastSetButtons[nIndex];
	nLastButtonPressed = nIndex;
	this->close();
}

int customQuestionDialog::exec()
{
	int nRetVal = QDialog::exec();
	if (nLastButtonPressed >= 0)
		return nLastButtonPressed;
	return nDefValue;
}
