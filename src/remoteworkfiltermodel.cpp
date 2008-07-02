#include "remoteworkfiltermodel.hpp"
#include "applicationmodel.hpp"
#include "workfilterlist.hpp"
#include "workfiltermodel.hpp"
#include <QApplication>

RemoteWorkFilterModel::RemoteWorkFilterModel(std::string name, std::string description){
	if(!QMetaType::type("QList<int>"))
		qRegisterMetaType<QList<int> >("QList<int>");

	//create our proxy and set its name and description
	RemoteWorkFilterModelProxy * proxy = new RemoteWorkFilterModelProxy;
	proxy->setName(QString(name.c_str()));
	proxy->setDescription(QString(description.c_str()));

	//move the proxy to the main thread
	proxy->moveToThread(QApplication::instance()->thread());

	//connect ourselves to our proxy
	QObject::connect(this, SIGNAL(workListing(QList<int>)),
			proxy, SLOT(setWorks(QList<int>)),
			Qt::QueuedConnection);
	QObject::connect(proxy, SIGNAL(requestingUpdate()),
			this, SLOT(reportWorks()),
			Qt::QueuedConnection);
	QObject::connect(this, SIGNAL(deleteingSelf()),
			proxy, SLOT(removeSelf()),
			Qt::BlockingQueuedConnection);

	//insert us (via our proxy) into the filter list
	QMetaObject::invokeMethod(ApplicationModel::instance()->workFilterList(), 
			"addFilter", Qt::QueuedConnection, Q_ARG(WorkFilterModel *, proxy));
}

RemoteWorkFilterModel::~RemoteWorkFilterModel(){
	//make sure our proxy knows that we are going away...
	emit(deleteingSelf());
	//XXX delete our proxy?
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

