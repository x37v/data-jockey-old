#ifndef SWIG_APPLICATION_MODEL_HPP
#define SWIG_APPLICATION_MODEL_HPP

class MixerPanelModel;

class ApplicationModelProxy {
   public:
      ApplicationModelProxy(Qt::ConnectionType type = Qt::QueuedConnection, QObject * parent = ApplicationModel::instance());
      MixerPanelModel * mixerPanel() const;
};

#endif

