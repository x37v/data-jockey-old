#include "buffer.hpp"
#include <fstream>
#include <math.h>
#include <yamlcpp/yaml.hpp>
#include <yamlcpp/parser.hpp>

using namespace DataJockey;
#define READ_FRAME_SIZE 2048
#define MIN(x,y) ((x) < (y) ? (x) : (y))

unsigned int AudioBuffer::mIdCnt = 0;
unsigned int BeatBuffer::mIdCnt = 0;

template<typename Type>
Type linear_interp(Type v0, Type v1, double dist){
	return v0 + (v1 - v0) * dist;
}

//adapted from from: http://local.wasp.uwa.edu.au/~pbourke/other/interpolation/
//mu is the distance between y1 and y2
template<typename Type>
Type cubic_interp(Type y0, Type y1, Type y2, Type y3, double mu){
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

float AudioBuffer::getSample(unsigned int chan, double time){
	double index = time * mSampleRate;
	unsigned int prevSampIndex = (unsigned int)index;
	unsigned int nextSampIndex = prevSampIndex + 1;
	if(time < 0 || mAudioBuffer.size() == 0 || nextSampIndex >= mAudioBuffer[0].size()){
		return 0;
	}
	return linear_interp<float>(mAudioBuffer[chan][prevSampIndex], 
			mAudioBuffer[chan][nextSampIndex], index - prevSampIndex);
}

float AudioBuffer::getSampleAtIndex(unsigned int chan, double index){
	unsigned int prevSampIndex = (unsigned int)index;
	unsigned int nextSampIndex = prevSampIndex + 1;
	if(index < 0 || mAudioBuffer.size() == 0 || nextSampIndex >= mAudioBuffer[0].size()){
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

/*
 * yaml format should be:
 *
 * beat locations :
 * 	time points : [sequence of doubles]
 * 	ctime: ..
 * 	...
 *
 * OR
 *
 * beat locations :
 * 	sequence [
 * 		- timepoints : [sequence of doubles]
 * 		  ctime:..
 * 		- timepoints : [sequence of doubles]
 * 		  ctime:...
 *
 */

void BeatBuffer::load(std::string beatDataLocation)
	throw(std::runtime_error)
{
	mBeatBuffer.clear();

	//read in the beat location information
	try {
		yaml::Parser y;
		yaml::node top = y.parseFile(beatDataLocation);
		yaml::map_ptr beatLocs = yaml::get<yaml::map_ptr>(top);
		yaml::map::iterator it = beatLocs->find(std::string("beat locations"));
		if(it == beatLocs->end())
			throw std::runtime_error("cannot find beat locations");

		//if it is a sequence use the last one for now!
		if(yaml::is<yaml::seq_ptr>(it->second)){
			yaml::seq_ptr beatLocSeq = yaml::get<yaml::seq_ptr>(it->second);

			//store the last
			beatLocs = yaml::get<yaml::map_ptr>( beatLocSeq->back());

			/*
			//this tries to find the one with the most recent mtime..
			beatLocs = yaml::get<yaml::map_ptr>( beatLocSeq->front());
			try {
				//do we have to check the first element?
				for(yaml::seq::iterator seq_it = beatLocSeq->begin();
						seq_it != beatLocSeq->end(); seq_it++){
					//grab the map
					yaml::map_ptr curBeats = yaml::get<yaml::map_ptr>(*seq_it);
					yaml::map::iterator mtime0 = beatLocs->find(std::string("mtime"));
					yaml::map::iterator mtime1 = curBeats->find(std::string("mtime"));;
					//make sure these are valid
					if(mtime0 != beatLocs->end() && mtime1 != curBeats->end()){
						//grab the times
						DateTimePtr t0 = yaml::get<DateTimePtr>(mtime0->second);
						DateTimePtr t1 = yaml::get<DateTimePtr>(mtime1->second);
						//if the beatlocs we're using was modified before the current beat locs, 
						//use the current beat locs
						if(*t0 < *t1)
							beatLocs = curBeats;
					}
				}
			} catch (...){
				//do nothing we've still got the first one set
			}
			*/

		} else 
			beatLocs = yaml::get<yaml::map_ptr>(it->second);

		//grab the time point vector
		it = beatLocs->find(std::string("time points"));
		if(it == beatLocs->end())
			throw std::runtime_error("cannot find time points");

		//copy over the data
		mBeatBuffer = yaml::getVector<double>(it->second);
	} catch (...){
		std::string str("error reading beat location file: ");
		str.append(beatDataLocation);
		throw std::runtime_error(str);
	}
}

double BeatBuffer::getValue(double beat_index){
	unsigned int prevBeat = (unsigned int)beat_index;
	unsigned int nextBeat = prevBeat + 1;
	//make sure our beat index is in range
	if(beat_index < 0 || nextBeat >= mBeatBuffer.size())
		return 0;
	return linear_interp<double>(mBeatBuffer[prevBeat], mBeatBuffer[nextBeat], beat_index - prevBeat);
}

double BeatBuffer::getBeatIndexAtTime(double time, double hint_beat_index){
	int cur_index = 0;
	
	//test the boundary cases
	if(mBeatBuffer.empty())
		return 0.0;
	if(time <= mBeatBuffer[0])
		return 0.0;
	if(time >= mBeatBuffer.back())
		return mBeatBuffer.size();

	//deal with the hint index, basically, where do we start searching
	if(hint_beat_index > 0)
		cur_index = hint_beat_index;
	if(hint_beat_index > mBeatBuffer.size())
		cur_index = mBeatBuffer.size() - 2;

	//there are 5 cases:
	//our time is less than the first item in the list
	//our time is greater than the last time in the list
	//our time is greater than the next item in the list
	//our time is less than the current item in the list
	//our time is between the current and the next item in the list [this is where we want to get to]

	//see for a position where mBeatBuffer[cur_index] <= time < mBeatBuffer[cur_index + 1]
	while(true){
		if (cur_index >= (int)mBeatBuffer.size())
			return mBeatBuffer.back();
		else if (cur_index < 0)
			return 0.0;
		else if(mBeatBuffer[cur_index] > time)
			cur_index--;
		else if(time >= mBeatBuffer[cur_index + 1])
			cur_index++;
		else
			break;
	}
	if (cur_index >= (int)mBeatBuffer.size())
		return mBeatBuffer.back();
	else if (cur_index < 0)
		return 0.0;
	else {
		//equation for a line!
		//times are y values, indices are x values
		//y = mx + b
		//m = (y1 - y0)  / (x1 - x0)
		//here x1 - x0 == 1
		//b = y - mx
		//x = (y - b) / m
		double t0 = mBeatBuffer[cur_index];
		double t1 = mBeatBuffer[cur_index + 1];
		double m = t1 - t0;
		double b = t0 - (cur_index * m);
		return (time - b) / m;
	}

}

double BeatBuffer::getBeatPeriod(double beat_index){
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

