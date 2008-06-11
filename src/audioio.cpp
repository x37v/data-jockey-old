#include "audioio.hpp"
using namespace DataJockey;

#include <iostream>
using std::cout;
using std::endl;

SLV2World AudioIO::cLv2World;
SLV2Plugins AudioIO::cLv2Plugins;
bool AudioIO::cLv2Inited = false;

BeatAndMeasureDriver::BeatAndMeasureDriver(unsigned int sampleRate, unsigned int beatsPerMeasure) :
	tempoDriver(sampleRate), 
	measureDriver(beatsPerMeasure)
{
}

AudioIO::AudioIO(unsigned int num_buf_players) : 
	JackCpp::AudioIO("datajockey", 0, 0), 
	mBeatAndMeasureDriver(getSampleRate()),
	mMsgBufToAudio(AUDIOIO_MAX_MESSAGES_PER_CALLBACK),
	mMsgBufFromAudio(AUDIOIO_MAX_MESSAGES_PER_CALLBACK)
{
	mMasterVolume = 1.0;
	mCueVolume = 0.8;
	//reserve as much space as messages we can get
	mMeasureQueue.reserve(AUDIOIO_MAX_MESSAGES_PER_CALLBACK);
	//add the output ports
	addOutPort("output0");
	addOutPort("output1");
	addOutPort("cue0");
	addOutPort("cue1");
	addOutPort("measure");

	mSyncToClock = true;
	mSyncBufferPlayerIndex = 0;
	mLastBeatIndex = 0.0;

	//lv2 stuff
	if(!cLv2Inited){
		cLv2Inited = true;
		cLv2World = slv2_world_new();
		slv2_world_load_all(cLv2World);
		cLv2Plugins	= slv2_world_get_all_plugins(cLv2World);
	}

	for(unsigned int i = 0; i < num_buf_players; i++){
		BufferPlayerPtr p(new BufferPlayer(getSampleRate(), &mBeatAndMeasureDriver.tempoDriver, cLv2World, cLv2Plugins));
		addBufferPlayer(p);
		float ** stereoAudioBuf = new float*[2];
		stereoAudioBuf[0] = new float[getBufferSize()];
		stereoAudioBuf[1] = new float[getBufferSize()];
		mAudioBuffers.push_back(stereoAudioBuf);
	}
}

AudioIO::~AudioIO(){
	//free up audio buffers
	for(std::vector<float **>::iterator it = mAudioBuffers.begin(); 
			it != mAudioBuffers.end(); it++){
		delete [] (*it)[0];
		delete [] (*it)[1];
		delete [] (*it);
	}
	//free up lv2 stuff
	//XXX:
//	slv2_plugins_free(cLv2World, cLv2Plugins);
//	slv2_world_free(cLv2World);
}

void AudioIO::handleInputEvents(){
	while(mMsgBufToAudio.getReadSpace() > 0){
		//since the cmd is defined inside the while block
		//the reference count is actually correct after
		//it is read from the buffer as it is incremented
		//before being put in the buffer, but is not incremented
		//when read from it

		AudioIOCmdPtr cmd;
		mMsgBufToAudio.read(cmd);

		if(!cmd->getWaitForMeasure()){
			processCommand(cmd);
		} else {
			mMeasureQueue.push_back(cmd);
		}
	}
}

void AudioIO::handleMeasureEvents(){
	while(!mMeasureQueue.empty()){
		AudioIOCmdPtr cmd;
		cmd = mMeasureQueue.back();
		mMeasureQueue.pop_back();
		processCommand(cmd);
	}
}

void AudioIO::processCommand(AudioIOCmdPtr cmd){
	switch(cmd->getTypeID()){
		case AudioIOCmd::buffer_player_command : 
			{
				AudioIOBufferPlayerCmdPtr playerCmd = 
					boost::static_pointer_cast<AudioIOBufferPlayerCmd,AudioIOCmd>(cmd);
				unsigned int index = playerCmd->getBufferPlayerIndex();
				if(index >= mBufferPlayers.size()){
					cmd->setRejected();
				} else {
					BufferPlayer::CmdPtr bPlayerCmd(playerCmd->getBufferPlayerCmd());
					bPlayerCmd->operateOnPlayer(mBufferPlayers[index]);
					cmd->setCompleted();

					//set up the clock so that the tempo doesn't shift drastically when we
					//sync to the audio work
					//XXX not sure about this one..
					if (bPlayerCmd->getTypeID() == BufferPlayer::Cmd::set_buffers &&
							!mSyncToClock && mSyncBufferPlayerIndex == index){
						if(mBufferPlayers[mSyncBufferPlayerIndex]->validBeatPeriod()){
							mBeatAndMeasureDriver.tempoDriver.syncToPeriod(
									mBufferPlayers[mSyncBufferPlayerIndex]->getBeatPeriod( mLastBeatIndex) / 
									mBufferPlayers[mSyncBufferPlayerIndex]->getTempoMultiplier()
									);
						}
					}
				}
			} 
			break;
		case AudioIOCmd::set_bpm : 
			{
				AudioIOSetBPMPtr bpmCmd = 
					boost::static_pointer_cast<AudioIOSetBPM,AudioIOCmd>(cmd);
				mBeatAndMeasureDriver.tempoDriver.setBPM(bpmCmd->getBPM());
				cmd->setCompleted();
			}
			break;
		case AudioIOCmd::set_tempo_scale : 
			{
				AudioIOSetTempoScalePtr tempoCmd = 
					boost::static_pointer_cast<AudioIOSetTempoScale,AudioIOCmd>(cmd);
				TempoDriver * syncSrc = mBeatAndMeasureDriver.tempoDriver.getSyncSrc();
				//if there is no sync src then scale our tempo
				//otherwise, scale the tempo of the clock we're in sync with
				if(syncSrc == NULL)
					mBeatAndMeasureDriver.tempoDriver.setTempoScale(tempoCmd->getTempoScale());
				else
					syncSrc->setTempoScale(tempoCmd->getTempoScale());
				cmd->setCompleted();
			}
			break;
		case AudioIOCmd::set_volume : 
			{
				AudioIOSetVolumePtr volCmd = 
					boost::static_pointer_cast<AudioIOSetVolume,AudioIOCmd>(cmd);
				mMasterVolume = volCmd->getVolume();
				cmd->setCompleted();
			}
			break;
		//here we deal with state request commands
		case AudioIOCmd::get_state : 
			{
				TempoDriver * syncSrc = mBeatAndMeasureDriver.tempoDriver.getSyncSrc();
				AudioIOGetStatePtr stateCmd = 
					boost::static_pointer_cast<AudioIOGetState,AudioIOCmd>(cmd);
				//make sure the buffer for the information requested actually exists
				for(unsigned int i = 0; i < stateCmd->getBufferPlayerStates().size(); i++){
					if(i < mBufferPlayers.size()){
						stateCmd->getBufferPlayerStates()[i]->operateOnPlayer(mBufferPlayers[i]);
						stateCmd->getBufferPlayerStates()[i]->setCompleted();
					} else
						stateCmd->getBufferPlayerStates()[i]->setRejected();
				}
				stateCmd->setSyncToClock(mSyncToClock);
				stateCmd->setSyncBufferPlayerIndex(mSyncBufferPlayerIndex);
				//if we're syncing then we reflect that in the state update
				if(syncSrc != NULL){
					stateCmd->setPeriod(syncSrc->getPeriod());
					stateCmd->setTempoScale(syncSrc->getTempoScale());
				} else {
					stateCmd->setPeriod(mBeatAndMeasureDriver.tempoDriver.getPeriod());
					stateCmd->setTempoScale(mBeatAndMeasureDriver.tempoDriver.getTempoScale());
				}

				cmd->setCompleted();
			}
			break;
		case AudioIOCmd::sync_to_buffer_player : 
			{
				AudioIOSyncToBufferPlayerPtr syncBufferPlayer = 
					boost::static_pointer_cast<AudioIOSyncToBufferPlayer,AudioIOCmd>(cmd);
				bool wasSyncingToClock = mSyncToClock;
				unsigned int oldSyncBufferPlayerIndex = mSyncBufferPlayerIndex;
				//if the index is in range then sync to that!
				if (syncBufferPlayer->getBufferPlayerIndex() < mBufferPlayers.size()){
					mSyncToClock = false;
					mSyncBufferPlayerIndex = syncBufferPlayer->getBufferPlayerIndex();
					//if we were in sync with another buffer player then we sync to
					//that instead of the main clock
					if(!wasSyncingToClock && mSyncBufferPlayerIndex != oldSyncBufferPlayerIndex){
						if(mBufferPlayers[mSyncBufferPlayerIndex]->validBeatPeriod() &&
								mBufferPlayers[mSyncBufferPlayerIndex]->getPlayMode() == BufferPlayer::syncPlayback){
							mBufferPlayers[mSyncBufferPlayerIndex]->getTempoDriver()->syncToPeriod(
									mBufferPlayers[oldSyncBufferPlayerIndex]->getTempoDriver()->getPeriod());
							//reset the tempoMul of the old tempo driver if that player is playing in sync mode.
							if(mBufferPlayers[oldSyncBufferPlayerIndex]->getPlayMode() == BufferPlayer::syncPlayback){
								mBufferPlayers[oldSyncBufferPlayerIndex]->getTempoDriver()->setPeriodMul(1.0);
							}
						}
					} else {
						if(mBufferPlayers[mSyncBufferPlayerIndex]->validBeatPeriod() &&
								mBufferPlayers[mSyncBufferPlayerIndex]->getPlayMode() == BufferPlayer::syncPlayback){
							mBufferPlayers[mSyncBufferPlayerIndex]->getTempoDriver()->syncToPeriod(
									mBeatAndMeasureDriver.tempoDriver.getPeriod());
						}
					}
					//if we have a valid beat period then we sync to that clock..
					//otherwise we stay at the same setting we had before
					if(mBufferPlayers[mSyncBufferPlayerIndex]->validBeatPeriod()){
						mBeatAndMeasureDriver.tempoDriver.syncTo(
								mBufferPlayers[mSyncBufferPlayerIndex]->getTempoDriver());
					} else {
						mSyncBufferPlayerIndex = oldSyncBufferPlayerIndex;
						mSyncToClock = wasSyncingToClock;
					}
				}
			}
			break;
		case AudioIOCmd::sync_to_master_clock : 
			{
				TempoDriver * syncSrc = mBeatAndMeasureDriver.tempoDriver.getSyncSrc();
				mSyncToClock = true;
				if(syncSrc != NULL){
					mBeatAndMeasureDriver.tempoDriver.runFree();
					//only scale the period mul if we're syncing
					if(mBufferPlayers[mSyncBufferPlayerIndex]->getPlayMode() == BufferPlayer::syncPlayback){
						syncSrc->setPeriodMul(1.0);
					}
				}
				break;
			}
		default:
			//otherwise reject it and push it onto the output
			cmd->setRejected();
	}
	//you have to add a reference before putting it into
	//the buffer because the buffer doesn't keep track of the
	//reference count
	intrusive_ptr_add_ref(cmd.get());
	mMsgBufFromAudio.write(cmd);
}

int AudioIO::audioCallback(jack_nframes_t nframes, 
		std::vector<jack_default_audio_sample_t *> inBufs,
		std::vector<jack_default_audio_sample_t *> outBufs){
	float beatIndex;
	bool beat = false, measure = false;

	handleInputEvents();

	for(unsigned int i = 0; i < nframes; i++){
		//tick the beat and measure clocks
		beat = mBeatAndMeasureDriver.tempoDriver.tick(beatIndex);
		mLastBeatIndex = beatIndex;

		//send out the measure index so that other apps can sync
		outBufs[MEASURE_OUT][i] = (beatIndex + (float)mBeatAndMeasureDriver.measureDriver.getCnt()) / 
			(float)mBeatAndMeasureDriver.measureDriver.getDivAmt();

		if(beat)
			measure = mBeatAndMeasureDriver.measureDriver.tick();
		//if a measure happened then operate on that
		if(measure)
			handleMeasureEvents();

		//zero out the output buffers
		for(unsigned int j = 0; j < 4; j++){
			outBufs[j][i] = 0;
		}

		//tick the player's buffers
		for(std::vector<BufferPlayerPtr>::iterator player_it = mBufferPlayers.begin();
				player_it != mBufferPlayers.end(); player_it++){
			(*player_it)->tickTempoDriver();
		}

		//SYNC!
		//master
		mBeatAndMeasureDriver.tempoDriver.sync();
		//players
		for(std::vector<BufferPlayerPtr>::iterator player_it = mBufferPlayers.begin();
				player_it != mBufferPlayers.end(); player_it++){
			(*player_it)->sync();
		}

		//loop through the players and get their samples
		std::vector<float **>::iterator buffer_it = mAudioBuffers.begin();
		for(std::vector<BufferPlayerPtr>::iterator player_it = mBufferPlayers.begin();
				!((player_it == mBufferPlayers.end()) || (buffer_it == mAudioBuffers.end())); 
				player_it++, buffer_it++){

			for(unsigned int j = 0; j < 2; j++)
				(*buffer_it)[j][i] = (*player_it)->getSample(j);
		}
	}

	std::vector<float **>::iterator buffer_it = mAudioBuffers.begin();
	for(std::vector<BufferPlayerPtr>::iterator player_it = mBufferPlayers.begin();
			!((player_it == mBufferPlayers.end()) || (buffer_it == mAudioBuffers.end())); 
			player_it++, buffer_it++){

		(*player_it)->applyEq(nframes, *buffer_it);

		//are we going to the main out or the cue out?
		if((*player_it)->getOutPort() == BufferPlayer::mainOut){
			for(unsigned int j = 0; j < 2; j++){
				for(unsigned int i = 0; i < nframes; i++){
					outBufs[j][i] += mMasterVolume * (*buffer_it)[j][i];
				}
			}
		} else {
			for(unsigned int j = 0; j < 2; j++){
				for(unsigned int i = 0; i < nframes; i++){
					outBufs[j + 2][i] += mCueVolume * (*buffer_it)[j][i];
				}
			}
		}
	}
	return 0;
}


//don't call this after we've started the audio!
void AudioIO::addBufferPlayer(BufferPlayerPtr buffer_player)
	throw(std::runtime_error)
{
	if(getState() == JackCpp::AudioIO::active){
		throw std::runtime_error("trying to add bufferplayer while active");
	} else {
		mBufferPlayers.push_back(buffer_player);
	}
}

void AudioIO::sendCommand(AudioIOCmdPtr cmd){
	//increment the count as it isn't correctly
	//kept track of once it gets written to the buffer
	intrusive_ptr_add_ref(cmd.get());
	mMsgBufToAudio.write(cmd);
}

AudioIOGetStatePtr AudioIO::consume(){
	AudioIOGetStatePtr state;
	//you have to define the cmd inside the block since
	//the read doesn't update the reference increment
	while(mMsgBufFromAudio.getReadSpace() > 0){
		AudioIOCmdPtr cmd;
		mMsgBufFromAudio.read(cmd);
		if(cmd->getStatus() == Command::rejected){
			cout << cmd->getGroupName() << " : " << cmd->getTypeID() << " was rejected" << endl;
		}
		if(cmd->getStatus() == Command::completed &&
				cmd->getTypeID() == AudioIOCmd::get_state){
			state = 
				boost::static_pointer_cast<AudioIOGetState,AudioIOCmd>(cmd);
		}
	}
	return state;
}

//************
//AudioIOCmd
//**************

AudioIOCmd::AudioIOCmd(bool wait_for_measure) : Command(){
	mWaitForMeasure = wait_for_measure;
}

//************
//AudioIOBufferPlayerCmd
//**************

AudioIOBufferPlayerCmd::AudioIOBufferPlayerCmd(
		unsigned int buffer_player_index, 
		BufferPlayer::CmdPtr buffer_cmd,
		bool wait_for_measure) :
	AudioIOCmd(wait_for_measure)
{
	mBufferPlayerIndex = buffer_player_index;
	mBufferPlayerCmd = buffer_cmd;
}

//************
//AudioIOSetBPM
//**************
AudioIOSetBPM::AudioIOSetBPM(double bpm, bool wait_for_measure) : 
	AudioIOCmd(wait_for_measure)
{
	mBPM = bpm;
}

//************
//AudioIOSetTempoScale
//**************
AudioIOSetTempoScale::AudioIOSetTempoScale(double scale, bool wait_for_measure) : 
	AudioIOCmd(wait_for_measure)
{
	mTempoScale = scale;
}

//************
//AudioIOSetVolume
//**************

AudioIOSetVolume::AudioIOSetVolume(double vol, bool wait_for_measure) :
	AudioIOCmd(wait_for_measure)
{
	mVol = vol;
}

//************
//AudioIOGetState
//**************

AudioIOGetState::AudioIOGetState(unsigned int nbufferplayers) :
	AudioIOCmd(false)
{
	//allocate the buffer player states here
	for(unsigned int i = 0; i < nbufferplayers; i++){
		BufferPlayer::GetStatePtr bpState = new BufferPlayer::GetState();
		mBufferPlayerStates.push_back(bpState);
	}
}

//*************
//AudioIOSyncToBufferPlayer
//****************

AudioIOSyncToBufferPlayer::AudioIOSyncToBufferPlayer(
		unsigned int bufferPlayerIndex, bool wait_for_measure) :
	AudioIOCmd(wait_for_measure)
{
	mBufferPlayerIndex = bufferPlayerIndex;
}

//*************
//AudioIOSyncToMasterClock
//****************

AudioIOSyncToMasterClock::AudioIOSyncToMasterClock(bool wait_for_measure) :
	AudioIOCmd(wait_for_measure)
{
	//don't need to do anything!
}