#ifndef APPLICATION_HPP
#define APPLICATION_HPP

class MixerPanelView;
class MixerPanelModel;
class AudioDriver;

#include <QObject>

class MixerProxyObject : public QObject {
	Q_OBJECT
	public:
		MixerProxyObject(unsigned int num, QObject * parent = NULL);
	signals:
		void cueModeChanged(unsigned int mixer, bool cue, bool wait_for_measure);
		void syncModeChanged(unsigned int mixer, bool sync, bool wait_for_measure);
		void beatOffsetChanged(unsigned int mixer, unsigned int offset, bool wait_for_measure);
		void pausedChanged(unsigned int mixer, bool pause, bool wait_for_measure);
		void workChanged(unsigned int mixer, std::string audiobufloc, std::string beatbufloc, bool wait_for_measure);
		void playbackPosChanged(unsigned int mixer, unsigned int pos, bool wait_for_measure);
		void seekSig(unsigned int mixer, int beats, bool wait_for_measure);
		void loopPointsChanged(unsigned int mixer, unsigned int start, unsigned int end, bool wait_for_measure);
		void loop(unsigned int mixer, bool loop, bool wait_for_measure);
		void volumeChanged(unsigned int mixer, float vol, bool wait_for_measure);
		void eqChanged(unsigned int mixer, float low, float mid, float high, bool wait_for_measure);
		void progressChanged(float progress);
	public slots:
		void setCue(bool cue = true, bool wait_for_measure = false);
		void setFree(bool free = true, bool wait_for_measure = false);
		void setBeatOffset(unsigned int offset, bool wait_for_measure = false);
		void setPause(bool pause = true, bool wait_for_measure = false);
		void setWork(std::string audiobufloc, std::string beatbufloc, bool wait_for_measure = false);
		void setPlaybackPosition(unsigned int pos, bool wait_for_measure = false);
		void seek(int beats, bool wait_for_measure = false);
		void setLoopPoints(unsigned int start, unsigned int end, bool wait_for_measure = false);
		void setLooping(bool loop, bool wait_for_measure = false);
		void setVolume(float vol, bool wait_for_measure = false);
		void setEQVals(float low, float mid, float high, bool wait_for_measure = false);
		void setProgress(unsigned int mixer, float value);
	private:
		unsigned int mNum;
};

class DataJockeyApplication {
	public:
		static int run(int argc, char *argv[]);
		static void connectMixerPanelModelView(MixerPanelModel * model, MixerPanelView * view);
		static void connectMixerPanelModelDriver(MixerPanelModel * model, AudioDriver * driver);
};

#endif

