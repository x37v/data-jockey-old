#include "workdetailview.hpp"
#include "tagview.hpp"
#include <QLineEdit>
#include <QGridLayout>
#include <QTreeView>
#include <QSqlQuery>
#include <QSqlRecord>

#include "tagmodel.hpp"
#include "worktagmodelfilter.hpp"

const QString WorkDetailView::cWorkQuery(
		"select audio_works.id id, audio_works.name title, artists.name artist\n"
		"from audio_works\n"
		"\tinner join artist_audio_works on artist_audio_works.audio_work_id = audio_works.id\n"
		"\tinner join artists on artists.id = artist_audio_works.artist_id\n"
		"\twhere audio_works.id = ");

WorkDetailView::WorkDetailView(
		const QSqlDatabase & db,
		QWidget * parent) :
	QWidget(parent), 
	mWorkQuery(db)
{
	mLayout = new QGridLayout(this);
	mArtist = new QLineEdit(tr("artist"), this);
	mTitle = new QLineEdit(tr("title"), this);
	mTagModel = new WorkTagModelFilter(new TagModel(db, this));
	mTagView = new TagView(mTagModel, this);

	mArtist->setReadOnly(true);
	mTitle->setReadOnly(true);

	mArtist->setToolTip(tr("artist"));
	mTitle->setToolTip(tr("song title"));

	mLayout->addWidget(mTitle, 0, 0);
	mLayout->addWidget(mArtist, 1, 0);
	mLayout->addWidget(mTagView, 0, 1, 3, 1);
	mLayout->setContentsMargins(0,0,0,0);

	mLayout->setColumnStretch(0,0);
	mLayout->setColumnStretch(1,0);
	mLayout->setRowStretch(0,0);
	mLayout->setRowStretch(1,0);
	setLayout(mLayout);
}

void WorkDetailView::setWork(int work_id){
	//build up the queries
	QString workQueryStr(cWorkQuery);
	QString id;
	id.setNum(work_id);
	workQueryStr.append(id);

	//execute and fill in our data
	mWorkQuery.exec(workQueryStr);
	if(mWorkQuery.first()){
		QSqlRecord rec = mWorkQuery.record();
		int titleCol = rec.indexOf("title");
		int artistCol = rec.indexOf("artist");
		mTitle->setText(mWorkQuery.value(titleCol).toString());
		mArtist->setText(mWorkQuery.value(artistCol).toString());
		mTagModel->setWork(work_id);
		mTagView->expandAll();
	} else {
		clear();
	}
}

void WorkDetailView::clear(){
	mArtist->setText(tr("artist"));
	mTitle->setText(tr("title"));
	mTagModel->clear();
}

void WorkDetailView::expandTags(bool expand){
	if(expand)
		mTagView->expandAll();
	else
		mTagView->collapseAll();
}

