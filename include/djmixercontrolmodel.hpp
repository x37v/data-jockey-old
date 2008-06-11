#ifndef DJ_MIXER_CONTROL_MODEL_HPP
#define DJ_MIXER_CONTROL_MODEL_HPP

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
		float progress() const;
	public slots:
		void setCueing(bool cue = true);
		void setPaused(bool paused = true);
		void setPlay(bool play = true);
		void setSync(bool sync = true);
		void setRunFree(bool free = true);
		void setProgress(float progress);
		void setLoadProgress(float progress);
	signals:
		void cueModeChanged(bool cue);
		void pausedChanged(bool paused);
		void syncModeChanged(bool free);
		void progressChanged(float progress);
	private:
		float mProgress;
		bool mPaused;
		bool mSynced;
		bool mCueing;
};

#endif
