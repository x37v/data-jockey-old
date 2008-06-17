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
	TreeModel(parent), mQuery(cQueryStr, db), mAddTagQuery("", db)
{
	mDb = db;
	mClassList = new QList<QPair<int, QString> *>;
	mQuery.exec(cQueryStr);
	QList<QVariant> rootData;
	rootData << "empty";
	setRoot(new TreeItem(rootData));
	buildFromQuery();
}

TagModel::~TagModel(){
	//XXX delete mClassList and its contents;
}

QSqlDatabase TagModel::db() const {
	return mDb;
}

int TagModel::idColumn(){
	return ID_COL;
}

QList<QPair<int, QString> *> * TagModel::classList() const {
	return mClassList;
}

#include <iostream>
using namespace std;

void TagModel::addTag(int classId, QString tagName){

	QString queryStr;
	queryStr.sprintf("insert into tags (name, tag_class_id) values(\"%s\", %d)", tagName.toStdString().c_str(), classId);
	mAddTagQuery.exec(queryStr);

	//grab the tag id
	queryStr.sprintf("select id from tags where name = \"%s\" and tag_class_id = %d", tagName.toStdString().c_str(), classId);
	mAddTagQuery.exec(queryStr);

	if(mAddTagQuery.first()){
		//find the parent
		TreeItem * parent = NULL;
		//classes are children of the root
		for(int i = 0; i < root()->childCount(); i++){
			TreeItem * child = root()->child(i);
			//if the child's id is the same as our class id.. we've found it
			if(child->data(TagModel::idColumn()).toInt() == classId){
				parent = child;
				break;
			}
		}
		if(parent != NULL){
			beginInsertRows(createIndex(parent->row(), 0, parent), parent->childCount(), parent->childCount());
			QList<QVariant> itemData;
			itemData << tagName << mAddTagQuery.value(0).toInt();
			TreeItem * newTag = new TreeItem(itemData, parent);
			parent->appendChild(newTag);
			endInsertRows();
		} 
	} 

	//mQuery.exec(cQueryStr);
	//buildFromQuery();
}

void TagModel::addClassAndTag(QString className, QString tagName){
	QString queryStr;

	//make sure we don't already have the class
	queryStr.sprintf("select id from tag_classes where name = \"%s\"", 
			className.toStdString().c_str());
	mAddTagQuery.exec(queryStr);
	//if we already have that class then we just call the addTag with that
	//class_id and the tagName
	if(mAddTagQuery.first()){
		addTag(mAddTagQuery.value(0).toInt(), tagName);
		return;
	}

	//insert the tag class
	queryStr.sprintf("insert into tag_classes set name=\"%s\"", className.toStdString().c_str());
	mAddTagQuery.exec(queryStr);

	//insert the tag
	queryStr.sprintf("insert into tags set name=\"%s\", tag_class_id = (select id from tag_classes where name = \"%s\")",
			tagName.toStdString().c_str(), className.toStdString().c_str());
	mAddTagQuery.exec(queryStr);

	queryStr.sprintf("select tags.id id, tags.tag_class_id class_id from tags\n"
			"\njoin tag_classes on tags.tag_class_id = tag_classes.id\n"
			"where tags.name = \"%s\" and tag_classes.name = \"%s\"",
			tagName.toStdString().c_str(), className.toStdString().c_str());
	mAddTagQuery.exec(queryStr);
	
	//if we got the data then create our new items
	if(mAddTagQuery.first()){
		//QModelIndex rootIndex = createIndex(root()->row(), 0, root());
		//beginInsertRows(rootIndex, root()->childCount(), root()->childCount());

		//populate the tag class and tag item data..
		QList<QVariant> parentData;
		QList<QVariant> itemData;
		parentData << className << mAddTagQuery.value(1).toInt();
		itemData << tagName << mAddTagQuery.value(0).toInt();

		//create the nodes and insert them
		TreeItem * parent = new TreeItem(parentData, root());
		parent->appendChild(new TreeItem(itemData, parent));
		root()->appendChild(parent);

		//endInsertRows();
		reset();
		QPair<int, QString> classInfo(mAddTagQuery.value(1).toInt(), className);
		emit(classAdded(classInfo));
	}
}

void TagModel::buildFromQuery(){
	mClassList->clear();
	if(mQuery.first()){
		QSqlRecord rec = mQuery.record();
		int idCol = rec.indexOf("id");
		int classIdCol = rec.indexOf("class_id");
		int classCol = rec.indexOf("class");
		int nameCol = rec.indexOf("name");

		QList<QVariant> rootData;
		rootData << "tag (class -> name)" << "id";
		TreeItem * newRoot = new TreeItem(rootData);

		//create the first class item
		QList<QVariant> parentData;
		parentData << mQuery.value(classCol).toString() << mQuery.value(classIdCol).toInt() ;
		TreeItem * curParent = new TreeItem(parentData, newRoot);
		newRoot->appendChild(curParent);

		QPair<int, QString> * newClassListItem = new QPair<int, QString>(
				mQuery.value(classIdCol).toInt(),
				mQuery.value(classCol).toString());
		mClassList->push_back(newClassListItem);

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
				newClassListItem = new QPair<int, QString>(classId, className);
				mClassList->push_back(newClassListItem);
			}
			//create the child (tag)
			childData << name << id;
			curParent->appendChild(new TreeItem(childData, curParent));
		} while(mQuery.next());
		setRoot(newRoot);
	} else {
	}
	reset();
}
