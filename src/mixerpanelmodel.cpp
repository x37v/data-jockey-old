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
		mMixerObjectIndexMap[djMixerModel->DJMixerControl()] = i;
		mMixerObjectIndexMap[djMixerModel->mixerChannel()] = i;
		mMixerObjectIndexMap[djMixerModel->mixerChannel()->eq()] = i;

		//signals
		//mixer
		QObject::connect(djMixerModel->mixerChannel()->eq(),
				SIGNAL(valuesChanged(QObject *)),
				this,
				SLOT(setEqVal(QObject *)));
		QObject::connect(djMixerModel->mixerChannel(),
				SIGNAL(volumeChanged(QObject *)),
				this,
				SLOT(setMixerVolume(QObject *)));
		//volume takes care of muted
		QObject::connect(djMixerModel->mixerChannel(),
				SIGNAL(mutedChanged(QObject *)),
				this,
				SLOT(setMixerVolume(QObject *)));
		//dj mixer control
		QObject::connect(djMixerModel->DJMixerControl(),
				SIGNAL(cueModeChanged(QObject *)),
				this,
				SLOT(setMixerCueMode(QObject *)));
		QObject::connect(djMixerModel->DJMixerControl(),
				SIGNAL(pausedChanged(QObject *)),
				this,
				SLOT(setMixerPaused(QObject *)));
		QObject::connect(djMixerModel->DJMixerControl(),
				SIGNAL(syncModeChanged(QObject *)),
				this,
				SLOT(setMixerSyncMode(QObject *)));
		QObject::connect(djMixerModel->DJMixerControl(),
				SIGNAL(tempoMulChanged(QObject *)),
				this,
				SLOT(setMixerTempoMul(QObject *)));
		QObject::connect(djMixerModel->DJMixerControl(),
				SIGNAL(seeking(QObject *, int)),
				this,
				SLOT(mixerSeek(QObject *, int)));
		QObject::connect(djMixerModel->DJMixerControl(),
				SIGNAL(playbackPositionChanged(QObject *)),
				this,
				SLOT(mixerSetPlaybackPos(QObject *)));
		QObject::connect(djMixerModel,
				SIGNAL(loadingWork(QObject *, int)),
				this,
				SLOT(mixerLoad(QObject *, int)));
	}
	//connect up the xfade internally
	QObject::connect(mXFade,
			SIGNAL(positionChanged(float)),
			this,
			SLOT(crossFadeUpdate()));
	QObject::connect(mXFade,
			SIGNAL(mixersChanged(unsigned int, unsigned int)),
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
void MixerPanelModel::setEqVal(QObject * ob){
	EQModel * eq = (EQModel *)ob;
	unsigned int index = mMixerObjectIndexMap[eq];
	emit(mixerEQValuesChanged(index, eq->low(), eq->mid(), eq->high()));
}

void MixerPanelModel::setMixerVolume(QObject * ob){
	MixerChannelModel * mixer = (MixerChannelModel *)ob;
	setMixerVolume(mMixerObjectIndexMap[mixer]);
}

void MixerPanelModel::setMixerVolume(unsigned int index){
	float volume = mixerVolume(index);
	emit(mixerVolumeChanged(index, volume));
}

void MixerPanelModel::setMixerCueMode(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(mixerControl->cueing())
		emit(mixerCueModeChanged(index,true));
	else
		emit(mixerCueModeChanged(index,false));
	//we need to change the mixer volume as it might be affected by the xfade now
	setMixerVolume(index);
}

void MixerPanelModel::setMixerPaused(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(mixerControl->paused())
		emit(mixerPausedChanged(index, true));
	else
		emit(mixerPausedChanged(index, false));
}

void MixerPanelModel::setMixerSyncMode(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(mixerControl->synced())
		emit(mixerSyncModeChanged(index, true));
	else
		emit(mixerSyncModeChanged(index, false));
}

void MixerPanelModel::setMixerTempoMul(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	emit(mixerTempoMulChanged(index, mixerControl->tempoMul()));
}

void MixerPanelModel::mixerSeek(QObject * ob, int amt){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	emit(mixerSeeking(index, amt));
}

void MixerPanelModel::mixerSetPlaybackPos(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	int pos = mixerControl->beatOffset();
	emit(mixerPlaybackPosChanged(index, pos));
}

void MixerPanelModel::mixerLoad(QObject * ob, int work_id){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];

	//if we're syncing to a mixer that is loading, stop syncing to it
	if(mMaster->syncSource() == (index + 1)){
		mMaster->setSyncSource(0);
	}

	emit(mixerLoading(index, work_id));
}

#include <iostream>
using namespace std;

void MixerPanelModel::crossFadeUpdate(){
	//update the mixers volumes
	for(unsigned int i = 0; i < mDJMixerChannels.size(); i++)
		setMixerVolume(i);
}

void MixerPanelModel::mixerUpdateProgress(unsigned int mixer, float progress){
	if(mixer < mDJMixerChannels.size())
		mDJMixerChannels[mixer]->DJMixerControl()->setProgress(progress);
}

void MixerPanelModel::mixerSetTempoMul(unsigned int mixer, double mul){
	if(mixer < mDJMixerChannels.size())
		mDJMixerChannels[mixer]->DJMixerControl()->setTempoMul(mul);
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
		if(!mixer->mixerChannel()->muted() &&
				mixer->DJMixerControl()->playing() &&
				!mixer->DJMixerControl()->cueing() &&
				mXFade->scaleVolume(index, mixer->mixerChannel()->volume()) > 0.0 &&
				mixer->DJMixerControl()->progress() > 0.0 &&
				mixer->DJMixerControl()->progress() < 1.0
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
	float volume = djMixer->mixerChannel()->volume();
	//if it is muted the volume is 0, if it isn't cueing we let the xfade scale it
	if(djMixer->mixerChannel()->muted())
		volume = 0.0f;
	else if(!djMixer->DJMixerControl()->cueing())
		volume = mXFade->scaleVolume(index, volume);
	return volume;
}

