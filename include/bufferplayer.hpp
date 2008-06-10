#ifndef DATAJOCKEY_BUFFERPLAYER_HPP
#define DATAJOCKEY_BUFFERPLAYER_HPP

#include <vector>
#include <stdexcept>
#include <sndfile.hh>
#include <boost/intrusive_ptr.hpp>
#include "object.hpp"
#include "command.hpp"
#include "buffer.hpp"
#include "tempo.hpp"

#include <slv2/slv2.h>

#define DJ_EQ_URI "http://plugin.org.uk/swh-plugins/dj_eq"

namespace DataJockey {

	class BufferPlayer;
	typedef boost::intrusive_ptr<BufferPlayer> BufferPlayerPtr;

	class BufferPlayer : public Object {
		public:
			enum outputPort_t {mainOut, cueOut};
			enum playMode_t {syncPlayback, freePlayback};

			class Cmd;
			class CmdBundle;
			class GetState;
			class SetBuffers;
			class SetVolume;
			class SetTempoMultiplier;
			class SetLoopPoints;
			class SetLooping;
			class SetPlaybackPosition;
			class IncPlaybackPosition;
			class SetOutPort;
			class SetBeatOffset;
			class SetEQVals;
			class SetPlayMode;
			class SetPlaying;

			typedef boost::intrusive_ptr<Cmd> CmdPtr;
			typedef boost::intrusive_ptr<CmdBundle> CmdBundlePtr;
			typedef boost::intrusive_ptr<GetState> GetStatePtr;

			class LoopDescription {
				private:
					unsigned int mStartBeat;
					unsigned int mEndBeat;
				public:
					LoopDescription();
					void setLoopPoints(unsigned int start_beat, unsigned int end_beat);
					unsigned int getLoopStartBeat(){return mStartBeat;}
					unsigned int getLoopEndBeat(){return mEndBeat;}
			};

		private:
			unsigned int mPlaybackSampleRate;
			playMode_t mPlayMode;
			outputPort_t mOutPort;
			//float mTempoMul;
			unsigned int mId;
			static unsigned int mIdCnt;
			unsigned int mBeatIndex;
			float mVolScale;
			AudioBufferPtr mAudioBuffer;
			BeatBufferPtr mBeatBuffer;
			bool mLoop;
			LoopDescription mLoopPoints;
			float mBeatOffset;
			//this is used to keep track of the maximum volume sample,
			//so that we can do vu meters etc
			float mMaxSample;

			bool mPlaying;

			TempoDriver mMyTempoDriver;
			TempoDriver * mDefaultSync;

			//lv2 eq
			///**< Plugin "class" (actually just a few strings) */
			SLV2Plugin     mEqPlugin;
			/**< Plugin "instance" (loaded shared lib) */
			SLV2Instance   mEqInstance;

			uint32_t mEqLeftAudioIn;
			uint32_t mEqRightAudioIn;
			uint32_t mEqLeftAudioOut;
			uint32_t mEqRightAudioOut;
			float mEqControlHigh;
			float mEqControlMid;
			float mEqControlLow;
			float mEqControlOut;

		public:
			BufferPlayer(unsigned int sample_rate, TempoDriver * defaultSync, SLV2World lv2World, SLV2Plugins lv2plugins);
			virtual ~BufferPlayer();
			void setBuffers(AudioBufferPtr audio_buf, BeatBufferPtr beat_buf);
			BeatBufferPtr getBeatBuffer(){return mBeatBuffer;}
			AudioBufferPtr getAudioBuffer(){return mAudioBuffer;}
			unsigned int getBeatIndex();
			void setBeatIndex(unsigned int index);
			void reset();
			void advanceBeat(int num_beats = 1);
			TempoDriver * getTempoDriver() { return &mMyTempoDriver; } 

			void sync();

			float getSample(unsigned int chan);
			void setVolScale(float volume);
			unsigned int getId(){return mId;}

			void setLoopPoints(unsigned int start_beat, unsigned int end_beat);
			bool looping(){return mLoop;}
			void setLooping(bool loop = true);

			void setTempoMultiplier(float mult);
			float getTempoMultiplier();

			outputPort_t getOutPort(){return mOutPort;}
			void setOutPort(outputPort_t out);

			playMode_t getPlayMode(){return mPlayMode;}
			void setPlayMode(playMode_t mode);

			void tickTempoDriver();

			float getBeatOffset(){return mBeatOffset;}
			void setBeatOffset(float offset);

			float getMaxSample(){return mMaxSample;}
			void resetMaxSample(){mMaxSample = 0.0;}

			//this gets the beat period from the beat buffer
			bool validBeatPeriod();
			float getBeatPeriod(float beatIndexOffset);

			void applyEq(unsigned int nframes, float ** audio);
			void setEQVals(float low, float mid, float high);

			void play();
			void pause();
			bool getPlaying();
	};


	class BufferPlayer::Cmd : public Command {
		public:
			Cmd();
//			~Cmd(){ cout << "destroying me " << endl; }
			static const groupName_t classGroupName(){return "buffer_player_command";}
			virtual const groupName_t getGroupName(){return BufferPlayer::Cmd::classGroupName();}
			enum commandID_t {
				buffer_command_bundle,
				set_buffers,
				set_volume,
				set_tempo_multiplier,
				set_loop_points,
				set_looping,
				set_playback_position,
				inc_playback_position,
				set_out_port,
				set_beat_offset,
				get_state,
				set_eq_values,
				set_play_mode,
				set_playing,
			};
			virtual void operateOnPlayer(BufferPlayerPtr player) = 0;
	};

	class BufferPlayer::CmdBundle : public BufferPlayer::Cmd {
		private:
			std::vector<BufferPlayer::CmdPtr> mCmds;
		public:
			CmdBundle();
//			~CmdBundle(){cout << "destroying bplayer command bundler" << endl;}
			void addCmd(BufferPlayer::CmdPtr cmd);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::buffer_command_bundle;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetBuffers : public BufferPlayer::Cmd {
		private:
			bool mReset;
			BeatBufferPtr mBeatBuffer;
			BeatBufferPtr mOldBeatBuffer;
			AudioBufferPtr mAudioBuffer;
			AudioBufferPtr mOldAudioBuffer;
		public:
			SetBuffers(AudioBufferPtr audio_buffer, BeatBufferPtr beat_buffer, bool reset = true);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_buffers;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetVolume : public BufferPlayer::Cmd {
		private:
			float mVolume;
		public:
			SetVolume(float volume);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_volume;}
			float getVolume(){return mVolume;}
			void setVolume(float vol);
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetTempoMultiplier : public BufferPlayer::Cmd {
		private:
			double mTempoMul;
		public:
			SetTempoMultiplier(double mul);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_tempo_multiplier;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetLoopPoints : public BufferPlayer::Cmd {
		private:
			LoopDescription mLoopPoints;
		public:
			SetLoopPoints(unsigned int start_beat, unsigned int end_beat);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_loop_points;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetLooping : public BufferPlayer::Cmd {
		private:
			bool mLooping;
		public:
			SetLooping(bool looping = true);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_looping;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	//set position
	class BufferPlayer::SetPlaybackPosition : public BufferPlayer::Cmd {
		private:
			unsigned int mNextBeatIndex;
		public:
			SetPlaybackPosition(unsigned int beat_index);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_playback_position;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	//inc position
	class BufferPlayer::IncPlaybackPosition : public BufferPlayer::Cmd {
		private:
			int mIncrement;
		public:
			IncPlaybackPosition(int increment);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::inc_playback_position;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetOutPort : public BufferPlayer::Cmd {
		private:
			BufferPlayer::outputPort_t mOutPort;
		public:
			SetOutPort(BufferPlayer::outputPort_t out);
			BufferPlayer::outputPort_t getOutPort(){return mOutPort;}
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_out_port;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetBeatOffset : public BufferPlayer::Cmd {
		private:
			float mBeatOffset;
		public:
			SetBeatOffset(float offset);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_beat_offset;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	//this is an object the the audio callback fills in
	class BufferPlayer::GetState : public BufferPlayer::Cmd {
		private:
			float mBeatOffset;
			unsigned int mCurBeat;
			unsigned int mLastBeat;
			float mTempoMul;
			float mMaxSample;
			playMode_t mPlayMode;
			bool mPlaying;
		public:
			GetState();
			GetState(BufferPlayer::GetStatePtr other);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::get_state;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
			float getBeatOffset(){return mBeatOffset;}
			unsigned int getCurBeat(){return mCurBeat;}
			unsigned int getLastBeat(){return mLastBeat;}
			float getTempoMultiplier(){return mTempoMul;}
			float getMaxSample(){return mMaxSample;}
			playMode_t getPlayMode(){return mPlayMode;}
			bool getPlaying(){return mPlaying;}

			void setBeatOffset(float val){mBeatOffset = val;}
			void setCurBeat(unsigned int val){mCurBeat = val;}
			void setLastBeat(unsigned int val){mLastBeat = val;}
			void setTempoMultiplier(float val){mTempoMul = val;}
			void setMaxSample(float val){mMaxSample = val;}
			void setPlayMode(playMode_t mode){mPlayMode = mode;}
			void setPlaying(bool playing){mPlaying = playing;}
	};

	class BufferPlayer::SetEQVals : public BufferPlayer::Cmd {
		private:
			float mLow;
			float mMid;
			float mHigh;
		public:
			SetEQVals(float low, float mid, float high);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_eq_values;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
	};

	class BufferPlayer::SetPlayMode : public BufferPlayer::Cmd {
		private:
			playMode_t mMode;
		public:
			SetPlayMode(playMode_t mode);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_play_mode;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
			playMode_t getMode(){ return mMode; }
	};

	class BufferPlayer::SetPlaying : public BufferPlayer::Cmd {
		private:
			bool mPlaying;
		public:
			SetPlaying(bool play = true);
			virtual const typeID_t getTypeID(){return BufferPlayer::Cmd::set_playing;}
			virtual void operateOnPlayer(BufferPlayerPtr player);
			bool getPlaying(){ return mPlaying; }
	};


}

#endif

