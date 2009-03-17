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

#ifndef DATAJOCKEY_AUDIOIO_HPP
#define DATAJOCKEY_AUDIOIO_HPP

#include <jackaudioio.hpp>
#include <jackringbuffer.hpp>
#include <vector>
#include <stdexcept>
#include "tempo.hpp"
#include "buffer.hpp"
#include "bufferplayer.hpp"
#include "command.hpp"
#include "object.hpp"

#include <slv2/slv2.h>

#define AUDIOIO_MAX_MESSAGES_PER_CALLBACK 1024
#define AUDIOIO_PREVIEW_BUFFER_MS 200
#define MEASURE_OUT 4

namespace DataJockey {

	class AudioIOCmd;
	class AudioIOBufferPlayerCmd;
	class AudioIOSetBPM;
	class AudioIOSetTempoScale;
	class AudioIOSetVolume;
	class AudioIOGetState;
	class AudioIOSyncToBufferPlayer;
	class AudioIOSyncToMasterClock;
	typedef boost::intrusive_ptr<AudioIOCmd> AudioIOCmdPtr;
	typedef boost::intrusive_ptr<AudioIOBufferPlayerCmd> AudioIOBufferPlayerCmdPtr;
	typedef boost::intrusive_ptr<AudioIOSetBPM> AudioIOSetBPMPtr;
	typedef boost::intrusive_ptr<AudioIOSetTempoScale> AudioIOSetTempoScalePtr;
	typedef boost::intrusive_ptr<AudioIOSetVolume> AudioIOSetVolumePtr;
	typedef boost::intrusive_ptr<AudioIOGetState> AudioIOGetStatePtr;
	typedef boost::intrusive_ptr<AudioIOSyncToBufferPlayer> AudioIOSyncToBufferPlayerPtr;
	typedef boost::intrusive_ptr<AudioIOSyncToMasterClock> AudioIOSyncToMasterClockPtr;

	class BeatAndMeasureDriver {
		public:
			TempoDriver tempoDriver;
			TempoDivider measureDriver;
			BeatAndMeasureDriver(unsigned int sampleRate, unsigned int beatsPerMeasure=4);
	};

	class AudioIO : public JackCpp::AudioIO, public Object {
		private:
			//here we deal with events that are on our mMsgBufToAudio
			void handleInputEvents();
			void handleMeasureEvents();
			void processCommand(AudioIOCmdPtr cmd);

			BeatAndMeasureDriver mBeatAndMeasureDriver;
			std::vector<BufferPlayerPtr> mBufferPlayers;
			JackCpp::RingBuffer<AudioIOCmdPtr> mMsgBufToAudio;
			JackCpp::RingBuffer<AudioIOCmdPtr> mMsgBufFromAudio;
			std::vector<AudioIOCmdPtr> mMeasureQueue;
			double mMasterVolume;
			double mCueVolume;

			std::vector<float **> mAudioBuffers;

			static bool cLv2Inited;
			static SLV2World cLv2World;
			static SLV2Plugins cLv2Plugins;

			bool mSyncToClock;
			unsigned int mSyncBufferPlayerIndex;
			float mLastBeatIndex;

			//indicates if we've updated the sync src internally
			bool mSyncSrcChanged;

			JackCpp::RingBuffer<jack_default_audio_sample_t> mPreviewBuffer;

		public:
			AudioIO(unsigned int num_buf_players = 4);
			~AudioIO();
			virtual int audioCallback(jack_nframes_t nframes, 
					std::vector<jack_default_audio_sample_t *> inBufs,
					std::vector<jack_default_audio_sample_t *> outBufs);
			virtual void jackShutdownCallback();
			//don't call this while the audio is running, it is not
			//realtime safe and will therefore thrown an error
			void addBufferPlayer(BufferPlayerPtr buffer_player)
				throw(std::runtime_error);
			void sendCommand(AudioIOCmdPtr cmd);
			AudioIOGetStatePtr consume();
			unsigned int previewFrames();
			unsigned int previewFramesFree();
			void queuePreviewFrames(jack_default_audio_sample_t * buffer, jack_nframes_t frames);
	};

	class AudioIOCmd : public Command {
		private:
			bool mWaitForMeasure;
		public:
			AudioIOCmd(bool wait_for_measure = true);
			static const groupName_t classGroupName(){return "audioio_command";}
			virtual const groupName_t getGroupName(){return AudioIOCmd::classGroupName();}
			bool getWaitForMeasure(){return mWaitForMeasure;}
			void setWaitForMeasure(bool wait_status = true){mWaitForMeasure = wait_status;}
			enum commandID_t {
				buffer_player_command,
				set_bpm,
				set_tempo_scale,
				set_volume,
				get_state,
				sync_to_buffer_player,
				sync_to_master_clock
			};
	};

	//this is a command that contains a command for a specific buffer player
	class AudioIOBufferPlayerCmd : public AudioIOCmd {
		private:
			unsigned int mBufferPlayerIndex;
			BufferPlayer::CmdPtr mBufferPlayerCmd;
		public:
			AudioIOBufferPlayerCmd(
					unsigned int buffer_player_index, 
					BufferPlayer::CmdPtr buffer_cmd, 
					bool wait_for_measure = true);
			unsigned int getBufferPlayerIndex(){return mBufferPlayerIndex;}
			BufferPlayer::CmdPtr getBufferPlayerCmd(){return mBufferPlayerCmd;}
			virtual const typeID_t getTypeID(){return AudioIOCmd::buffer_player_command;}
	};

	class AudioIOSetBPM : public AudioIOCmd {
		private:
			double mBPM;
		public:
			AudioIOSetBPM(double bpm, bool wait_for_measure = true);
			virtual const typeID_t getTypeID(){return AudioIOCmd::set_bpm;}
			double getBPM(){return mBPM;}
	};

	class AudioIOSetTempoScale : public AudioIOCmd {
		private:
			double mTempoScale;
		public:
			AudioIOSetTempoScale(double scale, bool wait_for_measure = false);
			virtual const typeID_t getTypeID(){return AudioIOCmd::set_tempo_scale;}
			double getTempoScale(){return mTempoScale;}
	};

	class AudioIOSetVolume : public AudioIOCmd {
		private:
			double mVol;
		public:
			AudioIOSetVolume(double vol, bool wait_for_measure = false);
			virtual const typeID_t getTypeID(){return AudioIOCmd::set_volume;}
			double getVolume(){return mVol;}
	};

	class AudioIOGetState : public AudioIOCmd {
		private:
			std::vector<BufferPlayer::GetStatePtr> mBufferPlayerStates;
			unsigned int mSyncBufferPlayerIndex;
			bool mSyncToClock;
			double mPeriod;
			double mTempoScale;
			bool mSyncSrcChanged;
		public:
			AudioIOGetState(unsigned int nbufferplayers);
			virtual const typeID_t getTypeID(){return AudioIOCmd::get_state;}
			std::vector<BufferPlayer::GetStatePtr> getBufferPlayerStates(){
				return mBufferPlayerStates;
			}
			bool getSyncToClock(){ return mSyncToClock;}
			void setSyncToClock(bool sync){mSyncToClock = sync;}
			unsigned int getSyncBufferPlayerIndex(){ return mSyncBufferPlayerIndex;}
			void setSyncBufferPlayerIndex(unsigned int index){ mSyncBufferPlayerIndex = index;}
			void setSyncSrcChanged(){ mSyncSrcChanged = true;}
			bool getSyncSrcChanged(){ return mSyncSrcChanged;}
			double getPeriod(){return mPeriod;}
			void setPeriod(double period){ mPeriod = period;}
			double getTempoScale(){return mTempoScale;}
			void setTempoScale(double scale){ mTempoScale = scale;}
	};

	class AudioIOSyncToBufferPlayer : public AudioIOCmd {
		private:
			unsigned int mBufferPlayerIndex;
		public:
			AudioIOSyncToBufferPlayer(
					unsigned int bufferPlayerIndex, 
					bool wait_for_measure = false);
			virtual const typeID_t getTypeID(){return AudioIOCmd::sync_to_buffer_player;}
			unsigned int getBufferPlayerIndex(){return mBufferPlayerIndex;}
	};

	class AudioIOSyncToMasterClock : public AudioIOCmd {
		public:
			AudioIOSyncToMasterClock(bool wait_for_measure = false);
			virtual const typeID_t getTypeID(){return AudioIOCmd::sync_to_master_clock;}
	};

}

#endif

