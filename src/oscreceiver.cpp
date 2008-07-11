#include "oscreceiver.hpp"
#include "osc/OscReceivedElements.h"
#include <boost/regex.hpp>
#include "mixerpanelmodel.hpp"
#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include "mixerchannelmodel.hpp"
#include "eqmodel.hpp"
#include <stdlib.h>
#include <iostream>

bool boolFromBoolOrInt(const osc::ReceivedMessageArgument a){
	if(a.IsBool())
		return a.AsBool();
	else if(a.IsInt32())
		return (a.AsInt32() != 0);
	else if(a.IsInt64())
		return (a.AsInt64() != 0);
	else
		throw osc::WrongArgumentTypeException();
}

float floatFromOscNumber(const osc::ReceivedMessageArgument a){
	if(a.IsFloat())
		return a.AsFloat();
	if(a.IsDouble())
		return (float)a.AsDouble();
	else if(a.IsInt32())
		return (float)a.AsInt32();
	else if(a.IsInt64())
		return (float)a.AsInt64();
	else
		throw osc::WrongArgumentTypeException();
}

OscReceiver::OscReceiver(MixerPanelModel * model){
	mModel = model;
}


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
		std::cerr << "An Exception occured while processing incoming OSC packets." << std::endl;
		std::cerr << e.what() << std::endl;
	}
}

void OscReceiver::processMixerMessage(const std::string addr, const osc::ReceivedMessage& m){
	boost::regex mixer_re("^(\\d+)/(.+)");
	boost::regex volume_re("^volume(/relative){0,1}/{0,1}$");
	boost::regex mute_re("^mute(/toggle){0,1}/{0,1}$");
	boost::regex eq_re("^eq/(high|mid|low)(/relative|/cut|/cut/toggle){0,1}/{0,1}$");
	boost::cmatch matches;
	osc::ReceivedMessage::const_iterator arg_it = m.ArgumentsBegin();

	if(boost::regex_match(addr.c_str(), matches, mixer_re)){
		unsigned int mixer = (unsigned int)atoi(matches[1].str().c_str());
		std::string remain(matches[2].str());
		//make sure we're in range
		if(mixer >= mModel->numMixerChannels())
			return;
		if(boost::regex_match(remain.c_str(), matches, volume_re)){
			//make sure our matches list is long enough and that we have an argument
			if(matches.size() == 2 && arg_it != m.ArgumentsEnd()){
				float num = floatFromOscNumber(*arg_it);
				//"" == absolute, otherwise, relative
				if(strcmp("", matches[1].str().c_str()) == 0)
					mModel->mixerChannels()->at(mixer)->setVolume(num);
				else {
					mModel->mixerChannels()->at(mixer)->setVolume(
							mModel->mixerChannels()->at(mixer)->volume() + num);
				}
			} else
				throw osc::MissingArgumentException();
		} else if(boost::regex_match(remain.c_str(), matches, mute_re)){
			//make sure our matches list is long enough to test
			if(matches.size() == 2){
				//if we have no argument then we're just setting the mute
				//otherwise toggle mute
				if(strcmp("", matches[1].str().c_str()) == 0) {
					if(arg_it != m.ArgumentsEnd())
						mModel->mixerChannels()->at(mixer)->setMuted(boolFromBoolOrInt(*arg_it));
					else 
						throw osc::MissingArgumentException();
				} else {
					mModel->mixerChannels()->at(mixer)->setMuted(
							!mModel->mixerChannels()->at(mixer)->muted());
				}
			}
		} else if(boost::regex_match(remain.c_str(), matches, eq_re)){
			if(matches.size() == 3){
				EQModel * eqModel = mModel->mixerChannels()->at(mixer)->eq();
				//figure out the band
				EQModel::band band;
				if(strcmp(matches[1].str().c_str(), "low") == 0)
					band = EQModel::LOW;
				else if(strcmp(matches[1].str().c_str(), "mid") == 0)
					band = EQModel::MID;
				else
					band = EQModel::HIGH;

				if(strcmp(matches[2].str().c_str(), "") == 0){
					//absolute
					if(arg_it == m.ArgumentsEnd())
						throw osc::MissingArgumentException();
					else
						eqModel->set(band, floatFromOscNumber(*arg_it));
				} else if(strcmp(matches[2].str().c_str(), "/cut") == 0){
					//cut
					if(arg_it == m.ArgumentsEnd())
						throw osc::MissingArgumentException();
					else
						eqModel->cut(band, boolFromBoolOrInt(*arg_it));
				} else if(strcmp(matches[2].str().c_str(), "/cut/toggle") == 0){
					//toggle cut
					eqModel->toggleCut(band);
				} else {
					//otherwise it is relative
					if(arg_it == m.ArgumentsEnd())
						throw osc::MissingArgumentException();
					else {
						eqModel->set(band, 
								eqModel->value(band) + floatFromOscNumber(*arg_it));
					}
				}
			}
		}
	}
}

void OscReceiver::processXFadeMessage(const std::string addr, const osc::ReceivedMessage& m){
}

void OscReceiver::processMasterMessage(const std::string addr, const osc::ReceivedMessage& m){
}

