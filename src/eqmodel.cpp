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

#include "eqmodel.hpp"

EQModel::EQModel(QObject *parent) :
	QObject(parent)
{
	mLow = mMid = mHigh = 0.0;
	mLowLast = mMidLast = mHighLast = 0.0;
	mCuttingHigh = mCuttingMid = mCuttingLow = false;
}

void EQModel::syncToModel(EQModel * other, Qt::ConnectionType connectionType){
	//this -> other
	QObject::connect(this,
			SIGNAL(highChanged(float)),
			other,
			SLOT(setHigh(float)),
			connectionType);
	QObject::connect(this,
			SIGNAL(midChanged(float)),
			other,
			SLOT(setMid(float)),
			connectionType);
	QObject::connect(this,
			SIGNAL(lowChanged(float)),
			other,
			SLOT(setLow(float)),
			connectionType);
	//other -> this
	QObject::connect(other,
			SIGNAL(highChanged(float)),
			this,
			SLOT(updateHigh(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(midChanged(float)),
			this,
			SLOT(updateMid(float)),
			connectionType);
	QObject::connect(other,
			SIGNAL(lowChanged(float)),
			this,
			SLOT(updateLow(float)),
			connectionType);
	//XXX what about cuts?
}

float EQModel::high(){
	return mHigh;
}

float EQModel::mid(){
	return mMid;
}

float EQModel::low(){
	return mLow;
}

float EQModel::value(band b){
	switch(b) {
		case LOW:
			return low();
		case MID:
			return mid();
		default:
			return high();
	}
}

void EQModel::reset(){
	setHigh(0.0);
	setMid(0.0);
	setLow(0.0);
	mCuttingHigh = mCuttingMid = mCuttingLow = false;
}

void EQModel::set(band b, float val){
	switch(b) {
		case LOW:
			setLow(val);
			break;
		case MID:
			setMid(val);
			break;
		case HIGH:
			setHigh(val);
			break;
	}
}

void EQModel::toggleCut(band b){
	switch(b) {
		case LOW:
			toggleLowCut();
			break;
		case MID:
			toggleMidCut();
			break;
		case HIGH:
			toggleHighCut();
			break;
	}
}

void EQModel::cut(band b, bool cut){
	switch(b) {
		case LOW:
			cutLow(cut);
			break;
		case MID:
			cutMid(cut);
			break;
		case HIGH:
			cutHigh(cut);
			break;
	}
}

void EQModel::cutLow(bool cut){
	if(mCuttingLow != cut){
		mCuttingLow = cut;
		emit(lowCutChanged(mCuttingLow));
	}
}

void EQModel::cutMid(bool cut){
	if(mCuttingMid != cut){
		mCuttingMid = cut;
		emit(midCutChanged(mCuttingMid));
	}
}

void EQModel::cutHigh(bool cut){
	if(mCuttingHigh != cut){
		mCuttingHigh = cut;
		emit(highCutChanged(mCuttingHigh));
	}
}

void EQModel::updateHigh(float val){
	if(val > 1.0f)
		val = 1.0f;
	else if (val < -1.0f)
		val = -1.0f;
	mHigh = val;
}

void EQModel::updateMid(float val){
	if(val > 1.0f)
		val = 1.0f;
	else if (val < -1.0f)
		val = -1.0f;
	mMid = val;
}

void EQModel::updateLow(float val){
	if(val > 1.0f)
		val = 1.0f;
	else if (val < -1.0f)
		val = -1.0f;
	mLow = val;
}

void EQModel::setHigh(float val){
	if(mCuttingHigh != false){
		mCuttingHigh = false;
		emit(highCutChanged(mCuttingHigh));
	}
	if(mHigh != val){
		mHighLast = mHigh;
		updateHigh(val);
		emit(highChanged(mHigh));
		emit(valuesChanged(mLow, mMid, mHigh, this));
	}
}

void EQModel::setMid(float val){
	if(mCuttingMid != false){
		mCuttingMid = false;
		emit(midCutChanged(mCuttingMid));
	}
	if(mMid != val){
		mMidLast = mMid;
		updateMid(val);
		emit(midChanged(mMid));
		emit(valuesChanged(mLow, mMid, mHigh, this));
	}
}

void EQModel::setLow(float val){
	if(mCuttingLow != false){
		mCuttingLow = false;
		emit(lowCutChanged(mCuttingLow));
	}
	if(mLow != val){
		mLowLast = mLow;
		updateLow(val);
		emit(lowChanged(mLow));
		emit(valuesChanged(mLow, mMid, mHigh, this));
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

