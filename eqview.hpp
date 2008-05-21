#ifndef EQVIEW_HPP
#define EQVIEW_HPP

#include <QWidget>

class QDial;
class QVBoxLayout;
class QWidget;
class QLabel;

class EQView : public QWidget {
	Q_OBJECT
	public:
		EQView(QWidget *parent = NULL);
		void reset();
		QDial * low();
		QDial * mid();
		QDial * high();
	private:
		QDial * mLow;
		QDial * mMid;
		QDial * mHigh;
		QLabel * mLowLabel;
		QLabel * mMidLabel;
		QLabel * mHighLabel;
		QVBoxLayout * mLayout;
};

#endif
