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
	public slots:
		void reset();
		void setHigh(float val);
		void setMid(float val);
		void setLow(float val);
	private:
		float mHigh;
		float mMid;
		float mLow;
};

#endif

