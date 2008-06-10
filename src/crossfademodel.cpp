#include "crossfademodel.hpp"

CrossFadeModel::CrossFadeModel(unsigned int numMixers, QObject * parent) :
	QObject(parent)
{
	mNumMixers = numMixers;
	mEnabled = false;
}

void CrossFadeModel::setMixers(unsigned int left, unsigned int right){
	//make sure we're in range
	if(left > (mNumMixers + 1) || right > (mNumMixers + 1)){
		return;
	}
	if(left != mLeft || right != mRight){
		mLeft = left;
		mRight = right;
		mEnabled = true;
		emit(mixersChanged(mLeft,mRight));
	}
}

void CrossFadeModel::disable(){
	if(mEnabled){
		mEnabled = false;
		emit(disabled());
	}
}


