#include "djmixerchannelview.hpp"
#include "mixerchannelview.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

DJMixerChannelView::DJMixerChannelView(QWidget *parent)
	: QWidget(parent)
{
	mLayout = new QVBoxLayout(this);
	setLayout(mLayout);
	mMixerChannel = new MixerChannelView(this);
	mLayout->setContentsMargins(0,0,0,0);
	mLayout->setSpacing(1);

	//mPlayBtn = new QPushButton(">", this);
	//mCueBtn = new QPushButton("c", this);
	//mLoadBtn = new QPushButton("load", this);
	//mResetBtn = new QPushButton("r", this);
	
	//mPlayBtn->setCheckable(true);
	//mCueBtn->setCheckable(true);

	//addLayout sets parent, no need to explicitly set it here
	QHBoxLayout * seekLayout = new QHBoxLayout;
	seekLayout->setContentsMargins(0,0,0,0);
	seekLayout->setSpacing(1);
	mSeekFwdBtn = new QPushButton(">>", this);
	mSeekBkwdBtn = new QPushButton("<<", this);
	seekLayout->addWidget(mSeekBkwdBtn,1);
	seekLayout->addWidget(mSeekFwdBtn,1);

	mLayout->addLayout(seekLayout,1);
	mLayout->addWidget(mMixerChannel,0);
}
