#ifndef DJ_MIXER_CHANNEL_MODEL_HPP
#define DJ_MIXER_CHANNEL_MODEL_HPP

#include <QObject>

class DJMixerControlModel;
class MixerChannelModel;

class DJMixerChannelModel : public QObject {
	Q_OBJECT
	public:
		DJMixerChannelModel(QObject * parent = NULL);
		DJMixerControlModel * DJMixerControl() const;
		MixerChannelModel * mixerChannel() const;
		int work() const;
	signals:
		void workChanged(int work);
	public slots:
		void loadWork(int work);
	private:
		int mWork;
		DJMixerControlModel * mDJMixerControl;
		MixerChannelModel * mMixerChannel;
};

#endif
