#include "masterview.hpp"
#include "mixerpanelview.hpp"
#include "mixerchannelview.hpp"
#include "djmixerchannelview.hpp"
#include "djmixercontrolview.hpp"
#include "crossfadeview.hpp"
#include "eqview.hpp"
#include <QSlider>
#include <QGridLayout>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QRadioButton>

MixerPanelView::MixerPanelView(unsigned int numMixers, QWidget *parent)
: QWidget(parent)
{
	mLayout = new QGridLayout(this);
	if(numMixers == 0)
		numMixers = 1;
	mMaster = new MasterView(numMixers, this);
	mXFade = new CrossFadeView(numMixers, this);

	//create and add the mixer channels to the mixer panel
	for(unsigned int i = 0; i < numMixers; i++){
		DJMixerChannelView * cur = new DJMixerChannelView(this);
		mDJMixerChannels.push_back(cur);
		mLayout->addWidget(cur->DJMixerControl(),0, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->eq(),1, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->muteButton(),2, i, 1, 1, Qt::AlignHCenter);
		mLayout->addWidget(cur->mixerChannel()->volumeSlider(),3, i, 1, 1, Qt::AlignHCenter);
		mLayout->setColumnStretch(i,0);
	}

	//add the master widgets and the cross fade
	mLayout->addWidget(mMaster->volume(), 3, numMixers, 1, 1, Qt::AlignHCenter);
	mLayout->addWidget(mMaster->tempoWidget(), 0, numMixers, 2, 1, Qt::AlignCenter);
	mLayout->addWidget(mXFade, 4, 0, 1, numMixers + 1);

	//set layout settings
	mLayout->setColumnStretch(numMixers,10);
	mLayout->setRowStretch(0,0);
	mLayout->setRowStretch(1,0);
	mLayout->setRowStretch(2,0);
	mLayout->setRowStretch(3,10);
	mLayout->setRowStretch(4,0);
	mLayout->setSpacing(1);
	mLayout->setContentsMargins(0,0,0,0);
	setLayout(mLayout);
}


CrossFadeView * MixerPanelView::crossFade(){
	return mXFade;
}

MasterView * MixerPanelView::master(){
	return mMaster;
}

std::vector<DJMixerChannelView *> * MixerPanelView::mixerChannels(){
	return &mDJMixerChannels;
}

