#ifndef SWIG_EQ_MODEL_HPP
#define SWIG_EQ_MODEL_HPP


class EQModel {
   public:
      EQModel(QObject *parent = NULL);
      float high();
      float mid();
      float low();
      void reset();
      void setHigh(float val);
      void setMid(float val);
      void setLow(float val);
      void toggleHighCut();
      void toggleMidCut();
      void toggleLowCut();
};

#endif

