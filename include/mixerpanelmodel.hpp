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
		CrossFadeModel * crossFade() const;
		MasterModel * master() const;
		std::vector<DJMixerChannelModel *> * mixerChannels();
	signals:
		void mixerEQValuesChanged(unsigned int index, float low, float mid, float high);
		void mixerVolumeChanged(unsigned int index, float vol);
		//void mixerMutedChanged(unsigned int index, bool muted);
	//these protected slots are for relaying data from our members out to the above signals
	protected slots:
		void setEqVal(QObject * ob);
		void setMixerVolume(QObject * ob);
		void setMixerMuted(QObject * ob);
	private:
		std::vector<DJMixerChannelModel *> mDJMixerChannels;
		CrossFadeModel * mXFade;
		MasterModel * mMaster;
		std::map<QObject *, unsigned int> mMixerObjectIndexMap;
};

#endif
