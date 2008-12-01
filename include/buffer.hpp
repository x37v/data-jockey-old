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
			float getSample(unsigned int chan, double time);
			//this version takes a sample index, not a time index
			float getSampleAtIndex(unsigned int chan, double index);
			unsigned int getSampleRate(){return mSampleRate;}
			unsigned int length();
	};


	class BeatBuffer : public Object {
		private:
			std::vector<double> mBeatBuffer;
			unsigned int mId;
			static unsigned int mIdCnt;
			void load(std::string beatDataLocation) throw(std::runtime_error);
		public:
			BeatBuffer(std::string beatDataLocation) throw(std::runtime_error);
			double getValue(double beat_index);
			double getBeatPeriod(double beat_index);
			//find a beat index based on a time input, the hint index lets us get
			//closer to where we need to go faster
			double getBeatIndexAtTime(double time, double hint_beat_index = 0);
			unsigned int length(){return mBeatBuffer.size();}
	};

}

#endif

