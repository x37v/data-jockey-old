#include "mixerchannelmodel.hpp"
#include "eqmodel.hpp"

MixerChannelModel::MixerChannelModel(QObject * parent) : QObject(parent) {
	mEQ = new EQModel(this);
	mVolume = 1.0;
	mMuted = false;
}

float MixerChannelModel::volume() const {
	return mVolume;
}

bool MixerChannelModel::muted() const {
	return mMuted;
}

void MixerChannelModel::setVolume(float volume){
	if(mVolume != volume){
		mVolume = volume;
		emit(volumeChanged(volume));
	}
}

void MixerChannelModel::setMuted(bool muted){
	if(muted != mMuted){
		mMuted = muted;
		emit(mutedChanged(mMuted));
	}
}

