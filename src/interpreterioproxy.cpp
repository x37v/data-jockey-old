#include "interpreterioproxy.hpp"
#include "interpretermodel.hpp"
#include "applicationmodel.hpp"
#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdexcept>
#include <QCoreApplication>

InterpreterIOProxy::InterpreterIOProxy(){
	struct stat stat_buf;

	//does the file exist?
	if(stat(DATAJOCKEY_INTERP_PIPE_LOC, &stat_buf)){
		//create our pipe if it doesn't exist
		QString cmd = QString("mkfifo %1").arg(DATAJOCKEY_INTERP_PIPE_LOC);
		system(cmd.toStdString().c_str());
	} else if(!S_ISFIFO(stat_buf.st_mode)){
		//if it exists but isn't a fifo that is problem
		throw std::runtime_error("DATAJOCKEY_INTERP_PIPE_LOC exists but is not a fifo");
	}

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
		Qt::DirectConnection);
		//Qt::QueuedConnection);
}

std::string InterpreterIOProxy::pipeLocation(){
	return DATAJOCKEY_INTERP_PIPE_LOC;
}

void InterpreterIOProxy::processEvents(){
	QCoreApplication::processEvents();
}

/*
bool InterpreterIOProxy::newInput(){
	return !mInputList.empty();
}

std::string InterpreterIOProxy::getInput(){
	QString input;
	if(!mInputList.empty()){
		input = mInputList[0];
		mInputList.pop_front();
	}
	return input.toStdString();
}
*/

void InterpreterIOProxy::addToInput(QString input){
	//mInputList.push_back(input);
	
	//write to pipe
	std::ofstream fout(DATAJOCKEY_INTERP_PIPE_LOC);
	fout << input.toStdString() << std::endl;
	//fout.write(input.toStdString().c_str());
	fout.close();
}

void InterpreterIOProxy::addToOutput(std::string output){
	QString outputString(output.c_str());
	emit(newOutput(outputString));
}

