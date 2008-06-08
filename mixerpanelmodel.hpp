#ifndef MIXER_PANEL_MODEL_HPP
#define MIXER_PANEL_MODEL_HPP

#include <QObject>
#include <vector>

class DJMixerChannelModel;
class MasterModel;
class CrossFadeModel;

class MixerPanelModel : public QObject {
	Q_OBJECT
	public:
		MixerPanelModel(unsigned int numMixers = 4, QObject *parent = NULL);
		CrossFadeModel * crossFade();
		MasterModel * master();
		std::vector<DJMixerChannelModel *> * mixerChannels();
	private:
		std::vector<DJMixerChannelModel *> mDJMixerChannels;
		CrossFadeModel * mXFade;
		MasterModel * mMaster;
};

#endif
