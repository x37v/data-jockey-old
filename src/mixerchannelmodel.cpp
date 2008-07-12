#include "mixerchannelmodel.hpp"
#include "eqmodel.hpp"

MixerChannelModel::MixerChannelModel(QObject * parent) : QObject(parent) {
	mEQ = new EQModel(this);
	mVolume = 1.0;
	mMuted = false;
}

void MixerChannelModel::syncToModel(MixerChannelModel * other, Qt::ConnectionType connectionType){
	//this -> other
	QObject::connect(this,
			SIGNAL(volumeChanged(float, QObject *)),
			other,
			SLOT(setVolume(float)),
			connectionType);
	QObject::connect(this,
			SIGNAL(mutedChanged(bool, QObject *)),
			other,
			SLOT(setMuted(bool)),
			connectionType);
	//other -> this
	QObject::connect(other,
			SIGNAL(volumeChanged(float, QObject *)),
			this,
			SLOT(updateVolume(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(mutedChanged(bool, QObject *)),
			this,
			SLOT(setMuted(bool)),
			connectionType);

	//connect up eq
	mEQ->syncToModel(other->eq(), connectionType);
}

float MixerChannelModel::volume() const {
	return mVolume;
}

bool MixerChannelModel::muted() const {
	return mMuted;
}

EQModel * MixerChannelModel::eq() const {
	return mEQ;
}

void MixerChannelModel::setVolume(float volume){
	if(mVolume != volume){
		mVolume = volume;
		emit(volumeChanged(volume, this));
	}
}

void MixerChannelModel::updateVolume(float volume){
	mVolume = volume;
}

void MixerChannelModel::setMuted(bool muted){
	if(muted != mMuted){
		mMuted = muted;
		emit(mutedChanged(mMuted, this));
	}
}

