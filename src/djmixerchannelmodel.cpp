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
	//this -> other (state changes)
	//XXX this could be problematic as it doens't actually load the work.. but
	//just be careful
	QObject::connect(this,
			SIGNAL(workChanged(int)),
			other,
			SLOT(setWork(int)),
			connectionType);
	//other -> this (state changes)
	QObject::connect(other,
			SIGNAL(workChanged(int)),
			this,
			SLOT(setWork(int)),
			connectionType);
	//this -> other (non [direct] state changes)
	QObject::connect(this,
			SIGNAL(loadingWork(int)),
			other,
			SLOT(loadWork(int)),
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

void DJMixerChannelModel::loadWork(int work){
	if(mWork != work){
		emit(loadingWork(work));
	}
}

void DJMixerChannelModel::reset(){
	mDJMixerControl->reset();
}

