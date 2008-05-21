#include "mixerchannelview.hpp"

MixerChannelView::MixerChannelView(QWidget * parent)
	: QWidget(parent) 
{
	mEQView = new EQView(this); 
	mVolumeSlider = new QSlider(Qt::Vertical, this);
	mMuteBtn = new QPushButton("mute", this);
	mLayout = new QVBoxLayout(this);

	mVolumeSlider->setRange(0,200);
	mVolumeSlider->setTickPosition(QSlider::TicksLeft);
	mVolumeSlider->setValue(100);

	mMuteBtn->setCheckable(true);

	mLayout->setContentsMargins(0,0,0,0);

	mLayout->setAlignment(Qt::AlignHCenter);
	mLayout->addWidget(mEQView, 1);
	mLayout->addWidget(mMuteBtn, 0, Qt::AlignHCenter);
	mLayout->addWidget(mVolumeSlider, 10, Qt::AlignHCenter);
	setLayout(mLayout);
}

EQView * MixerChannelView::eq(){
	return mEQView;
}

QSlider * MixerChannelView::volumeSlider(){
	return mVolumeSlider;
}

void MixerChannelView::reset(){
	mEQView->reset();
}

