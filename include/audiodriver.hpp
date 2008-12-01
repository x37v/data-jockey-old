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

#ifndef AUDIO_DRIVER_HPP
#define AUDIO_DRIVER_HPP

#include "audioio.hpp"
#include <QObject>
#include <QThread>

class MixerPanelModel;
class AudioDriverThread;

class AudioDriver : public QObject {
	Q_OBJECT
	public:
		AudioDriver(MixerPanelModel * mixerModel, QObject * parent = NULL);
		DataJockey::AudioIO * audioIO() const;
	signals:
		void progressChanged(unsigned int mixer, float value);
		void mixerTempoMulChanged(unsigned int mixer, double value);
		void tempoChanged(float value);
		void tempoMulChanged(double value);
	protected:
		void run();
	public slots:
		void start();
		void stop();

		void processAudioEvents();

		//master
		void masterSetVolume(float vol, bool wait_for_measure = false);
		void masterSetTempo(float tempo, bool wait_for_measure = false);
		void masterSetTempoMul(double mul, bool wait_for_measure = false);
		void masterSetSyncSrc(unsigned int src, bool wait_for_measure = false);

		//mixers
		void mixerSetCue(unsigned int mixer, bool cue = true, bool wait_for_measure = false);
		void mixerSetSync(unsigned int mixer, bool sync = true, bool wait_for_measure = false);
		void mixerSetTempoMul(unsigned int mixer, double mul, bool wait_for_measure = false);
		void mixerSetFree(unsigned int mixer, bool free = true, bool wait_for_measure = false);
		void mixerSetBeatOffset(unsigned int mixer, unsigned int offset, bool wait_for_measure = false);
		void mixerSetPause(unsigned int mixer, bool pause = true, bool wait_for_measure = false);
		void mixerSetPlay(unsigned int mixer, bool play = true, bool wait_for_measure = false);
		void mixerLoad(unsigned int mixer, 
				DataJockey::AudioBufferPtr audio_buffer, 
				DataJockey::BeatBufferPtr beat_buffer, bool wait_for_measure = false);
		void mixerSetPlaybackPosition(unsigned int mixer, int pos, bool wait_for_measure = false);
		void mixerSeek(unsigned int mixer, int beats, bool wait_for_measure = false);
		void mixerSetLoopPoints(unsigned int mixer, unsigned int start, unsigned int end, bool wait_for_measure = false);
		void mixerSetLooping(unsigned int mixer, bool loop, bool wait_for_measure = false);
		void mixerSetVolume(unsigned int mixer, float vol, bool wait_for_measure = false);
		void mixerSetEQVals(unsigned int mixer, float low, float mid, float high, bool wait_for_measure = false);

	private:
		void connectToMixerPanel();
		double mTempoMul;
		bool mSyncToMaster;
		DataJockey::AudioIO mAudioIO;
		MixerPanelModel * mMixerPanel;
		unsigned int mNumMixers;
		bool mReportTempoMul;
		std::vector<bool> mReportPlayerTempoMul;
};

class AudioDriverThread : public QThread {
	Q_OBJECT
	public:
		AudioDriverThread(QObject * parent = NULL);
		void setAudioDriver(AudioDriver * driver);
		void run();
	public slots:
		void stop();
	private:
		AudioDriver * mDriver;
};

#endif

