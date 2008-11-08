#include "bufferplayer.hpp"
#include <math.h>

#include <iostream>
using std::cerr;
using std::endl;

float abs_max(float x,float y){
	float abs_x = fabsf(x);
	float abs_y = fabsf(y);
	if(abs_x > abs_y)
		return abs_x;
	else
		return abs_y;
}

using namespace DataJockey;

unsigned int BufferPlayer::mIdCnt = 0;

//**************************
// BufferPlayer
//**************************

BufferPlayer::BufferPlayer(unsigned int sampleRate, TempoDriver * defaultSync, SLV2World slv2world, SLV2Plugins slv2plugins) :
	Object(), mMyTempoDriver(sampleRate)
{
	//XXX tmp
	mSampleIndex = 0;

	mOutPort = cueOut;
	mLoop = false;
	mId = mIdCnt++;
	mBeatIndex = 0;
	mVolScale = 1;
	//mTempoMul = 1;
	mBeatOffset = 0.0;
	mMaxSample = 0.0;
	mPlayMode = syncPlayback;
	mPlaybackSampleRate = sampleRate;
	//this is the default clock to sync to
	mDefaultSync = defaultSync;
	//initially we sync
	mMyTempoDriver.syncTo(defaultSync);
	mBeatBuffer = NULL;

	mPlaying = true;

	//slv2
	SLV2Value plugin_uri = slv2_value_new_uri(slv2world, DJ_EQ_URI);
	mEqPlugin = slv2_plugins_get_by_uri(slv2plugins, plugin_uri);
	if(!mEqPlugin){
		cerr << "could not load eq lv2 plugin, do you have dj eq installed?:" << endl;
		cerr << "\t\t" << DJ_EQ_URI << endl;
		exit(-1);
	}
	/* Instantiate the plugin */
	mEqInstance = slv2_plugin_instantiate(
		mEqPlugin, sampleRate, NULL);

	if (!mEqInstance){
		cerr << "could not instantiate eq lv2 plugin, do you have dj eq installed?:" << endl;
		cerr << "\t\t" << DJ_EQ_URI << endl;
		exit(-1);
	}
	mEqLeftAudioIn = 3;
	mEqRightAudioIn = 4;

	mEqLeftAudioOut = 5;
	mEqRightAudioOut = 6;
	
	mEqControlLow = mEqControlMid = mEqControlHigh = mEqControlOut = 0.0f;

	//connect up the controls
	slv2_instance_connect_port(mEqInstance, 0, &mEqControlLow);
	slv2_instance_connect_port(mEqInstance, 1, &mEqControlMid);
	slv2_instance_connect_port(mEqInstance, 2, &mEqControlHigh);
	slv2_instance_connect_port(mEqInstance, 7, &mEqControlOut);

	slv2_instance_activate(mEqInstance);
}

BufferPlayer::~BufferPlayer(){
	if(mEqInstance)
		slv2_instance_free(mEqInstance);
}

void BufferPlayer::setBuffers(AudioBufferPtr audio_buf, BeatBufferPtr beat_buf){
	mAudioBuffer = audio_buf;
	mBeatBuffer = beat_buf;
}

unsigned int BufferPlayer::getBeatIndex(){
	return mBeatIndex;
}

void BufferPlayer::setBeatIndex(unsigned int index){
	if(mBeatBuffer != NULL && index <= mBeatBuffer->length())
		mBeatIndex = index;
	if(mBeatBuffer != NULL && mAudioBuffer != NULL) 
		mSampleIndex = mBeatBuffer->getValue(mBeatIndex + mBeatOffset) * mAudioBuffer->getSampleRate();
}

void BufferPlayer::reset(){
	mSampleIndex = 0.0;
	setBeatIndex(0);
}

void BufferPlayer::advanceBeat(int num_beats){
	if((int)mBeatIndex + num_beats < 0)
		reset();
	else
		mBeatIndex += num_beats;
	if(looping() && mBeatIndex >= mLoopPoints.getLoopEndBeat())
		mBeatIndex = mLoopPoints.getLoopStartBeat();

	//make sure we don't advance too far past our last beat
	if(mBeatBuffer != NULL && mBeatIndex > mBeatBuffer->length()){
		mBeatIndex = mBeatBuffer->length();
	}

	if(mBeatBuffer != NULL && mAudioBuffer != NULL)
		mSampleIndex = mBeatBuffer->getValue(mBeatIndex + mBeatOffset) * mAudioBuffer->getSampleRate();
}

void BufferPlayer::sync(){
	if(mPlayMode == freePlayback || 
			(mMyTempoDriver.getSyncSrc() != NULL && mMyTempoDriver.getSyncSrc()->getSyncSrc() == &mMyTempoDriver)){
		double prevIndex = mBeatIndex + mBeatOffset + mMyTempoDriver.getIndex();
		double newBeatIndex;

		//make sure we have valid data
		if(mBeatBuffer == NULL || mAudioBuffer == NULL){
			return;
		}

		//increment our index if we're not paused
		if(mPlaying)
			mSampleIndex += mMyTempoDriver.getTempoScale();

		//get the beat index at this point in time based on our mSampleIndex;
		newBeatIndex = mBeatBuffer->getBeatIndexAtTime(mSampleIndex / mAudioBuffer->getSampleRate(), mBeatIndex + mBeatOffset);

		//update the tempo driver accordingly
		if ((newBeatIndex - prevIndex) > 1)
			mMyTempoDriver.setOverflowed();
		mMyTempoDriver.setIndex(newBeatIndex - floor(newBeatIndex));
		mMyTempoDriver.setPeriod(mBeatBuffer->getBeatPeriod(newBeatIndex));

		//store the index, ditch the offset
		mBeatIndex = newBeatIndex - mBeatOffset;
	} else 
		mMyTempoDriver.sync();
}

float BufferPlayer::getSample(unsigned int chan){
	float val;
	float index;
	float beatIndexOffset;
	TempoDriver * syncSrc = mMyTempoDriver.getSyncSrc();
	bool syncSrcSynchingToMe = (syncSrc != NULL && syncSrc->getSyncSrc() == &mMyTempoDriver);

	if (!mPlaying){
		return 0.0;
	}

	//update our period, syncing to ourself, tricky huh?
	if (chan == 0 && mPlayMode == syncPlayback && !syncSrcSynchingToMe){
		if(validBeatPeriod())
			mMyTempoDriver.setPeriod( getBeatPeriod(mMyTempoDriver.getIndex()));
		if(mMyTempoDriver.overflow())
			advanceBeat();
	}
	beatIndexOffset = mMyTempoDriver.getIndex();

	//if we don't have audio or beat buffers then just make no sound
	if(mAudioBuffer == NULL || mBeatBuffer == NULL)
		return 0;

	//if we're in free mode then our tempo driver sets the period mul for us..
	if (mPlayMode == freePlayback || syncSrcSynchingToMe){
		val = mVolScale * mAudioBuffer->getSampleAtIndex(chan, mSampleIndex);
	} else {
		double time;
		index = (1.0 / mMyTempoDriver.getPeriodMul()) * (mBeatOffset + beatIndexOffset + mBeatIndex);
		//update mSampleIndex, we might update this ourselves later if we switch to free mode
		time = mBeatBuffer->getValue(index);
		val = mVolScale * mAudioBuffer->getSample(chan, time);
		mSampleIndex = time * mAudioBuffer->getSampleRate();
	}

	mMaxSample = abs_max(mMaxSample, val);
	return val;
}

void BufferPlayer::setVolScale(float volume){
	mVolScale = volume;
}

void BufferPlayer::setLoopPoints(unsigned int start_beat, unsigned int end_beat){
	mLoopPoints.setLoopPoints(start_beat,end_beat);
}

void BufferPlayer::setLooping(bool loop){
	mLoop = loop;
}

void BufferPlayer::setTempoMultiplier(float mul){
	mMyTempoDriver.setPeriodMul(1.0 / mul);

}

float BufferPlayer::getTempoMultiplier(){
	return 1.0 / mMyTempoDriver.getPeriodMul();
}

void BufferPlayer::setOutPort(outputPort_t out){
	mOutPort = out;
}

void BufferPlayer::setPlayMode(playMode_t mode){
	mPlayMode = mode;
	//our sync source's sync source
	TempoDriver * syncSourceSyncSource = mDefaultSync->getSyncSrc();
	//if we're doing free playback then the tempo mul we use
	//is actually the tempo mul of our clock, so
	if (mode == freePlayback){
		//if our sync source isn't syncing to us then run free
		//otherwise we just turn off syncing but don't update
		//any state
		if(syncSourceSyncSource != &mMyTempoDriver)
			mMyTempoDriver.runFree(true);
		else
			mMyTempoDriver.syncTo(NULL);
	} else {
		mMyTempoDriver.syncTo(mDefaultSync);
		//if we're going to sync to a clock that is syncing to us then
		//we shouldn't change our perido mul
		if(syncSourceSyncSource != &mMyTempoDriver)
			mMyTempoDriver.setPeriodMul(1.0);
	}
}

void BufferPlayer::tickTempoDriver(){
	if(mMyTempoDriver.tick()){
		//advanceBeat();
	}
}

void BufferPlayer::setBeatOffset(float offset){
	mBeatOffset = offset;
}

bool BufferPlayer::validBeatPeriod(){
	if(mBeatBuffer == NULL){
		return false;
	} else
		return true;
}

float BufferPlayer::getBeatPeriod(float beatIndexOffset){
	if(mBeatBuffer == NULL){
		return 0;
	}
	return mBeatBuffer->getBeatPeriod(mBeatOffset + beatIndexOffset + mBeatIndex);
}

void BufferPlayer::applyEq(unsigned int nframes, float ** audio){
	slv2_instance_connect_port(mEqInstance, mEqLeftAudioIn, audio[0]);
	slv2_instance_connect_port(mEqInstance, mEqRightAudioIn, audio[1]);
	slv2_instance_connect_port(mEqInstance, mEqLeftAudioOut, audio[0]);
	slv2_instance_connect_port(mEqInstance, mEqRightAudioOut, audio[1]);

	/* Run plugin for this cycle */
	slv2_instance_run(mEqInstance, nframes);
}

void BufferPlayer::setEQVals(float low, float mid, float high){
	mEqControlLow = low;
	mEqControlMid = mid;
	mEqControlHigh = high;
}

void BufferPlayer::play(){
	if (!mPlaying && mPlayMode == freePlayback)
		mMyTempoDriver.reset();
	mPlaying = true;
}

void BufferPlayer::pause(){
	mPlaying = false;
}

bool BufferPlayer::getPlaying(){
	return mPlaying;
}

//******************
// LoopDescription
//****************

BufferPlayer::LoopDescription::LoopDescription(){
	mStartBeat = 0;
	mEndBeat = 4;
}

void BufferPlayer::LoopDescription::setLoopPoints(unsigned int start_beat, unsigned int end_beat){
	mStartBeat = start_beat;
	mEndBeat = end_beat;
}

//******************
// BufferPlayerCmd
//****************

BufferPlayer::Cmd::Cmd() : Command(){
}

//******************
// BufferPlayerCmdBundle
//****************

BufferPlayer::CmdBundle::CmdBundle()
	:BufferPlayer::Cmd()
{
}
void BufferPlayer::CmdBundle::addCmd(BufferPlayer::CmdPtr cmd){
	mCmds.push_back(cmd);
}
void BufferPlayer::CmdBundle::operateOnPlayer(BufferPlayerPtr player){
	for(std::vector<BufferPlayer::CmdPtr>::iterator it = mCmds.begin();
			it != mCmds.end(); it++){
		(*it)->operateOnPlayer(player);
	}
}

//******************
// BufferPlayerSetBuffers
//****************

BufferPlayer::SetBuffers::SetBuffers(
		AudioBufferPtr audio_buffer, BeatBufferPtr beat_buffer, bool reset) :
	BufferPlayer::Cmd()
{
	mReset = reset;
	mAudioBuffer = audio_buffer;
	mBeatBuffer = beat_buffer;
}

void BufferPlayer::SetBuffers::operateOnPlayer(BufferPlayerPtr player){
	mOldBeatBuffer = player->getBeatBuffer();
	mOldAudioBuffer = player->getAudioBuffer();
	player->setBuffers(mAudioBuffer, mBeatBuffer);
	if(mReset){
		player->reset();
		player->setTempoMultiplier(1);
		player->setLooping(false);
	}
}

//******************
// BufferPlayerSetVolume
//***************

BufferPlayer::SetVolume::SetVolume(float volume) : 
	BufferPlayer::Cmd()
{
	setVolume(volume);
}

void BufferPlayer::SetVolume::setVolume(float vol){
	mVolume = vol;
}

void BufferPlayer::SetVolume::operateOnPlayer(BufferPlayerPtr player){
	player->setVolScale(mVolume);
}

//******************
// BufferPlayerSetTempoMultiplier
//***************

BufferPlayer::SetTempoMultiplier::SetTempoMultiplier(double mul) :
	BufferPlayer::Cmd()
{
	mTempoMul = mul;
}

void BufferPlayer::SetTempoMultiplier::operateOnPlayer(BufferPlayerPtr player){
	player->setTempoMultiplier(mTempoMul);
}

//******************
// BufferPlayerSetLoopPoints
//***************

BufferPlayer::SetLoopPoints::SetLoopPoints(
		unsigned int start_beat, unsigned int end_beat):
	BufferPlayer::Cmd()
{
	mLoopPoints.setLoopPoints(start_beat, end_beat);
}

void BufferPlayer::SetLoopPoints::operateOnPlayer(BufferPlayerPtr player){
	player->setLoopPoints(mLoopPoints.getLoopStartBeat(),
			mLoopPoints.getLoopEndBeat());
}

//******************
// BufferPlayerSetLooping
//***************

BufferPlayer::SetLooping::SetLooping( bool looping) :
	BufferPlayer::Cmd()
{
	mLooping = looping;
}

void BufferPlayer::SetLooping::operateOnPlayer(BufferPlayerPtr player){
	player->setLooping(mLooping);
}

//******************
// BufferPlayerSetPlaybackPosition
//***************

BufferPlayer::SetPlaybackPosition::SetPlaybackPosition( unsigned int beat_index) :
	BufferPlayer::Cmd()
{
	mNextBeatIndex = beat_index;
}

void BufferPlayer::SetPlaybackPosition::operateOnPlayer(BufferPlayerPtr player){
	player->setBeatIndex(mNextBeatIndex);
}

//******************
// BufferPlayerIncPlaybackPosition
//***************

BufferPlayer::IncPlaybackPosition::IncPlaybackPosition( int increment) :
	BufferPlayer::Cmd()
{
	mIncrement = increment;
}

void BufferPlayer::IncPlaybackPosition::operateOnPlayer(BufferPlayerPtr player){
	if((mIncrement + (int)player->getBeatIndex()) < 0)
		player->setBeatIndex(0);
	else
		player->setBeatIndex(mIncrement + player->getBeatIndex());
}

//******************
// BufferPlayerSetOutPort
//***************

BufferPlayer::SetOutPort::SetOutPort(BufferPlayer::outputPort_t out) :
	BufferPlayer::Cmd()
{
	mOutPort = out;
}

void BufferPlayer::SetOutPort::operateOnPlayer(BufferPlayerPtr player){
	player->setOutPort(mOutPort);
}

//******************
// BufferPlayerSetBeatOffset
//***************

BufferPlayer::SetBeatOffset::SetBeatOffset(float offset):
	BufferPlayer::Cmd()
{
	mBeatOffset = offset;
}

void BufferPlayer::SetBeatOffset::operateOnPlayer(BufferPlayerPtr player){
	player->setBeatOffset(mBeatOffset);
}


//******************
// BufferPlayer::GetState
//***************

BufferPlayer::GetState::GetState():
	BufferPlayer::Cmd()
{
		mBeatOffset = 0;
		mCurBeat = 0;
		mTempoMul = 1;
		mLastBeat = 0;
		mMaxSample = 0;
		mPlaying = false;
}

BufferPlayer::GetState::GetState(BufferPlayer::GetStatePtr other){
   if(other != NULL){
      mBeatOffset = other->getBeatOffset();
      mCurBeat = other->getCurBeat();
      mTempoMul = other->getTempoMultiplier();
      mLastBeat = other->getLastBeat();
		mMaxSample = other->getMaxSample();
		mPlaying = other->getPlaying();
   } else {
		mBeatOffset = 0;
		mCurBeat = 0;
		mTempoMul = 1;
		mLastBeat = 0;
		mMaxSample = 0;
		mPlaying = false;
	}
}

void BufferPlayer::GetState::operateOnPlayer(BufferPlayerPtr player){
	if (player->getBeatBuffer() != NULL && player->getAudioBuffer() != NULL){
		mBeatOffset = player->getBeatOffset();
		mTempoMul = player->getTempoMultiplier();
		if(player->getPlayMode() == syncPlayback){
			//mCurBeat = (unsigned int)(player->getBeatIndex() * mTempoMul);
			mCurBeat = (unsigned int)(player->getBeatIndex());
			mPlayMode = syncPlayback;
		} else {
			mCurBeat = (unsigned int)(player->getBeatIndex());
			mPlayMode = freePlayback;
		}
		mLastBeat = player->getBeatBuffer()->length();
		mMaxSample = player->getMaxSample();
		player->resetMaxSample();
		mPlaying = player->getPlaying();
	}
}

//******************
// BufferPlayerSetEQVals
//***************

BufferPlayer::SetEQVals::SetEQVals(float low, float mid, float high):
	BufferPlayer::Cmd()
{
	mLow = low;
	mMid = mid;
	mHigh = high;
	if(mLow < -70.0f)
		mLow = -70.0f;
	else if (mLow > 6.0f)
		mLow = 6.0f;

	if(mMid < -70.0f)
		mMid = -70.0f;
	else if (mMid > 6.0f)
		mMid = 6.0f;

	if(mHigh < -70.0f)
		mHigh = -70.0f;
	else if (mHigh > 6.0f)
		mHigh = 6.0f;
}

void BufferPlayer::SetEQVals::operateOnPlayer(BufferPlayerPtr player){
	player->setEQVals(mLow, mMid, mHigh);
}

BufferPlayer::SetPlayMode::SetPlayMode(playMode_t mode) :
	BufferPlayer::Cmd() 
{
	mMode = mode;
}

void BufferPlayer::SetPlayMode::operateOnPlayer(BufferPlayerPtr player){
	player->setPlayMode(mMode);
}

BufferPlayer::SetPlaying::SetPlaying(bool play) :
	BufferPlayer::Cmd() 
{
	mPlaying = play;
}

void BufferPlayer::SetPlaying::operateOnPlayer(BufferPlayerPtr player){
	if (mPlaying)
		player->play();
	else
		player->pause();
}


