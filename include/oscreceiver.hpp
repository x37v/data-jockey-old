#ifndef DATAJOCKEY_OSC_RECEIVER_HPP
#define DATAJOCKEY_OSC_RECEIVER_HPP

#include "osc/OscPacketListener.h"
#include <string>
class MixerPanelModel;
class DJMixerControlModel;

class OscReceiver : public osc::OscPacketListener {
	public:
		OscReceiver(MixerPanelModel * model);
	protected:
		virtual void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint );
		void processMixerMessage(const std::string addr, const osc::ReceivedMessage& m);
		void processDJControlMessage(const std::string addr, DJMixerControlModel * control, const osc::ReceivedMessage& m);
		void processXFadeMessage(const std::string addr, const osc::ReceivedMessage& m);
		void processMasterMessage(const std::string addr, const osc::ReceivedMessage& m);
	private:
		MixerPanelModel * mModel;
};

#endif
