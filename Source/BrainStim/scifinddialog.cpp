//BrainStim
//Copyright (C) 2014  Sven Gijsen
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


#include "scifinddialog.h"
#include <QKeyEvent>
#include <QLineEdit>

//#include "IconManager.h"
//#include "MainSettings.h"

QString sciFindDialog::lastString_ = "";
QString sciFindDialog::lastReplaceText_ = "";
bool sciFindDialog::matchCase_ = false;
bool sciFindDialog::backward_ = false;
bool sciFindDialog::regExpMode_ = false;
bool sciFindDialog::wholeWords_ = false;
bool sciFindDialog::multiLine_ = false;
QStringList sciFindDialog::strings_;
QStringList sciFindDialog::replaces_;

sciFindDialog::sciFindDialog(QWidget* parent, bool repl) : QDialog(parent) 
{
	uiFind.setupUi(this);

	foreach (QString str, strings_)
		uiFind.findCmb->addItem(str);
	foreach (QString repl, replaces_)
		uiFind.replaceCmb->addItem(repl);

	uiFind.findCmb->setCompleter(0);
	uiFind.replaceCmb->setCompleter(0);

	connect(uiFind.findBtn, SIGNAL(clicked()), SLOT(accept()));
	connect(uiFind.cancelBtn, SIGNAL(clicked()), SLOT(reject()));
	connect(uiFind.replaceChk, SIGNAL(toggled(bool)), SLOT(setReplaceMode(bool)));
	connect(uiFind.multiLineChk, SIGNAL(toggled(bool)), SLOT(multiLineChecked(bool)));
	connect(uiFind.regexpChk, SIGNAL(toggled(bool)), SLOT(regExpChecked(bool)));

	uiFind.findCmb->setEditText(lastString_);
	uiFind.mlEd->setText(lastString_);
	uiFind.replaceCmb->setEditText(lastReplaceText_);
	uiFind.matchCaseChk->setChecked(matchCase_);
	uiFind.backwardChk->setChecked(backward_);
	uiFind.replaceChk->setChecked(false);
	uiFind.regexpChk->setChecked(regExpMode_);
	uiFind.multiLineChk->setChecked(multiLine_);
	uiFind.wholeWordsChk->setChecked(wholeWords_);

	setReplaceMode(repl);
	if ( !multiLine_ ) {
		uiFind.findCmb->setFocus();
		uiFind.findCmb->lineEdit()->selectAll();
	}
	else {
		uiFind.mlEd->setFocus();
		uiFind.mlEd->selectAll();
	}
	uiFind.mlEd->setVisible(multiLine_);
	uiFind.findCmb->setVisible(!multiLine_);

	//resize(MainSettings::sciFindDialogRect().size());
}

sciFindDialog::~sciFindDialog() 
{
	lastReplaceText_ = uiFind.replaceCmb->currentText();
	matchCase_ = uiFind.matchCaseChk->isChecked();
	backward_ = uiFind.backwardChk->isChecked();
	regExpMode_ = uiFind.regexpChk->isChecked();
	wholeWords_ = uiFind.wholeWordsChk->isChecked();
	multiLine_ = uiFind.multiLineChk->isChecked();
	if ( multiLine_ ) {
		lastString_ = uiFind.mlEd->toPlainText();
	}
	else {
		lastString_ = uiFind.findCmb->currentText();
	}

	//MainSettings::setsciFindDialogRect(rect());

	if ( strings_.contains(lastString_) )
		strings_.removeAll(lastString_);
	if ( replaces_.contains(lastReplaceText_) )
		replaces_.removeAll(lastReplaceText_);
	if ( !lastString_.isEmpty() )
		strings_.prepend(lastString_);
	if ( !lastReplaceText_.isEmpty() )
		replaces_.prepend(lastReplaceText_);
}

void sciFindDialog::setText(const QString& t) 
{
	if ( uiFind.multiLineChk->isChecked() ) {
		uiFind.mlEd->setText(t); 
		uiFind.mlEd->selectAll();
	}
	else {
		if ( !t.isEmpty() ) {
			uiFind.findCmb->insertItem(0, t);
			uiFind.findCmb->setCurrentIndex(0); 
			uiFind.findCmb->lineEdit()->selectAll();
		}
	}
}

QString sciFindDialog::text() const 
{
	if ( uiFind.multiLineChk->isChecked() )
		return uiFind.mlEd->toPlainText();
	else
		return uiFind.findCmb->currentText();
}

QString sciFindDialog::replaceTo() const 
{
	QString replStr = uiFind.replaceCmb->currentText();
	replStr.replace("\\n", "\n");
	replStr.replace("\\r", "\r");
	replStr.replace("\\t", "\t");
	return replStr;
}

DocFindFlags sciFindDialog::flags() const 
{
	return DocFindFlags(uiFind.replaceChk->isChecked(), uiFind.matchCaseChk->isChecked(), 
		uiFind.backwardChk->isChecked(), uiFind.regexpChk->isChecked(), 
		uiFind.wholeWordsChk->isChecked(), uiFind.multiLineChk->isChecked());
}

void sciFindDialog::keyPressEvent(QKeyEvent* e) 
{
	if ( ( e->key() == Qt::Key_F || e->key() == Qt::Key_R ) && e->modifiers() & Qt::ControlModifier) {
		uiFind.replaceChk->toggle();
	}
	QDialog::keyPressEvent(e);
}

void sciFindDialog::setReplaceMode(bool replaceMode) 
{
	if (replaceMode) {
		uiFind.findBtn->setText(tr("Replace"));
		//uiFind.findBtn->setIcon(IconManager::instance()->getIcon(ID_REPLACE));
		setWindowTitle(tr("Replace"));
		uiFind.replaceCmb->setFocus();
		uiFind.replaceCmb->lineEdit()->selectAll();
	}
	else {
		uiFind.findBtn->setText(tr("Find"));
		//uiFind.findBtn->setIcon(IconManager::instance()->getIcon(ID_FIND));
		setWindowTitle(tr("Find"));
	}
	if (uiFind.replaceChk->isChecked() != replaceMode)
		uiFind.replaceChk->setChecked(replaceMode);
}

void sciFindDialog::multiLineChecked(bool chk) 
{
	if ( chk ) {
		uiFind.lowerSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
		uiFind.mlEd->setText(uiFind.findCmb->currentText());
	}
	else {
		uiFind.findCmb->setEditText(uiFind.mlEd->toPlainText());
		uiFind.lowerSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	}
}

void sciFindDialog::regExpChecked(bool chk) 
{
	if ( !chk ) {
		if ( uiFind.multiLineChk->isChecked() ) {
			multiLine_ = true;
			uiFind.multiLineChk->setChecked(false);
		}
	}
	else {
		uiFind.multiLineChk->setChecked(multiLine_);
	}
}
