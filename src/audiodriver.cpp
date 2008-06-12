#include "audiodriver.hpp"
#include "mixerpanelmodel.hpp"
#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include <sys/mman.h>

#include <QThread>

using namespace DataJockey;

AudioDriver::AudioDriver(MixerPanelModel * mixer, QObject * parent) :
	QObject(parent), mAudioIO(mixer->mixerChannels()->size())
{
	//unlock memory because we use a lot of it!
	munlockall();

	mMixerPanel = mixer;
	mNumMixers = mMixerPanel->mixerChannels()->size();
	mSyncToMaster = true;
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
			float progress = (float)bufferPlayerStates[i]->getCurBeat() / 
				(float)bufferPlayerStates[i]->getLastBeat();
			emit(progressChanged(i, progress));
		}
		//deal with tempo changes
		//it only changes when we're in sync with a buffer
		if(!state->getSyncToClock()){
			float tempo = 60.0f / state->getPeriod();
			emit(tempoChanged(tempo));
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

void AudioDriver::masterSetTempoMul(float mul, bool wait_for_measure){
	AudioIOSetTempoScalePtr cmd = new AudioIOSetTempoScale(mul, wait_for_measure);
	mAudioIO.sendCommand(cmd);

}

void AudioDriver::masterSetSyncSrc(unsigned int src, bool wait_for_measure){
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
	BufferPlayer::CmdPtr cmd;
	if(sync)
		cmd = new BufferPlayer::SetPlayMode(BufferPlayer::syncPlayback);
	else
		cmd = new BufferPlayer::SetPlayMode(BufferPlayer::freePlayback);
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

void AudioDriver::mixerReportLoadProgress(unsigned int mixer, float progress){
	//report the load progress
	if(mixer < mMixerPanel->mixerChannels()->size()){
		QMetaObject::invokeMethod(mMixerPanel->mixerChannels()->at(mixer)->DJMixerControl(), 
				"setLoadProgress", 
				Qt::QueuedConnection, Q_ARG(float, progress));
	}
}
