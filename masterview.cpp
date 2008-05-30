#include "masterview.hpp"
#include <QSlider>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <string>
#include <sstream>

MasterView::MasterView(unsigned int numMixers, QWidget *parent) :
	QWidget(parent)
{
	mTempoWidget = new QWidget(this);
	mVolumeSlider = new QSlider(this);
	mTempo = new QDoubleSpinBox(this);
	mTempoMul = new QDoubleSpinBox(this);
	mSyncSource.push_back(new QRadioButton("master", this));
	mSyncSource.front()->setChecked(true);

	//when syncing to the master clock the tempo mul doesn't work, just the master tempo
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
		newRadioButton = new QRadioButton(syncSrcName.c_str(), this);
		mSyncSource.push_back(newRadioButton);
		//when syncing to a mixer channel the 'master tempo' doesn't work.. just the tempo mul
		QObject::connect(newRadioButton, SIGNAL(clicked(bool)),
				mTempoMul, SLOT(setEnabled(bool)));
		QObject::connect(newRadioButton, SIGNAL(clicked(bool)),
				mTempo, SLOT(setDisabled(bool)));
	}

	mVolumeSlider->setRange(0,200);
	mVolumeSlider->setTickPosition(QSlider::TicksLeft);
	mVolumeSlider->setValue(100);

	//set up the Sync source radio button layout
	QVBoxLayout * syncLayout = new QVBoxLayout();
	QLabel * syncLabel = new QLabel("<b>sync src</b>", this); 
	syncLabel->setToolTip("set master clock sync src");
	syncLayout->addWidget(syncLabel, 0, Qt::AlignHCenter);
	for(std::vector<QRadioButton *>::iterator it = mSyncSource.begin();
			it != mSyncSource.end(); it++){
		syncLayout->addWidget(*it, 0, Qt::AlignLeft);
	}
	syncLayout->setSpacing(1);

	//set up the tempo layout
	QVBoxLayout * tempoLayout = new QVBoxLayout();
	QLabel * tempoLabel = new QLabel("<b>tempo</b>", this);
	QLabel * tempoMulLabel = new QLabel("<b>tempo mul</b>", this);
	mTempo->setToolTip("master tempo (bpm)");
	mTempo->setRange(10.0,300.0);
	mTempoMul->setToolTip("master tempo multiply");
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
}

QSlider * MasterView::volume(){
	return mVolumeSlider;
}

QDoubleSpinBox * MasterView::tempo(){
	return mTempo;
}

QWidget * MasterView::tempoWidget(){
	return mTempoWidget;
}

