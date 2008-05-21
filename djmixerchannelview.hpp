#ifndef DJ_MIXER_CHANNEL_VIEW_HPP
#define DJ_MIXER_CHANNEL_VIEW_HPP

#include <QWidget>
#include "mixerchannelview.hpp"

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;

class DJMixerChannelView : public QWidget {
	Q_OBJECT
	public:
		DJMixerChannelView(QWidget *parent = NULL);
		MixerChannelView * mixerChannel();
		void reset();
	private:
		MixerChannelView * mMixerChannel;

		QPushButton * mPlayBtn;
		QPushButton * mCueBtn;
		QPushButton * mLoadBtn;
		QPushButton * mResetBtn;

		QPushButton * mSeekFwdBtn;
		QPushButton * mSeekBkwdBtn;

		QVBoxLayout * mLayout;
};


#endif

