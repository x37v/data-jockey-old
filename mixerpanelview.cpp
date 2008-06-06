#include "masterview.hpp"
#include "mixerpanelview.hpp"
#include "mixerchannelview.hpp"
#include "djmixerchannelview.hpp"
#include "djmixercontrolview.hpp"
#include "djmixerworkinfoview.hpp"
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
		mLayout->addWidget(cur->DJMixerWorkInfo(), 0, i);
		mLayout->addWidget(cur->DJMixerControl(), 1, i, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->eq(), 2, i, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->muteButton(), 3, i, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->volumeSlider(), 4, i, Qt::AlignHCenter);
		mLayout->setColumnStretch(i,0);
	}

	//add the master widgets and the cross fade
	mLayout->addWidget(mMaster->volume(), 4, numMixers, Qt::AlignHCenter);
	mLayout->addWidget(mMaster->tempoWidget(), 0, numMixers, 3, 1, Qt::AlignCenter);
	mLayout->addWidget(mXFade, 5, 0, 1, numMixers + 1);

	//set layout settings
	mLayout->setColumnStretch(numMixers,10);
	mLayout->setRowStretch(0,0);
	mLayout->setRowStretch(4,10);
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

