#ifndef DJ_MIXER_CHANNEL_VIEW_HPP
#define DJ_MIXER_CHANNEL_VIEW_HPP

#include <QWidget>

class DJMixerControlView;
class MixerChannelView;

class DJMixerChannelView : public QWidget {
	Q_OBJECT
	public:
		DJMixerChannelView(QWidget *parent = NULL);
		DJMixerControlView * DJMixerControl();
		MixerChannelView * mixerChannel();
	private:
		DJMixerControlView * mDJMixerControl;
		MixerChannelView * mMixerChannel;
};

#endif

