#ifndef MIXER_CHANNEL_VIEW_HPP
#define MIXER_CHANNEL_VIEW_HPP

#include "eqview.hpp"
#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>

class MixerChannelView : public QWidget {
	Q_OBJECT
	public:
		MixerChannelView(QWidget *parent = NULL);
		EQView * eq();
		QSlider * volumeSlider();
		void reset();
	private:
		EQView * mEQView;
		QSlider * mVolumeSlider;
		QVBoxLayout * mLayout;
		QPushButton * mMuteBtn;
};

#endif

