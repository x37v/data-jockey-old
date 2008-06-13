#ifndef DJ_MIXER_CHANNEL_MODEL_HPP
#define DJ_MIXER_CHANNEL_MODEL_HPP

#include <QObject>

class DJMixerControlModel;
class MixerChannelModel;

class DJMixerChannelModel : public QObject {
	Q_OBJECT
	public:
		DJMixerChannelModel(QObject * parent = NULL);
		//connect this model to another model of this same type
		void connectSignalsTo(DJMixerChannelModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		DJMixerControlModel * DJMixerControl() const;
		MixerChannelModel * mixerChannel() const;
		int work() const;
	signals:
		void workChanged(int work);
	public slots:
		void setWork(int work);
		void reset();
	private:
		int mWork;
		DJMixerControlModel * mDJMixerControl;
		MixerChannelModel * mMixerChannel;
};

#endif
