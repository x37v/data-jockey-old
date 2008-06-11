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

float DJMixerControlModel::progress() const {
	return mProgress;
}

//slots
void DJMixerControlModel::setCueing(bool cue){
	if(cue != mCueing){
		mCueing = cue;
		emit(cueModeChanged(mCueing));
	}
}

void DJMixerControlModel::setPaused(bool paused){
	if(paused != mPaused){
		mPaused = paused;
		emit(pausedChanged(mPaused));
	}
}

void DJMixerControlModel::setPlay(bool play){
	setPaused(!play);
}

void DJMixerControlModel::setSync(bool sync){
	if(sync != mSynced){
		mSynced = sync;
		bool free = !sync;
		emit(syncModeChanged(free));
	}
}

void DJMixerControlModel::setRunFree(bool free){
	setSync(!free);
}

void DJMixerControlModel::setProgress(float progress){
	if(mProgress != progress){
		mProgress = progress;
		emit(progressChanged(mProgress));
	}
}

void DJMixerControlModel::setLoadProgress(float progress){
	if(mProgress != progress){
		mProgress = progress;
		emit(progressChanged(mProgress));
	}
}

