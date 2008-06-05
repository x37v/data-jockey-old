#include "mixerchannelmodel.hpp"

MixerChannelModel::MixerChannelModel(QObject * parent) : QObject(parent) {
	mVolume = 1.0;
	mMuted = false;
}

double MixerChannelModel::volume() const {
	return mVolume;
}

bool MixerChannelModel::muted() const {
	return mMuted;
}

void MixerChannelModel::setVolume(double volume){
	mVolume = volume;
	emit(volumeChanged(volume));
}

void MixerChannelModel::setMuted(bool muted){
	mMuted = muted;
}

