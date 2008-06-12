#include "djmixercontrolmodel.hpp"
#include <QSignalMapper>

DJMixerControlModel::DJMixerControlModel(QObject *parent):
	QObject(parent)
{
	mBeatOffset = 0;
	mProgress = 0.0f;
	mPaused = false;
	mSynced = true;
	mCueing = false;

	//set up our internal mappings
	QSignalMapper * cueMapper = new QSignalMapper(this);
	QSignalMapper * pauseMapper = new QSignalMapper(this);
	QSignalMapper * syncMapper = new QSignalMapper(this);
	QSignalMapper * playposMapper = new QSignalMapper(this);
	QSignalMapper * loadMapper = new QSignalMapper(this);
	cueMapper->setMapping(this,this);
	pauseMapper->setMapping(this,this);
	syncMapper->setMapping(this,this);
	playposMapper->setMapping(this,this);
	loadMapper->setMapping(this,this);
	//cue
	QObject::connect(this,
			SIGNAL(cueModeChanged(bool)),
			cueMapper,
			SLOT(map()));
	QObject::connect(cueMapper,
			SIGNAL(mapped(QObject *)),
			this,
			SIGNAL(cueModeChanged(QObject *)));
	//pause
	QObject::connect(this,
			SIGNAL(pausedChanged(bool)),
			pauseMapper,
			SLOT(map()));
	QObject::connect(pauseMapper,
			SIGNAL(mapped(QObject *)),
			this,
			SIGNAL(pausedChanged(QObject *)));
	//sync mode
	QObject::connect(this,
			SIGNAL(syncModeChanged(bool)),
			syncMapper,
			SLOT(map()));
	QObject::connect(syncMapper,
			SIGNAL(mapped(QObject *)),
			this,
			SIGNAL(syncModeChanged(QObject *)));
	//playback pos
	QObject::connect(this,
			SIGNAL(playbackPositionChanged(int)),
			playposMapper,
			SLOT(map()));
	QObject::connect(playposMapper,
			SIGNAL(mapped(QObject *)),
			this,
			SIGNAL(playbackPositionChanged(QObject *)));
	//load
	QObject::connect(this,
			SIGNAL(load()),
			loadMapper,
			SLOT(map()));
	QObject::connect(loadMapper,
			SIGNAL(mapped(QObject *)),
			this,
			SIGNAL(load(QObject *)));
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

int DJMixerControlModel::beatOffset() const {
	return mBeatOffset;
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

void DJMixerControlModel::seekFwd(){
	emit(seek(this,1));
}

void DJMixerControlModel::seekBkwd(){
	emit(seek(this,-1));
}

void DJMixerControlModel::setBeatOffset(int offset){
	if(offset != mBeatOffset){
		int offsetDiff = offset - mBeatOffset;
		mBeatOffset = offset;
		emit(seek(this, offsetDiff));
		emit(beatOffsetChanged(mBeatOffset));
	}
}

void DJMixerControlModel::setTempoMul(float mul){
	if(mul != mTempoMul){
		mTempoMul = mul;
		emit(tempoMulChanged(mTempoMul));
	}
}

void DJMixerControlModel::resetWorkPosition(){
	emit(playbackPositionChanged(mBeatOffset));
}

void DJMixerControlModel::loadWork(){
	emit(load());
}

void DJMixerControlModel::reset(){
	setBeatOffset(0);
	setTempoMul(0.0f);
	//XXX there should be a setting to decide if we do this or not
	setCueing(true);
	setSync(true);
}

