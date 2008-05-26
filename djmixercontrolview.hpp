#ifndef DJ_MIXER_CONTROL_VIEW_HPP
#define DJ_MIXER_CONTROL_VIEW_HPP

#include <QWidget>
#include <QHBoxLayout>

class QVBoxLayout;
class QToolButton;
class QProgressBar;
class QSpinBox;

class DJMixerControlView : public QWidget {
	Q_OBJECT
	public:
		DJMixerControlView(QWidget *parent = NULL);
		void reset();
	private:
		QToolButton * mLoadBtn;
		QToolButton * mResetBtn;
		QToolButton * mCueBtn;
		QToolButton * mSyncBtn;
		QToolButton * mPlayBtn;
		QToolButton * mSeekFwdBtn;
		QToolButton * mSeekBkwdBtn;
		QProgressBar * mProgressBar;
		QSpinBox * mBeatOffeset;

		QVBoxLayout * mLayout;
};

#endif

