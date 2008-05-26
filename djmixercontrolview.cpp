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

	mBeatOffeset = new QSpinBox(this);
	mBeatOffeset->setToolTip("beat start offset");
	mBeatOffeset->setMaximum(999);
	mBeatOffeset->setMinimum(-16);

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
	mLayout->addWidget(mBeatOffeset, 1, Qt::AlignHCenter);
}
