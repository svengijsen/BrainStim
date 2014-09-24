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


#ifndef CUSTOMQSCISCINTILLA_H
#define CUSTOMQSCISCINTILLA_H

#include <QObject>
#include <QtWidgets>
#include <QEvent>
#include <QKeyEvent>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciprinter.h>
#include "scifinddialog.h"
#include "globalapplicationinformation.h"

struct QSciCommandSetDef
{
	int nKey;
	int nAltKey;
	QString sDescription;
	QsciCommand::Command eCommand;
};

class CustomQsciScintilla : public QsciScintilla//, QObject
{
	Q_OBJECT

	public:
		CustomQsciScintilla(GlobalApplicationInformation::DocType docType);
		~CustomQsciScintilla();

		void setManagerObject(QObject *managerObject,QObject *childWindowObject);
		void toggleLineComment();
		void toggleBlockComment();
		void commentLine(int line, const QString& str1, const QString& comment); 
		void uncommentLine(int line, const QString& str1, const QString& comment);
		void replaceSelectedText(const QString& text); 
		void insertText(const QString& text); 
		bool print();
		void toggleMarker(int line);
		void toggleMarker();
		void nextMarker();
		void prevMarker();
		void removeAllMarkers();
		void gotoLine(int line);
		int lineCount() const;
		QList<int> markers() const;
		QString getLine(int line) const;
		QString wordUnderCursor();
		void find(const QString& str, const DocFindFlags& flags);
		void startFind(const QString& str, const DocFindFlags& flags); 
		void replace(const QString& str1, const QString& str2, const DocFindFlags& flags, bool replaceAll = false);
		void startReplace(const QString& str1, const QString& str2, const DocFindFlags& flags, bool& replaceAll, int& count);

	public slots:

#ifdef DEBUG
		QString testFunction(QString inp = "");
#endif

	protected:
		void closeEvent(QCloseEvent *event);

	private:
		bool createAllowedCommandKeyList();
		//bool eventFilter(QObject * obj, QEvent * ev);
		void prepareForFind(const QString& s, const DocFindFlags& flags);
		bool findsci(const QString& s, const DocFindFlags& flags);
		bool findML(const QString& s, const DocFindFlags& flags);
		long curPos() const;
		long lineColToPos(int line, int col) const;
		void posToLineCol(long pos, int& line, int& col) const;
		void stepOver(bool back);
		int doReplace(const QString& str1, const QString& str2, const DocFindFlags& flags, bool& replaceAll);
		void replaceSelected(const QString& targetText, bool backwards);

		QObject *manager;
		QObject *childWindow;
		QList<QsciCommand::Command> lAllowedCommands;

		int searchStartingLine_;
		int searchStartingCol_;
		bool searchSteppedOver_;
		int searchStartingScroll_;
};

#endif
