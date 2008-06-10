#include "buffer.hpp"
#include <fstream>
#include <math.h>
using namespace DataJockey;
#define READ_FRAME_SIZE 2048
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#include <iostream>
using std::cout;
using std::endl;

unsigned int AudioBuffer::mIdCnt = 0;
unsigned int BeatBuffer::mIdCnt = 0;

template<typename Type>
Type linear_interp(Type v0, Type v1, float dist){
	return v0 + (v1 - v0) * dist;
}

//adapted from from: http://local.wasp.uwa.edu.au/~pbourke/other/interpolation/
//mu is the distance between y1 and y2
template<typename Type>
Type cubic_interp(Type y0, Type y1, Type y2, Type y3, float mu){
   Type a0,a1,a2,a3,mu2;
   mu2 = mu*mu;
   a0 = y3 - y2 - y0 + y1;
   a1 = y0 - y1 - a0;
   a2 = y2 - y0;
   a3 = y1;
   return(a0*mu*mu2 + a1*mu2 + a2*mu + a3);
}

AudioBuffer::AudioBuffer(std::string sndfile_location)
	throw(std::runtime_error)
{
	load(sndfile_location);
	mId = mIdCnt++;
}

void AudioBuffer::load(std::string sndfile_location)
	throw(std::runtime_error)
{
	SoundFile sndFile(sndfile_location.c_str());
	mAudioBuffer.clear();

	//check to make sure soundfile exists
	if(!sndFile){
		std::string str("cannot open soundfile: ");
		str.append(sndfile_location);
		throw std::runtime_error(str);
	}

	//read in the audio data
	float * inbuf = new float[READ_FRAME_SIZE * sndFile.channels()];
	unsigned int frames_read;
	unsigned int chans = sndFile.channels();
	mSampleRate = sndFile.samplerate();

	//we must resize the audio buffer because we want to make sure that
	//we can do mAudioBuffer[i].push_back
	mAudioBuffer.resize(chans);
//	for(unsigned int i = 0; i < chans; i++){
//		mAudioBuffer[i].resize(sndFile.frames();
//	}
	while((frames_read = sndFile.readf(inbuf, READ_FRAME_SIZE)) != 0){
		for(unsigned int i = 0; i < frames_read; i++){
			for(unsigned int j = 0; j < chans; j++){
				mAudioBuffer[j].push_back(inbuf[i * chans + j]);
			}
		}
	}
	delete [] inbuf;
}

float AudioBuffer::getSample(unsigned int chan, float time){
	float index = time * mSampleRate;
	unsigned int prevSampIndex = (unsigned int)index;
	unsigned int nextSampIndex = prevSampIndex + 1;
	if(time < 0 || mAudioBuffer.size() == 0 || nextSampIndex >= mAudioBuffer[0].size()){
		return 0;
	}
	return linear_interp<float>(mAudioBuffer[chan][prevSampIndex], 
			mAudioBuffer[chan][nextSampIndex], index - prevSampIndex);
}

//**************************
// BeatBuffer
//**************************

BeatBuffer::BeatBuffer(std::string beatDataLocation)
	throw(std::runtime_error)
{
	load(beatDataLocation);
	mId = mIdCnt++;
}

void BeatBuffer::load(std::string beatDataLocation)
	throw(std::runtime_error)
{
	std::fstream beatFile(beatDataLocation.c_str(), std::fstream::in);
	mBeatBuffer.clear();

	//check to make sure beatfile exists
	if(!beatFile){
		std::string str("cannot open beat location file: ");
		str.append(beatDataLocation);
		throw std::runtime_error(str);
	}

	//read in the beat location information
	float indata;
	beatFile >> indata;
	while(!beatFile.eof()){
		if(beatFile.fail()){
			std::string str("read failed for: ");
			str.append(beatDataLocation);
			str.append("does it have malformed data?");
			throw std::runtime_error(str);
		}
		mBeatBuffer.push_back(indata);
		beatFile >> indata;
	}
}

float BeatBuffer::getValue(float beat_index){
	unsigned int prevBeat = (unsigned int)beat_index;
	unsigned int nextBeat = prevBeat + 1;
	//make sure our beat index is in range
	if(beat_index < 0 || nextBeat >= mBeatBuffer.size())
		return 0;
	return linear_interp<float>(mBeatBuffer[prevBeat], mBeatBuffer[nextBeat], beat_index - prevBeat);
}


float BeatBuffer::getBeatPeriod(float beat_index){
	unsigned int index = (unsigned int)floor(beat_index);
	if (beat_index < 0 || mBeatBuffer.size() < 2){
		//XXX this should not happen
		return 0.5;
	}
	else if (beat_index > (mBeatBuffer.size() - 1))
		index = mBeatBuffer.size() - 2;

	return mBeatBuffer[index + 1] - mBeatBuffer[index];
	/*
	unsigned int index = (unsigned int)floor(beat_index);
	float mu = beat_index - index;

	//we subtract half a beat because the tempo is actually or the
	//time at the mid point between beats
//	beat_index -= 0.5;
	index = (unsigned int)floor(beat_index);
	mu = beat_index - index;

	//XXX if we don't have enough to do cubic interp then there is osmething worng
	if(mBeatBuffer.size() < 5){
		return 0;
	}

	//make sure we are within bounds
	//don't extrapolate now, just peg the tempo values at the beg and end to that
	//for the next or prev valid tempo calc
	if(beat_index < 2){
		index = 1;
		mu = 0;
	} else if(beat_index + 2 >= mBeatBuffer.size()){
		index = mBeatBuffer.size() - 3;
		mu = 1;
	}
	return mBeatBuffer[index] - mBeatBuffer[index - 1];
//	return cubic_interp<float>(
//			mBeatBuffer[index - 1] - mBeatBuffer[index - 2], 
//			mBeatBuffer[index] - mBeatBuffer[index - 1],
//			mBeatBuffer[index + 1] - mBeatBuffer[index],
//			mBeatBuffer[index + 2] - mBeatBuffer[index + 1],
//			mu);

*/
}

