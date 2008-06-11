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
		QObject::connect(djMixerModel->mixerChannel()->eq(),
				SIGNAL(valuesChanged(QObject *)),
				this,
				SLOT(setEqVal(QObject *)));
		QObject::connect(djMixerModel->mixerChannel(),
				SIGNAL(volumeChanged(QObject *)),
				this,
				SLOT(setMixerVolume(QObject *)));
	}
}

//we know the type of the object because these are protected slots and
//we made the connections
void MixerPanelModel::setEqVal(QObject * ob){
	EQModel * eq = (EQModel *)ob;
	unsigned int index = mMixerObjectIndexMap[eq];
	emit(eqValuesChanged(index, eq->low(), eq->mid(), eq->high()));
}

void MixerPanelModel::setMixerVolume(QObject * ob){
	MixerChannelModel * mixer = (MixerChannelModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixer];
	emit(mixerVolumeChanged(index, mixer->volume()));
}

void MixerPanelModel::setMixerMuted(QObject * ob){
	MixerChannelModel * mixer = (MixerChannelModel *)ob;
	unsigned int index = mMixerObjectIndexMap[mixer];
	emit(mixerMutedChanged(index, mixer->muted()));
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

