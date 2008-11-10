#ifndef DATAJOCKEY_TEMPO_HPP
#define DATAJOCKEY_TEMPO_HPP

namespace DataJockey {

	//this class is essentially a ramp that is used as an index for buffers of data
	class TempoDriver {
		private:
			bool mOverflow;
			double mBeatIndex;
			double mPeriod;
			unsigned int mSampleRate;
			double mNextTick;
			//double mLastTick;
			unsigned int mSampleCnt;
			//period mul is used to sync with a buffer player
			double mPeriodMul;
			TempoDriver * mSyncSrc;
		public:
			//period is in seconds, a period of 0.5 gives 120 beats per minute
			TempoDriver(unsigned int sample_rate, double period = 0.5);
			void reset();
			//returns true if the phase was reset
			bool tick(double &sample_val);
			bool tick();
			void setSampleRate(unsigned int rate);
			void setPeriod(double period, bool updateTick = true);
			double getPeriod(bool include_mul = true);
			void setBPM(double bpm);

			void syncTo(TempoDriver * driver);
			void sync();
			void syncToPeriod(double period);
			void runFree(bool set_period_mul = false);
			TempoDriver * getSyncSrc(){ return mSyncSrc; }

			bool overflow(){ return mOverflow; }
			void setOverflowed(bool overflow = true){ mOverflow = overflow;}

			void setIndex(double index);
			double getIndex(){ return mBeatIndex; }

			void setPeriodMul(double periodMul);
			double getPeriodMul(){return mPeriodMul;}

			void setTempoScale(double scale);
			double getTempoScale(){return 1.0 / mPeriodMul;}

			unsigned int getSampleCount(){return mSampleCnt;}
			void setSampleCount(unsigned int cnt){mSampleCnt = cnt;}

			double getNextTick(){return mNextTick;}
			void setNextTick(double next){ mNextTick = next;}

			//double getLastTick(){return mLastTick;}
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

