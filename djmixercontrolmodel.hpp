#ifndef DJ_MIXER_CONTROL_VIEW_HPP
#define DJ_MIXER_CONTROL_VIEW_HPP

#include <QObject>

class DJMixerControlModel : public QObject {
	Q_OBJECT
	public:
		DJMixerControlModel(QObject * parent = NULL);
		bool paused() const;
		bool playing() const;
		bool synced() const;
		bool runningFree() const;
		bool cueing() const;
	public slots:
		void setCueing(bool cue = true);
		void setPaused(bool paused = true);
		void setPlay(bool play = true);
		void setSync(bool sync = true);
		void setRunFree(bool free = true);
	signals:
		void cueModeChanged(bool cue);
		void pausedChanged(bool paused);
		void runningFreeChanged(bool free);
	private:
		float mProgress;
		bool mPaused;
		bool mSynced;
		bool mCueing;
};

#endif
