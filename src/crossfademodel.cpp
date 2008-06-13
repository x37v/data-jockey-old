#include "crossfademodel.hpp"

CrossFadeModel::CrossFadeModel(unsigned int numMixers, QObject * parent) :
	QObject(parent)
{
	mNumMixers = numMixers;
	mEnabled = false;
}

//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
void CrossFadeModel::syncToModel(CrossFadeModel * other, Qt::ConnectionType connectionType){
	//this -> other
	QObject::connect(this,
			SIGNAL(mixersChanged(unsigned int, unsigned int)),
			other,
			SLOT(setMixers(unsigned int, unsigned int)),
			connectionType);
	QObject::connect(this,
			SIGNAL(disabled()),
			other,
			SLOT(disable()),
			connectionType);
	//other -> this
	QObject::connect(other,
			SIGNAL(mixersChanged(unsigned int, unsigned int)),
			this,
			SLOT(setMixers(unsigned int, unsigned int)),
			connectionType);
	QObject::connect(other,
			SIGNAL(disabled()),
			this,
			SLOT(disable()),
			connectionType);
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

