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
		//connect this model to another model of this same type
		void connectSignalsTo(MixerPanelModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		CrossFadeModel * crossFade() const;
		MasterModel * master() const;
		std::vector<DJMixerChannelModel *> * mixerChannels();
	signals:
		//from mixer
		void mixerEQValuesChanged(unsigned int index, float low, float mid, float high);
		void mixerVolumeChanged(unsigned int index, float vol);
		//from dj mixer control
		void mixerCueModeChanged(unsigned int index, bool cue);
		void mixerPausedChanged(unsigned int index, bool paused);
		void mixerSyncModeChanged(unsigned int index, bool free);
		void mixerSeek(unsigned int index, int amt);
		void mixerPlaybackPosChanged(unsigned int index, int pos);
		void mixerLoad(unsigned int index);

	//these protected slots are for relaying data from our members out to the above signals
	protected slots:
		//from mixer
		void setEqVal(QObject * ob);
		void setMixerVolume(QObject * ob);
		void setMixerMuted(QObject * ob);
		//from dj mixer control
		void setMixerCueMode(QObject * ob);
		void setMixerPaused(QObject * ob);
		void setMixerSyncMode(QObject * ob);
		void mixerSeekSlot(QObject * ob, int amt);
		void mixerSetPlaybackPos(QObject * ob);
		void mixerLoad(QObject * ob);

	public slots:
		void mixerUpdateProgress(unsigned int mixer, float progress);

	private:
		std::vector<DJMixerChannelModel *> mDJMixerChannels;
		CrossFadeModel * mXFade;
		MasterModel * mMaster;
		std::map<QObject *, unsigned int> mMixerObjectIndexMap;
};

#endif
