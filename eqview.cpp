#include "eqview.hpp"
#include <QDial>
#include <QVBoxLayout>
#include <QWidget>

EQView::EQView(QWidget *parent)
	: QWidget(parent) 
{
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

	//mLow.setMouseTracking(true);
	//mMid.setMouseTracking(true);
	//mHigh.setMouseTracking(true);
	
	mLayout->setSpacing(0);
	mLayout->setContentsMargins(0,0,0,0);
	mLayout->addWidget(mHigh, 0, Qt::AlignHCenter);
	mLayout->addWidget(mMid, 0, Qt::AlignHCenter);
	mLayout->addWidget(mLow, 0, Qt::AlignHCenter);

	setLayout(mLayout);
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

void EQView::reset(){
	mLow->setValue(0);
	mMid->setValue(0);
	mHigh->setValue(0);
}

