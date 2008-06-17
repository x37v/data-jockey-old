#include <QVariant>
#include <QList>
#include "tagmodel.hpp"
#include "treeitem.h"
#include <QSqlRecord>

#define ID_COL 1

const QString TagModel::cQueryStr(
		"select tags.id id,\n"
		"\ttag_classes.id class_id,\n"
		"\ttag_classes.name class,\n"
		"\ttags.name name\n"
		"from tags\n"
		"\tinner join tag_classes on tags.tag_class_id = tag_classes.id\n"
		"order by class, name\n");

TagModel::TagModel(const QSqlDatabase & db, QObject * parent) :
	TreeModel(parent), mQuery(cQueryStr, db)
{
	mDb = db;
	mQuery.exec(cQueryStr);
	QList<QVariant> rootData;
	rootData << "empty";
	setRoot(new TreeItem(rootData));
	buildFromQuery();
}

QSqlDatabase TagModel::db() const {
	return mDb;
}

int TagModel::idColumn(){
	return ID_COL;
}

void TagModel::buildFromQuery(){
	if(mQuery.first()){
		QSqlRecord rec = mQuery.record();
		int idCol = rec.indexOf("id");
		int classIdCol = rec.indexOf("class_id");
		int classCol = rec.indexOf("class");
		int nameCol = rec.indexOf("name");

		QList<QVariant> rootData;
		rootData << "tag (class -> name)" << "id";
		TreeItem * newRoot = new TreeItem(rootData);

		QList<QVariant> parentData;
		parentData << mQuery.value(classCol).toString() << mQuery.value(classIdCol).toInt() ;
		TreeItem * curParent = new TreeItem(parentData, newRoot);
		newRoot->appendChild(curParent);
		do {
			QList<QVariant> childData;
			int classId = mQuery.value(classIdCol).toInt();
			int id = mQuery.value(idCol).toInt();
			QString name(mQuery.value(nameCol).toString());
			QString className(mQuery.value(classCol).toString());

			//do we need a new parent? if so create it
			if(classId != parentData[1].toInt()){
				parentData.clear();
				parentData << className << classId;
				curParent = new TreeItem(parentData, newRoot);
				newRoot->appendChild(curParent);
			}
			//create the child (tag)
			childData << name << id;
			curParent->appendChild(new TreeItem(childData, curParent));
		} while(mQuery.next());
		setRoot(newRoot);
	} else {
	}
}
