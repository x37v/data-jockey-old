#include "mixerchannelmodel.hpp"
#include "eqmodel.hpp"
#include <QSignalMapper>

MixerChannelModel::MixerChannelModel(QObject * parent) : QObject(parent) {
	mEQ = new EQModel(this);
	mVolume = 1.0;
	mMuted = false;

	//set up our internal mappings
	QSignalMapper * volMapper = new QSignalMapper(this);
	QSignalMapper * muteMapper = new QSignalMapper(this);
	volMapper->setMapping(this,this);
	muteMapper->setMapping(this,this);
	QObject::connect(this,
			SIGNAL(volumeChanged(float)),
			volMapper,
			SLOT(map()));
	QObject::connect(volMapper,
			SIGNAL(mapped(QObject *)),
			this,
			SIGNAL(volumeChanged(QObject *)));
	QObject::connect(this,
			SIGNAL(mutedChanged(bool)),
			muteMapper,
			SLOT(map()));
	QObject::connect(muteMapper,
			SIGNAL(mapped(QObject *)),
			this,
			SIGNAL(mutedChanged(QObject *)));
}

//connect this model to another model of this same type
void MixerChannelModel::connectSignalsTo(MixerChannelModel * other, Qt::ConnectionType connectionType){
	QObject::connect(this,
			SIGNAL(volumeChanged(float)),
			other,
			SLOT(setVolume(float)),
			connectionType);
	QObject::connect(this,
			SIGNAL(mutedChanged(bool)),
			other,
			SLOT(setMuted(bool)),
			connectionType);
	//connect up eq
	mEQ->connectSignalsTo(other->eq(), connectionType);
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
		emit(volumeChanged(volume));
	}
}

void MixerChannelModel::setMuted(bool muted){
	if(muted != mMuted){
		mMuted = muted;
		emit(mutedChanged(mMuted));
	}
}

