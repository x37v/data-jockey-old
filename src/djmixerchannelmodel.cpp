#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include "mixerchannelmodel.cpp"

DJMixerChannelModel::DJMixerChannelModel(QObject * parent):
	QObject(parent) 
{
	mDJMixerControl = new DJMixerControlModel(this);
	mMixerChannel = new MixerChannelModel(this);
	mWork = -1;
}

DJMixerControlModel * DJMixerChannelModel::DJMixerControl() const {
	return mDJMixerControl;
}

MixerChannelModel * DJMixerChannelModel::mixerChannel() const {
	return mMixerChannel;
}

int DJMixerChannelModel::work() const {
	return mWork;
}

void DJMixerChannelModel::loadWork(int work){
	if(mWork != work){
		mWork = work;
		emit(workChanged(mWork));
	}
}

void DJMixerChannelModel::reset(){
	mDJMixerControl->reset();
}

