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
		void resetWorkPosition();
		void loadWork();
		//this resets the settings.. beat offset, etc.
		void reset();
	signals:
		void cueModeChanged(bool cue);
		void pausedChanged(bool paused);
		void syncModeChanged(bool sync);
		void progressChanged(float progress);
		void playbackPositionChanged(int pos);
		void beatOffsetChanged(int offset);
		void tempoMulChanged(double mul);
		void load();
		void seeking(int amt);

		void cueModeChanged(QObject * ob);
		void pausedChanged(QObject * ob);
		void syncModeChanged(QObject * ob);
		void seeking(QObject * ob, int amt);
		void playbackPositionChanged(QObject * ob);
		void load(QObject * ob);
		void tempoMulChanged(QObject * ob);
	private:
		float mProgress;
		bool mPaused;
		bool mSynced;
		bool mCueing;
		int mBeatOffset;
		double mTempoMul;
};

#endif
