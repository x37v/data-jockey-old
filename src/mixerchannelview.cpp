#include "mixerchannelview.hpp"
#include "eqview.hpp"

MixerChannelView::MixerChannelView(QWidget * parent)
	: QObject(parent) 
{
	mRecursing = false;
	mEQView = new EQView(parent); 
	mVolumeSlider = new QSlider(Qt::Vertical, parent);
	mMuteBtn = new MuteButton(parent);

	mVolumeSlider->setToolTip("volume");

	mVolumeSlider->setRange(0,200);
	mVolumeSlider->setTickPosition(QSlider::TicksLeft);
	mVolumeSlider->setValue(100);


	//connect our widget's signals to our slots (or signals)
	QObject::connect(
			mVolumeSlider,
			SIGNAL(valueChanged(int)),
			this, SLOT(setVolume(int)));
	QObject::connect(
			mMuteBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(mutedChanged(bool)));
}

EQView * MixerChannelView::eq(){
	return mEQView;
}

QSlider * MixerChannelView::volumeSlider(){
	return mVolumeSlider;
}

MuteButton * MixerChannelView::muteButton(){
	return mMuteBtn;
}

float MixerChannelView::volume() const {
	return (float)mVolumeSlider->value() / 100.0;
}

bool MixerChannelView::muted() const {
	return false;
}

void MixerChannelView::setVolume(float volume){
	if(mRecursing)
		return;
	mRecursing = true;

	int volInt = volume * 100;
	if(volInt != mVolumeSlider->value()){
		mVolumeSlider->setValue(volInt);
		emit(volumeChanged(volume));
	}

	mRecursing = false;
}

void MixerChannelView::setVolume(int volume){
	if(mRecursing)
		return;
	mRecursing = true;

	if(volume != mVolumeSlider->value())
		mVolumeSlider->setValue(volume);
	//always emit because the signal could have
	//come internally and we need to update slots
	//that are connected to us
	float volFloat = ((float)volume) / 100.0;
	emit(volumeChanged(volFloat));

	mRecursing = false;
}


void MixerChannelView::setMuted(bool muted){
	if(mRecursing){
		return;
	}
	mRecursing = true;

	if(muted != mMuteBtn->isChecked()){
		mMuteBtn->setChecked(muted);
		emit(mutedChanged(muted));
	}

	mRecursing = false;
}

