#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include "mixerchannelmodel.cpp"

DJMixerChannelModel::DJMixerChannelModel(QObject * parent):
	QObject(parent) 
{
	mDJMixerControl = new DJMixerControlModel(this);
	mMixerChannel = new MixerChannelModel(this);
}

DJMixerControlModel * DJMixerChannelModel::DJMixerControl(){
	return mDJMixerControl;
}

MixerChannelModel * DJMixerChannelModel::mixerChannel(){
	return mMixerChannel;
}

