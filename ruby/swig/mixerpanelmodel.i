#ifndef SWIG_MIXER_PANEL_MODEL_HPP
#define SWIG_MIXER_PANEL_MODEL_HPP

//#include <vector>
//
//class DJMixerChannelModel;
//class MasterModel;
//class CrossFadeModel;

%rename(mixer) MixerPanelModel::mixerChannel;
%rename(numMixers) MixerPanelModel::numMixerChannels;

class MixerPanelModel {
   public:
      MixerPanelModel(unsigned int numMixers = 4);
      CrossFadeModel * crossFade() const;
      MasterModel * master() const;
      DJMixerChannelModel * mixerChannel(unsigned int i) const;
      unsigned int numMixerChannels() const;
      bool mixerAudible(unsigned int index) const;
      float mixerVolume(unsigned int index) const;
};

#endif
