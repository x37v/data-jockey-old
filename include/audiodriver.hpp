#ifndef AUDIO_DRIVER_HPP
#define AUDIO_DRIVER_HPP

#include <QObject>
#include "audioio.hpp"

class AudioDriver : public QObject {
	Q_OBJECT
	public:
		AudioDriver(unsigned int nMixers = 4, QObject * parent = NULL);
	public slots:
		//master
		void masterSetVolume(float vol, bool wait_for_measure = false);
		void masterSetTempo(float tempo, bool wait_for_measure = false);
		void masterSetTempoMul(float mul, bool wait_for_measure = false);
		void masterSetSyncSrc(unsigned int src, bool wait_for_measure = false);

		//mixers
		void mixerSetCue(unsigned int mixer, bool cue = true, bool wait_for_measure = false);
		void mixerSetSync(unsigned int mixer, bool sync = true, bool wait_for_measure = false);
		void mixerSetFree(unsigned int mixer, bool free = true, bool wait_for_measure = false);
		void mixerSetBeatOffset(unsigned int mixer, unsigned int offset, bool wait_for_measure = false);
		void mixerSetPause(unsigned int mixer, bool pause = true, bool wait_for_measure = false);
		void mixerSetPlay(unsigned int mixer, bool play = true, bool wait_for_measure = false);
		void mixerSetWork(unsigned int mixer, unsigned int work, bool wait_for_measure = false);
		void mixerSetPlaybackPosition(unsigned int mixer, unsigned int pos, bool wait_for_measure = false);
		void mixerSeek(unsigned int mixer, int beats, bool wait_for_measure = false);
		void mixerSetLoopPoints(unsigned int mixer, unsigned int start, unsigned int end, bool wait_for_measure = false);
		void mixerSetLooping(unsigned int mixer, bool loop, bool wait_for_measure = false);

		void mixerSetVolume(unsigned int mixer, float vol, bool wait_for_measure = false);
		void mixerSetEQHigh(unsigned int mixer, float val, bool wait_for_measure = false);
		void mixerSetEQMid(unsigned int mixer, float val, bool wait_for_measure = false);
		void mixerSetEQLow(unsigned int mixer, float val, bool wait_for_measure = false);

	private:
		DataJockey::AudioIO mAudioIO;
};

#endif

