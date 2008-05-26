#ifndef MIXER_PANEL_VIEW_HPP
#define MIXER_PANEL_VIEW_HPP

#include <QWidget>
#include "djmixercontrolview.hpp"

class DJMixerControlView;
class EQView;
class QSlider;
class MixerPanelView;
class QGridLayout;

class DJMixerChannelView {
	public:
		EQView * eq();
		QSlider * volumeSlider();
		DJMixerControlView * DJMixerControl();
	private:
		friend class MixerPanelView;
		EQView * mEQ;
		QSlider * mVolumeSlider;
		DJMixerControlView * mDJMixerControl;
};

class MixerPanelView : public QWidget {
	Q_OBJECT
	public:
		MixerPanelView(unsigned int numMixers = 4, QWidget *parent = NULL);
		//XXX destructor
	private:
		QGridLayout * mLayout;
		std::vector<DJMixerChannelView *> mDJMixerChannels;
		QSlider * mMasterVolume;
};

#endif

