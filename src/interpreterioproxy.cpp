#include "interpreterioproxy.hpp"
#include "interpretermodel.hpp"
#include "applicationmodel.hpp"
#include <iostream>
#include <fstream>

#include <stdexcept>
#include <QCoreApplication>

InterpreterIOProxy::InterpreterIOProxy(){
	QObject::connect(this,
		SIGNAL(newOutput(QString)),
		ApplicationModel::instance()->interpreter(),
		SLOT(addToOutput(QString)),
		Qt::QueuedConnection);
	QObject::connect(
		ApplicationModel::instance()->interpreter(),
		SIGNAL(newInput(QString)),
		this,
		SLOT(addToInput(QString)),
		Qt::QueuedConnection);
		//Qt::DirectConnection);
}

void InterpreterIOProxy::processEvents(){
	QCoreApplication::processEvents();
}

bool InterpreterIOProxy::newInput(){
	return !mInputList.empty();
}

std::string InterpreterIOProxy::getInput(){
	QString input;
	if(mInputList.empty())
		return "";
	input = mInputList[0];
	mInputList.pop_front();
	return input.toStdString();
}

void InterpreterIOProxy::addToInput(QString input){
	mInputList.push_back(input);
}

void InterpreterIOProxy::addToOutput(std::string output){
	QString outputString(output.c_str());
	emit(newOutput(outputString));
}

