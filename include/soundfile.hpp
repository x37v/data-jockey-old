#ifndef ALEX_SOUNDFILE_HPP
#define ALEX_SOUNDFILE_HPP

#include <sndfile.hh>
#include <string>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <mad.h>
#include <fstream>
#include <vector>

class SoundFile {
	public:
		typedef struct {

			struct mad_stream	stream;
			struct mad_frame	frame;
			struct mad_synth	synth;

			unsigned char *inputBuffer;
			unsigned long inBufLength;
			std::ifstream inputFile;
			
			bool endOfFile;

			//this is a count of unread frames remaining in synth.pcm
			unsigned int remaining;
		} MP3FileData;
	private:
		enum filetype {UNSUPPORTED, SNDFILE, OGG, MP3};

		SndfileHandle mSndFile;
		filetype mType;
		OggVorbis_File mOggFile;
		int mOggIndex;
		MP3FileData mMP3Data;

		void synthMadFrame();
		int mSampleRate;
		unsigned int mChannels;

		//private member functions for reading ogg/mp3 shorts
		unsigned int oggReadShortFrame (short *ptr, unsigned int frames);
		unsigned int mp3ReadShortFrame (short *ptr, unsigned int frames);
		//this will read either mp3 or ogg float frames
		unsigned int readFloatFrame (float *ptr, unsigned int frames);
	public:
		SoundFile(std::string location);
		~SoundFile();
		unsigned int samplerate();
		unsigned int channels();
		unsigned int readf (float *ptr, unsigned int frames) ;
		unsigned int readf (short *ptr, unsigned int frames) ;
		operator bool () const ;
};

#endif

