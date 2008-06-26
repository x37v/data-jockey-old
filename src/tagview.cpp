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
	setColumnHidden(TagModel::idColumn(),true);

	//connect internal signals/slots
	QObject::connect(
			selectionModel(), SIGNAL(selectionChanged ( const QItemSelection &, const QItemSelection &)),
			this, SLOT(updateTagSelection()));
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

void TagView::updateTagSelection(){
	emit(tagSelectionChanged(selectedTagIds()));
}
