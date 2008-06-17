#include "tagview.hpp"
#include "treemodel.h"
#include "treeitem.h"

TagView::TagView(QAbstractItemModel * model, QWidget * parent) :
	QTreeView(parent)
{
	mModel = model;
	setModel(mModel);
	setColumnHidden(1,true);
}

