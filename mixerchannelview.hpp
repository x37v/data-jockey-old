#ifndef MIXER_CHANNEL_VIEW_HPP
#define MIXER_CHANNEL_VIEW_HPP

#include <QVBoxLayout>
#include <QSlider>
#include <QToolButton>
class EQView;

class MixerChannelView : public QWidget {
	Q_OBJECT
	public:
		MixerChannelView(QWidget *parent = NULL);
		EQView * eq();
		QSlider * volumeSlider();
		QToolButton * muteButton();
		void reset();
		double volume() const;
		bool muted() const;
	signals:
		void volumeChanged(double volume);
		void mutedChanged(bool muted);
	public slots:
		void setVolume(double volume);
		void setVolume(int volume);
		void setMuted(bool muted = true);
	private:
		bool mRecursing;
		EQView * mEQView;
		QSlider * mVolumeSlider;
		QVBoxLayout * mLayout;
		QToolButton * mMuteBtn;
};

#endif

