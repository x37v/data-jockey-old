#include "audioworkdbview.hpp"
#include "audioworktablemodel.hpp"
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSqlRecord>
#include <QSqlTableModel>

#define WORK_ID_COL 0

AudioWorkDBView::AudioWorkDBView(AudioWorkTableModel * model, 
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
	mTableView->verticalHeader()->setVisible(false);
	mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTableView->setSelectionMode(QAbstractItemView::SingleSelection);

	//create the buttons
	mApplyFilterButton = new QPushButton("apply filter", this);
	mRemoveFilterButton = new QPushButton("remove filter", this);

	QObject::connect(mApplyFilterButton, SIGNAL(clicked(bool)),
			model, SLOT(setFiltered()));
	QObject::connect(mRemoveFilterButton, SIGNAL(clicked(bool)),
			model, SLOT(setUnFiltered()));

	//add items to the buttonLayout
	buttonLayout->addWidget(mApplyFilterButton, 0);
	buttonLayout->addWidget(mRemoveFilterButton, 0);

	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(1);

	layout->addWidget(mTableView, 10);
	layout->addLayout(buttonLayout, 0);
	setLayout(layout);

	//QObject::connect(mTableView, SIGNAL(clicked(const QModelIndex)),
			//this, SLOT(selectWork(const QModelIndex)));

	QObject::connect(mTableView->selectionModel(), 
			SIGNAL(selectionChanged(const QItemSelection, const QItemSelection)),
			this,
			SLOT(selectionChanged(const QItemSelection)));
}

QTableView * AudioWorkDBView::tableView(){
	return mTableView;
}

QPushButton * AudioWorkDBView::applyFilterButton(){
	return mApplyFilterButton;
}

QPushButton * AudioWorkDBView::removeFilterButton(){
	return mRemoveFilterButton;
}

void AudioWorkDBView::selectWork(const QModelIndex & index ){
	QSqlRecord record = ((QSqlTableModel *)mTableView->model())->record(index.row());
	QVariant itemData = record.value(WORK_ID_COL);
	//find the id of the work and emit that
	if(itemData.isValid() && itemData.canConvert(QVariant::Int)){
		int work = itemData.toInt();
		emit(workSelected(work));
	}
}

void AudioWorkDBView::selectionChanged( const QItemSelection & selected){
	//if valid, otherwise send a -1 for work selected
	if(selected.indexes().size() > 0)
		selectWork(selected.indexes()[0]);
	else 
		emit(workSelected(-1));
}

