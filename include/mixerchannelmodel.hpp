#ifndef MIXER_CHANNEL_MODEL_HPP
#define MIXER_CHANNEL_MODEL_HPP

#include <QObject>

class EQModel;

class MixerChannelModel : public QObject {
	Q_OBJECT
	public:
		MixerChannelModel(QObject *parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(MixerChannelModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		float volume() const;
		bool muted() const;
		EQModel * eq() const;
	signals:
		void volumeChanged(float volume, QObject *me);
		void mutedChanged(bool muted, QObject *me);
	public slots:
		void setVolume(float volume);
		void setMuted(bool muted = true);
	protected slots:
		void updateVolume(float volume);
	private:
		float mVolume;
		bool mMuted;
		EQModel * mEQ;
};

#endif
