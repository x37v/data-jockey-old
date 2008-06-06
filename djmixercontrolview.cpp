#include "djmixercontrolview.hpp"
#include <QVBoxLayout>
#include <QToolButton>
#include <QProgressBar>
#include <QSpinBox>

DJMixerControlView::DJMixerControlView(QWidget *parent)
	: QWidget(parent)
{
	mLayout = new QVBoxLayout(this);
	setLayout(mLayout);
	mLayout->setContentsMargins(0,0,0,0);
	mLayout->setSpacing(0);

	mProgressBar = new QProgressBar(this);
	mProgressBar->setAlignment(Qt::AlignHCenter);
	mProgressBar->setValue(0);

	mLoadBtn = new QToolButton(this);
	mResetBtn = new QToolButton(this);
	mCueBtn = new QToolButton(this);
	mSyncBtn = new QToolButton(this);
	mPlayBtn = new QToolButton(this);
	mSeekFwdBtn = new QToolButton(this);
	mSeekBkwdBtn = new QToolButton(this);

	mBeatOffset = new QSpinBox(this);
	mBeatOffset->setToolTip("beat start offset");
	mBeatOffset->setMaximum(999);
	mBeatOffset->setMinimum(-16);

	mLoadBtn->setToolTip("load selected file");
	mResetBtn->setToolTip("reset playback position");
	mLoadBtn->setArrowType(Qt::UpArrow);
	mResetBtn->setArrowType(Qt::DownArrow);

	mCueBtn->setToolTip("cue (toggle)");
	mSyncBtn->setToolTip("sync/free playback (toggle)");
	mPlayBtn->setToolTip("play/pause (toggle)");
	mCueBtn->setCheckable(true);
	mSyncBtn->setCheckable(true);
	mPlayBtn->setCheckable(true);

	mSeekFwdBtn->setArrowType(Qt::RightArrow);
	mSeekBkwdBtn->setArrowType(Qt::LeftArrow);
	mSeekFwdBtn->setToolTip("seek forward");
	mSeekBkwdBtn->setToolTip("seek backward");

	QHBoxLayout * loadResetLayout = new QHBoxLayout;
	loadResetLayout->setContentsMargins(0,0,0,0);
	loadResetLayout->setSpacing(0);
	loadResetLayout->addStretch();
	loadResetLayout->addWidget(mLoadBtn);
	loadResetLayout->addWidget(mResetBtn);
	loadResetLayout->addStretch();

	QHBoxLayout * playLayout = new QHBoxLayout;
	playLayout->setContentsMargins(0,0,0,0);
	playLayout->setSpacing(0);
	playLayout->addStretch();
	playLayout->addWidget(mCueBtn);
	playLayout->addWidget(mSyncBtn);
	playLayout->addWidget(mPlayBtn);
	playLayout->addStretch();
	
	//addLayout sets parent, no need to explicitly set it here
	QHBoxLayout * seekLayout = new QHBoxLayout;
	seekLayout->setContentsMargins(0,0,0,0);
	seekLayout->setSpacing(0);
	seekLayout->addStretch();
	seekLayout->addWidget(mSeekBkwdBtn, 0, Qt::AlignHCenter);
	seekLayout->addWidget(mSeekFwdBtn, 0, Qt::AlignHCenter);
	seekLayout->addStretch();

	mLayout->addLayout(loadResetLayout,0);
	mLayout->addLayout(playLayout,0);
	mLayout->addLayout(seekLayout,0);
	mLayout->addWidget(mProgressBar,1, Qt::AlignHCenter);
	mLayout->addWidget(mBeatOffset, 1, Qt::AlignHCenter);

	//connect up our signals
	QObject::connect(
			mCueBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(cueModeChanged(bool)));
	QObject::connect(
			mPlayBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(pausedChanged(bool)));
	QObject::connect(
			mSyncBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(syncModeChanged(bool)));
	QObject::connect(
			mSeekFwdBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(seekFwdClicked(bool)));
	QObject::connect(
			mSeekBkwdBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(seekBwdClicked(bool)));
	QObject::connect(
			mLoadBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(loadClicked(bool)));
	QObject::connect(
			mResetBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(resetClicked(bool)));
	QObject::connect(
			mBeatOffset,
			SIGNAL(valueChanged(int)),
			this, SIGNAL(beatOffsetChanged(int)));
}

bool DJMixerControlView::cueMode() const {
	return mCueBtn->isChecked();
}

bool DJMixerControlView::paused() const {
	return mPlayBtn->isChecked();
}

bool DJMixerControlView::runningFree() const {
	return mSyncBtn->isChecked();
}

int DJMixerControlView::beatOffset() const {
	return mBeatOffset->value();
}

//this slots emit signals when necessary through the actual buttons

void DJMixerControlView::setCueing(bool cueing){
	if(cueing != mCueBtn->isChecked())
		mCueBtn->setChecked(cueing);
}

void DJMixerControlView::setPaused(bool paused){
	if(paused != mPlayBtn->isChecked())
		mPlayBtn->setChecked(paused);
}

void DJMixerControlView::setPlaying(bool playing){
	setPaused(!playing);
}

void DJMixerControlView::setRunningFree(bool free){
	if(free != mSyncBtn->isChecked())
		mSyncBtn->setChecked(free);
}

void DJMixerControlView::setSynced(bool synced){
	setRunningFree(!synced);
}

void DJMixerControlView::setProgress(float progress){
	mProgressBar->setValue(100 * progress);
}

void DJMixerControlView::setProgress(int progress){
	mProgressBar->setValue(progress);
}

void DJMixerControlView::setBeatOffset(int offset){
	mBeatOffset->setValue(offset);
}

