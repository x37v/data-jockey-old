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

