#ifndef SWIG_MASTER_MODEL_HPP
#define SWIG_MASTER_MODEL_HPP


class MasterModel {
   public:
      MasterModel();
      unsigned int syncSource() const ;
      float tempo();
      float volume();
      void setVolume(float vol);
      void setTempo(float tempo);
      void setTempoMul(double mul);
      void setSyncSource(unsigned int src);
};

#endif

