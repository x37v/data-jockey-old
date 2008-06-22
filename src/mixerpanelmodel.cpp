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
		QObject::connect(djMixerModel->mixerChannel(),
				SIGNAL(mutedChanged(QObject *)),
				this,
				SLOT(setMixerMuted(QObject *)));
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
	unsigned int index = mMixerObjectIndexMap[mixer];
	if(!mixer->muted())
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

void MixerPanelModel::setMixerCueMode(QObject * ob){
	DJMixerControlModel * mixerControl = (DJMixerControlModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixerControl];
	if(mixerControl->cueing())
		emit(mixerCueModeChanged(index,true));
	else
		emit(mixerCueModeChanged(index,false));
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

unsigned int MixerPanelModel::numMixerChannels() const {
	return mDJMixerChannels.size();
}

