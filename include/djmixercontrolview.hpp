/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DJ_MIXER_CONTROL_VIEW_HPP
#define DJ_MIXER_CONTROL_VIEW_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QPushButton>

class QVBoxLayout;
class QPushButton;
class QProgressBar;
class QDoubleSpinBox;

class PowerOfTwoSpinBox : public QSpinBox {
	Q_OBJECT
	public:
		PowerOfTwoSpinBox(QWidget * parent = NULL);
		virtual QString textFromValue(int val) const;
	signals:
		void valueChanged(double);
	public slots:
		void setValue(double val);
	protected slots:
		void setValue(int val);
};

class SeekFwdButton : public QPushButton {
	Q_OBJECT
	public:
		SeekFwdButton(QWidget * parent) : QPushButton(parent){
			setAutoRepeat(true);
			setToolTip(tr("seek forward"));
		}
};

class SeekBkwdButton : public QPushButton {
	Q_OBJECT
	public:
		SeekBkwdButton(QWidget * parent) : QPushButton(parent){
			setAutoRepeat(true);
			setToolTip(tr("seek backward"));
		}
};

class LoadButton : public QPushButton {
	Q_OBJECT
	public:
		LoadButton(QWidget * parent) : QPushButton(parent){
			setToolTip(tr("load selected file"));
		}
};

class ResetButton : public QPushButton {
	Q_OBJECT
	public:
		ResetButton(QWidget * parent) : QPushButton(parent){
			setToolTip(tr("reset playback position"));
		}
};

class CueButton : public QPushButton {
	Q_OBJECT
	public:
		CueButton(QWidget * parent) : QPushButton(parent){
			setToolTip(tr("cue (toggle)"));
			setCheckable(true);
		}
};

class SyncButton : public QPushButton {
	Q_OBJECT
	public:
		SyncButton(QWidget * parent) : QPushButton(parent){
			setToolTip(tr("sync/free playback (toggle)"));
			setCheckable(true);
		}
};

class PlayButton : public QPushButton {
	Q_OBJECT
	public:
		PlayButton(QWidget * parent) : QPushButton(parent){
			setToolTip(tr("play/pause (toggle)"));
			setCheckable(true);
		}
};


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
		void seekingFwd();
		void seekingBwd();
		void loadClicked();
		void resetClicked();
		void beatOffsetChanged(int offset);
		void tempoMulChanged(double mul);
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
		void setTempoMul(double mul);
	protected slots:
		void changeTempoMulStep(bool sync);
		void tempoMulFreeChanged(double mul);
		void tempoMulSyncedChanged(int mul);
	private:
		LoadButton * mLoadBtn;
		ResetButton * mResetBtn;
		QPushButton * mCueBtn;
		QPushButton * mSyncBtn;
		QPushButton * mPlayBtn;
		SeekFwdButton * mSeekFwdBtn;
		SeekBkwdButton * mSeekBkwdBtn;
		QProgressBar * mProgressBar;
		QSpinBox * mBeatOffset;
		QDoubleSpinBox * mTempoMulFree;
		PowerOfTwoSpinBox * mTempoMulSynced;

		QVBoxLayout * mLayout;
};

#endif

