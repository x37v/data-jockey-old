#include "eqmodel.hpp"

#include <iostream>
using namespace std;


EQModel::EQModel(QObject *parent) :
	QObject(parent)
{
	mLow = mMid = mHigh = 0.0;
}

void EQModel::reset(){
	setHigh(0.0);
	setMid(0.0);
	setLow(0.0);
}

void EQModel::setHigh(float val){
	if(mHigh != val){
		mHigh = val;
		emit(highChanged(mHigh));
	}
}

void EQModel::setMid(float val){
	if(mMid != val){
		mMid = val;
		emit(midChanged(mMid));
	}
}

void EQModel::setLow(float val){
	if(mLow != val){
		mLow = val;
		emit(lowChanged(mLow));
	}
}

