#include "workfilterlistview.hpp"
#include "workfilterlist.hpp"
#include "workfiltermodel.hpp"
#include <QTextEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>

//XXX needs its own model

//how to make check boxes look like radio buttons:
//12:13 < kibab> ax: you could render a delegate to a pixmap and paint it that way in a delegate
//12:16 < kibab> ax: if you go the pixmap route, you could cache the pixmap and use a single pixmap for all the displayed ones...

WorkFilterListView::WorkFilterListView(WorkFilterList * sourceList, QWidget * parent):
	QWidget(parent)
{
	mFilterList = sourceList;
	mDetails = new QTextEdit(this);
	mSelectionTable = new QTableView(this);
	mModel = new QStandardItemModel(this);
	QVBoxLayout * layout = new QVBoxLayout(this);

	mDetails->setText(tr("details about filter"));
	mDetails->setReadOnly(true);

	mSelectionTable->setModel(mModel);
	mSelected = 0;

	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(mSelectionTable);
	layout->addWidget(mDetails);
	setLayout(layout);

	//add items to our model which are already in our source
	for(unsigned int i = 0; i < mFilterList->size(); i++){
		QStandardItem * newItem = new QStandardItem(QString(mFilterList->at(i)->name().c_str()));
		newItem->setCheckable(true);
		if(i == mFilterList->selectedIndex())
			newItem->setCheckState(Qt::Checked);
		mModel->appendRow(newItem);
	}
	
	//connect up the item changed signal
	connect(mModel, SIGNAL(itemChanged(QStandardItem *)),
             this, SLOT(listDataChanged(QStandardItem *)));
	connect(mSelectionTable->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
			this, SLOT(setSelection(const QItemSelection)));

	connect(mFilterList, SIGNAL(selectionChanged(unsigned int, unsigned int)),
			this, SLOT(updateSelection(unsigned int, unsigned int)));
	connect(this, SIGNAL(selectionChanged(unsigned int)),
			mFilterList, SLOT(selectFilter(unsigned int)));

	connect(mFilterList, SIGNAL(filterAdded(unsigned int)),
			this, SLOT(addFilter(unsigned int)));
}

void WorkFilterListView::addFilter(unsigned int list_size){
	if(list_size >= mFilterList->size()){
		QStandardItem * newItem = new QStandardItem(QString(mFilterList->at(list_size - 1)->name().c_str()));
		newItem->setCheckable(true);
		mModel->appendRow(newItem);
	}
}

void WorkFilterListView::removeFilter(unsigned int index){
}

/*
void WorkFilterListView::setSelection(unsigned int index){
	if(mSelected != index && index < mFilterList->size()){
		mSelected = index;
		emit(selectionChanged(mSelected));
	}
}
*/

void WorkFilterListView::updateSelection(unsigned int index, unsigned int old_index){
	if(mModel->item(index) && mModel->item(index)->checkState() == Qt::Unchecked){
		mModel->item(index)->setCheckState(Qt::Checked);
		//we don't need to emit selection changed here because it happens
		//through our internal connection to listDataChanged
	} 
	if(mModel->item(old_index) && mModel->item(old_index)->checkState() == Qt::Checked)
		mModel->item(old_index)->setCheckState(Qt::Unchecked);
}

void WorkFilterListView::listDataChanged(QStandardItem * item){
	unsigned int index = 0;
	//find the index of this item
	for(unsigned int i = 0; i < (unsigned int)mModel->rowCount(); i++){
		if(mModel->item(i) == item){
			index = i;
			break;
		}
	}
	if(item){
		//if it is checked then, if it isn't already selected in the model
		//select it.
		//if it is unchecked and it is selected in the model, check it
		if(item->checkState() == Qt::Checked) {
			//find the item and indicate its state
			if(mFilterList->selectedIndex() != index)
				emit(selectionChanged(index));
		} else if(mFilterList->selectedIndex() == index)
			item->setCheckState(Qt::Checked);
	}
}

//update the details depending on the position of the cursor
void WorkFilterListView::setSelection( const QItemSelection & selected){
	if(selected.indexes().size() > 0 && selected.indexes()[0].row() < (int)mFilterList->size()){
		mDetails->setText(QString(tr(mFilterList->at(
							selected.indexes()[0].row()
							)->description().c_str())));
	} else
		mDetails->setText(tr("details about filter"));
}

