#ifndef TAG_VIEW_HPP
#define TAG_VIEW_HPP

#include <QTreeView>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "treemodel.h"
#include <QList>

class TreeModel;
class QAbstractItemModel;

class TagView : public QTreeView {
	Q_OBJECT
	public:
		TagView(QAbstractItemModel * model,
				QWidget * parent = NULL);
		QList<int> selectedTagIds() const;
	private:
		QAbstractItemModel * mModel;
};

#endif
