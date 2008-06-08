#include "mixerpanelmodel.hpp"
#include "mastermodel.hpp"
#include "djmixerchannelmodel.cpp"
#include "crossfademodel.hpp"

MixerPanelModel::MixerPanelModel(unsigned int numMixers, QObject *parent) :
	QObject(parent)
{
	mMaster = new MasterModel(numMixers, this);
	mXFade = new CrossFadeModel(numMixers, this);
	for(unsigned int i = 0; i < numMixers; i++)
		mDJMixerChannels.push_back(new DJMixerChannelModel(this));
}

CrossFadeModel * MixerPanelModel::crossFade(){
	return mXFade;
}

MasterModel * MixerPanelModel::master(){
	return mMaster;
}

std::vector<DJMixerChannelModel *> * MixerPanelModel::mixerChannels(){
	return &mDJMixerChannels;
}

