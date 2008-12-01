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

#ifndef INTEPRETER_MODEL_HPP
#define INTEPRETER_MODEL_HPP

#include <QObject>
#include <QList>
#include <QString>
#include <QFile>

class InterpreterModel : public QObject {
	Q_OBJECT
	public:
		InterpreterModel(QObject * parent = NULL);
		//returns the number of input lines that we have in our history
		unsigned int inputHistoryLines();
		QString line(unsigned int lineno);
	signals:
		void newOutput(QString output);
		void newInput(QString output);
		void cancelingInput();
	public slots:
		void addToInput(QString input);
		void cancelInput();
		void addToOutput(QString output);
		void setHistoryFile(QString filename);
		void writeHistory();
	private:
		QList<QString> mInput;
		QList<QString> mInputHistory;
		QList<QString> mOutputHistory;
		QFile mHistoryFile;
};

#endif

