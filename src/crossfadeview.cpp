#include "crossfadeview.hpp"
#include <QVBoxLayout>
#include <QSlider>
#include <QComboBox>
#include <QList>
#include <string>
#include <sstream>

CrossFadeView::CrossFadeView(unsigned int numMixers, QWidget *parent) :
	QWidget(parent)
{
	mSlider = new QSlider(Qt::Horizontal, this);
	mLayout = new QVBoxLayout(this);
	mSelection = new QComboBox(this);

	//set up Slider params
	mSlider->setTickPosition(QSlider::TicksBelow);
	mSlider->setRange(0,100);
	mSlider->setToolTip(tr("crossfade slider"));
	
	//set up the Combo Box
	mSelection->addItem("disabled", 0);
	mSelection->setToolTip(tr("crossfade mixer selection"));
	for(unsigned int i = 0; i < numMixers; i++){
		std::stringstream leftNum; 
		leftNum << i;
		for(unsigned int j = 0; j < numMixers; j++){
			if(j != i){
				unsigned int * pair = new unsigned int[2];
				pair[0] = i;
				pair[1] = j;
				mPairList.push_back(pair);
				std::stringstream rightNum; 
				std::string label("");
				rightNum << j;
				label.append(leftNum.str());
				label.append(" <-> ");
				label.append(rightNum.str());
				mSelection->addItem(label.c_str(), pair);
			}
		}
	}

	//set up layout params
	mLayout->setSpacing(5);
	mLayout->setContentsMargins(20,0,20,0);

	//add items to the layout
	mLayout->addStretch();
	mLayout->addWidget(mSlider, 10);
	mLayout->addWidget(mSelection, 0, Qt::AlignHCenter);
	mLayout->addStretch();

	//set this widgets layout to mLayout
	setLayout(mLayout);

	//connect our internal signals
	QObject::connect(mSlider,
			SIGNAL(valueChanged(int)),
			this,
			SLOT(setPosition(int)));
	QObject::connect(mSelection,
			SIGNAL(currentIndexChanged(int)),
			this,
			SLOT(setIndex(int)));
}

CrossFadeView::~CrossFadeView(){
	//clean up the pair list
	for(std::vector<unsigned int *>::iterator it = mPairList.begin();
			it != mPairList.end(); it++)
		delete [] *it;
}

void CrossFadeView::setMixers(unsigned int left, unsigned int right){
	for(unsigned int i = 0; i < mPairList.size(); i++){
		if(mPairList[i][0] == left && mPairList[i][1] == right){
			//index 0 is 'disabled'
			mSelection->setCurrentIndex(i + 1);
			break;
		}
	}
}

void CrossFadeView::disable(){
	if(mSelection->currentIndex() != 0){
		mSelection->setCurrentIndex(0);
		emit(disabled());
	}
}

void CrossFadeView::setIndex(int index){
	if(index == 0){
		emit(disabled());
	} else if (index > 0 && (index - 1) < (int)mPairList.size()){
		int left, right;
		left = mPairList[index - 1][0];
		right = mPairList[index - 1][1];
		emit(mixersChanged(left,right));
	} else {
		//XXX this shouldn't ever happen
	}
}

void CrossFadeView::setPosition(float val){
	if(val < 0.0f)
		val = 0.0f;
	else if(val > 1.0f)
		val = 1.0f;
	mSlider->setValue((int)(val * 100));
}

void CrossFadeView::setPosition(int val){
	float floatVal = (float)val / 100.0f;
	emit(positionChanged(floatVal));
}

