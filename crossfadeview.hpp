#ifndef CROSS_FADE_VIEW_HPP
#define CROSS_FADE_VIEW_HPP

#include <QWidget>
#include <vector>

class QVBoxLayout;
class QSlider;
class QComboBox;

class CrossFadeView : public QWidget {
	Q_OBJECT
	public:
		CrossFadeView(unsigned int numMixers, QWidget *parent = NULL);
		~CrossFadeView();
	private:
		QVBoxLayout * mLayout;
		QSlider * mSlider;
		QComboBox * mSelection;
		std::vector<unsigned int *> mPairList;
};

#endif

