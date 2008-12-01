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

#include "djmixercontrolmodel.hpp"

DJMixerControlModel::DJMixerControlModel(QObject *parent):
	QObject(parent)
{
	mBeatOffset = 0;
	mProgress = 0.0f;
	mPaused = false;
	mSynced = true;
	mCueing = false;

}

void DJMixerControlModel::syncToModel(DJMixerControlModel * other, Qt::ConnectionType connectionType){
	//this -> other (state changes)
	QObject::connect(this,
			SIGNAL(cueModeChanged(bool, QObject *)),
			other,
			SLOT(setCueing(bool)),
			connectionType);
	QObject::connect(this,
			SIGNAL(pausedChanged(bool, QObject *)),
			other,
			SLOT(setPaused(bool)),
			connectionType);
	QObject::connect(this,
			SIGNAL(syncModeChanged(bool, QObject *)),
			other,
			SLOT(setSync(bool)),
			connectionType);
	QObject::connect(this,
			SIGNAL(playbackPositionChanged(int, QObject *)),
			other,
			SLOT(setPlaybackPosition(int)),
			connectionType);
	QObject::connect(this,
			SIGNAL(beatOffsetChanged(int, QObject *)),
			other,
			SLOT(setBeatOffset(int)),
			connectionType);
	QObject::connect(this,
			SIGNAL(tempoMulChanged(double, QObject *)),
			other,
			SLOT(setTempoMul(double)),
			connectionType);
	//other -> this (state changes)
	QObject::connect(other,
			SIGNAL(cueModeChanged(bool, QObject *)),
			this,
			SLOT(setCueing(bool)),
			connectionType);
	QObject::connect(other,
			SIGNAL(pausedChanged(bool, QObject *)),
			this,
			SLOT(setPaused(bool)),
			connectionType);
	QObject::connect(other,
			SIGNAL(syncModeChanged(bool, QObject *)),
			this,
			SLOT(setSync(bool)),
			connectionType);
	QObject::connect(other,
			SIGNAL(progressChanged(float, QObject *)),
			this,
			SLOT(setProgress(float)),
			connectionType);
	//QObject::connect(other,
			//SIGNAL(playbackPositionChanged(int)),
			//this,
			//SLOT(setPlaybackPosition(int)),
			//connectionType);
	QObject::connect(other,
			SIGNAL(beatOffsetChanged(int, QObject *)),
			this,
			SLOT(setBeatOffset(int)),
			connectionType);
	QObject::connect(other,
			SIGNAL(tempoMulChanged(double, QObject *)),
			this,
			SLOT(updateTempoMul(double)),
			connectionType);
	//this -> other (non state changing)
	QObject::connect(this,
			SIGNAL(seeking(int, QObject *)),
			other,
			SLOT(seek(int)),
			connectionType);
	//XXX LOAD
}


bool DJMixerControlModel::paused() const {
	return mPaused;
}

bool DJMixerControlModel::playing() const {
	return !mPaused;
}

bool DJMixerControlModel::synced() const {
	return mSynced;
}

bool DJMixerControlModel::runningFree() const {
	return !mSynced;
}

bool DJMixerControlModel::cueing() const {
	return mCueing;
}

float DJMixerControlModel::progress() const {
	return mProgress;
}

int DJMixerControlModel::beatOffset() const {
	return mBeatOffset;
}

double DJMixerControlModel::tempoMul() const {
	return mTempoMul;
}

//slots
void DJMixerControlModel::setCueing(bool cue){
	if(cue != mCueing){
		mCueing = cue;
		emit(cueModeChanged(mCueing, this));
	}
}

void DJMixerControlModel::setPaused(bool paused){
	if(paused != mPaused){
		mPaused = paused;
		emit(pausedChanged(mPaused, this));
	}
}

void DJMixerControlModel::setPlay(bool play){
	setPaused(!play);
}

void DJMixerControlModel::setSync(bool sync){
	if(sync != mSynced){
		mSynced = sync;
		emit(syncModeChanged(sync, this));
	}
}

void DJMixerControlModel::setRunFree(bool free){
	setSync(!free);
}

void DJMixerControlModel::setProgress(float progress){
	if(mProgress != progress){
		mProgress = progress;
		emit(progressChanged(mProgress, this));
	}
}

void DJMixerControlModel::setPlaybackPosition(int pos){
	emit(playbackPositionChanged(pos, this));
}

void DJMixerControlModel::seekFwd(){
	seek(1);
}

void DJMixerControlModel::seekBkwd(){
	seek(-1);
}

void DJMixerControlModel::seek(int amt){
	emit(seeking(amt, this));
}

void DJMixerControlModel::setBeatOffset(int offset){
	if(offset != mBeatOffset){
		int offsetDiff = offset - mBeatOffset;
		mBeatOffset = offset;
		emit(seeking(offsetDiff, this));
		emit(beatOffsetChanged(mBeatOffset, this));
	}
}

void DJMixerControlModel::setTempoMul(double mul){
	if(mul != mTempoMul){
		mTempoMul = mul;
		emit(tempoMulChanged(mTempoMul, this));
	}
}

void DJMixerControlModel::updateTempoMul(double mul){
	if(mul != mTempoMul){
		mTempoMul = mul;
	}
}

void DJMixerControlModel::resetWorkPosition(){
	emit(playbackPositionChanged(mBeatOffset, this));
}

void DJMixerControlModel::loadWork(){
	emit(load(this));
}

void DJMixerControlModel::reset(){
	setBeatOffset(0);
	setTempoMul(1.0);
	//XXX there should be a setting to decide if we do this or not
	setCueing(true);
	setSync(true);
}

