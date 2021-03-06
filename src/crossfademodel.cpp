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

#include "crossfademodel.hpp"
#include "math.h"

CrossFadeModel::CrossFadeModel(unsigned int numMixers, QObject * parent) :
	QObject(parent)
{
	mNumMixers = numMixers;
	mLeft = 0;
	mRight = 0;
	mEnabled = false;
	mPosition = 0.0;
	mRecursing = false;
}


CrossFadeModel::CrossFadeModel(QObject * parent) :
	QObject(parent)
{
	mNumMixers = 0;
	mLeft = 0;
	mRight = 0;
	mEnabled = false;
	mPosition = 0.0;
	mRecursing = false;
}

float CrossFadeModel::position() const {
	return mPosition;
}

float CrossFadeModel::valueLeft() const {
	if(mPosition >= 1.0f)
		return 0.0f;
	else if(mPosition <= 0.0f)
		return 1.0f;
	//equal power!
	return (float)sin((M_PI / 2) * (1.0f + mPosition));
}

float CrossFadeModel::valueRight() const {
	if(mPosition <= 0.0f)
		return 0.0f;
	else if(mPosition >= 1.0f)
		return 1.0f;
	//equal power!
	return (float)sin((M_PI / 2) * mPosition);
}

bool CrossFadeModel::enabled() const {
	return mEnabled;
}

unsigned int CrossFadeModel::mixerLeft() const {
	return mLeft;
}

unsigned int CrossFadeModel::mixerRight() const {
	return mRight;
}

float CrossFadeModel::scaleVolume(unsigned int index, float mixer_volume){
	if(!mEnabled)
		return mixer_volume;
	else if(index == mLeft)
		return mixer_volume * valueLeft();
	else if(index == mRight)
		return mixer_volume * valueRight();
	else
		return mixer_volume;
}

//sync this model's state to another model
//signals which don't change the model's state only go from this model to the other, not back
void CrossFadeModel::syncToModel(CrossFadeModel * other, Qt::ConnectionType connectionType){
	//make our number of mixers the same
	mNumMixers = other->mNumMixers;
	//this -> other
	QObject::connect(this,
			SIGNAL(enabled(bool)),
			other,
			SLOT(enable(bool)),
			connectionType);
	//QObject::connect(this,
			//SIGNAL(leftMixerChanged(unsigned int)),
			//other,
			//SLOT(setLeftMixer(unsigned int)),
			//connectionType);
	//QObject::connect(this,
			//SIGNAL(rightMixerChanged(unsigned int)),
			//other,
			//SLOT(setRightMixer(unsigned int)),
			//connectionType);
	QObject::connect(this,
			SIGNAL(mixersChanged(unsigned int, unsigned int)),
			other,
			SLOT(setMixers(unsigned int, unsigned int)),
			connectionType);
	QObject::connect(this,
			SIGNAL(positionChanged(float)),
			other,
			SLOT(setPosition(float)),
			connectionType);
	//other -> this
	QObject::connect(other,
			SIGNAL(enabled(bool)),
         this,
			SLOT(enable(bool)),
			connectionType);
	//QObject::connect(other,
			//SIGNAL(leftMixerChanged(unsigned int)),
         //this,
			//SLOT(setLeftMixer(unsigned int)),
			//connectionType);
	//QObject::connect(other,
			//SIGNAL(rightMixerChanged(unsigned int)),
         //this,
			//SLOT(setRightMixer(unsigned int)),
			//connectionType);
	QObject::connect(other,
			SIGNAL(mixersChanged(unsigned int, unsigned int)),
			this,
			SLOT(setMixers(unsigned int, unsigned int)),
			connectionType);
	QObject::connect(other,
			SIGNAL(positionChanged(float)),
			this,
			SLOT(updatePosition(float)),
			connectionType);
}

void CrossFadeModel::setLeftMixer(unsigned int index){
   if(index >= mNumMixers)
      return;
   if(index != mLeft){
      mLeft = index;
      emit(leftMixerChanged(mLeft));
   }
	if(!mEnabled){
		mEnabled = true;
		emit(enabled(mEnabled));
	}
}

void CrossFadeModel::setRightMixer(unsigned int index){
   if(index >= mNumMixers)
      return;
   if(index != mRight){
      mRight = index;
      emit(rightMixerChanged(mRight));
   }
	if(!mEnabled){
		mEnabled = true;
		emit(enabled(mEnabled));
	}
}

void CrossFadeModel::setMixers(unsigned int left, unsigned int right){
	//make sure we're in range
	if(left >= mNumMixers || right >= mNumMixers){
		return;
	}
	if(left == right)
		return;
	if((left != mLeft) || (right != mRight) || !mEnabled){
		setLeftMixer(left);
		setRightMixer(right);
		emit(mixersChanged(mLeft,mRight));
	}
}

void CrossFadeModel::disable(){
	if(mEnabled){
		mEnabled = false;
		emit(disabled());
		emit(enabled(mEnabled));
	}
}

void CrossFadeModel::enable(bool value){
   if(mEnabled != value){
		mEnabled = value;
		emit(enabled(mEnabled));
   }
}

void CrossFadeModel::updatePosition(float pos){
	if(pos < 0.0f)
		mPosition = 0.0f;
	else if (pos > 1.0f)
		mPosition = 1.0f;
	else
		mPosition = pos;
}

void CrossFadeModel::setPosition(float pos){
	if(mRecursing)
		return;
	mRecursing = true;
	if(pos != mPosition){
		updatePosition(pos);
		emit(positionChanged(mPosition));
	}
	mRecursing = false;
}

