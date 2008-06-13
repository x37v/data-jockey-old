#ifndef MASTER_MODEL_HPP
#define MASTER_MODEL_HPP

#include <QObject>

class MasterModel : public QObject {
	Q_OBJECT
	public:
		MasterModel(unsigned int numMixers, QObject *parent = NULL);
		//connect this model to another model of this same type
		void connectSignalsTo(MasterModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);
	signals:
		void volumeChanged(float vol);
		void tempoChanged(float tempo);
		void tempoMulChanged(float mul);
		void syncSourceChanged(unsigned int src);
	public slots:
		void setVolume(float vol);
		void setTempo(float tempo);
		void setTempoMul(float mul);
		//0 is master, 1 is mixer 0, 2 is mixer 1..
		void setSyncSource(unsigned int src);
	private:
		float mVolume;
		float mTempo;
		float mTempoMul;
		unsigned int mSyncSource;
		unsigned int mNumMixers;
};

#endif

