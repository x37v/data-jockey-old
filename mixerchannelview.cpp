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

	//XXX set muteIcon
	mMuteBtn->setToolTip("mute (toggle)");
	mMuteBtn->setCheckable(true);

	mLayout->setContentsMargins(0,0,0,0);

	mLayout->setAlignment(Qt::AlignHCenter);
	mLayout->addWidget(mEQView, 1);
	mLayout->addWidget(mMuteBtn, 0, Qt::AlignHCenter);
	mLayout->addWidget(mVolumeSlider, 10, Qt::AlignHCenter);
	setLayout(mLayout);

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
	double volDouble = ((double)volume) / 100.0;
	emit(volumeChanged(volDouble));

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

