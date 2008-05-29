#include "mixerpanelview.hpp"
#include "djmixerchannelview.cpp"
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
	//create and add the mixer channels to the mixer panel, also create the sync src buttons
	for(unsigned int i = 0; i < numMixers; i++){
		DJMixerChannelView * cur = new DJMixerChannelView;
		mDJMixerChannels.push_back(cur);
		mLayout->addWidget(cur->DJMixerControl(),0, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->eq(),1, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->muteButton(),2, i, 1, 1, Qt::AlignHCenter);
		mLayout->addWidget(cur->mixerChannel()->volumeSlider(),3, i, 1, 1, Qt::AlignHCenter);
		mLayout->setColumnStretch(i,0);
	}

	mLayout->addWidget(mMaster->volume(), 3, numMixers, 1, 1, Qt::AlignHCenter);
	mLayout->addWidget(mMaster->tempoWidget(), 0, numMixers, 2, 1, Qt::AlignCenter);

	//set layout settings
	mLayout->setColumnStretch(numMixers,10);
	mLayout->setRowStretch(0,0);
	mLayout->setRowStretch(1,0);
	mLayout->setRowStretch(2,0);
	mLayout->setRowStretch(3,10);
	mLayout->setSpacing(1);
	mLayout->setContentsMargins(0,0,0,0);
	setLayout(mLayout);
}


