#ifndef DJ_MIXER_CHANNEL_VIEW_HPP
#define DJ_MIXER_CHANNEL_VIEW_HPP

#include <QWidget>
#include "mixerchannelview.hpp"

class QVBoxLayout;
class QHBoxLayout;

class DJMixerChannelView : public QWidget {
	Q_OBJECT
	public:
		DJMixerChannelView(QWidget *parent = NULL);
		MixerChannelView * mixerChannel();
		void reset();
	private:
		MixerChannelView * mMixerChannel;
		QVBoxLayout * mLayout;
		QHBoxLayout * mButtonLayout;
};


#endif

