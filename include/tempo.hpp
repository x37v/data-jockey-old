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

#ifndef DATAJOCKEY_TEMPO_HPP
#define DATAJOCKEY_TEMPO_HPP

namespace DataJockey {

	//this class is essentially a ramp that is used as an index for buffers of data
	class TempoDriver {
		private:
			double mIncrement;
			double mBeatIndex;
			bool mOverflow;
			double mPeriod;
			unsigned int mSampleRate;
		public:
			//period is in seconds, a period of 0.5 gives 120 beats per minute
			TempoDriver(unsigned int sample_rate, double period = 0.5);
			void reset();
			//returns true if the phase was reset
			bool tick(double &sample_val);
			bool tick();
			void setSampleRate(unsigned int rate);
			void setPeriod(double period);
			double getPeriod();
			void setBPM(double bpm);

			bool overflow(){ return mOverflow; }
			void setOverflowed(bool overflow = true){ mOverflow = overflow;}

			double getIndex(){ return mBeatIndex; }
			void sync(double index, double period, bool overflow);
	};

	class TempoDivider {
		private:
			unsigned int mDivAmt;
			unsigned int mTickCnt;
		public:
			TempoDivider(unsigned int division = 4);
			unsigned int getDivAmt();
			void setDivAmt(unsigned int division);
			bool tick();
			void setCnt(unsigned int cnt);
			unsigned int getCnt();
			void reset();
	};
}

#endif

