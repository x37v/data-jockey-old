/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

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

BufferPlayer::BufferPlayer(unsigned int sampleRate, SLV2World slv2world, SLV2Plugins slv2plugins) :
	Object()
{
	//XXX maybe this should be a unsigned int/long and a double offset?
	mSampleIndex = 0;
	mSampleIncrement = 1.0;
	mLastSampleIndex = 0.0;
	mOverflow = false;

	mOutPort = cueOut;
	mLoop = false;
	mId = mIdCnt++;
	mBeatIndex = 0;
	mSubBeatIndex = 0.0;
	mVolScale = 1;
	mTempoMul = 1.0;
	mBeatOffset = 0.0;
	mMaxSample = 0.0;
	mPlayMode = syncPlayback;
	mPlaybackSampleRate = sampleRate;
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
	mSampleIncrement = mTempoMul = 1.0;
	mSampleIndex = 0.0;
	mBeatIndex = 0;
	mSubBeatIndex = 0.0;
	if(!canSync() && audio_buf != NULL)
		mPlayMode = freePlayback;
}

unsigned int BufferPlayer::getBeatIndex(){
	return mBeatIndex;
}

void BufferPlayer::setBeatIndex(unsigned int index){
	if(mBeatBuffer != NULL && index <= mBeatBuffer->length()){
		mBeatIndex = index;
	}
	if(mBeatBuffer != NULL && mAudioBuffer != NULL) 
		mSampleIndex = mBeatBuffer->getValue(mBeatIndex + mBeatOffset) * mAudioBuffer->getSampleRate();
	//if we don't have a beat buffer then we just have the index be 'seconds'
	if(!canSync() && mAudioBuffer != NULL) 
		mSampleIndex = mAudioBuffer->getSampleRate() * index;
}

void BufferPlayer::reset(){
	setBeatIndex(0);
	if(!canSync()){
		if(mAudioBuffer) {
			mSampleIndex = mBeatOffset * mAudioBuffer->getSampleRate();
		} else
			mSampleIndex = 0.0;
	}
}

void BufferPlayer::advanceBeat(int num_beats){
	//if we don't have a beat buffer then we just have the index be 'seconds'
	if(!canSync() && mAudioBuffer != NULL){
		mSampleIndex += ((int)mAudioBuffer->getSampleRate() * num_beats);

		if(mSampleIndex > mAudioBuffer->length())
			mSampleIndex = mAudioBuffer->length() + 1;
		else if(mSampleIndex < 0.0)
			mSampleIndex = mBeatOffset * mAudioBuffer->getSampleRate();
		return;
	}

	if((int)mBeatIndex + num_beats < 0)
		reset();
	else {
		mBeatIndex += num_beats;
	}
	/*
	if(looping() && mBeatIndex >= mLoopPoints.getLoopEndBeat())
		mBeatIndex = mLoopPoints.getLoopStartBeat();
		*/

	//make sure we don't advance too far past our last beat
	if(mBeatBuffer != NULL && mBeatIndex > mBeatBuffer->length()){
		mBeatIndex = mBeatBuffer->length();
	}

	if(mBeatBuffer != NULL && mAudioBuffer != NULL)
		mSampleIndex = mBeatBuffer->getValue(mBeatIndex + mBeatOffset) * mAudioBuffer->getSampleRate();
}

void BufferPlayer::sync(TempoDriver * syncSrc){
		//double prevIndex = mBeatIndex + mBeatOffset + mMyTempoDriver.getIndex();

		//make sure we have valid data and we're playing
		if(mAudioBuffer == NULL || !mPlaying)
			return;

		//increment our index if we're not paused
		mSampleIndex += mSampleIncrement;
		if(syncSrc && mPlayMode == syncPlayback && syncSrc->overflow()){
			advanceBeat();
		}

		//don't see past the end of our audio buffer
		if(mSampleIndex > mAudioBuffer->length())
			mSampleIndex = mAudioBuffer->length() + 1;

		//if we have a beat buffer and we are not syncing
		if(mBeatBuffer != NULL && (!syncSrc || mPlayMode == freePlayback)){
			unsigned int prevBeatIndex = mBeatIndex;
			//get the beat index at this point in time based on our mSampleIndex;
			double newBeatIndex = 
				mBeatBuffer->getBeatIndexAtTime(mSampleIndex / mAudioBuffer->getSampleRate(), 
						mBeatIndex + mBeatOffset);
			//store the index, ditch the offset
			mBeatIndex = newBeatIndex - mBeatOffset;
			//get the index between beats
			mSubBeatIndex = newBeatIndex - mBeatIndex - mBeatOffset;
			//if we've crossed a beat boundry, set overflow = true.. else false
			if(mBeatIndex > prevBeatIndex)
				mOverflow = true;
			else
				mOverflow = false;
		}

}

float BufferPlayer::getSample(unsigned int chan, TempoDriver * syncSrc){
	float val;
	float index;

	if (!mPlaying){
		return 0.0;
	}

	//if we don't have audio
	if(mAudioBuffer == NULL)
		return 0.0;

	//if we cannot sync, then we will always be in free playback mode
	if(!canSync())
		mPlayMode = freePlayback;

	//if we're in free mode then our tempo driver sets the period mul for us..
	//if we don't have a beat buffer than we can only play in free mode
	if ((mPlayMode == freePlayback) || !syncSrc){
		//val = mVolScale * mAudioBuffer->getSampleAtIndex(chan, mSampleIndex);
	} else {
		index = mTempoMul * (mBeatOffset + syncSrc->getIndex() + mBeatIndex);
		//update mSampleIndex, we might update this ourselves later if we switch to free mode
		mSampleIndex = mBeatBuffer->getValue(index) * mAudioBuffer->getSampleRate();
		//val = mVolScale * mAudioBuffer->getSample(chan, time);

		//update the sample increment
		if(chan == 0){
			double newIncrement = mSampleIndex - mLastSampleIndex;
			//LPF
			if(newIncrement > 0.0)
				mSampleIncrement = (9 * mSampleIncrement + newIncrement) / 10;
			mLastSampleIndex = mSampleIndex;
		}
	}
	//get the sample
	val = mVolScale * mAudioBuffer->getSampleAtIndex(chan, mSampleIndex);

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
	if(mPlayMode == syncPlayback)
		mTempoMul = mul;
	else
		mSampleIncrement = mul;
}

void BufferPlayer::setSampleIncrement(double increment){
	mSampleIncrement = increment;
}

float BufferPlayer::getTempoMultiplier(){
	if(mPlayMode == syncPlayback)
		return mTempoMul;
	else
		return mSampleIncrement;
}

void BufferPlayer::setOutPort(outputPort_t out){
	mOutPort = out;
}

//we don't set the playmode to free if we have !canSync() here because, we
//might not have loaded the audio yet, so we don't know if we can sync or not
void BufferPlayer::setPlayMode(playMode_t mode){
	mPlayMode = mode;
}

void BufferPlayer::setBeatOffset(float offset){
	mBeatOffset = offset;
}

double BufferPlayer::getAudioLength(){
	if(mAudioBuffer == NULL)
		return 0.0;
	else
		return mAudioBuffer->length();
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

float BufferPlayer::getBeatPeriod(){
	//if we cannot sync then we cannot give a beat period
	if(!canSync())
		return 0.5;
	return mBeatBuffer->getBeatPeriod(mBeatOffset + mBeatIndex) / mSampleIncrement;
}

bool BufferPlayer::getOverflow(){
	return mOverflow;
}

double BufferPlayer::getSubBeatIndex(){
	return mSubBeatIndex;
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
	mPlaying = true;
}

void BufferPlayer::pause(){
	mPlaying = false;
}

bool BufferPlayer::getPlaying(){
	return mPlaying;
}

bool BufferPlayer::canSync(){
	if(mBeatBuffer == NULL || mBeatBuffer->length() < 4)
		return false;
	else
		return true;
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
		player->setTempoMultiplier(1.0);
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
	player->advanceBeat(mIncrement);
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
		mSampleIndex = other->getSampleIndex();
		mLength = other->getAudioLength();
   } else {
		mBeatOffset = 0;
		mCurBeat = 0;
		mTempoMul = 1;
		mLastBeat = 0;
		mMaxSample = 0;
		mPlaying = false;
		mLength = 0;
		mSampleIndex = 0;
	}
}

void BufferPlayer::GetState::operateOnPlayer(BufferPlayerPtr player){

	mBeatOffset = player->getBeatOffset();
	mTempoMul = player->getTempoMultiplier();
	mMaxSample = player->getMaxSample();
	player->resetMaxSample();

	if(player->getAudioBuffer() == NULL){
		mMaxSample = 0.0;
		mSampleIndex = 0.0;
		mLength = 0.0;
		mPlaying = false;
		return;
	}
	mSampleIndex = player->getSampleIndex();
	mLength = player->getAudioLength();
	mPlaying = player->getPlaying();

	if(!player->canSync()){
		mPlayMode = freePlayback;
	} else {
		mLastBeat = player->getBeatBuffer()->length();
		if(player->getPlayMode() == syncPlayback){
			//mCurBeat = (unsigned int)(player->getBeatIndex() * mTempoMul);
			mCurBeat = (unsigned int)(player->getBeatIndex());
			mPlayMode = syncPlayback;
		} else {
			mCurBeat = (unsigned int)(player->getBeatIndex());
			mPlayMode = freePlayback;
		}
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


