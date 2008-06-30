%module datajockey
%{
/* Includes the header in the wrapper code */
#include "../include/applicationmodel.hpp"
#include "../include/crossfademodel.hpp"
#include "../include/djmixerchannelmodel.hpp"
#include "../include/djmixercontrolmodel.hpp"
#include "../include/eqmodel.hpp"
#include "../include/mixerchannelmodel.hpp"
#include "../include/mixerpanelmodel.hpp"
%}

/* Parse the header file to generate wrappers */
%include "std_string.i"
%include "std_except.i"

%include "applicationmodel.i"
%include "crossfademodel.i"
%include "djmixerchannelmodel.i"
%include "djmixercontrolmodel.i"
%include "eqmodel.i"
%include "mixerchannelmodel.i"
%include "mixerpanelmodel.i"
