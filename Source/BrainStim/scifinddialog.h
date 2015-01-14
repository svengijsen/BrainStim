//BrainStim
//Copyright (C) 2015  Sven Gijsen
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


#ifndef SCIFINDDIALOG_H
#define SCIFINDDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QTextDocument>
#include <QtScript>
#include "ui_scifinddialog.h"

/*!
 * \defgroup sciFindDialogStructures Find/Replace Dialog Structures
 * @{
 */
/*! The structure (DocFindFlags) can store a specification for the Find/replace Dialog */
typedef struct _DocFindFlags 
{
	_DocFindFlags(bool Replace = false, bool MatchCase = false, bool Backwards = false, 
		bool IsRegExp = false, bool WholeWords = false, bool MultiLine = false) 
	{
		replace = Replace;
		matchCase = MatchCase;
		backwards = Backwards;
		isRegExp = IsRegExp;
		wholeWords = WholeWords;
		multiLine = MultiLine;
	}
	bool replace;		//!< Whether the search value should also be replaced.
	bool matchCase;		//!< Whether the search action is CaseSensitive.
	bool backwards;		//!< Whether the search action should go backwards from the current cursor position.
	bool isRegExp;		//!< Whether the search String contains a Regular Expression String.
	bool wholeWords;	//!< Whether the search action should only search for whole words.
	bool multiLine;		//!< Whether the serach action wraps around the end of the text.
} DocFindFlags;
/**@}*/

//Add the #include <QtScript>
//Declare the Type in Meta
Q_DECLARE_METATYPE(DocFindFlags)
//Next, the DocFindFlags conversion functions. We represent the DocFindFlags value as a script object and just copy the properties:

class sciFindDialog : public QDialog
{
	Q_OBJECT

public:
	sciFindDialog(QWidget*, bool);
	virtual ~sciFindDialog();

	QString text() const;
	QString replaceTo() const;
	DocFindFlags flags() const;
	bool isReplaceMode() const { return uiFind.replaceChk->isChecked(); }
	void setText(const QString&);

	static QString lastText() { return lastString_; }
	static QString lastReplaceText() { return lastReplaceText_; }
	static bool lastMatchCase() { return matchCase_; }
	static bool lastRegExpMode() { return regExpMode_; }
	static bool lastWholeWords() { return wholeWords_; }

	static QScriptValue DocFindFlagstoScriptValue(QScriptEngine *engine, const DocFindFlags &s)
	{
		QScriptValue obj = engine->newObject();
		obj.setProperty("replace", s.replace);
		obj.setProperty("matchCase", s.matchCase);
		obj.setProperty("backwards", s.backwards);
		obj.setProperty("isRegExp", s.isRegExp);
		obj.setProperty("wholeWords", s.wholeWords);
		obj.setProperty("multiLine", s.multiLine);
		return obj;
	}

	static void DocFindFlagsfromScriptValue(const QScriptValue &obj, DocFindFlags &s)
	{
		s.replace = obj.property("replace").toBoolean();
		s.matchCase = obj.property("matchCase").toBoolean();
		s.backwards = obj.property("backwards").toBoolean();
		s.isRegExp = obj.property("isRegExp").toBoolean();
		s.wholeWords = obj.property("wholeWords").toBoolean();
		s.multiLine = obj.property("multiLine").toBoolean();
	}

	static QScriptValue DocFindFlagsConstructor(QScriptContext *, QScriptEngine *engine)
	{
		DocFindFlags s;
		return engine->toScriptValue(s);
	}

public slots:
	void setReplaceMode(bool);

protected slots:
	void multiLineChecked(bool);
	void regExpChecked(bool);

protected:
	virtual void keyPressEvent(QKeyEvent*);

private:

	Ui::sciFindDialogClass uiFind;

	static QString lastString_;
	static QString lastReplaceText_;
	static bool matchCase_;
	static bool backward_;
	static bool regExpMode_;
	static bool wholeWords_;
	static bool multiLine_;
	static QStringList strings_;
	static QStringList replaces_;
};

#endif // SCIFINDDIALOG_H
