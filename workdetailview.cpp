#include "workdetailview.hpp"
#include <QLineEdit>
#include <QGridLayout>
#include <QTreeView>
#include <QSqlQuery>

WorkDetailView::WorkDetailView(
		const QSqlDatabase & db,
		QWidget * parent) :
	QWidget(parent), 
	//mTagModel(parent),
	mTagQuery(db), mWorkQuery(db)
{
	mLayout = new QGridLayout(this);
	mArtist = new QLineEdit("artist", this);
	mTitle = new QLineEdit("title", this);
	mTagView = new QTreeView(this);

	mLayout->addWidget(mTitle, 0, 0);
	mLayout->addWidget(mArtist, 1, 0);
	mLayout->addWidget(mTagView, 0, 1, 1, 3);
	setLayout(mLayout);
}

void WorkDetailView::setWork(int work_id){
}

void WorkDetailView::clear(){
}

