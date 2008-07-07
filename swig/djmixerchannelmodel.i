#ifndef SWIG_DJ_MIXER_CHANNEL_MODEL_HPP
#define SWIG_DJ_MIXER_CHANNEL_MODEL_HPP

%rename(control) DJMixerChannelModel::DJMixerControl;

class DJMixerChannelModel {
   public:
      DJMixerChannelModel(QObject * parent = NULL);
      DJMixerControlModel * DJMixerControl() const;
      int work() const;
      void loadWork(int work);
      void reset();
      float volume() const;
      bool muted() const;
      EQModel * eq() const;
      void setVolume(float volume);
      void setMuted(bool muted = true);
};


#endif
