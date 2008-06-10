#include "utilities.hpp"
#include "soundfile.hpp"
#define PCM_READ_SIZE 2048

void DataJockey::soundFileToWave(std::string inFileName, std::string outFileName){
	SoundFile inFile(inFileName.c_str());
	short inbuf[PCM_READ_SIZE];
	unsigned int frames_read;

	//check to make sure soundfile exists
	if(!inFile){
		std::string str("cannot open input soundfile: ");
		str.append(inFileName);
		throw std::runtime_error(str);
	}

	//we open the output file, we want it to be a wave file
	SndfileHandle outFile(outFileName.c_str(), 
			SFM_WRITE, 
			SF_FORMAT_WAV | SF_FORMAT_PCM_16, 
			inFile.channels(), 
			inFile.samplerate());
	if(!outFile){
		std::string str("cannot open output soundfile: ");
		str.append(outFileName);
		throw std::runtime_error(str);
	}

	//read in the audio data and write to the output file
	while((frames_read = inFile.readf(inbuf, PCM_READ_SIZE / inFile.channels())) != 0)
		outFile.writef(inbuf, frames_read);
}
