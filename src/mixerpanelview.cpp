/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

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
		mLayout->addWidget(cur->control(), 1, i, Qt::AlignCenter);
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

CrossFadeView * MixerPanelView::crossFade() const {
	return mXFade;
}

MasterView * MixerPanelView::master() const {
	return mMaster;
}

std::vector<DJMixerChannelView *> * MixerPanelView::mixerChannels() {
	return &mDJMixerChannels;
}

