#include "mixerchannelmodel.hpp"

MixerChannelModel::MixerChannelModel(QObject * parent) : QObject(parent) {
	mVolume = 1.0;
	mMuted = false;
}

float MixerChannelModel::volume() const {
	return mVolume;
}

bool MixerChannelModel::muted() const {
	return mMuted;
}

#include <iostream>
using namespace std;

void MixerChannelModel::setVolume(float volume){
	if(mVolume != volume){
		cout << "volume changed" << volume << endl;
		mVolume = volume;
		emit(volumeChanged(volume));
	}
}

void MixerChannelModel::setMuted(bool muted){
	if(muted != mMuted){
		cout << "mute changed " << muted << endl;
		mMuted = muted;
		emit(mutedChanged(mMuted));
	}
}

