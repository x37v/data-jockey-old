#ifndef SWIG_CROSS_FADE_MODEL_HPP
#define SWIG_CROSS_FADE_MODEL_HPP

class CrossFadeModel {
   public:
      float position() const;
      float valueLeft() const;
      float valueRight() const;
      bool enabled() const;
      unsigned int mixerLeft() const;
      unsigned int mixerRight() const;
      float scaleVolume(unsigned int index, float mixer_volume);
      void setMixers(unsigned int left, unsigned int right);
      void disable();
      void setPosition(float val);
};

#endif

