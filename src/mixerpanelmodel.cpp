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

#include "mixerpanelmodel.hpp"
#include "mastermodel.hpp"
#include "djmixerchannelmodel.cpp"
#include "crossfademodel.hpp"
#include "eqmodel.hpp"

MixerPanelModel::MixerPanelModel(unsigned int numMixers, QObject *parent) :
	QObject(parent)
{
	mMaster = new MasterModel(numMixers, this);
	mXFade = new CrossFadeModel(numMixers, this);
	for(unsigned int i = 0; i < numMixers; i++){
		DJMixerChannelModel * djMixerModel = new DJMixerChannelModel(this);
		mDJMixerChannels.push_back(djMixerModel);
		//we set up this mapping so that we can later map object pointers to
		//indices
		mMixerObjectIndexMap[djMixerModel] = i;
		mMixerObjectIndexMap[djMixerModel->control()] = i;
		mMixerObjectIndexMap[djMixerModel->eq()] = i;

		//signals
		//mixer
		QObject::connect(djMixerModel->eq(),
				SIGNAL(valuesChanged(float, float, float, QObject *)),
				this,
				SLOT(setEqVal(float, float, float, QObject *)));
		QObject::connect(djMixerModel,
				SIGNAL(volumeChanged(float, QObject *)),
				this,
				SLOT(setMixerVolume(float, QObject *)));
		//volume takes care of muted
		QObject::connect(djMixerModel,
				SIGNAL(mutedChanged(bool, QObject *)),
				this,
				SLOT(setMixerMute(bool, QObject *)));
		//dj mixer control
		QObject::connect(djMixerModel->control(),
				SIGNAL(cueModeChanged(bool, QObject *)),
				this,
				SLOT(setMixerCueMode(bool, QObject *)));
		QObject::connect(djMixerModel->control(),
				SIGNAL(pausedChanged(bool, QObject *)),
				this,
				SLOT(setMixerPaused(bool, QObject *)));
		QObject::connect(djMixerModel->control(),
				SIGNAL(syncModeChanged(bool, QObject *)),
				this,
				SLOT(setMixerSyncMode(bool, QObject *)));
		QObject::connect(djMixerModel->control(),
				SIGNAL(tempoMulChanged(double, QObject *)),
				this,
				SLOT(setMixerTempoMul(double, QObject *)));
		QObject::connect(djMixerModel->control(),
				SIGNAL(seeking(int, QObject *)),
				this,
				SLOT(mixerSeek(int, QObject *)));
		QObject::connect(djMixerModel->control(),
				SIGNAL(playbackPositionChanged(int, QObject *)),
				this,
				SLOT(mixerSetPlaybackPos(int, QObject *)));
		QObject::connect(djMixerModel,
				SIGNAL(loadingWork(int, QObject *)),
				this,
				SLOT(mixerLoad(int, QObject *)));
	}
	//connect up the xfade internally
	QObject::connect(mXFade,
			SIGNAL(positionChanged(float)),
			this,
			SLOT(crossFadeUpdate()));
	QObject::connect(mXFade,
			SIGNAL(rightMixerChanged(unsigned int)),
			this,
			SLOT(crossFadeUpdate()));
	QObject::connect(mXFade,
			SIGNAL(leftMixerChanged(unsigned int)),
			this,
			SLOT(crossFadeUpdate()));
	QObject::connect(mXFade,
			SIGNAL(disabled()),
			this,
			SLOT(crossFadeUpdate()));
}

void MixerPanelModel::syncToModel(MixerPanelModel * other, Qt::ConnectionType connectionType){
	//connect up our master, xfade and mixers
	mMaster->syncToModel(other->master(), connectionType);
	mXFade->syncToModel(other->crossFade(), connectionType);
	//make sure not to overstep
	unsigned int last = mDJMixerChannels.size();
	if(other->mixerChannels()->size() < last)
		last = other->mixerChannels()->size();
	for(unsigned int i = 0; i < last; i++){
		mDJMixerChannels[i]->syncToModel(other->mixerChannels()->at(i), connectionType);
	}
}

//we know the type of the object because these are protected slots and
//we made the connections
void MixerPanelModel::setEqVal(float low, float mid, float high, QObject * ob){
	EQModel * eq = (EQModel *)ob;
	unsigned int index = mMixerObjectIndexMap[eq];
	emit(mixerEQValuesChanged(index, low, mid, high));
}

void MixerPanelModel::setMixerVolume(float , QObject * ob){
	MixerChannelModel * mixer = (MixerChannelModel *)ob;
	setMixerVolume(mMixerObjectIndexMap[mixer]);
}

void MixerPanelModel::setMixerMute(bool , QObject * ob){
	MixerChannelModel * mixer = (MixerChannelModel *)ob;
	setMixerVolume(mMixerObjectIndexMap[mixer]);
}

void MixerPanelModel::setMixerVolume(unsigned int index){
	float volume = mixerVolume(index);
	emit(mixerVolumeChanged(index, volume));
}

void MixerPanelModel::setMixerCueMode(bool cue, QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(cue)
		emit(mixerCueModeChanged(index,true));
	else
		emit(mixerCueModeChanged(index,false));
	//we need to change the mixer volume as it might be affected by the xfade now
	setMixerVolume(index);
}

void MixerPanelModel::setMixerPaused(bool pause, QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(pause)
		emit(mixerPausedChanged(index, true));
	else
		emit(mixerPausedChanged(index, false));
}

void MixerPanelModel::setMixerSyncMode(bool sync, QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(sync)
		emit(mixerSyncModeChanged(index, true));
	else
		emit(mixerSyncModeChanged(index, false));
}

void MixerPanelModel::setMixerTempoMul(double mul, QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	emit(mixerTempoMulChanged(index, mul));
}

void MixerPanelModel::mixerSeek(int amt, QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	emit(mixerSeeking(index, amt));
}

void MixerPanelModel::mixerSetPlaybackPos(int pos, QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	emit(mixerPlaybackPosChanged(index, pos));
}

void MixerPanelModel::mixerLoad(int work_id, QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];

	//if we're syncing to a mixer that is loading, stop syncing to it
	if(mMaster->syncSource() == (index + 1)){
		mMaster->setSyncSource(0);
	}

	emit(mixerLoading(index, work_id));
}

void MixerPanelModel::crossFadeUpdate(){
	//update the mixers volumes
	for(unsigned int i = 0; i < mDJMixerChannels.size(); i++)
		setMixerVolume(i);
}

void MixerPanelModel::mixerUpdateProgress(unsigned int mixer, float progress){
	if(mixer < mDJMixerChannels.size())
		mDJMixerChannels[mixer]->control()->setProgress(progress);
}

void MixerPanelModel::mixerSetTempoMul(unsigned int mixer, double mul){
	if(mixer < mDJMixerChannels.size())
		mDJMixerChannels[mixer]->control()->setTempoMul(mul);
}


CrossFadeModel * MixerPanelModel::crossFade() const {
	return mXFade;
}

MasterModel * MixerPanelModel::master() const {
	return mMaster;
}

std::vector<DJMixerChannelModel *> * MixerPanelModel::mixerChannels() {
	return &mDJMixerChannels;
}

DJMixerChannelModel * MixerPanelModel::mixerChannel(unsigned int i) const {
	if(i >= mDJMixerChannels.size())
		return NULL;
	else
		return mDJMixerChannels[i];
}


unsigned int MixerPanelModel::numMixerChannels() const {
	return mDJMixerChannels.size();
}

bool MixerPanelModel::mixerAudible(unsigned int index) const {
	if(index < mDJMixerChannels.size()){
		DJMixerChannelModel * mixer = mDJMixerChannels.at(index);
		if(!mixer->muted() &&
				mixer->control()->playing() &&
				!mixer->control()->cueing() &&
				mXFade->scaleVolume(index, mixer->volume()) > 0.0f &&
				mixer->control()->progress() > 0.0f &&
				mixer->control()->progress() < 1.0f
				)
			return true;
		return false;
	} else
		return false;

}

float MixerPanelModel::mixerVolume(unsigned int index) const {
	if(index >= mDJMixerChannels.size())
		return 0.0f;
	DJMixerChannelModel * djMixer = mDJMixerChannels[index];
	float volume = djMixer->volume();
	//if it is muted the volume is 0, if it isn't cueing we let the xfade scale it
	if(djMixer->muted())
		volume = 0.0f;
	else if(!djMixer->control()->cueing())
		volume = mXFade->scaleVolume(index, volume);
	return volume;
}

