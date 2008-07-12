#ifndef MIXER_PANEL_MODEL_HPP
#define MIXER_PANEL_MODEL_HPP

#include <QObject>
#include <vector>
#include <map>

class DJMixerChannelModel;
class MasterModel;
class CrossFadeModel;
class EQModel;

class MixerPanelModel : public QObject {
	Q_OBJECT
	public:
		MixerPanelModel(unsigned int numMixers = 4, QObject *parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(MixerPanelModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		CrossFadeModel * crossFade() const;
		MasterModel * master() const;
		std::vector<DJMixerChannelModel *> * mixerChannels();
		DJMixerChannelModel * mixerChannel(unsigned int i) const;
		unsigned int numMixerChannels() const;
		bool mixerAudible(unsigned int index) const;
		float mixerVolume(unsigned int index) const;
	signals:
		//from mixer
		void mixerEQValuesChanged(unsigned int mixer_index, float low, float mid, float high);
		void mixerVolumeChanged(unsigned int mixer_index, float vol);
		//from dj mixer control
		void mixerCueModeChanged(unsigned int mixer_index, bool cue);
		void mixerPausedChanged(unsigned int mixer_index, bool paused);
		void mixerSyncModeChanged(unsigned int mixer_index, bool free);
		void mixerTempoMulChanged(unsigned int mixer_index, double mul);
		void mixerSeeking(unsigned int mixer_index, int amt);
		void mixerPlaybackPosChanged(unsigned int mixer_index, int pos);
		void mixerLoading(unsigned int mixer_index, int work_id);

	//these protected slots are for relaying data from our members out to the above signals
	protected slots:
		//from mixer
		void setEqVal(QObject * ob);
		void setMixerVolume(float vol, QObject * ob);
		void setMixerMute(bool mute, QObject * ob);
		//from dj mixer control
		void setMixerCueMode(bool cue, QObject * ob);
		void setMixerPaused(bool paused, QObject * ob);
		void setMixerSyncMode(bool sync, QObject * ob);
		void setMixerTempoMul(double mul, QObject * ob);
		void mixerSeek(int amt, QObject * ob);
		void mixerSetPlaybackPos(int pos, QObject * ob);
		void mixerLoad(int work_id, QObject * ob);

		void crossFadeUpdate();

	public slots:
		void mixerUpdateProgress(unsigned int mixer, float progress);
		void mixerSetTempoMul(unsigned int mixer, double mul);

	private:
		void setMixerVolume(unsigned int index);
		std::vector<DJMixerChannelModel *> mDJMixerChannels;
		CrossFadeModel * mXFade;
		MasterModel * mMaster;
		std::map<QObject *, unsigned int> mMixerObjectIndexMap;
};

#endif
