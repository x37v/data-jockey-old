#ifndef MIXER_CHANNEL_MODEL_HPP
#define MIXER_CHANNEL_MODEL_HPP

#include <QObject>

class MixerChannelModel : public QObject {
	Q_OBJECT
	public:
		MixerChannelModel(QObject *parent = NULL);
		double volume() const;
		bool muted() const;
	signals:
		void volumeChanged(double volume);
		void mutedChanged(bool muted);
	public slots:
		void setVolume(double volume);
		void setMuted(bool muted = true);
	private:
		double mVolume;
		bool mMuted;
};

#endif
