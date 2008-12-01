/*
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

/* 
 * written by Alex Norman
 * adapted from:
 * vorbisfile_example.c (C) COPYRIGHT 1994-2007
 * by the Xiph.Org Foundation http://www.xiph.org/
 * and
 * madlld.c (c) 2001--2004 Bertrand Petit
 *
 */

#include "soundfile.hpp"

#include <stdint.h>
#ifndef INT16_MAX
#define INT16_MAX 32767
#endif

#define MAX_BUF_LEN 2048

//forward declarations
static inline signed int madScale(mad_fixed_t sample);

//open the soundfile
SoundFile::SoundFile(std::string location) : 
	mSndFile(location.c_str()), 
	mType(UNSUPPORTED),
	mSampleRate(0),
	mChannels(0)
{
	std::string oggEnd0("ogg");
	std::string oggEnd1("OGG");
	std::string mp3End0("mp3");
	std::string mp3End1("MP3");

	//determine what type of soundfile it is
	if(mSndFile){
		mType = SNDFILE;
		mSampleRate = mSndFile.samplerate();
		mChannels = mSndFile.channels();
		//see if it is an mp3
	} else {
		//we need to have enough characters to see the extension
		if(location.length() < 4){
			return;
		}
		//see if we have the correct suffix
		//ogg vorbis
		if(location.compare(location.length() - 3, 3, oggEnd0) == 0 ||
				location.compare(location.length() - 3, 3, oggEnd1) == 0) {
			//try to open the file. if file cannot be opened then this file isn't supported
			if(ov_fopen((char *)location.c_str(), &mOggFile) < 0) {
				mType = UNSUPPORTED;
				return;
			}
			mType = OGG;
			//grab the ogg info and store the sample rate plus the number of
			//channels in the file
			vorbis_info *vi=ov_info(&mOggFile,-1);
			mSampleRate = vi->rate;
			mChannels = vi->channels;
		//mp3
		} else if(location.compare(location.length() - 3, 3, mp3End0) == 0 ||
				location.compare(location.length() - 3, 3, mp3End1) == 0) {
			mType = MP3;

			//if libsoundfile couldn't open.. see if we can open it as an mp3
			mMP3Data.inputFile.open(location.c_str());

			mMP3Data.inputBuffer = new unsigned char[MAX_BUF_LEN + MAD_BUFFER_GUARD];

			mMP3Data.remaining = 0;
			mMP3Data.endOfFile = false;

			mad_stream_init(&mMP3Data.stream);
			mad_frame_init(&mMP3Data.frame);
			mad_synth_init(&mMP3Data.synth);

			//synth our first frame so that we can get the sample rate
			//mMP3Data.stream.error = MAD_ERROR_BUFLEN;
			synthMadFrame();
			mSampleRate = mMP3Data.synth.pcm.samplerate;
			mChannels = mMP3Data.synth.pcm.channels;
		}
	}
}

SoundFile::~SoundFile(){
	//clean up!
	switch(mType){
		case SNDFILE:
			break;
		case OGG:
			ov_clear(&mOggFile);
			break;
		case MP3: 
			mMP3Data.inputFile.close();
			mad_stream_finish(&mMP3Data.stream);
			mad_frame_finish(&mMP3Data.frame);
			mad_synth_finish(&mMP3Data.synth);
			delete [] mMP3Data.inputBuffer;
			break;
		default:
			break;
	};
}

unsigned int SoundFile::samplerate(){
	return mSampleRate;
}

unsigned int SoundFile::channels(){
	return mChannels;
}

unsigned int SoundFile::readFloatFrame(float *ptr, unsigned int frames){
	short pcmData[mChannels * 1024];
	int toRead = frames;
	unsigned int framesRead;
	float * curBufPtr = ptr;
	while(toRead > 0){
		if (mType == OGG){
			if(toRead > 1024)
				framesRead = oggReadShortFrame(pcmData, 1024);
			else
				framesRead = oggReadShortFrame(pcmData, toRead);
		} else {
			if(toRead > 1024)
				framesRead = mp3ReadShortFrame(pcmData, 1024);
			else
				framesRead = mp3ReadShortFrame(pcmData, toRead);
		}
		//if we didn't read anything then we're at the end of the file..
		if (framesRead == 0) {
			break;
		} else {
			//XXX deal with sample rate changes
			//convert to float
			for(unsigned int i = 0; i < framesRead * mChannels; i++)
				curBufPtr[i] = ((float)pcmData[i]) / INT16_MAX;
			//inc the current buffer pointer for the next round
			//decrement the number of frames to read next round
			curBufPtr += framesRead * mChannels;
			toRead -= framesRead;
		}
	}
	return frames - toRead;
}

unsigned int SoundFile::oggReadShortFrame(short *ptr, unsigned int frames){
	long ret;
	//we loop so that if there is a recoverable error, we still read frames
	do {
		ret = ov_read(&mOggFile, (char *)ptr, sizeof(short) * frames * mChannels, 
				0, 2, 1, &mOggIndex);
		if (ret < 0) {
			//XXX deal with errors
		}
	} while(ret < 0);
	if (ret == 0) {
		return 0;
	} else {
		//XXX deal with sample rate changes
		//ret is in bytes, convert it to frames
		return ret / (sizeof(short) * mChannels);
	}
}

unsigned int SoundFile::mp3ReadShortFrame(short *ptr, unsigned int frames){
	unsigned int framesRead = 0;
	do {
		//first read in any remaining data we have left from last time
		if(mMP3Data.remaining > 0){
			unsigned int nchannels = mMP3Data.synth.pcm.channels;
			unsigned int nsamples;
			unsigned int offset = (mMP3Data.synth.pcm.length - mMP3Data.remaining);
			mad_fixed_t const *left_ch   = mMP3Data.synth.pcm.samples[0] + offset;

			//see how much we should read.. if there are more samples remaining than
			//we need to read then make sure we only read enough..
			if(mMP3Data.remaining + framesRead > frames){
				nsamples = frames - framesRead;
				mMP3Data.remaining = mMP3Data.synth.pcm.length - nsamples;
			} else {
				nsamples = mMP3Data.remaining;
				mMP3Data.remaining = 0;
			}

			//read in our samples
			if(nchannels == 1){
				while(nsamples--){
					ptr[framesRead] = madScale(*left_ch++);
					framesRead++;
				}
			} else {
				mad_fixed_t const *right_ch  = mMP3Data.synth.pcm.samples[1] + offset;
				while(nsamples--){
					ptr[framesRead * 2] = madScale(*left_ch++);
					ptr[framesRead * 2 + 1] = madScale(*right_ch++);
					framesRead++;
				}
			}

		} else if(mMP3Data.endOfFile){
			return framesRead;
		}

		if(framesRead >= frames)
			return frames;
		//get our frame
		synthMadFrame();
	} while (true);
}

unsigned int SoundFile::readf(float *ptr, unsigned int frames){

	//if this file is not valid/supported, we don't return any data
	if(!*this)
		return 0;

	switch(mType){
		case SNDFILE:
			return mSndFile.readf(ptr, frames);
		case OGG:
			return readFloatFrame(ptr,frames);
		case MP3:
			return readFloatFrame(ptr,frames);
		default:
			return 0;
	};
}

unsigned int SoundFile::readf(short *ptr, unsigned int frames){

	//if this file is not valid/supported, we don't return any data
	if(!*this)
		return 0;

	switch(mType){
		case SNDFILE:
			return mSndFile.readf(ptr, frames);
		case OGG:
			return oggReadShortFrame(ptr,frames);
		case MP3:
			return mp3ReadShortFrame(ptr,frames);
		default:
			return 0;
	};
}

SoundFile::operator bool () const {
	switch(mType){
		case SNDFILE:
			return true;
		case MP3: 
			if(mSampleRate == 0)
				return false;
			else
				return true;
		case OGG:
			if(mSampleRate == 0)
				return false;
			else
				return true;
		default:
			return false;
	};
}

void SoundFile::synthMadFrame(){
	size_t inputRemaining = 0;

	do {

		//fill up the input buffer
		if(mMP3Data.stream.buffer == NULL || mMP3Data.stream.error == MAD_ERROR_BUFLEN){
			//copy the unprocessed data to the start of our buffer..
			if(mMP3Data.stream.next_frame != NULL){
				inputRemaining = mMP3Data.stream.bufend - mMP3Data.stream.next_frame;
				memmove(mMP3Data.inputBuffer, mMP3Data.stream.next_frame, inputRemaining);
			}

			mMP3Data.inBufLength = 0;

			//grab input data
			for(unsigned int i = inputRemaining; i < MAX_BUF_LEN; i++){
				//read in the data
				int ch;
				ch = mMP3Data.inputFile.get();
				if(EOF == ch){
					//if we're at the end zero out the rest of the input buffer
					memset(mMP3Data.inputBuffer + i, 0, sizeof(unsigned char) * (MAD_BUFFER_GUARD + MAX_BUF_LEN - i));
					mMP3Data.endOfFile = true;
					//we need to have some zeros to be able to decode the last frame
					mMP3Data.inBufLength += MAD_BUFFER_GUARD;
					break;
				}
				mMP3Data.inBufLength = i + 1;
				mMP3Data.inputBuffer[i] = (unsigned char)ch;
			}
			//decode our data
			mad_stream_buffer(&mMP3Data.stream, mMP3Data.inputBuffer, mMP3Data.inBufLength);
			mMP3Data.stream.error = (mad_error)0;
		}

		if(mad_frame_decode(&mMP3Data.frame,&mMP3Data.stream)) {
			if(MAD_RECOVERABLE(mMP3Data.stream.error)){
				if(mMP3Data.endOfFile)
					return;
				else
					continue;
			} else {
				if(mMP3Data.stream.error == MAD_ERROR_BUFLEN){
					mMP3Data.remaining = 0;
					if(mMP3Data.endOfFile)
						return;
					else
						continue;
				} else {
					//XXX THROW ERROR!!
					return;
				}
			}
		} 
		//synth the frame!
		mad_synth_frame(&mMP3Data.synth,&mMP3Data.frame);
		//we just synthed the frame, so, we have length remaining to process
		mMP3Data.remaining = mMP3Data.synth.pcm.length;
		return;
	} while(true);
}

//MAD stuff
static inline signed int madScale(mad_fixed_t sample) {
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*

#include <iostream>
using std::cout;
using std::endl;

int main(){
	short frame[1024 * 2];
	for(unsigned int i = 0; i < 2048; i++)
		frame[i] = 0;
	//SoundFile f("/mp3/adolescents/adolescents/01-i_hate_children.mp3");
	SoundFile f("/mp3/dj_deeon/gimme_head_ep/b1-like_it_raw.mp3");
	//SoundFile f("/mp3/woody_guthrie/dust_bowl_ballads/01-the_great_dust_storm_dust_storm_disaster.ogg");
	//SoundFile f("/tmp/asdf12345");
	if(f){
		cout << f.samplerate() << endl;
		SndfileHandle sndFile("/tmp/test.wav",
				SFM_WRITE, 
				SF_FORMAT_WAV | SF_FORMAT_PCM_16, 
				2, 44100);
		unsigned int samples;
		while(0 < (samples = f.readf(frame,1024))){
			sndFile.writef(frame,samples);
		}
		return 0;
	} else {
		cout << "NOPE" << endl;
		return -1;
	}
}
*/
