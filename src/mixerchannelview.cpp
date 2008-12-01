/*
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

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

