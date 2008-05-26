#ifndef MIXER_PANEL_VIEW_HPP
#define MIXER_PANEL_VIEW_HPP

#include <QWidget>
#include "djmixercontrolview.hpp"
#include "mixerchannelview.hpp"

class DJMixerControlView;
class EQView;
class QSlider;
class QToolButton;
class QGridLayout;
class MixerPanelView;

class DJMixerChannelView : public QWidget {
	Q_OBJECT
	public:
		DJMixerChannelView(QWidget *parent = NULL);
		DJMixerControlView * DJMixerControl();
		MixerChannelView * mixerChannel();
	private:
		friend class MixerPanelView;
		DJMixerControlView * mDJMixerControl;
		MixerChannelView * mMixerChannel;
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

