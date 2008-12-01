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

#include "interpreterioproxy.hpp"
#include "interpretermodel.hpp"
#include "applicationmodel.hpp"
#include <iostream>
#include <fstream>

#include <stdexcept>
#include <QCoreApplication>

QList<QString> InterpreterIOProxy::mInputList;
InterpreterIOProxy * InterpreterIOProxy::cInstance = NULL;

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
	QObject::connect(
		ApplicationModel::instance()->interpreter(),
		SIGNAL(cancelingInput()),
		this,
		SLOT(cancelInput()),
		Qt::QueuedConnection);
}

InterpreterIOProxy * InterpreterIOProxy::instance(){
	if(cInstance == NULL)
		cInstance = new InterpreterIOProxy();
	return cInstance;
}

void InterpreterIOProxy::processEvents(){
	QCoreApplication::processEvents();
}

bool InterpreterIOProxy::newInput(){
	return !instance()->mInputList.empty();
}

std::string InterpreterIOProxy::getInput(){
	if(!cInstance)
		instance();
	QString input;
	if(mInputList.empty())
		return "";
	input = mInputList[0];
	mInputList.pop_front();
	return input.toStdString();
}

void InterpreterIOProxy::addToInput(QString input){
	if(!cInstance)
		instance();
	mInputList.push_back(input);
}

void InterpreterIOProxy::cancelInput(){
	if(!cInstance)
		instance();
	mInputList.push_back("DATAJOCKEY_CANCEL_INPUT");
}

void InterpreterIOProxy::addToOutput(std::string output){
	QString outputString(output.c_str());
	instance()->newOutput(outputString);
}

