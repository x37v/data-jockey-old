#ifndef CROSS_FADE_MODEL_HPP
#define CROSS_FADE_MODEL_HPP

#include <QObject>

class CrossFadeModel : public QObject {
	Q_OBJECT
	public:
		CrossFadeModel(unsigned int numMixers, QObject * parent = NULL);
		//connect this model to another model of this same type
		void connectSignalsTo(CrossFadeModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);
	signals:
		void mixersChanged(unsigned int left, unsigned int right);
		void disabled();
	public slots:
		void setMixers(unsigned int left, unsigned int right);
		void disable();
	private:
		unsigned int mNumMixers;
		unsigned int mLeft;
		unsigned int mRight;
		bool mEnabled;
};

#endif

