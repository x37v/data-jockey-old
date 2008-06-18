#include "worktagmodelfilter.hpp"
#include "tagmodel.hpp"
#include <QSqlQuery>
#include <QSqlRecord>

WorkTagModelFilter::WorkTagModelFilter(TagModel * model):
	QSortFilterProxyModel(model), mWorkTagsQuery("", model->db())
{
	setSourceModel(model);
	mWork = -1;
}

void WorkTagModelFilter::setWork(int work){
	if(mWork != work){
		mWork = work;

		QString queryStr("select tags.id id, tags.tag_class_id class_id from tags\n"
				"\tjoin audio_work_tags on audio_work_tags.tag_id = tags.id\n"
				"\tjoin audio_works on audio_works.id = audio_work_tags.audio_work_id\n"
				"\nwhere audio_works.id = ");
		QString id;
		id.setNum(mWork);
		queryStr.append(id);

		mTagClassIds.clear();
		mTagIds.clear();
		//populate the mTagClassIds and mTagIds sets
		mWorkTagsQuery.exec(queryStr);
		if(mWorkTagsQuery.first()){
			do {
				mTagIds.insert(mWorkTagsQuery.value(0).toInt());
				mTagClassIds.insert(mWorkTagsQuery.value(1).toInt());
			} while(mWorkTagsQuery.next());
		}

		invalidateFilter();
		emit(workChanged(mWork));
	}
}

void WorkTagModelFilter::clear() {
	mWork = -1;
	mTagClassIds.clear();
	mTagIds.clear();
	invalidateFilter();
	emit(workChanged(mWork));
}

bool WorkTagModelFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	//if my parent's model is the source model then this is a tag not a tag class
	if(sourceParent.model() == sourceModel()){
		QModelIndex rowIndex = sourceModel()->index(sourceRow, TagModel::idColumn(), sourceParent);
		if(mTagIds.find(rowIndex.data().toInt()) != mTagIds.end())
			return true;
		else
			return false;
	} else {
		//this is a parent..
		QModelIndex rowIndex = sourceModel()->index(sourceRow, TagModel::idColumn(), sourceParent);
		if(mTagClassIds.find(rowIndex.data().toInt()) != mTagClassIds.end())
			return true;
		else
			return false;
	}
}

