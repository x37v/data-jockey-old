#include "mastermodel.hpp"

MasterModel::MasterModel(unsigned int numMixers, QObject *parent) :
	QObject(parent)
{
	mVolume = 1.0f;
	mTempo = 120.0f;
	mTempoMul = 1.0f;
	mSyncSource = 0;
	mNumMixers = numMixers;
	mRecursing = false;
}
void MasterModel::syncToModel(MasterModel * other, Qt::ConnectionType connectionType){
	//this -> other
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
			SIGNAL(tempoMulChanged(double)),
			other,
			SLOT(setTempoMul(double)),
			connectionType);
	QObject::connect(this,
			SIGNAL(syncSourceChanged(unsigned int)),
			other,
			SLOT(setTempoMul(unsigned int)),
			connectionType);
	//other -> this
	QObject::connect(other,
			SIGNAL(volumeChanged(float)),
			this,
			SLOT(setVolume(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(tempoChanged(float)),
			this,
			SLOT(setTempo(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(tempoMulChanged(float)),
			this,
			SLOT(setTempoMul(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(syncSourceChanged(unsigned int)),
			this,
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


#include <iostream>
using namespace std;

void MasterModel::setTempoMul(double mul){
	if(mRecursing)
		return;
	mRecursing = true;
	if(mul != mTempoMul){
		mTempoMul = mul;
		emit(tempoMulChanged(mTempoMul));
	}
	mRecursing = false;
}

void MasterModel::setSyncSource(unsigned int src){
	if(src <= mNumMixers && src != mSyncSource){
		mSyncSource = src;
		emit(syncSourceChanged(mSyncSource));
	}
}

