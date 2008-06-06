#ifndef MIXER_CHANNEL_MODEL_HPP
#define MIXER_CHANNEL_MODEL_HPP

#include <QObject>

class EQModel;

class MixerChannelModel : public QObject {
	Q_OBJECT
	public:
		MixerChannelModel(QObject *parent = NULL);
		float volume() const;
		bool muted() const;
		EQModel * eq();
	signals:
		void volumeChanged(float volume);
		void mutedChanged(bool muted);
	public slots:
		void setVolume(float volume);
		void setMuted(bool muted = true);
	private:
		float mVolume;
		bool mMuted;
		EQModel * mEQ;
};

#endif