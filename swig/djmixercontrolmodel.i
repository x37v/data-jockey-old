#ifndef SWIG_DJ_MIXER_CONTROL_MODEL_HPP
#define SWIG_DJ_MIXER_CONTROL_MODEL_HPP

class DJMixerControlModel {
   public:
      DJMixerControlModel(QObject * parent = NULL);
      bool paused() const;
      bool playing() const;
      bool synced() const;
      bool runningFree() const;
      bool cueing() const;
      float progress() const;
      int beatOffset() const;
      double tempoMul() const;
      void setCueing(bool cue = true);
      void setPaused(bool paused = true);
      void setPlay(bool play = true);
      void setSync(bool sync = true);
      void setRunFree(bool free = true);
      void setPlaybackPosition(int pos);
      void seekFwd();
      void seekBkwd();
      void seek(int amt);
      void setBeatOffset(int offset);
      void setTempoMul(double mul);
      void resetWorkPosition();
};

#endif
