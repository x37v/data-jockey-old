/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

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
