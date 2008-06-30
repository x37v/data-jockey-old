#ifndef SWIG_DJ_MIXER_CHANNEL_MODEL_HPP
#define SWIG_DJ_MIXER_CHANNEL_MODEL_HPP

class DJMixerChannelModel {
   public:
      DJMixerChannelModel(QObject * parent = NULL);
      DJMixerControlModel * DJMixerControl() const;
      MixerChannelModel * mixerChannel() const;
      int work() const;
      void loadWork(int work);
      void reset();
};

#endif
