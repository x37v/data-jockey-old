#include "mastermodel.hpp"

MasterModel::MasterModel(unsigned int numMixers, QObject *parent) :
	QObject(parent)
{
	mVolume = 1.0f;
	mTempo = 120.0f;
	mTempoMul = 1.0f;
	mSyncSource = 0;
	mNumMixers = numMixers;
}
void MasterModel::syncToModel(MasterModel * other, Qt::ConnectionType connectionType){
	QObject::connect(this,
			SIGNAL(volumeChanged(float)),
			other,
			SLOT(setVolume(float)),
			connectionType);
	QObject::connect(this,
			SIGNAL(tempoChanged(float)),
			other,
			SLOT(setTempo(float)),
			connectionType);
	QObject::connect(this,
			SIGNAL(tempoMulChanged(float)),
			other,
			SLOT(setTempoMul(float)),
			connectionType);
	QObject::connect(this,
			SIGNAL(syncSourceChanged(unsigned int)),
			other,
			SLOT(setTempoMul(unsigned int)),
			connectionType);
}

void MasterModel::setVolume(float vol){
	if(vol != mVolume){
		mVolume = vol;
		emit(volumeChanged(mVolume));
	}
}

void MasterModel::setTempo(float tempo){
	if(tempo != mTempo){
		mTempo = tempo;
		emit(tempoChanged(mTempo));
	}
}

void MasterModel::setTempoMul(float mul){
	if(mul != mTempoMul){
		mTempoMul = mul;
		emit(tempoMulChanged(mTempoMul));
	}
}

void MasterModel::setSyncSource(unsigned int src){
	if(src <= mNumMixers && src != mSyncSource){
		mSyncSource = src;
		emit(syncSourceChanged(mSyncSource));
	}
}

