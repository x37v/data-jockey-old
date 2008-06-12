#ifndef DJ_MIXER_CONTROL_VIEW_HPP
#define DJ_MIXER_CONTROL_VIEW_HPP

#include <QWidget>
#include <QHBoxLayout>

class QVBoxLayout;
class QToolButton;
class QProgressBar;
class QSpinBox;
class QDoubleSpinBox;

class DJMixerControlView : public QWidget {
	Q_OBJECT
	public:
		DJMixerControlView(QWidget *parent = NULL);
		bool cueMode() const;
		bool paused() const;
		bool runningFree() const;
		int beatOffset() const;
	signals:
		void cueModeChanged(bool cueing);
		void pausedChanged(bool paused);
		void syncModeChanged(bool free);
		void seekFwdClicked(bool clicked);
		void seekBwdClicked(bool clicked);
		void loadClicked(bool clicked);
		void resetClicked(bool reset);
		void beatOffsetChanged(int offset);
	public slots:
		void setCueing(bool cueing = true);
		void setPaused(bool paused = true);
		void setPlaying(bool playing = true);
		void setRunFree(bool free = true); 
		void setSync(bool synced = true);
		//0..1
		void setProgress(float progress);
		//0..100
		void setProgress(int progress);
		void setBeatOffset(int offset);
	private:
		QToolButton * mLoadBtn;
		QToolButton * mResetBtn;
		QToolButton * mCueBtn;
		QToolButton * mSyncBtn;
		QToolButton * mPlayBtn;
		QToolButton * mSeekFwdBtn;
		QToolButton * mSeekBkwdBtn;
		QProgressBar * mProgressBar;
		QSpinBox * mBeatOffset;
		QDoubleSpinBox * mTempoMul;

		QVBoxLayout * mLayout;
};

#endif

