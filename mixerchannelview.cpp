#include "mixerchannelview.hpp"

MixerChannelView::MixerChannelView(QWidget * parent)
	: QWidget(parent) 
{
	mEQView = new EQView(this); 
	mVolumeSlider = new QSlider(Qt::Vertical, this);
	mMuteBtn = new QToolButton(this);
	mLayout = new QVBoxLayout(this);
	mVolumeSlider->setToolTip("volume");

	mVolumeSlider->setRange(0,200);
	mVolumeSlider->setTickPosition(QSlider::TicksLeft);
	mVolumeSlider->setValue(100);

	//XXX set muteIcon
	mMuteBtn->setToolTip("mute (toggle)");
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

QToolButton * MixerChannelView::muteButton(){
	return mMuteBtn;
}

void MixerChannelView::reset(){
	mEQView->reset();
}

