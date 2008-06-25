#include "applicationmodel.hpp"
#include "audioworktablemodel.hpp"
#include "mixerpanelmodel.hpp"
#include "tagmodel.hpp"
#include "workfiltermodel.hpp"

ApplicationModel::ApplicationModel(unsigned int num_mixers, QSqlDatabase db, QObject * parent):
	QObject(parent)
{
	mDB = db;
	mAudioWorkTable = new AudioWorkTableModel(db);
	mMixerPanel = new MixerPanelModel(num_mixers);
	mTagModel = new TagModel(db);
	mFilterProxy = new WorkFilterModelProxy(mAudioWorkTable);
}

QSqlDatabase ApplicationModel::db(){
	return mDB;
}

AudioWorkTableModel * ApplicationModel::audioWorkTable(){
	return mAudioWorkTable;
}

MixerPanelModel * ApplicationModel::mixerPanel(){
	return mMixerPanel;
}

TagModel * ApplicationModel::tagModel(){
	return mTagModel;
}

WorkFilterModelProxy * ApplicationModel::filteredWorkTable(){
	return mFilterProxy;
}
