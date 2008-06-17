#ifndef TAG_MODEL_HPP
#define TAG_MODEL_HPP

#include "treemodel.h"
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPair>
#include <QList>

class TagModel : public TreeModel {
	Q_OBJECT
	public:
		TagModel(const QSqlDatabase & db = QSqlDatabase(),
				QObject * parent = NULL);
		virtual ~TagModel();
		static int idColumn();
		QSqlDatabase db() const;
		QList<QPair<int, QString> *> * classList() const;
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
};
#endif
