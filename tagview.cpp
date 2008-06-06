#include "tagview.hpp"
#include "treemodel.h"
#include <QSqlRecord>

const QString TagView::cAllTagsStr(
		"select tags.id id,\n"
		"\ttag_classes.id class_id,\n"
		"\ttag_classes.name class,\n"
		"\ttags.name name\n"
		"from tags\n"
		"\tinner join tag_classes on tags.tag_class_id = tag_classes.id\n"
		"order by class, name\n");

const QString TagView::cSingleWorkStr(
		"select audio_work_tags.id id,\n"
		"\ttag_classes.id class_id,\n"
		"\ttag_classes.name class,\n"
		"\ttags.name name\n"
		"from audio_work_tags\n"
		"\tinner join tags on tags.id = audio_work_tags.tag_id\n"
		"\tinner join tag_classes on tags.tag_class_id = tag_classes.id\n"
		"where audio_work_tags.audio_work_id = ");

const QString TagView::cOrderStr("\norder by class, name\n");

TagView::TagView(const QSqlDatabase & db, QWidget * parent) :
	QTreeView(parent), mQuery(db)
{
	mModel = new TreeModel(this);
}

void TagView::setWork(int work_id){
	//build up the query
	QString queryStr(cSingleWorkStr);
	QString id;
	id.setNum(work_id);
	queryStr.append(id);
	queryStr.append(cOrderStr);

	//exec and build the model
	mQuery.exec(queryStr);
	buildFromQuery();
}

void TagView::setShowAll(){
	mQuery.exec(cAllTagsStr);
	buildFromQuery();
}

void TagView::clear(){
}

void TagView::buildFromQuery(){
	if(mQuery.first()){
		QSqlRecord rec = mQuery.record();
		int idCol = rec.indexOf("id");
		int classIdCol = rec.indexOf("class_id");
		int classCol = rec.indexOf("class");
		int nameCol = rec.indexOf("name");
		do {
		} while(mQuery.next());
	} else {
	}
}

