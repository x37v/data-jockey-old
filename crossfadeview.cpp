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
	
	//set up the Combo Box
	mSelection->addItem("disabled", 0);
	for(unsigned int i = 0; i < numMixers; i++){
		std::stringstream leftNum; 
		leftNum << i;
		for(unsigned int j = 0; j < numMixers; j++){
			if(j != i){
				unsigned int * pair = new unsigned int[2];
				mPairList.push_back(pair);
				std::stringstream rightNum; 
				std::string label("");
				rightNum << j;
				label.append(leftNum.str());
				label.append(" <-> ");
				label.append(rightNum.str());
				pair[0] = i;
				pair[1] = j;
				mSelection->addItem(label.c_str(), pair);
			}
		}
	}

	//set up layout params
	mLayout->setSpacing(5);
	mLayout->setContentsMargins(0,0,0,0);

	//add items to the layout
	mLayout->addStretch();
	mLayout->addWidget(mSlider, 10, Qt::AlignHCenter);
	mLayout->addWidget(mSelection, 0, Qt::AlignHCenter);
	mLayout->addStretch();

	//set this widgets layout to mLayout
	setLayout(mLayout);
}

CrossFadeView::~CrossFadeView(){
	//clean up the pair list
	for(std::vector<unsigned int *>::iterator it = mPairList.begin();
			it != mPairList.end(); it++)
		delete [] *it;
}

