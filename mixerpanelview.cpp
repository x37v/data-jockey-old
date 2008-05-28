#include "mixerpanelview.hpp"
#include "djmixerchannelview.cpp"
#include <QSlider>
#include <QGridLayout>
#include <QToolButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <string>
#include <sstream>

MixerPanelView::MixerPanelView(unsigned int numMixers, QWidget *parent)
: QWidget(parent)
{
	mLayout = new QGridLayout(this);
	if(numMixers == 0)
		numMixers = 1;
	mSyncSource.push_back(new QRadioButton("master", this));
	mSyncSource.front()->setChecked(true);

	//create and add the mixer channels to the mixer panel, also create the sync src buttons
	for(unsigned int i = 0; i < numMixers; i++){
		std::string syncSrcName("mixer ");
		std::stringstream numAsString; 
		numAsString << i;
		syncSrcName.append(numAsString.str());
		DJMixerChannelView * cur = new DJMixerChannelView;
		mDJMixerChannels.push_back(cur);
		mSyncSource.push_back(new QRadioButton(syncSrcName.c_str(), this));
		mLayout->addWidget(cur->DJMixerControl(),0, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->eq(),1, i, 1, 1, Qt::AlignCenter);
		mLayout->addWidget(cur->mixerChannel()->muteButton(),2, i, 1, 1, Qt::AlignHCenter);
		mLayout->addWidget(cur->mixerChannel()->volumeSlider(),3, i, 1, 1, Qt::AlignHCenter);
		mLayout->setColumnStretch(i,0);
	}


	//create the master volume and have it reflect the settings of the mixer channels volume
	QSlider * tmpSlider = mDJMixerChannels[0]->mixerChannel()->volumeSlider();
	mMasterVolume = new QSlider(this);
	mMasterVolume->setTickPosition(tmpSlider->tickPosition());
	mMasterVolume->setRange(tmpSlider->minimum(), tmpSlider->maximum());
	mMasterVolume->setValue(tmpSlider->value());
	mLayout->addWidget(mMasterVolume, 3, numMixers, 1, 1, Qt::AlignHCenter);

	QVBoxLayout * syncLayout = new QVBoxLayout();
	QLabel * syncLabel = new QLabel("<b>sync src</b>", this); 
	syncLabel->setToolTip("set master clock sync src");
	syncLayout->addWidget(syncLabel, 0, Qt::AlignHCenter);
	for(std::vector<QRadioButton *>::iterator it = mSyncSource.begin();
			it != mSyncSource.end(); it++){
		syncLayout->addWidget(*it, 0, Qt::AlignLeft);
	}
	syncLayout->setSpacing(1);
	QVBoxLayout * tempoLayout = new QVBoxLayout();
	QLabel * tempoLabel = new QLabel("<b>tempo</b>", this);
	mTempo = new QDoubleSpinBox(this);
	mTempo->setToolTip("master tempo (bpm)");
	mTempo->setRange(10.0,300.0);
	tempoLayout->addWidget(tempoLabel, 0, Qt::AlignHCenter);
	tempoLayout->addWidget(mTempo, 0, Qt::AlignHCenter);
	tempoLayout->addLayout(syncLayout, 0);
	mLayout->addLayout(tempoLayout, 0, numMixers, 2, 1, Qt::AlignCenter);

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

QSlider * MixerPanelView::masterVolume(){
	return mMasterVolume;
}

QDoubleSpinBox * MixerPanelView::tempo(){
	return mTempo;
}

