%module datajockey
%{
/* Includes the header in the wrapper code */
#include "../include/applicationmodel.hpp"
#include "../include/crossfademodel.hpp"
#include "../include/config.hpp"
#include "../include/djmixerchannelmodel.hpp"
#include "../include/djmixercontrolmodel.hpp"
#include "../include/eqmodel.hpp"
#include "../include/mastermodel.hpp"
#include "../include/mixerchannelmodel.hpp"
#include "../include/mixerpanelmodel.hpp"
#include "../include/interpreterioproxy.hpp"
#include "../include/remoteworkfiltermodel.hpp"
#include "scriptcallbackfilter.hpp"
%}

/* Parse the header file to generate wrappers */
%include "std_string.i"
%include "std_except.i"

%include "applicationmodel.i"
%include "crossfademodel.i"
%include "djmixerchannelmodel.i"
%include "djmixercontrolmodel.i"
%include "eqmodel.i"
%include "mastermodel.i"
%include "mixerpanelmodel.i"
%include "interpreterioproxy.i"
/*%include "remoteworkfiltermodel.i" */
%include "scriptcallbackfilter.i"
%include "config.i"
