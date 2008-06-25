#include "tagview.hpp"
#include "tagmodel.hpp"
#include "treemodel.h"
#include "treeitem.h"
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QItemSelectionModel>

TagView::TagView(QAbstractItemModel * model, QWidget * parent) :
	QTreeView(parent)
{
	mModel = model;
	setModel(mModel);
	setColumnHidden(1,true);
}

QList<int> TagView::selectedTagIds() const {
	QItemSelectionModel * selection = selectionModel();
	QList<int> ids;
	if(selection->hasSelection()){
		const TagModel * tagModel = qobject_cast<const TagModel *>(mModel);
		if(tagModel){
			foreach(QModelIndex index, selection->selection().indexes()){
				if(tagModel->isTag(index))
					ids.push_back(index.sibling(index.row(), TagModel::idColumn()).data().toInt()); 
			}
		}
	}
	return ids;
}

