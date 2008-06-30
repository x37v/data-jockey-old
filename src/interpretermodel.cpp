#include "interpretermodel.hpp"

InterpreterModel::InterpreterModel(QObject * parent) :
	QObject(parent)
{
}

void InterpreterModel::addToInput(QString input){
	mInput.push_back(input);
	mInputHistory.push_back(input);
	emit(newInput(input));
}

void InterpreterModel::addToOutput(QString output){
	mOutputHistory.push_back(output);
	emit(newOutput(output));
}

