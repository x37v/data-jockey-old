#include "applicationmodel.hpp"
#include "worktablemodel.hpp"
#include "mixerpanelmodel.hpp"
#include "tagmodel.hpp"
#include "workfiltermodel.hpp"
#include "workfilterlist.hpp"

#include <stdexcept>

unsigned int ApplicationModel::cNumMixers = 0;
QSqlDatabase ApplicationModel::cDB;
ApplicationModel * ApplicationModel::cInstance = NULL;

void ApplicationModel::setNumberOfMixers(unsigned int num){
	if(num == 0)
		throw std::logic_error("number of mixers must be greater than 0");
	if(cNumMixers == 0)
		cNumMixers = num;
}

void ApplicationModel::setDataBase(QString type, 
		QString name, 
		QString password, 
		int port, 
		QString host){
	cDB = QSqlDatabase::addDatabase(type);
	cDB.setDatabaseName(name);
	if(!cDB.open())
		throw std::runtime_error("cannot open database");
}

ApplicationModel * ApplicationModel::instance(){
	if(!cInstance)
		cInstance = new ApplicationModel();
	return cInstance;
}

ApplicationModel::ApplicationModel()
{
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

ApplicationModelProxy::ApplicationModelProxy(Qt::ConnectionType type, QObject * parent) :
	QObject(parent)
{
	ApplicationModel * model = ApplicationModel::instance();
	//sync our mixer panel
	mMixerPanel = new MixerPanelModel(model->mixerPanel()->numMixerChannels());
	mMixerPanel->syncToModel(model->mixerPanel());
}

MixerPanelModel * ApplicationModelProxy::mixerPanel() const {
	return mMixerPanel;
}

