#include "mixerchannelview.hpp"
#include "eqview.hpp"

MixerChannelView::MixerChannelView(QWidget * parent)
	: QWidget(parent) 
{
	mRecursing = false;
	mEQView = new EQView(this); 
	mVolumeSlider = new QSlider(Qt::Vertical, this);
	mMuteBtn = new QToolButton(this);
	mLayout = new QVBoxLayout(this);
	mVolumeSlider->setToolTip("volume");

	mVolumeSlider->setRange(0,200);
	mVolumeSlider->setTickPosition(QSlider::TicksLeft);
	mVolumeSlider->setValue(100);

	QObject::connect(
			mVolumeSlider,
			SIGNAL(valueChanged(int)),
			this, SLOT(setVolume(int)));

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

double MixerChannelView::volume() const {
	return (double)mVolumeSlider->value() / 100.0;
}

bool MixerChannelView::muted() const {
	return false;
}

void MixerChannelView::reset(){
	mEQView->reset();
}

void MixerChannelView::setVolume(double volume){
	if(mRecursing)
		return;
	mRecursing = true;

	mVolumeSlider->setValue((int)(volume * 100));
	emit(volumeChanged(volume));

	mRecursing = false;
}

void MixerChannelView::setVolume(int volume){
	if(mRecursing)
		return;
	mRecursing = true;

	mVolumeSlider->setValue(volume);
	emit(volumeChanged(((double)volume) / 100.0));

	mRecursing = false;
}

void MixerChannelView::setMuted(bool muted){
	mMuteBtn->setChecked(muted);
}

