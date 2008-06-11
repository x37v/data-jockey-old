#ifndef EQ_MODEL_HPP
#define EQ_MODEL_HPP

#include <QObject>

class EQModel : public QObject {
	Q_OBJECT
	public:
		EQModel(QObject *parent = NULL);
		float high();
		float mid();
		float low();
	signals:
		void highChanged(float high);
		void midChanged(float mid);
		void lowChanged(float low);
		void highCutChanged(bool cut);
		void midCutChanged(bool cut);
		void lowCutChanged(bool cut);
		void valuesChanged(float low, float mid, float high);
	public slots:
		void reset();
		void setHigh(float val);
		void setMid(float val);
		void setLow(float val);
		void toggleHighCut();
		void toggleMidCut();
		void toggleLowCut();
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

