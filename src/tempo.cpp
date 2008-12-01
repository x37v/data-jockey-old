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

#include "tempo.hpp"
#include <unistd.h>

/*
#include <iostream>
using std::cerr;
using std::endl;
*/

namespace DataJockey {

	//*************
	// TempoDriver
	//************
	
	TempoDriver::TempoDriver(unsigned int sample_rate, double period){
		mOverflow = false;
		mPeriodMul = 1.0;
		reset();
		mSampleRate = sample_rate;
		setPeriod(period);
		mSyncSrc = NULL;
	}

	void TempoDriver::reset(){
		mSampleCnt = 0;
		//mLastTick = 0;
		mBeatIndex = 0;
		mOverflow = false;
	}

	//XXX this is not perfect yet, something with updating the 
	//period messes with the clock slightly..
	bool TempoDriver::tick(double &sample_val){

		mOverflow = false;

		/*
		if(mSyncSrc != NULL){
			sync();
			sample_val = mSyncSrc->getIndex();
			return mSyncSrc->overflow();
		}
		*/

		//XXX
		if(mNextTick > mSampleCnt){
			sample_val = mBeatIndex = (double)mSampleCnt / mNextTick;
			mSampleCnt++;
		} else {
			mNextTick = mPeriodMul * mPeriod * mSampleRate + (mNextTick - mSampleCnt);
			mBeatIndex = mSampleCnt = 0;
			sample_val = 1.0;
			mOverflow = true;
		}
		return mOverflow;

		if(mNextTick > mSampleCnt){
			sample_val = mBeatIndex;
			mBeatIndex += (1.0 - mBeatIndex) / (double)(mNextTick - mSampleCnt);
			/*
			if(mBeatIndex > 1.0){
				cerr << "mNextTick: " << mNextTick << endl;
				cerr << "mSampleCnt: " << mSampleCnt <<  endl;
				cerr << "mBeatIndex: " << mBeatIndex << cerr.precision(20) << endl;
				cerr << "sample_val: " << sample_val << cerr.precision(20) << endl;
				cerr << endl;
			}
			*/
			mSampleCnt++;
		} else {
			//set the next tick value
			//the number of samples to the next tick plus the error left over
			//from the previous tick
			mNextTick = mPeriodMul * mPeriod * mSampleRate + (mNextTick - mSampleCnt);
			mBeatIndex = mSampleCnt = 0;
			sample_val = 1;
			mOverflow = true;
		}
		/*
		if(sample_val > 1.0){
			mOverflow = true;
			sample_val = 1.0;
			mNextTick = mPeriodMul * mPeriod * mSampleRate + (mNextTick - mSampleCnt);
			mBeatIndex = mSampleCnt = 0;
		}
		*/
		return mOverflow;
	}

	bool TempoDriver::tick(){
		double whocares;
		return tick(whocares);
	}

//	bool TempoDriver::tick(float &sample_val){
//		if(mNextTick <= mLastTick)
//			sample_val = 0;
//		else
//			sample_val = (float)((mSampleCnt - mLastTick) / (mNextTick - mLastTick));
//		mSampleCnt++;
//		if(mSampleCnt > mNextTick){
//			mLastTick = mNextTick;
//			mNextTick = mSampleCnt + mPeriod * mSampleRate;
//			return true;
//		}
//		return false;
//	}

	void TempoDriver::setSampleRate(unsigned int rate){
		mSampleRate = rate;
	}

	double TempoDriver::getPeriod(bool include_mul){
		if (include_mul)
			return mPeriod * mPeriodMul;
		else
			return mPeriod;
	}

	void TempoDriver::setPeriod(double period, bool updateTick){
		mPeriod = period;
		if (updateTick)
			mNextTick = mPeriod * mSampleRate * mPeriodMul;
	}

	void TempoDriver::setBPM(double bpm){
		setPeriod(60.0 / bpm);
	}

	void TempoDriver::syncTo(TempoDriver * driver){
		mSyncSrc = driver;
		sync();
	}

	void TempoDriver::sync(){
		if(mSyncSrc != NULL){
			mBeatIndex = mSyncSrc->getIndex();
			mOverflow = mSyncSrc->overflow();
			mNextTick = mSyncSrc->getNextTick();
			//mLastTick = mSyncSrc->getLastTick();
			mSampleCnt = mSyncSrc->getSampleCount();
		}
	}

	void TempoDriver::syncToPeriod(double period){
		//mPeriodMul = getPeriod() / period;
		mPeriodMul = period / getPeriod();
	}

	void TempoDriver::runFree(bool set_period_mul){
		sync();
		if(mSyncSrc != NULL){
			if(set_period_mul){
				mPeriodMul = mSyncSrc->getPeriodMul() * mSyncSrc->getPeriod() / mPeriod;
			} else {
				mPeriod = mSyncSrc->getPeriod();
				//mPeriodMul = mSyncSrc->getPeriodMul();
				//mPeriod /= mPeriodMul;
				mPeriodMul = 1.0;
			}
		}
		mSyncSrc = NULL;
	}

	void TempoDriver::setIndex(double index){
		mBeatIndex = index;
	}

	void TempoDriver::setPeriodMul(double mul){
		mPeriodMul = mul;
	}

	void TempoDriver::setTempoScale(double scale){
		mPeriodMul = 1.0 / scale;
	}


	//*************
	// TempoDivider
	//************

	TempoDivider::TempoDivider(unsigned int division){
		setDivAmt(division);
		setCnt(0);
	}

	void TempoDivider::setDivAmt(unsigned int division){
		if (division > 0)
			mDivAmt = division;
		else
			mDivAmt = 1;
	}

	unsigned int TempoDivider::getDivAmt(){
		return mDivAmt;
	}

	void TempoDivider::setCnt(unsigned int cnt){
		mTickCnt = cnt;
	}
	unsigned int TempoDivider::getCnt(){
		return mTickCnt;
	}

	void TempoDivider::reset(){
		setCnt(0);
	}

	//update the tick count and output a true if we reset
	bool TempoDivider::tick(){
		bool ret = false;
		mTickCnt++;
		if(mTickCnt >= mDivAmt)
			mTickCnt = 0;
		if (mTickCnt == 0)
			ret = true;
		return ret;
	}

}

/*
//compile with:
//g++ -I/usr/local/include tempo.cpp `pkg-config --libs --cflags jack` -ljack-cpp
#include "jackaudioio.hpp"

class TestTempoDriver: public JackAudioIO {
	private:
		DataJockey::TempoDriver mDriver;
		DataJockey::TempoDivider mDivider;
	public:
		virtual int audioCallback(jack_nframes_t nframes, 
				std::vector<jack_default_audio_sample_t *> inBufs,
				std::vector<jack_default_audio_sample_t *> outBufs){
			float sample;
			for(unsigned i = 0; i < nframes; i++){
				if(mDriver.tick(sample)){
					if(mDivider.tick())
						outBufs[0][i] = outBufs[1][i] = 1;
					else {
						outBufs[0][i] = 1;
						outBufs[1][i] = 0;
					}
				} else 
					outBufs[0][i] = outBufs[1][i] = 0;
			}
		}
		TestTempoDriver() : JackAudioIO("jackcpp-test", 2,2), mDriver(44100), mDivider(4){
				mDriver.setSampleRate(getSampleRate());
		}
};


int main(){
	TestTempoDriver * t = new TestTempoDriver;
	t->start();
	t->connectToPhysical(0,0);
	t->connectToPhysical(1,1);
	sleep(300);
	return 0;
}
*/
