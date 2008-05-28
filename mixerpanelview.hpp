#ifndef MIXER_PANEL_VIEW_HPP
#define MIXER_PANEL_VIEW_HPP

#include <QWidget>

class DJMixerChannelView;
class EQView;
class QSlider;
class QGridLayout;
class QDoubleSpinBox;
class QRadioButton;

class MixerPanelView : public QWidget {
	Q_OBJECT
	public:
		MixerPanelView(unsigned int numMixers = 4, QWidget *parent = NULL);
		//XXX destructor?
		QSlider * masterVolume();
		QDoubleSpinBox * tempo();
	private:
		QGridLayout * mLayout;
		std::vector<DJMixerChannelView *> mDJMixerChannels;
		std::vector<QRadioButton *> mSyncSource;
		QSlider * mMasterVolume;
		QDoubleSpinBox * mTempo;
};

#endif

