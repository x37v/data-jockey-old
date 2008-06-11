#include "eqmodel.hpp"

#include <iostream>
using namespace std;


EQModel::EQModel(QObject *parent) :
	QObject(parent)
{
	mLow = mMid = mHigh = 0.0;
	mLowLast = mMidLast = mHighLast = 0.0;
	mCuttingHigh = mCuttingMid = mCuttingLow = false;
}

void EQModel::reset(){
	setHigh(0.0);
	setMid(0.0);
	setLow(0.0);
	mCuttingHigh = mCuttingMid = mCuttingLow = false;
}

void EQModel::setHigh(float val){
	if(mCuttingHigh != false){
		mCuttingHigh = false;
		emit(highCutChanged(mCuttingHigh));
	}
	if(mHigh != val){
		mHighLast = mHigh;
		mHigh = val;
		emit(highChanged(mHigh));
		emit(valuesChanged(mLow, mMid, mHigh));
	}
}

void EQModel::setMid(float val){
	if(mCuttingMid != false){
		mCuttingMid = false;
		emit(midCutChanged(mCuttingMid));
	}
	if(mMid != val){
		mMidLast = mMid;
		mMid = val;
		emit(midChanged(mMid));
		emit(valuesChanged(mLow, mMid, mHigh));
	}
}

void EQModel::setLow(float val){
	if(mCuttingLow != false){
		mCuttingLow = false;
		emit(lowCutChanged(mCuttingLow));
	}
	if(mLow != val){
		mLowLast = mLow;
		mLow = val;
		emit(lowChanged(mLow));
		emit(valuesChanged(mLow, mMid, mHigh));
	}
}

void EQModel::toggleHighCut(){
	//if we're not cutting then cut and notify listeners
	//if we are cutting, return to our previous value
	if(!mCuttingHigh){
		setHigh(-1.0f);
		mCuttingHigh = true;
		emit(highCutChanged(mCuttingHigh));
	} else {
		//this will toggle us to stop cutting as well
		setHigh(mHighLast);
	}
}

void EQModel::toggleMidCut(){
	//if we're not cutting then cut and notify listeners
	//if we are cutting, return to our previous value
	if(!mCuttingMid){
		setMid(-1.0f);
		mCuttingMid = true;
		emit(midCutChanged(mCuttingMid));
	} else {
		//this will toggle us to stop cutting as well
		setMid(mMidLast);
	}
}

void EQModel::toggleLowCut(){
	//if we're not cutting then cut and notify listeners
	//if we are cutting, return to our previous value
	if(!mCuttingLow){
		setLow(-1.0f);
		mCuttingLow = true;
		emit(lowCutChanged(mCuttingLow));
	} else {
		//this will toggle us to stop cutting as well
		setLow(mLowLast);
	}
}

