#ifndef TAG_MODEL_HPP
#define TAG_MODEL_HPP

#include "treemodel.h"
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

class TagModel : public TreeModel {
	Q_OBJECT
	public:
		TagModel(const QSqlDatabase & db = QSqlDatabase(),
				QObject * parent = NULL);
		static int idColumn();
		QSqlDatabase db() const;
	private:
		QSqlDatabase mDb;
		void buildFromQuery();
		const static QString cQueryStr;
		QSqlQuery mQuery;
};
#endif
