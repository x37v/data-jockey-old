#include "eqview.hpp"
#include <QDial>
#include <QVBoxLayout>
#include <QWidget>

EQView::EQView(QWidget *parent)
	: QWidget(parent) 
{
	mRecursing = false;
	mLow = new QDial(this);
	mMid = new QDial(this);
	mHigh = new QDial(this);
	mLayout = new QVBoxLayout(this);
	mLow->setRange(-100,100);
	mMid->setRange(-100,100);
	mHigh->setRange(-100,100);

	mLow->setToolTip("eq low");
	mMid->setToolTip("eq mid");
	mHigh->setToolTip("eq high");

	mLow->setFixedSize(mLow->minimumSizeHint());
	mMid->setFixedSize(mMid->minimumSizeHint());
	mHigh->setFixedSize(mHigh->minimumSizeHint());

	//mLow->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	//mMid->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	//mHigh->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	//mLow.setMouseTracking(true);
	//mMid.setMouseTracking(true);
	//mHigh.setMouseTracking(true);
	
	mLayout->setSpacing(0);
	mLayout->setContentsMargins(0,0,0,0);
	mLayout->addWidget(mHigh, 0, Qt::AlignHCenter);
	mLayout->addWidget(mMid, 0, Qt::AlignHCenter);
	mLayout->addWidget(mLow, 0, Qt::AlignHCenter);

	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	setLayout(mLayout);

	QObject::connect(
			mHigh,
			SIGNAL(valueChanged(int)),
			this, SLOT(setHigh(int)));
	QObject::connect(
			mMid,
			SIGNAL(valueChanged(int)),
			this, SLOT(setMid(int)));
	QObject::connect(
			mLow,
			SIGNAL(valueChanged(int)),
			this, SLOT(setLow(int)));
}

QDial * EQView::low(){
	return mLow;
}

QDial * EQView::mid(){
	return mMid;
}

QDial * EQView::high(){
	return mHigh;
}

float EQView::highValue() const {
	return (float)mHigh->value() / 100.0;
}

float EQView::midValue() const {
	return (float)mMid->value() / 100.0;
}

float EQView::lowValue() const {
	return (float)mLow->value() / 100.0;
}

void EQView::setHigh(float val){
	if(mRecursing)
		return;
	mRecursing = true;

	int intVal = 100 * val;
	if(intVal != mHigh->value()){
		mHigh->setValue(intVal);
		emit(highChanged(val));
	}

	mRecursing = false;
}

void EQView::setHigh(int val){
	if(mRecursing)
		return;
	mRecursing = true;

	float floatVal = ((float)val) / 100.0;
	if(val != mHigh->value()){
		mHigh->setValue(val);
	}
	//always need to emit because this could come from
	//an internal signal
	emit(highChanged(floatVal));

	mRecursing = false;
}

void EQView::setMid(float val){
	if(mRecursing)
		return;
	mRecursing = true;

	int intVal = 100 * val;
	if(intVal != mMid->value()){
		mMid->setValue(intVal);
		emit(midChanged(val));
	}

	mRecursing = false;
}

void EQView::setMid(int val){
	if(mRecursing)
		return;
	mRecursing = true;

	float floatVal = ((float)val) / 100.0;
	if(val != mMid->value()){
		mMid->setValue(val);
	}
	//always need to emit because this could come from
	//an internal signal
	emit(midChanged(floatVal));

	mRecursing = false;
}

void EQView::setLow(float val){
	if(mRecursing)
		return;
	mRecursing = true;

	int intVal = 100 * val;
	if(intVal != mLow->value()){
		mLow->setValue(intVal);
		emit(lowChanged(val));
	}

	mRecursing = false;
}

void EQView::setLow(int val){
	if(mRecursing)
		return;
	mRecursing = true;

	float floatVal = ((float)val) / 100.0;
	if(val != mLow->value()){
		mLow->setValue(val);
	}
	//always need to emit because this could come from
	//an internal signal
	emit(lowChanged(floatVal));

	mRecursing = false;
}

