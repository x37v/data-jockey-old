#ifndef AUDIO_DRIVER_HPP
#define AUDIO_DRIVER_HPP

#include "audioio.hpp"
#include <QObject>

class MixerPanelModel;

class AudioDriver : public QObject {
	Q_OBJECT
	public:
		AudioDriver(MixerPanelModel * mixerModel, QObject * parent = NULL);
	signals:
		void progressChanged(unsigned int mixer, float value);
		void tempoChanged(float value);
		void temoMulChanged(float value);
	protected:
		void run();
	public slots:
		void start();
		void stop();

		void processAudioEvents();

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
		void mixerLoad(unsigned int mixer, QString audiobufloc, QString beatbufloc, bool wait_for_measure = false);
		void mixerLoad(unsigned int mixer, 
				DataJockey::AudioBufferPtr audio_buffer, 
				DataJockey::BeatBufferPtr beat_buffer, bool wait_for_measure = false);
		void mixerSetPlaybackPosition(unsigned int mixer, int pos, bool wait_for_measure = false);
		void mixerSeek(unsigned int mixer, int beats, bool wait_for_measure = false);
		void mixerSetLoopPoints(unsigned int mixer, unsigned int start, unsigned int end, bool wait_for_measure = false);
		void mixerSetLooping(unsigned int mixer, bool loop, bool wait_for_measure = false);
		void mixerSetVolume(unsigned int mixer, float vol, bool wait_for_measure = false);
		void mixerSetEQVals(unsigned int mixer, float low, float mid, float high, bool wait_for_measure = false);
	protected slots:
		void mixerReportLoadProgress(unsigned int mixer, float progress);

	private:
		bool mSyncToMaster;
		DataJockey::AudioIO mAudioIO;
		MixerPanelModel * mMixerPanel;
		unsigned int mNumMixers;
};

#endif

