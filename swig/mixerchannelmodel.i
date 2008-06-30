#ifndef SWIG_MIXER_CHANNEL_MODEL_HPP
#define SWIG_MIXER_CHANNEL_MODEL_HPP

class EQModel;

class MixerChannelModel {
   public:
      float volume() const;
      bool muted() const;
      EQModel * eq() const;
      void setVolume(float volume);
      void setMuted(bool muted = true);
};

#endif
