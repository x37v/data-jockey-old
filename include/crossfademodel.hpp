#ifndef CROSS_FADE_MODEL_HPP
#define CROSS_FADE_MODEL_HPP

#include <QObject>

class CrossFadeModel : public QObject {
	Q_OBJECT
	public:
		CrossFadeModel(unsigned int numMixers, QObject * parent = NULL);
		CrossFadeModel(QObject * parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(CrossFadeModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);
		float valueLeft() const;
		float valueRight() const;
		bool enabled() const;
		unsigned int mixerLeft() const;
		unsigned int mixerRight() const;
		float scaleVolume(unsigned int index, float mixer_volume);
	signals:
		void mixersChanged(unsigned int left, unsigned int right);
		void disabled();
		void positionChanged(float val);
      void leftMixerChanged(unsigned int);
      void rightMixerChanged(unsigned int);
	public slots:
      void setLeftMixer(unsigned int left);
      void setRightMixer(unsigned int right);
		void setMixers(unsigned int left, unsigned int right);
		void disable();
		void setPosition(float val);
	private:
		bool mRecursing;
		float mPosition;
		unsigned int mNumMixers;
		unsigned int mLeft;
		unsigned int mRight;
		bool mEnabled;
};

#endif

