#ifndef DATAJOCKEY_BUFFER_HPP
#define DATAJOCKEY_BUFFER_HPP

#include <vector>
#include <stdexcept>
#include "soundfile.hpp"
#include <boost/intrusive_ptr.hpp>
#include "object.hpp"
#include "command.hpp"

#include <iostream>

namespace DataJockey {

	class AudioBuffer;
	class BeatBuffer;
	typedef boost::intrusive_ptr<AudioBuffer> AudioBufferPtr;
	typedef boost::intrusive_ptr<BeatBuffer> BeatBufferPtr;

	class AudioBuffer : public Object {
		private:
			std::vector<std::vector<float> > mAudioBuffer;
			unsigned int mId;
			static unsigned int mIdCnt;
			unsigned int mSampleRate;
			void load(std::string sndfile_location) throw(std::runtime_error);
		public:
			AudioBuffer(std::string sndfile_location) throw(std::runtime_error);
//			~AudioBuffer(){
//				std::cout << "destroying audio buffer" << std::endl;
//			}
			float getSample(unsigned int chan, float index);
			unsigned int getSampleRate(){return mSampleRate;}
	};


	class BeatBuffer : public Object {
		private:
			std::vector<double> mBeatBuffer;
			unsigned int mId;
			static unsigned int mIdCnt;
			void load(std::string beatDataLocation) throw(std::runtime_error);
		public:
			BeatBuffer(std::string beatDataLocation) throw(std::runtime_error);
			float getValue(float beat_index);
			float getBeatPeriod(float beat_index);
			unsigned int length(){return mBeatBuffer.size();}
	};

}

#endif

