#ifndef DJ_MIXER_CHANNEL_MODEL_HPP
#define DJ_MIXER_CHANNEL_MODEL_HPP

#include <QObject>

class DJMixerControlModel;
class MixerChannelModel;

class DJMixerChannelModel : public QObject {
	Q_OBJECT
	public:
		DJMixerChannelModel(QObject * parent = NULL);
		DJMixerControlModel * DJMixerControl();
		MixerChannelModel * mixerChannel();
	private:
		DJMixerControlModel * mDJMixerControl;
		MixerChannelModel * mMixerChannel;
};

#endif
