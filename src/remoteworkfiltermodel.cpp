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

#include "remoteworkfiltermodel.hpp"
#include "applicationmodel.hpp"
#include "workfilterlist.hpp"
#include "workfiltermodel.hpp"
#include <QApplication>

RemoteWorkFilterModel::RemoteWorkFilterModel(std::string name, std::string description){
	if(!QMetaType::type("QList<int>"))
		qRegisterMetaType<QList<int> >("QList<int>");

	//create our proxy and set its name and description
	mProxy = new RemoteWorkFilterModelProxy;
	mProxy->setName(QString(name.c_str()));
	mProxy->setDescription(QString(description.c_str()));

	//move the proxy to the main thread
	mProxy->moveToThread(QApplication::instance()->thread());

	//connect ourselves to our proxy
	QObject::connect(this, SIGNAL(workListing(QList<int>)),
			mProxy, SLOT(setWorks(QList<int>)),
			Qt::QueuedConnection);
	QObject::connect(mProxy, SIGNAL(requestingUpdate()),
			this, SLOT(reportWorks()),
			Qt::QueuedConnection);
	QObject::connect(this, SIGNAL(deleteingSelf()),
			mProxy, SLOT(removeSelf()),
			Qt::BlockingQueuedConnection);
}

RemoteWorkFilterModel::~RemoteWorkFilterModel(){
	//make sure our proxy knows that we are going away...
	emit(deleteingSelf());
	//schedule the proxy to be deleted
	mProxy->deleteLater();
}

void RemoteWorkFilterModel::clearWorks(){
	mSelectedWorks.clear();
}

void RemoteWorkFilterModel::addWork(int id){
	mSelectedWorks.push_back(id);
}

void RemoteWorkFilterModel::setWorks(std::vector<int> idlist){
	mSelectedWorks.clear();
	for(unsigned int i = 0; i < idlist.size(); i++)
		mSelectedWorks.push_back(idlist[i]);
	emit(workListing(mSelectedWorks));
}

RemoteWorkFilterModelProxy * RemoteWorkFilterModel::proxy() const {
	return mProxy;
}

void RemoteWorkFilterModel::reportWorks(){
	emit(workListing(mSelectedWorks));
}

// ******* RemoteWorkFilterModelProxy **********

RemoteWorkFilterModelProxy::RemoteWorkFilterModelProxy(QObject * parent) :
	WorkFilterModel(ApplicationModel::instance(), parent)
{
	mName = QString(tr("Remote Filter Model"));
	mDescription = QString(tr("This is a filter that is executed in another thread, "
				"usually by an interpreter in another language."));
	mUpdated = false;
}

RemoteWorkFilterModelProxy::~RemoteWorkFilterModelProxy(){
	removeSelf();
}

//if we're not up to date with our list of works then we request an update
//and deny being used as a filter until we've been updated
bool RemoteWorkFilterModelProxy::beforeFilter(){
	if(!mUpdated)
		emit(requestingUpdate());
	return mUpdated;
}

bool RemoteWorkFilterModelProxy::acceptsWork(int work_id){
	//once we execute the filter we're no longer up to date
	mUpdated = false;
	//if it isn't in the mSelectedWorks then we don't accept it
	if(mSelectedWorks.empty() || mSelectedWorks.find(work_id) == mSelectedWorks.end())
		return false;
	else
		return true;
}

std::string RemoteWorkFilterModelProxy::description(){
	return mDescription.toStdString();
}

std::string RemoteWorkFilterModelProxy::name(){
	return mName.toStdString();
}

void RemoteWorkFilterModelProxy::setWorks(QList<int> idlist){
	mSelectedWorks.clear();
	foreach(int id, idlist){
		mSelectedWorks.insert(id);
	}
	mUpdated = true;

	//if we are the currently selected filter then refilter
	if(ApplicationModel::instance()->workFilterList()->selected() == this)
		ApplicationModel::instance()->filteredWorkTable()->filter();
}

void RemoteWorkFilterModelProxy::setName(QString name){
	mName = name;
}

void RemoteWorkFilterModelProxy::setDescription(QString description){
	mDescription = description;
}

void RemoteWorkFilterModelProxy::removeSelf(){
	//if we are the current work filter than set the work filter to null
	if(ApplicationModel::instance()->filteredWorkTable()->workFilter() == this)
		ApplicationModel::instance()->filteredWorkTable()->setFilter(NULL);
	//remove us from the filter model list
	ApplicationModel::instance()->workFilterList()->removeFilter(this);
}

