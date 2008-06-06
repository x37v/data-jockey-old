#ifndef TAG_VIEW_HPP
#define TAG_VIEW_HPP

#include <QTreeView>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "treemodel.h"

class TreeModel;

class TagView : public QTreeView {
	Q_OBJECT
	public:
		TagView(const QSqlDatabase & db = QSqlDatabase(),
				QWidget * parent = NULL);
	public slots:
		void setWork(int work_id);
		void setShowAll();
		void clear();
	private:
		void buildFromQuery();
		TreeModel * mModel;
		static const QString cAllTagsStr;
		static const QString cSingleWorkStr;
		static const QString cOrderStr;
		QSqlQuery mQuery;
};

#endif
