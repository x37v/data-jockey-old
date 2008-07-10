#include "oscreceiver.hpp"
#include "osc/OscReceivedElements.h"
#include <boost/regex.hpp>
#include "mixerpanelmodel.hpp"
#include <stdlib.h>

OscReceiver::OscReceiver(MixerPanelModel * model){
	mModel = model;
}

#include <iostream>
using namespace std;

void OscReceiver::ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint ){
	boost::regex top_re("^/datajockey/(\\w*)/(.*)$");
	boost::regex mixer_re("^mixer$");
	boost::regex xfade_re("^crossfade$");
	boost::regex master_re("^master$");
	boost::cmatch matches;
	std::string addr;
	try {
		//matches /datajockey/(\w*)/(.*)
		if(boost::regex_match(m.AddressPattern(), matches, top_re)){
			std::string sub_match(matches[1]);
			if(boost::regex_match(sub_match, mixer_re)){
				processMixerMessage(matches[2], m);
			} else if(boost::regex_match(sub_match, xfade_re)){
				processXFadeMessage(matches[2], m);
			} else if(boost::regex_match(sub_match, master_re)){
				processMasterMessage(matches[2], m);
			}
		}
	} catch( osc::Exception& e ){
	}
}

void OscReceiver::processMixerMessage(const std::string addr, const osc::ReceivedMessage& m){
	boost::regex mixer_re("^(\\d+)/(.+)");
	boost::regex volume_re("^volume(/relative){0,1}/{0,1}$");
	boost::regex mute_re("^mute(/toggle){0,1}/{0,1}$");
	boost::regex eq_re("^eq/(high|mid|low)(/relative|/cut|/cut/toggle){0,1}/{0,1}$");
	boost::cmatch matches;
	if(boost::regex_match(addr.c_str(), matches, mixer_re)){
		unsigned int mixer = (unsigned int)atoi(matches[1].str().c_str());
		std::string remain(matches[2].str());
		//make sure we're in range
		if(mixer >= mModel->numMixerChannels())
			return;
		if(boost::regex_match(remain.c_str(), matches, volume_re)){
			if(matches.size() == 2){
				if(strcmp("", matches[1].str().c_str()) == 0)
					cout << "absolute" << endl;
				else
					cout << "relative" << endl;
			}
		} else if(boost::regex_match(remain.c_str(), matches, mute_re)){
			if(matches.size() == 2){
				if(strcmp("", matches[1].str().c_str()) == 0)
					cout << "mute" << endl;
				else
					cout << "toggle" << endl;
			}
		} else if(boost::regex_match(remain.c_str(), matches, eq_re)){
			if(matches.size() == 3){
				//absolute
				if(strcmp(matches[2].str().c_str(), "") == 0){
					cout << "absolute " << matches[1] << endl;
					//cut
				} else if(strcmp(matches[2].str().c_str(), "/cut") == 0){
					cout << "cut " << matches[1] << endl;
					//toggle cut
				} else if(strcmp(matches[2].str().c_str(), "/cut/toggle") == 0){
					cout << "cut/toggle " << matches[1] << endl;
					//otherwise it is relative
				} else {
					cout << "relative " << matches[1] << endl;
				}
			}
		}
	}
}

void OscReceiver::processXFadeMessage(const std::string addr, const osc::ReceivedMessage& m){
}

void OscReceiver::processMasterMessage(const std::string addr, const osc::ReceivedMessage& m){
}

