#include "djmixercontrolmodel.hpp"

DJMixerControlModel::DJMixerControlModel(QObject *parent):
	QObject(parent)
{
	mProgress = 0.0f;
	mPaused = false;
	mSynced = true;
	mCueing = false;
}

bool DJMixerControlModel::paused() const {
	return mPaused;
}

bool DJMixerControlModel::playing() const {
	return !mPaused;
}

bool DJMixerControlModel::synced() const {
	return mSynced;
}

bool DJMixerControlModel::runningFree() const {
	return !mSynced;
}

bool DJMixerControlModel::cueing() const {
	return mCueing;
}

//slots
void DJMixerControlModel::setCueing(bool cue){
	if(cue != mCueing){
		mCueing = cue;
		emit(cueModeChanged(cue));
	}
}

void DJMixerControlModel::setPaused(bool paused){
	if(paused != mPaused){
		mPaused = paused;
		emit(pausedChanged(paused));
	}
}

void DJMixerControlModel::setPlay(bool play){
	setPaused(!play);
}

void DJMixerControlModel::setSync(bool sync){
	if(sync != mSynced){
		mSynced = sync;
		emit(runningFreeChanged(sync));
	}
}

void DJMixerControlModel::setRunFree(bool free){
	setSync(!free);
}

