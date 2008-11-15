#include "audiodriver.hpp"

//models
#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include "mastermodel.hpp"
#include "mixerpanelmodel.hpp"

#include <sys/mman.h>

#include <QTimer>

using namespace DataJockey;

AudioDriver::AudioDriver(MixerPanelModel * mixer, QObject * parent) :
	QObject(parent), mAudioIO(mixer->mixerChannels()->size())
{
	//unlock memory because we use a lot of it!
	munlockall();

	mMixerPanel = mixer;
	mNumMixers = mMixerPanel->mixerChannels()->size();
	mSyncToMaster = true;
	mTempoMul = 1.0;
	//we only report the tempo mul when we've changed sync states
	//before a user sets the tempo mul
	mReportTempoMul = true;
	for(unsigned int i = 0; i < mNumMixers; i++)
		mReportPlayerTempoMul.push_back(true);

	//connect
	connectToMixerPanel();
}

DataJockey::AudioIO * AudioDriver::audioIO() const {
	return const_cast<DataJockey::AudioIO *>(&mAudioIO);
}

void AudioDriver::start(){
	mAudioIO.start();
	//push a new state request on there
	AudioIOGetStatePtr stateReq = new AudioIOGetState(mNumMixers);
	mAudioIO.sendCommand(stateReq);
}

void AudioDriver::stop(){
	mAudioIO.stop();
}

void AudioDriver::processAudioEvents(){
	AudioIOGetStatePtr state = mAudioIO.consume();
	if(state){
		//deal with buffer player states
		std::vector<BufferPlayer::GetStatePtr> bufferPlayerStates = 
			state->getBufferPlayerStates();
		for(unsigned int i = 0; i < bufferPlayerStates.size(); i++){
			float progress;
			if(bufferPlayerStates[i]->getAudioLength() < 1)
				progress = 0.0;
			else {
				progress = (float)(bufferPlayerStates[i]->getSampleIndex() / 
						bufferPlayerStates[i]->getAudioLength());
				if(progress > 1.0)
					progress = 1.0;
			}
			emit(progressChanged(i, progress));
			//report the mixer's tempo mul if we should
			if(i < mNumMixers && mReportPlayerTempoMul[i]){
				double mixerTempoMul = bufferPlayerStates[i]->getTempoMultiplier();
				emit(mixerTempoMulChanged(i, mixerTempoMul));
			}
		}

		//deal with tempo changes
		//it only changes when we're in sync with a buffer
		if(!state->getSyncToClock()){
			float tempo = 60.0f / state->getPeriod();
			emit(tempoChanged(tempo));
		}

		if(state->getTempoScale() != mTempoMul && mReportTempoMul){
			mTempoMul = state->getTempoScale();
			emit(tempoMulChanged(mTempoMul));
		}

		//push a new state request on there
		state = new AudioIOGetState(mNumMixers);
		mAudioIO.sendCommand(state);
	}
}

void AudioDriver::masterSetVolume(float vol, bool wait_for_measure){
	AudioIOSetVolumePtr cmd = new AudioIOSetVolume(vol, wait_for_measure);
	mAudioIO.sendCommand(cmd);
}

void AudioDriver::masterSetTempo(float tempo, bool wait_for_measure){
	//only send if we're syncing to the master
	if(mSyncToMaster){
		AudioIOSetBPMPtr cmd = new AudioIOSetBPM(tempo, wait_for_measure);
		mAudioIO.sendCommand(cmd);
	}
}

void AudioDriver::masterSetTempoMul(double mul, bool wait_for_measure){
	if(!mSyncToMaster){
		//since the user has set the tempo mul then we'll no longer send reports about it
		mReportTempoMul = false;
		AudioIOSetTempoScalePtr cmd = new AudioIOSetTempoScale(mul, wait_for_measure);
		mTempoMul = mul;
		mAudioIO.sendCommand(cmd);
	}

}

void AudioDriver::masterSetSyncSrc(unsigned int src, bool wait_for_measure){
	mReportTempoMul = true;
	if(src == 0){
		AudioIOSyncToMasterClockPtr cmd = new AudioIOSyncToMasterClock(wait_for_measure);
		mSyncToMaster = true;
		mAudioIO.sendCommand(cmd);
	} else {
		mSyncToMaster = false;
		AudioIOSyncToBufferPlayerPtr cmd = new AudioIOSyncToBufferPlayer(src - 1, wait_for_measure);
		mAudioIO.sendCommand(cmd);
	}

}

void AudioDriver::mixerSetCue(unsigned int mixer, bool cue, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd;
	if(cue)
		cmd = new BufferPlayer::SetOutPort(BufferPlayer::cueOut);
	else
		cmd = new BufferPlayer::SetOutPort(BufferPlayer::mainOut);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSetSync(unsigned int mixer, bool sync, bool wait_for_measure){
	//after changing the sync mode we report the tempo mul
	if(mixer < mNumMixers)
		mReportPlayerTempoMul[mixer] = true;
	BufferPlayer::CmdPtr cmd;
	if(sync)
		cmd = new BufferPlayer::SetPlayMode(BufferPlayer::syncPlayback);
	else
		cmd = new BufferPlayer::SetPlayMode(BufferPlayer::freePlayback);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSetTempoMul(unsigned int mixer, double mul, bool wait_for_measure){
	//once the user sets the tempo mul we don't report it
	if(mixer < mNumMixers)
		mReportPlayerTempoMul[mixer] = false;
	BufferPlayer::CmdPtr cmd= new BufferPlayer::SetTempoMultiplier(mul);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}


void AudioDriver::mixerSetFree(unsigned int mixer, bool free, bool wait_for_measure){
	mixerSetSync(mixer, !free, wait_for_measure);
}

void AudioDriver::mixerSetBeatOffset(unsigned int mixer, unsigned int offset, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetBeatOffset((float)offset);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSetPause(unsigned int mixer, bool pause, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetPlaying(!pause);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}


void AudioDriver::mixerSetPlay(unsigned int mixer, bool play, bool wait_for_measure){
	mixerSetPause(mixer, !play, wait_for_measure);
}

void AudioDriver::mixerLoad(unsigned int mixer, 
		DataJockey::AudioBufferPtr audio_buffer, 
		DataJockey::BeatBufferPtr beat_buffer, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetBuffers(audio_buffer, beat_buffer);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSetPlaybackPosition(unsigned int mixer, int pos, bool wait_for_measure){
	//XXX change this to allow for negative pos so that we can have lead in time
	if(pos < 0)
		pos = 0;
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetPlaybackPosition((unsigned int)pos);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSeek(unsigned int mixer, int beats, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd = new BufferPlayer::IncPlaybackPosition(beats);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSetLoopPoints(unsigned int mixer, unsigned int start, unsigned int end, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetLoopPoints(start,end);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}
void AudioDriver::mixerSetLooping(unsigned int mixer, bool loop, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetLooping(loop);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSetVolume(unsigned int mixer, float vol, bool wait_for_measure){
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetVolume(vol);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::mixerSetEQVals(unsigned int mixer, float low, float mid, float high, bool wait_for_measure){
	//remap the eq values
	if(low < 0)
		low *= 70.0f;
	else
		low *= 6.0f;
	if(mid < 0)
		mid *= 70.0f;
	else
		mid *= 6.0f;
	if(high < 0)
		high *= 70.0f;
	else
		high *= 6.0f;
	BufferPlayer::CmdPtr cmd = new BufferPlayer::SetEQVals(low, mid, high);
	AudioIOBufferPlayerCmdPtr audioIOcmd = new AudioIOBufferPlayerCmd(mixer, cmd, wait_for_measure);
	mAudioIO.sendCommand(audioIOcmd);
}

void AudioDriver::connectToMixerPanel(){
	//volume + mute
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerVolumeChanged(unsigned int, float)),
			this,
			SLOT(mixerSetVolume(unsigned int, float)),
			Qt::QueuedConnection);
	//eq
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerEQValuesChanged(unsigned int, float, float, float)),
			this,
			SLOT(mixerSetEQVals(unsigned int, float, float, float)),
			Qt::QueuedConnection);
	//cue mode
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerCueModeChanged(unsigned int, bool)),
			this,
			SLOT(mixerSetCue(unsigned int, bool)),
			Qt::QueuedConnection);
	//pause
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerPausedChanged(unsigned int, bool)),
			this,
			SLOT(mixerSetPause(unsigned int, bool)),
			Qt::QueuedConnection);
	//sync
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerSyncModeChanged(unsigned int, bool)),
			this,
			SLOT(mixerSetSync(unsigned int, bool)),
			Qt::QueuedConnection);
	//tempo mul
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerTempoMulChanged(unsigned int, double)),
			this,
			SLOT(mixerSetTempoMul(unsigned int, double)),
			Qt::QueuedConnection);
	//seek
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerSeeking(unsigned int, int)),
			this,
			SLOT(mixerSeek(unsigned int, int)),
			Qt::QueuedConnection);
	//playback position
	QObject::connect(
			mMixerPanel,
			SIGNAL(mixerPlaybackPosChanged(unsigned int, int)),
			this,
			SLOT(mixerSetPlaybackPosition(unsigned int, int)),
			Qt::QueuedConnection);

	//report progress
	QObject::connect(
			this,
			SIGNAL(progressChanged(unsigned int, float)),
			mMixerPanel,
			SLOT(mixerUpdateProgress(unsigned int, float)),
			Qt::QueuedConnection);
	//report mixer tempo mul
	QObject::connect(
			this,
			SIGNAL(mixerTempoMulChanged(unsigned int, double)),
			mMixerPanel,
			SLOT(mixerSetTempoMul(unsigned int, double)),
			Qt::QueuedConnection);

	//master
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(volumeChanged(float)),
			this,
			SLOT(masterSetVolume(float)),
			Qt::QueuedConnection);
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(tempoChanged(float)),
			this,
			SLOT(masterSetTempo(float)),
			Qt::QueuedConnection);
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(tempoMulChanged(double)),
			this,
			SLOT(masterSetTempoMul(double)),
			Qt::QueuedConnection);
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(syncSourceChanged(unsigned int)),
			this,
			SLOT(masterSetSyncSrc(unsigned int)),
			Qt::QueuedConnection);
	//report tempo
	QObject::connect(
			this,
			SIGNAL(tempoChanged(float)),
			mMixerPanel->master(),
			SLOT(setTempo(float)),
			Qt::QueuedConnection);
	QObject::connect(
			this,
			SIGNAL(tempoMulChanged(double)),
			mMixerPanel->master(),
			SLOT(setTempoMul(double)),
			Qt::QueuedConnection);
}

AudioDriverThread::AudioDriverThread(QObject * parent) :
	QThread(parent)
{
}

void AudioDriverThread::setAudioDriver(AudioDriver * driver){
	mDriver = driver;
	mDriver->moveToThread(this);
}

void AudioDriverThread::run(){
	if(mDriver){
		//start the driver if it hasn't been started already
		if(mDriver->audioIO()->getState() != JackCpp::AudioIO::active)
			mDriver->start();
		QTimer *timer = new QTimer(mDriver);
		connect(timer, SIGNAL(timeout()), mDriver, SLOT(processAudioEvents()));
		//every 5 ms
		timer->start(5);
	}
	exec();
}

void AudioDriverThread::stop(){
	if(mDriver)
		mDriver->stop();
	quit();
}
