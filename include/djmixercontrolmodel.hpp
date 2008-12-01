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
