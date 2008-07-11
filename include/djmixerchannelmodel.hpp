#ifndef DJ_MIXER_CHANNEL_MODEL_HPP
#define DJ_MIXER_CHANNEL_MODEL_HPP

#include <QObject>
#include "mixerchannelmodel.hpp"

class DJMixerControlModel;

class DJMixerChannelModel : public MixerChannelModel {
	Q_OBJECT
	public:
		DJMixerChannelModel(QObject * parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(DJMixerChannelModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		DJMixerControlModel * control() const;
		int work() const;
	signals:
		void workChanged(int work);
		void loadingWork(int work);
		void loadingWork(QObject * me, int work);
	public slots:
		void setWork(int work);
		//loadWork doesn't set our work.. though, eventually, once the work is
		//loaded set work will be called
		void loadWork(int work);
		void reset();
	private:
		int mWork;
		DJMixerControlModel * mDJMixerControl;
};

#endif
