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
		//give a list of the ids of the selected tags
		QList<int> selectedTagIds() const;
	signals:
		void tagSelectionChanged(QList<int>);
	protected slots:
		void updateTagSelection();
	private:
		QAbstractItemModel * mModel;
};

#endif
