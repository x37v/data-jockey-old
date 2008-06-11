#ifndef MASTER_VIEW_HPP
#define MASTER_VIEW_HPP

#include <QWidget>

class QSlider;
class QDoubleSpinBox;
class QRadioButton;

class MasterView : public QObject {
	Q_OBJECT
	public:
		MasterView(unsigned int numMixers, QWidget *parent = NULL);
		QSlider * volume() const;
		QDoubleSpinBox * tempo() const;
		QWidget * tempoWidget() const;
	signals:
		void volumeChanged(float vol);
		void tempoChanged(float tempo);
		void tempoMulChanged(float mul);
		void syncSourceChanged(unsigned int src);
	//public slots will only be called externally
	//so they don't emit signals
	public slots:
		void setVolume(float vol);
		void setTempo(float tempo);
		void setTempoMul(float mul);
		//0 is master, 1 is mixer 0, 2 is mixer 1..
		void setSyncSource(unsigned int src);
	//protected slots are called internally and, do emit signals
	protected slots:
		void setVolume(int vol);
		void setTempoDouble(double tempo);
		void setTempoMulDouble(double mul);
		void syncSrcClicked(bool clicked);
	private:
		bool mRecursing;
		QSlider * mVolumeSlider;
		QDoubleSpinBox * mTempo;
		QDoubleSpinBox * mTempoMul;
		std::vector<QRadioButton *> mSyncSource;
		QWidget * mTempoWidget;
};

#endif

