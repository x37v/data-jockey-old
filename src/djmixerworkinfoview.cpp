#include "djmixerworkinfoview.hpp"

#include <QLineEdit>
#include <QVBoxLayout>

DJMixerWorkInfoView::DJMixerWorkInfoView(QWidget * parent) :
	QWidget(parent)
{
	mLayout = new QVBoxLayout(this);
	mArtistLabel = new QLineEdit(tr("artist"), this);
	mWorkLabel = new QLineEdit(tr("title"), this);
	mArtistLabel->setReadOnly(true);
	mWorkLabel->setReadOnly(true);
	mArtistLabel->setToolTip(tr("artist"));
	mWorkLabel->setToolTip(tr("title"));
	mLayout->addWidget(mArtistLabel, 0);
	mLayout->addWidget(mWorkLabel, 0);
	mLayout->setContentsMargins(0,0,0,0);
	mLayout->setSpacing(1);
	setLayout(mLayout);
}

void DJMixerWorkInfoView::setArtistText(QString text){
	mArtistLabel->setText(text);
	mArtistLabel->setCursorPosition(0);
}

void DJMixerWorkInfoView::setTitleText(QString text){
	mWorkLabel->setText(text);
	mWorkLabel->setCursorPosition(0);
}

void DJMixerWorkInfoView::reset(){
	setArtistText("artist");
	setTitleText("title");
}

