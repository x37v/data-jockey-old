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

void DJMixerChannelModel::syncToModel(DJMixerChannelModel * other, Qt::ConnectionType connectionType){
	QObject::connect(this,
			SIGNAL(workChanged(int)),
			other,
			SLOT(setWork(int)),
			connectionType);
	//connect our children too
	mDJMixerControl->syncToModel(other->DJMixerControl(), connectionType);
	mMixerChannel->syncToModel(other->mixerChannel(), connectionType);
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

void DJMixerChannelModel::setWork(int work){
	if(mWork != work){
		mWork = work;
		emit(workChanged(mWork));
	}
}

void DJMixerChannelModel::reset(){
	mDJMixerControl->reset();
}

