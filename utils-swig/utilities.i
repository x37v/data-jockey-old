%module datajockey_utils
%{
#include "../include/utilities.hpp"
%}

%include "std_string.i"
%include "std_except.i"

namespace DataJockey {
void soundFileToWave(std::string inFileName, std::string outFileName) throw (std::runtime_error);
std::string computeDescriptors(std::string inFileName, unsigned int windowSize = 1024);
}
