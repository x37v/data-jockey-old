#include "audioworkdbview.hpp"
#include "audioworktablemodel.hpp"
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

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

	//create the buttons
	mApplyFilterButton = new QPushButton("apply filter", this);
	mRemoveFilterButton = new QPushButton("remove filter", this);

	QObject::connect(mApplyFilterButton, SIGNAL(clicked(bool)),
			model, SLOT(setFiltered()));
	QObject::connect(mApplyFilterButton, SIGNAL(clicked(bool)),
			model, SLOT(query()));

	QObject::connect(mRemoveFilterButton, SIGNAL(clicked(bool)),
			model, SLOT(setUnFiltered()));
	QObject::connect(mRemoveFilterButton, SIGNAL(clicked(bool)),
			model, SLOT(query()));

	//add items to the buttonLayout
	buttonLayout->addWidget(mApplyFilterButton, 0);
	buttonLayout->addWidget(mRemoveFilterButton, 0);

	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(1);

	layout->addWidget(mTableView, 10);
	layout->addLayout(buttonLayout, 0);
	setLayout(layout);
}

