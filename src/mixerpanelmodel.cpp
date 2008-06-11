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
				SIGNAL(seek(QObject *, int)),
				this,
				SLOT(mixerSeekSlot(QObject *, int)));
		QObject::connect(djMixerModel->DJMixerControl(),
				SIGNAL(playbackPositionChanged(QObject *)),
				this,
				SLOT(mixerSetPlaybackPos(QObject *)));
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
	unsigned int index = mMixerObjectIndexMap[mixer];
	emit(mixerVolumeChanged(index, mixer->volume()));
}

void MixerPanelModel::setMixerMuted(QObject * ob){
	MixerChannelModel * mixer = (MixerChannelModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixer];
	//emit(mixerMutedChanged(index, mixer->muted()));
	if(mixer->muted())
		emit(mixerVolumeChanged(index, 0.0f));
	else
		emit(mixerVolumeChanged(index, mixer->volume()));

}

#include <iostream>
using namespace std;

void MixerPanelModel::setMixerCueMode(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(mixerControl->cueing())
		emit(mixerCueModeChanged(index,true));
	else
		emit(mixerCueModeChanged(index,false));
	cout << "cuemode " << index << " " << mixerControl->cueing() << endl;
}

void MixerPanelModel::setMixerPaused(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(mixerControl->paused())
		emit(mixerPausedChanged(index, true));
	else
		emit(mixerPausedChanged(index, false));
	cout << "paused" << index << mixerControl->paused() << endl;
}

void MixerPanelModel::setMixerSyncMode(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(mixerControl->synced())
		emit(mixerSyncModeChanged(index, true));
	else
		emit(mixerSyncModeChanged(index, false));
	cout << "syncModeChanged" << index << mixerControl->synced() << endl;
}
void MixerPanelModel::mixerSeekSlot(QObject * ob, int amt){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	emit(mixerSeek(index, amt));
	cout << "seek " << index << " " << amt << endl;
}

void MixerPanelModel::mixerSetPlaybackPos(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	int pos = mixerControl->beatOffset();
	emit(mixerPlaybackPosChanged(index, pos));
	cout << index << " jump to " <<  pos << endl;
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

