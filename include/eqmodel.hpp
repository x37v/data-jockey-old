#ifndef EQ_MODEL_HPP
#define EQ_MODEL_HPP

#include <QObject>

class EQModel : public QObject {
	Q_OBJECT
	public:
		EQModel(QObject *parent = NULL);
		enum band {LOW,MID,HIGH};
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(EQModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);
		float high();
		float mid();
		float low();
		float value(band b);
	signals:
		void highChanged(float high);
		void midChanged(float mid);
		void lowChanged(float low);
		void highCutChanged(bool cut);
		void midCutChanged(bool cut);
		void lowCutChanged(bool cut);
		void valuesChanged(float low, float mid, float high);
		void valuesChanged(QObject * ob);
	public slots:
		void reset();
		void set(band b, float val);
		void toggleCut(band b);
		void cut(band b, bool cut);
		void cutLow(bool cut);
		void cutMid(bool cut);
		void cutHigh(bool cut);
		void setHigh(float val);
		void setMid(float val);
		void setLow(float val);
		void toggleHighCut();
		void toggleMidCut();
		void toggleLowCut();
	//protected slots, they don't emit signals, just change our internal values
	protected slots:
		void updateHigh(float val);
		void updateMid(float val);
		void updateLow(float val);
	private:
		float mHigh;
		float mMid;
		float mLow;
		float mHighLast;
		float mMidLast;
		float mLowLast;
		bool mCuttingHigh;
		bool mCuttingMid;
		bool mCuttingLow;
};

#endif

