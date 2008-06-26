#include "applicationmodel.hpp"
#include "audioworktablemodel.hpp"
#include "mixerpanelmodel.hpp"
#include "tagmodel.hpp"
#include "workfiltermodel.hpp"
#include "workfilterlist.hpp"

ApplicationModel::ApplicationModel(unsigned int num_mixers, QSqlDatabase db, QObject * parent):
	QObject(parent)
{
	mDB = db;
	mAudioWorkTable = new AudioWorkTableModel(db);
	mMixerPanel = new MixerPanelModel(num_mixers);
	mTagModel = new TagModel(db);
	mFilterProxy = new WorkFilterModelProxy(mAudioWorkTable);
	mWorkFilterList = new WorkFilterList(this);

	mWorkFilterList->addFilter(new TagSelectionFilter(this));
	mWorkFilterList->addFilter(new TagSelectionFilter(this));
	mWorkFilterList->addFilter(new TagSelectionFilter(this));
	mWorkFilterList->addFilter(new TagSelectionFilter(this));
	mWorkFilterList->addFilter(new TagSelectionFilter(this));
}

QSqlDatabase ApplicationModel::db() const {
	return mDB;
}

AudioWorkTableModel * ApplicationModel::audioWorkTable() const {
	return mAudioWorkTable;
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

