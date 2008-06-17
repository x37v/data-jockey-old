#ifndef TAG_VIEW_HPP
#define TAG_VIEW_HPP

#include <QTreeView>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "treemodel.h"

class TreeModel;
class QAbstractItemModel;

class TagView : public QTreeView {
	Q_OBJECT
	public:
		TagView(QAbstractItemModel * model,
				QWidget * parent = NULL);
	private:
		QAbstractItemModel * mModel;
};

#endif
