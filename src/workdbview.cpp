#include "workdbview.hpp"
#include "worktablemodel.hpp"
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QAbstractItemModel>

WorkDBView::WorkDBView(QAbstractItemModel * model, 
		QWidget *parent) :
	QWidget(parent)
{
	//create the layouts
	QVBoxLayout * layout = new QVBoxLayout(this);
	QHBoxLayout * buttonLayout = new QHBoxLayout;

	//create and set up the tableview
	mTableView = new QTableView(this);
	mTableView->setSortingEnabled(true);
	mTableView->setModel(model);
	mTableView->setColumnHidden(0, true);
	mTableView->horizontalHeader()->setMovable(true);
	//mTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	mTableView->verticalHeader()->setVisible(false);
	mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTableView->setSelectionMode(QAbstractItemView::SingleSelection);

	//create the buttons
	mApplyFilterButton = new QPushButton("apply filter", this);
	mRemoveFilterButton = new QPushButton("remove filter", this);

	//add items to the buttonLayout
	buttonLayout->addWidget(mApplyFilterButton, 0);
	buttonLayout->addWidget(mRemoveFilterButton, 0);

	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(1);

	layout->addWidget(mTableView, 10);
	layout->addLayout(buttonLayout, 0);
	setLayout(layout);

	//connect up our internal signals
	QObject::connect(mTableView->selectionModel(), 
			SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
			this,
			SLOT(setSelection(const QItemSelection)));
	QObject::connect(mApplyFilterButton, SIGNAL(clicked(bool)),
			this, SIGNAL(applyFilterPushed()));
	QObject::connect(mRemoveFilterButton, SIGNAL(clicked(bool)),
			this, SIGNAL(removeFilterPushed()));

}

QTableView * WorkDBView::tableView(){
	return mTableView;
}

QPushButton * WorkDBView::applyFilterButton(){
	return mApplyFilterButton;
}

QPushButton * WorkDBView::removeFilterButton(){
	return mRemoveFilterButton;
}

void WorkDBView::selectWork(const QModelIndex & index ){
	QSqlRecord record = ((QSqlTableModel *)mTableView->model())->record(index.row());
	QVariant itemData = record.value(WorkTableModel::idColumn);
	//find the id of the work and emit that
	if(itemData.isValid() && itemData.canConvert(QVariant::Int)){
		int work = itemData.toInt();
		emit(workSelected(work));
	}
}

void WorkDBView::setSelection( const QItemSelection & selected){
	Q_UNUSED(selected);
	QModelIndex index = mTableView->selectionModel()->currentIndex(); 
	index = index.sibling(index.row(), WorkTableModel::idColumn);
	int work_id = -1;
	if(index.isValid())
		work_id = mTableView->model()->data(index).toInt();
	emit(workSelected(work_id));
}

void WorkDBView::setFiltered(){
	//bool selected = false;
	//int work_index = 0;
	//QModelIndexList indexes = mTableView->selectionModel()->selectedIndexes();
	//if(indexes.size() > 0){
		//QSqlRecord record = ((QSqlTableModel *)mTableView->model())->record(indexes[0].row());
		//QVariant itemData = record.value(WORK_ID_COL);
		//work_index = itemData.toInt();
		//selected = true;
	//}
	//mModel->setFiltered();
	//if(selected){
	//XXX what to do now?
	//}
}

void WorkDBView::setUnFiltered(){
	//mModel->setUnFiltered();
}
