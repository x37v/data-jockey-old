#ifndef DJ_MIXER_CHANNEL_VIEW_HPP
#define DJ_MIXER_CHANNEL_VIEW_HPP

#include <QWidget>
#include "mixerchannelview.hpp"

class QVBoxLayout;
class QHBoxLayout;
class QToolButton;
class QProgressBar;
class QSpinBox;

class DJMixerChannelView : public QWidget {
	Q_OBJECT
	public:
		DJMixerChannelView(QWidget *parent = NULL);
		MixerChannelView * mixerChannel();
		void reset();
	private:
		MixerChannelView * mMixerChannel;
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

