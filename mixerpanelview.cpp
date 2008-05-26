#include "mixerpanelview.hpp"
#include "djmixercontrolview.hpp"
#include "eqview.hpp"
#include <QSlider>
#include <QGridLayout>

EQView * DJMixerChannelView::eq(){
	return mEQ;
}

QSlider * DJMixerChannelView::volumeSlider(){
	return mVolumeSlider;
}

DJMixerControlView * DJMixerChannelView::DJMixerControl(){
	return mDJMixerControl;
}

MixerPanelView::MixerPanelView(unsigned int numMixers, QWidget *parent)
: QWidget(parent)
{
	mLayout = new QGridLayout(this);
	mMasterVolume = new QSlider(this);
	for(unsigned int i = 0; i < numMixers; i++){
		DJMixerChannelView * cur = new DJMixerChannelView;
		mDJMixerChannels.push_back(cur);
		cur->mVolumeSlider = new QSlider(this);
		cur->mEQ = new EQView(this);
		cur->mDJMixerControl = new DJMixerControlView(this);
		mLayout->addWidget(cur->DJMixerControl(),0, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->eq(),1, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->volumeSlider(),2, i, 1, 1, Qt::AlignHCenter);
		mLayout->setColumnStretch(i,0);
	}
	mLayout->addWidget(mMasterVolume, 2, numMixers, 1, 1, Qt::AlignHCenter);
	mLayout->setColumnStretch(numMixers,0);
	mLayout->setRowStretch(0,0);
	mLayout->setRowStretch(1,0);
	mLayout->setRowStretch(2,10);
	mLayout->setSpacing(0);
	mLayout->setContentsMargins(0,0,0,0);
	setLayout(mLayout);
}
