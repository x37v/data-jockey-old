#include "djmixercontrolview.hpp"
#include <QVBoxLayout>
#include <QToolButton>
#include <QProgressBar>
#include <QSpinBox>
#include <QDoubleSpinBox>

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
	mBeatOffset->setToolTip(tr("beat start offset"));
	mBeatOffset->setRange(-16,99);

	mTempoMul = new QDoubleSpinBox(this);
	mTempoMul->setToolTip(tr("tempo multiplier"));
	mTempoMul->setRange(0.01,4);
	mTempoMul->setValue(1.0);

	mLoadBtn->setToolTip(tr("load selected file"));
	mResetBtn->setToolTip(tr("reset playback position"));
	mLoadBtn->setArrowType(Qt::UpArrow);
	mResetBtn->setArrowType(Qt::DownArrow);

	mCueBtn->setToolTip(tr("cue (toggle)"));
	mSyncBtn->setToolTip(tr("sync/free playback (toggle)"));
	mPlayBtn->setToolTip(tr("play/pause (toggle)"));
	mCueBtn->setCheckable(true);
	mSyncBtn->setCheckable(true);
	mPlayBtn->setCheckable(true);

	//we initially start in sync
	mSyncBtn->setChecked(true);

	mSeekFwdBtn->setArrowType(Qt::RightArrow);
	mSeekBkwdBtn->setArrowType(Qt::LeftArrow);
	mSeekFwdBtn->setToolTip(tr("seek forward"));
	mSeekBkwdBtn->setToolTip(tr("seek backward"));

	QHBoxLayout * loadResetLayout = new QHBoxLayout;
	loadResetLayout->setContentsMargins(0,0,0,0);
	loadResetLayout->setSpacing(0);
	loadResetLayout->addWidget(mLoadBtn, 0, Qt::AlignHCenter);
	loadResetLayout->addWidget(mResetBtn, 0, Qt::AlignHCenter);

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

	QHBoxLayout * tempoAndBeatLayout = new QHBoxLayout;
	tempoAndBeatLayout->setContentsMargins(0,0,0,0);
	tempoAndBeatLayout->setSpacing(0);
	tempoAndBeatLayout->addStretch();
	tempoAndBeatLayout->addWidget(mBeatOffset, 0, Qt::AlignHCenter);
	tempoAndBeatLayout->addWidget(mTempoMul, 0, Qt::AlignHCenter);
	tempoAndBeatLayout->addStretch();

	mLayout->addLayout(loadResetLayout,0);
	mLayout->addLayout(playLayout,0);
	mLayout->addLayout(seekLayout,0);
	mLayout->addWidget(mProgressBar,1, Qt::AlignHCenter);
	//mLayout->addWidget(mBeatOffset, 1, Qt::AlignHCenter);
	mLayout->addLayout(tempoAndBeatLayout, 0);

	//connect up our signals
	QObject::connect(
			mCueBtn,
			SIGNAL(toggled(bool)),
			this, SIGNAL(cueModeChanged(bool)));
	QObject::connect(
			mPlayBtn,
			SIGNAL(toggled(bool)),
			this, SIGNAL(pausedChanged(bool)));
	QObject::connect(
			mSyncBtn,
			SIGNAL(toggled(bool)),
			this, SIGNAL(syncModeChanged(bool)));
	QObject::connect(
			mSeekFwdBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(seekingFwd()));
	QObject::connect(
			mSeekBkwdBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(seekingBwd()));
	QObject::connect(
			mLoadBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(loadClicked()));
	QObject::connect(
			mResetBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(resetClicked()));
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

void DJMixerControlView::setRunFree(bool free){
	setSync(!free);
}

void DJMixerControlView::setSync(bool synced){
	if(synced != mSyncBtn->isChecked())
		mSyncBtn->setChecked(synced);
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

