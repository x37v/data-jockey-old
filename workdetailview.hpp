#ifndef WORK_DETAIL_VIEW_HPP
#define WORK_DETAIL_VIEW_HPP

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "treemodel.h"

class QLineEdit;
class QGridLayout;
class QTreeView;

class WorkDetailView : public QWidget {
	Q_OBJECT
	public:
		WorkDetailView(
				const QSqlDatabase & db = QSqlDatabase(),
				QWidget * parent = NULL
				);
	public slots:
		void setWork(int work_id);
		void clear();
	private:
		QGridLayout * mLayout;
		QLineEdit * mArtist;
		QLineEdit * mTitle;
		QTreeView * mTagView;

		//TreeModel mTagModel;
		QSqlQuery mTagQuery;
		QSqlQuery mWorkQuery;
};

#endif

