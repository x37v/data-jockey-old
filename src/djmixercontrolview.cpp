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

#include "djmixercontrolview.hpp"
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include <math.h>

PowerOfTwoSpinBox::PowerOfTwoSpinBox(QWidget * parent) :
	QSpinBox(parent)
{
	lineEdit()->setReadOnly(true);
	//connect up our internal signals and slots
	QObject::connect(this,
			SIGNAL(valueChanged(int)),
			this,
			SLOT(setValue(int)));
}

QString PowerOfTwoSpinBox::textFromValue(int val) const {
	QString powerOfTwoVal;
	//if we're greater than or equal to zero then we just print out the power of
	//two value. otherwise, we print out 1 / (2**-val)
	if(val >= 0.0)
		powerOfTwoVal.setNum(pow(2.0,(double)val));
	else {
		QString denom;
		denom.setNum((int)pow(2.0,(double)-val));
		powerOfTwoVal.append("1/");
		powerOfTwoVal.append(denom);
	}
	return powerOfTwoVal;
}

void PowerOfTwoSpinBox::setValue(double val){
	if(val == 0.0)
		QSpinBox::setValue(0);
	else {
		int intVal = (int)(log(val) / log(2.0));
		QSpinBox::setValue(intVal);
	}
}

//this is a protected function just so we emit the correct value
void PowerOfTwoSpinBox::setValue(int val){
	//convert to power of two!
	double powerOfTwoVal = pow(2.0,(double)val);
	emit(valueChanged(powerOfTwoVal));
}

DJMixerControlView::DJMixerControlView(QWidget *parent)
	: QWidget(parent)
{
	mLayout = new QVBoxLayout(this);
	setLayout(mLayout);
	mLayout->setContentsMargins(0,0,0,0);
	mLayout->setSpacing(0);

	mProgressBar = new QProgressBar(this);
	mProgressBar->setAlignment(Qt::AlignHCenter);
	mProgressBar->setValue(0);

	mLoadBtn = new LoadButton(this);
	mResetBtn = new ResetButton(this);
	mCueBtn = new CueButton(this);
	mSyncBtn = new SyncButton(this);
	mPlayBtn = new PlayButton(this);

	mSeekFwdBtn = new SeekFwdButton(this);
	mSeekBkwdBtn = new SeekBkwdButton(this);

	mBeatOffset = new QSpinBox(this);
	mBeatOffset->setToolTip(tr("beat start offset"));
	mBeatOffset->setRange(-16,99);

	mTempoMulFree = new QDoubleSpinBox(this);
	mTempoMulFree->setToolTip(tr("tempo multiplier"));
	mTempoMulFree->setRange(0.01,4);
	mTempoMulFree->setValue(1.0);
	mTempoMulFree->setSingleStep(0.01);

	mTempoMulSynced = new PowerOfTwoSpinBox(this);
	mTempoMulSynced->setToolTip(tr("tempo multiplier"));
	mTempoMulSynced->setRange(-2,2);
	mTempoMulSynced->setValue(0.0);

	//we initially start in sync
	mSyncBtn->setChecked(true);


	QHBoxLayout * loadResetLayout = new QHBoxLayout;
	loadResetLayout->setContentsMargins(0,0,0,0);
	loadResetLayout->setSpacing(0);
	loadResetLayout->addWidget(mLoadBtn, 0, Qt::AlignHCenter);
	loadResetLayout->addWidget(mResetBtn, 0, Qt::AlignHCenter);

	QHBoxLayout * playLayout = new QHBoxLayout;
	playLayout->setContentsMargins(0,0,0,0);
	playLayout->setSpacing(0);
	playLayout->addStretch();
	playLayout->addWidget(mCueBtn, 0, Qt::AlignHCenter);
	playLayout->addWidget(mSyncBtn, 0, Qt::AlignHCenter);
	playLayout->addWidget(mPlayBtn, 0, Qt::AlignHCenter);
	playLayout->addStretch();
	
	//addLayout sets parent, no need to explicitly set it here
	QHBoxLayout * seekLayout = new QHBoxLayout;
	seekLayout->setContentsMargins(0,0,0,0);
	seekLayout->setSpacing(0);
	seekLayout->addStretch();
	seekLayout->addWidget(mSeekBkwdBtn, 0, Qt::AlignHCenter);
	seekLayout->addWidget(mSeekFwdBtn, 0, Qt::AlignHCenter);
	seekLayout->addStretch();

	QHBoxLayout * tempoAndBeatLayout = new QHBoxLayout;
	tempoAndBeatLayout->setContentsMargins(0,0,0,0);
	tempoAndBeatLayout->setSpacing(0);
	tempoAndBeatLayout->addStretch();
	tempoAndBeatLayout->addWidget(mBeatOffset, 0, Qt::AlignHCenter);
	tempoAndBeatLayout->addWidget(mTempoMulSynced, 0, Qt::AlignHCenter);
	tempoAndBeatLayout->addWidget(mTempoMulFree, 0, Qt::AlignHCenter);
	tempoAndBeatLayout->addStretch();

	mLayout->addLayout(loadResetLayout,0);
	mLayout->addLayout(playLayout,0);
	mLayout->addLayout(seekLayout,0);
	mLayout->addWidget(mProgressBar,1, Qt::AlignHCenter);
	mLayout->addLayout(tempoAndBeatLayout, 0);

	//hide the free tempo mul
	mTempoMulFree->setVisible(false);

	//connect up our signals
	QObject::connect(
			mCueBtn,
			SIGNAL(toggled(bool)),
			this, SIGNAL(cueModeChanged(bool)));
	QObject::connect(
			mPlayBtn,
			SIGNAL(toggled(bool)),
			this, SIGNAL(pausedChanged(bool)));
	QObject::connect(
			mSyncBtn,
			SIGNAL(toggled(bool)),
			this, SIGNAL(syncModeChanged(bool)));
	
	//when the sync mode changes the tempo mul step changes too
	QObject::connect(
			mSyncBtn,
			SIGNAL(toggled(bool)),
			this, SLOT(changeTempoMulStep(bool)));

	QObject::connect(
			mSeekFwdBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(seekingFwd()));
	QObject::connect(
			mSeekBkwdBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(seekingBwd()));
	QObject::connect(
			mLoadBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(loadClicked()));
	QObject::connect(
			mResetBtn,
			SIGNAL(clicked(bool)),
			this, SIGNAL(resetClicked()));
	QObject::connect(
			mBeatOffset,
			SIGNAL(valueChanged(int)),
			this, SIGNAL(beatOffsetChanged(int)));
	QObject::connect(
			mTempoMulFree,
			SIGNAL(valueChanged(double)),
			this, SLOT(tempoMulFreeChanged(double)));
	QObject::connect(
			mTempoMulSynced,
			SIGNAL(valueChanged(int)),
			this, SLOT(tempoMulSyncedChanged(int)));
}

bool DJMixerControlView::cueMode() const {
	return mCueBtn->isChecked();
}

bool DJMixerControlView::paused() const {
	return mPlayBtn->isChecked();
}

bool DJMixerControlView::runningFree() const {
	return mSyncBtn->isChecked();
}

int DJMixerControlView::beatOffset() const {
	return mBeatOffset->value();
}

//this slots emit signals when necessary through the actual buttons

void DJMixerControlView::setCueing(bool cueing){
	if(cueing != mCueBtn->isChecked())
		mCueBtn->setChecked(cueing);
}

void DJMixerControlView::setPaused(bool paused){
	if(paused != mPlayBtn->isChecked())
		mPlayBtn->setChecked(paused);
}

void DJMixerControlView::setPlaying(bool playing){
	setPaused(!playing);
}

void DJMixerControlView::setRunFree(bool free){
	setSync(!free);
}

void DJMixerControlView::setSync(bool synced){
	if(synced != mSyncBtn->isChecked())
		mSyncBtn->setChecked(synced);
}

void DJMixerControlView::setProgress(float progress){
	mProgressBar->setValue(100 * progress);
}

void DJMixerControlView::setProgress(int progress){
	mProgressBar->setValue(progress);
}

void DJMixerControlView::setBeatOffset(int offset){
	mBeatOffset->setValue(offset);
}

void DJMixerControlView::setTempoMul(double mul){
	mTempoMulFree->setValue(mul);
	//XXX set power of two value as well..
}

void DJMixerControlView::tempoMulFreeChanged(double mul){
	if(!mSyncBtn->isChecked())
		emit(tempoMulChanged(mul));
}

void DJMixerControlView::tempoMulSyncedChanged(int mul){
	if(mSyncBtn->isChecked()){
		double mulDouble = pow(2.0, (double)mul);
		emit(tempoMulChanged(mulDouble));
	}
}

//we change the tempo mul step based on syncing..
//if we're running free we have a small step, if not we should jump
//by integer values.
void DJMixerControlView::changeTempoMulStep(bool sync){
	if(sync){
		mTempoMulFree->setVisible(false);
		mTempoMulSynced->setVisible(true);
	} else {
		mTempoMulSynced->setVisible(false);
		mTempoMulFree->setVisible(true);
	}
}

