#ifndef DJ_MIXER_CHANNEL_VIEW_HPP
#define DJ_MIXER_CHANNEL_VIEW_HPP

#include <QObject>

class DJMixerControlView;
class DJMixerWorkInfoView;
class MixerChannelView;

class DJMixerChannelView : public QObject {
	Q_OBJECT
	public:
		DJMixerChannelView(QWidget *parent = NULL);
		DJMixerControlView * control();
		DJMixerWorkInfoView * DJMixerWorkInfo();
		MixerChannelView * mixerChannel();
	public slots:
		void reset();
	private:
		DJMixerControlView * mDJMixerControl;
		DJMixerWorkInfoView * mDJMixerWorkInfo;
		MixerChannelView * mMixerChannel;
};

#endif

