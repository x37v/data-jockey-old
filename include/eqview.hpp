#ifndef EQVIEW_HPP
#define EQVIEW_HPP

#include <QWidget>

class QDial;
class QVBoxLayout;
class QWidget;

class EQView : public QWidget {
	Q_OBJECT
	public:
		EQView(QWidget *parent = NULL);
		QDial * low();
		QDial * mid();
		QDial * high();
		float highValue() const;
		float midValue() const;
		float lowValue() const;
	signals:
		void highChanged(float high);
		void midChanged(float mid);
		void lowChanged(float low);
	public slots:
		void setHigh(float val);
		void setHigh(int val);
		void setMid(float val);
		void setMid(int val);
		void setLow(float val);
		void setLow(int val);
	private:
		QDial * mLow;
		QDial * mMid;
		QDial * mHigh;
		QVBoxLayout * mLayout;
		//to prevent infinite loops
		bool mRecursing;
};

#endif
