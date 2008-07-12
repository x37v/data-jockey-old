#ifndef DJ_MIXER_CONTROL_MODEL_HPP
#define DJ_MIXER_CONTROL_MODEL_HPP

#include <QObject>

class DJMixerControlModel : public QObject {
	Q_OBJECT
	public:
		DJMixerControlModel(QObject * parent = NULL);
		//sync this model's state to another model
		//signals which don't change the model's state only go from this model to the other, not back
		void syncToModel(DJMixerControlModel * other, Qt::ConnectionType connectionType = Qt::AutoCompatConnection);

		bool paused() const;
		bool playing() const;
		bool synced() const;
		bool runningFree() const;
		bool cueing() const;
		float progress() const;
		int beatOffset() const;
		double tempoMul() const;
	public slots:
		void setCueing(bool cue = true);
		void setPaused(bool paused = true);
		void setPlay(bool play = true);
		void setSync(bool sync = true);
		void setRunFree(bool free = true);
		void setProgress(float progress);
		void setPlaybackPosition(int pos);
		void seekFwd();
		void seekBkwd();
		void seek(int amt);
		void setBeatOffset(int offset);
		void setTempoMul(double mul);
		//update, doesn't emit
		void updateTempoMul(double mul);
		void resetWorkPosition();
		void loadWork();
		//this resets the settings.. beat offset, etc.
		void reset();
	signals:
		void cueModeChanged(bool cue, QObject *me);
		void pausedChanged(bool paused, QObject *me);
		void syncModeChanged(bool sync, QObject *me);
		void progressChanged(float progress, QObject *me);
		void playbackPositionChanged(int pos, QObject * me);
		void beatOffsetChanged(int offset, QObject *me);
		void tempoMulChanged(double mul, QObject *me);
		void load(QObject *me);
		void seeking(int amt, QObject *me);
	private:
		float mProgress;
		bool mPaused;
		bool mSynced;
		bool mCueing;
		int mBeatOffset;
		double mTempoMul;
};

#endif
