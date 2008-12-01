/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
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

unsigned int MasterModel::syncSource() const {
	return mSyncSource;
}

float MasterModel::tempo(){
	return mTempo;
}

float MasterModel::volume(){
	return mVolume;
}

void MasterModel::syncToModel(MasterModel * other, Qt::ConnectionType connectionType){
	//make us have the same number of mixers
	mNumMixers = other->mNumMixers;
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
			SLOT(setSyncSource(unsigned int)),
			connectionType);
	//other -> this
	QObject::connect(other,
			SIGNAL(volumeChanged(float)),
			this,
			SLOT(updateVolume(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(tempoChanged(float)),
			this,
			SLOT(updateTempo(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(tempoMulChanged(double)),
			this,
			SLOT(updateTempoMul(double)),
			connectionType);
	QObject::connect(other,
			SIGNAL(syncSourceChanged(unsigned int)),
			this,
			SLOT(setSyncSource(unsigned int)),
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

void MasterModel::updateVolume(float vol){
	mVolume = vol;
}

void MasterModel::updateTempo(float tempo){
	mTempo = tempo;
}

void MasterModel::updateTempoMul(double mul){
	mTempoMul = mul;
}

void MasterModel::setSyncSource(unsigned int src){
	if(src <= mNumMixers && src != mSyncSource){
		mSyncSource = src;
		emit(syncSourceChanged(mSyncSource));
	}
}

