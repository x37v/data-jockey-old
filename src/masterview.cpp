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
#include <QSlider>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <string>
#include <sstream>

MasterView::MasterView(unsigned int numMixers, QWidget *parent) :
	QObject(parent)
{
	mRecursing = false;
	mTempoWidget = new QWidget(parent);
	mVolumeSlider = new QSlider(parent);
	mTempo = new QDoubleSpinBox(parent);
	mTempoMul = new QDoubleSpinBox(parent);
	mSyncSource.push_back(new QRadioButton("master", parent));
	mSyncSource.front()->setChecked(true);

	//interally call our syncSrcClicked, so that we can send our
	//syncSourceChanged signal
	QObject::connect(mSyncSource.front(), SIGNAL(clicked(bool)),
			this, SLOT(syncSrcClicked(bool)));

	//when syncing to the master clock the tempo mul doesn't work, just the master tempo
	//XXX should be done by the model?
	QObject::connect(mSyncSource.front(), SIGNAL(clicked(bool)),
                      mTempoMul, SLOT(setDisabled(bool)));
	QObject::connect(mSyncSource.front(), SIGNAL(clicked(bool)),
                      mTempo, SLOT(setEnabled(bool)));

	for(unsigned int i = 0; i < numMixers; i++){
		QRadioButton * newRadioButton;
		std::string syncSrcName("mixer ");
		std::stringstream numAsString; 
		numAsString << i;
		syncSrcName.append(numAsString.str());
		newRadioButton = new QRadioButton(syncSrcName.c_str(), parent);
		mSyncSource.push_back(newRadioButton);

		//interally call our syncSrcClicked, so that we can send our
		//syncSourceChanged signal
		QObject::connect(newRadioButton, SIGNAL(clicked(bool)),
				this, SLOT(syncSrcClicked(bool)));

		//when syncing to a mixer channel the 'master tempo' doesn't work.. just the tempo mul
		//XXX should this should be done by the model?
		QObject::connect(newRadioButton, SIGNAL(clicked(bool)),
				mTempoMul, SLOT(setEnabled(bool)));
		QObject::connect(newRadioButton, SIGNAL(clicked(bool)),
				mTempo, SLOT(setDisabled(bool)));
	}

	mVolumeSlider->setRange(0,200);
	mVolumeSlider->setTickPosition(QSlider::TicksLeft);
	mVolumeSlider->setValue(100);
	mVolumeSlider->setToolTip(tr("master volume"));

	//set up the Sync source radio button layout
	QVBoxLayout * syncLayout = new QVBoxLayout();
	QLabel * syncLabel = new QLabel(tr("sync src"), parent); 
	syncLabel->setToolTip(tr("set master clock sync src"));
	syncLayout->addWidget(syncLabel, 0, Qt::AlignHCenter);
	for(std::vector<QRadioButton *>::iterator it = mSyncSource.begin();
			it != mSyncSource.end(); it++){
		syncLayout->addWidget(*it, 0, Qt::AlignLeft);
	}
	syncLayout->setSpacing(1);

	//set up the tempo layout
	QVBoxLayout * tempoLayout = new QVBoxLayout();
	QLabel * tempoLabel = new QLabel(tr("tempo"), parent);
	QLabel * tempoMulLabel = new QLabel(tr("tempo mul"), parent);
	mTempo->setToolTip(tr("master tempo (bpm)"));
	mTempo->setRange(10.0,300.0);
	mTempo->setValue(120.0);
	mTempoMul->setToolTip(tr("master tempo multiply"));
	mTempoMul->setRange(0.1,10.0);
	mTempoMul->setSingleStep(0.01);
	mTempoMul->setValue(1.0);
	mTempoMul->setDisabled(true);

	//add items to the layout
	tempoLayout->addWidget(tempoLabel, 0, Qt::AlignHCenter);
	tempoLayout->addWidget(mTempo, 0, Qt::AlignHCenter);
	tempoLayout->addWidget(tempoMulLabel, 0, Qt::AlignHCenter);
	tempoLayout->addWidget(mTempoMul, 0, Qt::AlignHCenter);
	tempoLayout->addLayout(syncLayout, 0);
	mTempoWidget->setLayout(tempoLayout);

	//connect our widget's signals to our slots (or signals)
	QObject::connect(
			mVolumeSlider,
			SIGNAL(valueChanged(int)),
			this, SLOT(setVolume(int)));
	QObject::connect(
			mTempo,
			SIGNAL(valueChanged(double)),
			this, SLOT(setTempoDouble(double)));
	QObject::connect(
			mTempoMul,
			SIGNAL(valueChanged(double)),
			this, SLOT(setTempoMulSelf(double)));

	//XXX we get rid of the tempo mul for now because it isn't being used
	mTempoMul->setVisible(false);
}

QSlider * MasterView::volume() const {
	return mVolumeSlider;
}

QDoubleSpinBox * MasterView::tempo() const {
	return mTempo;
}

QWidget * MasterView::tempoWidget() const {
	return mTempoWidget;
}

void MasterView::setVolume(float vol){
	if(mRecursing)
		return;
	mRecursing = true;

	mVolumeSlider->setValue((int)(100 * vol));

	mRecursing = false;
}

void MasterView::setTempo(float tempo){
	if(mRecursing)
		return;
	mRecursing = true;

	mTempo->setValue(tempo);

	mRecursing = false;
}

void MasterView::setTempoMul(double mul){
	if(mRecursing)
		return;
	mRecursing = true;

	mTempoMul->setValue(mul);

	mRecursing = false;
}

void MasterView::setTempoMulSelf(double mul){
	if(mRecursing)
		return;
	mRecursing = true;

	//always emit because this is interal, will never happen unless there is a
	//change
	emit(tempoMulChanged(mul));

	mRecursing = false;
}

void MasterView::setSyncSource(unsigned int src){
	if(src < mSyncSource.size() && !mSyncSource[src]->isChecked()){
		mSyncSource[src]->setChecked(true);
	}
}

//this is only called internally so it simply emits the value
void MasterView::setVolume(int vol){
	if(mRecursing)
		return;
	mRecursing = true;

	float volFloat = ((float)vol) / 100.0f;
	emit(volumeChanged(volFloat));
	
	mRecursing = false;
}

//this is only called internally so it simply emits the value
void MasterView::setTempoDouble(double tempo){
	if(mRecursing)
		return;
	mRecursing = true;

	emit(tempoChanged(tempo));

	mRecursing = false;
}

void MasterView::syncSrcClicked(bool clicked){
	if(clicked) {
		for(unsigned int i = 0; i < mSyncSource.size(); i++){
			if(mSyncSource[i]->isChecked()){
				emit(syncSourceChanged(i));
				break;
			}
		}
	}
}

