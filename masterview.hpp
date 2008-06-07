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
		QSlider * volume();
		QDoubleSpinBox * tempo();
		QWidget * tempoWidget();
	private:
		QSlider * mVolumeSlider;
		QDoubleSpinBox * mTempo;
		QDoubleSpinBox * mTempoMul;
		std::vector<QRadioButton *> mSyncSource;
		QWidget * mTempoWidget;
};

#endif

