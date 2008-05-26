#include "mixerpanelview.hpp"
#include "djmixerchannelview.cpp"
#include <QSlider>
#include <QGridLayout>
#include <QToolButton>

MixerPanelView::MixerPanelView(unsigned int numMixers, QWidget *parent)
: QWidget(parent)
{
	mLayout = new QGridLayout(this);
	if(numMixers == 0)
		numMixers = 1;
	for(unsigned int i = 0; i < numMixers; i++){
		DJMixerChannelView * cur = new DJMixerChannelView;
		mDJMixerChannels.push_back(cur);
		mLayout->addWidget(cur->DJMixerControl(),0, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->eq(),1, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->muteButton(),2, i, 1, 1, Qt::AlignHCenter);
		mLayout->addWidget(cur->mixerChannel()->volumeSlider(),3, i, 1, 1, Qt::AlignHCenter);
		mLayout->setColumnStretch(i,0);
	}

	//create the master volume and have it reflect the settings of the mixer channels volume
	mMasterVolume = new QSlider(this);
	mMasterVolume->setTickPosition(mDJMixerChannels[0]->mixerChannel()->volumeSlider()->tickPosition());
	mMasterVolume->setRange(mDJMixerChannels[0]->mixerChannel()->volumeSlider()->minimum(),
			mDJMixerChannels[0]->mixerChannel()->volumeSlider()->maximum());
	mMasterVolume->setValue(100);
	mLayout->addWidget(mMasterVolume, 3, numMixers, 1, 1, Qt::AlignHCenter);

	//set layout settings
	mLayout->setColumnStretch(numMixers,0);
	mLayout->setRowStretch(0,0);
	mLayout->setRowStretch(1,0);
	mLayout->setRowStretch(2,0);
	mLayout->setRowStretch(3,10);
	mLayout->setSpacing(0);
	mLayout->setContentsMargins(0,0,0,0);
	setLayout(mLayout);
}
