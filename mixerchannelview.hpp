#ifndef MIXER_CHANNEL_VIEW_HPP
#define MIXER_CHANNEL_VIEW_HPP

#include "eqview.hpp"
#include <QVBoxLayout>
#include <QSlider>
#include <QToolButton>

class MixerChannelView : public QWidget {
	Q_OBJECT
	public:
		MixerChannelView(QWidget *parent = NULL);
		EQView * eq();
		QSlider * volumeSlider();
		QToolButton * muteButton();
		void reset();
	private:
		EQView * mEQView;
		QSlider * mVolumeSlider;
		QVBoxLayout * mLayout;
		QToolButton * mMuteBtn;
};

#endif

