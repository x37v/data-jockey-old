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

#include "interpretermodel.hpp"

InterpreterModel::InterpreterModel(QObject * parent) :
	QObject(parent)
{
	//XXX set a default file name
	mHistoryFile.setFileName("/tmp/datajockey_history");
}

unsigned int InterpreterModel::inputHistoryLines(){
	return mInput.size();
}

QString InterpreterModel::line(unsigned int lineno){
	if(lineno < (unsigned int)mInput.size())
		return mInputHistory[lineno];
	else
		return QString();
}

void InterpreterModel::addToInput(QString input){
	mInput.push_back(input);
	mInputHistory.push_front(input);
	emit(newInput(input));
}

void InterpreterModel::cancelInput(){
	emit(cancelingInput());
}

void InterpreterModel::addToOutput(QString output){
	mOutputHistory.push_back(output);
	emit(newOutput(output));
}

void InterpreterModel::setHistoryFile(QString filename){
	mHistoryFile.setFileName(filename);
}

void InterpreterModel::writeHistory(){
	//XXX write to file!
}

