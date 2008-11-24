#ifndef MIXER_CHANNEL_VIEW_HPP
#define MIXER_CHANNEL_VIEW_HPP

#include <QVBoxLayout>
#include <QSlider>
#include <QPushButton>
class EQView;

class MuteButton : public QPushButton {
	Q_OBJECT
	public:
		MuteButton(QWidget * parent) : QPushButton(parent){
			setToolTip("mute (toggle)");
			setCheckable(true);
		}
};

class MixerChannelView : public QObject {
	Q_OBJECT
	public:
		MixerChannelView(QWidget *parent = NULL);
		EQView * eq();
		QSlider * volumeSlider();
		MuteButton * muteButton();
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
		MuteButton * mMuteBtn;
		//to prevent infinite loops
		bool mRecursing;
};

#endif

