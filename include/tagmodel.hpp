#ifndef TAG_MODEL_HPP
#define TAG_MODEL_HPP

#include "treemodel.h"
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPair>
#include <QList>
#include <QMimeData>
#include <QStringList>
#include <QList>

//our own mime type for ids
class TagModelItemMimeData : public QMimeData {
	Q_OBJECT
	public:
		TagModelItemMimeData();
		virtual QStringList formats() const;
		virtual bool hasFormat(const QString & mimeType ) const;
		virtual QVariant retrieveData ( const QString & mimeType, QVariant::Type type ) const;
		void addItem(int id);
	private:
		QList<int> mData;
};

class TagModel : public TreeModel {
	Q_OBJECT
	public:
		TagModel(const QSqlDatabase & db = QSqlDatabase(),
				QObject * parent = NULL);
		virtual ~TagModel();
		static int idColumn();
		static int countColumn();
		QSqlDatabase db() const;
		QList<QPair<int, QString> *> * classList() const;
		//this is how you associate a tag with a work
		void addWorkTagAssociation(int work_id, int tag_id);
		//delete an association between a tag and a work
		void removeWorkTagAssociation(int work_id, int tag_id);
		void removeWorkTagAssociation(int work_id, QModelIndex tag_index);
		//the below are here to allow draging of data
		Qt::ItemFlags flags(const QModelIndex &index) const;
		virtual QStringList mimeTypes() const;
		virtual QMimeData * mimeData ( const QModelIndexList & indexes ) const;
		//idicates if this index is a tag or class..
		bool isTag(const QModelIndex index) const;
	signals:
		void classAdded(QPair<int, QString> classInfo);
	public slots:
		void addTag(int classId, QString tagName);
		void addClassAndTag(QString className, QString tagName);
	private:
		QList<QPair<int, QString> *> * mClassList;
		QSqlDatabase mDb;
		void buildFromQuery();
		const static QString cQueryStr;
		QSqlQuery mQuery;
		QSqlQuery mAddTagQuery;
		void updateTagCount(int tag_id);
};

#endif
