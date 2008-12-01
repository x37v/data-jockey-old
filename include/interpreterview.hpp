/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INTERPRETER_VIEW_HPP
#define INTERPRETER_VIEW_HPP

#include <QWidget>

class QLineEdit;
class QTextEdit;
class QKeyEvent;
class InterpreterModel;

class InterpreterView : public QWidget {
	Q_OBJECT
	public:
		InterpreterView(InterpreterModel * model, QWidget * parent = NULL);
		virtual void keyPressEvent ( QKeyEvent * event );
	protected:
		virtual bool eventFilter(QObject *obj, QEvent *ev);
	signals:
		void newInput(QString line);
		void cancelingInput();
	public slots:
		void setInput(QString line);
		void addToOutput(QString line);
	protected slots:
		void acceptNewInput();
		void cancelInput();
	private:
		//mWaitingForOutput is true when we've sent at least one line
		//before getting any input back.. it helps us set the prompt
		//on the input display
		bool mWaitingForOutput;
		QLineEdit * mTextEntry;
		QTextEdit * mOutputDisplay;
		//zero means current, use mHistoryIndex - 1 to get history line
		unsigned int mHistoryIndex;
		QString mCurrentInput;
		InterpreterModel * mModel;
};

#endif
