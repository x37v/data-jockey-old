%module datajockey_utils
%{
#include "../include/utilities.hpp"
%}

%include "std_string.i"
%include "std_except.i"

namespace DataJockey {
void soundFileToWave(std::string inFileName, std::string outFileName) throw (std::runtime_error);
}
