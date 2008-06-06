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
		float volume() const;
		bool muted() const;
	signals:
		void volumeChanged(float volume);
		void mutedChanged(bool muted);
	public slots:
		void setVolume(float volume);
		void setVolume(int volume);
		void setMuted(bool muted = true);
	private:
		EQView * mEQView;
		QSlider * mVolumeSlider;
		QVBoxLayout * mLayout;
		QToolButton * mMuteBtn;
		//to prevent infinite loops
		bool mRecursing;
};

#endif

