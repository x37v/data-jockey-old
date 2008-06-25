#include <QVariant>
#include <QList>
#include "tagmodel.hpp"
#include "treeitem.h"
#include <QSqlRecord>
#include <QStringList>

#define ID_COL 1

TagModelItemMimeData::TagModelItemMimeData(){
}

QStringList TagModelItemMimeData::formats() const {
	QStringList types;
	types << "application/tag-id-list";
	return types;
}

bool TagModelItemMimeData::hasFormat(const QString & mimeType ) const {
	if(mimeType == "application/tag-id-list")
		return true;
	else
		return false;
}

QVariant TagModelItemMimeData::retrieveData ( const QString & mimeType, QVariant::Type type ) const {
	if(mimeType == "application/tag-id-list" && type == QVariant::List){
		QList<QVariant> intList;
		for(int i = 0; i < mData.size(); i++)
			intList.push_back(mData[i]);
		return QVariant::fromValue(intList);
	} else 
		return QVariant();
}

void TagModelItemMimeData::addItem(int id){
	mData.push_back(id);
}


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
	setSupportedDragActions(Qt::ActionMask);
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

Qt::ItemFlags TagModel::flags(const QModelIndex &index) const {
	Qt::ItemFlags defaultFlags = TreeModel::flags(index);

	if (index.isValid()) {
		//if it isn't a tag class then it can be dragged
		if(index.parent().isValid() && index.parent().internalPointer() != root())
			return Qt::ItemIsDragEnabled | defaultFlags;
		else
			return defaultFlags;
	} else
		return defaultFlags;
}

QStringList TagModel::mimeTypes() const {
	QStringList types;
	types << "application/tag-id-list";
	return types;
}

QMimeData * TagModel::mimeData ( const QModelIndexList & indexes ) const {
	TagModelItemMimeData * data = new TagModelItemMimeData;
	foreach(QModelIndex index, indexes){
		//add the index to our data list, only if it isn't a class
		if(index.isValid() && 
				index.parent().isValid() &&
				index.parent().internalPointer() != root())
			data->addItem(index.sibling(index.row(), ID_COL).data().toInt());
	}
	return data;
}

void TagModel::addWorkTagAssociation(int work_id, int tag_id){
	QString queryStr;
	//first make sure this association doesn't already exist
	queryStr.sprintf("select * from tags\n"
			"\tjoin audio_work_tags on tags.id = audio_work_tags.tag_id\n"
			"where tags.id = %d AND audio_work_tags.audio_work_id = %d", tag_id, work_id);
	mAddTagQuery.exec(queryStr);
	//if it doesn't exist, then insert it
	if(!mAddTagQuery.first()){
		queryStr.sprintf("insert into audio_work_tags (audio_work_id, tag_id) values(%d, %d)", work_id, tag_id);
		mAddTagQuery.exec(queryStr);
	}
}

void TagModel::removeWorkTagAssociation(int work_id, int tag_id){
	QString queryStr;
	queryStr.sprintf("delete from audio_work_tags where tag_id = %d AND audio_work_id = %d", tag_id, work_id);
	mAddTagQuery.exec(queryStr);
}

void TagModel::removeWorkTagAssociation(int work_id, QModelIndex tag_index){
	if(tag_index.isValid() && 
			tag_index.parent().isValid() && 
			tag_index.parent().internalPointer() != root()){
			int tag_id = tag_index.sibling(tag_index.row(), ID_COL).data().toInt();
			removeWorkTagAssociation(work_id, tag_id);
	}
}

void TagModel::addTag(int classId, QString tagName){
	QString queryStr;

	//don't add a dup tag
	queryStr.sprintf("select * from tags where name = \"%s\" and tag_class_id = %d", tagName.toStdString().c_str(), classId);
	mAddTagQuery.exec(queryStr);
	if(mAddTagQuery.first())
		return;

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
