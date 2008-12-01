/*
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

#include "applicationmodel.hpp"
#include "worktablemodel.hpp"
#include "mixerpanelmodel.hpp"
#include "tagmodel.hpp"
#include "workfiltermodel.hpp"
#include "workfilterlist.hpp"
#include "interpretermodel.hpp"
#include "remoteworkfiltermodel.hpp"
#include "oscreceiver.hpp"

#include <stdexcept>

unsigned int ApplicationModel::cNumMixers = 0;
QSqlDatabase ApplicationModel::cDB;
ApplicationModel * ApplicationModel::cInstance = NULL;

void ApplicationModel::setNumberOfMixers(unsigned int num){ if(num == 0)
		throw std::logic_error("number of mixers must be greater than 0");
	if(cNumMixers == 0)
		cNumMixers = num;
}

void ApplicationModel::setDataBase(std::string type, 
		std::string name, 
		std::string username,
		std::string password, 
		int port, 
		std::string host){

	cDB = QSqlDatabase::addDatabase(type.c_str());

	cDB.setDatabaseName(name.c_str());

	if(username != "")
		cDB.setUserName(username.c_str());

	if(password != "")
		cDB.setPassword(password.c_str());

	if(!cDB.open())
		throw std::runtime_error("cannot open database");
}

ApplicationModel * ApplicationModel::instance(){
	if(!cInstance)
		cInstance = new ApplicationModel();
	return cInstance;
}

ApplicationModel::ApplicationModel() {
	if(!cDB.isOpen() || cNumMixers == 0){
		throw std::logic_error("you must set the number of mixers and set up the database before creating an instance of ApplicationModel");
	}
	//XXX make sure that the database is set up and the number of mixers are
	//also set up
	mWorkTable = new WorkTableModel(cDB, this);
	mMixerPanel = new MixerPanelModel(cNumMixers, this);
	mTagModel = new TagModel(cDB, this);
	mFilterProxy = new WorkFilterModelProxy(mWorkTable);
	mWorkFilterList = new WorkFilterList(this);
	mInterp = new InterpreterModel(this);
	MixerPanelModel * oscMixerPanelModel = new MixerPanelModel;
	oscMixerPanelModel->syncToModel(mMixerPanel, Qt::QueuedConnection);
	mOscReceiver = new OscReceiver(oscMixerPanelModel);

	//make internal connections
	QObject::connect(
			mWorkFilterList, SIGNAL(selectionChanged(WorkFilterModel *)),
			mFilterProxy, SLOT(setFilter(WorkFilterModel *)));
}

ApplicationModel::~ApplicationModel(){
	cDB.close();
}

QSqlDatabase ApplicationModel::db() const {
	return cDB;
}

WorkTableModel * ApplicationModel::WorkTable() const {
	return mWorkTable;
}

MixerPanelModel * ApplicationModel::mixerPanel() const {
	return mMixerPanel;
}

TagModel * ApplicationModel::tagModel() const {
	return mTagModel;
}

WorkFilterModelProxy * ApplicationModel::filteredWorkTable() const {
	return mFilterProxy;
}

WorkFilterList * ApplicationModel::workFilterList() const {
	return mWorkFilterList;
}

InterpreterModel * ApplicationModel::interpreter() const {
	return mInterp;
}

OscReceiver * ApplicationModel::oscReceiver() const {
	return mOscReceiver;
}

ApplicationModelProxy::ApplicationModelProxy(Qt::ConnectionType type, QObject * parent) :
	QObject(parent)
{
	ApplicationModel * model = ApplicationModel::instance();
	//sync our mixer panel
	mMixerPanel = new MixerPanelModel(model->mixerPanel()->numMixerChannels());
	mMixerPanel->syncToModel(model->mixerPanel(), type);
}

MixerPanelModel * ApplicationModelProxy::mixerPanel() const {
	return mMixerPanel;
}

void ApplicationModelProxy::addFilter(RemoteWorkFilterModel * filter){
	//if it isn't already in there, do your work
	if(!mFilterList.contains(filter)){
		mFilterList.push_back(filter);
		//insert it (via the proxy) into the filter list
		QMetaObject::invokeMethod(ApplicationModel::instance()->workFilterList(), 
				"addFilter", Qt::QueuedConnection, Q_ARG(WorkFilterModel *, filter->proxy()));
	}
}

void ApplicationModelProxy::removeFilter(RemoteWorkFilterModel * filter){
	if(mFilterList.contains(filter)){
		//remove it (via the proxy) into the filter list
		QMetaObject::invokeMethod(ApplicationModel::instance()->workFilterList(), 
				"removeFilter", Qt::QueuedConnection, Q_ARG(WorkFilterModel *, filter->proxy()));
		QMetaObject::invokeMethod(ApplicationModel::instance()->filteredWorkTable(), 
				"setFilter", Qt::QueuedConnection, Q_ARG(WorkFilterModel *, NULL));
		mFilterList.removeAll(filter);
	}
}

void ApplicationModelProxy::removeFilter(unsigned int i){
	if(i < (unsigned int)mFilterList.size())
		removeFilter(mFilterList[i]);
}

unsigned int ApplicationModelProxy::numFilters(){
	return (unsigned int)mFilterList.size();
}

RemoteWorkFilterModel * ApplicationModelProxy::filter(unsigned int i){
	if(i >= (unsigned int)mFilterList.size())
		return NULL;
	else
		return mFilterList[i];
}

