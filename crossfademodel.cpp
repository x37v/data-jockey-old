#include "crossfademodel.hpp"

CrossFadeModel::CrossFadeModel(QObject * parent) :
	QObject(parent)
{
	mEnabled = false;
}

void CrossFadeModel::setMixers(unsigned int left, unsigned int right){
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


