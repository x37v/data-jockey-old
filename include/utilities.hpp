#include <string>
#include <stdexcept>

namespace DataJockey {
	//convert a sound file to a wave file
	void soundFileToWave(std::string inFileName, std::string outFileName);
	std::string computeDescriptors(std::string inFileName, unsigned int windowSize = 1024);
}
