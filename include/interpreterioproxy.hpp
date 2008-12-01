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

#ifndef INTEPRETER_IO_PROXY_HPP
#define INTEPRETER_IO_PROXY_HPP

#include <QObject>

//this class simply acts as a way to get text to and from an
//interpreter that is running in another thread [and using another language]
//it connects itself to the application model's intepreter model

class InterpreterIOProxy : public QObject {
	Q_OBJECT
	protected:
		InterpreterIOProxy();
		InterpreterIOProxy(const InterpreterIOProxy&);
		InterpreterIOProxy& operator= (const InterpreterIOProxy&);
		static QList<QString> mInputList;
		static InterpreterIOProxy * cInstance;
	public:
		static InterpreterIOProxy * instance();
		static bool newInput();
		static std::string getInput();
		static void processEvents();
	signals:
		void newOutput(QString output);
	public slots:
		static void addToInput(QString input);
		static void cancelInput();
		static void addToOutput(std::string output);
};

#endif

