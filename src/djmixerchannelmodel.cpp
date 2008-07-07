#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include "mixerchannelmodel.cpp"

DJMixerChannelModel::DJMixerChannelModel(QObject * parent):
	MixerChannelModel(parent) 
{
	mDJMixerControl = new DJMixerControlModel(this);
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
	MixerChannelModel::syncToModel(other, connectionType);
}

DJMixerControlModel * DJMixerChannelModel::DJMixerControl() const {
	return mDJMixerControl;
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
		emit(loadingWork(this, work));
	}
}

void DJMixerChannelModel::reset(){
	mDJMixerControl->reset();
}

